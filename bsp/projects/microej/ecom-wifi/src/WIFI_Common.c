
/*
 * Copyright 2020-2022 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "WIFI_Common.h"
#include "app_config.h"
#include "wifi_config.h"
#include "wlan_bt_fw.h"
#include "wlan.h"
#include "wifi.h"
#include "wm_net.h"
#include <wm_os.h>
#include "dhcp-server.h"
#include <stdio.h>
#include "event_groups.h"
#include "LLECOM_WIFI_impl.h"
#include "ecom_wifi_configuration.h"


/*******************************************************************************
 * Definitions
 ******************************************************************************/

#define WPL_DEBUG 
#define MAX_JSON_NETWORK_RECORD_LENGTH 185

#define WPL_SYNC_TIMEOUT_MS portMAX_DELAY

#define UAP_NETWORK_NAME "uap-network"

#define EVENT_BIT(event) (1 << event)

#define WPL_SYNC_INIT_GROUP EVENT_BIT(WLAN_REASON_INITIALIZED) | EVENT_BIT(WLAN_REASON_INITIALIZATION_FAILED)

#define WPL_SYNC_CONNECT_GROUP                                                                  \
    EVENT_BIT(WLAN_REASON_SUCCESS) | EVENT_BIT(WLAN_REASON_CONNECT_FAILED) |                    \
        EVENT_BIT(WLAN_REASON_NETWORK_NOT_FOUND) | EVENT_BIT(WLAN_REASON_NETWORK_AUTH_FAILED) | \
        EVENT_BIT(WLAN_REASON_ADDRESS_FAILED)

#define WPL_SYNC_DISCONNECT_GROUP EVENT_BIT(WLAN_REASON_USER_DISCONNECT)

#define WPL_SYNC_UAP_START_GROUP EVENT_BIT(WLAN_REASON_UAP_SUCCESS) | EVENT_BIT(WLAN_REASON_UAP_START_FAILED)

#define WPL_SYNC_UAP_STOP_GROUP EVENT_BIT(WLAN_REASON_UAP_STOPPED) | EVENT_BIT(WLAN_REASON_UAP_STOP_FAILED)

#define EVENT_SCAN_DONE     23
#define WPL_SYNC_SCAN_GROUP EVENT_BIT(EVENT_SCAN_DONE)

typedef enum _wpl_state
{
    WPL_NOT_INITIALIZED,
    WPL_INITIALIZED,
    WPL_STARTED,
} wpl_state_t;

/*******************************************************************************
 * Variables
 ******************************************************************************/
static wpl_state_t s_wplState            = WPL_NOT_INITIALIZED;
static bool s_wplStaConnected            = false;
static bool s_wplUapActivated            = false;
struct wlan_network uap_network;
struct wlan_network sta_network;
static EventGroupHandle_t s_wplSyncEvent = NULL;
static linkLostCb_t s_linkLostCb         = NULL;
static char *ssids_json                  = NULL;
struct wlan_scan_result scan_result = {0};
int countAP = 0;
/*******************************************************************************
 * Prototypes
 ******************************************************************************/


/* Link lost callback */
static void LinkStatusChangeCallback(bool linkState)
{
    if (linkState == false)
    {
        PRINTF("-------- LINK LOST --------\r\n");
    }
    else
    {
        PRINTF("-------- LINK REESTABLISHED --------\r\n");
    }
}



/**
 * @brief function to initialize the wifi module
 *
 * @return true if success else false
 */
bool WIFI_initialize_f(void)
{
    wpl_ret_t err = WPLRET_FAIL;
    /* Initialize WIFI*/
    err = WPL_Init();
    if (err != WPLRET_SUCCESS)
    {
        PRINTF("[!] WPL_Init: Failed, error: %d\r\n", (uint32_t)err);
        return false;
    }

    err = WPL_Start(LinkStatusChangeCallback);
    if (err != WPLRET_SUCCESS)
    {
        PRINTF("[!] WPL_Start: Failed, error: %d\r\n", (uint32_t)err);
        return false;
    }
    return true;
}

