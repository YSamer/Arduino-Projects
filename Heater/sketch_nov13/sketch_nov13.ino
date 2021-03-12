
unsigned long lastTime = 0;
int i = 0;

void setup() {
  pinMode(A0,OUTPUT);
  pinMode(A1,OUTPUT);
  pinMode(A2,OUTPUT);
  pinMode(A3,INPUT);

  //digitalWrite(A0,HIGH);
  digitalWrite(A0,LOW);
  digitalWrite(A1,LOW);
  digitalWrite(A2,LOW);
}

void loop() {
  
  if(analogRead(A3)>600){
    if(((millis()-lastTime)>0)&&((millis()-lastTime)<20000)){
      digitalWrite(A0,HIGH);
      digitalWrite(A1,LOW);
      digitalWrite(A2,LOW);
    }
    if(((millis()-lastTime)>20000)&&((millis()-lastTime)<25000)){
      digitalWrite(A0,HIGH);
      digitalWrite(A1,LOW);
      digitalWrite(A2,HIGH);
    }
    if(((millis()-lastTime)>25000)&&((millis()-lastTime)<30000)){
      digitalWrite(A1,HIGH);
      digitalWrite(A0,HIGH);
      digitalWrite(A2,HIGH);
    }
    if((millis()-lastTime)>30000){
      if(analogRead(A3)>600){
        digitalWrite(A0,LOW);
        digitalWrite(A1,LOW);
        digitalWrite(A2,LOW);
      }
    }
    
  }
  
  if(analogRead(A3)<600){
    lastTime = millis();
    digitalWrite(A0,HIGH);
    digitalWrite(A1,HIGH);
    digitalWrite(A2,LOW);
  }
}


/*
if(isFire()){
    setOutTrue();
  }else if(!isFire()){
    i++;
    setOutFalse();
    if(i<=3){
      Delay();
    }
  }
*/
bool isFire(){
  if(analogRead(A3)>600){
    //dark
    return 0;
  }
  else if(analogRead(A3)<600){
    //light
    return 1;
  }
}

void setOutTrue(){
  //light
  digitalWrite(A0,LOW);
  digitalWrite(A1,LOW);
  digitalWrite(A2,HIGH);
}
void setOutFalse(){
  //dark
  //digitalWrite(A0,HIGH);
  digitalWrite(A0,LOW);
  digitalWrite(A1,HIGH);
  digitalWrite(A2,HIGH);
}

void Delay(){
  lastTime = millis();
  while(((millis()-lastTime)<60000)&((millis()-lastTime)>0)){
    digitalWrite(A0,LOW);
  }
  while(((millis()-lastTime)>60000)&((millis()-lastTime)<75000)){
    digitalWrite(A2,LOW);
  }
  while(((millis()-lastTime)>75000)&((millis()-lastTime)<90000)){
    digitalWrite(A1,LOW);
  }
  digitalWrite(A2,HIGH);
}

