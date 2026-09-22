
const int buzzerPin = 4;
int MicVolume = 0;

void setup() {
  Serial.begin(115200);
  pinMode (buzzerPin,OUTPUT);
}

void loop() {
 
  tone(buzzerPin,1000);
  delay(0);
  MicVolume = (analogRead(0));
  Serial.println(MicVolume);
  delay(0);
}
