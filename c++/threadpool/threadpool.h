#ifndef __FWS_THREADPOOL_H__
#define __FWS_THREADPOOL_H__

#include <vector>
#include "workthread.h"
#include "job.h"
 
using std::vector;

// 线程池
class CFwsThreadPool
{
public:
    CFwsThreadPool();
    CFwsThreadPool(int initnum);
    int start();

    int run(CJob* job, void *jobdata);
    int get_initnum();
    int get_idelnum();
    int get_busynum();

    void terminate_all();

protected:
    CWorkThread *get_idle_thread();

    void CreateIdleThread(int num);

private:
    unsigned int m_initnum;

    vector<CWorkThread *> m_idle_list;
    vector<CWorkThread *> m_busy_list;

	pthread_mutex_t m_mutex_idle;
	pthread_mutex_t m_mutex_busy;
};



#endif

