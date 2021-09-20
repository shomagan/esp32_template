#ifndef UDP_BROADCAST_H
#define UDP_BROADCAST_H 1

#include "type_def.h"
#include "main_config.h"
#define UDP_BROADCAST_SELF_PORT 65500
#define UDP_BROADCAST_MAX_PACKET_SIZE 512
#define UDP_BROADCAST_ADV_TIME_MS     60000
/**
 * @brief udp_broadcast_init
 * @return non zero value if error occured
 */
int udp_broadcast_init(void);
/**
 * @brief udp_broadcast_advertisement send broadcast message to wire
 * @return
 */
int udp_broadcast_advertisement(void);

/**
 * @brief udp_broadcast_deinit
 * @return non zero value if error occured
 */
int udp_broadcast_deinit(void);
#endif  //UDP_BROADCAST_H
