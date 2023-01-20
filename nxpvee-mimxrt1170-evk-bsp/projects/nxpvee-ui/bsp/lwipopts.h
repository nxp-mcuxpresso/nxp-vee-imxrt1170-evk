/**
 ******************************************************************************
 * @file    lwipopts.h
 * This file is based on \src\include\lwip\opt.h
 ******************************************************************************
 * Copyright (c) 2013-2016, Freescale Semiconductor, Inc.
 * Copyright 2016-2018, 2022 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef __LWIPOPTS_H__
#define __LWIPOPTS_H__

#include "fsl_device_registers.h"

#ifndef USE_RTOS
#ifdef SDK_OS_FREE_RTOS
#define USE_RTOS 1
#else
#define USE_RTOS 0
#endif /* SDK_OS_FREE_RTOS */
#endif /* USE_RTOS */
#if USE_RTOS
/**
 * SYS_LIGHTWEIGHT_PROT==1: if you want inter-task protection for certain
 * critical regions during buffer allocation, deallocation and memory
 * allocation and deallocation.
 */
#define SYS_LIGHTWEIGHT_PROT 1

/**
 * NO_SYS==0: Use RTOS
 */
#define NO_SYS 0
/**
 * LWIP_NETCONN==1: Enable Netconn API (require to use api_lib.c)
 */
#define LWIP_NETCONN 1
/**
 * LWIP_SOCKET==1: Enable Socket API (require to use sockets.c)
 */
#define LWIP_SOCKET 1

/**
 * LWIP_SO_RCVTIMEO==1: Enable receive timeout for sockets/netconns and
 * SO_RCVTIMEO processing.
 */
#define LWIP_SO_RCVTIMEO 1

#else
/**
 * NO_SYS==1: Bare metal lwIP
 */
#define NO_SYS       1
/**
 * LWIP_NETCONN==0: Disable Netconn API (require to use api_lib.c)
 */
#define LWIP_NETCONN 0

#endif/**
 * LWIP_SOCKET==0: Disable Socket API (require to use sockets.c)
 */
#define LWIP_NETIF_HOSTNAME             1

/*
   ------------------------------------
   ---------- LOOPIF options ----------
   ------------------------------------
*/
/**
 * LWIP_NETIF_LOOPBACK==1: Support sending packets with a destination IP
 * address equal to the netif IP address, looping them back up the stack.
 */
#define LWIP_NETIF_LOOPBACK             1//mandatory
#define LWIP_ALLOW_MEM_FREE_FROM_OTHER_CONTEXT 1

/* ---------- Core locking ---------- */
/*
#define LWIP_TCPIP_CORE_LOCKING 1

void sys_lock_tcpip_core(void);
#define LOCK_TCPIP_CORE() sys_lock_tcpip_core()

void sys_unlock_tcpip_core(void);
#define UNLOCK_TCPIP_CORE() sys_unlock_tcpip_core()

void sys_check_core_locking(void);
#define LWIP_ASSERT_CORE_LOCKED() sys_check_core_locking()

void sys_mark_tcpip_thread(void);
#define LWIP_MARK_TCPIP_THREAD() sys_mark_tcpip_thread()*/

/* ---------- Memory options ---------- */
/**
 * MEM_ALIGNMENT: should be set to the alignment of the CPU
 *    4 byte alignment -> #define MEM_ALIGNMENT 4
 *    2 byte alignment -> #define MEM_ALIGNMENT 2
 */
#ifndef MEM_ALIGNMENT
#define MEM_ALIGNMENT 4
#endif

/**
 * MEM_SIZE: the size of the heap memory. If the application will send
 * a lot of data that needs to be copied, this should be set high.
 */
#ifndef MEM_SIZE
#define MEM_SIZE (22 * 1024)
#endif

/* MEMP_NUM_PBUF: the number of memp struct pbufs. If the application
   sends a lot of data out of ROM (or other static memory), this
   should be set high. */
#ifndef MEMP_NUM_PBUF
#define MEMP_NUM_PBUF 15
#endif
/* MEMP_NUM_UDP_PCB: the number of UDP protocol control blocks. One
   per active UDP "connection". */
#ifndef MEMP_NUM_UDP_PCB
#define MEMP_NUM_UDP_PCB 6
#endif
/* MEMP_NUM_TCP_PCB: the number of simulatenously active TCP
   connections. */