int wlan_event_callback(enum wlan_event_reason reason, void *data);
static int WLP_process_results(unsigned int count);

/*******************************************************************************
 * Code
 ******************************************************************************/
/* Callback Function passed to WLAN Connection Manager. The callback function
 * gets called when there are WLAN Events that need to be handled by the
 * application.
 */
int wlan_event_callback(enum wlan_event_reason reason, void *data)
{
#ifdef WPL_DEBUG
#endif
    if (s_wplState >= WPL_INITIALIZED)
    {
        xEventGroupSetBits(s_wplSyncEvent, EVENT_BIT(reason));
    }

    switch (reason)
    {
        case WLAN_REASON_SUCCESS:
            if (s_wplStaConnected)
            {
                s_linkLostCb(true);
            }
            break;

        case WLAN_REASON_AUTH_SUCCESS:
            break;

        case WLAN_REASON_CONNECT_FAILED:
        case WLAN_REASON_NETWORK_NOT_FOUND:
        case WLAN_REASON_NETWORK_AUTH_FAILED:
            if (s_wplStaConnected)
            {
                s_linkLostCb(false);
            }
            break;

        case WLAN_REASON_ADDRESS_SUCCESS:
            break;
        case WLAN_REASON_ADDRESS_FAILED:
            break;
        case WLAN_REASON_LINK_LOST:
            if (s_wplStaConnected)
            {
                s_linkLostCb(false);
            }
            break;

        case WLAN_REASON_CHAN_SWITCH:
            break;
        case WLAN_REASON_WPS_DISCONNECT:
            break;
        case WLAN_REASON_USER_DISCONNECT:
            break;
        case WLAN_REASON_INITIALIZED:
            break;
        case WLAN_REASON_INITIALIZATION_FAILED:
            break;
        case WLAN_REASON_PS_ENTER:
            break;
        case WLAN_REASON_PS_EXIT:
            break;
        case WLAN_REASON_UAP_SUCCESS:
            break;

        case WLAN_REASON_UAP_CLIENT_ASSOC:
#ifdef WPL_DEBUG
            PRINTF("Client => ");
            print_mac((const char *)data);
            PRINTF("Associated with Soft AP\r\n");
#endif
            break;
        case WLAN_REASON_UAP_CLIENT_DISSOC:
#ifdef WPL_DEBUG
            PRINTF("Client => ");
            print_mac((const char *)data);
            PRINTF("Dis-Associated from Soft AP\r\n");
#endif
            break;

        case WLAN_REASON_UAP_START_FAILED:
            break;
        case WLAN_REASON_UAP_STOP_FAILED:
            break;
        case WLAN_REASON_UAP_STOPPED:
            break;
        default:
#ifdef WPL_DEBUG
            PRINTF("Unknown Wifi CB Reason %d\r\n", reason);
#endif
            break;
    }

    return WM_SUCCESS;
}

wpl_ret_t WPL_Init(void)
{
    wpl_ret_t status = WPLRET_SUCCESS;
    int ret;

    if (s_wplState != WPL_NOT_INITIALIZED)
    {
        status = WPLRET_FAIL;
    }

    if (status == WPLRET_SUCCESS)
    {
        if (s_wplSyncEvent == NULL)
        {
            s_wplSyncEvent = xEventGroupCreate();
        }

        if (s_wplSyncEvent == NULL)
        {
            status = WPLRET_FAIL;
        }
    }

    if (status == WPLRET_SUCCESS)
    {
        ret = wlan_init(wlan_fw_bin, wlan_fw_bin_len);
        if (ret != WM_SUCCESS)
        {
            status = WPLRET_FAIL;
        }
    }

    if (status == WPLRET_SUCCESS)
    {
        s_wplState = WPL_INITIALIZED;
    }

    return status;
}

