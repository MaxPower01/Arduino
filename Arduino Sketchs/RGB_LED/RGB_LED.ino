const int LED_YELLOW = 2;
const int LED_RED = 3;
const int DELAY_TIME = 1000;

void setup() {
  // put your setup code here, to run once:
  pinMode(LED_YELLOW, OUTPUT);
  pinMode(LED_RED, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(LED_YELLOW, HIGH);
  //digitalWrite(LED_RED, HIGH);
  delay(DELAY_TIME);
  digitalWrite(LED_YELLOW, LOW);
  //digitalWrite(LED_RED, LOW);
  delay(DELAY_TIME);
}
