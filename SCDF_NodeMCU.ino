#define BLYNK_PRINT Serial

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <HP20x_dev.h> //flow sensor hardware library
#include <Arduino.h> 
#include <Wire.h>

unsigned char ret = 0;

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "410e39b34914406dbd0e6470ef4165eb"; //allows connection between hardware and Blynk Cloud

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "your wifi network";
char pass[] = "password";

unsigned volatile int flow_frequency; // Measures flow sensor pulses
unsigned int l_hour; // Calculated litres/hour
char flowSensor = 2; // Sensor Input
void flow () // Interrupt function
{
   flow_frequency++;
}

void setup(){
 Serial.begin(9600);  // serial data rate is set for 9600 bpa (bits per second) 
 Blynk.begin(auth, ssid, pass, "blynk-cloud.com", 80);
 pinMode(flowSensor, INPUT);
 attachInterrupt(flowSensor, flow, RISING); // Setup Interrupt
 HP20x.begin();
 delay(100);
      /* Determine HP20x_dev is available or not*/
 ret = HP20x.isAvailable();
 if(OK_HP20X_DEV == ret){
        Serial.println("HP20x_dev is available.\n");
    }
 else{
        Serial.println("HP20x_dev isn't available.\n");
    }
}

BLYNK_READ(V5) //Blynk app has something on V5
{
 sei(); // Enable interrupts
 delay (1000);      //Wait 1 second
 cli();            //Disable interrupts
 // Pulse frequency (Hz) = 7.5Q, Q is flow rate in L/min.
 l_hour = (flow_frequency / 7.5); // (Pulse frequency) / 7.5Q = flowrate in L/min
 Blynk.virtualWrite(V5, flow_frequency/7.5); //sending to Blynk
 flow_frequency = 0; // Reset Counter 
}

BLYNK_READ(V6){
    char display[40];
    if(OK_HP20X_DEV == ret)
    {
        Serial.println("------------------\n");
        long Temperature = HP20x.ReadTemperature(); //This is where the Temperature value is obtained
        Serial.println("Temperature:");
        Temperature = Temperature/100.0;
        Blynk.virtualWrite(V6, Temperature);
    }
}

BLYNK_READ(V7){
        long Pressure = HP20x.ReadPressure(); //This is where the Pressure value is obtained
        Serial.println("Pressure:");
        Pressure = Pressure/100.0;
        Blynk.virtualWrite(V7, Pressure);
}

void loop(){
  Blynk.run();
}
