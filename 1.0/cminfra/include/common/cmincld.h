/* 
 *
 * Copyright  2012, 2013  Freescale Semiconductor
 *
 *
 * Licensed under the Apache License, version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at:
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 *     Unless required by applicable law or agreed to in writing, software
 *     distributed under the License is distributed on an "AS IS" BASIS,
 *     WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *     See the License for the specific language governing permissions and
 *     limitations under the License.
*/

/*
 *
 * File name: cmincld.h
 * Author: G Atmaram <B38856@freescale.com>
 * Date:   03/29/2013
 * Description: This file includes all the other header file 
 *              those are needed to build config middle-ware data model.
 * 
 */

#ifndef _UCMCMN_H_
#define _UCMCMN_H_

//#ifdef LINUX_SUPPORT
#if 1

#define COPY_FROM_USER_ERROR OF_FAILURE
#define COPY_TO_USER_ERROR   (OF_FAILURE -1)
#ifdef MEMORY_CHECK
#include "memchk.h"
#endif

#ifdef __KERNEL__

#include <linux/version.h>
#include <linux/autoconf.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/string.h>
#include <linux/slab.h>
#include <linux/skbuff.h>

#include <linux/ctype.h>
#include <asm/uaccess.h>
#include <net/ip.h>
#include <net/dst.h>
#define  printf printk

#else /* __KERNEL__ */

#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <dirent.h>
#include <assert.h>
#include <stddef.h>
#include <signal.h>
#include <setjmp.h>
#include <sys/time.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/file.h>
#include <sys/param.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <netinet/ip_icmp.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <sys/un.h>
#include <sys/poll.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#endif /* __KERNEL__ */
#include <string.h>
#include <malloc.h>
#include "oftype.h"
#include "cmtype.h"
#include "cmdefs.h"
#include "cmatmic.h"
#include "cmmath.h"
#include "cmerror.h"
#include "cmoslck.h"
#include "cmsock.h"
#include "lxoswrp.h"
#include "cmnet.h"
#include "cmsll.h"
#include "cmmempol.h"
#include "cmdll.h"
#include "mbufutil.h"
#include "cmoslck.h"
#include "cmdll.h"
//#include "cmsll.h"
//#include "cmsock.h"
#endif
#endif /* _UCMCMN_H_ */
