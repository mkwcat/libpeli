// peli/runtime/Thread.hpp
//   Written by mkwcat
//
// Copyright (c) 2025 mkwcat
// SPDX-License-Identifier: MIT

#pragma once

#include "../host/Config.h"
#include "../ppc/Context.hpp"
#include "../util/List.hpp"

#if defined(PELI_NEWLIB)
#include <sys/reent.h>
#endif

namespace peli::runtime {

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
  constexpr Thread() noexcept = default;

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
    __builtin_unreachable();
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
  ppc::Context m_context = {};
  Link m_link = {nullptr, nullptr};
  State m_state = State::Disabled;
  u8 *m_stack_top = nullptr;
  u8 *m_stack_bottom = nullptr;
  bool m_stack_owned = false;
  ThreadId m_unique_id = 0;

#if defined(PELI_NEWLIB)
  struct _reent m_newlib_reent = {};
#endif

  // Thread priority (0-63) and position in the run queue
  Priority m_priority = 0;
  Link m_run_link = {nullptr, nullptr};

  // The thread that this thread is waiting to join
  Thread *m_join_thread = nullptr;

  // The queue of threads that are waiting to join this thread
  List m_join_queue = {nullptr, nullptr};

  // Entry involving this thread waiting for a signal (e.g. joining another
  // thread or locking a mutex)
  ThreadQueue *m_wait_queue = nullptr;
  Link m_wait_link = {nullptr, nullptr};

  // The result of the thread function or Exit()
  void *m_result = nullptr;

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

} // namespace peli::runtime