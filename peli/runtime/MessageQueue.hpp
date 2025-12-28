// peli/runtime/MessageQueue.hpp
//   Written by mkwcat
//
// Copyright (c) 2025 mkwcat
// SPDX-License-Identifier: MIT

#pragma once

#include "../cmn/Types.hpp"
#include "../ppc/Msr.hpp"
#include "../util/NoConstruct.hpp"
#include "ThreadQueue.hpp"

namespace peli::runtime {

template <class MessageType, u32 Count = 0> class MessageQueue;

template <class MessageType> class MessageQueue<MessageType, 0> {
public:
  constexpr MessageQueue(util::NoConstruct)
      : m_max_count(0), m_messages(nullptr) {}

  MessageQueue(MessageType *messages, u32 count)
      : m_max_count(count), m_messages(messages) {}

  void Send(const MessageType &value) {
    ppc::Msr::NoInterruptsScope guard;

    while (IsFull()) {
      m_wait_queue.Sleep();
    }

    m_messages[(m_first + m_count++) % m_max_count] = value;

    if (m_count == 1) {
      m_wait_queue.WakeupOne();
    }
  }

  bool TrySend(const MessageType &value) {
    if (IsFull()) {
      return false;
    }

    ppc::Msr::NoInterruptsScope guard;

    // Check again for atomicity
    if (IsFull()) {
      return false;
    }

    m_messages[(m_first + m_count++) % m_max_count] = value;

    if (m_count == 1) {
      m_wait_queue.WakeupOne();
    }
    return true;
  }

  void Jam(const MessageType &value) {
    ppc::Msr::NoInterruptsScope guard;

    while (IsFull()) {
      m_wait_queue.Sleep();
    }

    m_first = (m_first - 1) % m_max_count;
    m_messages[m_first] = value;

    if (m_count == 1) {
      m_wait_queue.WakeupOne();
    }
  }

  bool TryJam(const MessageType &value) {
    if (IsFull()) {
      return false;
    }

    ppc::Msr::NoInterruptsScope guard;

    // Check again for atomicity
    if (IsFull()) {
      return false;
    }

    m_first = (m_first - 1) % m_max_count;
    m_messages[m_first] = value;
    m_count++;

    if (m_count == 1) {
      m_wait_queue.WakeupOne();
    }
    return true;
  }

  MessageType Receive() {
    ppc::Msr::NoInterruptsScope guard;

    while (IsEmpty()) {
      m_wait_queue.Sleep();
    }

    MessageType value = m_messages[m_first];
    m_first = (m_first + 1) % m_max_count;
    m_count--;

    if (m_count == m_max_count - 1) {
      m_wait_queue.WakeupOne();
    }
    return value;
  }

  bool TryReceive(MessageType &value) {
    if (IsEmpty()) {
      return false;
    }

    ppc::Msr::NoInterruptsScope guard;

    // Check again for atomicity
    if (IsEmpty()) {
      return false;
    }

    value = m_messages[m_first];
    m_first = (m_first + 1) % m_max_count;
    m_count--;

    if (m_count == m_max_count - 1) {
      m_wait_queue.WakeupOne();
    }
    return true;
  }

  MessageType Peek() const {
    ppc::Msr::NoInterruptsScope guard;

    while (IsEmpty()) {
      m_wait_queue.Sleep();
      m_wait_queue.WakeupOne(); // Prioritize receive over peek
    }

    return m_messages[m_first];
  }

  bool TryPeek(MessageType &value) const {
    if (IsEmpty()) {
      return false;
    }

    ppc::Msr::NoInterruptsScope guard;

    if (IsEmpty()) {
      return false;
    }

    value = m_messages[m_first];
    return true;
  }

  bool IsEmpty() const { return m_count == 0; }

  bool IsFull() const { return m_count >= m_max_count; }

private:
  mutable ThreadQueue m_wait_queue;
  const u32 m_max_count;
  u32 m_first = 0;
  u32 m_count = 0;
  MessageType *const m_messages = nullptr;
};

template <class MessageType, u32 Count>
class MessageQueue : public MessageQueue<MessageType, 0> {
public:
  MessageQueue() : MessageQueue<MessageType, 0>(m_messages_data, Count) {}
  constexpr MessageQueue(util::NoConstruct)
      : MessageQueue<MessageType, 0>(util::NoConstruct{}) {}

private:
  MessageType m_messages_data[Count] = {};
};

} // namespace peli::runtime