wpl_ret_t WPL_Start(linkLostCb_t callbackFunction)
{
    wpl_ret_t status = WPLRET_SUCCESS;
    int ret;
    EventBits_t syncBit;

    if (s_wplState != WPL_INITIALIZED)
    {
        status = WPLRET_NOT_READY;
    }

    if (status == WPLRET_SUCCESS)
    {
        xEventGroupClearBits(s_wplSyncEvent, WPL_SYNC_INIT_GROUP);

        ret = wlan_start(wlan_event_callback);
        if (ret != WM_SUCCESS)
        {
            status = WPLRET_FAIL;
        }
    }

    if (status == WPLRET_SUCCESS)
    {
        syncBit = xEventGroupWaitBits(s_wplSyncEvent, WPL_SYNC_INIT_GROUP, pdTRUE, pdFALSE, WPL_SYNC_TIMEOUT_MS);
        if (syncBit & EVENT_BIT(WLAN_REASON_INITIALIZED))
        {
            s_linkLostCb = callbackFunction;
            status       = WPLRET_SUCCESS;
        }
        else if (syncBit & EVENT_BIT(WLAN_REASON_INITIALIZATION_FAILED))
        {
            status = WPLRET_FAIL;
        }
        else
        {
            status = WPLRET_TIMEOUT;
            PRINTF("timeout \n");
        }
    }

    if (status == WPLRET_SUCCESS)
    {
        s_wplState = WPL_STARTED;
    }

    return status;
}

wpl_ret_t WPL_Stop(void)
{
    wpl_ret_t status = WPLRET_SUCCESS;
    int ret;

    if (s_wplState != WPL_STARTED)
    {
        status = WPLRET_NOT_READY;
    }

    if (status == WPLRET_SUCCESS)
    {
        ret = wlan_stop();
        if (ret != WM_SUCCESS)
        {
            status = WPLRET_FAIL;
        }
    }

    if (status == WPLRET_SUCCESS)
    {
        s_wplState = WPL_INITIALIZED;
    }

    return status;
}

wpl_ret_t WPL_Start_AP(char *ssid, char *password, int chan)
{
    wpl_ret_t status = WPLRET_SUCCESS;
    int ret;
    enum wlan_security_type security = WLAN_SECURITY_NONE;
    EventBits_t syncBit;

    if ((s_wplState != WPL_STARTED) || (s_wplUapActivated != false))
    {
        status = WPLRET_NOT_READY;
    }

    if (status == WPLRET_SUCCESS)
    {
        if ((strlen(ssid) == 0) || (strlen(ssid) > IEEEtypes_SSID_SIZE))
        {
            status = WPLRET_BAD_PARAM;
        }
    }

    if (status == WPLRET_SUCCESS)
    {
        if (strlen(password) == 0)
        {
            security = WLAN_SECURITY_NONE;
        }
        else if ((strlen(password) >= WPL_WIFI_PASSWORD_MIN_LEN) && (strlen(password) <= WPL_WIFI_PASSWORD_LENGTH))
        {
            security = WLAN_SECURITY_WPA2;
        }
        else
        {
            status = WPLRET_BAD_PARAM;
        }
    }

    if (status == WPLRET_SUCCESS)
    {
        wlan_initialize_uap_network(&uap_network);

        memcpy(uap_network.ssid, ssid, strlen(ssid));
        uap_network.ip.ipv4.address  = ipaddr_addr(WPL_WIFI_AP_IP_ADDR);
        uap_network.ip.ipv4.gw       = ipaddr_addr(WPL_WIFI_AP_IP_ADDR);
        uap_network.channel          = chan;
        uap_network.security.type    = security;
        uap_network.security.psk_len = strlen(password);
        strncpy(uap_network.security.psk, password, strlen(password));
    }

    if (status == WPLRET_SUCCESS)
    {
        ret = wlan_add_network(&uap_network);
        if (ret != WM_SUCCESS)
        {
            status = WPLRET_FAIL;
        }
    }

    if (status == WPLRET_SUCCESS)
    {
        xEventGroupClearBits(s_wplSyncEvent, WPL_SYNC_UAP_START_GROUP);

        ret = wlan_start_network(uap_network.name);
        if (ret != WM_SUCCESS)
        {
            status = WPLRET_FAIL;
        }
        else
        {
            syncBit =
                xEventGroupWaitBits(s_wplSyncEvent, WPL_SYNC_UAP_START_GROUP, pdTRUE, pdFALSE, WPL_SYNC_TIMEOUT_MS);
            if (syncBit & EVENT_BIT(WLAN_REASON_UAP_SUCCESS))
            {
                status = WPLRET_SUCCESS;
            }
            else if (syncBit & EVENT_BIT(WLAN_REASON_UAP_START_FAILED))
            {
                status = WPLRET_FAIL;
            }
            else
            {
                status = WPLRET_TIMEOUT;
            }
        }

        if (status != WPLRET_SUCCESS)
        {
            wlan_remove_network(uap_network.name);
        }
    }

    if (status == WPLRET_SUCCESS)
    {
        ret = dhcp_server_start(net_get_uap_handle());
        if (ret != WM_SUCCESS)
        {
            wlan_stop_network(uap_network.name);
            wlan_remove_network(uap_network.name);
            status = WPLRET_FAIL;
        }
    }

    if (status == WPLRET_SUCCESS)
    {
        s_wplUapActivated = true;
    }

    return status;
}

