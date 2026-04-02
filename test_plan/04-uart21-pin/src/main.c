#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>

static const struct device *const gpio1_dev = DEVICE_DT_GET(DT_NODELABEL(gpio1));

#define TOGGLE_PERIOD_MS 3000
#define UART21_TX_PIN 8
#define UART21_RX_PIN 9

int main(void)
{
	if (!device_is_ready(gpio1_dev)) {
		return 0;
	}

	if (gpio_pin_configure(gpio1_dev, UART21_TX_PIN, GPIO_OUTPUT_INACTIVE) != 0) {
		return 0;
	}

	if (gpio_pin_configure(gpio1_dev, UART21_RX_PIN, GPIO_OUTPUT_INACTIVE) != 0) {
		return 0;
	}

	while (1) {
		(void)gpio_pin_toggle(gpio1_dev, UART21_TX_PIN);
		(void)gpio_pin_toggle(gpio1_dev, UART21_RX_PIN);
		k_msleep(TOGGLE_PERIOD_MS);
	}
}
