#include <SoftwareSerial.h>

SoftwareSerial knxSerial(10, 11);

#define LDR_MANAGER A0
#define LDR_MEETING A1

const int ON_TH = 80;
const int OFF_TH = 140;

const unsigned long HVAC_DELAY_MS = 5000;

bool managerLightState = false;
bool managerHVACState = false;
bool meetingLightState = false;

bool hvacTimerActive = false;
bool hvacTargetState = false;
unsigned long hvacTimerStart = 0;

void sendTelegram(const char* ga, int value) {
  knxSerial.print(ga);
  knxSerial.print(",");
  knxSerial.println(value);
}

int readFiltered(int pin) {
  long sum = 0;

  for (int i = 0; i < 10; i++) {
    sum += analogRead(pin);
    delay(2);
  }

  return sum / 10;
}

bool updateDemandState(int sensorValue, bool currentState) {
  if (!currentState && sensorValue <= ON_TH) return true;
  if (currentState && sensorValue >= OFF_TH) return false;
  return currentState;
}

void setup() {
  knxSerial.begin(9600);
  Serial.begin(9600);
}

void loop() {
  unsigned long now = millis();

  int managerLux = readFiltered(LDR_MANAGER);
  int meetingLux = readFiltered(LDR_MEETING);

  bool newManagerLightState =
      updateDemandState(managerLux, managerLightState);

  if (newManagerLightState != managerLightState) {
    managerLightState = newManagerLightState;
    sendTelegram("2/0/1", managerLightState ? 1 : 0);

    hvacTargetState = managerLightState;
    hvacTimerStart = now;
    hvacTimerActive = true;
  }

  if (hvacTimerActive && (now - hvacTimerStart >= HVAC_DELAY_MS)) {
    if (managerHVACState != hvacTargetState) {
      managerHVACState = hvacTargetState;
      sendTelegram("2/1/1", managerHVACState ? 1 : 0);
    }

    hvacTimerActive = false;
  }

  bool newMeetingLightState =
      updateDemandState(meetingLux, meetingLightState);

  if (newMeetingLightState != meetingLightState) {
    meetingLightState = newMeetingLightState;
    sendTelegram("2/0/2", meetingLightState ? 1 : 0);
  }

  delay(50);
}
