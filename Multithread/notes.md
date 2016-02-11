## POSIX Thread

Документация:

* [pthreads](http://man7.org/linux/man-pages/man7/pthreads.7.html)
* [pthread_create](http://man7.org/linux/man-pages/man3/pthread_create.3.html)
* [pthread_join](http://man7.org/linux/man-pages/man3/pthread_join.3.html)
* [pthread_detach](http://man7.org/linux/man-pages/man3/pthread_detach.3.html)


Функция создания потока:
```cpp
pthread_create(pthread_t *thread_id, const pthread_attr_t *attr, void *(*start_fcn)(void *), void *arg);
```
_arg_ - будет передан в функцию `start_fcn`.

Пример:
```cpp
// Функция увеличивает переданное в неё число
// и возвращает увеличенное значение
void *thread_func(void *value) {
    // передаем целое число
    int *int_value = static_cast<int *> value;

    ++(*int_value);

    return int_value;
}
```

Потоки могут быть двух классов:

1. **Joinable** - похожи на процессы UNIX. Вызывая функции `wait` или `waitpid`, мы ожидаем статусы.
2. **Detached** - статус не ожидается

### Joinable-потоки

Любой поток при создании, если явно не указано другого, является *Joinable*. Идет
основной поток, мы вызвали `pthread_create`:

```
|
|
|
| pthread_create
x-----
|     |
|     |
|     x <- завершение и засыпание
|
| pthread_join
x <--- возврат результата из дочернего потока
|
```

Второй сценарий:

```
|
|
|
| pthread_create
x-----
|     |
x <---|---- pthread_join
      |
      |
      |
x <---|
```

```cpp
int pthread_join(pthread_t thread, void **retval);
```

_retval_ - получение возвращаемого значение из дочернего потока.


### Detached-потоки

Поток перестает нуждаться в join'е.

```
|
|
|
| pthread_create
x-----
|     |
|     |
|     |
|     x pthread_detach
|     |
|     x <- завершение потока
|
|
|
```

Но в этом случае возвращаемый результат теряется.

```cpp
int pthread_detach(pthread_t thread);
```

Что будет если потоки вызовут `join` друг для друга? Система не даст этого сделать,
так как получится **deadlock**. В этом случае `pthread_join` вернет `EDEADLK`.

Поток можно преобразовать из *Joinable* в *Detached*, но наоборот - нельзя.
Чтобы создать сразу *Detached*-поток, при создании необходимо задать соответствующий статус в структуре
`pthread_attr_t *attr` (для инициализации и удаления структуры используются функции
`pthread_attr_init` и `pthread_attr_destroy`). Для это используется функция

```cpp
int pthread_setdetachstate(pthread_attr_t *attr, int detachstate);
```
_detachstate_ - может быть двух типов - `PTHREAD_CREATE_JOINABLE` или `PTHREAD_CREATE_DETACHED`.

### Завершение потоков

Бывет явным и неявным. Пример неявного завершения:

```cpp
void *thread_func(void *arg) {
  // ...
  // ...
  return nullptr // <- Implicit termination
}
```

Для явного завершения потока используется функция:

```cpp
void pthread_exit(void *retval); // Explicit termination
```

Другой способ явно завершить поток, это отправка команды на завершение из другого потока.
Например:

```
|
|
x----
|   |
|   |
x <-x---- pthread_cancel
|
|
|
```

В этом случае мы не получаем возвращаемое значение дочернего потока.

```cpp
int pthread_cancel(pthread_t pthread);
```

Убить поток можно только во время исполнения какого-либо системного вызова. Например, во время
`read`, `waitpid` и т.д. Если мы используем конструкцию `while (true) {}`, то поток мы не сможем убить.
Функции `pthread_*`, `free`, `malloc`, `realloc`, `calloc` не являются точками завершения.
Мы можем самостоятельно установить точку завершения с помощью функции `void pthread_testcancel(void)`.

Ещё один момент:

```
|
| pthread_create
x---------
|         |
|         |
x----
|    |    |
|    |    |
|    x--->|---- pthread_join
|     ----|----> join вернул *retval = PTHREAD_CANCELED
x----x--->|---- pthread_cancel
|    |
|    |
```

Есть также параметры потока, которые мы можем установить находясь внутри него.

```cpp
int pthread_cancelstate(int state, int *oldstate);
```

_state_ - PTHREAD_CANCEL_ENABLED - можем отменять выполнение потока с помощью
`pthread_cancel`. PTHREAD_CANCEL_DISABLED - поток не может быть завершен с помощью
`pthread_cancel`.

```cpp
int pthread_canceltype(int type, int *oldtype);
```

_type_ - может принимать два значения:

* PTHREAD_CANCEL_DEFERED - стандартный режим с точками завершения
* PTHREAD_CANCEL_ASYNCHRONOUS - в этом режиме мы не ждем точку завершения, а можем сразу
прервать работу потока.

Для установки функции, которая будет вызываться при завершении работы потока имеются
следующие вызовы:

```cpp
void pthread_cleanup_push(...); // для установки функции
void pthread_cleanup_pop(...); // для сброса
```

## Мьютексы

Потоки являются частью одного процесса и вся память процесса является для них общей.
Как из разных потоков работать с общей памятью? Для этого существует несколько способов и
один из них это использование **мьютексов**.

Мьютекс как бинарный семафор (флаг), может находится либо в активном состоянии («1»), либо
в неактивном («0»). Это позволяет только одному процессу работать с каким-либо общим ресурсом
в конкретный момент времени.

Для создания мьютекса используется:

```cpp
pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER; // создание мьютекса

int pthread_mutex_init(pthread_mutex_t *mp, pthread_mutex_attr_t *mattrp); // инициализация памяти под мьютекс
```

Для удаления мьютекса используется функция:

```cpp
int pthread_mutex_destroy(pthread_mutex_t *mp);
```

Пример работы потоков с мьютексом:

```
|   |
|   |
x <-|--- lock_1
|   o <- lock_2
|   -    поток 2 спит
|   -    поток 2 спит
x <-x--- unlock_1 / lock_2
```

Для работы с блокировками мьютекса существуют следующие функции:

```cpp
int pthread_mutex_lock(pthread_mutex_t *mp); // lock в блокирующем режиме
int pthread_mutex_trylock(pthread_mutex_t *mp); // lock в неблокирующем режиме
                                                // в случае, если мьютекс занят, то вернется отказ
int pthread_mutex_unlock(pthread_mutex_t *mp); // снятие блокировки
```

### spin-блокировки

При работе с блокировками, если время работы первого процесса мало, то вместо того, чтобы заснуть и ожидать
освобождения мьютекса, целесообразнее использовать spin-блокировку. В этом случае другой поток, ожидающий
разблокировки мьютекса в бесконечном цикле постоянно проверяет мьютекс и «греет камень».

```
pthread_spin_lock_t psl;

int pthread_spin_init(pthread_spin_lock_t *lock, int pshared); // инициализация
```

spin-локи могут работать для синхронизации потоков в одном процессе, так и для синхронизации
потоков разных процессов! То есть можно синхронизировать процессы. В таком случае
переменная `pshared` должна принять значение `PTHREAD_PROCESS_SHARED`.
Если же `pshared` равно `PTHREAD_PROCESS_PRIVATE`, то в таком случае spin-lock используется в
рамках одного процесса.

Для уничтожения spin-lock'а используется:

```cpp
int pthread_spin_destroy(pthread_spin_lock_t *lock);
```

Для работы со spin-блокировками используются функции:

```cpp
int pthread_spin_destroy(pthread_spin_lock_t *lock);
int pthread_spin_lock(pthread_spin_lock_t *lock); // занять spin-lock (в блокирующем режиме)
int pthread_spin_trylock(pthread_spin_lock_t *lock); // занять spin-lock (в неблокирующем режиме)
int pthread_spin_unlock(pthread_spin_lock_t *lock); // освободить spin-lock
```

**Важно!** Если блокировки короткие по времени, то рекомендуется использовать spin-блокировки
вместо засыпания потока.

### Блокировки на запись и на чтение

* [PTHREAD_RWLOCK](https://www.daemon-systems.org/man/pthread_rwlock.3.html)

```cpp
pthread_rwlock_t *rwlock = PTHREAD_RWLOCK_INITIALIZER;

pthread_rwlock_init(...);
pthread_rwlock_destroy(...);
```

Установка блокировок на чтение и запись:

```cpp
int pthread_rwlock_rdlock(*rwlock);
int pthread_rwlock_tryrdlock(*rwlock);
int pthread_rwlock_wrlock(*rwlock);
int pthread_rwlock_trywrlock(*rwlock);
```
