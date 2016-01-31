#include <iostream>
#include <unordered_set>

int main() {
  std::unordered_set<std::string> us;
  std::string s;
  char cmd;

  while (std::cin >> cmd) {
    std::cin >> s;

    switch (cmd) {
      case '+':
        if (us.find(s) == us.cend()) {
          us.emplace(s);
          std::cout << "OK" << std::endl;
        }
        else {
          std::cout << "FAIL" << std::endl;
        }
        break;
      case '-': {
        auto it = us.find(s);
        if (it != us.cend()) {
          us.erase(it);
          std::cout << "OK" << std::endl;
        }
        else {
          std::cout << "FAIL" << std::endl;
        }
        break;
      }
      case '?':
        if (us.find(s) != us.cend()) {
          std::cout << "OK" << std::endl;
        }
        else {
          std::cout << "FAIL" << std::endl;
        }
        break;
    }
  }

  return 0;
}
