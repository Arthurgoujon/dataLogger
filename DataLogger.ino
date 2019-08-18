
#include <dht.h>
#include <SD.h>
#include "sha1.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////FUNCTIONS and DECLARATIONS///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
dht DHT;
#define DHT11_PIN 2

File myFile;
const int chipSelect = 10;

uint8_t* hash;
const char* privateKey = "secret";
int counter = 0;
String block;

char* translate(uint8_t* hash)
{
  int i;
  int j = 0;
  char* hex = (char *) malloc (39);
  for (i = 0; i < 20; i++) {
    hex[j] = ("0123456789abcdef"[hash[i] >> 4]);
    hex[j + 1] = ("0123456789abcdef"[hash[i] & 0xf]);
    j = j + 2;
  }
  hex[j] = '\0';
  return hex;
}


//this function is used to measure free SRAM

#ifdef __arm__
// should use uinstd.h to define sbrk but Due causes a conflict
extern "C" char* sbrk(int incr);
#else  // __ARM__
extern char *__brkval;
#endif  // __arm__

int freeMemory() {
  char top;
#ifdef __arm__
  return &top - reinterpret_cast<char*>(sbrk(0));
#elif defined(CORE_TEENSY) || (ARDUINO > 103 && ARDUINO != 151)
  return &top - __brkval;
#else  // __arm__
  return __brkval ? &top - __brkval : &top - __malloc_heap_start;
#endif  // __arm__
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////SETUP////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void setup() {
  Serial.begin(9600);
  Serial.print("Initializing SD card...");
  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    while (1);
  }
  Serial.println("card initialized.");

  //delete previous data from SD card
  SD.remove("blocks.txt");
  SD.remove("data.txt");

  //mine the first block
  Sha1.init();
  Sha1.print(privateKey);
  block = translate(Sha1.result());

  //create file for Proof on the SD card and save the first block
  File dataFile = SD.open("blocks.txt", FILE_WRITE);
  if (dataFile) {
    dataFile.println(block);
    dataFile.close(); // close the file
    Serial.print("Initial block: ");
    Serial.println(block);
    Serial.println();
  }
  else {
    Serial.println("error opening blocks.txt");
  }
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////LOOP/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void loop() {
  Serial.print("free memory: ");
  Serial.println(freeMemory());
  delay(3000);
  counter += 1;

  int chk = DHT.read11(DHT11_PIN);
  Serial.print(counter);
  Serial.print( ": " );
  Serial.print( "Temperature = " );
  Serial.print( DHT.temperature, 1 );
  Serial.print( " deg. C, H = " );
  Serial.print( DHT.humidity, 1 );
  Serial.println( "%" );

  // Log data
  File dataFile = SD.open("data.txt", FILE_WRITE);
  // if the file opened okay, write to it:
  if (dataFile) {
    dataFile.print(counter);
    dataFile.print( ": " );
    dataFile.print( "Temperature = " );
    dataFile.print( DHT.temperature, 1 );
    dataFile.print( " deg. C, H = " );
    dataFile.print( DHT.humidity, 1 );
    dataFile.println( "%" );
    dataFile.close(); // close the file
  }
  // if the file didn't open, print an error:
  else {
    Serial.println("error opening data.txt");
  }

  //read previous block on the SD card
  myFile = SD.open("blocks.txt");
  String OldBlock = myFile.readString();
  OldBlock.trim();
  Serial.print("Old block: ");
  Serial.println(OldBlock);
  myFile.close();

  //delete the old block on the SD card
  SD.remove("blocks.txt");

  //Concatenate
  block = OldBlock + DHT.temperature;
  Serial.print("concatenated: ");
  Serial.println(block);

  //mine and print
  Sha1.print(block);
  block = translate(Sha1.result());
  Serial.print("new block: ");
  Serial.println(block);

  //Save block in a new blocks.txt file
  File blockFile = SD.open("blocks.txt", FILE_WRITE);
  if (blockFile ) {
    blockFile .println(block);
    blockFile .close(); // close the file
    Serial.println("saved to SD card");
    Serial.println();
  }
  else {
    Serial.println("error opening blocks.txt");
  }

}
