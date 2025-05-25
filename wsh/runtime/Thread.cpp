// wsh/runtime/Thread.cpp
//   Written by mkwcat
//
// Copyright (c) 2025 mkwcat
// SPDX-License-Identifier: MIT

#include "Thread.hpp"
#include "../ios/LoMem.hpp"
#include "../ppc/Gpr.hpp"
#include "../ppc/Msr.hpp"
#include "../ppc/Sync.hpp"
#include "../util/Address.hpp"
#include "../util/Debug.hpp"
#include "ThreadQueue.hpp"
#include <bit>
#include <cstdio>
#include <cstdlib>

namespace wsh::runtime {

#ifndef WSH_THREAD_MIN_STACK_SIZE
#define WSH_THREAD_MIN_STACK_SIZE 0x1000
#endif

constinit Thread::ThreadId Thread::s_next_id = 1;
constinit Thread Thread::s_main_thread;
constinit Thread *Thread::s_current = nullptr;
constinit Thread::List Thread::s_thread_list = {nullptr, nullptr};
constinit Thread::List Thread::s_run_queue[64] = {};
constinit u64 Thread::s_run_queue_mask = 0;
constinit ppc::Context Thread::s_none_context = {};
constinit u8 *Thread::s_stray_stack = nullptr;

void Thread::SystemInit(void *stack, u32 stackSize) noexcept {
  constinit static bool s_is_init = false;

  if (s_is_init) {
    return;
  }

  s_next_id = 1;

  // Initialize the main thread
  s_main_thread.m_state = State::Running;
  s_main_thread.m_priority = 16;

  s_main_thread.m_libc_reent = _REENT_INIT(s_main_thread.m_libc_reent);
  _impure_ptr = &s_main_thread.m_libc_reent;

  s_main_thread.m_stack_bottom = reinterpret_cast<u8 *>(stack);
  s_main_thread.m_stack_top = s_main_thread.m_stack_bottom + stackSize;
  s_main_thread.m_stack_owned = false;

  s_thread_list = {nullptr, nullptr};
  s_current = &s_main_thread;

  s_main_thread.m_link = {nullptr, nullptr};
  s_main_thread.m_unique_id = 0;

  updateLoMem();
  ios::g_lo_mem.thread_info.em_current_context = &s_main_thread.m_context;
  ios::g_lo_mem.thread_info.rm_current_context =
      util::Physical(&s_main_thread.m_context);

  s_is_init = true;
}

Thread::Thread(ThreadFunc func, void *arg, void *stack, u32 stackSize,
               Priority priority, bool suspended) noexcept
    : m_context({}) {
  if (priority > 63) {
    priority = 63;
  }
  m_state = State::Suspended;
  m_priority = priority;

  m_unique_id = s_next_id++;

  // Initialize the thread context
  m_context.gqrs[0] = ppc::GetSpr<ppc::Spr::GQR0>();
  m_context.gqrs[1] = ppc::GetSpr<ppc::Spr::GQR1>();
  m_context.gqrs[2] = ppc::GetSpr<ppc::Spr::GQR2>();
  m_context.gqrs[3] = ppc::GetSpr<ppc::Spr::GQR3>();
  m_context.gqrs[4] = ppc::GetSpr<ppc::Spr::GQR4>();
  m_context.gqrs[5] = ppc::GetSpr<ppc::Spr::GQR5>();
  m_context.gqrs[6] = ppc::GetSpr<ppc::Spr::GQR6>();
  m_context.gqrs[7] = ppc::GetSpr<ppc::Spr::GQR7>();

  m_context.srr0 = reinterpret_cast<u32>(func);
  m_context.srr1 = ppc::GetSpr<ppc::Spr::MSR>(); // TODO
  m_context.gprs[2] = ppc::GetGpr<2>();
  m_context.gprs[13] = ppc::GetGpr<13>();
  m_context.gprs[3] = reinterpret_cast<u32>(arg);
  m_context.lr = reinterpret_cast<u32>(&Thread::ExitThread);

  if (stack && stackSize != 0) {
    m_stack_bottom = reinterpret_cast<u8 *>(stack);
    m_stack_owned = false;
  } else {
    if (stackSize == 0) {
      stackSize = WSH_THREAD_MIN_STACK_SIZE;
    }
    m_stack_bottom = static_cast<u8 *>(std::aligned_alloc(32, stackSize));
    m_stack_owned = true;
  }
  m_stack_top = m_stack_bottom + stackSize;

  m_context.gprs[1] = reinterpret_cast<u32>(m_stack_top - 0x8);
  *reinterpret_cast<u32 *>(m_stack_top - 0x4) = 0xFFFFFFFF;

  m_libc_reent = _REENT_INIT(m_libcReent);

  m_link = {nullptr, nullptr};

  // Disable interrupts for synchronization
  ppc::Msr::NoInterruptsScope guard;

  // Append to the thread list
  s_thread_list.EnqueueTail<&Thread::m_link>(this);

  if (!suspended) {
    // Enqueue to the run queue
    enqueueRunTail();
    m_state = State::Running;
  }
}

Thread::~Thread() noexcept {
  if (m_state == State::Disabled) {
    return;
  }

  ppc::Msr::NoInterruptsScope guard;

  if (s_stray_stack) {
    // Free the stray stack if there is one leftover
    std::free(s_stray_stack);
    s_stray_stack = nullptr;
  }

  m_state = State::Disabled;

  // Remove from the thread list
  s_thread_list.Dequeue<&Thread::m_link>(this);

  m_link = {nullptr, nullptr};

  if (s_current == this) {
    // Clear the exception context
    s_current = nullptr;
    ios::g_lo_mem.thread_info.em_current_context = &s_none_context;
    ios::g_lo_mem.thread_info.rm_current_context =
        util::Physical(&s_none_context);
    updateLoMem();

    // Can't delete the stack while it's in use, so pass it along to be freed by
    // another thread
    m_stack_owned = false;
    s_stray_stack = m_stack_bottom;

    // Exit the current thread, shouldn't return
    dispatchAny();
    util::Assert(false, "Dispatched thread is deleted");
  }

  if (m_stack_owned) {
    std::free(m_stack_bottom);
  }

  // Remove from the run queue
  if (m_state == State::Running) {
    dequeueRun();
  }
}

void Thread::Exit(void *result) noexcept {
  ppc::Msr::NoInterruptsScope guard;

  m_result = result;
  m_state = State::Exited;

  // Notify any threads waiting to join this thread
  WakeupAll(static_cast<ThreadQueue *>(&m_join_queue));

  if (s_current == this) {
    dispatchAny();
  }
}

bool Thread::Join(void **result) noexcept {
  Thread *current = s_current;
  if (!current || current == this) {
    return false;
  }

  ppc::Msr::NoInterruptsScope guard;

  if (m_state == State::Disabled) {
    return false;
  }

  if (m_state != State::Exited) {
    current->m_join_thread = this;
    current->Sleep(static_cast<ThreadQueue *>(&m_join_queue));
  }

  if (result) {
    *result = m_result;
  }
  return true;
}

void Thread::Sleep(ThreadQueue *queue) noexcept {
  Thread *current = s_current;
  if (!current) {
    return;
  }

  ppc::Msr::NoInterruptsScope guard;

  current->m_state = State::Waiting;

  if (queue) {
    queue->EnqueueTail<&Thread::m_wait_link>(current);
    current->m_wait_queue = queue;
  }
  dispatchAny();
}

void Thread::run() noexcept {
  m_state = State::Running;

  // Enqueue to the run queue
  enqueueRunTail();
}

void Thread::Wakeup() noexcept {
  ppc::Msr::NoInterruptsScope guard;

  if (m_state != State::Waiting) {
    return;
  }

  // Remove from the wait queue
  m_wait_queue->Dequeue<&Thread::m_wait_link>(this);
  m_wait_queue = nullptr;

  run();
}

void Thread::WakeupAll(ThreadQueue *queue) noexcept {
  ppc::Msr::NoInterruptsScope guard;

  for (Thread *thread = queue->head, *next = nullptr; thread != nullptr;
       thread = next) {
    next = thread->m_wait_link.next;
    thread->m_wait_link = {nullptr, nullptr};
    thread->m_wait_queue = nullptr;
    thread->run();
  }

  *queue = ThreadQueue();
}

void Thread::Yield() noexcept {
  Thread *current = s_current;
  if (!current) {
    return;
  }

  ppc::Msr::NoInterruptsScope guard;

  dispatchAny();
}

void Thread::enqueueRunTail() noexcept {
  s_run_queue[m_priority].EnqueueTail<&Thread::m_run_link>(this);
  s_run_queue_mask |= priorityBit(m_priority);
}

void Thread::dequeueRun() noexcept {
  s_run_queue[m_priority].Dequeue<&Thread::m_run_link>(this);
  if (s_run_queue[m_priority].IsEmpty()) {
    s_run_queue_mask &= ~priorityBit(m_priority);
  }
}

// Expects interrupts to be disabled
void Thread::dispatchAny() noexcept {
  Thread *next = nullptr;
  Thread *current = s_current;

  bool yield = current && current->m_state == State::Running;

  while (!s_run_queue_mask) {
    if (yield && current) {
      return;
    }

    // No threads to run, enable interrupts and idle
    {
      ppc::Msr::EnableInterruptsScope guard;

      while (!s_run_queue_mask &&
             (!current || current->m_state != State::Running)) {
        ppc::Sync();
      }
    }

    yield = current && current->m_state == State::Running;
  }

  // Find the next thread to run
  int priority = std::countl_zero(s_run_queue_mask);

  if (current && current->m_state == State::Running) {
    if (!yield && current->m_priority < priority) {
      // The current thread is still the highest priority
      return;
    }

    // Enqueue the current thread to the tail of the run queue
    current->enqueueRunTail();
  }

  // Dequeue and switch to the next thread
  next = s_run_queue[priority].DequeueHead<&Thread::m_run_link>();
  util::Assert(next != nullptr, "Dispatched thread is null");
  if (s_run_queue[priority].IsEmpty()) {
    s_run_queue_mask &= ~priorityBit(priority);
  }

  next->dispatch();
}

// Assumes interrupts are disabled
void Thread::dispatch() noexcept {
  _impure_ptr = &m_libc_reent;

  // Set the current thread
  s_current = this;
  updateLoMem();

  m_context.FastSwitch();

  // Hook here to free an old stack, which is kinda weird. If this doesn't get
  // called, worst case scenario it lives until another thread gets destroyed
  if (s_stray_stack) {
    std::free(s_stray_stack);
    s_stray_stack = nullptr;
  }
}

void Thread::updateLoMem() noexcept {
  ios::g_lo_mem.thread_info.current_thread =
      reinterpret_cast<ios::OSThread *>(s_current);
  ios::g_lo_mem.thread_info.thread_list.head =
      reinterpret_cast<ios::OSThread *>(s_thread_list.head);
  ios::g_lo_mem.thread_info.thread_list.tail =
      reinterpret_cast<ios::OSThread *>(s_thread_list.tail);
}

} // namespace wsh::runtime