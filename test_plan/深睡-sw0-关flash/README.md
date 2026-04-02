# PY25Q64HA System Off Demo for XIAO nRF54LM20A

This demo puts the external `PY25Q64HA-UXH-IR` SPI NOR flash into deep power-down
before entering `system_off`.

Implementation details:

- The board DTS already marks `py25q64ha` as supporting deep power-down.
- The Puya datasheet defines:
  - Deep Power-down: `B9h`
  - Release from Deep Power-down: `ABh`
- The application uses Zephyr device PM instead of sending raw SPI commands:
  - `pm_device_action_run(flash_dev, PM_DEVICE_ACTION_SUSPEND)`

When the `jedec,spi-nor` driver receives `PM_DEVICE_ACTION_SUSPEND`, it issues
the deep power-down command and waits for the configured DPD timing.

## Low-Power Troubleshooting

### Problem

Initial implementation measured ~80uA in system_off, far above the expected 2-3uA. Three issues were identified:

#### 1. DPD verification wakes the flash back up

After sending the DPD command, the code read the JEDEC ID to "verify" that the flash entered DPD. However, reading JEDEC ID pulls CS# low, which is exactly the condition that wakes the flash from DPD. The flash was put to sleep and immediately woken up.

**Fix:** Remove all verification logic. Trust the Zephyr spi-nor driver's `PM_DEVICE_ACTION_SUSPEND` to correctly send the 0xB9 DPD command (the DTS declares `has-dpd`).

#### 2. SPI GPIO pins float during system_off

After the SPI bus is suspended, pins become high-impedance. The flash's input pins (CS#, SCK, MOSI, WP#, HOLD#) float, causing leakage current. The PY25Q64HA datasheet requires all inputs at 0V or Vcc during DPD to achieve the 0.2uA typical current.

**Fix:** After suspending the SPI bus, manually configure all 6 GPIO pins to deterministic levels:

| Pin   | Function | Config          | Reason                        |
|-------|----------|-----------------|-------------------------------|
| P2.05 | CS#      | OUTPUT HIGH     | Keep flash deselected         |
| P2.00 | HOLD#    | OUTPUT HIGH     | Inactive                      |
| P2.03 | WP#      | OUTPUT HIGH     | Inactive                      |
| P2.01 | SCK      | OUTPUT LOW      | Deterministic level           |
| P2.02 | MOSI     | OUTPUT LOW      | Deterministic level           |
| P2.04 | MISO     | INPUT PULL_DOWN | Flash output, pull to 0V      |

#### 3. Missing hold-gpios in DTS overlay

The board DTS defines `wp-gpios` but not `hold-gpios`, leaving the HOLD# pin unmanaged by the driver.

**Fix:** Add `hold-gpios = <&gpio2 0 GPIO_ACTIVE_LOW>` to the overlay.

### Result

After all three fixes, system_off current dropped to ~2-3uA (nRF54LM20A ~2uA + PY25Q64HA DPD ~0.2uA).

### Key Takeaway

For external flash low-power: sending the DPD command alone is not enough. **All physical pins must be driven to deterministic levels.** Floating pins are a common source of hidden leakage current.

## Wiring

- `SPI_CS` -> `P2.05`
- `SPI_IO0` -> `P2.02`
- `SPI_IO1` -> `P2.04`
- `SPI_IO2` -> `P2.03`
- `SPI_IO3` -> `P2.00`
- `SPI_CLK` -> `P2.01`

## Behavior

1. Boot and print reset cause.
2. Configure `sw0` as a wakeup source.
3. Suspend the external flash device (enters deep power-down via spi-nor driver).
4. Suspend the SPI bus.
5. Configure all SPI GPIO pins to deterministic levels for minimal leakage.
6. Suspend the console device.
7. Enter `system_off`.
8. Press `sw0` to restart.

## Build

```bash
cd py25q64ha-system-off
pio run
```

## Flash

```bash
cd py25q64ha-system-off
pio run -t upload
```

## Expected log

```text
seeed_xiao_nrf54lm20a system off demo with PY25Q64HA
Flash device: SPI_NOR
Suspending external flash (entering DPD)...
External flash suspended.
SPI bus suspended.
SPI GPIO pins configured for system_off.
Entering system off; press sw0 to restart
```
