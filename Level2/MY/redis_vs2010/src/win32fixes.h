#ifndef WIN32FIXES_H
#define WIN32FIXES_H

#ifdef WIN32
#ifndef _WIN32
#define _WIN32
#endif
#endif

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOGDI
#define __USE_W32_SOCKETS

#include "fmacros.h"
#include <stdlib.h>
#include <stdio.h>
#include <io.h>
#include <signal.h>
#include <sys/types.h>
#include <winsock2.h>  /* setsocketopt */
#include <ws2tcpip.h>
#include <windows.h>
#include <float.h>
#include <fcntl.h>    /* _O_BINARY */
#include <limits.h>  /* INT_MAX */
#include <sys/types.h>

/* Misc */
#ifdef __STRICT_ANSI__
#define _exit exit
#define fileno(__F) ((__F)->_file)

#define strcasecmp lstrcmpiA

#define fseeko(stream, offset, origin) fseek(stream, offset, origin)
#define ftello(stream) ftell(stream)
#else
#define fseeko(stream, offset, origin) fseeko64(stream, offset, origin)
#define ftello(stream) ftello64(stream)
#endif

#define inline __inline

#define ftruncate(a,b) replace_ftruncate(a,b)
int replace_ftruncate(int fd, off_t length);

#define sleep(x) Sleep((x)*1000)

#ifndef __RTL_GENRANDOM
#define __RTL_GENRANDOM 1
typedef BOOLEAN (_stdcall* RtlGenRandomFunc)(void * RandomBuffer, ULONG RandomBufferLength);
RtlGenRandomFunc RtlGenRandom;
#endif

#define snprintf _snprintf
#define ftello64 _ftelli64
#define fseeko64 _fseeki64
#define strcasecmp _stricmp
#define strtoll _strtoi64
#define isnan _isnan
#define isfinite _finite
#define isinf(x) (!_finite(x))
/* following defined to choose little endian byte order */
#define __i386__ 1
#if !defined(va_copy)
#define va_copy(d,s)  d = (s)
#endif

typedef int ssize_t;
#define random() (long)replace_random()
#define rand() replace_random()
int replace_random();

#define mode_t long


/* Redis calls usleep(1) to give thread some time
* Sleep(0) should do the same on windows
* In other cases, usleep is called with milisec resolution,
* which can be directly translated to winapi Sleep() */
#undef usleep
#define usleep(x) (x == 1) ? Sleep(0) : Sleep((int)((x)/1000))

#define pipe(fds) _pipe(fds, 8192, _O_BINARY|_O_NOINHERIT)

/* Prcesses */
#define waitpid(pid,statusp,options) _cwait (statusp, pid, WAIT_CHILD)

#define WAIT_T int
#define WTERMSIG(x) ((x) & 0xff) /* or: SIGABRT ?? */
#define WCOREDUMP(x) 0
#define WEXITSTATUS(x) (((x) >> 8) & 0xff) /* or: (x) ?? */
#define WIFSIGNALED(x) (WTERMSIG (x) != 0) /* or: ((x) == 3) ?? */
#define WIFEXITED(x) (WTERMSIG (x) == 0) /* or: ((x) != 3) ?? */
#define WIFSTOPPED(x) 0

#define WNOHANG 1

/* file mapping */
#define PROT_READ 1
#define PROT_WRITE 2

#define MAP_FAILED   (void *) -1

#define MAP_SHARED 1
#define MAP_PRIVATE 2

#define RUSAGE_SELF     0
#define RUSAGE_CHILDREN (-1)

#ifndef _RUSAGE_T_
#define _RUSAGE_T_
struct rusage {
    struct timeval ru_utime;    /* user time used */
    struct timeval ru_stime;    /* system time used */
};
#endif

int getrusage(int who, struct rusage * rusage);

