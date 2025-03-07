/*
 * C
 *
 * Copyright 2015-2024 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */

/* Standards includes */
#include <stddef.h>
#include <string.h>
/* VEE port includes */
#include "LLNET_Common.h"
#include "WIFI_Common.h"
/* LWIP includes */
#include "lwip/dhcp.h"
#include "lwip/ip_addr.h"
#include "lwip/prot/dhcp.h"
#include "lwip/opt.h"
#include "lwip/init.h"
#include "lwip/netif.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"
#include "lwip/netifapi.h"
#include "lwip/tcpip.h"
#include "lwip/apps/lwiperf.h"
#include "lwip/sockets.h"
/* BSP/Drivers includes */
#include "netif/etharp.h"
#include "netif/ethernet.h"
#include "ethernetif.h"
#include "ksdk_mbedtls.h"
#include "arch/sys_arch.h"
#include "pin_mux.h"
#include "board.h"
#include "fsl_silicon_id.h"
#include "fsl_phy.h"
#include "fsl_component_serial_manager.h"
#include "fsl_phyrtl8201.h"
#include "fsl_phyrtl8211f.h"
#include "fsl_phyksz8081.h"
#include "fsl_gpio.h"
#include "fsl_mdio.h"
#include "fsl_enet.h"
#include "fsl_debug_console.h"

#include "mac_address_util.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define LWIP_ENET0                     ENET
#define LWIP_ENET0_PHY_ADDRESS         BOARD_ENET0_PHY_ADDRESS
#define LWIP_ENET0_INIT                ethernetif0_init
#ifdef MIMXRT1170_EVKB
#define LWIP_ENET0_PHY_OPS             &phyrtl8201_ops
#else // EVK Board
#define LWIP_ENET0_PHY_OPS             &phyksz8081_ops	
#endif
#define LWIP_ENET0_PHY_INT_PORT        GPIO3
#define LWIP_ENET0_PHY_INT_PIN         11U

#define LWIP_ENET1                     ENET_1G
#define LWIP_ENET1_PHY_ADDRESS         BOARD_ENET1_PHY_ADDRESS
#define LWIP_ENET1_INIT                ethernetif1_init
#define LWIP_ENET1_PHY_OPS             &phyrtl8211f_ops
#define LWIP_ENET1_PHY_INT_PORT        GPIO5
#define LWIP_ENET1_PHY_INT_PIN         13U

#define ENET_CLOCK_FREQ                CLOCK_GetRootClockFreq(kCLOCK_Root_Bus)

/* DHCP definitions */
#define MAX_DHCP_TRIES                 (4)
#define LWIP_DHCP_TASK_PRIORITY        (8)
#define LWIP_DHCP_POLLING_INTERVAL     (250)
/* DHCP process states */
#define DHCP_START                     (uint8_t) 1
#define DHCP_WAIT_ADDRESS              (uint8_t) 2
#define DHCP_ADDRESS_ASSIGNED          (uint8_t) 3
#define DHCP_TIMEOUT                   (uint8_t) 4
#define DHCP_LINK_DOWN                 (uint8_t) 5

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
static void ethernetif_static_ip_config(struct netif *net_if);

/*******************************************************************************
 * Variables
 ******************************************************************************/
char g_lwip_netif[NUMB_OF_NETIF_TO_STORE][MAX_SIZE_OF_NETIF_NAME];
#if (BOARD_NETWORK_USE_100M_ENET_PORT == 1)
static phy_handle_t            g_phy0_handle;
#if (NXP_EVKB_BOARD == 1)
static phy_rtl8201_resource_t  g_phy0_resource;
#else // EVK Board
static phy_ksz8081_resource_t  g_phy0_resource;
#endif
struct netif                   g_netif0;
#define DHCP_INTERFACE         (&g_netif0)
#else
static phy_handle_t            g_phy1_handle;
static phy_rtl8211f_resource_t g_phy1_resource;
struct netif                   g_netif1;
#define DHCP_INTERFACE         (&g_netif1)
#endif // (BOARD_NETWORK_USE_100M_ENET_PORT == 1)
static TaskHandle_t            g_dhcp_task_handle;
uint8_t                        g_dhcp_state;
static uint8_t                 g_dhcp_sleeping = 1;
uint8_t                        g_dns_servers_list_updated = 1;
static netif_ext_callback_t    g_link_status_callback_info;

