#include <DHT.h>
#include <EEPROM.h>

#define data 9
#define sclk 8
#define rclk 7
#define d0   10
#define d1   11
#define d2   12
#define R1   A3
#define BT   A0
#define pt   A1
#define InAl    A2
#define ExAl    A5
#define R1   A3
#define R2   A4

//sensor
#define DHTPIN 13
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);
float hum=0.0;
float temp=0.0;
float tempF=0.0;
int layer = 0;
unsigned long lTime=0;
byte lHTState=0;
byte lHFState=0;
unsigned long lBTime=0;
unsigned long lBBTime=0;
unsigned long lCTime=0;
int state1=0,state2=0,state3=1;
int lButton = 0;
int gW = 0; //global warmming

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
int HCState = 0; //E203
float upt = 0.0;//E5
float dt = 0.0;//E6
float ont = 0.0;//E7
float offt = 0.0;//E8
int alrt = 0;  //E205
float hAlT = 0.0; //E11
float hAlH = 0.0; //E12
float lAlT = 0.0; //E13
float lAlH = 0.0; //E14

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
  pinMode(InAl, OUTPUT);
  pinMode(ExAl, OUTPUT);
  pinMode(BT, INPUT);
  pinMode(pt, INPUT);
  
  digitalWrite(d0,HIGH);
  digitalWrite(d1,HIGH);
  digitalWrite(d2,HIGH);
  digitalWrite(R1,LOW);
  digitalWrite(R2,LOW);
  digitalWrite(InAl,LOW);
  digitalWrite(ExAl,LOW);
  

