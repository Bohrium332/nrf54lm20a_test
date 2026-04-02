# BME280 Sensor Demo for XIAO nRF54LM20A

This demo reads temperature, humidity, and pressure data from a BME280 sensor via I2C.

## Hardware Connection

Connect the BME280 sensor to the XIAO nRF54LM20A board:

| BME280 Pin | XIAO nRF54LM20A Pin |
|------------|---------------------|
| VCC        | 3V3                 |
| GND        | GND                 |
| SCL        | D5 (P1.7)           |
| SDA        | D4 (P1.3)           |

## I2C Configuration

- I2C Bus: i2c22
- I2C Address: 0x76
- SDA Pin: P1.03 (D4)
- SCL Pin: P1.07 (D5)
- Clock Frequency: 400kHz (Fast Mode)

## Build and Flash

```bash
# Build the project
pio run

# Upload to board
pio run --target upload

# Monitor serial output
pio device monitor
```

## Expected Output

```
[00:00:00.000,000] <inf> bme280_demo: Found device "bme280", getting sensor data
[00:00:00.000,000] <inf> bme280_demo: BME280 Sensor Demo for XIAO nRF54LM20A
[00:00:00.000,000] <inf> bme280_demo: Reading temperature, humidity, and pressure every 2 seconds...

=== BME280 Sensor Reading ===
Temperature: 25.123456 C
Pressure:    101.325000 kPa
Humidity:    45.678901 %
==============================
```

## References

- [Zephyr BME280 Sample](https://docs.zephyrproject.org/latest/samples/sensor/bme280/README.html)
- [BME280 Datasheet](https://www.bosch-sensortec.com/products/environmental-sensors/humidity-sensors-bme280/)
