#ifndef GLOBAL_H_
#define GLOBAL_H_

#define ZERO_MEM(a) memset(a, 0, sizeof(a))
#define ARRAY_SIZE_IN_ELEMENTS(a) (sizeof(a)/sizeof(a[0]))

#ifdef WIN32
#define SNPRINTF _snprintf_s
#define RANDOM rand
#define SRANDOM srand((unsigned)time(NULL))
float fmax(float a, float b);
#else
#define SNPRINTF snprintf
#define RANDOM random
#define SRANDOM srandom(getpid())
#endif

#define INVALID_UNIFORM_LOCATION 0xffffffff
#define INVALID_OGL_VALUE 0xffffffff

#endif