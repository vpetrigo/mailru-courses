/**
 * Author: vpetrigo
 * Task:
 * Напишите многопоточную программу (POSIX Threads).
 * Должны быть как минимум:
 * Один поток, ожидающий освобождения мьютекса.
 * Один поток, ожидающий освобождения спин-блокировки.
 * Два потока, ожидающих освобождения RW-блокировки (один должен ожидать чтения, другой - записи).
 * Текст программы должен находиться в файле /home/box/main.cpp
 * PID запущенного процесса должен быть сохранен в файле /home/box/main.pid
 */

#include <iostream>
#include <fstream>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>

// thread waiting for mutex
void *thread1_fn(void *arg) {
  std::cout << "Thread 1 try to get access to mutex" << std::endl;
  auto *mutex_ptr = static_cast<pthread_mutex_t *> (arg);
  pthread_mutex_lock(mutex_ptr);
  std::cout << "Thread 1 get access to mutex" << std::endl;
  pthread_mutex_unlock(mutex_ptr);

  return nullptr;
}

// thread waiting for spinlock
void *thread2_fn(void *arg) {
  std::cout << "Thread 2 try to get access to mutex" << std::endl;
  pthread_spin_lock(static_cast<pthread_spinlock_t *> (arg));

  return nullptr;
}

// thread waiting for read rdlock
void *thread3_fn(void *arg) {
  std::cout << "Thread 3 try to get access to mutex" << std::endl;
  pthread_rwlock_rdlock(static_cast<pthread_spinlock_t *> (arg));

  return nullptr;
}

// thread waiting for write rdlock
void *thread4_fn(void *arg) {
  std::cout << "Thread 4 try to get access to mutex" << std::endl;
  pthread_rwlock_wrlock(static_cast<pthread_spinlock_t *> (arg));

  return nullptr;
}

int main() {
  pthread_t thread1, thread2, thread3, thread4;
  std::ofstream ofs{"main.pid"};
  pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;
  pthread_rwlock_t rwl = PTHREAD_RWLOCK_INITIALIZER;
  pthread_spinlock_t spl;

  std::cout << "Start init..." << std::endl;
  pthread_spin_init(&spl, PTHREAD_PROCESS_PRIVATE);
  std::cout << "Write to file..." << std::endl;
  ofs << getpid() << std::endl;
  std::cout << "Lock mutex..." << std::endl;
  pthread_mutex_lock(&mut);
  std::cout << "Lock spin..." << std::endl;
  pthread_spin_lock(&spl);
  std::cout << "Lock rdlock..." << std::endl;
  pthread_rwlock_rdlock(&rwl);
//  pthread_rwlock_wrlock(&rwl);
  std::cout << "Start thread 1..." << std::endl;
  pthread_create(&thread1, nullptr, thread1_fn, &mut);
  std::cout << "Start thread 2..." << std::endl;
  pthread_create(&thread2, nullptr, thread2_fn, &spl);
  std::cout << "Start thread 3..." << std::endl;
  pthread_create(&thread3, nullptr, thread3_fn, &rwl);
  std::cout << "Start thread 4..." << std::endl;
  pthread_create(&thread4, nullptr, thread4_fn, &rwl);
  pthread_mutex_unlock(&mut);

  pthread_join(thread1, nullptr);

  return 0;
}