wpl_ret_t WPL_Stop_AP(void)
{
    wpl_ret_t status = WPLRET_SUCCESS;
    int ret;
    EventBits_t syncBit;

    if ((s_wplState != WPL_STARTED) || (s_wplUapActivated != true))
    {
        status = WPLRET_NOT_READY;
    }

    if (status == WPLRET_SUCCESS)
    {
        dhcp_server_stop();

        xEventGroupClearBits(s_wplSyncEvent, WPL_SYNC_UAP_START_GROUP);

        ret = wlan_stop_network(UAP_NETWORK_NAME);
        if (ret != WM_SUCCESS)
        {
            status = WPLRET_FAIL;
        }
        else
        {
            syncBit =
                xEventGroupWaitBits(s_wplSyncEvent, WPL_SYNC_UAP_STOP_GROUP, pdTRUE, pdFALSE, WPL_SYNC_TIMEOUT_MS);
            if (syncBit & EVENT_BIT(WLAN_REASON_UAP_STOPPED))
            {
                status = WPLRET_SUCCESS;
            }
            else if (syncBit & EVENT_BIT(WLAN_REASON_UAP_STOP_FAILED))
            {
                status = WPLRET_FAIL;
            }
            else
            {
                status = WPLRET_TIMEOUT;
            }
        }
    }

    if (status == WPLRET_SUCCESS)
    {
        ret = wlan_remove_network(UAP_NETWORK_NAME);
        if (ret != WM_SUCCESS)
        {
            status = WPLRET_FAIL;
        }
    }

    if (status == WPLRET_SUCCESS)
    {
        s_wplUapActivated = false;
    }

    return status;
}

static int WLP_process_results(unsigned int count)
{
    int ret                             = 0;
    uint32_t ssids_json_len             = count * MAX_JSON_NETWORK_RECORD_LENGTH;
    countAP = count;
    /* Add length of "{"networks":[]}" */
    ssids_json_len += 15;

    ssids_json = pvPortMalloc(ssids_json_len);
    if (ssids_json == NULL)
    {
        PRINTF("[!] Memory allocation failed\r\n");
        xEventGroupSetBits(s_wplSyncEvent, EVENT_BIT(EVENT_SCAN_DONE));
        return WM_FAIL;
    }

    /* Start building JSON */
    strcpy(ssids_json, "{\"networks\":[");
    uint32_t ssids_json_idx = strlen(ssids_json);

    for (int i = 0; i < count; i++)
    {
        wlan_get_scan_result(i, &scan_result);

        char security[40];
        security[0] = '\0';

        if (scan_result.wpa2_entp)
        {
            strcat(security, "WPA2_ENTP ");
        }
        if (scan_result.wep)
        {
            strcat(security, "WEP ");
        }
        if (scan_result.wpa)
        {
            strcat(security, "WPA ");
        }
        if (scan_result.wpa2)
        {
            strcat(security, "WPA2 ");
        }
        if (scan_result.wpa3_sae)
        {
            strcat(security, "WPA3_SAE ");
        }

        if (i != 0)
        {
            /* Add ',' separator before next entry */
            ssids_json[ssids_json_idx++] = ',';
        }
        if (ret > 0)
        {
            ssids_json_idx += ret;
        }
        else
        {
            vPortFree(ssids_json);
            ssids_json = NULL;
            xEventGroupSetBits(s_wplSyncEvent, EVENT_BIT(EVENT_SCAN_DONE));
            return WM_FAIL;
        }
    }

    /* End of JSON "]}" */
    strcpy(ssids_json + ssids_json_idx, "]}");

    xEventGroupSetBits(s_wplSyncEvent, EVENT_BIT(EVENT_SCAN_DONE));
    return WM_SUCCESS;
}

