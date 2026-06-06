# DSE7320 Modbus Register Map

The complete decimal Modbus address can be calculated as:

Decimal Address = (GenComm Page × 256) + Register Offset

## Generator Electrical Instrumentation

| Variable / Function | GenComm Page | Register Offset | Data Type / Scaling Factor |
|---|---:|---:|---|
| Generator Frequency | 4 | 7 | 0.1 Hz (16-bit) |
| Generator L1-N Voltage | 4 | 8-9 | 0.1 V (32-bit) |
| Generator L2-N Voltage | 4 | 10-11 | 0.1 V (32-bit) |
| Generator L3-N Voltage | 4 | 12-13 | 0.1 V (32-bit) |
| Generator L1-L2 Voltage | 4 | 14-15 | 0.1 V (32-bit) |
| Generator L1 Current | 4 | 20-21 | 0.1 A (32-bit) |
| Generator L2 Current | 4 | 22-23 | 0.1 A (32-bit) |
| Generator L3 Current | 4 | 24-25 | 0.1 A (32-bit) |
| Generator Total Watts | 6 | 0-1 | 1 W (32-bit) |
| Generator Total VA | 6 | 8-9 | 1 VA (32-bit) |
| Generator Total Power Factor | 6 | 28 | 0.001 (16-bit) |

## Engine / Mechanical Instrumentation

| Variable / Function | GenComm Page | Register Offset | Data Type / Scaling Factor |
|---|---:|---:|---|
| Engine Oil Pressure | 4 | 0 | 1 kPa (16-bit) |
| Coolant Temperature | 4 | 1 | 1 °C (16-bit) |
| Engine Oil Temperature | 4 | 2 | 1 °C (16-bit) |
| Fuel Level | 4 | 3 | 1 % (16-bit) |
| Engine Battery Voltage | 4 | 5 | 0.1 V (16-bit) |
| Engine Speed | 4 | 6 | 1 RPM (16-bit) |

## System Control Keys

| Function | Key Value |
|---|---:|
| Select STOP mode | 35700 |
| Select AUTO mode | 35701 |
| Telemetry Start | 35732 |
| Cancel Telemetry Start | 35733 |