#ifndef MEMP_NUM_TCP_PCB
#define MEMP_NUM_TCP_PCB 10
#endif
/* MEMP_NUM_TCP_PCB_LISTEN: the number of listening TCP
   connections. */
#ifndef MEMP_NUM_TCP_PCB_LISTEN
#define MEMP_NUM_TCP_PCB_LISTEN 6
#endif
/* MEMP_NUM_TCP_SEG: the number of simultaneously queued TCP
   segments. */
#ifndef MEMP_NUM_TCP_SEG
#define MEMP_NUM_TCP_SEG 22
#endif
/* MEMP_NUM_SYS_TIMEOUT: the number of simulateously active
   timeouts. */
#ifndef MEMP_NUM_SYS_TIMEOUT
#define MEMP_NUM_SYS_TIMEOUT 10
#endif
/* MEMP_NUM_REASS_DATA: The number of whole IP packets
   queued for reassembly. */
#ifndef MEMP_NUM_REASSDATA
#define MEMP_NUM_REASSDATA 2
#endif

/* ---------- Pbuf options ---------- */
/* PBUF_POOL_SIZE: the number of buffers in the pbuf pool. */
#ifndef PBUF_POOL_SIZE
#define PBUF_POOL_SIZE 5
#endif

/* ---------- Network Interfaces options ---------- */
/* LWIP_SINGLE_NETIF==1: use a single netif only. This is the common case for
 * small real-life targets. Some code like routing etc. can be left out.
 */
#ifndef LWIP_SINGLE_NETIF
#define LWIP_SINGLE_NETIF 0
#endif

/* ---------- IP options ---------- */
/**
 * LWIP_IPV4==1: Enable IPv4
 */
#ifndef LWIP_IPV4
#define LWIP_IPV4 1
#endif
/**
 * IP_FORWARD==1: Enables the ability to forward IP packets across network
 * interfaces. If you are going to run lwIP on a device with only one network
 * interface, define this to 0.
 */
#ifndef IP_FORWARD
#define IP_FORWARD 0
#endif
/**
 * IP_REASSEMBLY==1: Reassemble incoming fragmented IP packets. Note that
 * this option does not affect outgoing packet sizes, which can be controlled
 * via IP_FRAG.
 */
#ifndef IP_REASSEMBLY
#define IP_REASSEMBLY 1
#endif
/**
 * IP_FRAG==1: Fragment outgoing IP packets if their size exceeds MTU. Note
 * that this option does not affect incoming packet sizes, which can be
 * controlled via IP_REASSEMBLY.
 */
#ifndef IP_FRAG
#define IP_FRAG 1
#endif
/**
 * IP_DEFAULT_TTL: Default value for Time-To-Live used by transport layers.
 */
#ifndef IP_DEFAULT_TTL
#define IP_DEFAULT_TTL 255
#endif

/* ---------- ARP options ---------- */
/**
 * LWIP_ARP==1: Enable ARP functionality.
 */
#ifndef LWIP_ARP
#define LWIP_ARP 1
#endif
/* ---------- TCP options ---------- */
#ifndef LWIP_TCP
#define LWIP_TCP 1
#endif
#ifndef TCP_TTL
#define TCP_TTL IP_DEFAULT_TTL
#endif

/* Controls if TCP should queue segments that arrive out of
   order. Define to 0 if your device is low on memory. */
#ifndef TCP_QUEUE_OOSEQ
#define TCP_QUEUE_OOSEQ 1
#endif


/* TCP Maximum segment size. */
#ifndef TCP_MSS
#define TCP_MSS (1500 - 40) /* TCP_MSS = (Ethernet MTU - IP header size - TCP header size) */
#endif


/* TCP sender buffer space (bytes). */
/*
#ifndef TCP_SND_BUF
#define TCP_SND_BUF (6 * TCP_MSS) // 2
#endif
*/

/* TCP sender buffer space (pbufs). This must be at least = 2 *
   TCP_SND_BUF/TCP_MSS for things to work. */
#ifndef TCP_SND_QUEUELEN
#define TCP_SND_QUEUELEN (3 * TCP_SND_BUF) / TCP_MSS // 6
#endif

