# W25Q128JV SPI 8 MHz Demo for XIAO nRF54LM20A

This demo drives a `W25Q128JV / 25Q128JVS0` over `spi23` and performs one
simple validation cycle:

1. Print the configured SPI clock frequency as `8 MHz`.
2. Read and verify the flash JEDEC ID.
3. Erase the first demo sector.
4. Write 32 bytes of test data.
5. Read the data back and verify it matches.

## Wiring

- `SCK` -> `D8`  (`P1.04`)
- `MISO` -> `D9`  (`P1.05`)
- `MOSI` -> `D10` (`P1.06`)
- `CS` -> `D1` (`P1.31`)

`WP#` and `HOLD#` are not controlled by the MCU in this demo. They must be
held in their inactive high state by the external hardware.

## Files

- `platformio.ini`: PlatformIO environment for `seeed-xiao-nrf54lm20a`
- `src/main.c`: JEDEC ID read, erase, write, read-back, and verify flow
- `zephyr/boards/xiao_nrf54lm20a_nrf54lm20a_cpuapp.overlay`: enables the
  `spi23` flash node and its demo partition
- `zephyr/prj.conf`: enables SPI, SPI NOR, flash, and logging

## Build

```bash
cd w25q128jv-spi-8m-demo
pio run
```

## Flash

```bash
cd w25q128jv-spi-8m-demo
pio run -t upload
```

## Expected Log

```text
[00:00:00.000,000] <inf> w25q128jv_demo: SPI clock: 8 MHz (8000000 Hz)
[00:00:00.000,000] <inf> w25q128jv_demo: JEDEC ID: EF 40 18
[00:00:00.000,000] <inf> w25q128jv_demo: Erase success
[00:00:00.000,000] <inf> w25q128jv_demo: Write success: 32 bytes
[00:00:00.000,000] <inf> w25q128jv_demo: Verify OK
```
