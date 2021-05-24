/*

*/

#include <math.h>

const int RED_LED_PIN = 11;
const int BLUE_LED_PIN = 10;
const int GREEN_LED_PIN = 9;

const int SENSOR_ANALOG_PIN = A0;

const int WAITTIME = 200;

int min = NULL;
int max = NULL;

// Define color structure for rgb
struct color {
  int r;
  int g;
  int b;
};
typedef struct color Color;

void setup() {
  Serial.begin(9600);
  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(GREEN_LED_PIN, OUTPUT);
  pinMode(BLUE_LED_PIN, OUTPUT);
}

void loop() {
  double percentageAnalogValue;
  int sensorAnalogValue = analogRead(SENSOR_ANALOG_PIN);
  
  if (min == NULL || sensorAnalogValue < min) {
    min = sensorAnalogValue;
    percentageAnalogValue = 0.0;
  } else if (max == NULL || sensorAnalogValue > max) {
    max = sensorAnalogValue;
    percentageAnalogValue = 1.0;
  } else {
    percentageAnalogValue = (double)(sensorAnalogValue - min) / (max - min);
  }
  
  Serial.println(percentageAnalogValue, DEC);
  
  Color color = transitionOfHueRange(percentageAnalogValue, 0, 360);
  
  analogWrite(RED_LED_PIN, color.r);
  analogWrite(GREEN_LED_PIN, color.g);
  analogWrite(BLUE_LED_PIN, color.b);
  
  delay(WAITTIME);
}

Color newColor(int r, int g, int b) {
  Color c;
  
  c.r = r;
  c.g = g;
  c.b = b;
  
  return c;
}

// Let compute a color using the HSL color space
// Inspired by https://stackoverflow.com/a/46929712
Color transitionOfHueRange(double percentage, int startHue, int endHue) {
  // From 'startHue' 'percentage'-many to 'endHue'
  // Finally map from [0°, 360°] -> [0, 1.0] by dividing
  double hue = ((percentage * (endHue - startHue)) + startHue) / 360;

  double saturation = 1.0;
  double lightness = 0.5;

  // Get the color
  return hslColorToRgb(hue, saturation, lightness);
}

Color hslColorToRgb(double hue, double saturation, double lightness) {
  if (saturation == 0.0) {
    // The color is achromatic (has no color)
    // Thus use its lightness for a grey-scale color
    int grey = percToColor(lightness);
    return newColor(grey, grey, grey);
  }

  double q;
  if (lightness < 0.5) {
    q = lightness * (1 + saturation);
  } else {
    q = lightness + saturation - lightness * saturation;
  }
  double p = 2 * lightness - q;

  double oneThird = 1.0 / 3;
  double red = percToColor(hueToRgb(p, q, hue + oneThird));
  double green = percToColor(hueToRgb(p, q, hue));
  double blue = percToColor(hueToRgb(p, q, hue - oneThird));

  return newColor(red, green, blue);
}

double hueToRgb(double p, double q, double t) {
  if (t < 0) {
    t += 1;
  }
  if (t > 1) {
    t -= 1;
  }

  if (t < 1.0 / 6) {
    return p + (q - p) * 6 * t;
  }
  if (t < 1.0 / 2) {
    return q;
  }
  if (t < 2.0 / 3) {
    return p + (q - p) * (2.0 / 3 - t) * 6;
  }
  return p;
}

int percToColor(double percentage) {
  return round(percentage * 255);
}
