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
using iterator_traits_t = typename std::iterator_traits<It>::value_type;

template <typename It>
std::vector<iterator_traits_t<It>> merge(It begin, It mid, It end) {
    // init both parts pointers to the begining and to the ending
    std::vector<iterator_traits_t<It>> sorted;
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
    int prev_layer = 0;
    int cur_layer = 0;
    int dist = 0;
    
    for (size_t i = 0; i < v.size(); ++i) {
        if (v[i].second == begin_segment) {
            ++cur_layer;
        }
        else {
            --cur_layer;
        }
        // prev_layer == 1 and cur_layer == 2 when we get to the overlap segment
        // prev_layer == 1 and cur_layer == 0 when we get to the end of nonoverlaped segment
        if ((prev_layer == 1 && cur_layer == 2) || (prev_layer == 1 && cur_layer == 0)) {
            dist += v[i].first - v[i - 1].first;
        }
        
        prev_layer = cur_layer;
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