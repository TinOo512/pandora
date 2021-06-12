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

#define NUM_LEDS 1
#define DMX_FIXTURE_LEN 3

const int DMX_PIN = 3;

const int SENSOR_ANALOG_PIN = A0;

const int WAITTIME = 50;

//Define spectrum variables
int freq_amp;
int lastFrequencies[7][10] = {
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
};

int min = 1023;
int max = 0;

CRGB leds[NUM_LEDS];

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
  LEDS.addLeds<DMXSIMPLE, DMX_PIN, RGB>(leds, NUM_LEDS);
  DmxSimple.maxChannel(NUM_LEDS * DMX_FIXTURE_LEN);
  leds[0] = CHSV(0, 255, 255);
  FastLED.show();
}

void loop() {
  Read_Frequencies();
  double percentageAnalogValue;
  int maxLatestFrequencyAnalogValue = Choose_Frequency();
  
  if (maxLatestFrequencyAnalogValue < min) {
    min = maxLatestFrequencyAnalogValue;
    percentageAnalogValue = 0.0;
  } else if (maxLatestFrequencyAnalogValue > max) {
    max = maxLatestFrequencyAnalogValue;
    percentageAnalogValue = 1.0;
  } else {
    percentageAnalogValue = (double)(maxLatestFrequencyAnalogValue - min) / (max - min);
  }
  
  Serial.println(percentageAnalogValue, DEC);
  
  FastLED.setBrightness(percToColor(percentageAnalogValue));

  FastLED.delay(WAITTIME);
  
  // Graph_Frequencies();
}

int percToColor(double percentage) {
  return round(percentage * 255);
}

int getRange(int arr[]) {
  int min = arr[0];
  int max = arr[0];
  
  for(int i = 1; i < sizeof(arr); i++) {
    if(arr[i] < min) {
      min = arr[i];
    } else if(arr[i] > max) {
      max = arr[i];
    }
  }
  
  return max - min;
}

int Choose_Frequency() {
  int maxLatestFrequency = lastFrequencies[0][0];
  int maxRangeFrequency = getRange(lastFrequencies[0]);
  
  for (int i = 1; i < 7; i++) {
    int rangeFrequency = getRange(lastFrequencies[i]);
    if (rangeFrequency > maxRangeFrequency) {
      maxLatestFrequency = lastFrequencies[i][0];
    }
  }
  
  return maxLatestFrequency;
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

    for(int j = 10; j >= 1; j--) {
      lastFrequencies[freq_amp][j] = lastFrequencies[freq_amp][j - 1];
    }
    lastFrequencies[freq_amp][0] = (analogRead(DC_One) + analogRead(DC_Two)) / 2;
  }
}

/*****************Print Out Band Values for Serial Plotter*****************/
void Graph_Frequencies() {
  for (int i = 0; i < 7; i++)
  {
    // Serial.print(Frequencies_One[i]);
    // Serial.print(" ");
    // Serial.print(Frequencies_Two[i]);
    // Serial.print(" ");
    // Serial.print( (Frequencies_One[i] + Frequencies_Two[i]) / 2 );
    // Serial.print("    ");
  }
  Serial.println();
}