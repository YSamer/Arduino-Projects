#include "HX711.h"

const int DATA = A0;
const int CLK = A1;
HX711 scale;
double W = 0;
long x0=0;
double W0=0;
void setup() {
  
  Serial.begin(57600);
  scale.begin(DATA, CLK);
  delay(600);
  Serial.println("Starting");
  delay(1000);
  x0 = scale.read();
  W0 = (-0.10314*x0)-14537.77;
}

void loop() {
  delay(1000);
  if (scale.is_ready()) {
    long x = scale.read();
    W = (-0.10314*x)-14537.77-W0;
    Serial.print(x);
    Serial.print("   >>  ");
    Serial.print(W/1000);
    Serial.println(" Kg");
  } else {
    Serial.println("ERROR");
  }
}
