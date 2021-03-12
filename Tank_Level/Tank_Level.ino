#include <EEPROM.h>

#define data 9
#define sclk 8
#define rclk 7
#define d0   10
#define d1   11
#define d2   12
#define Echo A1
#define Trig A2
#define R1   A3
#define BT   A0

int layer = 0;
unsigned long lTime=0;
unsigned long lBTime=0;
int state1=0,state2=0,state3=1;
int lButton = 0;

const byte X[] =
{//AEDHCGFB
  0b11101011,  // 0
  0b00001001,  // 1
  0b11100101,  // 2
  0b10101101,  // 3
  0b00001111,  // 4
  0b10101110,  // 5
  0b11101110,  // 6
  0b10001001,  // 7
  0b11101111,  // 8
  0b10101111,  // 9
  0b11001111,  // A >>10
  0b01101110,  // b >>11
  0b11100010,  // C >>12
  0b01101101,  // d >>13
  0b11100110,  // E >>14
  0b11000110,  // F >>15
  0b01001111,  // H >>16
  0b00001000,  // i >>17
  0b01100110,  // t >>18
  0b11000111,  // P >>19
  0b01001100,  //n  >>20
  0b01100010,  //L  >>21
  0b00101111,  //y  >>22
  0b01101000  //u  >>23
};

byte Y[3]{
  0b00000000,
  0b00000000,
  0b00000000
};

int opass  = 0;
//>>>important eeprom value
int height = 100;//E201
float oLevel  = 50.0;//E1
int pass  = 0;//E200
float tryT = 5.0;//E2


void setup() {
  delay(1000);
  
  pinMode(data, OUTPUT);
  pinMode(rclk, OUTPUT);
  pinMode(sclk, OUTPUT);
  pinMode(d0, OUTPUT);
  pinMode(d1, OUTPUT);
  pinMode(d2, OUTPUT);  
  pinMode(R1, OUTPUT);
  pinMode(BT, INPUT);
  pinMode(Echo, INPUT);
  pinMode(Trig, OUTPUT);
  
  digitalWrite(d0,HIGH);
  digitalWrite(d1,HIGH);
  digitalWrite(d2,HIGH);
  digitalWrite(R1,LOW);
  digitalWrite(Trig,LOW);
  
  

//  EEPROMFloatWrite(1,50.0);
//  EEPROMFloatWrite(2,5.0);
//  EEPROMIntWrite(200,0);
//  EEPROMIntWrite(201,100);


  oLevel  = EEPROMFloatRead(1);
  tryT = EEPROMFloatRead(2);
  pass  = EEPROMIntRead(200);
  height  = EEPROMIntRead(201);
  
  lTime = millis();
  lBTime = millis();
}

