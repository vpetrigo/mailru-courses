#include <iostream>

void foo() {
  #pragma omp parallel num_threads(8)
  {
    std::cout << 1 << std::endl;
    std::cout << 2 << std::endl;
    #pragma omp barrier
    std::cout << 3 << std::endl;
  }
}

int main() {
  foo();
  
  return 0;
}
