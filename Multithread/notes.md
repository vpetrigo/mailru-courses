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
