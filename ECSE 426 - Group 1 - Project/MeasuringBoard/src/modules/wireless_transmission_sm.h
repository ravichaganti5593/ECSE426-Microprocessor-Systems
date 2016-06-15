#ifndef WIRELESS_TRANSMISSION_H
#define WIRELESS_TRANSMISSION_H

#include "stm32f4xx.h"

//Various states of the SFM
#define WIRELESS_TRANSMISSION_STATE_IDLE       0
#define WIRELESS_TRANSMISSION_STATE_RECEIVE    1
#define WIRELESS_TRANSMISSION_STATE_TRANSMIT   2
#define WIRELESS_TRANSMISSION_STATE_ERROR      3

//Maximum packet size (including headers and checksum)
#define WIRELESS_TRANSMISSION_PACKET_SIZE           200

//Checksum validation result
#define WIRELESS_TRANSMISSION_VERIFY_OK   					1
#define WIRELESS_TRANSMISSION_VERIFY_INCORRECT_CHECKSUM		2
#define WIRELESS_TRANSMISSION_VERIFY_INCORRECT_LENGTH		3
#define WIRELESS_TRANSMISSION_VERIFY_INVALID_CHECK_SUM		4
#define WIRELESS_TRANSMISSION_VERIFY_INVALID_CODEC			5

/**
 * Reserved bytes used to signal start and end of packet
 */
extern const uint8_t START_PACKET;
extern const uint8_t END_PACKET;

/**
 * Information about a received packet.
 * Client must ensure that status is OK before doing any operation on other fields
 * If passed checksum, packet content will be copied into buffer. The length of the received packet will also be populated
 */
typedef struct {
	uint8_t status;
	uint8_t len;
	uint8_t* buffer;
} wireless_received_packet;

/**
 * Module initialization
 */
void wireless_transmission_init(void);

/**
 * Retrieve current state of the internal SFM
 */
uint8_t wireless_transmission_get_state(void);

/**
 * SFM main operation
 */
void wireless_transmission_periodic(void);

/**
 * Start SFM to send a packet to the air.
 * WARNING: ALL BYTES IN THE PACKET MUST BE DIFFERENT FROM START_PACKET AND END_PACKET
 * If any byte in the packet is START_PACKET or END_PACKET, the receiver will not be able to receive
 */
uint8_t wireless_transmission_transmit(uint8_t* packet, uint8_t len);

/**
 * Start SFM to resend a packet to the air.
 */
uint8_t wireless_transmission_retransmit(void);

/**
 * Start SFM to receive packet from the air
 */
void wireless_transmission_receive_packet(void);

/**
 * Retrieve information about the received packet
 */
void wireless_transmission_get_received_packet(wireless_received_packet* received_packet);

#endif
