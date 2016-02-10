/**
 * Author: vpetrigo
 * Task:
 * Создайте общую память POSIX с именем:
 * /test.shm
 * Размер сегмента должен быть 1 мегабайт. Заполните каждый байт числом 13.
 */

#include <iostream>
#include <stdexcept>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main() {
  constexpr auto megabyte = 1 << 20;
  auto shmid = shm_open("/test.shm", O_CREAT | O_WRONLY,  S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);

  if (ftruncate(shmid, megabyte) < 0) {
    throw std::runtime_error{"Cannot truncate"};
  }

  char *memory = mmap(nullptr, megabyte, PROT_READ | PROT_WRITE, MAP_SHARED, shmid, 0);

  for (std::size_t i = 0; i < megabyte; ++i) {
    memory[i] = 13;
  }

  return 0;
}
