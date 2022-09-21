#include "threading.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <syslog.h>
// Optional: use these functions to add debug or error prints to your application
#define DEBUG_LOG(msg,...)
//#define DEBUG_LOG(msg,...) printf("threading: " msg "\n" , ##__VA_ARGS__)
#define ERROR_LOG(msg,...) printf("threading ERROR: " msg "\n" , ##__VA_ARGS__)

void* threadfunc(void* thread_param)
{

    // TODO: wait, obtain mutex, wait, release mutex as described by thread_data structure
    // hint: use a cast like the one below to obtain thread arguments from your parameter
    //struct thread_data* thread_func_args = (struct thread_data *) thread_param;
    struct thread_data *actual_thread_param =(struct thread_data *) thread_param; 
    actual_thread_param->thread_complete_success = true;

    //syslog(LOG_CRIT, "hello syslog");
    //sleep for wait_to_obtain_ms usleep(microseconds)
    usleep(actual_thread_param->wait_to_obtain_ms*1000);

    //aquire mutex
    syslog(LOG_CRIT, "Thread will now obtain mutex lock after waiting %d ms", actual_thread_param->wait_to_obtain_ms);
    int rc = 0;
    rc = pthread_mutex_lock(actual_thread_param->mutex);
    if( rc != 0 ){
        syslog(LOG_CRIT, "\n\n\npthread_mutex_lock failed with %d\n", rc);
        actual_thread_param->thread_complete_success = false;
    }
    else{
            //sleep for wait_to_release_ms usleep(microseconds)
        usleep(actual_thread_param->wait_to_release_ms*1000);

    
        //release mutex
        syslog(LOG_CRIT, "Thread will now release mutex lock after waiting %d ms", actual_thread_param->wait_to_release_ms);
        rc = pthread_mutex_unlock(actual_thread_param->mutex);
        if( rc != 0 ){
            syslog(LOG_CRIT, "\n\n\npthread_mutex_unlock failed with %d\n", rc);
            actual_thread_param->thread_complete_success = false;
            //return actual_thread_param;
        }
    }

    syslog(LOG_CRIT, "Thread return with value %d (1 is true, 0 is false)", actual_thread_param->thread_complete_success);
    return thread_param;
}


bool start_thread_obtaining_mutex(pthread_t *thread, pthread_mutex_t *mutex,int wait_to_obtain_ms, int wait_to_release_ms)
{
    /**
     * TODO: allocate memory for thread_data, setup mutex and wait arguments, pass thread_data to created thread
     * using threadfunc() as entry point.
     *
     * return true if successful.
     *
     * See implementation details in threading.h file comment block
     */



/**
* Start a thread which sleeps @param wait_to_obtain_ms number of milliseconds, then obtains the
* mutex in @param mutex, then holds for @param wait_to_release_ms milliseconds, then releases.
* The start_thread_obtaining_mutex function should only start the thread and should not block
* for the thread to complete.
* The start_thread_obtaining_mutex function should use dynamic memory allocation for thread_data
* structure passed into the thread.  The number of threads active should be limited only by the
* amount of available memory.
* The thread started should return a pointer to the thread_data structure when it exits, which can be used
* to free memory as well as to check thread_complete_success for successful exit.
* If a thread was started succesfully @param thread should be filled with the pthread_create thread ID
* coresponding to the thread which was started.
* @return true if the thread could be started, false if a failure occurred.
*/
    int rc = 0;

    //allocate the struct 
    struct thread_data * thread_param = malloc(sizeof *thread_param);

    //initialize the struc data with the two wait parameters in the struct
    thread_param->wait_to_obtain_ms = wait_to_obtain_ms;
    thread_param->wait_to_release_ms = wait_to_release_ms;
    thread_param->mutex = mutex;
    
    syslog(LOG_CRIT, "the wait_to_obtain_ms is %d", thread_param->wait_to_obtain_ms);
    syslog(LOG_CRIT, "the wait_to_release_ms is %d", thread_param->wait_to_release_ms);
    //initialize mutex
    //rc = pthread_mutex_init(thread_param->mutex,NULL);
    if(rc != 0){
        //error
        syslog(LOG_CRIT, "\n\n\nfailed to initialize mutex, error was %d\n", rc);
        return false;
    }

    //Start a thread, and pass the struct pointer as the arguement

    rc = pthread_create(thread, NULL, threadfunc, thread_param);
    if(rc != 0){
        syslog(LOG_CRIT, "\n\n\nfailed to create thread with error %d\n", rc);
        return false;
    }
    else{
        syslog(LOG_CRIT, "Thread created successfully with return code %d", rc);
        return true;
    }
    
    return false;
}

