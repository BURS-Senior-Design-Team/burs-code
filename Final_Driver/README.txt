Contents
  •	What this does
  •	Installation
  •	Dependencies
  •	Usage

What this does:

  This project is intended to be used with the B.U.R.S.: Balloon Ultraviolet Radiation Sensor. B.U.R.S. is a sensor package which is to be flown on a high altitude balloon, measure, and store UV and altitude data. The hardware this program utilizes is an Arduino Uno, LTR390 UV-A sensor, Mikroe UVB Click, ADS1115 ADC, Adafruit Micro SD, DS3231 RTC, and DPS310 altimeter. The FinalDriver.ino contains a state machine which controls the sample frequency and data saving rate based on the altimeter’s reading. The states include ARM, GROUND, CLIMB, FLOAT, DESCENT, CHECK, and OFF.

	  ARM
    •	Designed for I2C setup for each sensor and the sd writer
    •	Lights an led when I2C is completed and connection is established
    •	Saves the ground level altitude when the altimeter connects correctly
    •	Moves to the GROUND state when all sensors are connected and a micro SD is inserted
        o	Begins a timer for the leds to power off at the state change
        o	Saves the real time and date to a .txt file at the state change
    Ground
    •	First state where sampling and data saving occurs
    •	Sample/saving rate set to 1hz
    •	No heater control included
    •	LED’s will turn off after 30 seconds of being in this state
    •	Heartbeat LED blinks at the rate of sample
    •	Will move to the CLIMB state when the balloon has reached 30 meters above ground level
        o	Turns off the heartbeat LED upon state change
        o	Saves current time for reference at the end of flight upon state change
    CLIMB
    •	Sample/saving rate set to 4hz
    •	Controls heater
        o	If below 0 degrees C digital pin 9 is used to operate a relay which turns on the heater
    •	Will move to the FLOAT state when the balloon has reached 26 km
        o	Altitude not referenced to ground level altitude
    FLOAT
    •	Sample/saving rate set to 1hz
    •	Controls heater
        o	If below 0 degrees C digital pin 9 is used to operate a relay which turns on the heater
    •	Will move to the CHECK state when altitude falls below 25km
        o	Altitude not referenced to ground level altitude
        o	Starts a timer upon state change
    CHECK
    •	Sampling and saving are halted during this state
    •	Heater control halted during this state
    •	Checks altitude
        o	Will remain in CHECK if altitude is below 25km and the 5 second timer has not completed
        o	Will move back to FLOAT if altitude goes above 25km
        o	Will move to DESCENT if altitude is below 25km and the 5 second timer has completed
    DESCENT
    •	Sample/saving rate set to 16hz
    •	Controls heater
        o	If below 0 degrees C digital pin 9 is used to operate a relay which turns on the heater
    •	Moves to OFF state when altitude is 1km above the ground
        o	Altitude *IS* referenced to ground altitude for this change
    OFF
    •	No sampling or saving occurs in the off state
    •	Checks time referenced to time of flight and the saved flight start time saved upon the change from GROUND to CLIMB
        o	Stays OFF if overall flight time is greater than the expected flight time
        o	Goes back to DESCENT if the overall flight time is less than the expected flight time

Installation

•	All necessary files to run this program are included in the zip folder labeled final driver
•	Download final driver zip folder and use as necessary

Dependencies

•	SPI.h and SD.h are the only 2 libraries not covered by the .h and .cpp files included in the zip file
•	You will be guided to download these libraries if not already installed

Usage

	Preflight Software Adjustments
    1.	Adjust the seaLevelhPa value to reflect the flight locations closest sea level pressure in Hectopascals
          o	This will increase the accuracy of the altimeter readings
    2.	Adjust the projected_flight_time value to reflect the expected time of flight from take off to landing, not including time on the ground
          o	Set to 10 hours currently

	Flight Setup
    1.	Only use a 32GB or less micro-SD
    2.	Ensure previous files are deleted from the micro-SD
    3.	Remove front face of the CubeSat
          o	This is the face with 3 holes
          o	2.5mm Allen wrench is required
    4.	Connect to the Arduino using the USB-B connection and upload the modified program
    5.	Disconnect USB-B cable
    6.	Insert micro-SD card into the reader
          o	Located at the bottom of the CubeSat directly below the Arduino
    7.	Clean the lenses of the 3 UV sensors using alcohol
          o	The 3 sensors are located at the top of the CubeSat
              *	LTR390(UV-A sensor)
              *	UVB Click(UV-B Sensor)
              *	Purple PCB board (UV-C sensor)
    8.	Reinstall front face of the CubeSat
          o	Sensor viewports referenced to the top screw being inserted first
    9.	Connect battery to the cable located at the top of the CubeSat
  
  LED Configuration
    The numbered list below indicates the number from the top of the CubeSat for the 8 openings on the side panel. Note that the 4th location, as noted below, is not an LED and is the physical external thermistor.
    
    1.	Top Green LED: Heartbeat
          •	Will only blink when all sensors are operating correctly, and memory card is inserted
    2.	Yellow LED: SD reader/writer connection
          •	Only lights when a micro-SD is inserted
    3.	RTC
    4.	External Thermistor
          •	This is not an LED it is the location for the external thermistor
    5.	ADC (ADS1115)
          •	This allows I2C communication with the UV-C Sensor and the External Thermistor
    6.	UV-A sensor (LTR-390)
    7.	UV-B sensor (UVB Click)
    8.	Altimeter
