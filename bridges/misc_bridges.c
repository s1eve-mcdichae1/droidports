#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include <stdio.h>
#include <locale.h>
#include <signal.h>
#include <setjmp.h>
#include <malloc.h>
#include <sys/time.h>

#include "platform.h"
#include "so_util.h"
#include "gles2.h"
#include "gles2_bridge.h"

#ifndef PLATFORM_VITA
extern void *__gnu_Unwind_Find_exidx;
#else
void *__gnu_Unwind_Find_exidx(void *pc, int *pcount) { return 0; }
#endif
extern void *_ZdaPv;
extern void *_ZdlPv;
extern void *_Znaj;
extern void *_Znwj;

extern void *__assert2;
extern void *__cxa_atexit;
extern void *__cxa_finalize;
extern void *__cxa_pure_virtual;
extern void *__cxa_end_catch;
extern void *__cxa_allocate_exception;
extern void *__cxa_throw;
extern void *__cxa_rethrow;
extern void *__cxa_free_exception;
extern void *_ZTVN10__cxxabiv117__class_type_infoE;
extern void *_ZTVN10__cxxabiv120__si_class_type_infoE;

extern void *__stack_chk_fail;
extern void *__stack_chk_guard;

static int __stack_chk_guard_fake = 0xD2424242;

ABI_ATTR static void aeabi_memclr_impl(void *dst, size_t len)
{
    memset(dst, 0, len);
}

ABI_ATTR static void aeabi_memset_impl(void *s, size_t n, int c)
{
    memset(s, c, n);
}

ABI_ATTR static int impl__android_log_print(int prio, const char *tag, const char *fmt, ...)
{
    char andlog[2048] = {};
    va_list va;
    va_start(va, fmt);
    warning("LOG[%s]: ", tag);
    vsnprintf(&andlog, 2047, fmt, va);
    warning("%s", andlog);
    va_end(va);
}

ABI_ATTR static int impl__android_log_vprint(int prio, const char *tag, const char *fmt, va_list va)
{
    char andlog[2048] = {};
    warning("LOG[%s]: ", tag);
    vsnprintf(&andlog, 2047, fmt, va);
    warning("%s", andlog);
}

ABI_ATTR static int ret0(void)
{
    return 0;
}

ABI_ATTR static int ret1(void)
{
    return 1;
}

ABI_ATTR static int atexit_fake(void (*__func)(void))
{
    ;
}

ABI_ATTR static void *dlsym_fake(void *handle, const char *name)
{
    //if starts with gl, or xgl (such as egl or vgl)
    if ((strncmp(name, "gl", 2) == 0) || (name && strncmp(name+1, "gl", 2) == 0)) {
        void *funct = (void*)resolve_gl_symbol(name);
        if (funct)
            return funct;
    }
    
    return NULL;
}

ABI_ATTR static void *dlopen_fake(const char *filename, int flag)
{
    //TODO:: Add proper dlopen mechanism
    return (void*)0xAAAAAAAA;
}

#ifndef __time64_t
typedef int64_t __time64_t;
#endif

#ifdef PLATFORM_VITA
#ifndef __time_t
typedef int32_t __time_t;
#endif
#endif

struct tm *__localtime64 (const __time64_t *time)
{
    time_t _t = *time;
    return localtime(&_t);
}

typedef int64_t Year;
typedef __time64_t Time64_T;
struct tm *gmtime64_impl(const __time64_t *time)
{
    __time_t _t = *time;
    return gmtime(&_t);
}

struct TM
{
  int tm_sec;			/* Seconds.	[0-60] (1 leap second) */
  int tm_min;			/* Minutes.	[0-59] */
  int tm_hour;			/* Hours.	[0-23] */
  int tm_mday;			/* Day.		[1-31] */
  int tm_mon;			/* Month.	[0-11] */
  int tm_year;			/* Year	- 1900.  */
  int tm_wday;			/* Day of week.	[0-6] */
  int tm_yday;			/* Days in year.[0-365]	*/
  int tm_isdst;			/* DST.		[-1/0/1]*/

  long int __tm_gmtoff;		/* Seconds east of UTC.  */
  const char *__tm_zone;	/* Timezone abbreviation.  */
};

