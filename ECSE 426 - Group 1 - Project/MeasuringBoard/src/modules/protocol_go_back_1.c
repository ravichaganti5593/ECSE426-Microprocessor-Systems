#include <string.h>
#include <stdio.h>
#include "protocol_go_back_1.h"

#include "modules/wireless_transmission_sm.h"
#include "my_types.h"
#include "../utils/utils.h"

static const uint8_t MAX_TIMEOUT = 100;
static const uint8_t MIN_ACK_COUNT = 3;
static const uint8_t BASE_ID = 1; //This has to not cover the START_PACKET and END_PACKET signal
static const uint8_t ID_LEN = 1;

static uint8_t receive_buffer[WIRELESS_TRANSMISSION_PACKET_SIZE];
static wireless_received_packet received_packet;

static uint8_t mode;
static uint8_t state;
static uint8_t current_id, new_packet;
static uint8_t timeout_count;
static uint8_t ack_left;

/********************************************************************************************************************/
/******************************************Private methods **********************************************************/
/********************************************************************************************************************/
/**
 * Get the next id for the packet to send.
 * Result ranges from BASE_ID to 250
 *
 * @param void
 *
 * @return next id that can be used to send a packet with
 */
static uint8_t next_id(void) {
	static uint8_t id_count = BASE_ID;
	id_count = (id_count + 1 == 250) ? BASE_ID : id_count + 1;
	current_id = id_count;
	return id_count;
}

/**
 * Consider retransmit by counting down.
 * If count down reaches zero, then we should retransmit the packet.
 *
 * @param void
 *
 * @return TRUE if should retransmit, and FALSE if not
 */
static uint8_t consider_retransmit(void) {
	timeout_count--;
	if (timeout_count == 0) {
		wireless_transmission_retransmit();
		state = GO_BACK_ONE_SENDER_STATE_SEND;
		timeout_count = MAX_TIMEOUT;
		return TRUE;
	}
	return FALSE;
}

/**
 * Ask the underlying module to transmit an ACK
 * An ACK is simply a packet of 1 byte: the ID of the received packet
 *
 * @param void
 *
 * @return void
 */
static void transmit_ack(void) {
	static uint8_t ack_packet[1];
	ack_packet[0] = current_id;
	wireless_transmission_transmit(ack_packet, 1);
}

/********************************************************************************************************************/
/******************************************Main API *****************************************************************/
/********************************************************************************************************************/
/**
 * Initialize SFM of this module
 *
 * @param operation_mode 	Operation mode of the module (see header for choices)
 *
 * @return void
 */
void protocol_go_back_1_init(uint8_t operation_mode) {
	wireless_transmission_init();

	received_packet.buffer = receive_buffer;
	received_packet.len = 0;
	mode = operation_mode;

	current_id = 255;

	if (mode == GO_BACK_ONE_MODE_SENDER) {
		state = GO_BACK_ONE_SENDER_STATE_IDLE;
	} else {
		state = GO_BACK_ONE_RECEIVER_STATE_IDLE;
	}
}

/**
 * Retrieve current state of the internal SFM
 *
 * @param void
 *
 * @return current state of the internal SFM
 */
uint8_t protocol_go_back_1_get_state(void) {
	return state;
}

/**
 * Retrieve the received packet
 *
 * @param dest	pointer to the buffer where the received packet will be written to
 *
 * @return length of the received packet.
 */
uint8_t protocol_go_back_1_get_received_data(uint8_t* dest) {
	memcpy(dest, receive_buffer + 1, WIRELESS_TRANSMISSION_PACKET_SIZE);

	if (new_packet == TRUE) {
		return received_packet.len == 0 ? 0 : received_packet.len - 1;	
	} else {
		return 0;
	}
}

/**
 * Tell SFM to start receiving packet.
 *
 * @param void
 *
 * @return void
 */
void protocol_go_back_1_receive(void) {
	if (mode != GO_BACK_ONE_MODE_RECEIVER) {
		return;
	}

	new_packet = FALSE;
	state = GO_BACK_ONE_RECEIVER_STATE_RECEIVE;
	ack_left = MIN_ACK_COUNT;
	memset(receive_buffer, 0, WIRELESS_TRANSMISSION_PACKET_SIZE);
	wireless_transmission_receive_packet();
}

/**
 * Tell SFM to transmit a packet.
 *
 * @param packet 	pointer to the packet buffer
 * @param len 		length of the packet
 *
 * WARNING: PACKET BUFFER MUST HAVE ID_LEN WRITABLE BYTES AT THE FRONT.
 * NOT HAVING WRITABLE BYTES AT THE FRONT MAY LEAD TO WRITING TO UNALLOCATED REGIONS OF MEMORY
 *
 * @return void
 */
