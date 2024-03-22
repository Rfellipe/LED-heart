/*
 * Copyright (c) 2020 Seagate Technology LLC
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <errno.h>
#include <zephyr/drivers/led.h>
#include <zephyr/sys/util.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>

#include <zephyr/logging/log.h>
#include "main.h"

LOG_MODULE_REGISTER(main, CONFIG_LOG_DEFAULT_LEVEL);

K_THREAD_DEFINE(animation1, ANIMATIONS_STACK_SIZE,
								go_and_goback, NULL, NULL, NULL,
								16, 0, 0);

K_THREAD_DEFINE(animation2, ANIMATIONS_STACK_SIZE,
								sequence_on_off, NULL, NULL, NULL,
								16, 0, 0);

K_THREAD_DEFINE(animation3, ANIMATIONS_STACK_SIZE,
								pulse, NULL, NULL, NULL,
								16, 0, 0);

static const struct device *led_pwm = DEVICE_DT_GET(LED_PWM_NODE_ID);

const char *led_label[] = {
	DT_FOREACH_CHILD_SEP_VARGS(LED_PWM_NODE_ID, DT_PROP_OR, (,), label, NULL)
};

const int num_leds = ARRAY_SIZE(led_label);
int animation = 0;

void button_pressed(const struct device *dev, struct gpio_callback *cb, uint32_t pins) {
	animation++;
	
	if (animation > 2) {
		animation = 0;
	}

	switch (animation) {
		case 0:
		k_thread_resume(animation1);
		k_thread_suspend(animation2);
		k_thread_suspend(animation3);
		break;
		case 1:
		k_thread_suspend(animation1);
		k_thread_resume(animation2);
		k_thread_suspend(animation3);
		break;
		case 2:
		k_thread_suspend(animation1);
		k_thread_suspend(animation2);
		k_thread_resume(animation3);
		break;

		default:
		break;
	}
}

void go_and_goback(void) {
	int err, i, j;

	do {
		for (i = 0; i < num_leds; i++) {
			err = led_on(led_pwm, i);
			if (err) {
				LOG_ERR("led on error %s (err %d)", led_pwm->name, err);
			}
			k_sleep(K_MSEC(100));
		}
		
		for (j = num_leds - 1; j >= 0; j--) {
			err = led_off(led_pwm, j);
			if (err) {
				LOG_ERR("led off error %s (err %d)", led_pwm->name, err);
			}
			k_sleep(K_MSEC(100));
		}
		
	} while (true);
}

void sequence_on_off(void) {
	int err, i, j;

	do {
		for (i = 0; i < num_leds; i++) {
			err = led_on(led_pwm, i);
			if (err) {
				LOG_ERR("led on error (err %d)", err);
			}
			k_sleep(K_MSEC(100));
		}
		
		for (i = 0; i < num_leds; i++) {
			err = led_off(led_pwm, i);
			if (err) {
				LOG_ERR("led off error (err %d)", err);	
			}
			k_sleep(K_MSEC(100));
		}
		
	} while (true);
}

void pulse(void) {
	int err, i, level;

	do {
		for (i = 0; i <= 100; i++) {
			for (int j = 0; j < num_leds; j++) {
				err = led_set_brightness(led_pwm, j, i);
				if (err) {
					LOG_ERR("led on error (err %d)", err);
				}
			}

			k_sleep(K_MSEC(20));
		}

		for (i = 0; i <= 100; i++) {
			for (int j = 0; j < num_leds; j++) {
				err = led_set_brightness(led_pwm, j, 100 - i);
				if (err) {
					LOG_ERR("led on error (err %d)", err);
				}
			}

			k_sleep(K_MSEC(20));
		}
		
	} while (true);
}

int main(void)
{
	int err, ret;

	if (!gpio_is_ready_dt(&button)) {
		LOG_ERR("Error: button device %s is not ready",
		       button.port->name);
		return 0;
	}

	ret = gpio_pin_configure_dt(&button, GPIO_INPUT);
	if (ret != 0) {
		LOG_ERR("Error %d: failed to configure %s pin %d",
		       ret, button.port->name, button.pin);
		return 0;
	}

	ret = gpio_pin_interrupt_configure_dt(&button,
					      GPIO_INT_EDGE_TO_ACTIVE);
	if (ret != 0) {
		LOG_ERR("Error %d: failed to configure interrupt on %s pin %d",
			ret, button.port->name, button.pin);
		return 0;
	}

	gpio_init_callback(&button_cb_data, button_pressed, BIT(button.pin));
	gpio_add_callback(button.port, &button_cb_data);

	if (!device_is_ready(led_pwm)){
		LOG_ERR("error startings LEDs");
	}
	
	k_thread_suspend(animation2);
	k_thread_suspend(animation3);

	return 0;
}
