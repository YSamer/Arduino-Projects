#include <DHT.h>
#include <EEPROM.h>

#define data 9
#define sclk 8
#define rclk 7
#define d0   10
#define d1   11
#define d2   12
#define InAl    A3
#define ExAl    3
#define R1   A4   //Pestem
#define R2   A5   //Fan
#define R3   2    //Spark
#define BT   A0
#define cell   A1
#define pt   A2

//sensor
#define DHTPIN 13
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);
float hum=0.0;
float temp=0.0;
float tempF=0.0;
int layer = 0;
unsigned long lTime=0;
unsigned long lHTime=0;
unsigned long sparkTime=0;
byte lHTState=0;
byte lHFState=0;
unsigned long lBTime=0;
unsigned long lBBTime=0;
unsigned long lCTime=0;
unsigned long noFireT=0;
int state1=0,state2=0,state3=1;
int lButton = 0;
int Try = 1;
int noFireState = 0;

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
float oTemp = 0.0;//E0
float oTempF = 0.0;//E4
int pass  = 0;//E200
float difUT = 0.0;//E2
float difDT = 0.0;//E9
int TState = 0;  //E201
int FState = 1;  //E202
int PtState = 1; //E206
float offf = 0.0;//E5
float onf = 0.0;//E6
float offp = 0.0;//E7
int alrt = 0;  //E205
float hAlT = 0.0; //E11
float lAlT = 0.0; //E13

void setup() {
  dht.begin();
  
  pinMode(data, OUTPUT);
  pinMode(rclk, OUTPUT);
  pinMode(sclk, OUTPUT);
  pinMode(d0, OUTPUT);
  pinMode(d1, OUTPUT);
  pinMode(d2, OUTPUT);  
  
  pinMode(R1, OUTPUT);
  pinMode(R2, OUTPUT);
  pinMode(R3, OUTPUT);
  pinMode(InAl, OUTPUT);
  pinMode(ExAl, OUTPUT);
  pinMode(BT, INPUT);
  pinMode(cell, INPUT);
  pinMode(pt, INPUT);
  
  digitalWrite(d0,HIGH);
  digitalWrite(d1,HIGH);
  digitalWrite(d2,HIGH);
  digitalWrite(R1,LOW);
  digitalWrite(R2,LOW);
  digitalWrite(R3,LOW);
  digitalWrite(InAl,LOW);
  digitalWrite(ExAl,LOW);
  
  

//  EEPROMFloatWrite(0,25.0);
//  EEPROMFloatWrite(2,0);
//  EEPROMFloatWrite(9,0);
//  EEPROMFloatWrite(4,75.0);
//  EEPROMIntWrite(201,0);
//  EEPROMIntWrite(200,8);
//  EEPROMIntWrite(202,0);
//  EEPROMFloatWrite(5,40.0);
//  EEPROMFloatWrite(6,50.0);
//  EEPROMFloatWrite(7,75.0);
//  EEPROMIntWrite(205,0);
//  EEPROMIntWrite(206,1);
//  EEPROMFloatWrite(11,50.0);
//  EEPROMFloatWrite(13,25.0);


  oTemp = EEPROMFloatRead(0);
  oTempF = EEPROMFloatRead(4);
  pass  = EEPROMIntRead(200);
  difUT = EEPROMFloatRead(2);
  difDT = EEPROMFloatRead(9);
  TState = EEPROMIntRead(201);
  FState = EEPROMIntRead(202);
  offf = EEPROMFloatRead(5);
  onf = EEPROMFloatRead(6);
  offp = EEPROMFloatRead(7);
  alrt = EEPROMIntRead(205);
  hAlT = EEPROMFloatRead(11);
  lAlT = EEPROMFloatRead(13);
  PtState = EEPROMIntRead(206);

  
  lTime = millis();
  lCTime = millis();
  
}

