
unsigned long lastTime = 0;
int i = 0;

void setup() {
  pinMode(A0,OUTPUT);
  pinMode(A1,OUTPUT);
  pinMode(A2,OUTPUT);
  pinMode(A4,OUTPUT);
  pinMode(A3,INPUT);

  //digitalWrite(A0,HIGH);
  digitalWrite(A0,LOW);
  digitalWrite(A1,LOW);
  digitalWrite(A2,LOW);
  digitalWrite(A4,LOW);

  while((millis()-lastTime)<1250);
  digitalWrite(A0,HIGH);
  while((millis()-lastTime)<11250);
  lastTime = millis();
}

void loop() {
  if(i<3){
    if(analogRead(A3)>600){
      if(((millis()-lastTime)>0)&&((millis()-lastTime)<5000)){
        digitalWrite(A0,HIGH);
        digitalWrite(A1,LOW);
        digitalWrite(A2,HIGH);
      }
      if(((millis()-lastTime)>5000)&&((millis()-lastTime)<10000)){
        digitalWrite(A1,HIGH);
        digitalWrite(A0,HIGH);
        digitalWrite(A2,HIGH);
      }
      if((millis()-lastTime)>10000){
        if(analogRead(A3)>600){
          digitalWrite(A0,HIGH);
          digitalWrite(A1,LOW);
          digitalWrite(A2,LOW);
          digitalWrite(A4,HIGH);
          if((millis()-lastTime)>11000){
            digitalWrite(A4,LOW);
            while(((millis()-lastTime)>11000)&&((millis()-lastTime)<12000));
            if((millis()-lastTime)>12000){
              lastTime = millis();
              i++;
            }
          }
        }
      }
    }

      if(analogRead(A3)<600){
        lastTime = millis();
        digitalWrite(A0,HIGH);
        digitalWrite(A1,HIGH);
        digitalWrite(A2,LOW);
        digitalWrite(A4,LOW);
     }
  }
  if(i>=3){
    digitalWrite(A0,LOW);
    digitalWrite(A1,LOW);
    digitalWrite(A2,LOW);
    digitalWrite(A4,LOW);
  }
}

