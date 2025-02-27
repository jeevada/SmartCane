#define TRIG_PIN 5
#define ECHO_PIN 18
#define MOTOR_PIN 25
#define BUZZER_PIN 26

void setup() {
  Serial.begin(115200);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(MOTOR_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
}

long getDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH);
  long distance = duration * 0.034 / 2;  // Convert time to cm
  return distance;
}

void loop() {
  long distance = getDistance();
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  if (distance > 0 && distance <= 10) {
    digitalWrite(MOTOR_PIN, HIGH); // Turn on vibration motor
  } else {
    digitalWrite(MOTOR_PIN, LOW);  // Turn off vibration motor
  }

  if (distance > 0 && distance <= 15) {
    digitalWrite(BUZZER_PIN, HIGH); // Turn on buzzer
  } else {
    digitalWrite(BUZZER_PIN, LOW);  // Turn off buzzer
  }

  delay(500);  // Small delay for stability
}

