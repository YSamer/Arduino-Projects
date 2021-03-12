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

//sensor
#define DHTPIN A1
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);
float hum=0.0;
float temp=0.0;
float tempF=0.0;
int layer = 0;
unsigned long lTime=0;
unsigned long lBTime=0;
unsigned long lBBTime=0;
unsigned long lATime=0;
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
int pass  = 0;//E200
float difT = 0.0;//E2
float difH = 0.0;//E3
int TState = 0;  //E201
int FState = 1;  //E202
int HCState = 0; //E203
int HDState = 0; //E204
float upt = 0.0;//E5
float dt = 0.0;//E6
float ont = 0.0;//E7
float offt = 0.0;//E8
int alrt = 0;  //E205
float hAlT = 0.0; //E11
float hAlH = 0.0; //E12
float lAlT = 0.0; //E13
float lAlH = 0.0; //E14
float tAlrt = 0.0; //E15

void setup() {
  dht.begin();
  Serial.begin(9600);
  
  pinMode(data, OUTPUT);
  pinMode(rclk, OUTPUT);
  pinMode(sclk, OUTPUT);
  pinMode(d0, OUTPUT);
  pinMode(d1, OUTPUT);
  pinMode(d2, OUTPUT);  
  pinMode(R1, OUTPUT);
  pinMode(BT, INPUT);
  
  digitalWrite(d0,HIGH);
  digitalWrite(d1,HIGH);
  digitalWrite(d2,HIGH);
  digitalWrite(R1,HIGH);
  lTime = millis();
  lATime = millis();
  

//  EEPROMFloatWrite(2,0);
//  EEPROMFloatWrite(3,0);
//  EEPROMIntWrite(201,0);
//  EEPROMIntWrite(200,0);
//  EEPROMIntWrite(202,0);
//  EEPROMIntWrite(203,0);
//  EEPROMIntWrite(204,0);
//  EEPROMFloatWrite(5,0);
//  EEPROMFloatWrite(6,0);
//  EEPROMFloatWrite(7,0);
//  EEPROMFloatWrite(8,0);
//  EEPROMIntWrite(205,0);
//  EEPROMFloatWrite(11,22.0);
//  EEPROMFloatWrite(12,75.0);
//  EEPROMFloatWrite(13,18.0);
//  EEPROMFloatWrite(14,35.0);
//  EEPROMFloatWrite(15,1.0);


  pass  = EEPROMIntRead(200);
  difT = EEPROMFloatRead(2);
  difH = EEPROMFloatRead(3);
  TState = EEPROMIntRead(201);
  FState = EEPROMIntRead(202);
  HCState = EEPROMIntRead(203);
  HDState = EEPROMIntRead(204);
  upt = EEPROMFloatRead(5);
  dt = EEPROMFloatRead(6);
  ont = EEPROMFloatRead(7);
  offt = EEPROMFloatRead(8);
  alrt = EEPROMIntRead(205);
  hAlT = EEPROMFloatRead(11);
  hAlH = EEPROMFloatRead(12);
  lAlT = EEPROMFloatRead(13);
  lAlH = EEPROMFloatRead(14);
  tAlrt = EEPROMFloatRead(15);
}

