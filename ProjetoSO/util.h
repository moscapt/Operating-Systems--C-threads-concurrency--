/**********************************************
*
*util.h
*
***********************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <pthread.h>
#include <stdint.h>
#include <semaphore.h>
#include <signal.h>
//dont know what signal does
//#define SOCKET_PATH "grupoSO2015_2016"

#define UNIXSTR_PATH "/tmp/socket.unix.stream"
#define UNIXDG_PATH "/tmp/socket.unix.datagram"

#define UNIXDG_TMP "/tmp/socket.unix.dgXXXXXXX"

#define MAXLINHA 512

