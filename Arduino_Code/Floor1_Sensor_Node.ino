#define LDR_IT     A0
#define LDR_STAFF  A1

const int ON_TH  = 60;
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
}

bool updateDemandState(int sensorValue, bool currentState) {
  if (!currentState && sensorValue <= ON_TH) return true;
  else if (currentState && sensorValue >= OFF_TH) return false;
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

  bool newStaffLightState =
      updateDemandState(staffLux, staffLightState);

  if (newStaffLightState != staffLightState) {
    staffLightState = newStaffLightState;
    sendTelegram("1/0/2", staffLightState ? 1 : 0);
  }

  delay(20);
}
