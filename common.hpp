#ifndef _COMMON_HPP
#define _COMMON_HPP

#include <pthread.h>
#include <semaphore.h>

#define PORT (28772)
#define SOCKETERROR (-1)
extern sem_t newAccess;
extern sem_t writeAccess;
extern sem_t readerAccess;
extern int readerC;



#endif
