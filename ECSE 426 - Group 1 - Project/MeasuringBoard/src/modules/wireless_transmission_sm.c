#include <string.h>
#include "wireless_transmission_sm.h"
#include "drivers/cc2500.h"
#include "utils/utils.h"
#include "my_types.h"

/********************************************************************************************************************/
/*****************************************************Variables******************************************************/
/********************************************************************************************************************/
static uint8_t send_buffer[2*WIRELESS_TRANSMISSION_PACKET_SIZE];
static uint8_t receive_buffer[2*WIRELESS_TRANSMISSION_PACKET_SIZE];

static uint8_t operation_state;
static uint8_t sending_len;
static uint8_t sending_index, receiving_index;

/********************************************************************************************************************/
/*****************************************************Typedef and constants******************************************/
/********************************************************************************************************************/
const uint8_t START_PACKET = 255;
const uint8_t END_PACKET = START_PACKET - 1;

/**
 * Since no byte in the packet can be START_PACKET or END_PACKET, we have to replace the checksum with some other values
 * if the checksum byte does end up being START_PACKET or END_PACKET. The following are possible substitution for the checksum values
 * if it collides with START_PACKET or END_PACKET
 */
static const uint8_t COLLISION_CHECKSUM_REPLACEMENT_1 = END_PACKET - 1;
static const uint8_t COLLISION_CHECKSUM_REPLACEMENT_2 = END_PACKET - 2;
static const uint8_t COLLISION_CHECKSUM_REPLACEMENT_3 = END_PACKET - 3;
static const uint8_t COLLISION_CHECKSUM_REPLACEMENT_4 = END_PACKET - 4;

//Length and index constants
static const uint8_t START_SIGNAL_LEN = 3;
static const uint8_t LEN_LEN = 1;
static const uint8_t FULL_HEADER_LEN = START_SIGNAL_LEN + LEN_LEN;
static const uint8_t DATA_INDEX = FULL_HEADER_LEN - START_SIGNAL_LEN;
static const uint8_t END_SIGNAL_LEN = 3;
static const uint8_t CHECKSUM_LEN = 3;

static const uint8_t MAX_DATA_SIZE = WIRELESS_TRANSMISSION_PACKET_SIZE - FULL_HEADER_LEN - CHECKSUM_LEN - END_SIGNAL_LEN;

//Codec constant declarations
static const uint8_t CODEC_ESCAPE_BYTE = 0x09;

static const uint8_t CODEC_ESCAPED_CODEC_ESCAPE_BYTE = CODEC_ESCAPE_BYTE;
static const uint8_t CODEC_ESCAPED_PAD_BYTE = 0x10;
static const uint8_t CODEC_ESCAPED_START_PACKET = 0x11;
static const uint8_t CODEC_ESCAPED_END_PACKET = 0x12;

/********************************************************************************************************************/
/**
 * Check sum is CHECKSUM_LEN bytes length. It contains the following bytes in order
 * 1) xor of all data bytes in the packet
 * 2) xor of all data bytes in the packet whose index is divisible by 2
 * 3) xor of all data bytes in the packet whose index is divisible by 3
 */
typedef struct {
  uint8_t xor_all;
  uint8_t xor2;
  uint8_t xor3;
} my_check_sum;


/********************************************************************************************************************/
/***************************************** Forward declaration ******************************************************/
/********************************************************************************************************************/
uint8_t wireless_transmission_protocol_checksum(uint8_t* raw_packet, uint8_t* check_sum, uint8_t packet_len);

/********************************************************************************************************************/
/***************************************** Initialization and packet encapsulation logic ****************************/
/********************************************************************************************************************/
/**
 * Initialize SFM of this module
 *
 * @param void
 *
 * @return void
 */
void wireless_transmission_init(void) {
	operation_state = WIRELESS_TRANSMISSION_STATE_IDLE;

	sending_index = 0;
	receiving_index = 0;

	sending_len = 0;
}

