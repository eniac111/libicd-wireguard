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
#include <glib.h>

#include "libicd_wireguard.h"
#include "libicd_network_wireguard.h"

#include <asm/types.h>
#include <sys/socket.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <net/if.h>
#include <errno.h>

static int read_event(int sockint, char **iface_name, int *iface_status, int *iface_index)
{
	int status;
	int ret = 0;
	struct nlmsghdr *header;
	struct ifinfomsg *info;

	char buf[4096];
	struct iovec iov = { buf, sizeof buf };

	struct sockaddr_nl snl;
	struct msghdr msg = { (void *)&snl, sizeof snl, &iov, 1, NULL, 0, 0 };

	status = recvmsg(sockint, &msg, 0);

	if (status < 0) {
		if (errno == EWOULDBLOCK || errno == EAGAIN) {
			return ret;
		}

		return status;
	}

	if (status == 0) {
		/* TODO: deal with EOF? */
	}

	for (header = (struct nlmsghdr *)buf; NLMSG_OK(header, (unsigned int)status);
	     header = NLMSG_NEXT(header, status)) {
		if (header->nlmsg_type == NLMSG_DONE)
			return ret;

		if (header->nlmsg_type == NLMSG_ERROR) {
			return -1;
		}

		if (header->nlmsg_type == RTM_NEWLINK) {
			info = NLMSG_DATA(header);

			*iface_index = info->ifi_index;
			*iface_status = (info->ifi_flags & IFF_RUNNING) ? 1 : 0;

			*iface_name = malloc(IF_NAMESIZE);
			if (if_indextoname(info->ifi_index, *iface_name) == 0) {
				fprintf(stderr, "if_indextoname failed\n");
				free(*iface_name);
				*iface_name = NULL;

				return 0;
			}
		}
	}

	return ret;
}

static gboolean netlink_cb(GIOChannel * chan, GIOCondition cond, gpointer data)
{
	network_wireguard_private *priv = data;
	fprintf(stderr, "netlink_cb\n");

	int fd;
	int state = 0;
	int index = 0;
	char *iface = NULL;

	fd = g_io_channel_unix_get_fd(chan);

	/* TODO: figure out this logic some more, wrt how many times to call
	 * read_event */
	while (1) {
		int ret = read_event(fd, &iface, &state, &index);
		fprintf(stderr, "read_event: %d\n", ret);

		if (state == 0 && index == priv->state.wireguard_interface_index && priv->state.wireguard_interface_up) {
			network_wireguard_state new_state;
			memcpy(&new_state, &priv->state, sizeof(network_wireguard_state));
			new_state.wireguard_interface_up = FALSE;
			new_state.wireguard_interface_index = -1;
			new_state.wireguard_running = FALSE;
			wireguard_state_change(priv, NULL, new_state, EVENT_SOURCE_WIREGUARD_DOWN);

		} else if (iface) {
			fprintf(stderr, "iface: %s (%d), status: %d\n", iface, index, state);

			if (strcmp("icdwg0", iface) == 0) {
				fprintf(stderr, "wireguard_interface_up: %d\n", priv->state.wireguard_interface_up);

				/* We check for the wireguard up state here, because I have not
				 * figured out how to differentiate between interface created
				 * and interface down, and when we go up, we get created (which
				 * I see as down) and then up. */
				if (state == 1) {
					network_wireguard_state new_state;
					memcpy(&new_state, &priv->state, sizeof(network_wireguard_state));
					new_state.wireguard_interface_up = TRUE;
					new_state.wireguard_interface_index = index;
					wireguard_state_change(priv, NULL, new_state, EVENT_SOURCE_WIREGUARD_UP);
				} else if (state == 0 && priv->state.wireguard_interface_up) {
					network_wireguard_state new_state;
					memcpy(&new_state, &priv->state, sizeof(network_wireguard_state));
					new_state.wireguard_interface_up = FALSE;
					new_state.wireguard_interface_index = -1;
					new_state.wireguard_running = FALSE;
					wireguard_state_change(priv, NULL, new_state, EVENT_SOURCE_WIREGUARD_DOWN);
				}
			}

			free(iface);
			iface = NULL;
		}

		if (!ret)
			break;

	}

	return TRUE;
}

int open_netlink_listener(void *user_data)
{
	GIOChannel *io;

	struct sockaddr_nl addr;
	int fd;
	int event_id;

	fd = socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE);
	if (fd < 0) {
		/* TODO: socket creation failed */
	}

	memset(&addr, 0, sizeof(addr));
	addr.nl_family = AF_NETLINK;
	addr.nl_groups = RTMGRP_LINK;

	if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
		/* TODO: bind() failed */
	}

	io = g_io_channel_unix_new(fd);
	event_id = g_io_add_watch(io, G_IO_IN | G_IO_ERR | G_IO_HUP, netlink_cb, user_data);

	(void)event_id;

	// TODO
	return 0;
}

void close_netlink_listener(void)
{
	// TODO
}
