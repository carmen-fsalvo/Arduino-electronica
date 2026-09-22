const int buzzerPin = 4;
unsigned int frec = 440;
float MicVolume = 0.;

void setup() {
  Serial.begin(9600);
  pinMode(buzzerPin, OUTPUT);
}

void loop() {
  tone(buzzerPin, frec);

  MicVolume = analogRead(A0);
  Serial.println(MicVolume);

  delay(2);
}