#include <stdio.h>
#include "workthread.h"

// 执行任务线程函数
void *thread_func(void *args)
{
    CWorkThread *thr = (CWorkThread *)args;

    while(1)
    {
        // 加锁
        thr->lock();

        if ( thr->m_end )
            return NULL;

        if ( thr->m_job == 0 )
            thr->wait();

        if ( thr->m_end )
            return NULL;

        thr->m_job->run(thr->m_jobdata);

        thr->set_job(NULL, NULL);
        thr->set_state(E_THREAD_STATE_IDEL);

        // 解锁
        thr->unlock();
    }
}

CWorkThread::CWorkThread()
{
    m_job = NULL;
    m_jobdata = NULL;
    m_state = E_THREAD_STATE_IDEL;
    m_end = false;
}

int CWorkThread::lock()
{
    if(0 != pthread_mutex_lock(&m_mutex))
    {
        return -1;
    }
    return 0;
}

int CWorkThread::unlock()
{
    if(0 != pthread_mutex_unlock(&m_mutex))
    {
        return -1;
    }
    return 0;
}

void CWorkThread::wait()
{
    pthread_cond_wait(&m_cond, &m_mutex); 
}

void CWorkThread::weakup()
{
	pthread_cond_signal(&m_cond);
}

bool CWorkThread::join()
{
    lock();
    m_end = true;
    unlock();

	pthread_cond_signal(&m_cond);
    if ( pthread_join(m_tid, NULL) != 0 )
        return false;

    return true;
}

bool CWorkThread::start()
{
	if ( pthread_mutex_init(&m_mutex, NULL) != 0 )
        return false;

	if ( pthread_cond_init(&m_cond, NULL) != 0 )
        return false;
    
    if ( thread_func == NULL )
        return false;
    

    if( pthread_create(&m_tid, NULL, thread_func, (void *)this) != 0 ) 
    {
        printf("create thread failed!\n");
        return false;
    }
    m_state = E_THREAD_STATE_IDEL;

    return true;
}

ThreadState CWorkThread::get_state()
{
    return m_state;
}

void CWorkThread::set_state(ThreadState state)
{
    m_state = state;
}

unsigned long CWorkThread::get_tid()
{
    return m_tid;
}

void CWorkThread::set_job(CJob *job, void *jobdata)
{
    m_job = job;
    m_jobdata = jobdata;
}

