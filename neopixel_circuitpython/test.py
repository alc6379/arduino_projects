import time
import board
import neopixel
 
pixel_pin = board.A1
num_pixels = 8
 
pixels = neopixel.NeoPixel(pixel_pin, num_pixels, brightness=.03, auto_write=False, pixel_order=neopixel.RGBW)

BLANK = (0,0,0)
RED = (255, 0, 0)
YELLOW = (255, 150, 0)
GREEN = (0, 255, 0)
CYAN = (0, 255, 255)
BLUE = (0, 0, 255)
PURPLE = (180, 0, 255)

for p in range(num_pixels):
    pixels[p] = BLANK 
    
pixels.show()

color_order = [RED, YELLOW, GREEN, CYAN, BLUE, PURPLE]

color_delay = 0.1

previous_color = BLANK
current_color = BLANK

def moveToTop(position):
    for px in range(num_pixels - 1, position - 1, -1):
        pixels[px] = current_color
        for old_px in range(px + 1, num_pixels, 1):
            pixels[old_px] = previous_color
        pixels.show()
        time.sleep(color_delay)
    

while True:
    
    for color in color_order:
        current_color = color
        print(color)
        for p in range(num_pixels):
            moveToTop(p)
        previous_color = color
