
### TODO:

- use your Lab 1 firefly code to generate ADPS9960 I2C traffic and display it on a lab oscilloscope
- take a screenshot of some portion of this exchange, and figure out/annotate what's happening based on the protocol documentation in the ADPS9960 datasheet 

For this part, we run firefly code from lab1 in order to confirm that i2c communication is active between the QTPY2040 and APDS9960. We choose using Logic Analyzer Probe Cable to display it on oscilloscope. 

#### Set up 
- Select Digital on the Oscilloscope
- Select D0 and D1 in the oscilloscope 
- Connect SCL pin to D0 and SDA pin to D1 of Logic Analyzer Probe Cable

#### Screenshot of oscilloscope screen 
![](https://github.com/SEN316/ese519-lab2B/blob/main/5_i2c_traffic/i2c_traffic.jpg)
