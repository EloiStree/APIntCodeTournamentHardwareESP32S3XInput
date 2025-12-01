
( Note NTP and delayer is not in this first version )

- PC
  - Make a unit test for the gamepad in python that push to port 3615 of the ESP32 IP
- ESP32
  - Connect to Wifi
  - Display current IP and port to use
  - Make NTP Connection to server
  - Prepare the UART export on 17 18
  - Send `_PING_`
  - Wait `_PONG_` to check if there is receiver.
  - Check if bytes  is 4 8 12 16 bytes idd else it is text.
  - If Integer value is Mouse or keyboard
    - Print the Mouse value
    - Print the keyboard balue
  - else if gamepad value
    -  Send the Integer Value to UART


- Leonardo:
  - Listen to UART 9 8
  - Add a delay array with a fixed size of NN
  - if lenght 10 long set NTP Local Timestamp UTC from the host
  - if 4  execture directly
  - if 12 add integer as delayed

  
