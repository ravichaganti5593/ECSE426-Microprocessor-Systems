#include "interfaces/led_interface.h"
#include "modules/led_rotation_sm.h"

static uint8_t rotation_mode = LED_ROTATION_MODE_ROTATE_RACE;

static void led_rotation_rotate_leds_single(void) {
	static uint16_t count;
	count++;

	led_all_off();
	static const uint16_t count_max = 100;
	if (count % count_max < count_max / 4) {
		SET_LED(LED_1);
	} else if (count % count_max < count_max / 2) {
		SET_LED(LED_2);
	} else if (count % count_max < 3 * count_max / 4) {
		SET_LED(LED_3);
	} else {
		SET_LED(LED_4);
	}
}

static void led_rotation_rotate_leds_double(void) {
	static uint16_t count;
	count++;

	led_all_off();
	static const uint16_t count_max = 100;
	if (count % count_max < count_max / 2) {
		SET_LED(LED_1);
		SET_LED(LED_3);
	} else {
		SET_LED(LED_2);
		SET_LED(LED_4);
	}
}

static void led_rotation_rotate_leds_triple(void) {
	static uint16_t count;
	count++;

	led_all_on();
	static const uint16_t count_max = 100;
	if (count % count_max < count_max / 4) {
		CLEAR_LED(LED_1);
	} else if (count % count_max < count_max / 2) {
		CLEAR_LED(LED_2);
	} else if (count % count_max < 3 * count_max / 4) {
		CLEAR_LED(LED_3);
	} else {
		CLEAR_LED(LED_4);
	}
}

static void led_rotation_rotate_leds_race(void) {
	static uint16_t count;
	count++;

	led_all_off();
	static uint16_t count_max = 1000;
	count_max--;
	if (count_max == 20) {
		count_max = 1000;
	}

	if (count % count_max < count_max / 4) {
		SET_LED(LED_1);
	} else if (count % count_max < count_max / 2) {
		SET_LED(LED_2);
	} else if (count % count_max < 3 * count_max / 4) {
		SET_LED(LED_3);
	} else {
		SET_LED(LED_4);
	}
}

static void led_rotation_blink_all(void) {
	static uint16_t count;
	count++;

	
	static const uint16_t count_max = 100;
	if (count % count_max < count_max / 2) {
		led_all_off();
	} else {
		led_all_on();
	}
}

void led_rotation_set_mode(uint8_t mode) {
	rotation_mode = mode;
}

void led_rotation_rotate_leds(void) {
	if (rotation_mode == LED_ROTATION_MODE_ROTATE_SINGLE) {
		led_rotation_rotate_leds_single();
	} else if (rotation_mode == LED_ROTATION_MODE_ROTATE_DOUBLE) {
		led_rotation_rotate_leds_double();
	} else if (rotation_mode == LED_ROTATION_MODE_ROTATE_TRIPLE) {
		led_rotation_rotate_leds_triple();
	} else if (rotation_mode == LED_ROTATION_MODE_ROTATE_RACE) {
		led_rotation_rotate_leds_race();
	} else if (rotation_mode == LED_ROTATION_MODE_BLINK) {
		led_rotation_blink_all();
	}
}