void protocol_go_back_1_send(uint8_t* packet, uint8_t len) {
	if (mode != GO_BACK_ONE_MODE_SENDER) {
		return;
	}

	*(packet - ID_LEN) = next_id();
	uint8_t transmitted = wireless_transmission_transmit(packet - ID_LEN, len + ID_LEN);
	if (!transmitted) {
		state = GO_BACK_ONE_STATE_ERROR;
		return;
	}
	timeout_count = MAX_TIMEOUT;
	state = GO_BACK_ONE_SENDER_STATE_SEND;
}

/********************************************************************************************************************/
/******************************************SFM logics ***************************************************************/
/********************************************************************************************************************/
/**
 * SFM for sender
 *
 * 1) Send packet
 * 2) Wait for ACK
 * 3) If no ACK, wait for ACK and consider retransmit (count down)
 * 4) If timeout on ACK, retransmit (back to step 1)
 *
 * @param void
 *
 * @return void
 */
static void protocol_go_back_1_periodic_sender(void) {
	uint8_t lower_layer_state = wireless_transmission_get_state();
	if (lower_layer_state == WIRELESS_TRANSMISSION_STATE_ERROR) {
		state = GO_BACK_ONE_STATE_ERROR;
		return;
	}

	if (state == GO_BACK_ONE_SENDER_STATE_SEND) {
		if (lower_layer_state == WIRELESS_TRANSMISSION_STATE_IDLE) {
			wireless_transmission_receive_packet();
			state = GO_BACK_ONE_SENDER_STATE_ACK;
		}
	} else if (state == GO_BACK_ONE_SENDER_STATE_ACK) {
		if (lower_layer_state != WIRELESS_TRANSMISSION_STATE_IDLE) {
			consider_retransmit();
			return;
		}

		wireless_transmission_get_received_packet(&received_packet);
		printf("Received %d bytes\n", received_packet.len);
		print_buffer(receive_buffer, 20);
		if (received_packet.status != WIRELESS_TRANSMISSION_VERIFY_OK) {
			if (consider_retransmit() == FALSE) {
				wireless_transmission_receive_packet();	
			}
			return;
		}

		uint8_t id = receive_buffer[0];
		if (id == current_id) {
			printf("Finished with id %d\n", id);
			state = GO_BACK_ONE_SENDER_STATE_IDLE;
		} else {
			if (consider_retransmit() == FALSE) {
				wireless_transmission_receive_packet();	
			}
		}
	}
}

/**
 * SFM for sender
 * 1) Receive packet
 * 2) Send ACK multiple times (MIN_ACK_COUNT)
 *
 *
 * @param void
 *
 * @return void
 */
static void protocol_go_back_1_periodic_receiver(void) {
	uint8_t lower_layer_state = wireless_transmission_get_state();

	if (lower_layer_state == WIRELESS_TRANSMISSION_STATE_ERROR) {
		state = GO_BACK_ONE_STATE_ERROR;
		return;
	}

	if (state == GO_BACK_ONE_RECEIVER_STATE_RECEIVE) {
		if (lower_layer_state != WIRELESS_TRANSMISSION_STATE_IDLE) {
			return;
		}

		wireless_transmission_get_received_packet(&received_packet);
		if (received_packet.status != WIRELESS_TRANSMISSION_VERIFY_OK) {
			wireless_transmission_receive_packet();
			return;
		}

		uint8_t id = receive_buffer[0];
		if (id != current_id) {
			current_id = id;
			new_packet = TRUE;
		}
		
		transmit_ack();
		state = GO_BACK_ONE_RECEIVER_STATE_ACK;
	} else if (state == GO_BACK_ONE_RECEIVER_STATE_ACK) {
		if (lower_layer_state == WIRELESS_TRANSMISSION_STATE_IDLE) {
			ack_left--;
			if (ack_left == 0) {
				state = GO_BACK_ONE_RECEIVER_STATE_IDLE;
			} else {
				transmit_ack();
			}
		}
	}
}

/**
 * Top level SFM for the module
 *
 * @param void
 *
 * @return void
 */
void protocol_go_back_1_periodic(void) {
	wireless_transmission_periodic();

	if (mode == GO_BACK_ONE_MODE_SENDER) {
		protocol_go_back_1_periodic_sender();
	} else if (mode == GO_BACK_ONE_MODE_RECEIVER) {
		protocol_go_back_1_periodic_receiver();
	}
}