void loop() {


  if(getLevel()<50.0){
    digitalWrite(R1,HIGH);
  }
  if(getLevel()>=99.5){
    digitalWrite(R1,LOW);
  }
  
  //////////Layer 0
  if(layer == 0){
    if(millis()-lTime>1000){
      if(getLevel() == 404.0){
        setY(0,X[14],X[5]);
      }
      else{
        if(getLevel()>=100.0){
          setYf(100.0);
        }else{
          setYf(getLevel()); 
        }
      }
      lTime = millis();
    }
    
    Display();

    switch(checkButton()){
      case 1: break;
      case 2: break;
      case 3: break;
      case 4: layer = 1; break;
    }
    
    //Set Configration
    lBTime = millis();
    while((analogRead(BT)>450&&analogRead(BT)<500)){
      Display();
      if(millis()-lBTime>3000){
        layer = 2;
      }
    }
  }
  
  //////////Layer 1>>>Order Level
  if(layer == 1){ 
    setYf(oLevel);
    Display();
    switch(checkButton()){
      case 1: 
        if(oLevel>0&&oLevel<=100){oLevel-=0.1;}  
        else if(oLevel>100&&oLevel<=101){oLevel--;} 
        break;
      case 2: 
        if(oLevel>=-1&&oLevel<100){oLevel+=0.1;} 
        else if(oLevel>=100&&oLevel<100){oLevel++;} 
        break;
      case 3: layer = 0; EEPROMFloatWrite(1,oLevel); break;
      case 4: layer = 0; EEPROMFloatWrite(1,oLevel); break;
    }
  }

  //////////Layer 2//Set Configration layer
  if(layer == 2){
    if(state2 == 0){
      setY(X[5],X[10],X[19]);
      Display();
      switch(checkButton()){
        case 1: break;
        case 2: break;
        case 3: layer = 0; break;
        case 4: state2 = 1; break;
      }
    }
    
    if(state2 == 1){
      setYi(opass);
      Display();
      switch(checkButton()){
        case 1: if(opass>0&&opass<=255){opass--;} break;
        case 2: if(opass>=0&&opass<254){opass++;} break;
        case 3: state2 = 0; break;
        case 4: if(opass == pass){opass = 0;layer = 3; state3 = 1; state2 = 0;} break;
      }
    }
  }

  if(layer == 3){
    //Height
    if(state3 == 1){
      setY(0,0,X[16]);
      Display();
      switch(checkButton()){
        case 1: state3 = 2; break;
        case 2: state3 = 4;break;
        case 3: layer = 0; break;
        case 4: state3 = 11; break;
      }
    }
    {
      if(state3==11){
        setYi(height);
        Display();
        switch(checkButton()){
          case 1: if(height>10&&height<=490.1){height-=1;} break;
          case 2: if(height>=9&&height<490.0){height+=1;} break;
          case 3: state3 = 1; EEPROMIntWrite(201,height); break;
          case 4: state3 = 1; EEPROMIntWrite(201,height); break;
        }
      }
    }
    //Try Timer
    if(state3 == 2){
      setY(0,0,X[18]);
      Display();
      switch(checkButton()){
        case 1: state3 = 4; break;
        case 2: state3 = 1;break;
        case 3: layer = 0; break;
        case 4: state3 = 21; break;
      }
    }
    {
      if(state3==21){
        setYf(tryT);
        Display();
        switch(checkButton()){
          case 1: if(tryT>0&&tryT<=60.1){tryT-=0.1;} break;
          case 2: if(tryT>=-1&&tryT<60.0){tryT+=0.1;} break;
          case 3: state3 = 2; EEPROMFloatWrite(2,tryT); break;
          case 4: state3 = 2; EEPROMFloatWrite(2,tryT); break;
        }
      }
    }
    //PCH
    if(state3 == 4){
      setY(X[16],X[12],X[19]);
      Display();
      switch(checkButton()){
        case 1: state3 = 1; break;
        case 2: state3 = 2; break;
        case 3: layer = 0; break;
        case 4: state3 = 41; break;
      }
    }
    {
      if(state3==41){
        setYi(pass);
        Display();
        switch(checkButton()){
          case 1: if(pass>0&&pass<=999){pass--;} break;
          case 2: if(pass>=-1&&pass<998){pass++;} break;
          case 3: state3 = 4; EEPROMIntWrite(200,pass); break;
          case 4: state3 = 4; EEPROMIntWrite(200,pass); break;
        }
      }
    }
  }
}




void setYi(int L){
  if(L>=100&&L<255){
    int n1 = (L/100); 
    int n2 = (L-(n1*100))/10;
    int n3 = (L-(n1*100)-(n2*10));
    setY(X[n3],X[n2],X[n1]);
  }
  if(L>=10&&L<100){
    int n1 = (L/100); 
    int n2 = (L-(n1*100))/10;
    int n3 = (L-(n1*100)-(n2*10));
    setY(X[n3],X[n2],0);
  }
  if(L>=0&&L<10){
    int n1 = (L/100); 
    int n2 = (L-(n1*100))/10;
    int n3 = (L-(n1*100)-(n2*10));
    setY(X[n3],0,0);
  }
}

