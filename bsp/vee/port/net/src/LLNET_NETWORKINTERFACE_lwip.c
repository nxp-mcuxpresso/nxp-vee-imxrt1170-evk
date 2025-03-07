/*
 * C
 *
 * Copyright 2014-2023 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */


/**
 * @file
 * @brief LLNET_NETWORKINTERFACE 2.1.0 implementation over LWIP.
 * @author MicroEJ Developer Team
 * @version 0.2.2
 * @date 27 November 2020
 */

#include <LLNET_NETWORKINTERFACE_impl.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <lwip/opt.h>
#include <lwip/netif.h>
#include <lwip/sockets.h>
//#include "LLNET_CONSTANTS.h"
#include "LLNET_ERRORS.h"
#include "LLNET_Common.h"
#include "lwip_util.h"

#ifdef __cplusplus
	extern "C" {
#endif

int32_t LLNET_NETWORKINTERFACE_IMPL_getVMInterface(int32_t index, uint8_t* ifnameBuffer, int32_t ifnameBufferLength)
{
	char *llnet_netif[] = LLNET_NETIF_NAMES;
	char *platform_netif[] = PLATFORM_NETIF_NAMES;

//	LLNET_DEBUG_TRACE("%s(id=%d)\n", __func__, id);
	// retrieve network interface list global variable
	struct netif *pnetif_list = netif_list;
	// look at each network interface to find the one matching with platform_netif
	while (pnetif_list != NULL) {
		int8_t current_netif[6] = {0};
		sprintf((char *)current_netif, "%c%c%d", pnetif_list->name[0], pnetif_list->name[1], pnetif_list->num);
		// check if the mapped netif name matches with the platform netif name
		if (strncmp((char*)current_netif, platform_netif[index], strlen(platform_netif[index])) == 0) {
			// store the current lwip netif name
			set_lwip_netif_name(index, (char *)current_netif);
			// check if we have this netif id mapped in LLNET_configuration.h
			if (index < (sizeof(platform_netif) / sizeof(platform_netif[0]))) {
				// we need space for the netif name and the trailing 'NULL'.
				int32_t requiredLen = strlen(llnet_netif[index]) + 1;
				if (ifnameBufferLength < requiredLen) {
					return J_EINVAL;
				}
				strcpy((char*)ifnameBuffer, llnet_netif[index]);
				return requiredLen;
			} else {
				return J_EINVAL;
			}
		}
		// not found, look at the next interface
		pnetif_list = pnetif_list->next;
	}
	// error: no address found at the 'idAddr' index
	return 0;
}

// v6 is not supported yet, but here are the v4 and v6 constants 
// for the getVMInterfaceAddress protocol between Java and the native stacks

// ipv4 address info size (tag (1) + IP (4) + prefix (1) + hasBroadcast (1) + broadcast IP (4))
#define IPV4_ADDR_INFO_SIZE 11
// ipv6 address info size (tag (1) + IP (16) + prefix (1)
#define IPV6_ADDR_INFO_SIZE 18

// ipv4 address tag
#define IPV4_ADDR_TAG 4
// ipv6 address tag 
#define IPV6_ADDR_TAG 6

static int32_t _getMaskLen(struct netif *netif) 
{
	int32_t len = 0;
	u32_t mask = ip4_addr_get_u32(netif_ip4_netmask(netif));
	while (mask) {
		mask >>= 1;
		len++;
	}
	return len;
}
int32_t LLNET_NETWORKINTERFACE_IMPL_getVMInterfaceAddress(int32_t idxIf, uint8_t* ifname, int32_t ifnameLength, int32_t idxAddr, int8_t* addrInfo, int32_t addrInfoLength)
{
    //LLNET_DEBUG_TRACE("%s\n", __func__);
    // Récupérer la liste des interfaces réseau (variable globale)
    struct netif *pnetif_list = netif_list;

    // Parcourir chaque interface réseau pour trouver celle qui correspond à l'interface stockée dans lwip netif
    while (pnetif_list != NULL) {
        int8_t current_netif[6] = {0};
        sprintf((char*)current_netif, "%c%c%d", pnetif_list->name[0], pnetif_list->name[1], pnetif_list->num);

        // Vérifier si le nom de netif correspond au nom de lwip netif
        if (strcmp((char*)current_netif, get_lwip_netif_name(idxIf)) == 0) {
            // Interface trouvée, retourner l'adresse IP configurée
            // 'idAddr' n'est pas pris en compte ici car une seule adresse IP est configurée par interface réseau

            addrInfo[0] = IPV4_ADDR_TAG;
            memcpy(&addrInfo[1], ip_2_ip4(&pnetif_list->ip_addr), sizeof(ip4_addr_t));

            // Le préfixe peut être de 0 à 128, nous l'encodons donc sur 1 octet, de 0 à 0x80
            addrInfo[1 + sizeof(ip4_addr_t)] = _getMaskLen(pnetif_list);

            // Maintenant, la diffusion
            if ((pnetif_list->flags & NETIF_FLAG_BROADCAST) == 0) {
                addrInfo[2 + sizeof(ip4_addr_t)] = 0;
            } else {
                u32_t broadcast = ip4_addr_get_u32(netif_ip4_addr(pnetif_list)) | ~ip4_addr_get_u32(netif_ip4_netmask(pnetif_list));
                int32_t pos = 2 + sizeof(ip4_addr_t);
                addrInfo[pos++] = 1;
                addrInfo[pos++] = (broadcast) & 0xFF;
                addrInfo[pos++] = (broadcast >> 8) & 0xFF;
                addrInfo[pos++] = (broadcast >> 16) & 0xFF;
                addrInfo[pos++] = (broadcast >> 24) & 0xFF;
            }

            return IPV4_ADDR_INFO_SIZE;
        }

        // Non trouvé, passer à l'interface suivante
        pnetif_list = pnetif_list->next;
    }

    // Erreur : aucune adresse trouvée à l'index 'idAddr'
    return 0;
}



int32_t LLNET_NETWORKINTERFACE_IMPL_getVMInterfaceAddressesCount(int32_t index, uint8_t* ifname, int32_t ifnameLength)
{
	LLNET_DEBUG_TRACE("%s\n", __func__);
	return 1; // only one address configuration per network interface
}

int32_t LLNET_NETWORKINTERFACE_IMPL_getVMInterfacesCount(void)
{
	LLNET_DEBUG_TRACE("%s\n", __func__);
	struct netif *pnetif_list = netif_list;
	int32_t ifCnt = 0;
	while(pnetif_list != NULL){
		++ifCnt;
		// get next interface, and verify it exists or not
		pnetif_list = pnetif_list->next;
	}
	return ifCnt;
}

int32_t LLNET_NETWORKINTERFACE_IMPL_isLoopback(uint8_t* ifname, int32_t ifnameLength)
{
	LLNET_DEBUG_TRACE("%s for %s\n", __func__, (const char *)ifname);
	#if (defined(LWIP_NETIF_LOOPBACK) && (LWIP_NETIF_LOOPBACK != 0))
		char *llnet_netif[] = LLNET_NETIF_NAMES;
		char *platform_netif[] = PLATFORM_NETIF_NAMES;
		for (int32_t netif_idx = 0; netif_idx < (sizeof(llnet_netif) / sizeof(llnet_netif[0])); netif_idx++) {
			if (strcmp((char *)ifname, llnet_netif[netif_idx]) == 0) {
				return (strcmp("lo", platform_netif[netif_idx]) == 0) ? 0 : 1; // 0 means true
			}
		}
		return 1; // 1 means false
	#else
		return 1; // 1 means false
	#endif
}

int32_t LLNET_NETWORKINTERFACE_IMPL_isPointToPoint(uint8_t* ifname, int32_t ifnameLength)
{
	LLNET_DEBUG_TRACE("%s\n", __func__);
	return 1; // 0 means true
}

int32_t LLNET_NETWORKINTERFACE_IMPL_isUp(uint8_t* ifname, int32_t ifnameLength)
{
	LLNET_DEBUG_TRACE("%s\n", __func__);
	struct netif *pnetif = getNetworkInterface(ifname);
	if(pnetif != NULL){
		return netif_is_up(pnetif) ? 0 : 1; // 0 means true
	}
	return J_EUNKNOWN;
}

int32_t LLNET_NETWORKINTERFACE_IMPL_supportsMulticast(uint8_t* ifname, int32_t ifnameLength)
{
	LLNET_DEBUG_TRACE("%s\n", __func__);
	struct netif *pnetif = getNetworkInterface(ifname);
	if(pnetif != NULL){
		return (pnetif->flags & NETIF_FLAG_BROADCAST) != 0 ? 0 : 1; // 0 means true
	}
	return J_EUNKNOWN;
}

int32_t LLNET_NETWORKINTERFACE_IMPL_getHardwareAddress(uint8_t* ifname, int32_t ifnameLength,int8_t* hwAddr, int32_t hwAddrMaxLength)
{
	LLNET_DEBUG_TRACE("%s\n", __func__);
	struct netif *pnetif = getNetworkInterface(ifname);
	if(pnetif != NULL){
		memcpy(hwAddr, pnetif->hwaddr, pnetif->hwaddr_len);
		return pnetif->hwaddr_len;
	}
	// hardware address not found
	return 0;
}

int32_t LLNET_NETWORKINTERFACE_IMPL_getMTU(uint8_t* ifname, int32_t ifnameLength)
{
	LLNET_DEBUG_TRACE("%s\n", __func__);
	struct netif *pnetif = getNetworkInterface(ifname);
	if(pnetif != NULL){
		if (pnetif->mtu == 0) {
			// -1 means unknown MTU value 
			return -1;
		}
		return pnetif->mtu;
	}
	// hardware address not found
	return J_EUNKNOWN;
}

#ifdef __cplusplus
	}
#endif
