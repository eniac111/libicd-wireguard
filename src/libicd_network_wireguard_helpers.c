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

#include "libicd_network_wireguard.h"

/* XXX: Taken from ipv4 module */
gboolean string_equal(const char *a, const char *b)
{
	if (!a)
		return !b;

	if (b)
		return !strcmp(a, b);

	return FALSE;
}

wireguard_network_data *icd_wireguard_find_first_network_data(network_wireguard_private * private)
{
	GSList *l;

	for (l = private->network_data_list; l; l = l->next) {
		wireguard_network_data *found = (wireguard_network_data *) l->data;

		if (!found)
			WN_WARN("wireguard network data is NULL");
		else {
			return found;
		}
	}

	return NULL;
}

wireguard_network_data *icd_wireguard_find_network_data(const gchar * network_type,
							guint network_attrs,
							const gchar * network_id, network_wireguard_private * private)
{
	GSList *l;

	for (l = private->network_data_list; l; l = l->next) {
		wireguard_network_data *found = (wireguard_network_data *) l->data;

		if (!found)
			WN_WARN("wireguard network data is NULL");
		else {
			if (found->network_attrs == network_attrs &&
			    string_equal(found->network_type, network_type) &&
			    string_equal(found->network_id, network_id)) {
				return found;
			}
		}
	}

	return NULL;
}

/* pathname and arg are like in execv, returns pid, 0 is error */
pid_t spawn_as(const char *username, const char *pathname, char *args[])
{
	struct passwd *ent = getpwnam(username);
	if (ent == NULL) {
		WN_CRIT("spawn_as: getpwnam failed\n");
		return 0;
	}

	pid_t pid = fork();
	if (pid < 0) {
		WN_CRIT("spawn_as: fork() failed\n");
		return 0;
	} else if (pid == 0) {
		if (setgid(ent->pw_gid)) {
			WN_CRIT("setgid failed\n");
			exit(1);
		}
		if (setuid(ent->pw_uid)) {
			WN_CRIT("setuid failed\n");
			exit(1);
		}
		execv(pathname, args);

		WN_CRIT("execv failed\n");
		exit(1);
	} else {
		WN_DEBUG("spawn_as got pid: %d\n", pid);
		return pid;
	}

	return 0;		// Failure
}

void network_free_all(wireguard_network_data * network_data)
{
	network_wireguard_private *priv = network_data->private;
	if (priv->network_data_list) {
		priv->network_data_list = g_slist_remove(priv->network_data_list, network_data);
	}

	g_free(network_data->network_type);
	g_free(network_data->network_id);

	network_data->private = NULL;

	g_free(network_data);
}

void network_stop_all(wireguard_network_data * network_data)
{
	char *argss[] = { "/usr/bin/wg-quick", "down", "icdwg0", NULL };
	pid_t pid = spawn_as("root", "/usr/bin/wg-quick", argss);
	if (pid == 0) {
		WN_WARN("Failed to attempt to stop Wireguard\n");
		return;
	}
}

int startup_wireguard(wireguard_network_data * network_data, char *config)
{
	/* const char* config_filename = "/etc/wireguard/icdwg0.conf" */
	/* char *config_content = generate_config(config); */
	/* GError *error = NULL; */

	/* TODO: Not actually writing the file yet */
#if 0
	g_file_set_contents(config_filename, config_content, strlen(config_content), &error);
	if (error != NULL) {
		g_clear_error(&error);
		WN_WARN("Unable to write Wireguard config file\n");
		return 1;
	}
#endif

	char *argss[] = { "/usr/bin/wg-quick", "up", "icdwg0", NULL };
	pid_t pid = spawn_as("root", "/usr/bin/wg-quick", argss);
	if (pid == 0) {
		WN_WARN("Failed to start Wireguard\n");
		return 1;
	}

	WN_INFO("Got wg_quick_pid: %d\n", pid);
	network_data->wg_quick_pid = pid;
	network_data->private->watch_cb(pid, network_data->private->watch_cb_token);

	return 0;
}
