#define BUTTON 5
#define LED 4

void setup() {
  Serial.begin(115200);
  pinMode(BUTTON, INPUT_PULLDOWN);
  pinMode(LED, OUTPUT);
}

void loop() {
  int state = digitalRead(BUTTON);

  if (state == HIGH) {
    digitalWrite(LED, HIGH);
  } else {
    digitalWrite(LED, LOW);
  }

  delay(100);
}
