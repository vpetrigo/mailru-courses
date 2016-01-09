# Author: vpetrigo
# Task:
# Жадина.
#
# Вовочка ест фрукты из бабушкиной корзины. В корзине лежат фрукты разной массы.
# Вовочка может поднять не более K грамм. Каждый фрукт весит не более K грамм.
# За раз он выбирает несколько самых тяжелых фруктов, которые может поднять одновременно,
# откусывает от каждого половину и кладет огрызки обратно в корзину. Если фрукт весит нечетное число грамм,
# он откусывает большую половину. Фрукт массы 1гр он съедает полностью.
# Определить за сколько подходов Вовочка съест все фрукты в корзине.
#
# Напишите свой класс кучи, реализующий очередь с приоритетом.
#
# Формат входных данных. Вначале вводится n - количество фруктов и строка с
# целочисленными массами фруктов через пробел. Затем в отдельной строке K - "грузоподъемность".
#
# Формат выходных данных. Неотрицательное число - количество подходов к корзине.
#
# Sample Input:
# 3
# 1 2 2
# 2
# Sample Output:
# 4

import heapq
import sys
import math

class maxheap:
    def __init__(self, lst):
        self.heap = [-x for x in lst]
        heapq.heapify(self.heap)

    def push(self, item):
        heapq.heappush(self.heap, -item)

    def max(self):
        return -self.heap[0]

    def pop(self):
        assert len(self.heap) > 0

        return -heapq.heappop(self.heap)

    def __len__(self):
        return len(self.heap)

def calculate_steps(apple_bucket, weight):
    counter = 0
    priority_bucket = maxheap(apple_bucket)
    # untill we our bucket is not empty
    while len(priority_bucket):
        apples_to_eat = []
        cur_weight = 0
        while cur_weight <= weight and len(priority_bucket):
            max_apple_weight = priority_bucket.max()
            # if we are overweighted with a new apple from the bucket
            if cur_weight + max_apple_weight > weight:
                break
            # add an apple to the queue for eating :)
            apples_to_eat.append(priority_bucket.pop())
            cur_weight += max_apple_weight

        for apple in apples_to_eat:
            # if apple weight is 1 we just skip it, otherwise we eat greater piece
            # and put it back to out apple bucket
            if apple != 1:
                priority_bucket.push(math.floor(apple / 2))

        counter += 1

    return counter

def main():
    reader = (tuple(map(int, line.split())) for line in sys.stdin)
    n, *dummy = next(reader)
    vals = next(reader)
    possible_weight, *dummy = next(reader)

    print(calculate_steps(vals, possible_weight))

if __name__ == '__main__':
    main()