/* Signals */
#define SIGNULL  0 /* Null	Check access to pid*/
#define SIGHUP	 1 /* Hangup	Terminate; can be trapped*/
#define SIGINT	 2 /* Interrupt	Terminate; can be trapped */
#define SIGQUIT	 3 /* Quit	Terminate with core dump; can be trapped */
#define SIGTRAP  5
#define SIGBUS   7
#define SIGKILL	 9 /* Kill	Forced termination; cannot be trapped */
#define SIGPIPE 13
#define SIGALRM 14
#define SIGTERM	15 /* Terminate	Terminate; can be trapped  */
#define SIGSTOP 17
#define SIGTSTP 18
#define SIGCONT 19
#define SIGCHLD 20
#define SIGTTIN 21
#define SIGTTOU 22
#define SIGABRT 22
/* #define SIGSTOP	24 Pause the process; cannot be trapped*/
/*#define SIGTSTP	25   Terminal stop	Pause the process; can be trapped*/
/*#define SIGCONT	26 */
#define SIGWINCH 28
#define SIGUSR1  30
#define SIGUSR2  31

#define ucontext_t void*

#define SA_NOCLDSTOP    0x00000001u
#define SA_NOCLDWAIT    0x00000002u
#define SA_SIGINFO      0x00000004u
#define SA_ONSTACK      0x08000000u
#define SA_RESTART      0x10000000u
#define SA_NODEFER      0x40000000u
#define SA_RESETHAND    0x80000000u
#define SA_NOMASK       SA_NODEFER
#define SA_ONESHOT      SA_RESETHAND
#define SA_RESTORER     0x04000000

#ifndef _SIGSET_T_
#define _SIGSET_T_
typedef unsigned long sigset_t;
#endif /* _SIGSET_T_ */

#define sigemptyset(pset)    (*(pset) = 0)
#define sigfillset(pset)     (*(pset) = (unsigned int)-1)
#define sigaddset(pset, num) (*(pset) |= (1L<<(num)))
#define sigdelset(pset, num) (*(pset) &= ~(1L<<(num)))
#define sigismember(pset, num) (*(pset) & (1L<<(num)))

#ifndef SIG_SETMASK
#define SIG_SETMASK (0)
#define SIG_BLOCK   (1)
#define SIG_UNBLOCK (2)
#endif /*SIG_SETMASK*/

typedef	void (*__p_sig_fn_t)(int);
typedef int pid_t;

struct sigaction {
    int          sa_flags;
    sigset_t     sa_mask;
    __p_sig_fn_t sa_handler;
    __p_sig_fn_t sa_sigaction;
};

int sigaction(int sig, struct sigaction *in, struct sigaction *out);

  /* Socekts */

#define setsockopt(a,b,c,d,e) replace_setsockopt(a,b,c,d,e)

int replace_setsockopt(int socket, int level, int optname,
                    const void *optval, socklen_t optlen);

#define rename(a,b) replace_rename(a,b)
int replace_rename(const char *src, const char *dest);

int pthread_sigmask(int how, const sigset_t *set, sigset_t *oset);


/* Misc Unix -> Win32 */
int kill(pid_t pid, int sig);
int fsync (int fd);
pid_t wait3(int *stat_loc, int options, void *rusage);

int w32initWinSock(void);

/* redis-check-dump  */
void *mmap(void *start, size_t length, int prot, int flags, int fd, off_t offset);
int munmap(void *start, size_t length);

int fork(void);

  /* strtod does not handle Inf and Nan
   We need to do the check before calling strtod */
#undef strtod
#define strtod(nptr, eptr) wstrtod((nptr), (eptr))

double wstrtod(const char *nptr, char **eptr);

int gettimeofday(struct timeval *tv, struct timezone *tz);

/* loglevel */
#define LOG_DEBUG  1
#define LOG_INFO   2
#define LOG_NOTICE 3
#define LOG_WARNING 4
#define LOG_LOCAL0 5
#define LOG_PID    6
#define LOG_NDELAY 7
#define LOG_NOWAIT 8
#define LOG_LOCAL1 9
#define LOG_LOCAL2 10
#define LOG_LOCAL3 11
#define LOG_LOCAL4 12
#define LOG_LOCAL5 13
#define LOG_LOCAL6 14
#define LOG_LOCAL7 15
#define LOG_USER   16

/* sha1 */
typedef unsigned __int32 u_int32_t;

#endif /* WIN32 */
#endif /* WIN32FIXES_H */
