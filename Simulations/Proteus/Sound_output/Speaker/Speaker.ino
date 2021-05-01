void setup() {
  //Serial.begin(9600);
  for ( int i=0; i<8; i++){
    pinMode(i, OUTPUT);
  }
}

void loop() {
<<<<<<< HEAD
  /*for (int t=0; t<100; t++){
    PORTD = sine[t];
    delay(10);*/
    int t = analogRead(A0);
    PORTD = t;
    delayMicroseconds(8);
  }
=======
    PORTD = analogRead(A0);
    delayMicroseconds(1);
}
>>>>>>> cd4e81e2fb0b81f9a873a78d6e9de7aff86958d0
