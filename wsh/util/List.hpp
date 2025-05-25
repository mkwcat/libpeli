// wsh/util/List.hpp
//   Written by mkwcat
//
// Copyright (c) 2025 mkwcat
// SPDX-License-Identifier: MIT

#pragma once

namespace wsh::util {

template <class T> struct Link {
  T *next;
  T *prev;
};

template <class T, Link<T> T::*LinkMember = nullptr> class List {
public:
  T *head;
  T *tail;

  template <Link<T> T::*Member = LinkMember>
  constexpr void EnqueueHead(T *thread) noexcept {
    if (head == nullptr) {
      head = thread;
      tail = thread;
      thread->*Member = {nullptr, nullptr};
    } else {
      (head->*Member).prev = thread;
      (thread->*Member).next = head;
      head = thread;
      (head->*Member).prev = nullptr;
    }
  }

  template <Link<T> T::*Member = LinkMember>
  constexpr void EnqueueTail(T *thread) noexcept {
    if (head == nullptr) {
      head = thread;
      tail = thread;
      thread->*Member = {nullptr, nullptr};
    } else {
      (tail->*Member).next = thread;
      (thread->*Member).prev = tail;
      tail = thread;
      (tail->*Member).next = nullptr;
    }
  }

  template <Link<T> T::*Member = LinkMember>
  constexpr void Dequeue(T *thread) noexcept {
    if (head == thread) {
      head = (thread->*Member).next;
      if (head == nullptr) {
        tail = nullptr;
      } else {
        (head->*Member).prev = nullptr;
      }
    } else {
      if ((thread->*Member).prev) {
        ((thread->*Member).prev->*Member).next = (thread->*Member).next;
      }
      if ((thread->*Member).next) {
        ((thread->*Member).prev->*Member).prev = (thread->*Member).prev;
      }
    }
  }

  template <Link<T> T::*Member = LinkMember>
  constexpr T *DequeueHead() noexcept {
    if (head == nullptr) {
      return nullptr;
    }

    T *thread = head;
    head = (head->*Member).next;

    if (head == nullptr) {
      tail = nullptr;
    } else {
      (head->*Member).prev = nullptr;
    }

    return thread;
  }

  template <Link<T> T::*Member = LinkMember>
  constexpr T *DequeueTail() noexcept {
    if (tail == nullptr) {
      return nullptr;
    }

    T *thread = tail;
    tail = (tail->*Member).prev;

    if (tail == nullptr) {
      head = nullptr;
    } else {
      (tail->*Member).next = nullptr;
    }

    return thread;
  }

  constexpr bool IsEmpty() const noexcept { return head == nullptr; }
};

} // namespace wsh::util