char *WPL_Scan(void)
{
    wpl_ret_t status = WPLRET_SUCCESS;
    int ret;
    EventBits_t syncBit;

    if (s_wplState != WPL_STARTED)
    {
        status = WPLRET_NOT_READY;
    }

    if (status == WPLRET_SUCCESS)
    {
        ret = wlan_scan(WLP_process_results);
        if (ret != WM_SUCCESS)
        {
            status = WPLRET_FAIL;
        }
    }

    if (status == WPLRET_SUCCESS)
    {
        syncBit = xEventGroupWaitBits(s_wplSyncEvent, WPL_SYNC_SCAN_GROUP, pdTRUE, pdFALSE, WPL_SYNC_TIMEOUT_MS);
        if (syncBit & EVENT_BIT(EVENT_SCAN_DONE))
        {
            status = WPLRET_SUCCESS;
        }
        else
        {
            status = WPLRET_TIMEOUT;
        }
    }

    if (status == WPLRET_SUCCESS)
    {
        return ssids_json;
    }

    return NULL;
}

wpl_ret_t WPL_AddNetwork(char *ssid, char *password, char *label)
{
    wpl_ret_t status = WPLRET_SUCCESS;
    int ret;
    enum wlan_security_type security = WLAN_SECURITY_NONE;

    if (s_wplState != WPL_STARTED)
    {
        status = WPLRET_NOT_READY;
    }

    if (status == WPLRET_SUCCESS)
    {
        if ((strlen(label) == 0) || (strlen(label) > (WLAN_NETWORK_NAME_MAX_LENGTH - 1)))
        {
            status = WPLRET_BAD_PARAM;
        }
    }

    if (status == WPLRET_SUCCESS)
    {
        if ((strlen(ssid) == 0) || (strlen(ssid) > IEEEtypes_SSID_SIZE))
        {
            status = WPLRET_BAD_PARAM;
        }
    }

    if (status == WPLRET_SUCCESS)
    {
        if (strlen(password) == 0)
        {
            security = WLAN_SECURITY_NONE;
        }
        else if ((strlen(password) >= WPL_WIFI_PASSWORD_MIN_LEN) && (strlen(password) <= WPL_WIFI_PASSWORD_LENGTH))
        {
            security = WLAN_SECURITY_WILDCARD;
        }
        else
        {
            status = WPLRET_BAD_PARAM;
        }
    }

    if (status == WPLRET_SUCCESS)
    {
      
        memset(&sta_network, 0, sizeof(struct wlan_network));
        strcpy(sta_network.name, label);
        memcpy(sta_network.ssid, (const char *)ssid, strlen(ssid));
        sta_network.ip.ipv4.addr_type = ADDR_TYPE_DHCP;
        sta_network.ssid_specific     = 1;
        sta_network.security.type     = security;
        sta_network.security.psk_len  = strlen(password);
        strncpy(sta_network.security.psk, password, strlen(password));
    }

    if (status == WPLRET_SUCCESS)
    {
        ret = wlan_add_network(&sta_network);
        if (ret != WM_SUCCESS)
        {
            status = WPLRET_FAIL;
        }
    }

    return status;
}

