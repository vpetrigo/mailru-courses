/*
 * Author: vpetrigo
 * Task:
 *   Реализуйте дек с динамическим зацикленным буфером.
 *   Для тестирования дека на вход подаются команды.
 *   В первой строке количество команд. Затем в каждой строке записана одна команда.
 *   Каждая команда задаётся как 2 целых числа: a b.
 *   a = 1 - push front,
 *   a = 2 - pop front,
 *   a = 3 - push back,
 *   a = 4 - pop back.
 *
 *   Если дана команда pop*, то число b - ожидаемое значение. Если команда pop вызвана для пустой структуры данных, то ожидается “-1”.
 *
 *
 *   Требуется напечатать YES, если все ожидаемые значения совпали. Иначе, если хотя бы одно ожидание не оправдалось, то напечатать NO.
 *
 *   Sample Input:
 *   5
 *   1 44
 *   3 50
 *   2 44
 *   2 50
 *   2 -1
 *   Sample Output:
 *   YES
 */

#include <iostream>
#include <algorithm>
#include <deque>
#include <stdexcept>
#include <cstring>

class Deque {
public:
  Deque() : buffer{nullptr}, dsize{0}, dcapacity{0}, head{0}, end{0} {
  }

  ~Deque() {
    if (buffer) {
      delete[] buffer;
    }
  }

  using value_type = int;

  void push_back(int e) {
    if (capacity() == 0) {
      init_buffer();
    }

    if (!empty()) {
      if ((end + 1) % dcapacity == head) {
        // alloc new buffer and copy existing data
        reserve(mem_size * dcapacity);
      }
    }

    buffer[end++] = e;
    end %= dcapacity;
    ++dsize;
  }

  void push_front(int e) {
    if (capacity() == 0) {
      init_buffer();
    }

    if (!empty()) {
      if (head - 1 == end || (head == 0 && dcapacity - 1 == end)) {
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

  int front() const {
    return buffer[head];
  }

  int back() const {
    return end == 0 ? buffer[dcapacity - 1] : buffer[end - 1];
  }

  size_t size() const {
    return dsize;
  }

  size_t capacity() const {
    return dcapacity;
  }

  bool empty() const {
    return (dsize == 0) ? true : false;
  }

  void reserve(const size_t size) {
    if (size > dcapacity) {
      const int new_cap = size;
      int *new_buf = new(std::nothrow) int[new_cap];
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
    buffer = new int[dcapacity];
  }

  void copy_to_new_buffer(int *new_buf, const int buf_length) {
    if (head < end) {
      std::copy(buffer + head, buffer + end, new_buf + head);
    }
    else {
      std::copy(buffer + head, buffer + dcapacity, new_buf + head);
      std::copy(buffer, buffer + end, new_buf + dcapacity);
    }
  }

  int *buffer;
  size_t dsize;
  size_t dcapacity;
  int head;
  int end;
};

int main() {
  Deque d;
  // std::deque<int> d;
  int n;
  int cmd;
  int num;
  bool correct = true;

  std::cin >> n;

  for (size_t i = 0; i < n; ++i) {
    std::cin >> cmd >> num;

    switch (cmd) {
    case 1:
      d.push_front(num);
      break;
    case 2:
      if (d.empty()) {
        if (num != -1) {
          correct = false;
        }
      }
      else {
        int n = d.front();
        d.pop_front();
        if (num != n) {
          correct = false;
        }
      }
      break;
    case 3:
      d.push_back(num);
      break;
    case 4:
      if (d.empty()) {
        if (num != -1) {
          correct = false;
        }
      }
      else {
        int n = d.back();
        d.pop_back();
        if (num != n) {
          correct = false;
        }
      }
    default:
      break;
    }
  }

  if (!correct) {
    std::cout << "NO";
  }
  else {
    std::cout << "YES";
  }
  std::cout << std::endl;

  return 0;
}