static const int length_of_year[2] = { 365, 366 };
/* Some numbers relating to the gregorian cycle */
static const Year     years_in_gregorian_cycle   = 400;
#define               days_in_gregorian_cycle      ((365 * 400) + 100 - 4 + 1)
static const Time64_T seconds_in_gregorian_cycle = days_in_gregorian_cycle * 60LL * 60LL * 24LL;
#define IS_LEAP(n)      ((!(((n) + 1900) % 400) || (!(((n) + 1900) % 4) && (((n) + 1900) % 100))) != 0)

static const int julian_days_by_month[2][12] = {
    {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334},
    {0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335},
};

Time64_T timegm64_impl(const struct TM *date) {
    Time64_T days    = 0;
    Time64_T seconds = 0;
    Year     year;
    Year     orig_year = (Year)date->tm_year;
    int      cycles  = 0;
    if( orig_year > 100 ) {
        cycles = (orig_year - 100) / 400;
        orig_year -= cycles * 400;
        days      += (Time64_T)cycles * days_in_gregorian_cycle;
    }
    else if( orig_year < -300 ) {
        cycles = (orig_year - 100) / 400;
        orig_year -= cycles * 400;
        days      += (Time64_T)cycles * days_in_gregorian_cycle;
    }

    if( orig_year > 70 ) {
        year = 70;
        while( year < orig_year ) {
            days += length_of_year[IS_LEAP(year)];
            year++;
        }
    }
    else if ( orig_year < 70 ) {
        year = 69;
        do {
            days -= length_of_year[IS_LEAP(year)];
            year--;
        } while( year >= orig_year );
    }
    days += julian_days_by_month[IS_LEAP(orig_year)][date->tm_mon];
    days += date->tm_mday - 1;
    seconds = days * 60 * 60 * 24;
    seconds += date->tm_hour * 60 * 60;
    seconds += date->tm_min * 60;
    seconds += date->tm_sec;
    return(seconds);
}

#ifdef PLATFORM_VITA
int clock_gettime_impl(clockid_t _unused, struct timespec *tp)
{
    struct timeval tv;
    int ret = gettimeofday(&tv, NULL);
    if (ret == 0)
        TIMEVAL_TO_TIMESPEC(&tv, tp);

    return ret;
}

int nanosleep(const struct timespec *req, struct timespec *rem) {
  // no way to implement this as far as I can tell
  rem->tv_sec = 0;
  rem->tv_nsec = 0;
  const uint32_t usec = req->tv_sec * 1000000 + req->tv_nsec / 1000;
  return sceKernelDelayThreadCB(usec);
}
#endif

int usleep(long usec)
{
    struct timespec ts = {
        .tv_sec = (long int) (usec / 1000000),
        .tv_nsec = (long int) (usec % 1000000) * 1000ul 
    };

    return nanosleep(&ts, NULL);
}

