#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/pwm.h>
#include <zephyr/logging/log.h>

// Register log module
LOG_MODULE_REGISTER(pwm_example, CONFIG_LOG_DEFAULT_LEVEL);

// Define PWM period as 1 millisecond (1,000,000 nanoseconds)
// This corresponds to a 1 kHz PWM frequency
#define PWM_PERIOD_NS 1000000UL
#define PWM_PULSE_NS  (PWM_PERIOD_NS / 2U)
#define PWM_CHANNEL   0U

static const struct device *const pwm_dev = DEVICE_DT_GET(DT_NODELABEL(pwm20));

int main(void)
{
    int ret;

    LOG_INF("Starting Zephyr PWM constant-duty example...");

    // Check if PWM device is ready
    if (!device_is_ready(pwm_dev)) {
        LOG_ERR("Error: PWM device %s is not ready", pwm_dev->name);
        return 0;
    }

    LOG_INF("PWM Period set to %lu ns (1kHz frequency)", PWM_PERIOD_NS);
    LOG_INF("PWM duty cycle fixed at 50%% on D0");

    ret = pwm_set(pwm_dev, PWM_CHANNEL, PWM_PERIOD_NS, PWM_PULSE_NS,
              PWM_POLARITY_NORMAL);
    if (ret < 0) {
        LOG_ERR("Error %d: failed to set PWM duty cycle", ret);
        return 0;
    }

    while (1) {
        k_sleep(K_FOREVER);
    }

    return 0;
}
