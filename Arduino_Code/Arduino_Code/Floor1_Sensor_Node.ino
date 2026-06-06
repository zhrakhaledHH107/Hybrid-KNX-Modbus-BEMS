1) Python_Code/DSE7320_Modbus_Reader.py
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

  
2) Arduino_Code/Floor1_Sensor_Node.ino
#define LDR_IT A0
#define LDR_STAFF A1

const int ON_TH = 60;
const int OFF_TH = 70;
const unsigned long IT_HVAC_DELAY_MS = 5000;

bool itLightState = false;
bool itHVACState = false;
bool staffLightState = false;

bool hvacWaiting = false;
bool hvacTargetState = false;
unsigned long hvacChangeStart = 0;

void sendTelegram(const char* ga, int value) {
  Serial1.print(ga);
  Serial1.print(",");
  Serial1.println(value);

  Serial.print("TX ");
  Serial.print(ga);
  Serial.print(" -> ");
  Serial.println(value == 1 ? "ON" : "OFF");
}

bool updateDemandState(int sensorValue, bool currentState) {
  if (!currentState && sensorValue <= ON_TH) return true;
  if (currentState && sensorValue >= OFF_TH) return false;
  return currentState;
}

void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);
}

void loop() {
  unsigned long now = millis();

  int itLux = analogRead(LDR_IT);
  int staffLux = analogRead(LDR_STAFF);

  bool newItLightState = updateDemandState(itLux, itLightState);

  if (newItLightState != itLightState) {
    itLightState = newItLightState;
    sendTelegram("1/0/1", itLightState ? 1 : 0);

    hvacWaiting = true;
    hvacTargetState = itLightState;
    hvacChangeStart = now;
  }

  if (hvacWaiting && (now - hvacChangeStart >= IT_HVAC_DELAY_MS)) {
    if (itHVACState != hvacTargetState) {
      itHVACState = hvacTargetState;
      sendTelegram("1/1/1", itHVACState ? 1 : 0);
    }
    hvacWaiting = false;
  }

  bool newStaffLightState = updateDemandState(staffLux, staffLightState);

  if (newStaffLightState != staffLightState) {
    staffLightState = newStaffLightState;
    sendTelegram("1/0/2", staffLightState ? 1 : 0);
  }

  delay(20);
}


3) Arduino_Code/Floor1_Master.ino
#define RELAY_IT_LIGHT 7
#define RELAY_IT_HVAC 6
#define RELAY_STAFF_LIGHT 5

const int RELAY_ON = LOW;
const int RELAY_OFF = HIGH;

bool itLightState = false;
bool itHVACState = false;
bool staffLightState = false;

void sendStatusToPi() {
  Serial.print("F1,IT_LIGHT=");
  Serial.print(itLightState ? 1 : 0);
  Serial.print(",IT_HVAC=");
  Serial.print(itHVACState ? 1 : 0);
  Serial.print(",STAFF_LIGHT=");
  Serial.println(staffLightState ? 1 : 0);
}

void setRelay(String load, String state) {
  bool on = (state == "ON");

  if (load == "IT_LIGHT") {
    itLightState = on;
    digitalWrite(RELAY_IT_LIGHT, on ? RELAY_ON : RELAY_OFF);
  } else if (load == "IT_HVAC") {
    itHVACState = on;
    digitalWrite(RELAY_IT_HVAC, on ? RELAY_ON : RELAY_OFF);
  } else if (load == "STAFF_LIGHT") {
    staffLightState = on;
    digitalWrite(RELAY_STAFF_LIGHT, on ? RELAY_ON : RELAY_OFF);
  }

  sendStatusToPi();
}

void readCommandFromPi() {
  if (Serial.available()) {
    String cmd = Serial.readStringUntil('\n');
    cmd.trim();

    int commaIndex = cmd.indexOf(',');
    if (commaIndex == -1) return;

    String load = cmd.substring(0, commaIndex);
    String state = cmd.substring(commaIndex + 1);
    state.trim();

    if (state == "ON" || state == "OFF") {
      setRelay(load, state);
    }
  }
}

void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);

  pinMode(RELAY_IT_LIGHT, OUTPUT);
  pinMode(RELAY_IT_HVAC, OUTPUT);
  pinMode(RELAY_STAFF_LIGHT, OUTPUT);

  digitalWrite(RELAY_IT_LIGHT, RELAY_OFF);
  digitalWrite(RELAY_IT_HVAC, RELAY_OFF);
  digitalWrite(RELAY_STAFF_LIGHT, RELAY_OFF);

  sendStatusToPi();
}

void loop() {
  if (Serial1.available()) {
    String msg = Serial1.readStringUntil('\n');
    msg.trim();

    if (msg.length() > 0) {
      Serial.println(msg);
    }
  }

  readCommandFromPi();
}
