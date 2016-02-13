/**
 * Author: vpetrigo
 * Task:
 * Test
 */

#include <iostream>
#include <vector>
#include <pthread.h>

pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;

void *thread1_fn(void *arg) {
  auto *vptr = static_cast<std::vector<std::string> *> (arg);
  int i = 10;

  while (i--) {
    pthread_mutex_lock(&m);
    std::cout << "Thread 1: Enter your string: ";
    std::string s;
    std::cin >> s;
    vptr->emplace_back(std::move(s));
    pthread_mutex_unlock(&m);
  }

  return nullptr;
}

void *thread2_fn(void *arg) {
  auto *vptr = static_cast<std::vector<std::string> *> (arg);
  int i = 10;

  while (i--) {
    pthread_mutex_lock(&m);
    std::cout << "Thread 2: Enter your string: ";
    std::string s;
    std::cin >> s;
    vptr->emplace_back(std::move(s));
    pthread_mutex_unlock(&m);
  }

  return nullptr;
}

int main() {
  std::vector<std::string> v;
  pthread_t thread1, thread2;

  pthread_create(&thread1, nullptr, thread1_fn, &v);
  pthread_create(&thread2, nullptr, thread2_fn, &v);
  pthread_join(thread1, nullptr);
  pthread_join(thread2, nullptr);

  for (const auto& e : v) {
    std::cout << e << ' ';
  }
  std::cout << std::endl;

  return 0;
}