DynLibFunction symtable_misc[] = {
    {"_ZdaPv", (uintptr_t)&_ZdaPv},
    {"_ZdlPv", (uintptr_t)&_ZdlPv},
    {"_Znaj", (uintptr_t)&_Znaj},
    {"_Znwj", (uintptr_t)&_Znwj},
    {"__gnu_Unwind_Find_exidx", (uintptr_t)&__gnu_Unwind_Find_exidx},

    {"__aeabi_atexit", (uintptr_t)&atexit_fake},
    {"__aeabi_memclr", (uintptr_t)&aeabi_memclr_impl},
    {"__aeabi_memclr4", (uintptr_t)&aeabi_memclr_impl},
    {"__aeabi_memclr8", (uintptr_t)&aeabi_memclr_impl},
    {"__aeabi_memcpy", (uintptr_t)&memcpy},
    {"__aeabi_memcpy4", (uintptr_t)&memcpy},
    {"__aeabi_memcpy8", (uintptr_t)&memcpy},
    {"__aeabi_memmove", (uintptr_t)&memmove},
    {"__aeabi_memmove4", (uintptr_t)&memmove},
    {"__aeabi_memmove8", (uintptr_t)&memmove},
    {"__aeabi_memset", (uintptr_t)&aeabi_memset_impl},
    {"__aeabi_memset4", (uintptr_t)&aeabi_memset_impl},
    {"__aeabi_memset8", (uintptr_t)&aeabi_memset_impl},

    {"__android_log_print", (uintptr_t)&impl__android_log_print},
    {"__android_log_vprint", (uintptr_t)&impl__android_log_vprint},

    {"__cxa_atexit", (uintptr_t)&__cxa_atexit},
    {"__cxa_finalize", (uintptr_t)&__cxa_finalize},
    {"__cxa_pure_virtual", (uintptr_t)&__cxa_pure_virtual},
    {"__cxa_end_catch", (uintptr_t)&__cxa_end_catch},
    {"__cxa_allocate_exception", (uintptr_t)&__cxa_allocate_exception},
    {"__cxa_throw", (uintptr_t)&__cxa_throw},
    {"__cxa_rethrow", (uintptr_t)&__cxa_rethrow},
    {"__cxa_free_exception", (uintptr_t)&__cxa_free_exception},
    {"_ZTVN10__cxxabiv117__class_type_infoE", (uintptr_t)&_ZTVN10__cxxabiv117__class_type_infoE},
    {"_ZTVN10__cxxabiv120__si_class_type_infoE", (uintptr_t)&_ZTVN10__cxxabiv120__si_class_type_infoE},

#ifdef PLATFORM_VITA
    {"__errno", (uintptr_t)&__errno},
#else
    {"__errno", (uintptr_t)&__errno_location},
#endif
    {"__stack_chk_fail", (uintptr_t)&__stack_chk_fail},
    {"__stack_chk_guard", (uintptr_t)&__stack_chk_guard_fake},

    {"sigaction", (uintptr_t)&ret0},
    {"sigemptyset", (uintptr_t)&ret0},

    {"atoi", (uintptr_t)&atoi},
    {"atol", (uintptr_t)&atol},

    {"calloc", (uintptr_t)&calloc},
    {"free", (uintptr_t)&free},
    {"malloc", (uintptr_t)&malloc},
    {"mallinfo", (uintptr_t)&mallinfo},
    {"realloc", (uintptr_t)&realloc},

    {"clock_gettime", (uintptr_t)&clock_gettime},
    {"ctime", (uintptr_t)&ctime},
    {"gettimeofday", (uintptr_t)&gettimeofday},
    {"gmtime", (uintptr_t)&gmtime},
    {"gmtime64", (uintptr_t)&gmtime64_impl},
    {"timegm64", (uintptr_t)&timegm64_impl},
    {"localtime_r", (uintptr_t)&localtime_r},
    {"localtime64", (uintptr_t)&__localtime64},
    {"time", (uintptr_t)&time},
    {"strftime", (uintptr_t)&strftime},
    {"setlocale", (uintptr_t)&setlocale},

    {"eglGetDisplay", (uintptr_t)&ret0},
    {"eglGetProcAddress", (uintptr_t)&ret0},
    {"eglQueryString", (uintptr_t)&ret0},

    {"abort", (uintptr_t)&abort},
    {"exit", (uintptr_t)&exit},
    {"raise", (uintptr_t)&raise},
    {"mktime", (uintptr_t)&mktime},
    {"remove", (uintptr_t)&remove},
    
    {"dlsym", (uintptr_t)&dlsym_fake},
    {"dlopen", (uintptr_t)&dlopen_fake},
    {"dlclose", (uintptr_t)&ret0},
    {"dlerror", (uintptr_t)&ret0},

    {"getenv", (uintptr_t)&getenv},

    {"longjmp", (uintptr_t)&longjmp},
    {"setjmp", (uintptr_t)&setjmp},

    {"memcpy", (uintptr_t)&memcpy},
    {"memmove", (uintptr_t)&memmove},
    {"memset", (uintptr_t)&memset},
    {"memchr", (uintptr_t)&memchr},
    {"memcmp", (uintptr_t)&memcmp},

    {"puts", (uintptr_t)&puts},
    {"qsort", (uintptr_t)&qsort},
    {"rand", (uintptr_t)&rand},
    {"srand", (uintptr_t)&srand},
    {"nanosleep", (uintptr_t)&nanosleep},
    {"usleep", (uintptr_t)&usleep},

    {NULL, (uintptr_t)NULL}
};