//  EEPROMFloatWrite(0,25.0);
//  EEPROMFloatWrite(2,0);
//  EEPROMFloatWrite(9,0);
//  EEPROMFloatWrite(4,75.0);
//  EEPROMIntWrite(201,0);
//  EEPROMIntWrite(200,8);
//  EEPROMIntWrite(202,0);
//  EEPROMIntWrite(203,1);
//  EEPROMFloatWrite(5,0);
//  EEPROMFloatWrite(6,0);
//  EEPROMFloatWrite(7,0);
//  EEPROMFloatWrite(8,0);
//  EEPROMIntWrite(205,0);
//  EEPROMFloatWrite(11,2.0);
//  EEPROMFloatWrite(12,5.0);
//  EEPROMFloatWrite(13,2.0);
//  EEPROMFloatWrite(14,5.0);

  oTemp = EEPROMFloatRead(0);
  oTempF = EEPROMFloatRead(4);
  pass  = EEPROMIntRead(200);
  difUT = EEPROMFloatRead(2);
  difDT = EEPROMFloatRead(9);
  TState = EEPROMIntRead(201);
  FState = EEPROMIntRead(202);
  HCState = EEPROMIntRead(203);
  upt = EEPROMFloatRead(5);
  dt = EEPROMFloatRead(6);
  ont = EEPROMFloatRead(7);
  offt = EEPROMFloatRead(8);
  alrt = EEPROMIntRead(205);
  hAlT = EEPROMFloatRead(11);
  hAlH = EEPROMFloatRead(12);
  lAlT = EEPROMFloatRead(13);
  lAlH = EEPROMFloatRead(14);
  
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
  //Out  Config

  {
    //Relay 1 State //Temp
    if(HCState == 0){//Heating State
      if(getTemp()<=(getOTemp()-difDT)){
        digitalWrite(R1,HIGH);
      }
      Display();
      if(getTemp()>=(getOTemp()+difUT)){
        digitalWrite(R1,LOW);
      }
    }

    Display();
    if(HCState == 1){//Coolling State
      if(getTemp()>=(getOTemp()+difUT)){
        digitalWrite(R1,HIGH);
      }
      Display();
      if(getTemp()<=(getOTemp()-difDT)){
        digitalWrite(R1,LOW);
      }
    }
    Display();

    //Relay 3 State //Fan
    //1- Timer
    if((millis()-lCTime>0)&&(millis()-lCTime<(offt*60000))&& gW==0 && offt!=0){
      digitalWrite(R2,LOW);
    }else if((millis()-lCTime>=(offt*60000))&&(millis()-lCTime<((ont+offt)*60000)) && ont!=0){
      digitalWrite(R2,HIGH);
    }else if(millis()-lCTime>=((ont+offt)*60000)){
      lCTime = millis();
    }
    Display();
    // 2- Gloal warmming
    if(getTemp()>=(getOTemp()+upt) && upt!=0){
      digitalWrite(R2,HIGH);
      gW = 1;
    }else if(getTemp()<=(getOTemp()+dt) && dt!=0){
      gW = 0;
    }
    Display();
    
    //Alart
    if(getTemp()>=(getOTemp()+hAlT)){
      onAlert();
      Display();
    }else if(getTemp()<=(getOTemp()-lAlT)){
      onAlert();
      Display();
    }else offAlert();
   Display();
  }
  //////////Layer 0
  if(layer == 0){

    if (isnan(hum) || isnan(temp) || isnan(tempF)) {
      Display();
      if(isPT()){
        Display();
        setYt(getTemp());
      }else{
        setY(X[21],X[21],X[21]);
        Display();
      }
    }else{
      if(millis()-lTime<=5000){
        if(TState == 0){
          setYt(getTemp());
        }
        if(TState == 1){
          setYt(getTemp());
        }
        Display();
      }
      else if((millis()-lTime>5000)&&(millis()-lTime<=10000)){
        setYh(hum);
        Display();
      }
      else {
        lTime = millis();
      }
    }
    
    Display();

    switch(checkButton()){
      case 1: break;
      case 2: break;
      case 3: break;
      case 4: layer = 1; state1 = 0; break;
      case 5: layer = 2; state2 = 0; break;
    }
    Display();
  }
  
  //////////Layer 1>>>SR ORDER H AND T VaLUE
  if(layer == 1){
    if(state1==0){
      setY(0,0,X[18]);
      Display();
      switch(checkButton()){
        case 1: state1 = 0; break;
        case 2: state1 = 0; break;
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

  //////////Layer 2//Set Configration layer
  if(layer == 2){
    setY(X[5],X[10],X[19]);
    for(int i=0; i<500; i++){
      Display();
    }
    while(state2 == 0&&layer == 2){
      setY(X[5],X[10],X[19]);
      Display();
      switch(checkButton()){
        case 1: break;
        case 2: break;
        case 3: layer = 0; break;
        case 4: state2 = 1; break;
      }
    }
    
    while(state2 == 1&&layer == 2){
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
    //Pn
    if(state3 == 2){
      setY(0,X[20],X[19]);
      Display();
      switch(checkButton()){
        case 1: state3 = 3; break;
        case 2: state3 = 1; break;
        case 3: layer = 0; break;
        case 4: state3 = 21; break;
      }
    }
    {
      if(state3==21){
        setY(0,0,X[18]);
        Display();
        switch(checkButton()){
          case 1: break;
          case 2: break;
          case 3: state3 = 2; break;
          case 4: if(HCState == 0){state3 = 211;}if(HCState == 1){state3 = 212;} break;
        }
      }
      {
        if(state3==211){//Heat State
          setY(0,0,X[16]);
          Display();
          switch(checkButton()){
            case 1: state3 = 212; break;
            case 2: state3 = 212; break;
            case 3: state3 = 21; HCState = 0; EEPROMIntWrite(203,HCState); break;
            case 4: state3 = 21; HCState = 0; EEPROMIntWrite(203,HCState); break;
          }
        }
        if(state3==212){//Cool State
          setY(0,0,X[12]);
          Display();
          switch(checkButton()){
            case 1: state3 = 211; break;
            case 2: state3 = 211; break;
            case 3: state3 = 21; HCState = 1; EEPROMIntWrite(203,HCState); break;
            case 4: state3 = 21; HCState = 1; EEPROMIntWrite(203,HCState); break;
          }
        }
      }
           
    }
    
    //FC
    if(state3 == 3){
      setY(0,X[12],X[15]);
      Display();
      switch(checkButton()){
        case 1: state3 = 4; break;
        case 2: state3 = 2; break;
        case 3: layer = 0; break;
        case 4: if(TState==0){state3=31;}else if(TState==1){state3=32;}break;
      }
    }
    {
      if(state3==31){
        setY(0,0,X[12]);
        Display();
        switch(checkButton()){
          case 1: state3 = 32; break;
          case 2: state3 = 32; break;
          case 3: state3 = 3; TState=0; EEPROMIntWrite(201,TState); break;
          case 4: state3 = 3; TState=0; EEPROMIntWrite(201,TState); break;
        }
      }
      if(state3==32){
        setY(0,0,X[15]);
        Display();
        switch(checkButton()){
          case 1: state3 = 31; break;
          case 2: state3 = 31; break;
          case 3: state3 = 3; TState=1; EEPROMIntWrite(201,TState); break;
          case 4: state3 = 3; TState=1; EEPROMIntWrite(201,TState); break;
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
      if(state3 == 51){//Time Cy Cycle
      setY(0,X[22],X[12]);
      Display();
      switch(checkButton()){
        case 1: state3 = 52; break;
        case 2: state3 = 52; break;
        case 3: state3 = 5; break;
        case 4: state3 = 511; break;
      }
    }
    {
      if(state3 == 511){//On Time
        setY(0,X[20],X[0]);
        Display();
        switch(checkButton()){
          case 1: state3 = 512; break;
          case 2: state3 = 512; break;
          case 3: state3 = 51; break;
          case 4: state3 = 5111; break;
        }
      }
      {
        if(state3 == 5111){
          setYf(ont);
          Display();
          switch(checkButton()){
            case 1: if(ont>0&&ont<=10.1){ont-=0.1;} break;
            case 2: if(ont>=-1&&ont<10.0){ont+=0.1;} break;
            case 3: state3 = 511; EEPROMFloatWrite(7,ont); break;
            case 4: state3 = 511; EEPROMFloatWrite(7,ont); break;
          }
        }
      }
      if(state3 == 512){//Off Time
        setY(X[15],X[15],X[0]);
        Display();
        switch(checkButton()){
          case 1: state3 = 511; break;
          case 2: state3 = 511; break;
          case 3: state3 = 51; break;
          case 4: state3 = 5121; break;
        }
      }
      {
        if(state3 == 5121){
          setYf(offt);
          Display();
          switch(checkButton()){
            case 1: if(offt>0&&offt<=10.1){offt-=0.1;} break;
            case 2: if(offt>=-1&&offt<10.0){offt+=0.1;} break;
            case 3: state3 = 512; EEPROMFloatWrite(8,offt); break;
            case 4: state3 = 512; EEPROMFloatWrite(8,offt); break;
          }
        }
      }
    }

    if(state3 == 52){//High Temp Global Warmming
      setY(0,X[18],X[16]);
      Display();
      switch(checkButton()){
        case 1: state3 = 51; break;
        case 2: state3 = 51; break;
        case 3: state3 = 5; break;
        case 4: state3 = 521; break;
      }
    }
    {
      if(state3 == 521){//up temp
        setY(X[18],X[19],X[23]);
        Display();
        switch(checkButton()){
          case 1: state3 = 522; break;
          case 2: state3 = 522; break;
          case 3: state3 = 52; break;
          case 4: state3 = 5211; break;
        }
      }
      {
        if(state3 == 5211){
          setYf(upt);
          Display();
          switch(checkButton()){
            case 1: if(upt>0&&upt<=5.1){upt-=0.1;} break;
            case 2: if(upt>=-1&&upt<5.0){upt+=0.1;} break;
            case 3: state3 = 521; EEPROMFloatWrite(5,upt); break;
            case 4: state3 = 521; EEPROMFloatWrite(5,upt); break;
          }
        }
      }
      if(state3 == 522){// down temp
        setY(0,X[18],X[13]);
        Display();
        switch(checkButton()){
          case 1: state3 = 521; break;
          case 2: state3 = 521; break;
          case 3: state3 = 52; break;
          case 4: state3 = 5221; break;
        }
      }
      {
        if(state3 == 5221){
          setYf(dt);
          Display();
          switch(checkButton()){
            case 1: if(dt>0&&dt<=5.1){dt-=0.1;} break;
            case 2: if(dt>=-1&&dt<5.0){dt+=0.1;} break;
            case 3: state3 = 522; EEPROMFloatWrite(6,dt); break;
            case 4: state3 = 522; EEPROMFloatWrite(6,dt); break;
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
          case 1: state3=722; break;
          case 2: state3=722; break;
          case 3: state3=72; break;
          case 4: state3=7211; break;
        }
      }
        if(state3==7211){
          setYf(hAlT);
          Display();
          switch(checkButton()){
            case 1: if(hAlT>0&&hAlT<=25.1){hAlT-=0.1;} break;
            case 2: if(hAlT>=-1&&hAlT<25){hAlT+=0.1;} break;
            case 3: state3=721; EEPROMFloatWrite(11,hAlT); break;
            case 4: state3=721; EEPROMFloatWrite(11,hAlT); break;
          }
        }
      if(state3==722){
        setY(0,0,X[16]);
        Display();
        switch(checkButton()){
          case 1: state3=721; break;
          case 2: state3=721; break;
          case 3: state3=72; break;
          case 4: state3=7221; break;
        }
      }
        if(state3==7221){
          setYf(hAlH);
          Display();
          switch(checkButton()){
            case 1: if(hAlH>0&&hAlH<=25.1){hAlH-=0.1;} break;
            case 2: if(hAlH>=-1&&hAlH<25){hAlH+=0.1;} break;
            case 3: state3=722; EEPROMFloatWrite(12,hAlH); break;
            case 4: state3=722; EEPROMFloatWrite(12,hAlH); break;
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
          case 1: state3=732; break;
          case 2: state3=732; break;
          case 3: state3=73; break;
          case 4: state3 = 7311; break;
        }
      }
        if(state3==7311){
          setYf(lAlT);
          Display();
          switch(checkButton()){
            case 1: if(lAlT>0&&lAlT<=25.1){lAlT-=0.1;} break;
            case 2: if(lAlT>=-1&&lAlT<25){lAlT+=0.1;} break;
            case 3: state3=731; EEPROMFloatWrite(13,lAlT); break;
            case 4: state3=731; EEPROMFloatWrite(13,lAlT); break;
          }
        }
      if(state3==732){
        setY(0,0,X[16]);
        Display();
        switch(checkButton()){
          case 1: state3=731; break;
          case 2: state3=731; break;
          case 3: state3=73; break;
          case 4: state3=7321; break;
        }
      }
        if(state3==7321){
          setYf(lAlH);
          Display();
          switch(checkButton()){
            case 1: if(lAlH>0&&lAlH<=25.1){lAlH-=0.1;} break;
            case 2: if(lAlH>=-1&&lAlH<25){lAlH+=0.1;} break;
            case 3: state3=732; EEPROMFloatWrite(14,lAlH); break;
            case 4: state3=732; EEPROMFloatWrite(14,lAlH); break;
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
  A/=100;
  Display();
  float T = (A-747.7)/-9.27;
  Display();
  return T;
}

void offAlert(){
  digitalWrite(InAl,LOW);
  digitalWrite(ExAl,HIGH);
}
void onAlert(){
  if(alrt==0){
    digitalWrite(InAl,HIGH);
    digitalWrite(ExAl,HIGH);
  }else if(alrt==1){
    digitalWrite(InAl,LOW);
    digitalWrite(ExAl,LOW);
  }
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
  else if((analogRead(BT)>320&&analogRead(BT)<360)&&(millis()-lBTime>100)){
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
  else if((analogRead(BT)>700&&analogRead(BT)<750)&&(lButton != 4)){
    if(millis()-lBBTime>300){
      lBTime = millis();
      lButton = 4;
      return 4;//set
    }else{return 0;}
  }else if((analogRead(BT)>430&&analogRead(BT)<500)){
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
