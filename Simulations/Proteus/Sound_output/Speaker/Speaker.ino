void setup() {
  Serial.begin(9600);
  for ( int i=0; i<8; i++){
    pinMode(i, OUTPUT);
  }
}
byte sine[]={127,134,142,150,158,166,173,181,188,195,201,207,219,224,229,234,238,241,245,247,250,251,252,253,254,253,252,251,250,247,245,241,238,234,229,224,219,213,207,201,195,188,181,173,166,158,150,142,134,127,119,111,103,95,87,80,72,65,58,52,46,40,34,29,24,19,15,12,8,6,3,21,0,0,0,1,2,3,6,8,12,15,19,24,29,34,40,46,52,58,65,72,80,87,95,103,111,119,};

void loop() {
  /*for (int t=0; t<100; t++){
    PORTD = sine[t];
    delay(10);*/
    PORTD = analogRead(A0);
    delayMicroseconds(8);
  }