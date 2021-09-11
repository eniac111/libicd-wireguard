MAINTAINERCLEANFILES = \
	Makefile.in

INCLUDES = \
	@GLIB_CFLAGS@ \
	@GCONF_CFLAGS@ \
	@ICD2_CFLAGS@ \
	@OSSO_IC_DEV_CFLAGS@

LDFLAGS = -avoid-version

plugindir = @ICD_PLUGIN_DIR@

plugin_LTLIBRARIES = \
	libicd_provider_wireguard.la \
	libicd_network_wireguard.la

libicd_provider_wireguard_la_SOURCES = \
	libicd_provider_wireguard.c \
	libicd_wireguard_config.c \
	libicd_wireguard.h

libicd_network_wireguard_la_SOURCES = \
	libicd_network_wireguard.c \
	libicd_network_wireguard_helpers.c \
	libicd_network_wireguard_dbus.c \
	libicd_network_wireguard_netlink.c \
	libicd_network_wireguard.h \
	dbus_wireguard.c \
	dbus_wireguard.h \
	libicd_wireguard_config.c \
	libid_wireguard_shared.h \
	libicd_wireguard.h
