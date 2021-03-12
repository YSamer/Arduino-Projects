#include <HX711.h>
#include <Keypad.h>
//Max
#define MAX_Data 2
#define MAX_Load  4
#define MAX_Clk 3

//HX711
const int DATA = A0;
const int CLK = A1;
HX711 scale;
double W = 0;
long x = 0;
long x0 = 0;
double W0 = 0;

//calib
long y1=0, y2=0;
int N[6]={0,0,0,0,0,0};

void setup() {
  //Max Config
  pinMode(MAX_Data, OUTPUT);
  pinMode(MAX_Load, OUTPUT);
  pinMode(MAX_Clk, OUTPUT);
  digitalWrite(MAX_Clk, HIGH);
  delay(200);
  //Setup of MAX7219 chip
  shift(0x0f, 0x00); //display test register - test mode off
  shift(0x0c, 0x01); //shutdown register - normal operation
  shift(0x0b, 0x03); //scan limit register - display digits 0 thru 7
  shift(0x0a, 0x0f); //intensity register - max brightness
  shift(0x09, 0xff); //decode mode register - CodeB decode all digits

  //HX711 Config
  scale.begin(DATA, CLK);
  delay(1000);
  x0 = scale.read();
  W0 = (-0.10314 * x0) - 14537.77;

  //Kaypad
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);
  pinMode(6, INPUT);
  pinMode(7, INPUT);
  pinMode(8, INPUT);
  pinMode(9, INPUT);
  digitalWrite(10, LOW);
  digitalWrite(11, LOW);
  digitalWrite(12, LOW);
  digitalWrite(13, LOW);

  //Relay
  pinMode(A2, OUTPUT);
  pinMode(A3, OUTPUT);
  pinMode(A4, OUTPUT);
  pinMode(A5, OUTPUT);
  digitalWrite(A2, LOW);
  digitalWrite(A3, LOW);
  digitalWrite(A4, LOW);
  digitalWrite(A5, LOW);
  
}

void loop() {
  
  delay(500);
  if(checkKey()==0){zeros();}

  if(checkKey()==12){Calibration();}
  
  if (scale.is_ready()){
    x = scale.read();
    W = (-0.10314 * x) - 14537.77 - W0;
//    W = (int)(W*0.02);
//    W = W/0.02;
    if(W>-50&&W<50){
      segInt(0);
    }else{
      segFloat(W/1000);
    }
    
  }else {
    segInt(0);
  }
}


void Calibration(){
  
  int digitInt = 0;
  int calibState = 1;
  

  shift(0x0b, 0x06); shift(6, 11); shift(5, 1); shift(4, 15); shift(3, 15); shift(2, 15); shift(1, 15); //E1
  delay(2000);
  while(calibState==1){

    if(checkKey()<=9){
      digitInt++;
      if(digitInt==1){N[0]=checkKey();}
      else if(digitInt==2){N[1]=checkKey();}
      else if(digitInt==3){N[2]=checkKey();}
      else if(digitInt==4){N[3]=checkKey();}
      else if(digitInt==5){N[4]=checkKey();}
      else if(digitInt==6){N[5]=checkKey();}
    }
    
    while((checkKey()!=20));
    delay(100);
    
    y1=0;
    for(int i=1; i<=digitInt; i++){
      y1+=N[i-1]*pow(10,(digitInt-i));
    }
    if(y1>=100){y1+=1;}
    segInt(y1);

    
    if(checkKey()==15){calibState = 2;}
    if(checkKey()==14){calibState = 0;}
  }
  
  while(checkKey()!=20);
  if(calibState==2){
    for(int i=0; i<6; i++){N[i]=0;}
    digitInt = 0;
    shift(0x0b, 0x06); shift(6, 11); shift(5, 2); shift(4, 15); shift(3, 15); shift(2, 15); shift(1, 15); //E2
    delay(2000);
  }else{
    for(int i=0; i<6; i++){N[i]=0;}
    digitInt = 0;
    while(checkKey()!=20);
    delay(100);
  }
    
  
  while(calibState == 2){
    if(checkKey()<=9){
      digitInt++;
      if(digitInt==1){N[0]=checkKey();}
      else if(digitInt==2){N[1]=checkKey();}
      else if(digitInt==3){N[2]=checkKey();}
      else if(digitInt==4){N[3]=checkKey();}
      else if(digitInt==5){N[4]=checkKey();}
      else if(digitInt==6){N[5]=checkKey();}
    }
    
    while((checkKey()!=20));
    delay(200);
    
    y2=0;
    for(int i=1; i<=digitInt; i++){
      y2+=N[i-1]*pow(10,(digitInt-i));
    }
    if(y2>=100){y2+=1;}
    segInt(y2);
    
    if(checkKey()==15){calibState = 3;}
    if(checkKey()==14){calibState = 0;}
  }
  
}

void zeros() {
  x0 = scale.read();
  W0 = (-0.10314 * x0) - 14537.77;
}

