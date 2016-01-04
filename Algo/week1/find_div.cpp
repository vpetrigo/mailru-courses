#include <iostream>
#include <set>
#include <cmath>

bool is_prime(const int num) {
  for (size_t i = 2; i <= sqrt(num); ++i) {
    if (num % i == 0) {
      return false;
    }
  }

  return true;
}

std::multiset<int> find_div(int number) {
  std::multiset<int> pr_div;

  while (number) {
    if (is_prime(number)) {
      pr_div.insert(number);
      break;
    }

    for (int i = 2; i <= sqrt(number); ++i) {
      if (is_prime(i) && number % i == 0) {
        pr_div.insert(i);
        number /= i;
      }
    }
  }

  return pr_div;
}

int main() {
  int number;

  std::cin >> number;

  const auto&& result = find_div(number);

  for (const auto& elem : result) {
    std::cout << elem << ' ';
  }
  std::cout << std::endl;

  return 0;
}
