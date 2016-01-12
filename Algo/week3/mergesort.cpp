/*
 *  Author: vpetrigo
 *  Task:
 *  На числовой прямой окрасили N отрезков. Известны координаты левого и правого концов каждого отрезка (Li и Ri). 
 *  Найти сумму длин частей числовой прямой, окрашенных ровно в один слой.
 *
 *  Для сортировки реализуйте сортировку слиянием.
 *
 *  Sample Input:
 *  3
 *  1 4
 *  7 8
 *  2 5
 *  Sample Output:
 *  3
 */

#include <iostream>
#include <vector>
#include <utility>
#include <algorithm>
#include <iterator>

constexpr int begin_segment = -1;
constexpr int end_segment = 1;

template <typename It>
std::vector<typename std::iterator_traits<It>::value_type> merge(It begin, It mid, It end) {
    // init both parts pointers to the begining and to the ending
    std::vector<typename std::iterator_traits<It>::value_type> sorted;
    auto first_begin = begin;
    auto second_begin = mid;
    const auto first_end = mid;
    const auto second_end = end;
    
    while (first_begin != first_end && second_begin != second_end) {
        if (*first_begin <= *second_begin) {
            sorted.push_back(*first_begin++);
        }
        else {
            sorted.push_back(*second_begin++);
        }
    }
    
    std::copy(first_begin, first_end, std::back_inserter(sorted));
    std::copy(second_begin, second_end, std::back_inserter(sorted));
    
    return sorted;
}

template <typename It>
void merge_sort(It begin, It end) {
    auto size = std::distance(begin, end);
    
    if (size == 1) {
        return;
    }
    
    auto mid = std::next(begin, size / 2);
    merge_sort(begin, mid);
    merge_sort(mid, end);
    
    auto&& result = merge(begin, mid, end);
    std::move(result.begin(), result.end(), begin);
}

int check_segments(const std::vector<std::pair<int, int>>& v) {
    int layer = 0;
    int dist = 0;
    
    for (size_t i = 0; i < v.size(); ++i) {
        if (v[i].second == begin_segment) {
            if (layer == 1) {
                dist += v[i].first - v[i - 1].first;
            }
            
            ++layer;
        }
        else {
            --layer;
            if (layer == 0) {
                dist += v[i].first - v[i - 1].first;
            }
        }
    }
    
    return dist;
}

int main() {
    int n;
    std::vector<std::pair<int, int>> inp;
    
    std::cin >> n;
    
    for (int i = 0; i < n; ++i) {
        int b, e;
        
        std::cin >> b >> e;
        inp.push_back({b, begin_segment});
        inp.push_back({e, end_segment});
    }
    
    merge_sort(inp.begin(), inp.end());
    auto&& result = check_segments(inp);
    
    std::cout << result << std::endl;
    
    return 0;
}