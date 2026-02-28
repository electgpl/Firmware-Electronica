# Hi! This is a test for ThinkNode M5 Meshtastic

## If you want to try something different with your ThinkNode M5 hardware, this is the place!

To see the pinout of the ESP32S3 and its internal devices, leave the pinout map at the beginning of each example code. Then you can do whatever you want.

## Firmwares
* ThinkNodeM5_HWtest_v1.4.ino
> This code performs a comprehensive test of all internally connected hardware and peripherals. It's necessary for understanding the system and writing future code.

* ThinkNodeM5_LoRaComm_v2_0.ino
> This code creates a basic communicator between two or more devices with pre-recorded messages, ACKs, and timestamps.

## Back to Meshtastic
> You might want to reuse your hardware with Meshtastic later. Keep in mind that when you flash the node from the Arduino IDE, some bootloader values ​​may be overwritten. To avoid this, I recommend installing and running a Python script (I use Thonny).

First:
```
pip install esptool
```
Second (replace COMx with the COM port number recognized by your ThinkNode):
```
python -m esptool --chip esp32s3 --port COMX --baud 921600 erase_flash
```
Then you can return to https://flasher.meshtastic.org/ to install your official firmware.

Enjoy!