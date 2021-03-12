#include <LiquidCrystal.h>
const int rs = A5, en = A4, d4 = A3, d5 = A2, d6 = A1, d7 = A0;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
#define A 11
#define B 10
#define IR 12
#define S 9
#define R 8

int aState;
int lastState;

unsigned long i =0;
unsigned long j =0;
unsigned long lTime =0;

void(* resetFunc) (void) = 0;

void setup() {
  delay(1000);
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print(j);
  pinMode(IR,INPUT);
  pinMode(A,INPUT);
  pinMode(B,INPUT);
  pinMode(S,INPUT);
  pinMode(R,OUTPUT);
  digitalWrite(R,LOW);
  
  lastState = digitalRead(A);

  while(digitalRead(S)){
    aState = digitalRead(A);
    if(aState != lastState){
      if(digitalRead(B)!= aState){
        if(millis()-lTime<10){
          j+=50;
        }
        else if(millis()-lTime<20){
          j+=25;
        }
        else if(millis()-lTime<30){
          j+=15;
        }
        else
        j++;
        
        lTime = millis();
      }else{
        if(!(j==0)){
         j--; 
        }
      }
    }
    lastState = aState;
    lcd.setCursor(0,1);
    lcd.print(j/2);
    lcd.print("               ");
  }

  j=j/2;
  
}

void loop() {
  
  if(i<j){
    digitalWrite(R,HIGH);
    if(digitalRead(IR)) {
      i++;
      while(digitalRead(IR));
    }
    lcd.setCursor(0,0);
    lcd.print(i);
    lcd.print("               ");
  }else{
    digitalWrite(R,LOW);
    i=0;
    j=0;
    resetFunc();
  }
  
}
