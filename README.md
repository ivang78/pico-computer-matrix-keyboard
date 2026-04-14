# Raspberry Pi Zero 2W computer with matrix keyboard
This is old style keyboard-based computer with Raspberry Pi Zero 2W suitable for retro-gaming and other purposes.
Uses hand-wired matrix keyboard 14x5 with Cherry MX type switches. Keyboard has user space GPIO driver.
For video output both composite or  HDMI might be used.
For simple audio output piezo speaker directly attached to PWM GPIO pin. For other speaker types you need audio filter and amplifier. 
Computer fits into 3D printed case 

Built computer with monitor attached 
![Schematic_Pico-Computer-Image](img/Image.jpg?raw=true)

Hand-wired keyboard
![Hand-Wired-Keyboard.jpg](img/Hand-Wired-Keyboard.jpg?raw=true)

# Schematics
![Schematic_Pico-Computer-with-Matrix-Keyboard](img/Schematic_Pico-Computer-with-Matrix-Keyboard.png?raw=true)

# Raspberry Pi GPIO Matrix Keyboard Driver
Light adaptation of [Raspberry Pi GPIO Matrix Keyboard Driver](https://github.com/nutki/rpi-gpio-matrix-keyboard). 
Adaptation implements FN key function to use 1-0 keys as F1-F10 and arrow keys as Page Up, Page Down, Home and End keys.

## Driver compilation
Compile driver directly on Raspberry Pi with Raspbian OS. Tested with Raspbian Bullseye version.
```
cd driver
cmake .
make
```
## Driver installation
To install the keyboard driver as a systemd serice
```
sudo cmake install
```

## 3D Printed case
STL files located at stl/ folder. Case based on [this-3d-model](https://makerworld.com/ru/models/557186-60-keyboard-with-arrow-keys?from=search#profileId-2597653).
Right part of base has all neccessary holes and stands for mount Raspberry Pi Zero 2W and RCA socker for composite video output.
All other base parts unmodified. 
