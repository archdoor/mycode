#ifndef __FWS_JOB_H__
#define __FWS_JOB_H__

class CJob
{
public:
    CJob();
    virtual ~CJob();

    virtual void run ( void *ptr  ) = 0; 

private:
};


#endif


