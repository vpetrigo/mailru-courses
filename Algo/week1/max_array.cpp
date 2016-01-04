#include <iostream>
#include <vector>
#include <utility>

void fill_array(std::vector<int>& v) {
  for (auto& elem : v) {
    std::cin >> elem;
  }
}

void fill_max(const std::vector<int>& a, std::vector<std::pair<int, int> >& max_a) {
  for (size_t i = 1; i < a.size(); ++i) {
    if (max_a[i - 1].first < a[i]) {
      max_a[i] = std::make_pair(a[i], i);
    }
    else {
      max_a[i] = max_a[i - 1];
    }
  }
}

std::pair<int, int> max_ind(const std::vector<int>& a, const std::vector<int>& b) {
  std::vector<std::pair<int, int> > max_a(a.size(), std::make_pair(a.front(), 0));
  fill_max(a, max_a);

  std::pair<int, int> ans {0, 0};
  int max_sum = b.front() + max_a.front().first;

  for (size_t i = 1; i < b.size(); ++i) {
    if (max_a[i].second <= i) {
      const int cur_sum = max_a[i].first + b[i];
      if (max_sum < cur_sum) {
        ans.first = max_a[i].second;
        ans.second = i;
        max_sum = cur_sum;
      }
    }
  }

  return ans;
}

int main() {
  int n;

  std::cin >> n;

  std::vector<int> A(n);
  std::vector<int> B(n);

  fill_array(A);
  fill_array(B);

  const auto&& ans = max_ind(A, B);

  std::cout << ans.first << ' ' << ans.second << std::endl;

  return 0;
}
