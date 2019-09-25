/*
 * newlib_stubs.c
 *
 *  Created on: 2 Nov 2010
 *      Author: nanoage.co.uk
 */

#include <errno.h>
#include <sys/stat.h>
#include <sys/times.h>
#include <sys/types.h>
#include <sys/unistd.h>

#include "stm32f10x_usart.h"

#ifndef STDOUT_USART
#  define STDOUT_USART 1
#endif

#ifndef STDERR_USART
#  define STDERR_USART 1
#endif

#ifndef STDIN_USART
#  define STDIN_USART 1
#endif

#undef errno
extern int errno;

/*
 * environ
 * A pointer to a list of environment variables and their values.
 * For a minimal environment, this empty list is adequate:
 */
char *__env[1] = {0};
char **environ = __env;

int _write(int file, char *ptr, int len);

void _exit(int status) {
  _write(1, (char *)"exit", 4);

  while (1) {

    asm("nop");
  }
}

int _close(int file) { return -1; }

int _execve(char *name, char **argv, char **env)
/*
 * execve
 * Transfer control to a new process. Minimal implementation (for a system without processes):
 */

{
  errno = ENOMEM;
  return -1;
}

int _fork()
/*
 * fork
 * Create a new process. Minimal implementation (for a system without processes):
 */

{
  errno = EAGAIN;
  return -1;
}

int _fstat(int file, struct stat *st)
/*
 * fstat
 * Status of an open file. For consistency with other minimal implementations in these examples,
 * all files are regarded as character special devices.
 * The `sys/stat.h' header file required is distributed in the `include' subdirectory for this C library.
 */

{
  st->st_mode = S_IFCHR;
  return 0;
}

int _getpid()
/*
 * getpid
 * Process-ID; this is sometimes used to generate strings unlikely to conflict with other processes. Minimal
 * implementation, for a system without processes:
 */

{
  return 1;
}

int _isatty(int file)
/*
 * isatty
 * Query whether output stream is a terminal. For consistency with the other minimal implementations,
 */

{
  switch (file) {
  case STDOUT_FILENO:
  case STDERR_FILENO:
  case STDIN_FILENO:
    return 1;
  default:
    // errno = ENOTTY;
    errno = EBADF;
    return 0;
  }
}

int _kill(int pid, int sig)
/*
 * kill
 * Send a signal. Minimal implementation:
 */

{
  errno = EINVAL;
  return (-1);
}

int _link(char *old, char *nw)
/*
 * link
 * Establish a new name for an existing file. Minimal implementation:
 */

{
  errno = EMLINK;
  return -1;
}

int _lseek(int file, int ptr, int dir)
/*
 * lseek
 * Set position in a file. Minimal implementation:
 */

{
  return 0;
}

caddr_t _sbrk(int incr)
/*
 * sbrk
 * Increase program data space.
 * Malloc and related functions depend on this
 */

{
  extern char _ebss; // Defined by the linker
  static char *heap_end;
  char *prev_heap_end;

  if (heap_end == 0) {

    heap_end = &_ebss;
  }

  prev_heap_end = heap_end;

  char *stack = (char *)(size_t)__get_MSP();

  if (heap_end + incr > stack) {

    _write(STDERR_FILENO, (char *)"Heap and stack collision\n", 25);
    errno = ENOMEM;
    return (caddr_t)-1;
    // abort ();
  }

  heap_end += incr;
  return (caddr_t)prev_heap_end;
}

int _read(int file, char *ptr, int len)
/*
 * read
 * Read a character to a file. `libc' subroutines will use this system routine for input from all files, including stdin
 * Returns -1 on error or blocks until the number of characters have been read.
 */

{
  int n;
  int num = 0;
  switch (file) {
  case STDIN_FILENO: {
    for (n = 0; n < len; n++) {

#if STDIN_USART == 1

      while ((USART_GetFlagStatus(USART1, USART_FLAG_RXNE)) == (uint16_t)RESET)
        ;

      char c = (char)(USART_ReceiveData(USART1) & (uint16_t)0x01FF);

      if (c == '\r') {

        char new_line_char = '\n';
        _write(STDOUT_FILENO, &c, 1);
        _write(STDOUT_FILENO, &new_line_char, 1);
        *ptr++ = '\r';
        *ptr++ = '\n';
        num++;
        break;

      } else {

        _write(STDOUT_FILENO, &c, 1);
        *ptr++ = c;
        num++;
      }
#endif
    }

    break;
  }
  default:
    errno = EBADF;
    return -1;
  }
  return num;
}

int _stat(const char *filepath, struct stat *st)
/*
 * stat
 * Status of a file (by name). Minimal implementation:
 * int    _EXFUN(stat,( const char *__path, struct stat *__sbuf ));
 */

{
  st->st_mode = S_IFCHR;
  return 0;
}

clock_t _times(struct tms *buf)
/*
 * times
 * Timing information for current process. Minimal implementation:
 */

{
  return -1;
}

int _unlink(char *name)
/*
 * unlink
 * Remove a file's directory entry. Minimal implementation:
 */

{
  errno = ENOENT;
  return -1;
}

int _wait(int *status)
/*
 * wait
 * Wait for a child process. Minimal implementation:
 */

{
  errno = ECHILD;
  return -1;
}

int _write(int file, char *ptr, int len)
/*
 * write
 * Write a character to a file. `libc' subroutines will use this system routine for output to all files, including
 * stdout Returns -1 on error or number of bytes sent
 */

{
  int n;
  switch (file) {
  case STDOUT_FILENO: /* stdout */

    for (n = 0; n < len; n++) {

#if STDOUT_USART == 1

      while ((USART_GetFlagStatus(USART1, USART_FLAG_TC)) == (uint16_t)RESET)
        ;

      USART_SendData(USART1, *ptr++ & (uint16_t)0x01FF);

#endif
    }
    break;

  case STDERR_FILENO: /* stderr */

    for (n = 0; n < len; n++) {

#if STDERR_USART == 1

      while ((USART_GetFlagStatus(USART1, USART_FLAG_TC)) == (uint16_t)RESET)
        ;

      USART_SendData(USART1, *ptr++ & (uint16_t)0x01FF);

#endif
    }
    break;

  default:

    errno = EBADF;
    return -1;
  }

  return len;
}
