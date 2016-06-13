#include "util_timer.h"
#include "system_config.h"
#include "my_types.h"

/*	@param 		blocking_wait 		only using this parameter for this lab to enable no operation when this method is called
*/


static uint32_t timer;
static uint8_t expecting = 0;
static uint8_t expired = 0;
static function_ptr_t scheduled = NULL;

static uint32_t expected_time;

void timer_call_back() {
	timer++;

	if (expecting && timer == expected_time) {
		expecting = 0;
		expired = 1;
		if (scheduled != NULL) {
			scheduled();
			scheduled = NULL;
		}
	}
}

int timer_count_down(uint32_t count) {
	expecting = 1;
	expired = 0;
	expected_time = timer + count;
	return 0;
}

int timer_schedule(uint32_t count, function_ptr_t function) {
	expecting = 1;
	expired = 0;
	expected_time = timer + count;
	scheduled = function;
	return 0;
}

int timer_count_expired(void) {
	return expired;
}

int blocking_wait(uint32_t count) {
	for (uint32_t i = 0; i < count; i++) {
		__nop();		//no operation
		__nop();
		__nop();
		__nop();
		__nop();
	}
	return TRUE;
}
