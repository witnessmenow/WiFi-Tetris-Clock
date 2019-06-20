# WiFi-Tetris-Clock
A WiFi clock made of falling tetris blocks. Runs on an ESP32 with an RGB LED Matrix (Currently the EPS8266 version is crashing!)

![img](https://thumbs.gfycat.com/RecklessSpecificKoodoo-size_restricted.gif)

## Requirements
- [TetrisAnimation](https://github.com/toblum/TetrisAnimation)
- [ezTime](https://github.com/ropg/ezTime)
- [Adafruit GFX](https://github.com/adafruit/Adafruit-GFX-Library)


## Hardware

### Px RGB LED Matrix
- RGB LED Matrix 64 x 32 P3 (The one I'm using) 
  - [Amazon.co.uk*](https://amzn.to/2zLeqzl)
  - Amazon.com  - Sold out and equivalent has bad reviews
  - [Aliexpress*](http://s.click.aliexpress.com/e/EMvjy3z)
- [TinyPICO](https://www.crowdsupply.com/unexpected-maker/tinypico)
- [ESP32 Mini*](http://s.click.aliexpress.com/e/cozT6Vx6) (The generic ESP32 board I'm working on will be based on this)
- 5v 8A laptop style PSU
  - [Aliexpress*](http://s.click.aliexpress.com/e/vzJ2rnE)
  - [Amazon.co.uk*](https://amzn.to/2JBauB2)
  - [Amazon.com*](https://amzn.to/2Jl93qL)
- [TinyPICO Matrix Shield](https://www.tindie.com/products/brianlough/tinypico-matrix-shield/)
- [D1 Mini Matrix Shield](https://www.tindie.com/products/brianlough/d1-mini-matrix-shield/
) (Although not super useful for this project!)

#### Libraries
 - https://github.com/adafruit/Adafruit-GFX-Library
 - https://github.com/2dom/PxMatrix


### SSD1306 OLED

#### Libraries
 - https://github.com/adafruit/Adafruit_SSD1306


\* Affilate links

## Wiring:

Wiring is desctribed on the [PxMatrix library](https://github.com/2dom/PxMatrix) pretty well, but to try make it a little easier I have created wiring diagrams for the different types of boards.

#### Tiny PICO
![img](https://i.imgur.com/aDPyHFh.png)

#### Generic ESP32
![img](https://i.imgur.com/0FB11me.png)

#### D1 Mini ESP8266
Although not working with this sketch yet
![img](https://i.imgur.com/bIBcAXR.png)
