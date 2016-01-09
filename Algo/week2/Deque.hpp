#ifndef DEQUE_HPP
#define DEQUE_HPP

#include <iostream>
#include <algorithm>
#include <stdexcept>

template <typename T>
class Deque {
public:
  using value_type = T;
  using size_type = size_t;
  using reference = T&;
  using const_reference = const T&;

  Deque() : buffer{nullptr}, dsize{0}, dcapacity{0}, head{0}, end{0} {}

  ~Deque() {
    if (buffer) {
      delete[] buffer;
    }
  }

  void push_back(T e) {
    if (capacity() == 0) {
      init_buffer();
    }

    if (!empty()) {
      if (size() + 1 == capacity()) {
        // alloc new buffer and copy existing data
        reserve(mem_size * dcapacity);
      }
    }

    buffer[end++] = e;
    end %= dcapacity;
    ++dsize;
  }

  void push_front(T e) {
    if (capacity() == 0) {
      init_buffer();
    }

    if (!empty()) {
      if (size() + 1 == capacity()) {
        // alloc new buffer and copy existing data
        reserve(mem_size * dcapacity);
      }

      if (head == 0) {
        head = dcapacity;
      }
      buffer[--head] = e;
    }
    else {
      buffer[head] = e;
      ++end;
    }

    ++dsize;
  }

  void pop_back() {
    if (!empty()) {
      if (end == 0) {
        end = dcapacity;
      }
      --end;
      --dsize;
    }
  }
  void pop_front() {
    if (!empty()) {
      head = (head + 1) % dcapacity;
      --dsize;
    }
  }

  reference front() const {
    return buffer[head];
  }

  reference back() const {
    return end == 0 ? buffer[dcapacity - 1] : buffer[end - 1];
  }

  size_type size() const {
    return dsize;
  }

  size_type capacity() const {
    return dcapacity;
  }

  bool empty() const {
    return (dsize == 0) ? true : false;
  }

  void reserve(const size_t size) {
    if (size > dcapacity) {
      const int new_cap = size;
      T *new_buf = new(std::nothrow) T[new_cap];
      if (!new_buf) {
        throw std::bad_alloc {};
      }

      copy_to_new_buffer(new_buf, new_cap);
      delete[] buffer;

      if (head > end) {
        end = head + (dcapacity - head) + end;
      }

      buffer = new_buf;
      dcapacity = new_cap;
    }
  }

  void print_deque() const {
    std::cout << "head: " << head << "; end: " << end << std::endl;
    for (size_t i = 0; i < dcapacity; ++i) {
      if (i >= end || i < head) {
        std::cout << "|x";
      }
      else {
        std::cout << "|" << buffer[i];
      }

      if (i == head) {
        std::cout << "|>";
      }
      else if (i == end) {
        std::cout << "<|";
      }
      else {
        std::cout << "|";
      }
    }
    std::cout << std::endl;
  }

private:
  static constexpr size_t mem_size = 2;

  void init_buffer() {
    dcapacity = mem_size;
    buffer = new T[dcapacity];
  }

  void copy_to_new_buffer(T *new_buf, const size_t buf_length) {
    if (head < end) {
      std::copy(buffer + head, buffer + end, new_buf + head);
    }
    else {
      std::copy(buffer + head, buffer + dcapacity, new_buf + head);
      std::copy(buffer, buffer + end, new_buf + dcapacity);
    }
  }

  T *buffer;
  size_type dsize;
  size_type dcapacity;
  int head;
  int end;
};

#endif // DEQUE_HPP
