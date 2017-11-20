#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "threadpool.h"

// 默认线程池10个线程
CFwsThreadPool::CFwsThreadPool()
{
    m_initnum = 10;
    CreateIdleThread(m_initnum);
}

// 指定线程个数线程池
CFwsThreadPool::CFwsThreadPool(int initnum)
{
    m_initnum = initnum;
    CreateIdleThread(m_initnum);
}

// 创建空闲线程
void CFwsThreadPool::CreateIdleThread(int num)
{
    for ( unsigned int i = 0; i < m_initnum; ++i )
    {
        CWorkThread *thread = (CWorkThread *)malloc(sizeof(CWorkThread));
        if ( thread != NULL )
        {
            m_idle_list.push_back(thread);
        }
        else
        {
            printf("WorkThread malloc error\n");
            break;
        }
    }
}

// 开启线程池
int CFwsThreadPool::start()
{
	if ( pthread_mutex_init(&m_mutex_idle, NULL) != 0 )
        return -1;

	if ( pthread_mutex_init(&m_mutex_busy, NULL) != 0 )
        return -1;

    vector<CWorkThread *>::iterator it = m_idle_list.begin();
    while( it != m_idle_list.end() )
    {
        if ( (*it)->start() < 0 )
        {
            return -1;
        }
        it++;
    }
    return 0;
}
int CFwsThreadPool::get_initnum()
{
    return m_initnum;
}
int CFwsThreadPool::get_idelnum()
{
    return m_idle_list.size();
}
int CFwsThreadPool::get_busynum()
{
    return m_busy_list.size();
}
void CFwsThreadPool::terminate_all()
{
    if(0 != pthread_mutex_lock(&m_mutex_idle))
    {
        printf("lock fail\n");
        return ;
    }

    printf("start to end threadpool\n");
    vector<CWorkThread *>::iterator it = m_idle_list.begin();
    while( it != m_idle_list.end() )
    {
        if ( (*it)->join() )
            printf("end thread：%lx\n", (*it)->get_tid());
        else
            printf("end thread fail：%lx\n", (*it)->get_tid());

        it++;
    }

    if(0 != pthread_mutex_unlock(&m_mutex_idle))
    {
        printf("unlock fail\n");
    }
}

// 获取一个空闲线程
CWorkThread *CFwsThreadPool::get_idle_thread()
{
    CWorkThread *ret = NULL;

    if(0 != pthread_mutex_lock(&m_mutex_idle))
    {
        printf("lock fail\n");
        return NULL;
    }

    vector<CWorkThread *>::iterator it = m_idle_list.begin();
    while( it != m_idle_list.end() )
    {
        if ( (*it)->get_state() == E_THREAD_STATE_IDEL )
        {
            // 设置忙碌状态
            (*it)->set_state(E_THREAD_STATE_BUSY);
            ret = (*it);
            break;
        }
        it++;
    }

    if(0 != pthread_mutex_unlock(&m_mutex_idle))
    {
        printf("unlock fail\n");
    }

    return ret;
}

// 无空闲线程返回失败
int CFwsThreadPool::run(CJob* job, void *jobdata)
{
    CWorkThread *idlethr = get_idle_thread();
    if ( idlethr == NULL )
    {
        return -1;
    }
    idlethr->set_job(job, jobdata);
    idlethr->weakup();

    return 0;
}