/*******************************************************************************
 * Functions
 ******************************************************************************/
void BOARD_ENETFlexibleConfigure(enet_config_t *config, int enet_num) {
    if(enet_num == 0) {
		LLNET_DEBUG_TRACE("Ethernet %d initialized with RMII PHY\n");
        config->miiMode = kENET_RmiiMode;
    } else {
		LLNET_DEBUG_TRACE("Ethernet %d initialized with RGMII PHY\n");
        config->miiMode = kENET_RgmiiMode;
    }
}

/**
  * @brief  Reset the network interface ip, netmask and gateway addresses to zero.
  * @param  netif: the network interface
  * @retval None
  */
static void netif_addr_set_zero_ip4(struct netif* netif){
	ip_addr_set_zero_ip4(&netif->ip_addr);
	ip_addr_set_zero_ip4(&netif->netmask);
	ip_addr_set_zero_ip4(&netif->gw);
}

/**
  * @brief  This function is called when the network interface is disconnected.
  * @param  netif: the network interface
  * @retval None
  */
void netif_not_connected(struct netif *netif){
	netif_addr_set_zero_ip4(netif);
	LLNET_DEBUG_TRACE("[INFO] The network cable is not connected \n");
}

static void mdio_init(void) {
    (void)CLOCK_EnableClock(s_enetClock[ENET_GetInstance(LWIP_ENET0)]);
    ENET_SetSMI(LWIP_ENET0, ENET_CLOCK_FREQ, false);
    (void)CLOCK_EnableClock(s_enetClock[ENET_GetInstance(LWIP_ENET1)]);
    ENET_SetSMI(LWIP_ENET1, ENET_CLOCK_FREQ, false);
}

#if (BOARD_NETWORK_USE_100M_ENET_PORT == 1)
static status_t enet0_mdio_write(uint8_t phyAddr, uint8_t regAddr, uint16_t data){
    return ENET_MDIOWrite(LWIP_ENET0, phyAddr, regAddr, data);
}

static status_t enet0_mdio_read(uint8_t phyAddr, uint8_t regAddr, uint16_t *pData){
    return ENET_MDIORead(LWIP_ENET0, phyAddr, regAddr, pData);
}

static void netif0_config(void){
    ip_addr_t ipaddr;
    ip_addr_t netmask;
    ip_addr_t gw;

    ip_addr_set_zero_ip4(&ipaddr);
    ip_addr_set_zero_ip4(&netmask);
    ip_addr_set_zero_ip4(&gw);

    g_phy0_resource.read  = enet0_mdio_read;
    g_phy0_resource.write = enet0_mdio_write;
    ethernetif_config_t enet0_config ={.phyHandle     = &g_phy0_handle,
                                       .phyAddr       = LWIP_ENET0_PHY_ADDRESS,
                                       .phyOps        = LWIP_ENET0_PHY_OPS,
                                       .phyResource   = &g_phy0_resource,
                                       .srcClockHz    = ENET_CLOCK_FREQ,
#ifdef configMAC_ADDR
                                       .macAddress    = configMAC_ADDR
#endif
#if ETH_LINK_POLLING_INTERVAL_MS == 0
                                       .phyIntGpio    = LWIP_ENET0_PHY_INT_PORT,
                                       .phyIntGpioPin = LWIP_ENET0_PHY_INT_PIN
#endif
                                    };
#ifndef configMAC_ADDR
    /* Set special address for each chip. */
    (void)workaround_SILICONID_ConvertToMacAddr(&enet0_config.macAddress);
    LLNET_DEBUG_TRACE("MAC: %x:%x:%x:%x:%x:%x\n", enet0_config.macAddress[0],
                      enet0_config.macAddress[1], enet0_config.macAddress[2],
                      enet0_config.macAddress[3], enet0_config.macAddress[4],
                      enet0_config.macAddress[5]);
#endif

    /* Add the network interface */
#if NO_SYS
    netifapi_netif_add(&g_netif0, &ipaddr, &netmask, &gw, &enet0_config, LWIP_ENET0_INIT, &netif_input);
#else
    netifapi_netif_add(&g_netif0, &ipaddr, &netmask, &gw, &enet0_config, LWIP_ENET0_INIT, &tcpip_input);
#endif

    /* Set up the network interface */
    netifapi_netif_set_default(&g_netif0);
    netifapi_netif_set_up(&g_netif0);
}
#else
static status_t enet1_mdio_write(uint8_t phyAddr, uint8_t regAddr, uint16_t data){
    return ENET_MDIOWrite(LWIP_ENET1, phyAddr, regAddr, data);
}

