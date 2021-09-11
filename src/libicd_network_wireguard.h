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

#ifndef __LIBICD_NETWORK_TOR_H
#define __LIBICD_NETWORK_TOR_H
#include <string.h>
#include <stdio.h>
#include <glib.h>
#include <pwd.h>

#include <gconf/gconf-client.h>
#include <dbus/dbus-glib-lowlevel.h>
#include <osso-ic-gconf.h>
#include <network_api.h>

#include "icd/support/icd_log.h"

#include "dbus_wireguard.h"
#include "libicd_wireguard.h"

struct _network_wireguard_state {
	/* State data here, since without IAP we do not have wireguard_network_data */
	gboolean system_wide_enabled;
	gchar *active_config;
	gboolean iap_connected;
	gboolean service_provider_mode;

	gboolean wg_quick_running;
	gboolean wireguard_running;
	gboolean wireguard_up;

	/* We only have this status atm for handling netlink data, since I could not
	 * parse it properly*/
	gboolean wireguard_interface_up;
	gint wireguard_interface_index;

	gboolean gconf_transition_ongoing;

	gboolean dbus_failed_to_start;
#if 0
	gboolean network_is_tor_service_provider;
#endif
#if 0
	gboolean manual_start_requested;
	gboolean manual_stop_requested;
#endif
};
typedef struct _network_wireguard_state network_wireguard_state;

struct _network_wireguard_private {
	/* For pid monitoring */
	icd_nw_watch_pid_fn watch_cb;
	gpointer watch_cb_token;

	icd_nw_close_fn close_cb;

#if 0
	icd_srv_limited_conn_fn limited_conn_fn;
#endif

	GSList *network_data_list;

	GConfClient *gconf_client;
	guint gconf_cb_id_systemwide;

	network_wireguard_state state;
};
typedef struct _network_wireguard_private network_wireguard_private;

struct _wireguard_network_data {
	network_wireguard_private *private;

	icd_nw_ip_up_cb_fn ip_up_cb;
	gpointer ip_up_cb_token;

	icd_nw_ip_down_cb_fn ip_down_cb;
	gpointer ip_down_cb_token;

	/* Tor pid */
	pid_t wg_quick_pid;

	/* For matching / callbacks later on (like close and limited_conn callback) */
	gchar *network_type;
	guint network_attrs;
	gchar *network_id;
};
typedef struct _wireguard_network_data wireguard_network_data;

gboolean icd_nw_init(struct icd_nw_api *network_api,
		     icd_nw_watch_pid_fn watch_fn, gpointer watch_fn_token,
		     icd_nw_close_fn close_fn, icd_nw_status_change_fn status_change_fn, icd_nw_renew_fn renew_fn);

void wireguard_state_change(network_wireguard_private * private, wireguard_network_data * network_data,
			    network_wireguard_state new_state, int source);

/* Helpers */
void network_stop_all(wireguard_network_data * network_data);
void network_free_all(wireguard_network_data * network_data);
pid_t spawn_as(const char *username, const char *pathname, char *args[]);
wireguard_network_data *icd_wireguard_find_first_network_data(network_wireguard_private * private);
wireguard_network_data *icd_wireguard_find_network_data(const gchar * network_type,
							guint network_attrs,
							const gchar * network_id, network_wireguard_private * private);
gboolean string_equal(const char *a, const char *b);
int startup_wireguard(wireguard_network_data * network_data, char *config);

enum icd_wireguard_event_source_type {
	EVENT_SOURCE_IP_UP,
	EVENT_SOURCE_IP_DOWN,
	EVENT_SOURCE_GCONF_CHANGE,
	EVENT_SOURCE_WIREGUARD_UP,
	EVENT_SOURCE_WIREGUARD_DOWN,
	EVENT_SOURCE_WIREGUARD_QUICK_PID_EXIT,
	EVENT_SOURCE_DBUS_CALL_START,
	EVENT_SOURCE_DBUS_CALL_STOP,
};

/* DBus methods */
DBusHandlerResult start_callback(DBusConnection * connection, DBusMessage * message, void *user_data);
DBusHandlerResult stop_callback(DBusConnection * connection, DBusMessage * message, void *user_data);
DBusHandlerResult getstatus_callback(DBusConnection * connection, DBusMessage * message, void *user_data);
void emit_status_signal(network_wireguard_state state);

int open_netlink_listener(void *user_data);
void close_netlink_listener(void);

#endif
