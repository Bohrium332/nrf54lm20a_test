/*
 * ADC demo for XIAO nRF54LM20A
 *
 * Sampled analog inputs:
 *   A0 -> P1.00
 *   A1 -> P1.31
 *   A2 -> P1.30
 *   A3 -> P1.29
 *   A4 -> P1.06
 *   A5 -> P1.05
 *   A6 -> P1.04
 *   A7 -> P1.03
 *
 * A8 (P1.07) is not included here because the current board DTS exposes
 * eight SAADC channels only.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/drivers/adc.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(adc_demo, LOG_LEVEL_INF);

#define SAMPLE_INTERVAL_MS 1500
#define ADC_CHANNEL_COUNT 8

#define ADC_NODE DT_PATH(zephyr_user)

static const struct adc_dt_spec adc_channels[] = {
	ADC_DT_SPEC_GET_BY_IDX(ADC_NODE, 0),
	ADC_DT_SPEC_GET_BY_IDX(ADC_NODE, 1),
	ADC_DT_SPEC_GET_BY_IDX(ADC_NODE, 2),
	ADC_DT_SPEC_GET_BY_IDX(ADC_NODE, 3),
	ADC_DT_SPEC_GET_BY_IDX(ADC_NODE, 4),
	ADC_DT_SPEC_GET_BY_IDX(ADC_NODE, 5),
	ADC_DT_SPEC_GET_BY_IDX(ADC_NODE, 6),
	ADC_DT_SPEC_GET_BY_IDX(ADC_NODE, 7),
};

static const char *const adc_labels[ADC_CHANNEL_COUNT] = {
	"A0(P1.00)",
	"A1(P1.31)",
	"A2(P1.30)",
	"A3(P1.29)",
	"A4(P1.06)",
	"A5(P1.05)",
	"A6(P1.04)",
	"A7(P1.03)",
};

static int16_t adc_samples[ADC_CHANNEL_COUNT];

static int setup_adc_channels(void)
{
	for (size_t i = 0; i < ARRAY_SIZE(adc_channels); ++i) {
		if (!adc_is_ready_dt(&adc_channels[i])) {
			LOG_ERR("%s ADC device not ready", adc_labels[i]);
			return -ENODEV;
		}

		int ret = adc_channel_setup_dt(&adc_channels[i]);
		if (ret != 0) {
			LOG_ERR("adc_channel_setup_dt failed for %s: %d", adc_labels[i], ret);
			return ret;
		}
	}

	return 0;
}

static void log_channel_value(size_t index, int16_t raw)
{
	int32_t mv = raw;
	int ret = adc_raw_to_millivolts_dt(&adc_channels[index], &mv);

	if (ret == 0) {
		LOG_INF("%s raw=%d voltage=%d mV", adc_labels[index], raw, mv);
	} else {
		LOG_INF("%s raw=%d voltage=N/A (ret=%d)", adc_labels[index], raw, ret);
	}
}

int main(void)
{
	int ret;

	LOG_INF("ADC demo started");
	LOG_INF("Sampling A0-A7 every %d ms", SAMPLE_INTERVAL_MS);
	LOG_INF("A8(P1.07) is not sampled in this build");

	ret = setup_adc_channels();
	if (ret != 0) {
		return ret;
	}

	while (1) {
		for (size_t i = 0; i < ARRAY_SIZE(adc_channels); ++i) {
			struct adc_sequence sequence = {
				.buffer = &adc_samples[i],
				.buffer_size = sizeof(adc_samples[i]),
			};

			(void)adc_sequence_init_dt(&adc_channels[i], &sequence);

			ret = adc_read_dt(&adc_channels[i], &sequence);
			if (ret != 0) {
				LOG_ERR("adc_read_dt failed for %s: %d", adc_labels[i], ret);
				continue;
			}

			log_channel_value(i, adc_samples[i]);
		}

		LOG_INF("----------------------------------------");
		k_msleep(SAMPLE_INTERVAL_MS);
	}

	return 0;
}