static status_t enet1_mdio_read(uint8_t phyAddr, uint8_t regAddr, uint16_t *pData){
    return ENET_MDIORead(LWIP_ENET1, phyAddr, regAddr, pData);
}

static void netif1_config(void){
    ip_addr_t ipaddr;
    ip_addr_t netmask;
    ip_addr_t gw;

    ip_addr_set_zero_ip4(&ipaddr);
    ip_addr_set_zero_ip4(&netmask);
    ip_addr_set_zero_ip4(&gw);

    g_phy1_resource.read  = enet1_mdio_read;
    g_phy1_resource.write = enet1_mdio_write;
    ethernetif_config_t enet1_config ={.phyHandle     = &g_phy1_handle,
                                       .phyAddr       = LWIP_ENET1_PHY_ADDRESS,
                                       .phyOps        = LWIP_ENET1_PHY_OPS,
                                       .phyResource   = &g_phy1_resource,
                                       .srcClockHz    = ENET_CLOCK_FREQ,
#ifdef configMAC_ADDR
                                       .macAddress    = configMAC_ADDR
#endif
#if ETH_LINK_POLLING_INTERVAL_MS == 0
                                       .phyIntGpio    = LWIP_ENET1_PHY_INT_PORT,
                                       .phyIntGpioPin = LWIP_ENET1_PHY_INT_PIN
#endif
                                    };
#ifndef configMAC_ADDR
    /* Set special address for each chip. */
    (void)workaround_SILICONID_ConvertToMacAddr(&enet1_config.macAddress);
    LLNET_DEBUG_TRACE("MAC: %x:%x:%x:%x:%x:%x\n", enet1_config.macAddress[0],
                      enet1_config.macAddress[1], enet1_config.macAddress[2],
                      enet1_config.macAddress[3], enet1_config.macAddress[4],
                      enet1_config.macAddress[5]);
#endif

    /* Add the network interface */
#if NO_SYS
    netifapi_netif_add(&g_netif1, &ipaddr, &netmask, &gw, &enet1_config, LWIP_ENET1_INIT, &netif_input);
#else
    netifapi_netif_add(&g_netif1, &ipaddr, &netmask, &gw, &enet1_config, LWIP_ENET1_INIT, &tcpip_input);

#endif

    /* Registers the default network interface */
    netifapi_netif_set_default(&g_netif1);
    netifapi_netif_set_up(&g_netif1);
}
#endif // (BOARD_NETWORK_USE_100M_ENET_PORT == 1)

/*
 * Retrieve static IP configuration of the default network interface and set
 * IP parameters (Interface IP address, Netmask and Gateway IP address).
 */
