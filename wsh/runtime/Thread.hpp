// wsh/runtime/Thread.hpp
//   Written by mkwcat
//
// Copyright (c) 2025 mkwcat
// SPDX-License-Identifier: MIT

#pragma once

#include "../ppc/Context.hpp"
#include "../util/List.hpp"
#include <cstdlib>
#include <sys/reent.h>

namespace wsh::runtime {

class ThreadQueue;

class Thread {
  friend class ThreadQueue;

public:
  enum class State : u8 {
    Disabled,
    Running,
    Suspended,
    Waiting,
    Exited,
  };

  using ThreadFunc = void *(*)(void *);
  using Priority = u8;
  using ThreadId = int;

  using Link = util::Link<Thread>;
  using List = util::List<Thread>;

public:
  constexpr Thread() noexcept
      : m_context({}), m_link({nullptr, nullptr}), m_state(State::Disabled),
        m_stack_top(nullptr), m_stack_bottom(nullptr), m_stack_owned(false),
        m_unique_id(0), m_libc_reent({}), m_priority(0), m_run_link(),
        m_join_thread(nullptr), m_join_queue({nullptr, nullptr}),
        m_wait_queue(nullptr), m_wait_link({nullptr, nullptr}),
        m_result(nullptr) {}

  Thread(ThreadFunc func, void *arg, void *stack, u32 stackSize,
         Priority priority, bool suspended) noexcept;

  /**
   * Destroys the thread object and frees resources. If this is the current
   * thread, another thread will be dispatched and this function will never
   * return.
   */
  ~Thread() noexcept;

  /**
   * Exit the thread, returning the specified result, and halting execution.
   * Note that this will not destroy the thread object. To exit the thread _and_
   * destroy the thread, use `delete Thread::GetCurrent()`.
   */
  void Exit(void *result = nullptr) noexcept;

  /**
   * Wait for the thread to finish execution, and get the result from the thread
   * function or Exit().
   */
  bool Join(void **result = nullptr) noexcept;

  /**
   * Resume execution of the thread.
   */
  void Resume() noexcept;

  /**
   * Sleep the current thread on a thread queue.
   */
  static void Sleep(ThreadQueue *queue = nullptr) noexcept;

  /**
   * Wake up all threads sleeping on a thread queue.
   */
  static void WakeupAll(ThreadQueue *queue) noexcept;

  /**
   * Wake up the thread from sleeping on a thread queue.
   */
  void Wakeup() noexcept;

  /**
   * Get the queue the thread is enqueued on.
   */
  ThreadQueue *GetWaitQueue() const noexcept { return m_wait_queue; }

  /**
   * Get current state of the thread.
   */
  State GetState() const noexcept { return m_state; }

  /**
   * Yield the current thread, deferring execution to another thread.
   */
  static void Yield() noexcept;

  /**
   * Exit the current thread, returning the specified result. This is equivalent
   * to calling Exit() on the current thread.
   */
  [[noreturn]]
  static void ExitThread(void *result) noexcept {
    GetCurrent()->Exit(result);
    std::abort();
  }

  /**
   * Get the current running thread.
   */
  static Thread *GetCurrent() noexcept { return s_current; }

  static void SystemInit(void *stack, u32 stackSize) noexcept;

private:
  static u64 priorityBit(Priority priority) noexcept {
    return 1ull << 63 >> priority;
  }

  void run() noexcept;

  void enqueueRunTail() noexcept;
  void dequeueRun() noexcept;
  static void dispatchAny() noexcept;
  void dispatch() noexcept;
  static void updateLoMem() noexcept;

private:
  ppc::Context m_context;
  Link m_link;
  State m_state;
  u8 *m_stack_top;
  u8 *m_stack_bottom;
  bool m_stack_owned;
  ThreadId m_unique_id;
  struct _reent m_libc_reent;

  // Thread priority (0-63) and position in the run queue
  Priority m_priority;
  Link m_run_link;

  // The thread that this thread is waiting to join
  Thread *m_join_thread;

  // The queue of threads that are waiting to join this thread
  List m_join_queue;

  // Entry involving this thread waiting for a signal (e.g. joining another
  // thread or locking a mutex)
  ThreadQueue *m_wait_queue;
  Link m_wait_link;

  // The result of the thread function or Exit()
  void *m_result;

private:
  static ThreadId s_next_id;
  static Thread s_main_thread;
  static Thread *s_current;
  static Thread::List s_thread_list;
  static Thread::List s_run_queue[64];
  static u64 s_run_queue_mask;
  static ppc::Context s_none_context;
  static u8 *s_stray_stack;
};

} // namespace wsh::runtime