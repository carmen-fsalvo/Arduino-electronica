
int MicVolume = 0;

void setup() {
  Serial.begin(115200);
 
}

void loop() {
 
  MicVolume = (analogRead(0));
  Serial.println(MicVolume);
  delay(0);
}