/**
 * Check if a full packet is in the right format. The following constraints are checked
 * 1) Check if START_PACKET is at the beginning of the packet
 * 2) Check if END_PACKET is at the end of the packet
 * 3) Check if the packet length is correct
 * 4) Check if the checksum is correct
 * 
 * @deprecated
 * @param full_packet	Pointer to the full packet buffer
 * @param full_len 		Length of the whole packet
 *
 * @return TRUE if the conditions are met, and FALSE otherwise
 */
uint8_t wireless_transmission_protocol_sanity_check(uint8_t* full_packet, uint8_t full_len) {
	for (uint8_t i = 0; i < START_SIGNAL_LEN; i++) {
		if (full_packet[i] != START_PACKET) {
			return FALSE;
		}
	}

	for (uint8_t i = 0; i < END_SIGNAL_LEN; i++) {
		if (full_packet[full_len - i - 1] != END_PACKET) {
			return FALSE;
		}
	}

	if (full_len <= FULL_HEADER_LEN + CHECKSUM_LEN + END_SIGNAL_LEN) {
		return FALSE;
	}

	uint8_t data_len = full_len - FULL_HEADER_LEN - CHECKSUM_LEN - END_SIGNAL_LEN;
	return wireless_transmission_protocol_checksum(full_packet + FULL_HEADER_LEN, full_packet + FULL_HEADER_LEN + data_len, data_len);
}

/**
 * Calculate checksum for a packet. For checksum, see checksum struct definition
 *
 * @param raw_packet	Pointer to the raw packet content
 * @param len 			Length of the raw packet, not including checksum
 * @param result		pointer to checksum where results are written to
 *
 * @return void
 */
static void calculate_check_sum(uint8_t* raw_packet, uint8_t len, my_check_sum* result) {
	uint8_t xor_all = 0, xor2 = 0, xor3 = 0;
	for (uint8_t i = 0; i < len; i++) {
		uint8_t current_byte = raw_packet[i];
		xor_all ^= current_byte;

		if (i % 2 == 0) {
			xor2 ^= current_byte;
		}
		if (i % 3 == 0) {
			xor3 ^= current_byte;
		}
	}

	result->xor_all = xor_all == START_PACKET ? COLLISION_CHECKSUM_REPLACEMENT_2 : (xor_all == END_PACKET ? COLLISION_CHECKSUM_REPLACEMENT_4 : xor_all);
	result->xor2 = (xor2 == START_PACKET) ? COLLISION_CHECKSUM_REPLACEMENT_1 : (xor2 == END_PACKET ? COLLISION_CHECKSUM_REPLACEMENT_2 : xor2);
	result->xor3 = (xor3 == START_PACKET) ? COLLISION_CHECKSUM_REPLACEMENT_3 : (xor3 == END_PACKET ? COLLISION_CHECKSUM_REPLACEMENT_4 : xor3);
}

/**
 * Encapsulate a packet with headers and checksum
 *
 * @param raw_packet	Pointer to the raw packet content
 * @param len 			Length of the raw packet content
 *
 * @return length of the packet with header and checksum included
 */
uint8_t wireless_transmission_protocol_encapsulate(uint8_t* raw_packet, uint8_t len) {
	uint8_t* full_packet_start = raw_packet - FULL_HEADER_LEN;
	memset(full_packet_start, START_PACKET, START_SIGNAL_LEN);

	*(full_packet_start + START_SIGNAL_LEN) = len;

	calculate_check_sum(raw_packet, len, (my_check_sum*) (raw_packet + len));
	memset(raw_packet + len + CHECKSUM_LEN, END_PACKET, END_SIGNAL_LEN);

	return FULL_HEADER_LEN + CHECKSUM_LEN + END_SIGNAL_LEN + len;
}

/**
 * Validate the checksum of a packet
 *
 * @param raw_packet	Pointer to the raw packet content
 * @param check_sum 	Pointer to the location where checksum starts
 * @param packet_len 	Length of the packet, not including checksum
 *
 * @return TRUE if checksum is valid, FALSE otherwise
 */
uint8_t wireless_transmission_protocol_checksum(uint8_t* raw_packet, uint8_t* check_sum, uint8_t packet_len) {
	my_check_sum expecting;
	calculate_check_sum(raw_packet, packet_len, &expecting);
	return memcmp(check_sum, &expecting, CHECKSUM_LEN) == 0;
}