void setYf(float F){
  int L = (int)(F*10);
  if(L>=1000&&L<10000){
    int n1 = (L/1000); 
    int n2 = (L-(n1*1000))/100;
    int n3 = (L-(n1*1000)-(n2*100))/10;
    setY(X[n3],X[n2],X[n1]);
  }
  if(L>=100&&L<1000){
    int n1 = (L/100); 
    int n2 = (L-(n1*100))/10;
    int n3 = (L-(n1*100)-(n2*10));
    setY(X[n3],X[n2]|0b00010000,X[n1]);
  }
  if(L>=10&&L<100){
    int n1 = (L/100); 
    int n2 = (L-(n1*100))/10;
    int n3 = (L-(n1*100)-(n2*10));
    setY(X[n3],X[n2]|0b00010000,0);
  }
  if(L>=0&&L<10){
    int n1 = (L/100); 
    int n2 = (L-(n1*100))/10;
    int n3 = (L-(n1*100)-(n2*10));
    setY(X[n3],X[0]|0b00010000,0);
  }
}
void setY(byte L0,byte L1, byte L2){
  Y[0] = L0;
  Y[1] = L1;
  Y[2] = L2;
}

void Display(){
  
  digitalWrite(rclk, LOW);
  shiftOut(data, sclk, LSBFIRST, Y[0]);
  digitalWrite(rclk, HIGH);
  digitalWrite(d0,LOW);
  delayMicroseconds(1000);
  digitalWrite(d0,HIGH);

  digitalWrite(rclk, LOW);
  shiftOut(data, sclk, LSBFIRST, Y[1]);
  digitalWrite(rclk, HIGH);
  digitalWrite(d1,LOW);
  delayMicroseconds(1000);
  digitalWrite(d1,HIGH);

  digitalWrite(rclk, LOW);
  shiftOut(data, sclk, LSBFIRST, Y[2]);
  digitalWrite(rclk, HIGH);
  digitalWrite(d2,LOW);
  delayMicroseconds(1000);
  digitalWrite(d2,HIGH);
}

float readUS(){
  Display();
  digitalWrite(Trig, LOW);
  delayMicroseconds(2);
  digitalWrite(Trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(Trig, LOW);
  int val = pulseIn(Echo, HIGH);
  Display();
  return ((val*0.034/2)-10);
}

float getLevel(){
  if(readUS()>=0){
    Display();
    return ((height-readUS())/(height)*100);
  }else{
    return 404.0;
  }
  
}

int checkButton(){
  
  if((analogRead(BT)>160&&analogRead(BT)<200)&&(lButton != 1)){
    lButton = 1;
    return 1;//down
  }
  else if((analogRead(BT)>325&&analogRead(BT)<365)&&(lButton != 2)){
    lButton = 2;
    return 2;//up
  }
  else if((analogRead(BT)>500&&analogRead(BT)<540)&&(lButton != 3)){
    lButton = 3;
    return 3;//return
  }
  else if((analogRead(BT)>700&&analogRead(BT)<755)&&(lButton != 4)){
    lButton = 4;
    return 4;//set
  }else if((analogRead(BT)<6)){
    lButton = 0;
    return 0;
  }
  
}

void EEPROMFloatWrite(int adr,float val){
  int adr1 = (adr*4);
  EEPROM.put(adr1,val);
}

float EEPROMFloatRead(int adr){
  int adr1 = (adr*4);
  float x;
  EEPROM.get(adr1,x);
  return x;
}

void EEPROMIntWrite(int adr,int val){
  int adr1 = (adr*2);
  EEPROM.put(adr1,val);
}
int EEPROMIntRead(int adr){
  int adr1 = (adr*2);
  int x;
  EEPROM.get(adr1,x);
  return x;
}
