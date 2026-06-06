#define RELAY_IT_LIGHT   6
#define RELAY_STAFF      7
#define RELAY_IT_HVAC    5

const int RELAY_ON  = LOW;
const int RELAY_OFF = HIGH;

#define GA_IT_LIGHT    "1/0/1"
#define GA_IT_HVAC     "1/1/1"
#define GA_STAFF_LIGHT "1/0/2"

void applyRelay(int pin, int value) {
  digitalWrite(pin, (value == 1) ? RELAY_ON : RELAY_OFF);
}

void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);

  pinMode(RELAY_IT_LIGHT, OUTPUT);
  pinMode(RELAY_STAFF, OUTPUT);
  pinMode(RELAY_IT_HVAC, OUTPUT);

  digitalWrite(RELAY_IT_LIGHT, RELAY_OFF);
  digitalWrite(RELAY_STAFF, RELAY_OFF);
  digitalWrite(RELAY_IT_HVAC, RELAY_OFF);
}

void loop() {
  if (Serial1.available()) {
    String msg = Serial1.readStringUntil('\n');
    msg.trim();

    int comma = msg.indexOf(',');
    if (comma <= 0) return;

    String ga = msg.substring(0, comma);
    String valStr = msg.substring(comma + 1);

    int val = valStr.toInt();

    if (ga == GA_IT_LIGHT)
      applyRelay(RELAY_IT_LIGHT, val);

    else if (ga == GA_IT_HVAC)
      applyRelay(RELAY_IT_HVAC, val);

    else if (ga == GA_STAFF_LIGHT)
      applyRelay(RELAY_STAFF, val);
  }
}
