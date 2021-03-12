
#define Echo A1
#define Trig A2
float hieght = 1.5;

void setup() {
  pinMode(Echo, INPUT);
  pinMode(Trig, OUTPUT);
  Serial.begin(9600);
  digitalWrite(Trig,LOW);
}

void loop() {
  Serial.println(readUS());
  delay(1000);
}

float readUS(){
  digitalWrite(Trig, LOW);
  delayMicroseconds(2);
  digitalWrite(Trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(Trig, LOW);
  int val = pulseIn(Echo, HIGH);
  return (val*0.034/2);
}

float getLevel(){
  return ((hieght-readUS())/hieght*100);
}
