How to build PlatformIO based project
=====================================

1. [Install PlatformIO Core](https://docs.platformio.org/page/core.html)
2. Download [development platform with examples](https://github.com/Seeed-Studio/platform-seeedboards/archive/refs/heads/main.zip)
3. Extract ZIP archive
4. Run these commands:

```shell
# Change directory to example
$ cd platform-seeedboards/examples/zephyr-lowpower

# Build project
$ pio run

# Upload firmware
$ pio run --target upload

# Build specific environment
$ pio run -e seeed-xiao-nrf54l15

# Upload firmware for the specific environment
$ pio run -e seeed-xiao-nrf54l15 --target upload

# Build XIAO nRF54LM20A environment
$ pio run -e seeed-xiao-nrf54lm20a

# Upload firmware for XIAO nRF54LM20A
$ pio run -e seeed-xiao-nrf54lm20a --target upload

# Clean build files
$ pio run --target clean
## GRTC Wakeup Variant

`zephyr-lowpower-grtc-wakeup` is based on `zephyr-lowpower`, but it enters `System OFF`
and uses GRTC to wake the SoC automatically after 2 seconds.

Expected log flow:

```text
xiao_nrf54lm20a/nrf54lm20a/cpuapp system off demo
Raw reset cause: 0x...
Entering system off; wait 2 seconds to restart
```

After wakeup, the reset cause should report:

```text
Wakeup from System OFF by GRTC.
```