void loop() {
  
  //////////Layer 0
  if(layer == 0){
    hum   = dht.readHumidity();
    temp  = dht.readTemperature();
    tempF = dht.readTemperature(true);
    
    if (isnan(hum) || isnan(temp) || isnan(tempF)) {
      setY(X[21],X[21],X[21]);
    }else{
      if(millis()-lTime<=5000){
        if(TState == 0){
          setYt(temp);
        }
        if(TState == 1){
          setYt(tempF);
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
      case 4: layer = 1; state1 = 7; break;
      case 5: layer = 2; state2 = 0; break;
    }
    
    
    //Alart
    if((temp>=hAlT+difT)||(temp<=lAlT-difT)||(hum>=hAlH+difH)||(hum<=lAlH-difH)){
      if(alrt==0){
        digitalWrite(R1,LOW);
        lATime = millis();
      }
      if(alrt==1){
        if(millis()-lATime<(tAlrt*60000)){
          digitalWrite(R1,LOW);
        }else{
          digitalWrite(R1,HIGH);
        }
      }
    }else if((temp<=hAlT-difT)||(temp>=lAlT+difT)||(hum<=hAlH-difH)||(hum>=lAlH+difH)){
      digitalWrite(R1,HIGH);
      lATime = millis();
    }
    
  }
  
  //////////Layer 1>>>SR ORDER H AND T VaLUE
  if(layer == 1){
    if(state1 == 7){
      setY(0,X[21],X[10]);
      Display();
      switch(checkButton()){
        case 1: break;
        case 2: break;
        case 3: layer = 0; break;
        case 4: state1=71; break;
      }
    }
    {//In or Ex Alart
      if(state1 == 71){
        setY(X[21],X[10],X[18]);//time alart
        Display();
        switch(checkButton()){
          case 1: state1 = 72; break;
          case 2: state1 = 73; break;
          case 3: state1 = 7; break;
          case 4: if(alrt==0){state1=711;}else if(alrt==1){state1=712;}break;
        }
      }
      if(state1 == 711){
        setY(0,X[21],X[10]);//al all time 
        Display();
        switch(checkButton()){
          case 1: state1 = 712; break;
          case 2: state1 = 712; break;
          case 3: state1 = 71; alrt=0; EEPROMIntWrite(205,alrt); break;
          case 4: state1 = 71; alrt=0; EEPROMIntWrite(205,alrt); break;
        }
      }
      if(state1 == 712){
        setY(X[10],X[20],X[0]);//al on time
        Display();
        switch(checkButton()){
          case 1: state1 = 711; break;
          case 2: state1 = 711; break;
          case 3: state1 = 71; alrt=1; EEPROMIntWrite(205,alrt); break;
          case 4: state1 = 7121; alrt=1; EEPROMIntWrite(205,alrt); break;
        }
      }
      if(state1==7121){
        setYf(tAlrt);
        Display();
        switch(checkButton()){
          case 1: if(tAlrt>0&&tAlrt<=31){tAlrt-=0.1;} break;
          case 2: if(tAlrt>=-1&&tAlrt<30){tAlrt+=0.1;} break;
          case 3: state1=712; EEPROMFloatWrite(15,tAlrt); break;
          case 4: state1=712; EEPROMFloatWrite(15,tAlrt); break;
        }
      }
    }
    {//High Alart
      if(state1 == 72){
        setY(X[21],X[10],X[16]);
        Display();
        switch(checkButton()){
          case 1: state1=73; break;
          case 2: state1=71; break;
          case 3: state1=7; break;
          case 4: state1=721; break;
        }
      }
      if(state1==721){
        setY(0,0,X[18]);
        Display();
        switch(checkButton()){
          case 1: state1=722; break;
          case 2: state1=722; break;
          case 3: state1=72; break;
          case 4: state1=7211; break;
        }
      }
        if(state1==7211){
          setYf(hAlT);
          Display();
          switch(checkButton()){
            case 1: if(hAlT>0&&hAlT<=100){hAlT-=0.1;}  else if(hAlT>100&&hAlT<=201){hAlT--;} break;
            case 2: if(hAlT>=-1&&hAlT<100){hAlT+=0.1;} else if(hAlT>=100&&hAlT<200){hAlT++;} break;
            case 3: state1=721; EEPROMFloatWrite(11,hAlT); break;
            case 4: state1=721; EEPROMFloatWrite(11,hAlT); break;
          }
        }
      if(state1==722){
        setY(0,0,X[16]);
        Display();
        switch(checkButton()){
          case 1: state1=721; break;
          case 2: state1=721; break;
          case 3: state1=72; break;
          case 4: state1=7221; break;
        }
      }
        if(state1==7221){
          setYf(hAlH);
          Display();
          switch(checkButton()){
            case 1: if(hAlH>0&&hAlH<=100){hAlH-=0.1;}  else if(hAlH>100&&hAlH<=201){hAlH--;} break;
            case 2: if(hAlH>=-1&&hAlH<100){hAlH+=0.1;} else if(hAlH>=100&&hAlH<200){hAlH++;} break;
            case 3: state1=722; EEPROMFloatWrite(12,hAlH); break;
            case 4: state1=722; EEPROMFloatWrite(12,hAlH); break;
          }
        }
    }
    {//LOW Alart
      if(state1 == 73){
        setY(X[21],X[10],X[21]);
        Display();
        switch(checkButton()){
          case 1: state1 =71; break;
          case 2: state1 =72; break;
          case 3: state1=7; break;
          case 4: state1=731; break;
        }
      }
      if(state1==731){
        setY(0,0,X[18]);
        Display();
        switch(checkButton()){
          case 1: state1=732; break;
          case 2: state1=732; break;
          case 3: state1=73; break;
          case 4: state1 = 7311; break;
        }
      }
        if(state1==7311){
          setYf(lAlT);
          Display();
          switch(checkButton()){
            case 1: if(lAlT>0&&lAlT<=100){lAlT-=0.1;}  else if(lAlT>100&&lAlT<=201){lAlT--;} break;
            case 2: if(lAlT>=-1&&lAlT<100){lAlT+=0.1;} else if(lAlT>=100&&lAlT<200){lAlT++;} break;
            case 3: state1=731; EEPROMFloatWrite(13,lAlT); break;
            case 4: state1=731; EEPROMFloatWrite(13,lAlT); break;
          }
        }
      if(state1==732){
        setY(0,0,X[16]);
        Display();
        switch(checkButton()){
          case 1: state1=731; break;
          case 2: state1=731; break;
          case 3: state1=73; break;
          case 4: state1=7321; break;
        }
      }
        if(state1==7321){
          setYf(lAlH);
          Display();
          switch(checkButton()){
            case 1: if(lAlH>0&&lAlH<=100){lAlH-=0.1;}  else if(lAlH>100&&lAlH<=201){lAlH--;} break;
            case 2: if(lAlH>=-1&&lAlH<100){lAlH+=0.1;} else if(lAlH>=100&&lAlH<200){lAlH++;} break;
            case 3: state1=732; EEPROMFloatWrite(14,lAlH); break;
            case 4: state1=732; EEPROMFloatWrite(14,lAlH); break;
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

  if(layer == 3){
    //dif
    if(state3 == 1){
      setY(X[15],X[1],X[13]);
      Display();
      switch(checkButton()){
        case 1: state3 = 2; break;
        case 2: state3 = 6;break;
        case 3: layer = 0; break;
        case 4: state3 = 11; break;
      }
    }
    {
      if(state3==11){
        setY(0,0,X[18]);
        Display();
        switch(checkButton()){
          case 1: state3 = 12; break;
          case 2: state3 = 12; break;
          case 3: state3 = 1; break;
          case 4: state3 = 13; break;
        }
      }
      if(state3==12){
        setY(0,0,X[16]);
        Display();
        switch(checkButton()){
          case 1: state3 = 11; break;
          case 2: state3 = 11; break;
          case 3: state3 = 1; break;
          case 4: state3 = 14; break;
        }
      }
      if(state3==13){
        setYf(difT);
        Display();
        switch(checkButton()){
          case 1: if(difT>0&&difT<=5.1){difT-=0.1;} break;
          case 2: if(difT>=-1&&difT<5.0){difT+=0.1;} break;
          case 3: state3 = 11; EEPROMFloatWrite(2,difT); break;
          case 4: state3 = 11; EEPROMFloatWrite(2,difT); break;
        }
      }
      if(state3==14){
        setYf(difH);
        Display();
        switch(checkButton()){
          case 1: if(difH>0&&difH<=5.1){difH-=0.1;} break;
          case 2: if(difH>=-1&&difH<5.0){difH+=0.1;} break;
          case 3: state3 = 12;  EEPROMFloatWrite(3,difH); break;
          case 4: state3 = 12;  EEPROMFloatWrite(3,difH); break;
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
          case 1: state3 = 22; break;
          case 2: state3 = 22; break;
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
      if(state3==22){
        setY(0,0,X[16]);
        Display();
        switch(checkButton()){
          case 1: state3 = 21; break;
          case 2: state3 = 21; break;
          case 3: state3 = 2; break;
          case 4: if(HDState == 0){state3 = 221;}if(HDState == 1){state3 = 222;} break;
        }
      }
      {
        if(state3==221){
          setY(0,0,X[16]|0b00010000);
          Display();
          switch(checkButton()){
            case 1: state3 = 222; break;
            case 2: state3 = 222; break;
            case 3: state3 = 22; HDState = 0; EEPROMIntWrite(204,HDState); break;
            case 4: state3 = 22; HDState = 0; EEPROMIntWrite(204,HDState); break;
          }
        }
        if(state3==222){
          setY(0,0,X[13]);
          Display();
          switch(checkButton()){
            case 1: state3 = 221; break;
            case 2: state3 = 221; break;
            case 3: state3 = 22; HDState = 1; EEPROMIntWrite(204,HDState); break;
            case 4: state3 = 22; HDState = 1; EEPROMIntWrite(204,HDState); break;
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
        case 1: state3 = 1; break;
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
        FState=0;
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
