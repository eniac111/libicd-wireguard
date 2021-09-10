#ifndef __LIBICD_WIREGUARD_H
#define __LIBICD_WIREGUARD_H

#include <glib.h>
#include "libicd_wireguard_shared.h"

gboolean config_is_known(const char* config_name);
gboolean network_is_wireguard_provider(const char* network_id, char **ret_gconf_service_id);
gboolean get_system_wide_enabled(void);
char *generate_config(const char *config_name);
char *get_active_config(void);

#define WN_DEBUG(fmt, ...) ILOG_DEBUG(("[WIREGUARD NETWORK] "fmt), ##__VA_ARGS__)
#define WN_INFO(fmt, ...) ILOG_INFO(("[WIREGUARD NETWORK] " fmt), ##__VA_ARGS__)
#define WN_WARN(fmt, ...) ILOG_WARN(("[WIREGUARD NETWORK] %s.%d:" fmt), __func__, __LINE__, ##__VA_ARGS__)
#define WN_ERR(fmt, ...) ILOG_ERR(("[WIREGUARD NETWORK] %s.%d:" fmt), __func__, __LINE__, ##__VA_ARGS__)
#define WN_CRIT(fmt, ...) ILOG_CRIT(("[WIREGUARD NETWORK] %s.%d:" fmt), __func__, __LINE__, ##__VA_ARGS__)


#endif				/* __LIBICD_WIREGUARD_H */