/********************************************************************************************************************/
/****************************************** Codec section ***********************************************************/
/********************************************************************************************************************/
/**
 * Pad a packet with null byte.
 * This will append additional bytes at the end of the buffer. Therefore it is the responsibility of the client to ensure
 * that there is enough space in the input buffer
 *
 * @param src	Pointer to the source buffer where input values are read
 * @param len	Length of the source buffer
 *
 * @return number of bytes in the buffer after padding (padded length)
 */
static uint16_t wireless_transmission_codec_pad(uint8_t* src, uint16_t len) {
	src[len] = CODEC_ESCAPE_BYTE;
	src[len + 1] = CODEC_ESCAPED_PAD_BYTE;

	return len + 2;
}

/**
 * Encode a packet
 *
 * @param dst	Pointer to the destination buffer where result is written to
 * @param src	Pointer to the source buffer where input values are read
 * @param len	Length of the source buffer
 *
 * @return number of bytes in the destination buffer (encoded length)
 */
static uint16_t wireless_transmission_encode(uint8_t* dst, uint8_t* src, uint16_t len) {
	uint16_t index = 0;

	for (uint16_t i = 0; i < len; i++) {
		uint8_t current = src[i];

		if (current == CODEC_ESCAPE_BYTE) {
			dst[index++] = CODEC_ESCAPE_BYTE;
			dst[index++] = CODEC_ESCAPED_CODEC_ESCAPE_BYTE;
		} else if (current == START_PACKET) {
			dst[index++] = CODEC_ESCAPE_BYTE;
			dst[index++] = CODEC_ESCAPED_START_PACKET;
		} else if (current == END_PACKET) {
			dst[index++] = CODEC_ESCAPE_BYTE;
			dst[index++] = CODEC_ESCAPED_END_PACKET;
		} else {
			dst[index++] = current;
		}
	}

	return index;
}

/**
 * Decode a packet from an encoded packet
 *
 * @param dst	Pointer to the destination buffer where result is written to
 * @param src	Pointer to the source buffer where input values are read
 * @param len	Length of the source buffer
 *
 * @return number of bytes in the destination buffer (decoded length)
 */
static uint16_t wireless_transmission_decode(uint8_t* dst, uint8_t* src, uint16_t len) {
	uint16_t index = 0, is_escaped = FALSE;

	for (uint16_t i = 0; i < len; i++) {
		uint8_t current = src[i];

		if (is_escaped) {//Escaped mode
			if (current == CODEC_ESCAPED_CODEC_ESCAPE_BYTE) {
				dst[index++] = CODEC_ESCAPE_BYTE;
			} else if (current == CODEC_ESCAPED_PAD_BYTE) {
				//Intentionally left blank since padding element should be ignored
			} else if (current == CODEC_ESCAPED_START_PACKET) {
				dst[index++] = START_PACKET;
			} else if (current == CODEC_ESCAPED_END_PACKET) {
				dst[index++] = END_PACKET;
			} else {//Invalid escaped sequence. Unable to decode
				return 0;
			}

			is_escaped = FALSE;
		} else {//Regular mode
			if (current == CODEC_ESCAPE_BYTE) {
				is_escaped = TRUE;
			} else {
				dst[index++] = current;
			}
		}
	}

	return index;
}

/********************************************************************************************************************/
/***************************************** Main API *****************************************************************/
/********************************************************************************************************************/
/**
 * Retrieve the received packet information
 * If the packet was not successfully received, status will not be WIRELESS_TRANSMISSION_VERIFY_OK.
 * In such case, no oher parameter in the returning struct is guaranteed to be correct.
 *
 * This method checks for the packet length, as well as the packe checksum
 *
 * @param received_packet	pointer to the struct where result will be written
 *
 * @return void
 */
