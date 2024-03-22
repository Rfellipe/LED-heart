#define SW0_NODE	DT_ALIAS(bt0)
#define LED_PWM_NODE_ID	 DT_COMPAT_GET_ANY_STATUS_OKAY(pwm_leds)
#define MAX_BRIGHTNESS	100
#define ANIMATIONS_STACK_SIZE 1024


static const struct gpio_dt_spec button = GPIO_DT_SPEC_GET_OR(SW0_NODE, gpios, {0});
static struct gpio_callback button_cb_data;

void go_and_goback(void);

void sequence_on_off(void);

void pulse(void);