/* TCP receive window. */
#ifndef TCP_WND
#define TCP_WND (2 * TCP_MSS)
#endif

#define LWIP_NETIF_API 1

/* ---------- ICMP options ---------- */
#ifndef LWIP_ICMP
#define LWIP_ICMP 1
#endif
#ifndef ICMP_TTL
#define ICMP_TTL IP_DEFAULT_TTL
#endif

#define LWIP_IGMP 1

/* ---------- RAW options ---------- */
/**
 * LWIP_RAW==1: Enable application layer to hook into the IP layer itself.
 */
#ifndef LWIP_RAW
#define LWIP_RAW 0
#endif
#ifndef RAW_TTL
#define RAW_TTL IP_DEFAULT_TTL
#endif
/**
 * LWIP_BROADCAST_PING==1: respond to broadcast pings (default is unicast only)
 */
#ifndef LWIP_BROADCAST_PING
#define LWIP_BROADCAST_PING 1
#endif
/**
 * LWIP_MULTICAST_PING==1: respond to multicast pings (default is unicast only)
 */
#ifndef LWIP_MULTICAST_PING
#define LWIP_MULTICAST_PING 1
#endif

/**
 * LWIP_MULTICAST_TX_OPTIONS==1: respond to multicast pings (default is unicast only)
 */
#ifndef LWIP_MULTICAST_TX_OPTIONS
#define LWIP_MULTICAST_TX_OPTIONS 1
#endif
/* ---------- DHCP options ---------- */
/* Enable DHCP module. */
#ifndef LWIP_DHCP
#define LWIP_DHCP 1
#endif

/* ---------- UDP options ---------- */
#ifndef LWIP_UDP
#define LWIP_UDP 1
#endif
#ifndef UDP_TTL
#define UDP_TTL IP_DEFAULT_TTL
#endif

/* ---------- Statistics options ---------- */
#ifndef LWIP_STATS
#define LWIP_STATS 0
#endif
/* ---------- ERRNO options ---------- */
/** LWIP_PROVIDE_ERRNO==1: Let lwIP provide ERRNO values and the \'errno\' variable.
 * If this is disabled, cc.h must either define \'errno\', include <errno.h>,
 * define LWIP_ERRNO_STDINCLUDE to get <errno.h> included or
 * define LWIP_ERRNO_INCLUDE to <errno.h> or equivalent.
 */
#define LWIP_PROVIDE_ERRNO 1

/*
Some MCU allow computing and verifying the IP, UDP, TCP and ICMP checksums by hardware:
 - To use this feature let the following define uncommented.
 - To disable it and process by CPU comment the  the checksum.
*/
//#define CHECKSUM_BY_HARDWARE

#ifdef CHECKSUM_BY_HARDWARE
/* CHECKSUM_GEN_IP==0: Generate checksums by hardware for outgoing IP packets.*/
#define CHECKSUM_GEN_IP 0
/* CHECKSUM_GEN_UDP==0: Generate checksums by hardware for outgoing UDP packets.*/
#define CHECKSUM_GEN_UDP 0
/* CHECKSUM_GEN_TCP==0: Generate checksums by hardware for outgoing TCP packets.*/
#define CHECKSUM_GEN_TCP 0
/* CHECKSUM_CHECK_IP==0: Check checksums by hardware for incoming IP packets.*/
#define CHECKSUM_CHECK_IP 0
/* CHECKSUM_CHECK_UDP==0: Check checksums by hardware for incoming UDP packets.*/
#define CHECKSUM_CHECK_UDP 0
/* CHECKSUM_CHECK_TCP==0: Check checksums by hardware for incoming TCP packets.*/
#define CHECKSUM_CHECK_TCP 0
#else
/* CHECKSUM_GEN_IP==1: Generate checksums in software for outgoing IP packets.*/
#define CHECKSUM_GEN_IP    1
/* CHECKSUM_GEN_UDP==1: Generate checksums in software for outgoing UDP packets.*/
#define CHECKSUM_GEN_UDP   1
/* CHECKSUM_GEN_TCP==1: Generate checksums in software for outgoing TCP packets.*/
#define CHECKSUM_GEN_TCP   1
/* CHECKSUM_CHECK_IP==1: Check checksums in software for incoming IP packets.*/
#define CHECKSUM_CHECK_IP  1
/* CHECKSUM_CHECK_UDP==1: Check checksums in software for incoming UDP packets.*/
#define CHECKSUM_CHECK_UDP 1
/* CHECKSUM_CHECK_TCP==1: Check checksums in software for incoming TCP packets.*/
#define CHECKSUM_CHECK_TCP 1
#endif
/**
 * DEFAULT_THREAD_STACKSIZE: The stack size used by any other lwIP thread.
 * The stack size value itself is platform-dependent, but is passed to
 * sys_thread_new() when the thread is created.
 */
