#ifndef __FWS_WORKTHREAD_H__
#define __FWS_WORKTHREAD_H__

#include <pthread.h>
#include "job.h"

// 线程状态
typedef enum
{
    // 空闲
    E_THREAD_STATE_IDEL,
    // 忙碌
    E_THREAD_STATE_BUSY,
}ThreadState;

class CWorkThread
{
public:
    CWorkThread();

    void set_job(CJob *job, void *jobdata);

    int lock();
    int unlock();
    void wait();
    void weakup();
    bool join();

    ThreadState get_state();
    void set_state(ThreadState state);

    unsigned long get_tid();

    bool start();


    CJob *m_job;
    void *m_jobdata;
    bool m_end;
private:

    unsigned long m_tid;
    ThreadState m_state;


	pthread_mutex_t m_mutex;
	pthread_cond_t m_cond;
};

#endif

