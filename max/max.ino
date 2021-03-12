#define MAX_Data 2
#define MAX_Load  4
#define MAX_Clk 3

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

}

void loop() {
  
  segFloat(9875.25);
  delay(1000);
}

void segFloat(float F){
  long L = (long)(F*100);
  int digit = 1;//digit num
  int n1=0,n2=0,n3=0,n4=0,n5=0,n6=0;
  if(L>=100000&&L<1000000){
    digit = 6;
    shift(0x0b, 0x05);
  }else if(L>=10000&&L<100000){
    digit = 5;
    shift(0x0b, 0x04);
  }else if(L>=1000&&L<10000){
    digit = 4;
    shift(0x0b, 0x03);
  }else if(L>=100&&L<1000){
    digit = 3;
    shift(0x0b, 0x02);
  }else if(L>=10&&L<100){
    digit = 3;
    shift(0x0b, 0x02);
  }else if(L>=0&&L<10){
    digit = 3;
    shift(0x0b, 0x02);
  }else if(L<0&&L>-10){
    digit = -3;
    shift(0x0b, 0x03);
    shift(2,10);
  }else if(L<=-10&&L>-100){
    digit = -3;
    shift(0x0b, 0x03);
    shift(3,10);
  }else if(L<=-100&&L>-1000){
    digit = -3;
    shift(0x0b, 0x03);
    shift(4,10);
  }else if(L<=-1000&&L>-10000){
    digit = -4;
    shift(0x0b, 0x04);
    shift(5,10);
  }else if(L<=-10000&&L>-100000){
    digit = -5;
    shift(0x0b, 0x06);
    shift(6,10);
  }
  if(digit>=1&&digit<=6){
    int j=0;
    for(int i=digit; i>=1; i--){
      int n = L/pow(10,digit-1-j);
      if(digit-j==3){
        shift(digit-j,n|0x80);
      }else{
        shift(digit-j,n);
      }
      L-=n*pow(10,digit-1-j);
      j++;
    }
  }
  else if(digit<=-1&&digit>=-5){
    digit*=-1;
    shift(digit+1,10);
    L*=-1;
    int j=0;
    for(int i=digit; i>=1; i--){
      int n = L/pow(10,digit-1-j);
      if(digit-j==3){
        shift(digit-j,n|0x80);
      }else{
        shift(digit-j,n);
      }
      L-=n*pow(10,digit-1-j);
      j++;
    }
  }
}


void segInt(long L){
  int digit = 1;//digit num
  int n1=0,n2=0,n3=0,n4=0,n5=0,n6=0;
  if(L>=100000&&L<1000000){
    digit = 6;
    shift(0x0b, 0x05);
  }else if(L>=10000&&L<100000){
    digit = 5;
    shift(0x0b, 0x04);
  }else if(L>=1000&&L<10000){
    digit = 4;
    shift(0x0b, 0x03);
  }else if(L>=100&&L<1000){
    digit = 3;
    shift(0x0b, 0x02);
  }else if(L>=10&&L<100){
    digit = 2;
    shift(0x0b, 0x01);
  }else if(L>=0&&L<10){
    digit = 1;
    shift(0x0b, 0x00);
  }else if(L<0&&L>-10){
    digit = -1;
    shift(0x0b, 0x01);
    shift(2,10);
  }else if(L<=-10&&L>-100){
    digit = -2;
    shift(0x0b, 0x02);
    shift(3,10);
  }else if(L<=-100&&L>-1000){
    digit = -3;
    shift(0x0b, 0x03);
    shift(4,10);
  }else if(L<=-1000&&L>-10000){
    digit = -4;
    shift(0x0b, 0x04);
    shift(5,10);
  }else if(L<=-10000&&L>-100000){
    digit = -5;
    shift(0x0b, 0x06);
    shift(6,10);
  }
  if(digit>=1&&digit<=6){
    int j=0;
    for(int i=digit; i>=1; i--){
      int n = L/pow(10,digit-1-j);
      shift(digit-j,n);
      L-=n*pow(10,digit-1-j);
      j++;
    }
  }
  else if(digit<=-1&&digit>=-5){
    digit*=-1;
    shift(digit+1,10);
    L*=-1;
    int j=0;
    for(int i=digit; i>=1; i--){
      int n = L/pow(10,digit-1-j);
      shift(digit-j,n);
      L-=n*pow(10,digit-1-j);
      j++;
    }
  }
  
}

void shift(byte adr, byte dt){
  digitalWrite(MAX_Load, LOW);
  shiftOut(MAX_Data, MAX_Clk, MSBFIRST, adr);
  shiftOut(MAX_Data, MAX_Clk, MSBFIRST, dt);
  digitalWrite(MAX_Load, HIGH);
}