wpl_ret_t WPL_RemoveNetwork(char *label)
{
    wpl_ret_t status = WPLRET_SUCCESS;
    int ret;

    if (s_wplState != WPL_STARTED)
    {
        status = WPLRET_NOT_READY;
    }

    if (status == WPLRET_SUCCESS)
    {
        if ((strlen(label) == 0) || (strlen(label) > (WLAN_NETWORK_NAME_MAX_LENGTH - 1)))
        {
            status = WPLRET_BAD_PARAM;
        }
    }

    if (status == WPLRET_SUCCESS)
    {
        ret = wlan_remove_network(label);
        if (ret != WM_SUCCESS)
        {
            status = WPLRET_FAIL;
        }
    }

    return status;
}

wpl_ret_t WPL_Join(char *label)
{
    wpl_ret_t status = WPLRET_SUCCESS;
    int ret;
    EventBits_t syncBit;

    if ((s_wplState != WPL_STARTED) || (s_wplStaConnected != false))
    {
        status = WPLRET_NOT_READY;
    }

    if (status == WPLRET_SUCCESS)
    {
        if ((strlen(label) == 0) || (strlen(label) > (WLAN_NETWORK_NAME_MAX_LENGTH - 1)))
        {
            status = WPLRET_BAD_PARAM;
        }
    }

    if (status == WPLRET_SUCCESS)
    {
        xEventGroupClearBits(s_wplSyncEvent, WPL_SYNC_CONNECT_GROUP);

        ret = wlan_connect(label);
        if (ret != WM_SUCCESS)
        {
            status = WPLRET_FAIL;
        }
    }

    if (status == WPLRET_SUCCESS)
    {
        syncBit = xEventGroupWaitBits(s_wplSyncEvent, WPL_SYNC_CONNECT_GROUP, pdTRUE, pdFALSE, WPL_SYNC_TIMEOUT_MS);
        if (syncBit & EVENT_BIT(WLAN_REASON_SUCCESS))
        {
            status = WPLRET_SUCCESS;
        }
        else if (syncBit & EVENT_BIT(WLAN_REASON_CONNECT_FAILED))
        {
            status = WPLRET_FAIL;
        }
        else if (syncBit & EVENT_BIT(WLAN_REASON_NETWORK_NOT_FOUND))
        {
            status = WPLRET_NOT_FOUND;
        }
        else if (syncBit & EVENT_BIT(WLAN_REASON_NETWORK_AUTH_FAILED))
        {
            status = WPLRET_AUTH_FAILED;
        }
        else if (syncBit & EVENT_BIT(WLAN_REASON_ADDRESS_FAILED))
        {
            status = WPLRET_ADDR_FAILED;
        }
        else
        {
            status = WPLRET_TIMEOUT;
        }

        if (status != WPLRET_SUCCESS)
        {
            /* Abort the next connection attempt */
            WPL_Leave();
        }
    }

    if (status == WPLRET_SUCCESS)
    {
        PRINTF("connected \n");
        s_wplStaConnected = true;
    }

    return status;
}

wpl_ret_t WPL_Leave(void)
{
    wpl_ret_t status = WPLRET_SUCCESS;
    int ret;
    EventBits_t syncBit;

    if (s_wplState != WPL_STARTED)
    {
        status = WPLRET_NOT_READY;
    }

    enum wlan_connection_state connection_state = WLAN_DISCONNECTED;
    wlan_get_connection_state(&connection_state);
    if (connection_state == WLAN_DISCONNECTED)
    {
        s_wplStaConnected = false;
        return WPLRET_SUCCESS;
    }

    if (status == WPLRET_SUCCESS)
    {
        xEventGroupClearBits(s_wplSyncEvent, WPL_SYNC_DISCONNECT_GROUP);

        ret = wlan_disconnect();
        if (ret != WM_SUCCESS)
        {
            status = WPLRET_FAIL;
        }
    }

    if (status == WPLRET_SUCCESS)
    {
        syncBit = xEventGroupWaitBits(s_wplSyncEvent, WPL_SYNC_DISCONNECT_GROUP, pdTRUE, pdFALSE, WPL_SYNC_TIMEOUT_MS);
        if (syncBit & EVENT_BIT(WLAN_REASON_USER_DISCONNECT))
        {
            status = WPLRET_SUCCESS;
        }
        else
        {
            status = WPLRET_TIMEOUT;
        }
    }

    s_wplStaConnected = false;

    return status;
}

