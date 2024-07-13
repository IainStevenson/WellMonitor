/*
Derived from the LiquidCrystal Library - Hello World
 https://docs.arduino.cc/learn/electronics/lcd-displays

  The LCD circuit:
 * LCD RS pin to digital pin 12
 * LCD Enable pin to digital pin 11
 * LCD D4 pin to digital pin 5
 * LCD D5 pin to digital pin 4
 * LCD D6 pin to digital pin 3
 * LCD D7 pin to digital pin 2
 * LCD R/W pin to ground
 * LCD VSS pin to ground
 * LCD VCC pin to 5V
 * 10K resistor:
 * ends to +5V and ground
 * wiper to LCD VO pin (pin 3)

A HC-SR04 ultrasonic sensor is placed on pins 6 & 7


*/

// include the library code:
#include <Arduino.h>
#include <LiquidCrystal.h>
#include "KickSort.h"

// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
const int TRIGGER_PIN = 6;
const int ECHO_PIN = 7;
const int MAX_DEPTH_CM = 300;
const int MEASURE_INTERVAL = 10000; // 10 seconds
const float SPEED_OF_SOUND_CM_US = 0.0343;
const int numMeasurements = 10; // Number of measurements within 1 second
float distances[numMeasurements];
void initSensor();
float getDistance();
float getStdDev(float arr[], int size);
float getMedian(float arr[], int size);
void debugResults(float distances[], float median, float deviation);

void setup()
{
  initSensor();
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  // Print a message to the LCD.
  Serial.begin(115200);
}
float medianDistance = 0;

float stdDevDistance = 1;
void loop()
{
  lcd.blink();
  stdDevDistance = 2;
  // keep mearuring until we get a stable result.
  while (stdDevDistance > 1)
  {
    for (int i = 0; i < numMeasurements; i++)
    {
      distances[i] = getDistance();
      delay(100); // Delay 100ms between measurements to span 1 second (10 * 100ms = 1000ms = 1 second)
    }

    medianDistance = getMedian(distances, numMeasurements);
    stdDevDistance = getStdDev(distances, numMeasurements);
  }

  debugResults(distances, medianDistance, stdDevDistance);

  float depth = MAX_DEPTH_CM - medianDistance;

  lcd.setCursor(0, 0);
  lcd.print("Dist:");
  lcd.print(medianDistance);

  lcd.setCursor(0, 1);
  lcd.print("SDev:");
  lcd.print(stdDevDistance);
  lcd.noBlink();
  delay(10000);
}

void debugResults(float distances[], float median, float deviation)
{
  Serial.print("Results:");

  for (int i = 0; i < numMeasurements; i++)
  {
    Serial.print(" ");
    Serial.println(distances[i]);
  }

  Serial.print("Median: ");
  Serial.println(median);
  Serial.print("Std Deviation: ");
  Serial.println(deviation);
  Serial.println("");
}

void initSensor()
{
  pinMode(TRIGGER_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
}

float getDistance()
{
  digitalWrite(TRIGGER_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIGGER_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIGGER_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH);
  float distanceCm = duration * SPEED_OF_SOUND_CM_US / 2.0;
  return distanceCm;
}

float getMedian(float arr[], int size)
{
  // Sort the array
  KickSort<float>::quickSort(arr, numMeasurements);

  // Calculate the median
  if (size % 2 == 0)
  {
    return (arr[size / 2 - 1] + arr[size / 2]) / 2;
  }
  else
  {
    return arr[size / 2];
  }
}
float getStdDev(float arr[], int size)
{
  // First, calculate the mean
  float mean = 0;
  for (int i = 0; i < size; i++)
  {
    mean += arr[i];
  }
  mean /= size;

  // Then, calculate the sum of squared differences from the mean
  float sumSqDiff = 0;
  for (int i = 0; i < size; i++)
  {
    sumSqDiff += pow(arr[i] - mean, 2);
  }

  // Calculate the standard deviation
  float stdDev = sqrt(sumSqDiff / size);

  return stdDev;
}