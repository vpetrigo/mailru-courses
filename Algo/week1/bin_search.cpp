#include <iostream>
#include <algorithm>
#include <vector>
#include <utility>

using namespace std;

void fill_vector(vector<int>& v) {
  for (auto& e : v) {
    cin >> e;
  }
}

void print_elem(const int e, const string& msg) {
  cout << msg << e << endl;
}

pair<int, int> find_interval(const vector<int>& a, const int elem) {
  constexpr int two = 1;
  const size_t len = a.size();
  int p = 0;
  // left index of a vector
  int idx_left = p;
  // 2 ^ 0
  int idx_right = two;

  while (idx_right < len && a[idx_right] < elem) {
    ++p;
    idx_left = idx_right;
    idx_right = two << p;
  }

  if (idx_right > len) {
    if (idx_left > len) {
      idx_left = len;
    }
    idx_right = len;
  }

  return make_pair(idx_left, idx_right);
}

vector<int> find_elem(const vector<int>& a, const vector<int>& b) {
  vector<int> found_indexes;

  for (const auto& elem : b) {
    // find 2^p; 2^(p + 1) interval
    const auto&& indexes = find_interval(a, elem);
    // look for elem in bounds [begin() + left, begin() + right]
    const auto low_elem = lower_bound(a.begin() + indexes.first, a.begin() + indexes.second, elem);
    size_t index;
    // calculate which bound is closer to a current element
    if (low_elem == a.begin()) {
      index = 0;
    }
    else if (low_elem == a.end()) {
      index = distance(a.begin(), a.end()) - 1;
    }
    else {
      const int diff_r = *low_elem - elem;
      const int diff_l = elem - *(low_elem - 1);

      if (diff_r == 0 || diff_r < diff_l) {
        index = distance(a.begin(), low_elem);
      }
      else {
        index = distance(a.begin(), low_elem - 1);
      }
    }

    found_indexes.push_back(index);
  }

  return found_indexes;
}

int main() {
  int n, m;
  vector<int> a;
  vector<int> b;

  cin >> n;
  a.resize(n);
  fill_vector(a);

  cin >> m;
  b.resize(m);
  fill_vector(b);

  const auto&& ans = find_elem(a, b);

  for (const auto& elem : ans) {
    cout << elem << ' ';
  }
  cout << endl;

  return 0;
}
