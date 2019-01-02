
## Libraries

* Adafruit GFX Library
* ArduinoJson
* U8g2
* U8g2_for_Adafruit_GFX
* GxEPD2

## JSON tools

https://arduinojson.org/v5/assistant/

## Font tools

https://learn.adafruit.com/adafruit-gfx-graphics-library/using-fonts
https://github.com/adafruit/Adafruit-GFX-Library/pull/96
http://oleddisplay.squix.ch/#/home
https://tchapi.github.io/Adafruit-GFX-Font-Customiser/
https://bitbucket.org/castortiu/arduino-font-creator/downloads/

### Tip

I'm working on a project which needs German and French characters.

I just wanted to add that fontconvert works fine for this.

fontconvert myfont.ttf 11 32 252

And then save my source files in iso-8859-1 will work fine and print the correct characters to the display.

It's still not a proper way to handle things, but it will work in a pinch for those looking for a simple solution for languages that are within iso-8859-1.


//

It appears Adafruit use Code Page 437, also known as "OEM" https://en.wikipedia.org/wiki/Co.... I use a text editor that supports that (PsPad). However, my keyboard generates characters from ISO8859-1 or similar.

 

Unfortunately it is hard to get away from doing some conversion. UTF-8 seems to be commonly supported in editors, although this isn't maybe the best for small embedded systems.
