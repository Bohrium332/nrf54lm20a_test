#include <stdbool.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/kernel.h>

#define UART_NODE DT_NODELABEL(uart20)
#define LED_NODE DT_ALIAS(led0)

#if !DT_NODE_HAS_STATUS(UART_NODE, okay)
#error "uart20 must be enabled"
#endif

#if !DT_NODE_HAS_STATUS(LED_NODE, okay)
#error "led0 alias is required on this board"
#endif

static const struct device *const uart_dev = DEVICE_DT_GET(UART_NODE);
static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED_NODE, gpios);

static void uart_send_string(const char *str)
{
	while (*str != '\0') {
		uart_poll_out(uart_dev, (unsigned char)*str++);
	}
}

int main(void)
{
	uint8_t ch;
	bool led_on = false;
	int ret;

	if (!device_is_ready(uart_dev)) {
		return 0;
	}

	if (!gpio_is_ready_dt(&led)) {
		uart_send_string("LED not ready\r\n");
		return 0;
	}

	ret = gpio_pin_configure_dt(&led, GPIO_OUTPUT_INACTIVE);
	if (ret != 0) {
		uart_send_string("LED init failed\r\n");
		return 0;
	}

	uart_send_string("\r\nUART20 demo ready\r\n");
	uart_send_string("TX=P1.11 RX=P1.10 Baud=1000000\r\n");
	uart_send_string("Send 'a' to toggle LED\r\n");

	while (1) {
		if (uart_poll_in(uart_dev, &ch) == 0) {
			if (ch == 'a' || ch == 'A') {
				led_on = !led_on;
				(void)gpio_pin_toggle_dt(&led);
				uart_send_string("已收到 ");
				uart_send_string(led_on ? "LED=ON\r\n" : "LED=OFF\r\n");
			} else if (ch != '\r' && ch != '\n') {
				uart_send_string("Please send 'a'\r\n");
			}
		}

		k_msleep(5);
	}

	return 0;
}
