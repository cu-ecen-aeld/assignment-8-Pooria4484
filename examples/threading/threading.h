#ifndef THREADING_H
#define THREADING_H

#include <stdbool.h>
#include <pthread.h>

/**
 * ساختار داده‌های ترد. 
 * فیلدهای اضافی (mutex، wait_to_obtain_ms و wait_to_release_ms) جهت برقراری ارتباط بین تابع شروع ترد و ترد اصلی اضافه شده‌اند.
 */
struct thread_data{
    pthread_mutex_t *mutex;
    int wait_to_obtain_ms;
    int wait_to_release_ms;
    /**
     * true اگر ترد بدون خطا به پایان رسیده باشد،
     * false در غیر این صورت.
     */
    bool thread_complete_success;
};

bool start_thread_obtaining_mutex(pthread_t *thread, pthread_mutex_t *mutex, int wait_to_obtain_ms, int wait_to_release_ms);

#endif // THREADING_H
