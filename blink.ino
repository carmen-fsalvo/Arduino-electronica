int MicVolume=0;

void setup() {
  Serial.begin(9600);
}

void loop() {
 MicVolume = (analogRead(0));
 Serial.println(MicVolume);
 delay(5);
}
