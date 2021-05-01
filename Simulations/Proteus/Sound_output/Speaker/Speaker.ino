void setup() {
  //Serial.begin(9600);
  for ( int i=0; i<8; i++){
    pinMode(i, OUTPUT);
  }
}

void loop() {
    PORTD = analogRead(A0);
    delayMicroseconds(1);
}
