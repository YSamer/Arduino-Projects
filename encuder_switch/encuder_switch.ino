#define A 5
#define B 6
int counter = 0;
int aState;
int lastState;


void setup() {
  Serial.begin(9600);
  pinMode(A,INPUT);
  pinMode(B,INPUT);
  lastState = digitalRead(A);
}

void loop() {
  aState = digitalRead(A);
  if(aState != lastState){
    if(digitalRead(B)!= aState){
      counter++;
    }else{
      counter--;
    }
  }
  lastState = aState;
  Serial.println(counter);
  
}
