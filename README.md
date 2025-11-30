# APInt Code Tournament Hardware ESP32S3 XInput
  
> Simulate an XInput from a Wifi ESP32S3 UART connection to an Arduino Leonardo.  

You can simulate an Xbox controller with an HC05 but it is a pain in the ass to use Bluetooth on all platform at the same time.  
(And often lead you to use non open source and free code.)  

But it was a Wifi Websocket :)  
  
Let's try to have an Keyboard / mouse  ESP32S3 that redirect with UART the bytes for simulating the XInput on Leonardo.  
 
  
ESP32S3: https://www.amazon.com.be/-/en/Freenove-ESP32-S3-Dual-core-Microcontroller-Wireless/dp/B0DHJZ1V81  
Leonardo: https://www.amazon.com.be/-/en/KEYESTUDIO-Leonardo-Microcontroller-Development-Arduino/dp/B0827DHT3B  

The code I design in the past to use TTL or HC06:    
https://github.com/EloiStree/2023_06_21_ArduinoToX360    
That we are going to use here.    

I use the format [S2W](https://github.com/EloiStree/2024_08_29_ScratchToWarcraft) [IID](https://github.com/EloiStree/IID) for my project

Some Old code in case I need it:   
https://github.com/EloiStree/2024_11_31_ReadHardwareToIndexInteger/tree/main/  
 