wpl_ret_t WPL_GetIP(char *ip, int client)
{
    wpl_ret_t status = WPLRET_SUCCESS;
    int ret;
    struct wlan_ip_config addr;

    if (ip == NULL)
    {
        status = WPLRET_FAIL;
    }

    if (status == WPLRET_SUCCESS)
    {
        if (client)
        {
            ret = wlan_get_address(&addr);
        }
        else
        {
            ret = wlan_get_uap_address(&addr);
        }

        if (ret == WM_SUCCESS)
        {
            net_inet_ntoa(addr.ipv4.address, ip);
        }
        else
        {
            status = WPLRET_FAIL;
        }
    }

    return status;
}

bool WIFI_get_ap_ssid_f(unsigned int _ui_index, unsigned char *_puc_ssid, int _i_ssid_length)
{
    bool b_return;
    unsigned int ui_copy_length;
    if ((_ui_index < countAP) && (_i_ssid_length > 0))
    {
        b_return = true;

        int ap_ssid_length = scan_result.ssid_len;
        if (_i_ssid_length >= ap_ssid_length + 1) // buffer is large enough
        {
            ui_copy_length = ap_ssid_length;
        }
        else // buffer is not big enough
        {
            ui_copy_length = _i_ssid_length - 1;
        }

        memcpy(_puc_ssid, scan_result.ssid, ui_copy_length);
        _puc_ssid[ui_copy_length] = 0;
    }
    else
    {
        b_return = false;
    }

    return b_return;
}

bool WIFI_get_ap_bssid_f(unsigned int _ui_index, unsigned char *_puc_bssid, int _i_bssid_length)
{
    bool b_return;

    if ((_ui_index < countAP) && (_i_bssid_length > 0))
    {
        wlan_get_scan_result(_ui_index, &scan_result);
        int copy_length;

        b_return = true;

        if ((unsigned int)_i_bssid_length < sizeof(scan_result.bssid))
        {
            copy_length = _i_bssid_length;
        }
        else
        {
            copy_length = sizeof(scan_result.bssid);
        }

        strncpy((char *)_puc_bssid, (char *)scan_result.bssid, (size_t)copy_length);
    }
    else
    {
        b_return = false;
    }

    return b_return;
}

bool WIFI_get_ap_channel_f(unsigned int _ui_index, int *_pi_channel)
{
    bool b_return;

    if (_ui_index < countAP)
    {
        wlan_get_scan_result(_ui_index, &scan_result);
        b_return = true;
        PRINTF("channel is %d",scan_result.channel );
        *_pi_channel = scan_result.channel;
    }
    else
    {
        b_return = false;
    }

    return b_return;
}

bool WIFI_get_ap_rssi_f(unsigned int _ui_index, float *_pf_rssi, int _i_rssi_length)
{
    bool b_return;
    if ((_ui_index < countAP) && (_i_rssi_length > 0))
    {
        wlan_get_scan_result(_ui_index, &scan_result);
        b_return = true;
        *_pf_rssi = scan_result.rssi;
    }
    else
    {
        b_return = false;
    }

    return b_return;
}

unsigned int get_scan_count (void)
{
    return countAP;
}


