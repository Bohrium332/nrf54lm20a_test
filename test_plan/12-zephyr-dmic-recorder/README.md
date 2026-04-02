How to build PlatformIO based project
=====================================

1. [Install PlatformIO Core](https://docs.platformio.org/page/core.html)
2. Download [development platform with examples](https://github.com/Seeed-Studio/platform-seeedboards/archive/refs/heads/main.zip)
3. Extract ZIP archive
4. Run these commands:

```shell
# Change directory to example
$ cd platform-seeedboards/examples/zephyr-dmic-recorder

# Build project
$ pio run

# Upload firmware
$ pio run --target upload

# Build specific environment
$ pio run -e seeed-xiao-nrf54l15

# Upload firmware for the specific environment
$ pio run -e seeed-xiao-nrf54l15 --target upload

# Clean build files
$ pio run --target clean
```

Tested on XIAO nRF54LM20A + MSM261DGT006
========================================

This project has been adapted and tested for `Seeed Studio XIAO nRF54LM20A`
with a `MSM261DGT006` PDM microphone.

Hardware wiring
---------------

- `MSM261DGT006 CLK` -> `P1.13`
- `MSM261DGT006 DIN` -> `P1.14`
- `MSM261DGT006 VDD` -> board microphone power rail driven by `nPM1300 LDO1`
- `MSM261DGT006 GND` -> `GND`

Notes
-----

- The firmware enables `nPM1300` power before starting DMIC capture.
- Audio data is streamed over the board USB serial port exposed as
  `/dev/ttyACM0` on Linux.
- The current firmware records one 10-second clip automatically after boot,
  and `SW0` can trigger another recording.

Build and flash on XIAO nRF54LM20A
----------------------------------

```shell
cd /home/zzd/project/Nordic/nrf54lm20a/zephyr-dmic-recorder

# Build and Flash with the on-board CMSIS-DAP probe
pio run -t upload
```

Record audio on Linux
---------------------

```shell
cd /home/zzd/project/Nordic/nrf54lm20a/zephyr-dmic-recorder

python3 scripts/record.py -p /dev/ttyACM0 -b 921600 -o test.wav
```

Recommended test flow:

1. Start `record.py`.
2. Wait a few seconds for the automatic capture after boot.
3. If no capture starts, press `SW0` once.
4. Wait about 10 seconds for capture to complete.
5. Verify the WAV file:

```shell
ls -lh test.wav
file test.wav
```

Expected successful script output includes lines similar to:

```text
Synchronized (START packet received). Receiving audio data...
Received 320000 bytes of audio data.
Transfer verified (END packet received).
WAV file saved.
```
