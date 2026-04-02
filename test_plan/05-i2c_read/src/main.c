/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 * Copyright (c) 2021 Nordic Semiconductor ASA
 * Copyright (c) 2025 Seeed Studio
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/sys/printk.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(bme280_demo, LOG_LEVEL_INF);

/*
 * Get a device structure from a devicetree node with compatible
 * "bosch,bme280". (If there are multiple, just pick one.)
 */
const struct device *const dev = DEVICE_DT_GET_ANY(bosch_bme280);

SENSOR_DT_READ_IODEV(iodev, DT_COMPAT_GET_ANY_STATUS_OKAY(bosch_bme280),
	{SENSOR_CHAN_AMBIENT_TEMP, 0},
	{SENSOR_CHAN_HUMIDITY, 0},
	{SENSOR_CHAN_PRESS, 0});

RTIO_DEFINE(ctx, 1, 1);

static const struct device *check_bme280_device(void)
{
	if (dev == NULL) {
		/* No such node, or the node does not have status "okay". */
		LOG_ERR("No BME280 device found");
		return NULL;
	}

	if (!device_is_ready(dev)) {
		LOG_ERR("Device \"%s\" is not ready; check the driver initialization logs for errors.",
			dev->name);
		return NULL;
	}

	LOG_INF("Found device \"%s\", getting sensor data", dev->name);
	return dev;
}

int main(void)
{
	const struct device *dev = check_bme280_device();

	if (dev == NULL) {
		return 0;
	}

	LOG_INF("BME280 Sensor Demo for XIAO nRF54LM20A");
	LOG_INF("Reading temperature, humidity, and pressure every 2 seconds...");

	while (1) {
		uint8_t buf[128];
		int rc = sensor_read(&iodev, &ctx, buf, 128);

		if (rc != 0) {
			LOG_ERR("%s: sensor_read() failed: %d", dev->name, rc);
			return rc;
		}

		const struct sensor_decoder_api *decoder;
		rc = sensor_get_decoder(dev, &decoder);
		if (rc != 0) {
			LOG_ERR("%s: sensor_get_decoder() failed: %d", dev->name, rc);
			return rc;
		}

		uint32_t temp_fit = 0;
		struct sensor_q31_data temp_data = {0};
		decoder->decode(buf,
			(struct sensor_chan_spec) {SENSOR_CHAN_AMBIENT_TEMP, 0},
			&temp_fit, 1, &temp_data);

		uint32_t press_fit = 0;
		struct sensor_q31_data press_data = {0};
		decoder->decode(buf,
			(struct sensor_chan_spec) {SENSOR_CHAN_PRESS, 0},
			&press_fit, 1, &press_data);

		uint32_t hum_fit = 0;
		struct sensor_q31_data hum_data = {0};
		decoder->decode(buf,
			(struct sensor_chan_spec) {SENSOR_CHAN_HUMIDITY, 0},
			&hum_fit, 1, &hum_data);

		printk("\n=== BME280 Sensor Reading ===\n");
		printk("Temperature: %s%d.%d C\n",
			PRIq_arg(temp_data.readings[0].temperature, 6, temp_data.shift));
		printk("Pressure:    %s%d.%d kPa\n",
			PRIq_arg(press_data.readings[0].pressure, 6, press_data.shift));
		printk("Humidity:    %s%d.%d %%\n",
			PRIq_arg(hum_data.readings[0].humidity, 6, hum_data.shift));
		printk("==============================\n");

		k_sleep(K_MSEC(2000));
	}

	return 0;
}
