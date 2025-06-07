# NAMCO NeGcon to Nintendo Switch Converter  

This is a PlayStation NAMCO NeGcon to Nintendo Switch converter using an Arduino LEONARDO.  
It requires the installation of the PlayStation 1/2 Joypad and Nintendo Switch Controller libraries in the Arduino IDE.  
Using an Arduino LEONARDO with 3.3V I/O (e.g., LilyPad or 3.3V version of Pro Micro) is recommended.  

Instructions for wiring follows below.  

## To PSX joypad  

|PSX Pin | PSX Signal | Arduino Pin | Arduino Signal |
| :- |  :- |  :- |  :- |
| 1 | DAT | D14 | MISO [need pullup by 1k owm registor to 3.3V] |
| 2 | CMD | D16 | MOSI |
| 3 | 9V (for motor, If you not necessary NC)| - | - |
| 4 | GND | GND | Power GND |
| 5 | 3.3V | 3.3V | Power 3.3V |
| 6 | Attention | D2 | Digital Pin |
| 7 | SCK | D15 | SCK |
| 8 | NC | - | - |
| 9 | ACK | - | - |


