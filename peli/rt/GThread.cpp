// peli/rt/GThread.cpp - GNU syscalls for libstdc++
//   Written by mkwcat
//
// Copyright (c) 2025 mkwcat
// SPDX-License-Identifier: MIT

#include "Cond.hpp"
#include "Mutex.hpp"
#include "Once.hpp"
#include "Thread.hpp"
#include <bits/gthr-default.h>
#include <new>

namespace peli::rt {

extern "C" {

// Thread functions

int __GTHR_IMPL(active)() { return 1; }

struct GThread : Thread {
private:
  GThread(void *(*func)(void *), void *arg)
      : Thread(entry, new Args(this, func, arg), nullptr, 0, 16, false) {}

  struct Args {
    GThread *thread;
    void *(*func)(void *);
    void *arg;
  };

  static void *entry(void *arg) {
    Args *args_ptr = static_cast<Args *>(arg), args = *args_ptr;
    delete args_ptr;

    void *value = args.func(args.arg);
    if (args.thread->m_detached) {
      delete args.thread;
    } else {
      args.thread->Exit(value);
    }

    __builtin_unreachable();
  }

public:
  static __gthread_t *Create(void *(*func)(void *), void *arg) {
    GThread *thread = new GThread(func, arg);
    if (thread == nullptr) {
      return nullptr;
    }
    return reinterpret_cast<__gthread_t *>(thread);
  }

  static GThread *FromId(__gthread_t threadid) {
    if (threadid == nullptr) {
      return nullptr;
    }
    return reinterpret_cast<GThread *>(threadid);
  }

  static Thread *BaseFromId(__gthread_t threadid) {
    GThread *gthread = FromId(threadid);
    if (gthread == nullptr) {
      return Thread::GetCurrent();
    }
    return gthread;
  }

  bool m_detached = false;
};

int __GTHR_IMPL(create)(__gthread_t *__threadid, void *(*__func)(void *),
                        void *__args) {
  return (*__threadid = GThread::Create(__func, __args)) ? 0 : -1;
}

int __GTHR_IMPL(join)(__gthread_t __threadid, void **__value_ptr) {
  Thread *thread = GThread::BaseFromId(__threadid);
  if (thread == nullptr) {
    return -1;
  }

  if (thread->Join(__value_ptr)) {
    delete thread;
    return 0;
  } else {
    return -1;
  }
}

int __GTHR_IMPL(detach)(__gthread_t __threadid) {
  GThread *thread = GThread::FromId(__threadid);
  if (thread == nullptr) {
    return -1;
  }

  thread->m_detached = true;
  return 0;
}

int __GTHR_IMPL(equal)(__gthread_t __threadid1, __gthread_t __threadid2) {
  return GThread::FromId(__threadid1) == GThread::FromId(__threadid2);
}

int __GTHR_IMPL(yield)() {
  Thread::Yield();
  return 0;
}

__gthread_t __GTHR_IMPL(self)() {
  // nullptr is the current thread
  return nullptr;
}

int __GTHR_IMPL(once)(__gthread_once_t *__once, void (*__func)(void)) {
  static_assert(sizeof(__gthread_once_t) >= sizeof(OnceControl));
  OnceControl *once = reinterpret_cast<OnceControl *>(__once);

  if (auto guard = OnceControl::Guard(*once)) {
    __func();
  }
  return 0;
}

// Mutex functions

static_assert(sizeof(__gthread_mutex_t) >= sizeof(Mutex));

void __GTHR_IMPL(mutex_init_function)(__gthread_mutex_t *__mutex) {
  Mutex *mutex = reinterpret_cast<Mutex *>(__mutex);
  new (mutex) Mutex();
}

int __GTHR_IMPL(mutex_lock)(__gthread_mutex_t *__mutex) {
  Mutex *mutex = reinterpret_cast<Mutex *>(__mutex);
  mutex->Lock();
  return 0;
}

int __GTHR_IMPL(mutex_unlock)(__gthread_mutex_t *__mutex) {
  Mutex *mutex = reinterpret_cast<Mutex *>(__mutex);
  mutex->Unlock();
  return 0;
}

int __GTHR_IMPL(mutex_destroy)(__gthread_mutex_t *__mutex) {
  Mutex *mutex = reinterpret_cast<Mutex *>(__mutex);
  mutex->~Mutex();
  return 0;
}

int __GTHR_IMPL(mutex_trylock)(__gthread_mutex_t *__mutex) {
  Mutex *mutex = reinterpret_cast<Mutex *>(__mutex);
  if (mutex->TryLock()) {
    return 0;
  } else {
    return -1;
  }
}

// Cond functions

static_assert(sizeof(__gthread_cond_t) >= sizeof(Cond));

void __GTHR_IMPL(cond_init_function)(__gthread_cond_t *__cond) {
  Cond *cond = reinterpret_cast<Cond *>(__cond);
  new (cond) Cond();
}

int __GTHR_IMPL(cond_wait)(__gthread_cond_t *__cond,
                           __gthread_mutex_t *__mutex) {
  Cond *cond = reinterpret_cast<Cond *>(__cond);
  Mutex *mutex = reinterpret_cast<Mutex *>(__mutex);
  cond->Wait(mutex);
  return 0;
}

int __GTHR_IMPL(cond_signal)(__gthread_cond_t *__cond) {
  Cond *cond = reinterpret_cast<Cond *>(__cond);
  cond->Signal();
  return 0;
}

int __GTHR_IMPL(cond_broadcast)(__gthread_cond_t *__cond) {
  Cond *cond = reinterpret_cast<Cond *>(__cond);
  cond->Broadcast();
  return 0;
}

// Recursive mutex functions

static_assert(sizeof(__gthread_recursive_mutex_t) >= sizeof(RecursiveMutex));

int __GTHR_IMPL(recursive_mutex_init_function)(
    __gthread_recursive_mutex_t *__mutex) {
  RecursiveMutex *mutex = reinterpret_cast<RecursiveMutex *>(__mutex);
  new (mutex) RecursiveMutex();
  return 0;
}

int __GTHR_IMPL(recursive_mutex_lock)(__gthread_recursive_mutex_t *__mutex) {
  RecursiveMutex *mutex = reinterpret_cast<RecursiveMutex *>(__mutex);
  mutex->Lock();
  return 0;
}

int __GTHR_IMPL(recursive_mutex_unlock)(__gthread_recursive_mutex_t *__mutex) {
  RecursiveMutex *mutex = reinterpret_cast<RecursiveMutex *>(__mutex);
  mutex->Unlock();
  return 0;
}

int __GTHR_IMPL(recursive_mutex_destroy)(__gthread_recursive_mutex_t *__mutex) {
  RecursiveMutex *mutex = reinterpret_cast<RecursiveMutex *>(__mutex);
  mutex->~RecursiveMutex();
  return 0;
}

int __GTHR_IMPL(recursive_mutex_trylock)(__gthread_recursive_mutex_t *__mutex) {
  RecursiveMutex *mutex = reinterpret_cast<RecursiveMutex *>(__mutex);
  return mutex->TryLock() ? 0 : -1;
}

} // extern "C"

} // namespace peli::rt