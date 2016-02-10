/**
 * Author: vpetrigo
 * Task:
 * Напишите многопоточную программу (POSIX Threads).
 * Должен быть как минимум:
 * Один поток, ожидающий pthread_join.
 * Текст программы должен находиться в файле /home/box/main.cpp
 * PID запущенного процесса должен быть сохранен в файле /home/box/main.pid
 */
#include <iostream>
#include <fstream>
#include <pthread.h>
#include <unistd.h>

void *thread_fun(void *attr) {
  char *msg = static_cast<char *> (attr);
  std::cout << "Hello from the thread" << std::endl;
  std::cout << msg << std::endl;

  *msg = 'H';

  std::cout << "sleep until signal" << std::endl;
  pause();

  return attr;
}

int main() {
  std::ofstream ofs{"/home/box/main.pid"};
  pthread_t first_thread;
  void *retval = nullptr;
  char m[] = "how are you?";

  ofs << getpid() << std::endl;
  pthread_create(&first_thread, nullptr, thread_fun, static_cast<void *> (m));
  pthread_join(first_thread, &retval);

  std::cout << static_cast<char *> (retval) << std::endl;

  return 0;
}
