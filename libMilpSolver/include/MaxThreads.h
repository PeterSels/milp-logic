#ifndef MAX_THREADS_H
#define MAX_THREADS_H

#include "DoOpenMP.h"

#ifdef DO_OPEN_MP
#define MAX_N_THREADS (8)
#else
#define MAX_N_THREADS (1)
#endif

#endif // MAX_THREADS_H
