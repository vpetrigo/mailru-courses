/*
 *  Author: vpetrigo
 *  Task:
 *  Задано N точек на плоскости. Указать (N-1)-звенную несамопересекающуюся незамкнутую ломаную, 
 *  проходящую через все эти точки.
 *  Стройте ломаную в порядке возрастания x-координаты. Если имеются две точки с одинаковой x-координатой, 
 *  то расположите раньше ту точку, у которой y-координата меньше.
 *
 *  Для сортировки точек реализуйте пирамидальную сортировку.
 *
 *  Sample Input:
 *  4
 *  0 0
 *  1 1
 *  1 0
 *  0 1
 *  Sample Output:
 *  0 0
 *  0 1
 *  1 0
 *  1 1  
 */

#include <iostream>
#include <vector>
#include <utility>
#include <algorithm>
#include <cassert>
#include <functional>

template <typename T, typename Compare = std::less<T>>
class Heap {
public:
    Heap() : heap{}, comp{Compare{}}, size{0} {}
    Heap(const std::initializer_list<T>& lst) : heap{lst}, comp{Compare{}}, size{lst.size()} {
        heapify();
    }
    
    void push(const T& elem) {
        // add element to the heap
        heap.push_back(elem);
        ++size;
        
        if (size > 1) {
            sift_up(size - 1);
        }
    }
    
    const T& get_max() const {
        return heap.front();
    }
    
    void pop() {
        // pop the max element
        if (size == 0) {
            return;
        }
        
        constexpr size_t heap_top = 0;

        std::swap(heap.front(), heap.back());
        heap.pop_back();
        --size;
        sift_down(heap_top);
    }
    
    size_t get_size() const {
        return size;
    }
private:
    void heapify() {
        auto non_leaf_index = heap.size() / 2 - 1;
        
        while (non_leaf_index < size) {
            sift_down(non_leaf_index--);
        }
    }
    
    // sift down element in the heap with [elem_index]
    void sift_down(size_t elem_index) {
        auto left_child_index = 2 * elem_index + 1;
        auto right_child_index = 2 * elem_index + 2;
        
        while (left_child_index < size) {
            auto cur_value = heap[elem_index];
            auto left_child_value = heap[left_child_index];

            size_t index_to_swap = elem_index;
            
            if (comp(left_child_value, cur_value)) {
                index_to_swap = left_child_index;
                cur_value = left_child_value;
            }
            
            if (right_child_index < size) {
                auto right_child_value = heap[right_child_index];
                
                if (comp(right_child_value, cur_value)) {
                    index_to_swap = right_child_index;
                }
            }
            
            if (elem_index == index_to_swap) {
                break;
            }
            
            std::swap(heap[elem_index], heap[index_to_swap]);
            elem_index = index_to_swap;
            left_child_index = 2 * elem_index + 1;
            right_child_index = 2 * elem_index + 2;
        }
    }
    
    // sift up element in the heap with [elem_index]
    void sift_up(size_t elem_index) {
        assert(elem_index > 0 && elem_index < size);

        while (elem_index != 0) {
            const auto parent_index = (elem_index - 1) / 2;
            auto& elem = heap[elem_index];
            auto& parent = heap[parent_index];
            
            if (!comp(elem, parent)) {
                break;
            }
            
            std::swap(elem, parent);
            elem_index = parent_index;
        }
    }

    std::vector<T> heap;
    Compare comp;
    size_t size;
};

int main() {
    int n;
    Heap<std::pair<int, int>> h;
    
    std::cin >> n;
    
    // read coordinates
    for (int i = 0; i < n; ++i) {
        int x, y;
        
        std::cin >> x >> y;
        h.push(std::make_pair(x, y));
    }
    // extract points
    while (h.get_size()) {
        std::cout << h.get_max().first << ' ' << h.get_max().second << std::endl;
        h.pop();
    }
    
    return 0;
}