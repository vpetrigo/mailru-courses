/*
 *  Author: vpetrigo
 *  Task:
 *  Дана последовательность целых чисел a[0..n-1] и натуральное число k, такое что для любых i, j: если j >= i + k, то a[i] <= a[j]. 
 *  Требуется отсортировать последовательность. Последовательность может быть очень длинной. Время работы O(n * log(k)). Доп. память O(k).
 *
 *  Использовать слияние. 
 *
 *  Sample Input:
 *  20
 *  3 4 1 2 0 9 7 8 6 5 11 12 13 10 14 18 19 16 17 15
 *  10
 *  Sample Output:
 *  0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19
 */

#include <iostream>
#include <vector>
#include <cstddef>
#include <algorithm>

void part_sort(std::vector<int>& v, const std::size_t qb) {
    for (std::size_t i = 0; i < v.size(); i += qb) {
        std::sort(v.begin() + i, v.begin() + i + qb);
    }
}

int main() {
    int n, k;
    std::vector<int> input;
    
    std::cin >> n;
    input.resize(n);
    
    for (auto& e : input) {
        std::cin >> e;
    }
    
    std::cin >> k;
    
    // quantity of blocks to sort
    const std::size_t q_block = n / k;
    const std::size_t block_size = n / q_block;
    
    part_sort(input, block_size);
    
    for (const auto& e : input) {
        std::cout << e << ' ';
    }
    std::cout << std::endl;
    
    return 0;
}