import board
import neopixel
import busio
import time
import adafruit_apds9960.apds9960

i2c = board.STEMMA_I2C()
sensor = adafruit_apds9960.apds9960.APDS9960(i2c)
pixels = neopixel.NeoPixel(board.NEOPIXEL, 1)

sensor.enable_color = True
sensor.color_integration_time = 10
sensor.enable_proximity = True

while True:
    r, g, b, c = sensor.color_data
    print("Red: {0}, Green: {1}, Blue: {2}, Clear: {3}".format(r, g, b, c/ 65536 * 256))
    pixels.fill((c ,0 ,0 ))


