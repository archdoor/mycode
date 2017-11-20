#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "job.h"
#include "threadpool.h"

using namespace std;

class CXJob: public CJob
{
public:
    CXJob() {};
    ~CXJob() {};
    void run(void *jobdata)
    {
        printf("doing CXJob: %d\n", m_no);
        sleep(2);
    }

    int m_no;
};

int main()
{
    CFwsThreadPool pool(5);

    if ( pool.start() < 0 )
    {
        cout << "pool start error\n";
        return -1;
    }

    for ( int i = 0; i < 15; ++i )
    {
        CXJob *job = new CXJob();
        job->m_no = i;

        while ( pool.run(job, NULL) < 0 )
        {
            sleep(1);
        }
    }

    sleep(10);
    pool.terminate_all();

    cout << "threadpool end\n";

    return 0;
}

