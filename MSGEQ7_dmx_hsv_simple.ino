/*
63Hz --> bass range
160Hz --> bass range
400Hz --> instrument / vocals
1kHz --> instrument / vocals
2.5kHz
6.25kHz
16kHZ
*/

// DmxSimple - Version: Latest 
#include <DmxSimple.h>
// FastLED - Version: Latest 
#include <FastLED.h>

#include <math.h>

#define STROBE 4
#define RESET 5
#define DC_One A0
#define DC_Two A1

#define NUM_LEDS 4
#define DMX_FIXTURE_LEN 4

const int DMX_PIN = 3;

const int SENSOR_ANALOG_PIN = A0;

const int WAITTIME = 15;

//Define spectrum variables
int freq_amp;
int Frequencies_One[7];
int Frequencies_Two[7];
int lastHue = -255;

int min = 1023;
int max = 0;

void setup() {
  //Set spectrum Shield pin configurations
  pinMode(STROBE, OUTPUT);
  pinMode(RESET, OUTPUT);
  pinMode(DC_One, INPUT);
  pinMode(DC_Two, INPUT);

  //Initialize Spectrum Analyzers
  digitalWrite(STROBE, LOW);
  digitalWrite(RESET, LOW);
  delay(5);
  
  Serial.begin(9600);
  DmxSimple.usePin(DMX_PIN);
  DmxSimple.maxChannel(NUM_LEDS * DMX_FIXTURE_LEN);
}

void loop() {
  Read_Frequencies();
  double percentageAnalogValue;
  int bassAnalogValue = (((Frequencies_One[0] + Frequencies_Two[0]) / 2) + ((Frequencies_One[1] + Frequencies_Two[1]) / 2) / 2);
  
  if (bassAnalogValue < min) {
    min = bassAnalogValue;
    percentageAnalogValue = 0.0;
  } else if (bassAnalogValue > max) {
    max = bassAnalogValue;
    percentageAnalogValue = 1.0;
  } else {
    percentageAnalogValue = (double)(bassAnalogValue - min) / (max - min);
  }
  
  Serial.println(percentageAnalogValue, DEC);
  
  int hue = percToColor(percentageAnalogValue);
  if (abs(lastHue - hue) >= 10) {
    lastHue = hue;
    CHSV hsv(hue, 255, 255);
    CRGB rgb;
    hsv2rgb_rainbow(hsv, rgb);
  
    for(int i = 1; i <= NUM_LEDS; i++) {
      showLED(i, rgb);
    }
  }
  
  // Graph_Frequencies();
  delay(WAITTIME);
}

void showLED(int address, CRGB color) {
  // 4 channels because RGBW
  int start = DMX_FIXTURE_LEN * (address - 1) + 1;
  DmxSimple.write(start++, color.r);
  DmxSimple.write(start++, color.g);
  DmxSimple.write(start++, color.b);
  DmxSimple.write(start++, 0);
}

int percToColor(double percentage) {
  return round(percentage * 255);
}

/*************Pull frquencies from Spectrum Shield****************/
void Read_Frequencies() {
  digitalWrite(RESET, HIGH);
  delayMicroseconds(200);
  digitalWrite(RESET, LOW);
  delayMicroseconds(200);

  //Read frequencies for each band
  for (freq_amp = 0; freq_amp < 7; freq_amp++)
  {
    digitalWrite(STROBE, HIGH);
    delayMicroseconds(50);
    digitalWrite(STROBE, LOW);
    delayMicroseconds(50);

    Frequencies_One[freq_amp] = analogRead(DC_One);
    Frequencies_Two[freq_amp] = analogRead(DC_Two);
  }
}

/*****************Print Out Band Values for Serial Plotter*****************/
void Graph_Frequencies() {
  for (int i = 0; i < 7; i++)
  {
    Serial.print(Frequencies_One[i]);
    Serial.print(" ");
    Serial.print(Frequencies_Two[i]);
    Serial.print(" ");
    // Serial.print( (Frequencies_One[i] + Frequencies_Two[i]) / 2 );
    // Serial.print("    ");
  }
  Serial.println();
}