static void ethernetif_static_ip_config(struct netif *net_if){
	ip_addr_t ipaddr;
	ip_addr_t netmask;
	ip_addr_t gw;
	ip_addr_t dnsaddr;
	LLNET_DEBUG_TRACE("getting static address \n");
	// static IP configuration. Retrieve IP settings from user properties.
	ipaddr.addr  = 0;
	netmask.addr = 0;
	gw.addr      = 0;
	netif_set_addr(net_if, &ipaddr , &netmask, &gw);
	LLNET_DEBUG_TRACE("[INFO] Static IP address assigned: %s\n", inet_ntoa(ipaddr.addr));

	// set static DNS Host IP address.
	if(DNS_MAX_SERVERS > 0){
		char * static_dns_ip_addr = NULL;
		if(static_dns_ip_addr != NULL){
			dnsaddr.addr = inet_addr(static_dns_ip_addr);
			dns_setserver(0, &dnsaddr);
			// notify DNS servers IP address updated
			g_dns_servers_list_updated = 1;
		}
	}
	}

/**
  * @brief  This function notify user about link status changement.
  * @param  netif: the network interface
  * @retval None
  */
static void netif_link_status_callback(struct netif *netif_, netif_nsc_reason_t reason, const netif_ext_callback_args_t *args) {
    ip_addr_t ipaddr = { 0 };
    ip_addr_t netmask = { 0 };
    ip_addr_t gw = { 0 };
    int32_t dhcp_conf_enabled = true;

    LLNET_DEBUG_TRACE("%s: reason=%d, args->link_changed.state=%d, netif_->num=%d\n", __func__, reason, args->link_changed.state, netif_->num);
    if (LWIP_NSC_LINK_CHANGED == reason) {
        if (args->link_changed.state) {
            if (dhcp_conf_enabled) {
                LLNET_DEBUG_TRACE("%s: The network cable is now connected\n", __func__);
                // Update DHCP state machine.
                g_dhcp_state = DHCP_START;
                netif_set_addr(netif_, &ipaddr , &netmask, &gw);
                // Resume DCHP thread.
                g_dhcp_sleeping = 0;
                vTaskResume(g_dhcp_task_handle);
                netif_set_up(netif_);
            } else {
                // Launch static network interface configuration.
                ethernetif_static_ip_config(netif_);
            }
        } else {
            LLNET_DEBUG_TRACE("%s: The network cable is now disconnected\n", __func__);
            if (dhcp_conf_enabled) {
                // Update DHCP state machine.
                g_dhcp_state = DHCP_LINK_DOWN;
            }
            // Link is down, set the interface down as well.
            netif_set_down(netif_);
            netif_not_connected(netif_);
        }
    }
}

static void dhcp_thread(void const * argument){
  struct netif *netif = (struct netif *) argument;
  uint32_t IPaddress;

    for (;;){
        // check if DHCP thread has to suspend
        if(g_dhcp_sleeping == 1){
            vTaskSuspend(xTaskGetCurrentTaskHandle());
        }

        switch (g_dhcp_state){
            case DHCP_START:{
                netif_addr_set_zero_ip4(netif);
                IPaddress = 0;
                netifapi_dhcp_start(netif);
                g_dhcp_state = DHCP_WAIT_ADDRESS;
                LLNET_DEBUG_TRACE("[INFO] DHCP started\n");
                break;
            }

            case DHCP_WAIT_ADDRESS:{
                /* Read the new IP address */
                IPaddress = netif->ip_addr.addr;
                if (IPaddress != 0) {
                    g_dhcp_state = DHCP_ADDRESS_ASSIGNED;
                    /* Stop DHCP */
#if LWIP_VERSION_MAJOR == 1
                    dhcp_stop(netif);
#elif LWIP_VERSION_MAJOR == 2
                    // LwIP version 2 onward clears existing IP address if dhcp is stopped.
                    //dhcp_stop(netif);
#else
#error "Invalid LWIP version (LWIP_VERSION_MAJOR)."
#endif
                    g_dhcp_sleeping = 1;

                    LLNET_DEBUG_TRACE("[INFO] DHCP address assigned: %s\n", inet_ntoa(IPaddress));
                    if(1){
                        ip_addr_t dnsaddr;
                        if(DNS_MAX_SERVERS > 0) {
                            char * static_dns_ip_addr = NULL;
                            if(static_dns_ip_addr != NULL){
                                dnsaddr.addr = inet_addr(static_dns_ip_addr);
                                dns_setserver(0, &dnsaddr);
                            }
                        }
                    }

                    // notify DNS servers IP address updated
                    g_dns_servers_list_updated = 1;
                }else{
#if LWIP_VERSION_MAJOR == 1
                    struct dhcp *dhcp = netif->dhcp;
#elif LWIP_VERSION_MAJOR == 2
                    struct dhcp *dhcp = (struct dhcp*)netif_get_client_data(netif, LWIP_NETIF_CLIENT_DATA_INDEX_DHCP);
#else
#error "Invalid LWIP version (LWIP_VERSION_MAJOR)."
#endif
                    /* DHCP timeout */
                    if (dhcp->tries > MAX_DHCP_TRIES){
                        g_dhcp_state = DHCP_TIMEOUT;

                        /* Stop DHCP */
                        dhcp_stop(netif);
                        g_dhcp_sleeping = 1;

                        LLNET_DEBUG_TRACE("[INFO] DHCP timeout\n");
                    }
                }
                break;
            }
            default:{
                break;
            }
        }
        // Stop polling for 250 ms.
        vTaskDelay(pdMS_TO_TICKS(LWIP_DHCP_POLLING_INTERVAL));
    }
}

