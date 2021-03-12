#define data 5
#define rclk 6
#define sclk 7
#define d1   8
#define d2   9
#define d3   10


const byte X[16] =
{
  0b11111100,  // 0
  0b01100000,  // 1
  0b11011010,  // 2
  0b11110010,  // 3
  0b01100110,  // 4
  0b10110110,  // 5
  0b10111110,  // 6
  0b11100000,  // 7
  0b11111110,  // 8
  0b11110110,  // 9
  0b01101111,  // A
  0b00111110,  // b
  0b10011100,  // C
  0b01111010,  // d
  0b10011110,  // E
  0b10001110   // F
};
void setup() {
  pinMode(data, OUTPUT);
  pinMode(rclk, OUTPUT);
  pinMode(sclk, OUTPUT);
  pinMode(d1, OUTPUT);
  pinMode(d2, OUTPUT);
  pinMode(d3, OUTPUT);  

  digitalWrite(d1,HIGH);
  digitalWrite(d2,HIGH);
  digitalWrite(d3,HIGH);
  delay(100);
}

void loop() {
  updateShiftReg();
}

void updateShiftReg(){
  
  digitalWrite(rclk, LOW);
  shiftOut(data, sclk, LSBFIRST, X[1]);
  digitalWrite(rclk, HIGH);
  digitalWrite(d1,LOW);
  delayMicroseconds(1500);
  digitalWrite(d1,HIGH);

  digitalWrite(rclk, LOW);
  shiftOut(data, sclk, LSBFIRST, X[2]);
  digitalWrite(rclk, HIGH);
  digitalWrite(d2,LOW);
  delayMicroseconds(1500);
  digitalWrite(d2,HIGH);

  digitalWrite(rclk, LOW);
  shiftOut(data, sclk, LSBFIRST, X[3]);
  digitalWrite(rclk, HIGH);
  digitalWrite(d3,LOW);
  delayMicroseconds(1500);
  digitalWrite(d3,HIGH);
}