void wireless_transmission_get_received_packet(wireless_received_packet* received_packet) {
	if (operation_state != WIRELESS_TRANSMISSION_STATE_IDLE) {
		return;
	}

	if (receiving_index < DATA_INDEX + CHECKSUM_LEN) {
		received_packet->status = WIRELESS_TRANSMISSION_VERIFY_INCORRECT_LENGTH;
		return;
	}

	//Now verify whatever is in receive_buffer
	uint8_t packet_len = receive_buffer[0];
	if (packet_len == 0) {
		received_packet->status = WIRELESS_TRANSMISSION_VERIFY_INCORRECT_LENGTH;
		return;
	}
	received_packet->len = packet_len;

	uint8_t* check_sum_start_left = receive_buffer + DATA_INDEX + packet_len;
	uint8_t* check_sum_start_right = receive_buffer + receiving_index - CHECKSUM_LEN;
	if (check_sum_start_left != check_sum_start_right) {
		received_packet->status = WIRELESS_TRANSMISSION_VERIFY_INVALID_CHECK_SUM;
		return;
	}

	uint8_t check_sum_verify = wireless_transmission_protocol_checksum(receive_buffer + DATA_INDEX, check_sum_start_left, packet_len);
	if (!check_sum_verify) {
		received_packet->status = WIRELESS_TRANSMISSION_VERIFY_INCORRECT_CHECKSUM;
		return;
	}

	uint16_t decoded_len = wireless_transmission_decode(received_packet->buffer, receive_buffer + DATA_INDEX, receiving_index - DATA_INDEX - CHECKSUM_LEN);
	if (decoded_len == 0) {
		received_packet->status = WIRELESS_TRANSMISSION_VERIFY_INVALID_CODEC;
		return;
	}

	received_packet->len = (uint8_t) decoded_len;
	received_packet->status = WIRELESS_TRANSMISSION_VERIFY_OK;
}

/**
 * Tell SFM to transmit data wirelessly. This method will encapsulate the data with appropriate header and generate checksum.
 *
 * @param packet	the pointer to the data buffer
 * @param len 		length (in bytes) of the data buffer
 *
 * @return TRUE if success and FALSE otherwise
 */
uint8_t wireless_transmission_transmit(uint8_t* packet, uint8_t len) {
	if (len > WIRELESS_TRANSMISSION_PACKET_SIZE) {
		return FALSE;
	}

	if (len > MAX_DATA_SIZE) {
		return FALSE;
	}

	uint16_t encoded_len = wireless_transmission_encode(send_buffer + FULL_HEADER_LEN, packet, len);
	if (encoded_len == 0) {
		return FALSE;
	}

	while (encoded_len == END_PACKET || encoded_len == START_PACKET) {
		encoded_len = wireless_transmission_codec_pad(send_buffer + FULL_HEADER_LEN, encoded_len);
	}

	if (encoded_len > MAX_DATA_SIZE) {
		return FALSE;
	} else {
		len = (uint8_t) encoded_len;
	}
	
	uint8_t new_len = wireless_transmission_protocol_encapsulate(send_buffer + FULL_HEADER_LEN, len);
	if (new_len == 0) {
		return FALSE;
	}

	sending_index = 0;
	sending_len = new_len;

	operation_state = WIRELESS_TRANSMISSION_STATE_TRANSMIT;
	return TRUE;
}

/**
 * Tell SFM to retransmit the last packet.
 *
 * @param void
 *
 * @return TRUE if success and FALSE otherwise
 */
uint8_t wireless_transmission_retransmit(void) {
	if (sending_len == 0) {
		return FALSE;
	}

	sending_index = 0;
	operation_state = WIRELESS_TRANSMISSION_STATE_TRANSMIT;
	return TRUE;
}

/**
 * Tell SFM to start receiving a packet wirelessly.
 *
 * @param void
 *
 * @return void
 */
void wireless_transmission_receive_packet(void) {
	receiving_index = 0;
	memset(receive_buffer, 0, WIRELESS_TRANSMISSION_PACKET_SIZE);
	operation_state = WIRELESS_TRANSMISSION_STATE_RECEIVE;
}