bool WIFI_get_client_state_f(void)
{
    if (s_wplStaConnected)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool WIFI_get_access_point_state_f(void)
{
    if (s_wplUapActivated)
    {
        return true;
    }
    else
    {
        return false;
    }
}

static int convert_imx_to_llnet_security_mode_f(void)
{
    int _pi_llnet_security_mode;

    if (scan_result.wpa2_entp)
    {
        _pi_llnet_security_mode = SECURITY_MODE_ENTERPRISE_WPA2;
    }
    if (scan_result.wep)
    {
        _pi_llnet_security_mode = SECURITY_MODE_WEP64;
    }
    if (scan_result.wpa)
    {
        _pi_llnet_security_mode = SECURITY_MODE_WPA1;
    }
    if (scan_result.wpa2)
    {
        _pi_llnet_security_mode = SECURITY_MODE_WPA3;
    }
    else
    {
    _pi_llnet_security_mode = SECURITY_MODE_OPEN;
    }
    return _pi_llnet_security_mode;
}


static int convert_sec_to_llnet_security_mode_f(void)
{
    int _pi_llnet_security_mode;
    switch (sta_network.security.type)
    {
    case WLAN_SECURITY_NONE:
        _pi_llnet_security_mode = SECURITY_MODE_OPEN;
        break;

    case WLAN_SECURITY_WPA:
        _pi_llnet_security_mode = SECURITY_MODE_WPA1;
        break;

    case WLAN_SECURITY_WPA2:
        _pi_llnet_security_mode = SECURITY_MODE_WPA2;
        break;

    case WLAN_SECURITY_WPA_WPA2_MIXED:
        _pi_llnet_security_mode = SECURITY_MODE_MIXED;
        break;
    case WLAN_SECURITY_WILDCARD:
        _pi_llnet_security_mode = SECURITY_MODE_WPA3;
        break;
    default:
        _pi_llnet_security_mode = SECURITY_MODE_UNKNOWN;
        break;
    }

    return _pi_llnet_security_mode;
}

bool WIFI_get_ap_security_mode_f(unsigned int _ui_index, int * _pi_mode)
{
    bool b_return;    
    if (_ui_index < countAP)
    {
        wlan_get_scan_result(_ui_index, &scan_result);
        int security_mode = convert_imx_to_llnet_security_mode_f();
        b_return = 0 <= security_mode;
        if (false != b_return)
        {
            *_pi_mode = security_mode;
        }
    }
    else
    {
        b_return = false;
    }
    return b_return;
}

bool WIFI_set_name_softap_f(const char *_pc_name, int _i_name_length)
{
    strncpy(_pc_name, sta_network.name ,_i_name_length);
    return true;
}


bool WIFI_get_bssid_f(unsigned char *_puc_bssid, int _i_bssid_length)
{
    bool bret = false;
    uint8_t ap_addr[_i_bssid_length];
    /* Store BSSID information */
    int ret = WM_FAIL;
    ret = wlan_get_current_bssid(ap_addr);
    strncpy(_puc_bssid, ap_addr, _i_bssid_length);
    if (ret == WM_SUCCESS )
    {
        bret = true;
    }
    return bret;
}

bool WIFI_get_channel_f(short *_ps_channel)
{
    bool bRet = false;
    /* Store channel information */
    *_ps_channel = wlan_get_current_channel();
    if(*_ps_channel > 0)
    {
        bRet = true;
    }
    return bRet;
}

bool WIFI_get_rssi_f(float *_pf_rssi, int _i_rssi_length)
{
   int ret = WM_FAIL;
   bool result = false;
   /* Store RSSI information */
   float ap_addr[_i_rssi_length];
   ret = wlan_get_current_rssi(ap_addr);
   if(ret == WM_SUCCESS)
   {
        result = true;
   }
    return result ;
}

int WIFI_get_security_mode_f(void)
{
    int security_mode = convert_sec_to_llnet_security_mode_f();
    return security_mode;
}

bool WIFI_get_ssid_f(char *_pc_ssid, int _i_ssid_length)
{
    strncpy(_pc_ssid, sta_network.ssid, strlen(sta_network.ssid));
    return true;
}

bool WIFI_get_wps_modes_f(short *_ps_wps_modes)
{
    /* Not implemented */
    *_ps_wps_modes = WPS_MODE_NONE;
     return true;
}