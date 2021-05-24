/*

*/

const int OPTO_PIN = 2;

const int SENSOR_ANALOG_PIN = A0;

const int WAITTIME = 20000;

int min = NULL;
int max = NULL;

void setup() {
  Serial.begin(9600);
  pinMode(OPTO_PIN, OUTPUT);
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
  
  digitalWrite(OPTO_PIN, HIGH);
  delay(100); // 15 in the tutorial
  digitalWrite(OPTO_PIN, LOW);
  
  delay(WAITTIME);
}