/*
 * SH1106 128x64 SPI OLED demo for XIAO nRF54LM20A.
 *
 * Recommended wiring:
 *   OLED SCK -> D8  (P1.04)
 *   OLED SDA -> D10 (P1.06, SPI MOSI)
 *   OLED RST -> D3  (P1.29)
 *   OLED DC  -> D2  (P1.30)
 *   OLED CS  -> D1  (P1.31)
 *   OLED VCC -> 3V3
 *   OLED GND -> GND
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/display.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/sys/util.h>

LOG_MODULE_REGISTER(sh1106_oled_demo, LOG_LEVEL_INF);

#define OLED_NODE DT_CHOSEN(zephyr_display)
#define OLED_WIDTH DT_PROP(OLED_NODE, width)
#define OLED_HEIGHT DT_PROP(OLED_NODE, height)
#define OLED_BUF_SIZE (OLED_WIDTH * OLED_HEIGHT / 8)

#define BOX_W 18
#define BOX_H 18
#define STEP_DELAY_MS 40

static const struct device *const display_dev = DEVICE_DT_GET(OLED_NODE);
static uint8_t framebuffer[OLED_BUF_SIZE];
static bool mono10_mode;

static const struct display_buffer_descriptor frame_desc = {
	.buf_size = OLED_BUF_SIZE,
	.width = OLED_WIDTH,
	.height = OLED_HEIGHT,
	.pitch = OLED_WIDTH,
};

static void clear_frame(bool on)
{
	memset(framebuffer, on ? (mono10_mode ? 0x00 : 0xFF) :
			 (mono10_mode ? 0xFF : 0x00),
	       sizeof(framebuffer));
}

static void set_pixel(int x, int y, bool on)
{
	size_t index;
	uint8_t mask;

	if ((x < 0) || (x >= OLED_WIDTH) || (y < 0) || (y >= OLED_HEIGHT)) {
		return;
	}

	index = ((size_t)(y / 8) * OLED_WIDTH) + (size_t)x;
	mask = BIT(y % 8);

	if (mono10_mode) {
		if (on) {
			framebuffer[index] &= (uint8_t)~mask;
		} else {
			framebuffer[index] |= mask;
		}
	} else {
		if (on) {
			framebuffer[index] |= mask;
		} else {
			framebuffer[index] &= (uint8_t)~mask;
		}
	}
}

static void draw_hline(int x, int y, int width, bool on)
{
	for (int i = 0; i < width; ++i) {
		set_pixel(x + i, y, on);
	}
}

static void draw_vline(int x, int y, int height, bool on)
{
	for (int i = 0; i < height; ++i) {
		set_pixel(x, y + i, on);
	}
}

static void draw_rect(int x, int y, int width, int height, bool on)
{
	if ((width <= 0) || (height <= 0)) {
		return;
	}

	draw_hline(x, y, width, on);
	draw_hline(x, y + height - 1, width, on);
	draw_vline(x, y, height, on);
	draw_vline(x + width - 1, y, height, on);
}

static void fill_rect(int x, int y, int width, int height, bool on)
{
	for (int row = 0; row < height; ++row) {
		draw_hline(x, y + row, width, on);
	}
}

static int flush_frame(void)
{
	return display_write(display_dev, 0, 0, &frame_desc, framebuffer);
}

static int show_border_and_cross(void)
{
	clear_frame(false);
	draw_rect(0, 0, OLED_WIDTH, OLED_HEIGHT, true);

	for (int i = 0; i < MIN(OLED_WIDTH, OLED_HEIGHT); ++i) {
		set_pixel(i, i, true);
		set_pixel((OLED_WIDTH - 1) - i, i, true);
	}

	return flush_frame();
}

static int show_full_white(void)
{
	clear_frame(true);
	draw_rect(0, 0, OLED_WIDTH, OLED_HEIGHT, false);
	return flush_frame();
}

static int show_horizontal_stripes(void)
{
	clear_frame(false);

	for (int y = 0; y < OLED_HEIGHT; ++y) {
		if (((y / 4) % 2) == 0) {
			draw_hline(0, y, OLED_WIDTH, true);
		}
	}

	return flush_frame();
}

static int show_vertical_stripes(void)
{
	clear_frame(false);

	for (int x = 0; x < OLED_WIDTH; ++x) {
		if (((x / 4) % 2) == 0) {
			draw_vline(x, 0, OLED_HEIGHT, true);
		}
	}

	return flush_frame();
}

static int show_moving_box(void)
{
	int ret;

	for (int x = 2; x <= (OLED_WIDTH - BOX_W - 2); x += 2) {
		clear_frame(false);
		draw_rect(0, 0, OLED_WIDTH, OLED_HEIGHT, true);
		fill_rect(x, (OLED_HEIGHT - BOX_H) / 2, BOX_W, BOX_H, true);
		ret = flush_frame();
		if (ret != 0) {
			return ret;
		}
		k_msleep(STEP_DELAY_MS);
	}

	for (int x = OLED_WIDTH - BOX_W - 2; x >= 2; x -= 2) {
		clear_frame(false);
		draw_rect(0, 0, OLED_WIDTH, OLED_HEIGHT, true);
		fill_rect(x, (OLED_HEIGHT - BOX_H) / 2, BOX_W, BOX_H, true);
		ret = flush_frame();
		if (ret != 0) {
			return ret;
		}
		k_msleep(STEP_DELAY_MS);
	}

	return 0;
}

static int select_pixel_format(void)
{
	int ret;

	ret = display_set_pixel_format(display_dev, PIXEL_FORMAT_MONO01);
	if (ret == 0) {
		mono10_mode = false;
		return 0;
	}

	ret = display_set_pixel_format(display_dev, PIXEL_FORMAT_MONO10);
	if (ret == 0) {
		mono10_mode = true;
		return 0;
	}

	return ret;
}

int main(void)
{
	struct display_capabilities caps;
	int ret;

	if (!device_is_ready(display_dev)) {
		LOG_ERR("Display device not ready");
		return -ENODEV;
	}

	ret = select_pixel_format();
	if (ret != 0) {
		LOG_ERR("Failed to set a monochrome pixel format: %d", ret);
		return ret;
	}

	display_get_capabilities(display_dev, &caps);

	LOG_INF("SH1106 OLED demo started");
	LOG_INF("Display: %s", display_dev->name);
	LOG_INF("Resolution: %ux%u", caps.x_resolution, caps.y_resolution);
	LOG_INF("Pixel format: %s", mono10_mode ? "MONO10" : "MONO01");
	LOG_INF("Wiring: SCK=D8/P1.04 SDA=D10/P1.06 RST=D3/P1.29 DC=D2/P1.30 CS=D1/P1.31");

	ret = display_blanking_off(display_dev);
	if (ret != 0) {
		LOG_ERR("display_blanking_off failed: %d", ret);
		return ret;
	}

	while (1) {
		ret = show_border_and_cross();
		if (ret != 0) {
			LOG_ERR("show_border_and_cross failed: %d", ret);
			return ret;
		}
		k_sleep(K_MSEC(1200));

		ret = show_full_white();
		if (ret != 0) {
			LOG_ERR("show_full_white failed: %d", ret);
			return ret;
		}
		k_sleep(K_MSEC(800));

		ret = show_horizontal_stripes();
		if (ret != 0) {
			LOG_ERR("show_horizontal_stripes failed: %d", ret);
			return ret;
		}
		k_sleep(K_MSEC(1200));

		ret = show_vertical_stripes();
		if (ret != 0) {
			LOG_ERR("show_vertical_stripes failed: %d", ret);
			return ret;
		}
		k_sleep(K_MSEC(1200));

		ret = show_moving_box();
		if (ret != 0) {
			LOG_ERR("show_moving_box failed: %d", ret);
			return ret;
		}
	}

	return 0;
}
