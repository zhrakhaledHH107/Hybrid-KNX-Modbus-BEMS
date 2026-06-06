import minimalmodbus
import serial
import time

PORT = "/dev/ttyUSB0"
BAUDRATE = 19200
SLAVE_ID = 10

instrument = minimalmodbus.Instrument(PORT, SLAVE_ID)
instrument.serial.baudrate = BAUDRATE
instrument.serial.bytesize = 8
instrument.serial.parity = serial.PARITY_NONE
instrument.serial.stopbits = 1
instrument.serial.timeout = 1

SENSORS = {
    "Oil Pressure": {"address": 1024, "scaling": 1, "unit": "kPa"},
    "Coolant Temperature": {"address": 1025, "scaling": 1, "unit": "C"},
    "Fuel Level": {"address": 1027, "scaling": 1, "unit": "%"},
    "Charge Alternator Voltage": {"address": 1028, "scaling": 0.1, "unit": "V"},
    "Engine Battery Voltage": {"address": 1029, "scaling": 0.1, "unit": "V"},
}

while True:
    timestamp = time.strftime("%Y-%m-%d %H:%M:%S")
    print(f"\n=== DSE7320 MKII Monitoring at {timestamp} ===")

    for name, info in SENSORS.items():
        try:
            value = instrument.read_register(info["address"], 0, functioncode=3)
            scaled_value = value * info["scaling"]
            print(f"{name}: {scaled_value} {info['unit']}")
        except Exception as e:
            print(f"{name}: No Data ({e})")

    time.sleep(2)
