#include <iostream>
#include <vector>
#include <algorithm>

template <typename Container>
void fill_container_with_numbers(Container& c) {
    int num;

    while (std::cin >> num) {
        c.push_back(num);
    }
}

template <typename Container>
void print_container(const Container& c) {
    for (const auto& elem : c) {
        std::cout << elem << ' ';
    }
    std::cout << std::endl;
}

void insertion_sort(std::vector<int>& v) {
    for (size_t i = 1; i < v.size(); ++i) {
        int j = i;

        while (j > 0 && v[j - 1] > v[j]) {
            std::swap(v[j - 1], v[j]);
            --j;
        }
    }
}

int main() {
    std::vector<int> v;
    
    fill_container_with_numbers(v);
    insertion_sort(v);
    print_container(v);

    return 0;
}
