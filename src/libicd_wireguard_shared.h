/*
 * This file is part of libicd-wireguard
 *
 * Copyright (C) 2021, Merlijn Wajer <merlijn@wizzup.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * version 3.0 as published by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 */

#ifndef __LIBICD_WIREGUARD_SHARED_H
#define __LIBICD_WIREGUARD_SHARED_H

#define WIREGUARD_NETWORK_TYPE "WIREGUARD"
#define WIREGUARD_PROVIDER_TYPE "WIREGUARD"
#define WIREGUARD_PROVIDER_NAME "Wireguard Provider"

#define WIREGUARD_DEFAULT_SERVICE_ATTRIBUTES 0
#define WIREGUARD_DEFAULT_SERVICE_PRIORITY 0

#define GC_WIREGUARD "/system/osso/connectivity/providers/wireguard"
#define GC_ICD_WIREGUARD_AVAILABLE_IDS "/system/osso/connectivity/srv_provider/WIREGUARD/available_ids"

#define GC_NETWORK_TYPE "/system/osso/connectivity/network_type/WIREGUARD"
#define GC_WIREGUARD_ACTIVE  GC_NETWORK_TYPE"/active_config"
#define GC_WIREGUARD_SYSTEM  GC_NETWORK_TYPE"/system_wide_enabled"

#define GC_CFG_DNS           "DNS"
#define GC_CFG_PRIVATEKEY    "PrivateKey"
#define GC_CFG_ADDRESS       "Address"

#define GC_CONFIG_FILE_OVERRIDE "config_file_override"
#define GC_SYSTUNNEL     "systemtunnel-enabled"
#define GC_ADDRESS       "Address"
#define GC_DNS           "DNS"
#define GC_PRIVATEKEY    "PrivateKey"
#define GC_PEERS         "peers"
#define GC_PEER_IPS      "AllowedIPs"
#define GC_PEER_ENDPOINT "EndPoint"
#define GC_PEER_PUBKEY   "PublicKey"
#define GC_PEER_PSK      "PresharedKey"

#define ICD_WIREGUARD_DBUS_INTERFACE "org.maemo.Wireguard"
#define ICD_WIREGUARD_DBUS_PATH "/org/maemo/Wireguard"

#define ICD_WIREGUARD_METHOD_GETSTATUS ICD_WIREGUARD_DBUS_INTERFACE".GetStatus"

#define ICD_WIREGUARD_SIGNAL_STATUSCHANGED      "StatusChanged"
#define ICD_WIREGUARD_SIGNAL_STATUSCHANGED_FILTER "member='" ICD_WIREGUARD_SIGNAL_STATUSCHANGED "'"

#define ICD_WIREGUARD_SIGNALS_STATUS_STATE_CONNECTED "Connected"
#define ICD_WIREGUARD_SIGNALS_STATUS_STATE_STARTED "Started"
#define ICD_WIREGUARD_SIGNALS_STATUS_STATE_STOPPED "Stopped"

#define ICD_WIREGUARD_SIGNALS_STATUS_MODE_NORMAL "Normal"
#define ICD_WIREGUARD_SIGNALS_STATUS_MODE_PROVIDER "Provider"

enum WIREGUARD_DBUS_METHOD_START_RESULT {
	WIREGUARD_DBUS_METHOD_START_RESULT_OK,
	WIREGUARD_DBUS_METHOD_START_RESULT_FAILED,
	WIREGUARD_DBUS_METHOD_START_RESULT_INVALID_CONFIG,
	WIREGUARD_DBUS_METHOD_START_RESULT_INVALID_ARGS,
	WIREGUARD_DBUS_METHOD_START_RESULT_ALREADY_RUNNING,
	WIREGUARD_DBUS_METHOD_START_RESULT_REFUSED,
};

enum WIREGUARD_DBUS_METHOD_STOP_RESULT {
	WIREGUARD_DBUS_METHOD_STOP_RESULT_OK,
	WIREGUARD_DBUS_METHOD_STOP_RESULT_NOT_RUNNING,
	WIREGUARD_DBUS_METHOD_STOP_RESULT_REFUSED,
};

#endif				/* __LIBICD_WIREGUARD_SHARED_H */
