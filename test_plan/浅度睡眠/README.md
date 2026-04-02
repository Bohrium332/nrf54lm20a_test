XIAO nRF54LM20A shallow sleep demo
==================================

This demo keeps the application context alive and lets the CPU enter the
Zephyr idle state while waiting for the BOOT button.

Behavior
--------

1. Firmware prints a boot banner.
2. The blue LED turns off before sleep.
3. The main thread blocks on a semaphore, so the CPU can enter idle/WFI.
4. Press the BOOT button to trigger a GPIO interrupt and wake the CPU.
5. The blue LED turns on briefly and the wake counter is printed.

Notes
-----

- This is shallow sleep, not `System OFF`.
- SRAM and CPU context are retained.
- Wake-up returns to the same point in code, so the MCU does not reboot.
- On XIAO nRF54LM20A, `sw0` maps to the BOOT button (`P0.09` in the board DTS).

Build
-----

```shell
cd zephyr-shallow-sleep
pio run -e seeed-xiao-nrf54lm20a
```

USB log
-------

- Console is routed to USB CDC ACM.
- After reset, open the board's USB serial port on the host to assert DTR.
- Logs appear after the host opens the port.
