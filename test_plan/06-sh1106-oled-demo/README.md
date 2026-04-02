# SH1106 OLED Demo for XIAO nRF54LM20A

This demo drives a 1.3-inch `SH1106 128x64 SPI OLED` with Zephyr's built-in
display driver.

## Recommended Wiring

- `SCK` -> `D8`  (`P1.04`)
- `SDA` -> `D10` (`P1.06`, SPI MOSI)
- `RST` -> `D3`  (`P1.29`)
- `DC`  -> `D2`  (`P1.30`)
- `CS`  -> `D1`  (`P1.31`)
- `VCC` -> `3V3`
- `GND` -> `GND`

Note: this SPI OLED is write-only for the demo, so `MISO` is not used.

## Build with West

```bash
west build -b xiao_nrf54lm20a/nrf54lm20a/cpuapp \
  /home/zzd/project/Nordic/nrf54lm20a/sh1106-oled-demo/zephyr \
  --build-dir /tmp/sh1106-oled-demo-build \
  -- -DBOARD_ROOT=/disk/E/git_file/platform-seeedboards/zephyr
```

Adjust `BOARD_ROOT` if your local `platform-seeedboards` checkout is elsewhere.

## What You Should See

The display cycles through:

- border + diagonal cross
- full white
- horizontal stripes
- vertical stripes
- moving filled box

If the screen stays blank, first re-check `DC`, `RST`, and `CS`.