#ifndef DEFAULT_THREAD_STACKSIZE
#define DEFAULT_THREAD_STACKSIZE 3000
#endif

/**
 * DEFAULT_THREAD_PRIO: The priority assigned to any other lwIP thread.
 * The priority value itself is platform-dependent, but is passed to
 * sys_thread_new() when the thread is created.
 */
#ifndef DEFAULT_THREAD_PRIO
#define DEFAULT_THREAD_PRIO 3
#endif

/* ---------- IPv6 options ---------- */
/**
 * LWIP_IPV6==1: Enable IPv6
 */
#ifndef LWIP_IPV6
#define LWIP_IPV6 0
#endif
/**
 * LWIP_IPV6_NUM_ADDRESSES: Number of IPv6 addresses per netif.
 */
#ifndef LWIP_IPV6_NUM_ADDRESSES
#define LWIP_IPV6_NUM_ADDRESSES 3
#endif
/**
 * LWIP_IPV6_AUTOCONFIG==1: Enable stateless address autoconfiguration as per RFC 4862.
 */
#ifndef LWIP_IPV6_AUTOCONFIG
#define LWIP_IPV6_AUTOCONFIG 0
#endif
/**
 * LWIP_IPV6_DHCP6==1: enable DHCPv6 stateful/stateless address autoconfiguration.
 */
#ifndef LWIP_IPV6_DHCP6
#define LWIP_IPV6_DHCP6 0
#endif
/**
 * LWIP_IPV6_DHCP6_STATELESS==1: enable DHCPv6 stateless address autoconfiguration.
 */
#ifndef LWIP_IPV6_DHCP6_STATELESS
#define LWIP_IPV6_DHCP6_STATELESS LWIP_IPV6_DHCP6
#endif
/* ---------- Debugging options ---------- */
#define LWIP_DEBUG
// #define PBUF_DEBUG                 LWIP_DBG_ON
// #define MEM_DEBUG                  LWIP_DBG_ON
// #define PPP_DEBUG                  LWIP_DBG_ON
// #define MEM_DEBUG                  LWIP_DBG_ON
// #define MEMP_DEBUG                 LWIP_DBG_ON
// #define PBUF_DEBUG                 LWIP_DBG_ON
// #define API_LIB_DEBUG              LWIP_DBG_ON
// #define API_MSG_DEBUG              LWIP_DBG_ON
// #define TCPIP_DEBUG                LWIP_DBG_ON
// #define NETIF_DEBUG                LWIP_DBG_ON
// #define SOCKETS_DEBUG              LWIP_DBG_OFF
// #define DNS_DEBUG                  LWIP_DBG_ON
// #define AUTOIP_DEBUG               LWIP_DBG_ON
// #define DHCP_DEBUG                 LWIP_DBG_ON
// #define IP_DEBUG                   LWIP_DBG_ON
// #define IP_REASS_DEBUG             LWIP_DBG_ON
// #define ICMP_DEBUG                 LWIP_DBG_ON
// #define IGMP_DEBUG                 LWIP_DBG_ON
// #define UDP_DEBUG                  LWIP_DBG_ON
// #define TCP_DEBUG                  LWIP_DBG_ON
// #define TCP_INPUT_DEBUG            LWIP_DBG_ON
// #define TCP_OUTPUT_DEBUG           LWIP_DBG_ON
// #define TCP_RTO_DEBUG              LWIP_DBG_ON
// #define TCP_CWND_DEBUG             LWIP_DBG_ON
// #define TCP_WND_DEBUG              LWIP_DBG_ON
// #define TCP_FR_DEBUG               LWIP_DBG_ON
// #define TCP_QLEN_DEBUG             LWIP_DBG_ON
// #define TCP_RST_DEBUG              LWIP_DBG_ON