/**
 * Network initialization. Start network interfaces and configure it.
 * @return 0 if no error occurred, error code otherwise.
 */
int32_t llnet_lwip_init(void){
#ifndef ENABLE_WIFI
    LLNET_DEBUG_TRACE("network initialize \n");
    int32_t dhcp_conf_enabled =  true;
    /* Initialize the MDIO bus */
    mdio_init();
    /* Initialize the LwIP TCP/IP stack */
    tcpip_init(NULL, NULL);
    /* Configure the Network Interfaces */
#if (BOARD_NETWORK_USE_100M_ENET_PORT == 1)
    netif0_config();
#else
    netif1_config();
#endif // (BOARD_NETWORK_USE_100M_ENET_PORT == 1)
    if(dhcp_conf_enabled){
        /* Start DHCPClient */
        g_dhcp_sleeping = 0;
#if defined(__GNUC__)
        xTaskCreate((TaskFunction_t)dhcp_thread, "DHCP", configMINIMAL_STACK_SIZE * 5, DHCP_INTERFACE, LWIP_DHCP_TASK_PRIORITY, &g_dhcp_task_handle);
#else
        xTaskCreate((TaskFunction_t)dhcp_thread, "DHCP", configMINIMAL_STACK_SIZE * 2, DHCP_INTERFACE, LWIP_DHCP_TASK_PRIORITY, &g_dhcp_task_handle);
#endif
    }
#else
    /* Initialize WIFI */
    if (WIFI_initialize_f() != true){
        LLNET_DEBUG_TRACE("FAILED to init network. Reboot the board and try again.\n");
        return -1;
    }
    WPL_AddNetwork(WIFI_SSID, WIFI_PWD, "mywifi");
    WPL_Join("mywifi");
#endif
    /* Set the link callback function, this function is called on change of link status. */
    netif_add_ext_callback(&g_link_status_callback_info, netif_link_status_callback);
    return 0;
}

void set_lwip_netif_name(int32_t id, char *netif_name){
	(void)strcpy(g_lwip_netif[id], netif_name);
}

char *get_lwip_netif_name(int32_t id){
	return g_lwip_netif[id];
}

struct netif* getNetworkInterface(uint8_t* name){
	char *llnet_netif[] = LLNET_NETIF_NAMES;
	for (int32_t netif_idx = 0; netif_idx < (sizeof(llnet_netif) / sizeof(llnet_netif[0])); netif_idx++) {
		if (strcmp((char *)name, llnet_netif[netif_idx]) == 0) {
			// the same interface name at LLNET level can have different names at platform level
			// so, keep trying to find the lwip level interface until one is found or until we exhaust the list
			struct netif *ret = netif_find(g_lwip_netif[netif_idx]);
			if (ret) {
				return ret;
			}
		}
	}
	return NULL;
}
