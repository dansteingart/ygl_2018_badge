## Overview

This is the sensor and code layout for the YGL 2018 Badge. 

The badge measures
- Ambient Temperature (via a DHT22 and a MLX90614)
- Focussed Temperature (via a MLX90614)
- Ambient Humidity (via a DHT22)

The badge also allows the wearer to indicate comfort level
- Red (hot)
- Blue (cold)
- Green (comfortable)

The badge attempts to connect to preprogammed wifi SSID's, and if it is connect it reports the ambient conditions once a minute and the user comfort on signaling.

## Board Schematics

The board can be edited in [Eagle](https://www.autodesk.com/products/eagle/free-download) 

## Code

The code for this is meant to be uploaded through the [build.particle.io](https://build.particle.io), but can also be compiled and sent remotely.

## How To Measure Things
This [document](https://docs.google.com/document/d/e/2PACX-1vTs8eymRHai5XaTd2wvhhjKwmNey0X5QBFbXBKoaI53rYneKwUvtwl-gOKRnENuz1WqiknrLIXWhSsD/pub)([PDF](https://docs.google.com/document/d/11KgSaUlBZMfQc9rQ66gh0Wr3Of6fnvB99Y8Q6VOIqZM/export?format=pdf)) describes how and why this all works, and what we're trying to do.
