#include "threading.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

// Optional: use these functions to add debug or error prints to your application
#define DEBUG_LOG(msg,...) // printf("threading: " msg "\n", ##__VA_ARGS__)
#define ERROR_LOG(msg,...) printf("threading ERROR: " msg "\n", ##__VA_ARGS__)

void* threadfunc(void* thread_param)
{
    struct thread_data* thread_func_args = (struct thread_data *) thread_param;

    // منتظر ماندن به مدت wait_to_obtain_ms میلی‌ثانیه
    usleep(thread_func_args->wait_to_obtain_ms * 1000);

    // تلاش برای قفل کردن mutex
    int ret = pthread_mutex_lock(thread_func_args->mutex);
    if(ret != 0) {
        ERROR_LOG("Failed to lock mutex.");
        thread_func_args->thread_complete_success = false;
        return thread_param;
    }

    // منتظر ماندن به مدت wait_to_release_ms میلی‌ثانیه
    usleep(thread_func_args->wait_to_release_ms * 1000);

    // آزاد کردن mutex
    ret = pthread_mutex_unlock(thread_func_args->mutex);
    if(ret != 0) {
        ERROR_LOG("Failed to unlock mutex.");
        thread_func_args->thread_complete_success = false;
        return thread_param;
    }

    thread_func_args->thread_complete_success = true;
    return thread_param;
}


bool start_thread_obtaining_mutex(pthread_t *thread, pthread_mutex_t *mutex, int wait_to_obtain_ms, int wait_to_release_ms)
{
    /**
     * تخصیص حافظه برای thread_data، تنظیم فیلدهای مربوطه و ایجاد ترد با استفاده از threadfunc() به عنوان ورودی.
     */
    struct thread_data* data = (struct thread_data*) malloc(sizeof(struct thread_data));
    if (data == NULL) {
        ERROR_LOG("Failed to allocate memory for thread_data.");
        return false;
    }

    data->mutex = mutex;
    data->wait_to_obtain_ms = wait_to_obtain_ms;
    data->wait_to_release_ms = wait_to_release_ms;
    data->thread_complete_success = false;

    int ret = pthread_create(thread, NULL, threadfunc, (void*) data);
    if(ret != 0) {
        ERROR_LOG("Failed to create thread.");
        free(data);
        return false;
    }

    return true;
}
