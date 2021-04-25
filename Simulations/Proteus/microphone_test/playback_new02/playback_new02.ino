int inputPin = A0;
int outputPin = 6;
int sinValue;
int outputVal;

void setup() {
  // put your setup code here, to run once:
pinMode(inputPin,INPUT);
pinMode(outputPin,OUTPUT);
Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
sinValue = analogRead(inputPin);
outputVal = map(sinValue,0,1023,0,255);
Serial.println(outputVal);
analogWrite(outputPin,outputVal);
}
