#include <iostream>
#include <deque>

// class Deque {
//   Deque() : buffer{nullptr}, length{0}, head{nullptr}, end{nullptr} {
//   }
//
//   void push_back(int e) {
//     if (!buffer) {
//       length = mem_size;
//       buffer = new int[length];
//       head = buffer;
//       end = head;
//       *end = e;
//       ++end;
//     }
//     else if ((end + 1) % length == head) {
//       int *new_buf = new int[mem_size * length];
//
//     }
//   }
//
//   void push_front(int e);
//   int pop_back();
//   int pop_front();
// private:
//   static constexpr size_t mem_size = 2;
//
//   void copy_to_new_buffer(int *new_buf);
//
//   int *buffer;
//   size_t length;
//   int *head;
//   int *end;
// };

int main() {
  std::deque<int> d;
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
