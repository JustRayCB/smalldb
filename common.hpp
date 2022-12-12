#ifndef _COMMON_HPP
#define _COMMON_HPP

#include <pthread.h>

extern pthread_mutex_t newAccess;
extern pthread_mutex_t writeAccess;
extern pthread_mutex_t readerAccess;
extern int readerC;


#endif
