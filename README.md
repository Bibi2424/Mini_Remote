# Mini_Remote

## Desciption

A Mini Remote build around 2 joysticks, a 5-way switch, a NRF24L01 radio and a screen.
To make development easy, it is based on Arduino, but using Platformio because I don't like to code inside the arduino IDE.

Even if Arduino supports C++, I will try to keep in C as much as possible to be more easily portable.

## Generating image for the screen

At the moment, it expect a black background so black pixels will not been drawn to save time.
Color values are only stored as 2bits for each rgb to save program space.

Procedure:

 - Draw an image with the correct size. 
 - Save it in the ressource folder using 8bits color space and no anti-aliasing.
 - Run:

```
python tools\bmp_to_array.py -f ressources/PRoMo_empty_8b.bmp -o include/promo_outline.h -n promo_outline
```

 - After that, you can simply #include "promo_outline.h" and call draw_image(x, y, image_array, IMAGE_WIDTH, IMAGE_HEIGHT).


## Usefull Links

https://playground.arduino.cc/Main/PinChangeInterrupt/