# OutPI
OutPI utilizes BeamNG's OutGauge functionality to take Gauge info from the game and sending it out the Raspberry PI's GPIO.

Currently this project is in its experemental stage, not all functionality has been implemented yet. As of 11/10/24 only the dash lights are being displayed.

This project utilizes WiringPi which is available at: https://github.com/WiringPi/WiringPi

Installing WiringPi:

-
        sudo apt-get update
-
       git clone https://github.com/WiringPi/WiringPi
-  
       cd WiringPi
-  
         ./build


In my personal gauge cluster I have removed the older Incandscent lights and swapped them with LED's due to the PI's limited output.
I highly recommend you also do this since LEDs use up to 90% less energy than incandescent bulbs.
