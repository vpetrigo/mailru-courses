/**
 * Author: vpetrigo
 * Task:
 * Общая память SysV.
 * Создайте общую память SysV (не менее 1 мегабайта) с ключем:
 * key_t key = ftok("/tmp/mem.temp", 1);
 * Каждый байт первого мегабайта общей памяти заполните числом 42.
 */

#include <iostream>
#include <stdexcept>
#include <algorithm>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

int open_segment(const key_t& key, const int seg_size) throw(std::runtime_error) {
  static constexpr std::size_t min_mem_size = 1048576;
  std::size_t mem_size = (seg_size <= min_mem_size) ? min_mem_size : seg_size;
  int shmid = shmget(key, mem_size, IPC_CREAT | 0666);

  if (shmid < 0) {
    throw std::runtime_error{"Cannot create shared memory space"};
  }

  return shmid;
}

int main() {
  key_t key = ftok("/tmp/mem.temp", 1);
  auto shmid = open_segment(key, 1e6);
  char *shared = static_cast<char *> (shmat(shmid, nullptr, 0));
  std::size_t first_mb = 1048576;

  std::fill(shared, shared + first_mb, static_cast<char> (42));

  return 0;
}
