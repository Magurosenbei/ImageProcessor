#ifndef TOOLS_H
#define TOOLS_H

inline float FastSqrt_5(const float x)
{
	union {int i;float x;}u;
	u.x = x;
	u.i = (1<<29) + (u.i >> 1) - (1<<22);
	return u.x;
}

inline float FastSqrt_2(const float x)
{
	const float xhalf = 0.5f*x;
	union
	{float x;int i;} u;                     // get bits for floating value
	u.x = x;
	u.i = SQRT_MAGIC_F - (u.i >> 1);        // gives initial guess y0
	return (u.x*(1.5f - xhalf*u.x*u.x)) * x;// Newton step, repeating increases accuracy
}

inline void *  General_aligned_malloc(size_t bytes, size_t alignment)
{
    void *p1 ,*p2; // basic pointer needed for computation.

    if((p1 =(void *) malloc(bytes + alignment + sizeof(size_t)))==NULL)
    return NULL;

    size_t addr=(size_t)p1+alignment+sizeof(size_t);

    p2 = (void *)(addr - (addr%alignment));

    *((size_t *)p2-1)=(size_t)p1;

    return p2;
}

inline void General_aligned_free(void * p)
{
    free((void *)(*((size_t *) p-1)));
}

// High Resolution Timer UNIX ONLY
class System_Timer
{
    timeval	m_start, m_end;
public:
    inline void    Start()
    {
    	gettimeofday(&m_start, NULL);
	}

    inline double  End()
    {
    	gettimeofday(&m_end, NULL);
    	return  (m_end.tv_sec - m_start.tv_sec) * 1000.0        // sec to milis
            + (m_end.tv_usec - m_start.tv_usec) / 1000.0;       // micro to milis
    }
};


#endif
