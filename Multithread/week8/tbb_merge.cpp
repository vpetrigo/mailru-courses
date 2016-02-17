#include <iostream>
#include <iterator>
#include <vector>
#include <algorithm>
#include "tbb/parallel_for.h"

void Foo(float num) {
  std::cout << "Num: " << num << std::endl;
}

void ParallelApplyFoo(float a[], int n) {
  auto Apply = [=](const tbb::blocked_range<size_t>& r) {
    for (size_t i = r.begin(); i != r.end(); ++i) {
      Foo(a[i]);
    }
  };

  tbb::parallel_for(tbb::blocked_range<size_t>(0, n), Apply);
}

template <typename It>
std::vector<typename std::iterator_traits<It>::value_type> merge(It& begin, It& mid, It& end) {
  auto first_begin = begin;
  auto second_begin = mid;
  const auto first_end = mid;
  const auto second_end = end;
  std::vector<typename std::iterator_traits<It>::value_type> v;

  while (first_begin != first_end && second_begin != second_end) {
    if (*first_begin <= *second_begin) {
      v.emplace_back(*first_begin++);
    }
    else {
      v.emplace_back(*second_begin++);
    }
  }

  std::copy_backward(first_begin, first_end, v.end());
  std::copy_backward(second_begin, second_end, v.end());

  return v;
}

template <typename It>
void merge_sort(It& begin, It& end) {
  auto size = std::distance(begin, end);

  if (size > 1) {
    auto mid = std::advance(begin, size / 2);
    merge_sort(begin, mid);
    merge_sort(mid, end);
    merge(begin, mid, end);
  }
}

int main() {
  float arr[] = {0.3, .4, .5, .1, 2.3, 4.123};

  ParallelApplyFoo(arr, sizeof arr / sizeof arr[0]);

  return 0;
}
