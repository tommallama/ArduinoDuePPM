# ArduinoDuePPM
Arduino Due code for generating a high accuracy Pulse Position Modulation (PPM) signal. 
A PPM signal is used to encode multiple signals into a pulsed waveform. 
Great Explanation: https://skymixer.net/electronics/84-rc-receivers/78-rc-ppm-signal or just Google "RC PPM encoding"

## Understanding the Code
The code is a basic example that does all the heavy lifting. If you are interested to know what each bit of code does read the extensive comments. Additionally the comments surrounded in double asterisks reference a section of the following datasheet:
http://www.atmel.com/Images/Atmel-11057-32-bit-Cortex-M3-Microcontroller-SAM3X-SAM3A_Datasheet.pdf

##Uses
This code was used to "highjack" the trainer port of a Taranis X9D Plus transmitter using a logic level shifter (3.3V to 5V).
