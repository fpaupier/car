## Building a minimal robot car

![doc](doc/car-v1.png)
You can pilot it from your local wifi network from your laptop/phone
![portal](doc/browser%20control.jpeg)

### BOM
- 4 wheel chassis
- 4x engine continuous current between 3 an 12 volt - recommend (6 to 8V tension)
- 4x h bridge L298N 
- ESP32 WRoom 32 with wifi and BLE
- Powerbank of 1000mah with usb and usbc output (to power the esp32)
- 2x 9V battery with pos and neg output cable (to power the motors) 

My goal is to be able to pilot my car through wifi with a minimal web interface that I can access from my laptop or 
phone with simple controls to tell teh car to go either straight, left, right or straight. 

### Getting started

I use platform IO

Copy the `example.secrets.h` into `secrets.h` in the `include` directory to enable remote control

Build the project

Upload an monitor to get the ESP32 IP adress

Connect to your browser to pilot the car