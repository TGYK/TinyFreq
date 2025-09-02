# TinyFreq
An ATTiny85-based frequency multiplier for use in 1984-1985 Pontiac Fieros for digital cruise control conversion. May be applicable in other use cases. The PCB is designed to be plugged into the harness at C246 where the old cruise control servo control module plugged in, and jumpers relevant stalk signals to back of vehicle via old servo control wires. See table below for details.
|Function|Vacuum Cruise Connector|
|-:|:-|
|Set/Coast|A|
|Multiplied VSS|B|
|GND|C|
|Resume/Acc|D|
|Cruise Enable|E|

## Bill of Materials

|Reference|Device|Value|
|--------:|:----:|:----|
|R1|Resistor|100Ω|
|R2|Resistor|10kΩ|
|R3|Resistor|10kΩ|
|R4|Resistor|10kΩ|
|C1|Capacitor|N/A|
|C2|Capacitor|0.33㎌|
|C3|Capacitor|0.1u㎌|
|D1|Diode|1N4001|
|D2|TVS Diode|SA16A|
|D3|TVS Diode|SA30A|
|D4|TVS Diode|SA30A|
|Q1|N-Channel Enhancement FET|2N7000|
|Q2|N-Channel Enhancement FET|2N7000|
|U1|Linear Voltage Regulator|L78L05|
|U2|Microcontroller|ATTiny85|
|J2|Pin Header|2x3, 2.54mm|

## Uploading
### Using an Arduino Uno/Mega as a a programmer.
**THESE STEPS ARE CURRENTLY UNTESTED - FOLLOW AT YOUR OWN RISK**
1. [Setup your Arduino as a programmer and wire to the ISP connection](https://docs.arduino.cc/built-in-examples/arduino-isp/ArduinoISP/), but do not attempt to burn a bootloader to the target.
2. Open the PlatformIO project in VSCode and verify the project builds by clicking the checkmark on the toolbar at the bottom left of the window.
3. Modify the line ```upload_port = SERIAL_PORT_HERE``` within platformio.ini to reflect the serial port that your arduino-as-programmer is connected to, and save the file.
4. If using an arduino mega, modify the line ```${platformio.packages_dir}/tool-avrdude/avrdude.conf``` within platformio.ini to be ```${platformio.packages_dir}/tool-avrdude-megaavr/avrdude.conf``` and save the file.
4. Select the "program_via_ArduinoISP" environment on the bottom toolbar
5. Click the "Upload" arrow on the bottom toolbar


# THIS PROJECT WAS PROUDLY MADE WITHOUT THE USE OF "AI" OR LLM CHATBOTS.
