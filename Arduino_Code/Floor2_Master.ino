#define RELAY_MANAGER_LIGHT 7
#define RELAY_MANAGER_HVAC 6
#define RELAY_MEETING_LIGHT 5

const int RELAY_ON = LOW;
const int RELAY_OFF = HIGH;

void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);

  pinMode(RELAY_MANAGER_LIGHT, OUTPUT);
  pinMode(RELAY_MANAGER_HVAC, OUTPUT);
  pinMode(RELAY_MEETING_LIGHT, OUTPUT);

  digitalWrite(RELAY_MANAGER_LIGHT, RELAY_OFF);
  digitalWrite(RELAY_MANAGER_HVAC, RELAY_OFF);
  digitalWrite(RELAY_MEETING_LIGHT, RELAY_OFF);
}

void loop() {
  if (Serial1.available()) {
    String msg = Serial1.readStringUntil('\n');
    msg.trim();

    int commaIndex = msg.indexOf(',');
    if (commaIndex == -1) return;

    String ga = msg.substring(0, commaIndex);
    String valStr = msg.substring(commaIndex + 1);
    valStr.trim();

    if (!(valStr == "0" || valStr == "1")) return;

    int val = valStr.toInt();

    if (ga == "2/0/1") {
      digitalWrite(RELAY_MANAGER_LIGHT, val == 1 ? RELAY_ON : RELAY_OFF);
    }
    else if (ga == "2/1/1") {
      digitalWrite(RELAY_MANAGER_HVAC, val == 1 ? RELAY_ON : RELAY_OFF);
    }
    else if (ga == "2/0/2") {
      digitalWrite(RELAY_MEETING_LIGHT, val == 1 ? RELAY_ON : RELAY_OFF);
    }
  }
}
