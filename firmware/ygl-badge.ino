// Base Code for YGL Badge
// Author: Dan Steingart
// Date Started: 2018-03-25
// Date Last Modified: 2018-07-02

/*


This code
  - pings an attached DHT22 sensor for T/RH,
  - pings an attached MLX90614 sensor for focussed and ambient temperature
  - can detect whether or not it is in the badge and change sampling rate as a function thereof (for hacking)
  - pings the powersield for battery voltage and SOC
  - allows for comfort signaling via three buttons
  - samples for A4 for voltage divider demo
  - send JSON formatted code is sent via the YGL event

  ** as of 2018-06-08 there is noise in the A4 signal on a breadboard, likely due to a lack of pull up resistors.
  *** Because we are awesome we just use a rolling_mean on the data set to pretend it's not there.

*/

PRODUCT_ID(7743);
PRODUCT_VERSION(1);


#include "DHT.h"
#define DHTPINA A0  // Digital pin D2
#define DHTTYPE DHT22
#include "math.h"

#include "Adafruit_MLX90614.h"
#include "PowerShield.h"


DHT dhta(DHTPINA, DHTTYPE);

double calibration_factor = 0.64;
// USER INPUT ** END **

double h1;    // humidity
double t1;    // temperature c
double tb;
double ta;

Adafruit_MLX90614 mlx = Adafruit_MLX90614();
PowerShield batteryMonitor;


char publishString[200]; //a place holer for the publish string
char triggerString[200]; //a place holer for the publish string

int waiter = 10; //in ms
int samps = 100; //ticks before timed send (waiter*samps = ms )
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
  pinMode(A6, INPUT);

  pinMode(D2, OUTPUT); //led
  pinMode(D3, OUTPUT); //led
  pinMode(D7, OUTPUT); //led


  digitalWrite(D2,LOW);
  digitalWrite(D3,HIGH);

  dhta.begin();

  mlx.begin();
  batteryMonitor.begin();
  RGB.control(true);
  // RGB.color(255,255,255);
  RGB.color(0,0,0);
  RGB.brightness(0);


}

float calibrated_value(float input, float factor) {
    return input + factor;
}

void loop() //repeat this loop forever
{

    //badge ADCS
    int an1 = analogRead(3); //Red button
    int an2 = analogRead(1); //Blue button
    int an0 = analogRead(2); //Green button
    int an6 = analogRead(6); //badge detect if shorted to gnd.

    //detect badge yes/no
    bool badge = false;
    if (an6 < 20) badge = true;
    else badge = false;

    //if we're in the badge, send less to power save
    if (badge) samps = 500; // 500 to avoid DHT heating
    else samps = 100;

    if ((an0 < 10  || an1 < 10 || an2 < 10 ) && millis()-push_counter > push_waiter)
    {

            push_counter = millis();
            Particle.publish("YGL",triggerString);
            RGB.control(true);

            h1 = dhta.readHumidity();
            t1 = dhta.readTemperature(false);
            t1 = calibrated_value(t1, calibration_factor);

            double amb = mlx.readAmbientTempC();
            double finger = mlx.readObjectTempC();

            float a4 = 0;

            wait_counter = 0;


            int state = 42;

            if  (an0 < 10)
            {
                RGB.color(0,  255,  0);
                state = 1;
                //sprintf(triggerString,"{\"type\":\"trigger\",\"state\":\"good\"}");

            }
            else if (an1 < 10)
            {
                RGB.color(255,   0,  0);
                //sprintf(triggerString,"{\"type\":\"trigger\",\"state\":\"hot\"}");
                state = 0;
            }
            else if (an2 < 10)
            {
                RGB.color(   0, 0,  255);
                //sprintf(triggerString,"{\"type\":\"trigger\",\"state\":\"cold\"}");
                state = 2;
            }

            sprintf(publishString,"{\"type\":\"trigger\",\"T_DHT22\": %f,\"H_DHT22\": %f,\"T_amb_MLX\": %f,\"T_sen_MLX\": %f,\"a4\":%f,\"state\":%d}",t1,h1,amb,finger,a4,state);
            Particle.publish("YGL",publishString);
            RGB.brightness(255);
            delay(500);
            // RGB.color(255,255,255);
            // RGB.brightness(255);
            //RGB.color(0,0,0);
            RGB.brightness(10);

            //RGB.control(false);
    }

    delay(waiter);

    wait_counter += 1;
    if (wait_counter > samps)
    {
        h1 = 0;
        t1 = 0;
        double amb = 0;
        double finger = 0;

        if (badge)
        {
          h1 = dhta.readHumidity();
          t1 = dhta.readTemperature(false);
          t1 = calibrated_value(t1, calibration_factor);
          amb = mlx.readAmbientTempC();
          finger = mlx.readObjectTempC();
        }

        float a4 = 0;
        int lim = 1;
        for (int i = 0; i < lim;i++) a4 = a4 + analogRead(A4);
        a4 = a4/lim;

        wait_counter = 0;
        sprintf(publishString,"{\"type\":\"timer\",\"T_DHT22\": %f,\"H_DHT22\": %f,\"T_amb_MLX\": %f,\"T_sen_MLX\": %f,\"a4\":%f}",t1,h1,amb,finger,a4);
        Particle.publish("YGL",publishString);
        digitalWrite(7,1);
        delay(50);
        digitalWrite(7,0);


    }

}