int checkKey() {
  digitalWrite(10, HIGH);
  digitalWrite(11, LOW);
  digitalWrite(12, LOW);
  digitalWrite(13, LOW);
  if (digitalRead(6) == HIGH)return 1;
  else if (digitalRead(7) == HIGH)return 4;
  else if (digitalRead(8) == HIGH)return 7;
  else if (digitalRead(9) == HIGH)return 14; //*
  digitalWrite(10, LOW);
  digitalWrite(11, HIGH);
  digitalWrite(12, LOW);
  digitalWrite(13, LOW);
  if (digitalRead(6) == HIGH)return 2;
  else if (digitalRead(7) == HIGH)return 5;
  else if (digitalRead(8) == HIGH)return 8;
  else if (digitalRead(9) == HIGH)return 0; //0
  digitalWrite(10, LOW);
  digitalWrite(11, LOW);
  digitalWrite(12, HIGH);
  digitalWrite(13, LOW);
  if (digitalRead(6) == HIGH)return 3;
  else if (digitalRead(7) == HIGH)return 6;
  else if (digitalRead(8) == HIGH)return 9;
  else if (digitalRead(9) == HIGH)return 15; //#
  digitalWrite(10, LOW);
  digitalWrite(11, LOW);
  digitalWrite(12, LOW);
  digitalWrite(13, HIGH);
  if (digitalRead(6) == HIGH)return 10; //A
  else if (digitalRead(7) == HIGH)return 11; //B
  else if (digitalRead(8) == HIGH)return 12; //C
  else if (digitalRead(9) == HIGH)return 13; //D
  else return 20;
  digitalWrite(10, LOW);
  digitalWrite(11, LOW);
  digitalWrite(12, LOW);
  digitalWrite(13, LOW);
}

void segFloat(float F) {
  long L = (long)(F * 100);
  int digit = 1;//digit num
  int n1 = 0, n2 = 0, n3 = 0, n4 = 0, n5 = 0, n6 = 0;
  if (L >= 100000 && L < 1000000) {
    digit = 6;
    shift(0x0b, 0x05);
  } else if (L >= 10000 && L < 100000) {
    digit = 5;
    shift(0x0b, 0x04);
  } else if (L >= 1000 && L < 10000) {
    digit = 4;
    shift(0x0b, 0x03);
  } else if (L >= 100 && L < 1000) {
    digit = 3;
    shift(0x0b, 0x02);
  } else if (L >= 10 && L < 100) {
    digit = 3;
    shift(0x0b, 0x02);
  } else if (L >= 0 && L < 10) {
    digit = 3;
    shift(0x0b, 0x02);
  } else if (L < 0 && L > -10) {
    digit = -3;
    shift(0x0b, 0x03);
    shift(2, 10);
  } else if (L <= -10 && L > -100) {
    digit = -3;
    shift(0x0b, 0x03);
    shift(3, 10);
  } else if (L <= -100 && L > -1000) {
    digit = -3;
    shift(0x0b, 0x03);
    shift(4, 10);
  } else if (L <= -1000 && L > -10000) {
    digit = -4;
    shift(0x0b, 0x04);
    shift(5, 10);
  } else if (L <= -10000 && L > -100000) {
    digit = -5;
    shift(0x0b, 0x05);
    shift(6, 10);
  }
  if (digit >= 1 && digit <= 6) {
    int j = 0;
    for (int i = digit; i >= 1; i--) {
      int n = L / pow(10, digit - 1 - j);
      if (digit - j == 3) {
        shift(digit - j, n | 0x80);
      } else {
        shift(digit - j, n);
      }
      L -= n * pow(10, digit - 1 - j);
      j++;
    }
  }
  else if (digit <= -1 && digit >= -5) {
    digit *= -1;
    shift(digit + 1, 10);
    L *= -1;
    int j = 0;
    for (int i = digit; i >= 1; i--) {
      int n = L / pow(10, digit - 1 - j);
      if (digit - j == 3) {
        shift(digit - j, n | 0x80);
      } else {
        shift(digit - j, n);
      }
      L -= n * pow(10, digit - 1 - j);
      j++;
    }
  }
}

void segInt(long L) {
  int digit = 1;//digit num
  int n1 = 0, n2 = 0, n3 = 0, n4 = 0, n5 = 0, n6 = 0;
  if (L >= 100000 && L < 1000000) {
    digit = 6;
    shift(0x0b, 0x05);
  } else if (L >= 10000 && L < 100000) {
    digit = 5;
    shift(0x0b, 0x04);
  } else if (L >= 1000 && L < 10000) {
    digit = 4;
    shift(0x0b, 0x03);
  } else if (L >= 100 && L < 1000) {
    digit = 3;
    shift(0x0b, 0x02);
  } else if (L >= 10 && L < 100) {
    digit = 2;
    shift(0x0b, 0x01);
  } else if (L >= 0 && L < 10) {
    digit = 1;
    shift(0x0b, 0x00);
  } else if (L < 0 && L > -10) {
    digit = -1;
    shift(0x0b, 0x01);
    shift(2, 10);
  } else if (L <= -10 && L > -100) {
    digit = -2;
    shift(0x0b, 0x02);
    shift(3, 10);
  } else if (L <= -100 && L > -1000) {
    digit = -3;
    shift(0x0b, 0x03);
    shift(4, 10);
  } else if (L <= -1000 && L > -10000) {
    digit = -4;
    shift(0x0b, 0x04);
    shift(5, 10);
  } else if (L <= -10000 && L > -100000) {
    digit = -5;
    shift(0x0b, 0x05);
    shift(6, 10);
  }
  if (digit >= 1 && digit <= 6) {
    int j = 0;
    for (int i = digit; i >= 1; i--) {
      int n = L / pow(10, digit - 1 - j);
      shift(digit - j, n);
      L -= n * pow(10, digit - 1 - j);
      j++;
    }
  }
  else if (digit <= -1 && digit >= -5) {
    digit *= -1;
    shift(digit + 1, 10);
    L *= -1;
    int j = 0;
    for (int i = digit; i >= 1; i--) {
      int n = L / pow(10, digit - 1 - j);
      shift(digit - j, n);
      L -= n * pow(10, digit - 1 - j);
      j++;
    }
  }

}

void shift(byte adr, byte dt) {
  digitalWrite(MAX_Load, LOW);
  shiftOut(MAX_Data, MAX_Clk, MSBFIRST, adr);
  shiftOut(MAX_Data, MAX_Clk, MSBFIRST, dt);
  digitalWrite(MAX_Load, HIGH);
}
