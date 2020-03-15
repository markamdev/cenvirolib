#ifndef _LOGS_H_
#define _LOGS_H_

#ifdef DEBUG
#include <stdio.h>

#define LOG(x) printf(x)
#else
#define LOG(x)
#endif

#endif // _LOGS_H_