/**
 * SFM main operation.
 * When in transmit state:
 *		1) If CC2500 is in TX
 *			If there is at least one byte left to send
 *				Attempt to put 1 byte into TXFIFO
 *			Else go to 5
 *		   Otherwise go to 3
 *		2) Wait till CC2500 state changes to non TX
 * 		3) Send STX to ask CC2500 to go to TX.
 *		4) Go to 1
 *		5) Return to IDLE state
 *
 * When in receive state
 *		1) If CC2500 in RX
 *			Read number of bytes in RXFIFO.
 *			If number of bytes > 0, read all bytes in RXFIFO one by one and process them (process described below)
 *			Else go to 1 (restart this block)
 *		   Else go to 2
 *		2) Continuously send RTX until CC2500 is in RX. 
 *		3) Go to 1
 *
 * Upon receiving one byte:
 *		If SFM is expecting END_PACKET
 *			If it is END_PACKET, go to IDLE state
 *			Else if it is START_PACKET, stop expecting END_PACKET
 *			Else write one byte into receive buffer
 *		Else if SFM is not expecting END_PACKET
 *			If it is START_PACKET, start expecting END_PACKET
 *			Else drop received byte
 *
 * @param void
 *
 * @return void
 */
void wireless_transmission_periodic(void) {
	if (operation_state == WIRELESS_TRANSMISSION_STATE_TRANSMIT) {
		static uint8_t pending_send = FALSE;

		uint8_t state = CC2500_get_state();
		if (state == CC2500_STATE_TX_UNDERFLOW) {
			CC2500_flush_tx();
			operation_state = WIRELESS_TRANSMISSION_STATE_ERROR;
		} else if (state != CC2500_STATE_TX) {
			CC2500_read_one(CC2500_STX);
			pending_send = FALSE;

			if (sending_index == sending_len) {
				operation_state = WIRELESS_TRANSMISSION_STATE_IDLE;
			}
			return;
		} else {
			if (pending_send == FALSE) {
				// Sending multiple bytes at once. This feature is unfortunately disabled
				// because the underlying CC2500 driver is not configured to do so

				// uint8_t byte_left_to_send = sending_len - sending_index;
				// uint8_t byte_available = CC2500_get_txbytes();

				// uint8_t sending = MAX(byte_left_to_send, byte_available);
				// sending = MIN(sending, 3);

				// printf("Going to send\n");
				// print_buffer(send_buffer + sending_index, sending);

				// if (sending > 0) {
				// 	CC2500_write_tx(send_buffer + sending_index, sending);
				// 	sending_index += sending;
				// 	pending_send = TRUE;
				// }


				// Instead, send the packet byte by byte
				uint8_t to_send = send_buffer[sending_index];
				CC2500_write_tx_one(to_send);
				sending_index++;
				pending_send = TRUE;
			}
		}
	} else if (operation_state == WIRELESS_TRANSMISSION_STATE_RECEIVE) {
		static uint8_t expecting_end = FALSE;

		uint8_t state = CC2500_get_state();
		if (state == CC2500_STATE_RX_OVERFLOW) {
			CC2500_flush_rx();
			operation_state = WIRELESS_TRANSMISSION_STATE_ERROR;
		} else if (state != CC2500_STATE_RX) {
			do {
				CC2500_read_one(CC2500_SRX);
				state = CC2500_get_state();
			} while (state != CC2500_STATE_RX && state != CC2500_STATE_RX_OVERFLOW);
			wireless_transmission_periodic();
		} else {
			uint8_t rxbytes = CC2500_get_rxbytes();

			while (rxbytes > 0) {
				uint8_t received = CC2500_read_rx_one();
				if (expecting_end == FALSE) {
					if (received == START_PACKET) {
						expecting_end = TRUE;
					}
				} else {//expecting_end = TRUE
					if (received == END_PACKET || received == START_PACKET) {
						operation_state = WIRELESS_TRANSMISSION_STATE_IDLE;
						return;
					} else {
						receive_buffer[receiving_index] = received;
						receiving_index++;
					}
				}
				rxbytes--;
			}
		}
	}
}

/**
 * Retrieve current state of the internal SFM
 *
 * @param void
 *
 * @return current state of the internal SFM
 */
uint8_t wireless_transmission_get_state(void) {
	return operation_state;
}
