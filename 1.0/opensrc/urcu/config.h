/* config.h.  Generated from config.h.in by configure.  */
/* config.h.in.  Generated from configure.ac by autoheader.  */

/* Use the dmb instruction is available for use on ARM. */
/* #undef CONFIG_RCU_ARM_HAVE_DMB */

/* Compatibility mode for i386 which lacks cmpxchg instruction. */
/* #undef CONFIG_RCU_COMPAT_ARCH */

/* Defined when on a system that has memory fence instructions. */
/* #undef CONFIG_RCU_HAVE_FENCE */

/* Defined when on a system with futex support. */
#define CONFIG_RCU_HAVE_FUTEX 1

/* Enable SMP support. With SMP support enabled, uniprocessors are also
   supported. With SMP support disabled, UP systems work fine, but the
   behavior of SMP systems is undefined. */
#define CONFIG_RCU_SMP 1

/* Define to 1 if you have the `bzero' function. */
#define HAVE_BZERO 1

/* Define to 1 if the system has the type `cpu_set_t'. */
#define HAVE_CPU_SET_T 1

/* Define to 1 if we have CPU_ZERO and if it works */
#define HAVE_CPU_ZERO 1

/* Define to 1 if you have the <dlfcn.h> header file. */
#define HAVE_DLFCN_H 1

/* Define to 1 if you have the `getpagesize' function. */
#define HAVE_GETPAGESIZE 1

/* Define to 1 if you have the `gettimeofday' function. */
#define HAVE_GETTIMEOFDAY 1

/* Define to 1 if you have the <inttypes.h> header file. */
#define HAVE_INTTYPES_H 1

/* Define to 1 if your system has a GNU libc compatible `malloc' function, and
   to 0 otherwise. */
#define HAVE_MALLOC 1

/* Define to 1 if you have the <memory.h> header file. */
#define HAVE_MEMORY_H 1

/* Define to 1 if you have a working `mmap' system call. */
#define HAVE_MMAP 1

/* Define to 1 if you have the `munmap' function. */
#define HAVE_MUNMAP 1

/* Define to 1 if you have the `sched_getcpu' function. */
#define HAVE_SCHED_GETCPU 1

/* Define to 1 if you have the `sched_setaffinity' function. */
#define HAVE_SCHED_SETAFFINITY 1

/* Define to 1 if you have the <stdint.h> header file. */
#define HAVE_STDINT_H 1

/* Define to 1 if you have the <stdlib.h> header file. */
#define HAVE_STDLIB_H 1

/* Define to 1 if you have the <strings.h> header file. */
#define HAVE_STRINGS_H 1

/* Define to 1 if you have the <string.h> header file. */
#define HAVE_STRING_H 1

/* Define to 1 if you have the `strtoul' function. */
#define HAVE_STRTOUL 1

/* Define to 1 if you have the `sysconf' function. */
#define HAVE_SYSCONF 1

/* Define to 1 if you have the <sys/param.h> header file. */
#define HAVE_SYS_PARAM_H 1

/* Define to 1 if you have the <sys/stat.h> header file. */
#define HAVE_SYS_STAT_H 1

/* Define to 1 if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H 1

/* Define to 1 if you have the <unistd.h> header file. */
#define HAVE_UNISTD_H 1

/* Define to the sub-directory in which libtool stores uninstalled libraries.
   */
#define LT_OBJDIR ".libs/"

/* Name of package */
#define PACKAGE "userspace-rcu"

/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT "mathieu dot desnoyers at efficios dot com"

/* Define to the full name of this package. */
#define PACKAGE_NAME "userspace-rcu"

/* Define to the full name and version of this package. */
#define PACKAGE_STRING "userspace-rcu 0.6.6"

/* Define to the one symbol short name of this package. */
#define PACKAGE_TARNAME "userspace-rcu"

/* Define to the home page for this package. */
#define PACKAGE_URL ""

/* Define to the version of this package. */
#define PACKAGE_VERSION "0.6.6"

/* Define to sched_setaffinity's number of arguments. */
#define SCHED_SETAFFINITY_ARGS 3

/* Define to 1 if you have the ANSI C header files. */
#define STDC_HEADERS 1

/* Version number of package */
#define VERSION "0.6.6"

/* Define to `__inline__' or `__inline' if that's what the C compiler
   calls it, or to nothing if 'inline' is not supported under any name.  */
#ifndef __cplusplus
/* #undef inline */
#endif

/* Define to rpl_malloc if the replacement function should be used. */
/* #undef malloc */

/* Define to `int' if <sys/types.h> does not define. */
/* #undef pid_t */

/* Define to `unsigned int' if <sys/types.h> does not define. */
/* #undef size_t */
