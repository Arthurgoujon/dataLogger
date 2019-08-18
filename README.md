Purpose: 
  Log data from a sensor in an SD card, and use a hashing function to create a unique signature of the data log and prevent any tampering with existing data. 
  
Parts: 
- Arduino Uno
- DHT11 temperature and humidity sensor 
- SD card reader 
- HC05 bluetooth connector (not included in program yet)

Schematics: 
  I will provide schematics diagram later but for now: 
    - Connect SD card reader CS pin to Arduino/10
    - Connect DHT11 signal pin to Arduino/2

Dependencies: 
- Sha1 library: 
  - I am using this library: https://github.com/Cathedrow/Cryptosuite. 
  - Installation: 
    Make a 'libraries' directory with your Arduino sketches folder if you do not already have one.
    Copy the 'Sha' directory into that directory.
    Restart Arduino to rescan for new libraries.
  - I found some issues when using the library at first due to some deprecations. 
    I made small changes and included the new version in this project. 
- SD library 
- DHT library 

Known issues: 
- the program currently fails after several iterations. This is likely due to abusive use of String and string operations, 
  and cluttering of the Heap memory. I am curerrntly working on a refactoring of the code using functions to move memory to the Stack and char instead of String.

