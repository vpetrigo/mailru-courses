#include <iostream>
#include <vector>
#include <array>
#include <algorithm>
#include <initializer_list>
#include <iterator>
#include <cstdlib>

class Multiset {
public:
  Multiset() {}

  Multiset(const std::initializer_list<int>& il) : data{il} {
    std::sort(data.begin(), data.end());
  }

  int operator[](int index) const {
    return data[index];
  }

  void print_data(std::ostream& os) const {
    os << "(";
    for (auto cit = data.cbegin(); cit != data.cend(); ++cit) {
      os << *cit;
      if (cit + 1 != data.cend()) {
        os << ",";
      }
    }
    os << ")";
  }

  Multiset diff(const Multiset& m) const {
    Multiset res;

    for (auto cit = data.cbegin(); cit != data.cend(); ++cit) {
      if (std::find(m.data.cbegin(), m.data.cend(), *cit) == m.data.cend()) {
        res.data.push_back(*cit);
      }
    }

    return res;
  }

  friend Multiset operator+(const Multiset& m1, const Multiset& m2);
  friend Multiset operator-(const Multiset& m1, const Multiset& m2);
private:
  std::vector<int> data;
};

Multiset operator+(const Multiset& m1, const Multiset& m2) {
  Multiset res;
  auto cit_m1 = m1.data.cbegin();
  auto cit_m2 = m2.data.cbegin();

  while (cit_m1 != m1.data.cend() && cit_m2 != m2.data.cend()) {
    if (*cit_m1 < *cit_m2) {
      res.data.push_back(*cit_m1++);
    }
    else {
      res.data.push_back(*cit_m2++);
    }
  }

  std::copy(cit_m1, m1.data.cend(), std::back_inserter(res.data));
  std::copy(cit_m2, m2.data.cend(), std::back_inserter(res.data));

  return res;
}

Multiset operator-(const Multiset& m1, const Multiset& m2) {
  Multiset res;

  for (auto cit = m1.data.cbegin(); cit != m1.data.cend(); ++cit)   {
    if (std::find(m2.data.cbegin(), m2.data.cend(), *cit) != m2.data.cend()) {
      res.data.push_back(*cit);
    }
  }

  return res;
}

std::ostream& operator<<(std::ostream& os, const Multiset& m) {
  m.print_data(os);

  return os;
}

int main() {
  char ch;
  std::array<int, 4> a;

  // read a "(X,X,X,X)" sequence
  std::cin >> ch >> a[0] >> ch >> a[1] >> ch >> a[2] >> ch >> a[3] >> ch;
  Multiset m1{a[0], a[1], a[2], a[3]};

  // read a delimeter ','
  std::cin >> ch;
  std::cin >> ch >> a[0] >> ch >> a[1] >> ch >> a[2] >> ch >> a[3] >> ch;

  Multiset m2{a[0], a[1], a[2], a[3]};
  Multiset un = m1 + m2;
  Multiset cr = m1 - m2;

  std::cout << un.diff(cr) << std::endl;

  return 0;
}
