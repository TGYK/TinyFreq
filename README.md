# TinyFreq
An ATTiny85-based frequency multiplier for use in 1984-1985 Pontiac Fieros for digital cruise control conversion. May be applicable in other use cases. The PCB is designed to be plugged into the harness at C246 where the old cruise control servo control module plugged in, and jumpers relevant stalk signals to back of vehicle via old servo control wires. See table below for details.
|Function|Vacuum Cruise Connector|
|-:|:-|
|Set/Coast|A|
|Multiplied VSS|B|
|GND|C|
|Resume/Acc|D|
|Cruise Enable|E|

## Building
This project is designed to use Platformio and VSCode. Detailed build instructions are to come; Make sure to burn fuses and bootloader before uploading the project to the chip.

#THIS PROJECT WAS PROUDLY MADE WITHOUT THE USE OF "AI" OR LLM CHATBOTS.
