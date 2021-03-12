
long A=0;
double T = 0;

void setup() {
  pinMode(A0,INPUT);
  Serial.begin(9600);
  
}

void loop() {
  for(int i=0;i<100;i++){
    A+=analogRead(A0);
  }
  A/=100;
  T = (A-747.7)/-9.27;
  Serial.println(T);
  A=0;
  delay(1000);
}