void loop() {
  hum   = dht.readHumidity();
  Display();
  temp  = dht.readTemperature();
  Display();
  tempF = dht.readTemperature(true);
  Display();
  
  
  
if(layer == 0){
 {
    if (isnan(hum) || isnan(temp) || isnan(tempF)) {
      if(isPT()){
        setYt(getTemp());
      }else{
        setY(X[21],X[21],X[21]);
      }
    }else{
      if(millis()-lTime<=5000){
        if(TState == 0){
          setYt(temp);
        }
        else if(TState == 1){
          setYt(tempF);
        }
        Display();
      }
      else if((millis()-lTime>5000)&&(millis()-lTime<=10000)){
        if(PtState == 0){
          setYh(hum);
        }else{
          setYf(ptTRead());
        }
        
        Display();
      }
      else {
        lTime = millis();
      }
      Display();
      switch(checkButton()){
        case 1: break;
        case 2: break;
        case 3: break;
        case 4: layer = 1; state1 = 0; break;
        case 5: layer = 2; state2 = 0; break;
      }
    }


   //Out  Config

  if(getTemp()<=(getOTemp()-difDT)&&getTemp()<=(getOTemp())&&Try<=3){
    if(lHTState==0){
      lHTime = millis();
      lHTState = 1;
      noFireState = 0;
    }
    Display();

    if(millis()-lHTime<1000){
      digitalWrite(R1,LOW);
      digitalWrite(R2,LOW);
      digitalWrite(R3,HIGH);
    }else if((millis()-lHTime>=1000)&&(millis()-lHTime<4000)){
      digitalWrite(R1,LOW);
      digitalWrite(R2,HIGH);
      digitalWrite(R3,HIGH);
    }else if((millis()-lHTime>=4000)&&(millis()-lHTime<6000)){
      digitalWrite(R1,HIGH);
      digitalWrite(R2,HIGH);
      digitalWrite(R3,HIGH);
    }else if((millis()-lHTime>=6000)){
      digitalWrite(R1,HIGH);
      digitalWrite(R2,HIGH);
      digitalWrite(R3,LOW);

      if((millis()-lHTime>=9000) && analogRead(cell)>980){
        if(millis()-noFireT>=1500){
          digitalWrite(R1,LOW);
          digitalWrite(R2,LOW);
          digitalWrite(R3,LOW);
          noFireState = 1;
        }
      }else{
        noFireT = millis();
        noFireState = 0;
      }
      Display();
      if((millis()-lHTime>=(9000+(5000*Try))) && analogRead(cell)>980 && noFireState==1){
        lHTState=0;
        Try++;
      }if(analogRead(cell)<980){
        Try=1;
        noFireState = 0;
      }
    }
     if(millis()-sparkTime>=5000){
        digitalWrite(R3,HIGH);
     }
     if((millis()-sparkTime>=6000)&&(millis()-lHTime>=9000)){
        digitalWrite(R3,LOW);
        sparkTime = millis();
     }
    
    Display();
      
  }else if(getTemp()>=(getOTemp()) && getTemp()<=(getOTemp()+difUT) && lHTState==1){
    if((millis()-lHTime>=6000)){
      digitalWrite(R1,LOW);
      digitalWrite(R2,HIGH);
      digitalWrite(R3,LOW);
    }
  }else if(getTemp()>=(getOTemp()+difUT)){
    digitalWrite(R1,LOW);
    digitalWrite(R2,LOW);
    digitalWrite(R3,LOW);
    lHTState=0;
    Try=1;
  }
  Display();
    
    //Alart
    if(getTemp()>=hAlT){
      onAlert();
    }else if(getTemp()<=lAlT){
      onAlert();
    }else if(getTemp()<=(getOTemp()-difDT)&&Try>3){
      onAlert();
    }else offAlert();
   
  }
  
    Display();
    switch(checkButton()){
      case 1: break;
      case 2: break;
      case 3: break;
      case 4: layer = 1; state1 = 0; break;
      case 5: layer = 2; state2 = 0; break;
    }

    
  }

  layer1:
  //////////Layer 1>>>SR ORDER H AND T VaLUE
  if(layer == 1){
    if(state1==0){
      setY(0,0,X[18]);
      Display();
      switch(checkButton()){
        case 1: break;
        case 2: break;
        case 3: layer = 0; break;
        case 4: state1 = 2; break;
      }
    }
    
    if(state1==2){
      if(TState == 0){
        setYf(oTemp);
        Display();
        switch(checkButton()){
          case 1: 
            if(oTemp>0&&oTemp<=100){oTemp-=0.1;}  
            else if(oTemp>100&&oTemp<=151){oTemp--;} 
            break;
          case 2: 
            if(oTemp>=-1&&oTemp<100){oTemp+=0.1;} 
            else if(oTemp>=100&&oTemp<150){oTemp++;} 
            break;
          case 3: state1 = 0; EEPROMFloatWrite(0,oTemp); break;
          case 4: state1 = 0; EEPROMFloatWrite(0,oTemp); break;
        }
      }
      if(TState == 1){
        setYf(oTempF);
        Display();
        switch(checkButton()){
          case 1: 
            if(oTempF>0&&oTempF<=100){oTempF-=0.1;}
            else if(oTempF>100&&oTempF<=251){oTemp--;}
            break;
          case 2: 
            if(oTempF>=-1&&oTempF<100){oTempF+=0.1;}
            else if(oTempF>=100&&oTempF<250){oTempF++;}
            break;
          case 3: state1 = 0; EEPROMFloatWrite(4,oTempF); break;
          case 4: state1 = 0; EEPROMFloatWrite(4,oTempF); break;
        }
      }
      
    }
    
  }

  layer2:
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



  //////////Layer 3//Set Configration layer
  if(layer == 3){
    //dif
    if(state3 == 1){
      setY(X[15],X[1],X[13]);
      Display();
      switch(checkButton()){
        case 1: state3 = 2; break;
        case 2: state3 = 7;break;
        case 3: layer = 0; break;
        case 4: state3 = 11; break;
      }
    }
    {
      if(state3==11){//dif Temp
        setY(0,0,X[18]);
        Display();
        switch(checkButton()){
          case 1: break;
          case 2: break;
          case 3: state3 = 1; break;
          case 4: state3 = 111; break;
        }
      }

      {
        if(state3==111){//dif Temp up
          setY(0,X[19],X[23]);
          Display();
          switch(checkButton()){
            case 1: state3 = 112; break;
            case 2: state3 = 112; break;
            case 3: state3 = 11; break;
            case 4: state3 = 1111; break;
          }
        }
        if(state3==1111){
          setYf(difUT);
          Display();
          switch(checkButton()){
            case 1: if(difUT>0&&difUT<=5.1){difUT-=0.1;} break;
            case 2: if(difUT>=-1&&difUT<5.0){difUT+=0.1;} break;
            case 3: state3 = 111; EEPROMFloatWrite(2,difUT); break;
            case 4: state3 = 111; EEPROMFloatWrite(2,difUT); break;
          }
        }
        if(state3==112){//dif Temp down
          setY(0,X[20],X[13]);
          Display();
          switch(checkButton()){
            case 1: state3 = 111; break;
            case 2: state3 = 111; break;
            case 3: state3 = 11; break;
            case 4: state3 = 1121; break;
          }
        }
        if(state3==1121){
          setYf(difDT);
          Display();
          switch(checkButton()){
            case 1: if(difDT>0&&difDT<=5.1){difDT-=0.1;} break;
            case 2: if(difDT>=-1&&difDT<5.0){difDT+=0.1;} break;
            case 3: state3 = 112; EEPROMFloatWrite(9,difDT); break;
            case 4: state3 = 112; EEPROMFloatWrite(9,difDT); break;
          }
        }
      }
    }
    
    //FC
    if(state3 == 2){
      setY(0,X[12],X[15]);
      Display();
      switch(checkButton()){
        case 1: state3 = 3; break;
        case 2: state3 = 1; break;
        case 3: layer = 0; break;
        case 4: if(TState==0){state3=21;}else if(TState==1){state3=22;}break;
      }
    }
    {
      if(state3==21){
        setY(0,0,X[12]);
        Display();
        switch(checkButton()){
          case 1: state3 = 22; break;
          case 2: state3 = 22; break;
          case 3: state3 = 2; TState=0; EEPROMIntWrite(201,TState); break;
          case 4: state3 = 2; TState=0; EEPROMIntWrite(201,TState); break;
        }
      }
      if(state3==22){
        setY(0,0,X[15]);
        Display();
        switch(checkButton()){
          case 1: state3 = 21; break;
          case 2: state3 = 21; break;
          case 3: state3 = 2; TState=1; EEPROMIntWrite(201,TState); break;
          case 4: state3 = 2; TState=1; EEPROMIntWrite(201,TState); break;
        }
      }
    }

    //Pt on off
    if(state3 == 3){
      setY(0,X[18],X[19]);
      Display();
      switch(checkButton()){
        case 1: state3 = 4; break;
        case 2: state3 = 2; break;
        case 3: layer = 0; break;
        case 4: if(PtState==0){state3=31;}else if(PtState==1){state3=32;}break;
      }
    }
    {
      if(state3==31){
        setY(X[15],X[15],X[0]);
        Display();
        switch(checkButton()){
          case 1: state3 = 32; break;
          case 2: state3 = 32; break;
          case 3: state3 = 3; PtState=0; EEPROMIntWrite(206,PtState); break;
          case 4: state3 = 3; PtState=0; EEPROMIntWrite(206,PtState); break;
        }
      }
      if(state3==32){
        setY(0,X[20],X[0]);
        Display();
        switch(checkButton()){
          case 1: state3 = 31; break;
          case 2: state3 = 31; break;
          case 3: state3 = 3; PtState=1; EEPROMIntWrite(206,PtState); break;
          case 4: state3 = 3; PtState=1; EEPROMIntWrite(206,PtState); break;
        }
      }
    }

    
    //PCH
    if(state3 == 4){
      setY(X[16],X[12],X[19]);
      Display();
      switch(checkButton()){
        case 1: state3 = 5; break;
        case 2: state3 = 3; break;
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
    //Fan
    if(state3 == 5){
      setY(X[20],X[10],X[15]);
      Display();
      switch(checkButton()){
        case 1: state3 = 6; break;
        case 2: state3 = 4; break;
        case 3: layer = 0; break;
        case 4: state3 = 51; break;
      }
    }
    { 
      {
        if(state3 == 51){//off fan
          setY(X[20],X[15],X[0]);
          Display();
          switch(checkButton()){
            case 1: state3 = 52; break;
            case 2: state3 = 53; break;
            case 3: state3 = 5; break;
            case 4: state3 = 511; break;
          }
        }
        {
          if(state3 == 511){
            setYf(offf);
            Display();
            switch(checkButton()){
              case 1: 
                if(offf>0&&offf<=100){offf-=0.1;}  
                else if(offf>100&&offf<=151){offf--;} 
                break;
              case 2: 
                if(offf>=-1&&offf<100){offf+=0.1;} 
                else if(offf>=100&&offf<150){offf++;} 
                break;
              case 3: state3 = 51; EEPROMFloatWrite(5,offf); break;
              case 4: state3 = 51; EEPROMFloatWrite(5,offf); break;
            }
          }
        }
        if(state3 == 52){// on fan
          setY(X[20],X[15],X[1]);
          Display();
          switch(checkButton()){
            case 1: state3 = 53; break;
            case 2: state3 = 51; break;
            case 3: state3 = 5; break;
            case 4: state3 = 521; break;
          }
        }
        {
          if(state3 == 521){
            setYf(onf);
            Display();
            switch(checkButton()){
              case 1: 
                if(onf>0&&onf<=100){onf-=0.1;}  
                else if(onf>100&&onf<=151){onf--;} 
                break;
              case 2: 
                if(onf>=-1&&onf<100){onf+=0.1;} 
                else if(onf>=100&&onf<150){onf++;} 
                break;
              case 3: state3 = 52; EEPROMFloatWrite(6,onf); break;
              case 4: state3 = 52; EEPROMFloatWrite(6,onf); break;
            }
          }
        }
        if(state3 == 53){// off pestem
          setY(X[5],X[19],X[0]);
          Display();
          switch(checkButton()){
            case 1: state3 = 51; break;
            case 2: state3 = 52; break;
            case 3: state3 = 5; break;
            case 4: state3 = 531; break;
          }
        }
        {
          if(state3 == 531){
            setYf(offp);
            Display();
            switch(checkButton()){
              case 1: 
                if(offp>0&&offp<=100){offp-=0.1;}  
                else if(offp>100&&offp<=151){offp--;} 
                break;
              case 2: 
                if(offp>=-1&&offp<100){offp+=0.1;} 
                else if(offp>=100&&offp<150){offp++;} 
                break;
              case 3: state3 = 53; EEPROMFloatWrite(7,offp); break;
              case 4: state3 = 53; EEPROMFloatWrite(7,offp); break;
            }
          }
        }
      }
    }
    
    //dt Dot Show
    if(state3 == 6){
      setY(0,X[18],X[13]);
      Display();
      switch(checkButton()){
        case 1: state3 = 7; break;
        case 2: state3 = 5; break;
        case 3: layer = 0; break;
        case 4: if(FState==0){state3=61;}else if(FState==1){state3=62;}break;
      }
    }
    {
      if(state3 == 61){
        setY(X[15],X[15],X[0]);
        Display();
        switch(checkButton()){
          case 1: state3 = 62; break;
          case 2: state3 = 62; break;
          case 3: state3 = 6; FState=0; EEPROMIntWrite(202,FState); break;
          case 4: state3 = 6; FState=0; EEPROMIntWrite(202,FState); break;
        }
      }
      if(state3 == 62){
        setY(0,X[20],X[0]);
        Display();
        switch(checkButton()){
          case 1: state3 = 61; break;
          case 2: state3 = 61; break;
          case 3: state3 = 6; FState=1; EEPROMIntWrite(202,FState); break;
          case 4: state3 = 6; FState=1; EEPROMIntWrite(202,FState); break;
        }
      }
    }

    //Alart
    if(state3 == 7){
      setY(0,X[21],X[10]);
      Display();
      switch(checkButton()){
        case 1: state3 = 1; break;
        case 2: state3 = 6; break;
        case 3: layer = 0; break;
        case 4: state3=71; break;
      }
    }
    {//In or Ex Alart
      if(state3 == 71){
        setY(0,X[12],X[10]);
        Display();
        switch(checkButton()){
          case 1: state3 = 72; break;
          case 2: state3 = 73; break;
          case 3: state3 = 7; break;
          case 4: if(alrt==0){state3=711;}else if(alrt==1){state3=712;}break;
        }
      }
      if(state3 == 711){
        setY(0,X[20],X[1]);
        Display();
        switch(checkButton()){
          case 1: state3 = 712; break;
          case 2: state3 = 712; break;
          case 3: state3 = 71; alrt=0; EEPROMIntWrite(205,alrt); break;
          case 4: state3 = 71; alrt=0; EEPROMIntWrite(205,alrt); break;
        }
      }
      if(state3 == 712){
        setY(0,X[18],X[14]);
        Display();
        switch(checkButton()){
          case 1: state3 = 711; break;
          case 2: state3 = 711; break;
          case 3: state3 = 7; alrt=1; EEPROMIntWrite(205,alrt); break;
          case 4: state3 = 7; alrt=1; EEPROMIntWrite(205,alrt); break;
        }
      }
    }
    {//High Alart
      if(state3 == 72){
        setY(X[21],X[10],X[16]);
        Display();
        switch(checkButton()){
          case 1: state3=73; break;
          case 2: state3=71; break;
          case 3: state3=7; break;
          case 4: state3=721; break;
        }
      }
      if(state3==721){
        setY(0,0,X[18]);
        Display();
        switch(checkButton()){
          case 1: break;
          case 2: break;
          case 3: state3=72; break;
          case 4: state3=7211; break;
        }
      }
        if(state3==7211){
          setYf(hAlT);
          Display();
          switch(checkButton()){
            case 1: if(hAlT>0&&hAlT<=100){hAlT-=0.1;}  else if(hAlT>100&&hAlT<=201){hAlT--;} break;
            case 2: if(hAlT>=-1&&hAlT<100){hAlT+=0.1;} else if(hAlT>=100&&hAlT<200){hAlT++;} break;
            case 3: state3=721; EEPROMFloatWrite(11,hAlT); break;
            case 4: state3=721; EEPROMFloatWrite(11,hAlT); break;
          }
        }
      
    }
    {//LOW Alart
      if(state3 == 73){
        setY(X[21],X[10],X[21]);
        Display();
        switch(checkButton()){
          case 1: state3 =71; break;
          case 2: state3 =72; break;
          case 3: state3=7; break;
          case 4: state3=731; break;
        }
      }
      if(state3==731){
        setY(0,0,X[18]);
        Display();
        switch(checkButton()){
          case 1: break;
          case 2: break;
          case 3: state3=73; break;
          case 4: state3 = 7311; break;
        }
      }
        if(state3==7311){
          setYf(lAlT);
          Display();
          switch(checkButton()){
            case 1: if(lAlT>0&&lAlT<=100){lAlT-=0.1;}  else if(lAlT>100&&lAlT<=201){lAlT--;} break;
            case 2: if(lAlT>=-1&&lAlT<100){lAlT+=0.1;} else if(lAlT>=100&&lAlT<200){lAlT++;} break;
            case 3: state3=731; EEPROMFloatWrite(13,lAlT); break;
            case 4: state3=731; EEPROMFloatWrite(13,lAlT); break;
          }
        }
    }
    
  }
}

void setYt(float F){
  if(FState == 0){
    int s=12;
    if(TState == 0){
      s=12;
    }
    if(TState == 1){
      s=15;
    }
    if(F>=0&&F<10){
      int L = (F*10);
      int n1 = (L/100); 
      int n2 = (L-(n1*100))/10;
      int n3 = (L-(n1*100)-(n2*10));
      setY(X[s],X[n2],0);
    }
    else if(F>=10&&F<100){
      int L = (F*10);
      int n1 = (L/100); 
      int n2 = (L-(n1*100))/10;
      int n3 = (L-(n1*100)-(n2*10));
      setY(X[s],X[n2],X[n1]);
    }
    else if(F>=100&&F<1000){
      int L = (F*10);
      int n1 = (L/1000); 
      int n2 = (L-(n1*1000))/100;
      int n3 = ((L-(n1*1000)-(n2*100)))/10;
      setY(X[n3],X[n2],X[n1]);
    }
  }
  if(FState == 1){
    if(F>=0&&F<10){
      int L = (F*10);
      int n1 = (L/100); 
      int n2 = (L-(n1*100))/10;
      int n3 = (L-(n1*100)-(n2*10));
      setY(X[n3]|0b00010000,X[n2]|0b00010000,0);
    }
    else if(F>=10&&F<100){
      int L = (F*10);
      int n1 = (L/100); 
      int n2 = (L-(n1*100))/10;
      int n3 = (L-(n1*100)-(n2*10));
      setY(X[n3]|0b00010000,X[n2]|0b00010000,X[n1]);
    }
    else if(F>=100&&F<1000){
      int L = (F*10);
      int n1 = (L/1000); 
      int n2 = (L-(n1*1000))/100;
      int n3 = ((L-(n1*1000)-(n2*100)))/10;
      setY(X[n3]|0b00010000,X[n2],X[n1]);
    }
  }
  
}

void setYh(float F){
  if(FState == 0){
    if(F>=0&&F<10){
      int L = (F*10);
      int n1 = (L/100); 
      int n2 = (L-(n1*100))/10;
      int n3 = (L-(n1*100)-(n2*10));
      setY(X[16],X[n2],0);
    }
    else if(F>=10&&F<100){
      int L = (F*10);
      int n1 = (L/100); 
      int n2 = (L-(n1*100))/10;
      int n3 = (L-(n1*100)-(n2*10));
      setY(X[16],X[n2],X[n1]);
    }
    else if(F>=100&&F<1000){
      int L = (F*10);
      int n1 = (L/1000); 
      int n2 = (L-(n1*1000))/100;
      int n3 = ((L-(n1*1000)-(n2*100)))/10;
      setY(X[n3],X[n2],X[n1]);
    }
  }
  if(FState == 1){
    if(F>=0&&F<10){
      int L = (F*10);
      int n1 = (L/100); 
      int n2 = (L-(n1*100))/10;
      int n3 = (L-(n1*100)-(n2*10));
      setY(X[n3],X[n2]|0b00010000,0);
    }
    else if(F>=10&&F<100){
      int L = (F*10);
      int n1 = (L/100); 
      int n2 = (L-(n1*100))/10;
      int n3 = (L-(n1*100)-(n2*10));
      setY(X[n3],X[n2]|0b00010000,X[n1]);
    }
    else if(F>=100&&F<1000){
      int L = (F*10);
      int n1 = (L/1000); 
      int n2 = (L-(n1*1000))/100;
      int n3 = ((L-(n1*1000)-(n2*100)))/10;
      setY(X[n3],X[n2],X[n1]);
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


float getTemp(){
  if (isnan(hum) || isnan(temp) || isnan(tempF)) {
      if(isPT()){
        if(TState == 0){
          return ptTRead();
        }else if(TState == 1){
          return (ptTRead()*1.8+32.00);
        }
      }
  }else{
    if(TState == 0){
      return temp;
    }else if(TState == 1){
      return tempF;
    }
  }
}

float getOTemp(){
  if(TState == 0){
    return oTemp;
  }
  if(TState == 1){
    return oTempF;
  }
}

void offAlert(){
  if(alrt==0){
    digitalWrite(InAl,LOW);
    digitalWrite(ExAl,LOW);
  }else if(alrt==1){
    digitalWrite(InAl,LOW);
    digitalWrite(ExAl,HIGH);
  }
}
void onAlert(){
  if(alrt==0){
    digitalWrite(InAl,HIGH);
    digitalWrite(ExAl,LOW);
  }else if(alrt==1){
    digitalWrite(InAl,LOW);
    digitalWrite(ExAl,LOW);
  }
}

bool isPT(){
  if(analogRead(pt)>1020){
    return 0;
  }else if(analogRead(pt)<1020){
    return 1;
  }
}

float ptTRead(){
  long A=0;
  for(int i=0;i<100;i++){
    A+=analogRead(pt);
    Display();
  }
  Display();
  A/=100;
  float T = (A-747.7)/-9.27;
  return T;
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

int checkButton(){
  
  if((analogRead(BT)>150&&analogRead(BT)<190)&&(millis()-lBTime>100)){
    if(millis()-lBBTime>300){
      lBTime = millis();
      lButton = 1;
      return 1;//down
    }else{return 0;}
    
  }
  else if((analogRead(BT)>330&&analogRead(BT)<370)&&(millis()-lBTime>100)){
    if(millis()-lBBTime>300){
      lBTime = millis();
      lButton = 2;
      return 2;//up
    }else{return 0;}
  }
  else if((analogRead(BT)>500&&analogRead(BT)<540)&&(lButton != 3)){
    if(millis()-lBBTime>300){
      lBTime = millis();
      lButton = 3;
      return 3;//return
    }else{return 0;}
  }
  else if((analogRead(BT)>660&&analogRead(BT)<730)&&(lButton != 4)){
    if(millis()-lBBTime>300){
      lBTime = millis();
      lButton = 4;
      return 4;//set
    }else{return 0;}
  }else if((analogRead(BT)>450&&analogRead(BT)<500)){
    lButton = 5;
    if(millis()-lBTime>=3000){return 5;}//set
    else{return 0;}
  }
  else if((analogRead(BT)<6)){
    lBTime = millis();
    lBBTime = millis();
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
