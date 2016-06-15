#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <inttypes.h>

#include "stm32f4xx.h"

#include "drivers/cc2500.h"
#include "drivers/lsm9ds1.h"
#include "modules/commands.h"
#include "modules/map.h"
#include "modules/protocol_go_back_1.h"
#include "modules/step.h"
#include "modules/turn.h"
#include "interfaces/acc.h"
#include "interfaces/gyro.h"
#include "utils/utils.h"

/**
 * The maximum number of coordinates to send via the wireless.
 */
#define MAX_COORDINATES_TO_SEND 20

/**
 * Main function wireless states.
 */
typedef enum {
    CLEAR_STATE,
    COORDINATES_STATE,
    PLOT_STATE
} wireless_states;

/**
 * Flag to indicate when the SysTick timer ticks.
 */
bool has_ticked = false;

/**
 * Temporary buffer used to transfer data to the wireless.
 */
static uint8_t wireless_temp[200];

/**
 * The next set of coordnates to send
 */
static int16_t next_coordinates[MAX_COORDINATES_TO_SEND * 2 + 1];

/**
 * Time to wait between wireless calls.
 */
static uint8_t wireless_wait = 30;

/**
 * Wireless sending and receiving section. This follows the following steps:
 * 1) Send CLEAR command to LCD board. Wait for ACK
 * 2) Once ACK for the CLEAR command is received, turn off step recognition,
 * 3) Transmit all recorded coordinates to the LCD board.
 * 4) Send PLOT command to LCD board.
 * 5) Once ACK for PLOT command has been received, go back to 1
 *
 * @param void
 *
 * @return void
 */
void wireless_loop(void) {
	protocol_go_back_1_periodic();

	static uint8_t prev = 0;
	uint8_t state = protocol_go_back_1_get_state();
	if (state == GO_BACK_ONE_SENDER_STATE_IDLE) {
		static wireless_states sending_index = CLEAR_STATE;
		if (sending_index == CLEAR_STATE) {
			printf("Sending CLEAR\n");
			memcpy(wireless_temp + 1, CLEAR_COMMAND, COMMAND_CLEAR_LEN);
			protocol_go_back_1_send(wireless_temp + 1, COMMAND_CLEAR_LEN);
			sending_index = COORDINATES_STATE;
		} else if (sending_index == COORDINATES_STATE) {
            uint16_t length;
            map_terminate_processing();
            int8_t ret = map_get_next_coordinates(next_coordinates + 1, &length, 
                MAX_COORDINATES_TO_SEND * 2);
            if (!ret) {
                sending_index = PLOT_STATE;
            } else {
                printf("Length: %d\n", length);
                printf("Coordinates to send: ");
                for (uint16_t i = 1; i <= length; i++) {
                    printf("%" PRIi16 " - ", next_coordinates[i]);
                }	
                printf("\n");
                protocol_go_back_1_send((uint8_t *) (next_coordinates + 1), 
                    length * sizeof(int16_t));
            }
		} else if (sending_index == PLOT_STATE) {
			printf("Sending PLOT\n");
			memcpy(wireless_temp + 1, PLOT_COMMAND, COMMAND_PLOT_LEN);
			protocol_go_back_1_send(wireless_temp + 1, COMMAND_PLOT_LEN);
			sending_index++;
		} else {
			sending_index = CLEAR_STATE;
			printf("Done\n");
		}
	} else {
		if (state == GO_BACK_ONE_SENDER_STATE_SEND) {
			wireless_wait = 3;
		} else {
			wireless_wait = 1;
		}
		
		if (state != prev) {
			prev = state;
			printf("State is %d\n", state);
		}
	}
}

/**
 * Initialize all system components.
 */
void init() {
    // Set the frequency of the SysTick interrupt to 50 Hz
	SysTick_Config(SystemCoreClock / 50);

    // Initialize wireless
	CC2500_LowLevel_Init();
	CC2500_Reset();
	protocol_go_back_1_init(GO_BACK_ONE_MODE_SENDER);
    // Initialize accelerometer
    acc_init();
	// Initialize gyroscope
    gyro_init();
    // Initialize map
    map_init();

    // Update accelerometer once manually to enable interrupt
    acc_update();
	// Update gyroscope once manually to enable interrupt
	gyro_update();
}

/**
 * Main function of application.
 */
int main() {
    // Initialize all system components
    init();
    
    // Application loop
    while (1) {
        // Interrupts
        if (acc_interrupt) {
            acc_update();
        }
		if (gyro_interrupt) {
            gyro_update();
        }

		if (has_ticked) {

            // Step and turn detection
            step_update(acc_get_x());
            turn_update(gyro_get_x());
            
            // Print out acc and gyro values for debug purposes
            static uint8_t agcount = 0;
            agcount = (agcount + 1) % 50;
            if (agcount == 0) {
                printf("%f - %f\n", acc_get_x(), gyro_get_x());
            }
            
            // Wireless transmission
			static uint8_t count = 0;
			count = (count + 1) % wireless_wait;
			if (count == 0) {
				wireless_loop();
			}
							
            has_ticked = false;
        }
    }
}

/**
 * Handler for SysTick timer.
 */
void SysTick_Handler() {
	has_ticked = true;
}
