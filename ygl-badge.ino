//Simple Base Code for YGL Badge
//This code pings an attached 1 one DHT sensor for T/RH, and allows for comfort signaling via three buttons
//JSON formatted code is sent via the YGL event

// This #include statement was automatically added by the Particle IDE.
#include "DHT.h"
#define DHTPINA A0  // Digital pin D2
#define DHTTYPE DHT22
#include "math.h"

DHT dhta(DHTPINA, DHTTYPE);

double calibration_factor = 0.64;
// USER INPUT ** END **

double h1;    // humidity
double t1;    // temperature c
double tb;
double ta;


char publishString[200]; //a place holer for the publish string
char triggerString[200]; //a place holer for the publish string

int waiter = 1000; //in ms
int samps = 1000; //sampler counter for smooth smoothness
int push_waiter = 2000;
long push_counter = 0;
long wait_counter = 0;
void setup() //run this loop just once upon start, or upon reset
{
  //This will send back the big data string
  Particle.variable("YGL",triggerString);
  //We'll use these to send the digital state

  //set analog pins to input mode
  pinMode(A3, INPUT);
  pinMode(A1, INPUT);
  pinMode(A2, INPUT);
  pinMode(A4, INPUT);

  pinMode(D7, OUTPUT); //led

  dhta.begin();
  RGB.control(true);
  RGB.color(0,0,0); //save energy yo

}

float calibrated_value(float input, float factor) {
    return input + factor;
}

void loop() //repeat this loop forever
{

    int an1 = analogRead(3);
    int an2 = analogRead(1);
    int an0 = analogRead(2);

    if ((an0 < 10  || an1 < 10 || an2 < 10 ) && millis()-push_counter > push_waiter)
    {
        
            push_counter = millis();
            Particle.publish("YGL",triggerString);
            RGB.control(true);

            String state = "";

            if      (an0 < 10)
            {
                RGB.color(255,  0,  0);
                sprintf(triggerString,"{\"type\":\"trigger\",\"state\":\"hot\"}");

            }
            else if (an1 < 10)
            {
                RGB.color(0,   0,  255);
                sprintf(triggerString,"{\"type\":\"trigger\",\"state\":\"cold\"}");

            }
            else if (an2 < 10)
            {
                RGB.color(   0,255,  0);
                sprintf(triggerString,"{\"type\":\"trigger\",\"state\":\"good\"}");

            }
            
            Particle.publish("YGL",triggerString);
            RGB.brightness(255);
            delay(500);
            RGB.brightness(0);

            //RGB.control(false);
    }

    delay(10);
    
    wait_counter += 1;
    if (wait_counter > samps)
    {
        h1 = dhta.readHumidity();
        // Read Temperature and correct it
        t1 = dhta.readTemperature(false);
        // tb = t1;
        t1 = calibrated_value(t1, calibration_factor);

        int l = analogRead(A4);

        wait_counter = 0;
        sprintf(publishString,"{\"type\":\"timer\",\"T\": %f,\"H\": %f}",t1,h1);
        Particle.publish("YGL",publishString);
        digitalWrite(7,1);
        delay(50);
        digitalWrite(7,0);

        
    }

}