#ifdef LWIP_DEBUG
#define U8_F  "c"
#define S8_F  "c"
#define X8_F  "02x"
#define U16_F "u"
#define S16_F "d"
#define X16_F "x"
#define U32_F "u"
#define S32_F "d"
#define X32_F "x"
#define SZT_F "u"
#endif

#define TCPIP_MBOX_SIZE        32
#define TCPIP_THREAD_STACKSIZE 1024
#define TCPIP_THREAD_PRIO      8

/**
 * DEFAULT_RAW_RECVMBOX_SIZE: The mailbox size for the incoming packets on a
 * NETCONN_RAW. The queue size value itself is platform-dependent, but is passed
 * to sys_mbox_new() when the recvmbox is created.
 */
#define DEFAULT_RAW_RECVMBOX_SIZE 30

/**
 * DEFAULT_UDP_RECVMBOX_SIZE: The mailbox size for the incoming packets on a
 * NETCONN_UDP. The queue size value itself is platform-dependent, but is passed
 * to sys_mbox_new() when the recvmbox is created.
 */
#define DEFAULT_UDP_RECVMBOX_SIZE 30

/**
 * DEFAULT_TCP_RECVMBOX_SIZE: The mailbox size for the incoming packets on a
 * NETCONN_TCP. The queue size value itself is platform-dependent, but is passed
 * to sys_mbox_new() when the recvmbox is created.
 */
#define DEFAULT_TCP_RECVMBOX_SIZE 12
/**
 * DEFAULT_ACCEPTMBOX_SIZE: The mailbox size for the incoming connections.
 * The queue size value itself is platform-dependent, but is passed to
 * sys_mbox_new() when the acceptmbox is created.
 */
#define DEFAULT_ACCEPTMBOX_SIZE 12

#define LWIP_DNS                       1
#define LWIP_NETIF_EXT_STATUS_CALLBACK 1
#if (LWIP_DNS || LWIP_IGMP || LWIP_IPV6) && !defined(LWIP_RAND)
/* When using IGMP or IPv6, LWIP_RAND() needs to be defined to a random-function returning an u32_t random value*/
#include "lwip/arch.h"
u32_t lwip_rand(void);
#define LWIP_RAND() lwip_rand()
#endif
/**
 * IP_REASS_MAX_PBUFS: Number of buffers reserved for IP fragment reassembly.
 */
#ifndef IP_REASS_MAX_PBUFS
#define IP_REASS_MAX_PBUFS 4
#endif


/*
   ------------------------------------
   ---------- Socket options ----------
   ------------------------------------
*/
/**
 * LWIP_SO_SNDTIMEO==1: Enable send timeout for sockets/netconns and
 * SO_SNDTIMEO processing.
 */
#define LWIP_SO_SNDTIMEO                1//mandatory

/**
 * LWIP_SO_RCVTIMEO==1: Enable receive timeout for sockets/netconns and
 * SO_RCVTIMEO processing.
 */
#define LWIP_SO_RCVTIMEO                1//mandatory

/**
 * LWIP_TCP_KEEPALIVE==1: Enable TCP_KEEPIDLE, TCP_KEEPINTVL and TCP_KEEPCNT
 * options processing. Note that TCP_KEEPIDLE and TCP_KEEPINTVL have to be set
 * in seconds. (does not require sockets.c, and will affect tcp.c)
 */
#define LWIP_TCP_KEEPALIVE              1//mandatory

/**
 * LWIP_SO_LINGER==1: Enable SO_LINGER processing.
 */
#define LWIP_SO_LINGER                  1//mandatory

/**
 * LWIP_SO_RCVBUF==1: Enable SO_RCVBUF processing.
 */
#define LWIP_SO_RCVBUF                  1//mandatory

/**
 * SO_REUSE==1: Enable SO_REUSEADDR option.
 * This option is set via menuconfig.
 */
#define SO_REUSE                        1//mandatory
/**
 * TCP_LISTEN_BACKLOG==1: Handle backlog connections.
 */
#define TCP_LISTEN_BACKLOG 1
#define MEMP_NUM_NETCONN        12

#endif /* __LWIPOPTS_H__ */

/*****END OF FILE****/