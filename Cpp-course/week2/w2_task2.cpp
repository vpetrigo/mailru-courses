/*
  Author: vpetrigo
  Task: Implement Vector class that provides functionality for checking input
  3D-vectors for collinearity
 */
#include <iostream>
#include <vector>
#include <initializer_list>
#include <cmath>
#include <stdexcept>
#include <boost/type_index.hpp>

class Vector {
public:
  Vector() {}
  Vector(const std::initializer_list<int>& il) : vect{il} {}

  void init(const std::initializer_list<int>& il) {
    vect.resize(il.size());
    std::move_backward(il.begin(), il.end(), vect.end());
  }

  bool check_collinearity(const Vector& v) const {
    const auto v1_len = vect.size();
    const auto v2_len = v.vect.size();

    if (v1_len != v2_len) {
      return false;
    }

    const double len_prod = vector_length() * v.vector_length();
    const double sc_prod = scalar_product(v);

    if (sc_prod == len_prod || sc_prod == -len_prod) {
      return true;
    }

    return false;
  }

  double vector_length() const noexcept {
    int sq_sum = 0;

    for (const auto& num : vect) {
      sq_sum += num * num;
    }

    return std::sqrt(sq_sum);
  }

  double scalar_product(const Vector& v) const {
    const auto v1_len = vect.size();
    const auto v2_len = v.vect.size();

    if (v1_len != v2_len) {
      throw std::runtime_error{"Vectors is the different lenghts"};
    }

    int sc_prod = 0;
    for (auto i = 0; i < v1_len; ++i) {
      sc_prod += vect[i] * v.vect[i];
    }

    return sc_prod;
  }
private:
  std::vector<int> vect;
};

std::istream& operator>>(std::istream& is, Vector& v) {
  char ch1, ch2, ch3, ch4;
  int x, y, z;

  while (is.get() != '(') {
    // pass all input char untill we meet an opening brace (x,y,z)
  }

  is.unget();
  is >> ch1 >> x >> ch2 >> y >> ch3 >> z >> ch4;
  v.init({x, y, z});

  return is;
}

int main() {
  using boost::typeindex::type_id_with_cvr;
  Vector v1, v2;

  std::cin >> v1 >> v2;

  if (v1.check_collinearity(v2)) {
    std::cout << "yes" << std::endl;
  }
  else {
    std::cout << "no" << std::endl;
  }

  return 0;
}
