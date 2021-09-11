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

#include <stdio.h>

#include "libicd_wireguard.h"
#include "dbus_wireguard.h"
#include "libicd_network_wireguard.h"

struct wireguard_method_callbacks {
	const gchar *method_name;
	DBusHandleMessageFunction call;
};

static DBusHandlerResult error_callback(DBusConnection * connection,
					DBusMessage * message, void *user_data);

static struct wireguard_method_callbacks callbacks[] = {
	{"Start", &start_callback},
	{"Stop", &stop_callback},
	{"GetStatus", &getstatus_callback},

	{NULL,}
};

/**
 * Receive registered method calls and find a handler for them
 *
 * @param connection  D-Bus connection
 * @param message     D-Bus message
 * @param user_data   dbus api data structure
 */
static DBusHandlerResult wireguard_icd_dbus_api_request(DBusConnection * connection,
						  DBusMessage * message,
						  void *user_data)
{
	if (dbus_message_get_type(message) != DBUS_MESSAGE_TYPE_METHOD_CALL) {
		return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
	}

	WN_DEBUG("ICD2 Wireguard dbus api request\n");

	const char *member = dbus_message_get_member(message);

	int i = 0;

	while (callbacks[i].method_name != NULL) {
		if (strcmp(member, callbacks[i].method_name) == 0) {
			WN_DEBUG("Match for method %s", member);
			return callbacks[i].call(connection, message,
						 user_data);
		}

		i++;
	}
	return error_callback(connection, message, user_data);
}

static DBusHandlerResult error_callback(DBusConnection * connection,
					DBusMessage * message, void *user_data)
{
	DBusMessage *err_msg;

	WN_INFO("received '%s.%s' request has no handler implemented",
		  dbus_message_get_interface(message),
		  dbus_message_get_member(message));

	err_msg = dbus_message_new_error(message, DBUS_ERROR_NOT_SUPPORTED,
					 "Unsupported interface or method");
	icd_dbus_send_system_msg(err_msg);
	dbus_message_unref(err_msg);

	return DBUS_HANDLER_RESULT_HANDLED;
}

int setup_wireguard_dbus(void *user_data)
{
	WN_DEBUG("Registering ICD2 Wireguard dbus service");
	if (icd_dbus_register_system_service(ICD_WIREGUARD_DBUS_PATH,
					     ICD_WIREGUARD_DBUS_INTERFACE,
					     DBUS_NAME_FLAG_REPLACE_EXISTING |
					     DBUS_NAME_FLAG_DO_NOT_QUEUE,
					     wireguard_icd_dbus_api_request,
					     user_data) == FALSE) {
		WN_ERR("Failed to register DBUS interface\n");
		return 1;
	}
	WN_DEBUG("Successfully registered ICD2 Wireguard dbus service");

	return 0;
}

int free_wireguard_dbus(void)
{
	icd_dbus_unregister_system_service(ICD_WIREGUARD_DBUS_PATH,
					   ICD_WIREGUARD_DBUS_INTERFACE);
	return 0;
}
