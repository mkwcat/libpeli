#pragma once

#include "../common/Types.hpp"
#include "../host/Host.hpp"
#include "../util/Address.hpp"
#include "../util/Concept.hpp"
#include "Request.hpp"
#include "low/Ipc.hpp"

namespace peli::ios {

template <util::EnumType TIoctlCmdType> class Interface {
public:
  using IoctlCmdType = TIoctlCmdType;
  using IosInterfaceType = Interface<TIoctlCmdType>;

  template <class... TArgs> struct In {
    static constexpr size_t Count = sizeof...(TArgs);
  };

  template <class... TArgs> struct Out {
    static constexpr size_t Count = sizeof...(TArgs);
  };

protected:
  template <bool TReversed, size_t TInCount, size_t TOutCount,
            size_t TOutPtrCount, class...>
  struct Vector;

public:
  template <class, class> struct InOutOrdered;

  template <class... TTypes1, class... TTypes2>
  struct InOutOrdered<In<TTypes1...>, Out<TTypes2...>> {
    using InType = In<TTypes1...>;
    using OutType = Out<TTypes2...>;
    using Vector = Interface<TIoctlCmdType>::Vector<false, sizeof...(TTypes1),
                                                    sizeof...(TTypes2), 0,
                                                    TTypes1..., TTypes2...>;
    static constexpr bool Reversed = false;
  };

  template <class... TTypes1, class... TTypes2>
  struct InOutOrdered<Out<TTypes1...>, Out<TTypes2...>> {
    using InType = In<TTypes2...>;
    using OutType = Out<TTypes1...>;
    using Vector = Interface<TIoctlCmdType>::Vector<true, sizeof...(TTypes2),
                                                    sizeof...(TTypes1), 0,
                                                    TTypes2..., TTypes2...>;
    static constexpr bool Reversed = true;
  };

protected:
  template <size_t TVectorCount> struct LowVectors {
    alignas(low::Alignment) low::IOVector m_vectors[TVectorCount];

    constexpr low::IOVector *Vectors() { return m_vectors; }
  };

  template <size_t TVectorCount>
    requires(TVectorCount == 0)
  struct LowVectors<TVectorCount> {
    constexpr low::IOVector *Vectors() { return nullptr; }
  };

  template <bool TReversed, size_t TInCount, size_t TOutCount,
            size_t TOutPtrCount, class...>
  struct Vector {
    static constexpr bool IsPointer = false;
    static constexpr size_t Index = TInCount + TOutCount;
    static constexpr size_t ExpectedParamCount = TInCount + TOutPtrCount;
    static constexpr size_t OutPtrCount = TOutPtrCount;

    u8 *m_v_stack_ptr;
    size_t m_alloc_size;

    template <class... TDefaults>
    constexpr Vector(size_t alloc_size, low::IOVector *,
                     const TDefaults &...) noexcept {
      m_alloc_size = alloc_size;
      m_v_stack_ptr =
          alloc_size > 0
              ? static_cast<u8 *>(host::Alloc(low::Alignment, alloc_size))
              : nullptr;
    }

    constexpr ~Vector() noexcept {
      if (m_v_stack_ptr) {
        host::Free(m_v_stack_ptr, m_alloc_size);
      }
    }

    // Custom placement new here to remove dependency on <new>
    static constexpr void *operator new(size_t, void *ptr) noexcept {
      return ptr;
    }

    template <size_t TGetIndex> constexpr void *Get() noexcept {
      static_assert(TGetIndex && false, "Invalid get index");
      return nullptr;
    }
  };

  template <bool TReversed, size_t TInCount, size_t TOutCount,
            size_t TOutPtrCount>
    requires(TOutPtrCount == 0)
  struct Vector<TReversed, TInCount, TOutCount, TOutPtrCount> {
    static constexpr bool IsPointer = false;
    static constexpr size_t Index = TInCount + TOutCount;
    static constexpr size_t ExpectedParamCount = TInCount + TOutPtrCount;
    static constexpr size_t OutPtrCount = TOutPtrCount;

    template <class... TDefaults>
    constexpr Vector(size_t, low::IOVector *, const TDefaults &...) noexcept {}

    // Custom placement new here to remove dependency on <new>
    static constexpr void *operator new(size_t, void *ptr) noexcept {
      return ptr;
    }

    template <size_t TGetIndex>
    constexpr void *Get(low::IOVector * = nullptr) noexcept {
      static_assert(TGetIndex && false, "Invalid get index");
      return nullptr;
    }
  };

  // Stack tuple for non-stack allocatable types, i.e. pointers to data with
  // variable sizes
  template <bool TReversed, size_t TInCount, size_t TOutCount,
            size_t TOutPtrCount, class TThis, class... TExtra>
  struct Vector<TReversed, TInCount, TOutCount, TOutPtrCount, TThis *,
                TExtra...>
      : public Vector<TReversed, TInCount, TOutCount,
                      TOutPtrCount + (sizeof...(TExtra) < TOutCount),
                      TExtra...> {

    static constexpr bool IsPointer = true;
    static constexpr size_t Index =
        TInCount + TOutCount - sizeof...(TExtra) - 1;
    static constexpr bool IsInput = Index < TInCount;
    static constexpr bool IsOutput = !IsInput;
    static constexpr size_t VecIndex =
        !TReversed ? Index : Index + (IsInput ? TOutCount : -TOutCount);
    using Base =
        Vector<TReversed, TInCount, TOutCount,
               TOutPtrCount + (sizeof...(TExtra) < TOutCount), TExtra...>;

    // Non-const for output data
    template <class... TDefaults>
    constexpr Vector(size_t alloc_size, low::IOVector *vec, TThis *const &value,
                     const TDefaults &...defaults) noexcept
        : Base(alloc_size, vec, defaults...) {
      vec[VecIndex].data = value;
      vec[VecIndex].size = sizeof(TThis);
    }

    // Const for input data
    template <class... TDefaults>
    constexpr Vector(size_t alloc_size, low::IOVector *vec,
                     const TThis *const &value,
                     const TDefaults &...defaults) noexcept
        : Base(alloc_size, vec, defaults...) {
      static_assert(IsInput, "Output pointer cannot be const");
      vec[VecIndex].data = const_cast<TThis *>(value);
      if constexpr (__is_same_as(TThis, const char)) {
        // Treat const char* as a null-terminated string
        for (vec[VecIndex].size = 0;
             value != nullptr && value[vec[VecIndex].size];
             vec[VecIndex].size++) {
        }
      } else {
        vec[VecIndex].size = value != nullptr ? sizeof(TThis) : 0;
      }
    }

    // Null pointer
    template <class... TDefaults>
    constexpr Vector(size_t alloc_size, low::IOVector *vec,
                     const decltype(nullptr) &,
                     const TDefaults &...defaults) noexcept
        : Base(alloc_size, vec, defaults...) {
      vec[VecIndex].data = nullptr;
      vec[VecIndex].size = 0;
    }

    // Supplying u32 for allocating data on the fly
    template <class... TDefaults>
    constexpr Vector(size_t alloc_size, low::IOVector *vec,
                     const u32 &alloc_size_in,
                     const TDefaults &...defaults) noexcept
        : Base(alloc_size + util::AlignUp(low::Alignment, alloc_size_in), vec,
               defaults...) {
      static_assert(IsOutput,
                    "Input pointer cannot be allocated by passing size");
      if (alloc_size_in > 0) {
        vec[VecIndex].data = Base::m_v_stack_ptr + alloc_size;
        vec[VecIndex].size = alloc_size_in;
      } else {
        vec[VecIndex].data = nullptr;
        vec[VecIndex].size = 0;
      }
    }

    // Supplying low::IOVector
    template <class... TDefaults>
    constexpr Vector(size_t alloc_size, low::IOVector *vec,
                     const low::IOVector &vector,
                     const TDefaults &...defaults) noexcept
        : Base(alloc_size, vec, defaults...) {
      vec[VecIndex].data = vector.data;
      vec[VecIndex].size = vector.size;
    }

    // Get data by index
    template <size_t TGetIndex>
    constexpr const TThis *Get(low::IOVector *vec) noexcept
      requires(TGetIndex == VecIndex)
    {
      return static_cast<const TThis *>(vec[VecIndex].data);
    }

    template <size_t TGetIndex>
    constexpr auto Get(low::IOVector *vec) noexcept {
      return Base::template Get<TGetIndex>(vec);
    }
  };

  template <bool TReversed, size_t TInCount, size_t TOutCount,
            size_t TOutPtrCount, class TThis, class... TExtra>
  struct Vector<TReversed, TInCount, TOutCount, TOutPtrCount, TThis, TExtra...>
      : public Vector<TReversed, TInCount, TOutCount, TOutPtrCount, TExtra...> {
    static constexpr bool IsPointer = false;
    static constexpr size_t Index =
        TInCount + TOutCount - sizeof...(TExtra) - 1;
    static constexpr bool IsInput = Index < TInCount;
    static constexpr bool IsOutput = !IsInput;
    static constexpr size_t VecIndex =
        !TReversed ? Index : Index + (IsInput ? TOutCount : -TOutCount);
    using Base =
        Vector<TReversed, TInCount, TOutCount, TOutPtrCount, TExtra...>;

    union {
      u8 m_stub;
      alignas(low::Alignment) TThis m_this;
    };

    // For input data
    template <class... TDefaults>
    constexpr Vector(size_t alloc_size, low::IOVector *vec, const TThis &value,
                     const TDefaults &...defaults) noexcept
      requires(IsInput)
        : Base(alloc_size, vec, defaults...), m_this(value) {
      vec[VecIndex].data = &m_this;
      vec[VecIndex].size = sizeof(m_this);
    }

    // For output data
    template <class... TDefaults>
    constexpr Vector(size_t alloc_size, low::IOVector *vec,
                     const TDefaults &...defaults) noexcept
      requires(IsOutput)
        : Base(alloc_size, vec, defaults...), m_stub() {
      vec[VecIndex].data = &m_this;
      vec[VecIndex].size = sizeof(m_this);
    }

    // Null pointer
    template <class... TDefaults>
    constexpr Vector(size_t alloc_size, low::IOVector *vec,
                     const decltype(nullptr) &,
                     const TDefaults &...defaults) noexcept
        : Base(alloc_size, vec, defaults...) {
      vec[VecIndex].data = nullptr;
      vec[VecIndex].size = 0;
    }

    // Supplying custom pointer
    template <class... TDefaults>
    constexpr Vector(size_t alloc_size, low::IOVector *vec, TThis *const &value,
                     const TDefaults &...defaults) noexcept
        : Base(alloc_size, vec, defaults...), m_this(*value) {
      vec[VecIndex].data = value;
      vec[VecIndex].size = sizeof(TThis);
    }

    // Const pointer for input
    template <class... TDefaults>
    constexpr Vector(size_t alloc_size, low::IOVector *vec,
                     const TThis *const &value,
                     const TDefaults &...defaults) noexcept
        : Base(alloc_size, vec, defaults...), m_this(*value) {
      static_assert(IsInput, "Output pointer cannot be const");
      vec[VecIndex].data = const_cast<TThis *>(value);
      vec[VecIndex].size = sizeof(TThis);
    }

  private:
    // Can't use builtin directly in function signature
    template <class TType, size_t TSize = sizeof(TThis) / sizeof(TType)>
    consteval bool isArrayOf() const noexcept {
      return __is_same_as(TThis, TType[TSize]);
    }

  public:
    // const char* for string input
    template <class... TDefaults>
    constexpr Vector(size_t alloc_size, low::IOVector *vec,
                     const char *const value, const TDefaults &...defaults)
      requires(IsInput && isArrayOf<char>())
        : Base(alloc_size, vec, defaults...) {
      size_t i;
      for (i = 0; i < sizeof(TThis) - 1 && value[i]; i++) {
        m_this[i] = value[i];
      }
      m_this[i] = '\0';
      vec[VecIndex].data = m_this;
      vec[VecIndex].size = sizeof(TThis);
    }

    // Get data by index
    template <size_t TGetIndex>
    constexpr const TThis &Get(low::IOVector * = nullptr) noexcept
      requires(TGetIndex == VecIndex)
    {
      return m_this;
    }

    template <size_t TGetIndex>
    constexpr auto Get(low::IOVector *vec = nullptr) noexcept {
      return Base::template Get<TGetIndex>(vec);
    }
  };

  template <bool TReversed, size_t TInCount, size_t TOutCount,
            size_t TOutputIndex>
  static constexpr size_t OutputVecIndex =
      TReversed ? TOutputIndex : TOutputIndex + TInCount;

  template <bool TReversed, size_t TInCount, size_t TOutCount,
            size_t TInputIndex>
  static constexpr size_t InputVecIndex =
      TReversed ? TInputIndex + TOutCount : TInputIndex;

public:
  template <TIoctlCmdType TCmd, class TInType = void, class TOutType = void>
  class IoctlDef;

  template <TIoctlCmdType TCmd, class... TInTypes, class... TOutTypes,
            template <class...> class TL1, template <class...> class TL2>
  class IoctlDef<TCmd, TL1<TInTypes...>, TL2<TOutTypes...>> {
    static_assert(sizeof...(TInTypes) <= 1 && sizeof...(TOutTypes) <= 1,
                  "IoctlDef supports only one input and one output type");

    template <class...> struct Contained;

    template <template <class...> class T> struct Contained<T<>> {
      using Type = void;
    };

    template <template <class...> class T, class TType>
    struct Contained<T<TType>> {
      using Type = TType;
    };

    using OrderedTypes = InOutOrdered<TL1<TInTypes...>, TL2<TOutTypes...>>;

  public:
    /**
     * The ioctl command.
     */
    static constexpr TIoctlCmdType Command = TCmd;

    /**
     * The input type.
     */
    using InType = Contained<typename OrderedTypes::InType>::Type;

    /**
     * The output type.
     */
    using OutType = Contained<typename OrderedTypes::OutType>::Type;

    /**
     * Whether or not the input and output types are reversed. This is done by
     * the SSL module.
     */
    static constexpr bool Reversed = OrderedTypes::Reversed;

    class Request : public ios::Request {
      void sendRequest(s32 fd, const low::IOVector *vectors) noexcept {
        static constexpr int swap = (!Reversed && sizeof...(TInTypes) == 0) ||
                                    (Reversed && sizeof...(TOutTypes) == 0);

        low::IOS_IoctlAsync(fd, static_cast<u32>(TCmd), vectors[swap].data,
                            vectors[swap].size, vectors[swap ^ 1].data,
                            vectors[swap ^ 1].size, m_queue, &m_cmd_block);
      }

    private:
      using TempVector = low::IOVector[2];

      Request(TempVector vectors, s32 fd) noexcept
        requires(sizeof...(TInTypes) == 0 &&
                 (sizeof...(TOutTypes) == 0 ||
                  OrderedTypes::Vector::OutPtrCount == 0))
          : m_stack(0, vectors) {
        sendRequest(fd, vectors);
      }

      Request(TempVector vectors, s32 fd, auto in) noexcept
        requires(sizeof...(TInTypes) > 0 &&
                 (sizeof...(TOutTypes) == 0 ||
                  OrderedTypes::Vector::OutPtrCount == 0))
          : m_stack(0, vectors, in) {
        sendRequest(fd, vectors);
      }

      Request(TempVector vectors, s32 fd, auto in, auto out) noexcept
        requires(sizeof...(TInTypes) > 0 && sizeof...(TOutTypes) > 0 &&
                 OrderedTypes::Vector::OutPtrCount == 1)
          : m_stack(0, vectors, in, out) {
        sendRequest(fd, vectors);
      }

      Request(TempVector vectors, s32 fd, auto out) noexcept
        requires(sizeof...(TInTypes) == 0 && sizeof...(TOutTypes) > 0 &&
                 OrderedTypes::Vector::OutPtrCount == 1)
          : m_stack(0, vectors, out) {
        sendRequest(fd, vectors);
      }

    public:
      constexpr Request(s32 fd) : Request(TempVector{}, fd) {}
      constexpr Request(s32 fd, auto in) : Request(TempVector{}, fd, in) {}
      constexpr Request(s32 fd, auto in, auto out)
          : Request(TempVector{}, fd, in, out) {}

      Request &Sync() noexcept {
        return static_cast<Request &>(ios::Request::Sync());
      }

      template <size_t TOutputIndex = 0> constexpr auto GetOutput() {
        if constexpr (OrderedTypes::Vector::OutPtrCount > 0) {
          return *static_cast<const OutType *>(m_stack.m_v_stack_ptr);
        } else {
          return m_stack.template Get<
              OutputVecIndex<Reversed, sizeof...(TInTypes),
                             sizeof...(TOutTypes), TOutputIndex>>();
        }
      }

      template <size_t TOutputIndex = 0>
      constexpr Request &CopyOutput(auto &out) {
        out = GetOutput<TOutputIndex>();
        return *this;
      }

    private:
      typename OrderedTypes::Vector m_stack;
    };
  };

  template <TIoctlCmdType, class TIn, class TOut> class IoctlVecDef;

  template <TIoctlCmdType TCmd, class... TInTypes, class... TOutTypes,
            template <class...> class TL1, template <class...> class TL2>
  class IoctlVecDef<TCmd, TL1<TInTypes...>, TL2<TOutTypes...>> {
    using OrderedTypes = InOutOrdered<TL1<TInTypes...>, TL2<TOutTypes...>>;

  public:
    /**
     * The ioctl command.
     */
    static constexpr TIoctlCmdType Command = TCmd;

    /**
     * The input types, serialized in the In class. May be empty.
     */
    using InType = typename OrderedTypes::InType;

    /**
     * The output types, serialized in the Out class. May be empty.
     */
    using OutType = typename OrderedTypes::OutType;

    /**
     * Total vector count.
     */
    static constexpr size_t Count = InType::Count + OutType::Count;

    /**
     * Whether or not the input and output types are reversed. This is done by
     * the SSL module.
     */
    static constexpr bool Reversed = OrderedTypes::Reversed;

    class Request : public ios::Request {
      template <size_t TDefaultCount>
      static constexpr bool checkExpectedParamCount() {
        if constexpr (TDefaultCount ==
                      OrderedTypes::Vector::ExpectedParamCount) {
          return true;
        } else {
          static_assert(TDefaultCount ==
                            OrderedTypes::Vector::ExpectedParamCount,
                        "invalid number of parameters for the Ioctlv request");
          return false;
        }
      }

    public:
      template <class... TCallArgs>
        requires(checkExpectedParamCount<sizeof...(TCallArgs)>())
      explicit Request(s32 fd, const TCallArgs &...args) noexcept
          : m_stack(0, m_vectors.Vectors(), args...) {
        low::IOS_IoctlvAsync(fd, static_cast<u32>(Command), InType::Count,
                             OutType::Count, m_vectors.Vectors(), m_queue,
                             &m_cmd_block);
      }

      Request(const Request &) = delete;
      Request(Request &&) = delete;

      ~Request() noexcept {}

      Request &Sync() noexcept {
        return static_cast<Request &>(ios::Request::Sync());
      }

      template <size_t TOutputIndex> constexpr auto GetOutput() {
        return m_stack.template Get<OutputVecIndex<
            Reversed, InType::Count, OutType::Count, TOutputIndex>>(
            m_vectors.Vectors());
      }

      template <size_t TOutputIndex> constexpr Request &CopyOutput(auto &out) {
        out = GetOutput<TOutputIndex>();
        return *this;
      }

    private:
      LowVectors<sizeof...(TInTypes) + sizeof...(TOutTypes)> m_vectors;
      typename OrderedTypes::Vector m_stack;
    };
  };
};

} // namespace peli::ios