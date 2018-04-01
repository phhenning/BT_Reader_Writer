

//LIBRARIES
#include <SPI.h>
#include <Wire.h>
#include <MFRC522.h>
#include <MFRC522Extended.h>  //Library extends MFRC522.h to support RATS for ISO-14443-4 PICC.
#include <LCD5110_Graph.h> 
#include <DS3232RTC.h>
#include <TimeLib.h>
//#include <Streaming.h>      //not sure this is needed? Dan
#include "pitches.h"     //if error message involving pitches.h comes up then make sure pitches.h file is in the same directory as the sketch
#include "iTimeAfricaV2.h"
#include <SoftwareSerial.h>
//#include <stdlib.h>
#include <EEPROM.h>

//GLOBAL DEFINITIONS
String messageBattery;
int tagCount = 0;
int flashLed = LOW;                                           //PH PWR good , led on, power low, led flashes
int typeIndex = 0xFF;
BTProg thisPod ;

// RFID COMPONENT Create MFRC522 instance
MFRC522 mfrc522(RFID_CS, LCD_RST);                            // PH get pins from PCB header file 
MFRC522::MIFARE_Key key;

LCD5110 myGLCD(LCD_CLK, LCD_DIN , LCD_COM , LCD_RST, LCD_CE);   // PH get pins from PCB header file

// BLUETOOTH COMPONENT
SoftwareSerial BT(UART_RX, UART_TX); // creates a "virtual" serial port/UART 
           

void setup() {
    Wire.begin();                                         // Init I2C buss
 
    Serial.begin(57600);             // PH to use status LED serial needs to be disbaled
    Serial.println("OK");
    //PH BT.begin(9600);                     // Initialize serial communications with BT (Check default Baud Rate of BT module)
    myGLCD.InitLCD();                                     // initialise LCD
    initIoPins();
    initMfrc522();
    configrePod();
}

void loop() {

    updateLcdScreen();                            // invoke LCD Screen fucntion
    delay(200);
  if ( ! mfrc522.PICC_IsNewCardPresent()) {    // Look for new cards
    return;
  }
  if ( ! mfrc522.PICC_ReadCardSerial()) {      // Select one of the cards
    return;
  } else {    
    if(BTProgList[typeIndex].progid == 1){             // Runs BT program 1
 //     writeRiderData();   //If value is 1 then invoke writeriderdata function
    }   
    if(BTProgList[typeIndex].progid == 2){         // Runs BT program 2
      Serial.println("read now");
      readCardData();     //If value is 2 then invoke readcard function
    }    
    if(BTProgList[typeIndex].progid == 3){         // Runs  BT program 3
 //     wipeCard();    //If value is 3 then invoke wipecard function
    }
    if(BTProgList[typeIndex].progid == 4){         // Runs  BT program 4
 //     wipeTimes();    //If value is 4 then invoke wipetimes function
    }
    mfrc522.PICC_HaltA();                       // Halt PICC
    mfrc522.PCD_StopCrypto1();                  // Stop encryption on PCD     
  } 


}

/* reads and display name, surname and tag no 
*  error status is returened 
*/
byte readTagIdentities(byte block){
  byte buffer[TAG_RD_BUF_SIZE];
  byte size = TAG_RD_BUF_SIZE;
  byte status;

  // Authentication
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, block, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK){ 
    //BT.print(F("PCD_Authenticate() failed: "));
    //BT.println(mfrc522.GetStatusCodeName(status));
    BT.println(F("Error-ReScan"));
    BT.println(); 
    return status;
  }

  // Read data from the block into buffer
  status = mfrc522.MIFARE_Read(block, buffer, &size);
  if (status != MFRC522::STATUS_OK){
    BT.println(F("Error-ReScan"));
    BT.println();  
    return status;
  }
  
  // Write buffer to console
  for (uint8_t i = 0; i < TAG_WR_BUF_SIZE; i++) {
    BT.write(buffer[i]) ;   //writes data byte by byte to console
    //myGLCD.print(temp10), CENTER, 30);   //writes data byte by byte to console
  }
  BT.print("\t") ;             //tab separator

//  // Write buffer to console
//  for (int i =0; i < 16; i++){
//    Serial.print(buffer[i]);
//  }
//  Serial.println();  
  return OK;
}


/* reads timestamp from tag and converts to ms
*  timestamo is passes as unsugned ging mstimestamp
*  error status is returened 
*/
void readStageTime(byte block, unsigned long *msTimesTamp){
  byte readBuffer[TAG_RD_BUF_SIZE];
  byte size = TAG_RD_BUF_SIZE;
  byte status;
  
  // Authentication
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A,block, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK){ 
    //       BT.print(F("PCD_Authenticate() failed: "));
    //       BT.println(mfrc522.GetStatusCodeName(status));
    BT.println(F("Error-ReScan"));
    BT.println(); 
    return status;
  }
 
  // Read data from the block into buffer
  status = mfrc522.MIFARE_Read(block, readBuffer, &size);
  if (status != MFRC522::STATUS_OK){
    BT.println(F("Error-ReScan"));
    BT.println();  
    return status;   
  }

  // Convert buffer type byte to unsigned long and store
  int msSize = 4;
  unsigned long ms;
  for (int i=0; i < msSize ; i++ ){
    ms = ms + ((unsigned long)readBuffer[i]  << (i*8));// & 0xFF
  }
  
  //Serial.println(ms);
  *msTimesTamp = ms;
  return OK  ;

}

unsigned long getStageResutls(int stageNo){
  unsigned long startMs = 0;
  unsigned long endMs   = 0;
  unsigned long stageMs = 0;

  byte startListIndex  = stageNo*2 - 2;
  byte finisListhIndex = stageNo*2 - 1;

  readStageTime(podTypeList[startListIndex].block,  &startMs);
  readStageTime(podTypeList[finisListhIndex].block, &endMs);
  Serial.println(podTypeList[startListIndex].podID  + String(startMs));
  Serial.println(podTypeList[finisListhIndex].podID + String(endMs));

  

  if (startMs > endMs) {                                    // if ss1Start bigger than ss1Finish, then
    BT.print(F("ERROR")) ;                                         // write ERROR to BT
    stageMs = 0;                                                  // set stage time to zero for total count
  } else if (startMs != 0 && endMs != 0)  { 
    stageMs = endMs - startMs;
//    Serial.print(F("ok "));
//    Serial.println(String(stageMs));
    printTimeFromMs(stageMs);
    //TODO log start end and stage time to excel
  } else {
    Serial.println(F("missing"));
    BT.print(F("DNS/DNF")) ;
    stageMs = 0;
  }
  BT.print(F("\t")) ;             //tab separator
  return stageMs;
}

void readCardData() {
  int StageCount = 0;
  unsigned long totalRaceTimeMS = 0;

  // read name, surname and tag number
  for (int i = 0; i < LIST_SIZE_ID; i++){
    readTagIdentities(tagIdList[i].block);
  }

  int noStages = 6;
  for ( int i = 1; i <= noStages; i++ ){
      unsigned long stageMs = getStageResutls(i);
      totalRaceTimeMS = totalRaceTimeMS + stageMs;
      if (stageMs != 0) {
        StageCount++;
      }
  }

  Serial.println(F("Total "));
  printTimeFromMs(totalRaceTimeMS);
  // printTimeFromS(totalRaceTime);
  BT.print(F("\t")) ;             //tab separator  
     
  BT.print(StageCount);
  BT.print(F("\t")) ;             //tab separator  
  BT.println();
  beepsLights();  
}


/*
 * Inserts leading zero if required and prints HH:MM:SS.000 format
 */
void printTimeFromMs(unsigned long timeMs) {
  int ms = timeMs % 1000;
  unsigned long timeS = timeMs /1000;
  unsigned int hours = (timeS/60/60);
  unsigned int mins = (timeS-(hours*60*60))/60;
  unsigned int secs = timeS-(hours*60*60)-(mins*60);
  Serial.println(String(hours)+ (F(":")) + String(mins) + (F(":")) + String(secs) + (F(".")) + String(ms));

  if (hours < 10) {BT.print("0");}       
  BT.print(hours);BT.print(F(":"));
  if (mins < 10) {BT.print("0");}
  BT.print(mins);BT.print(F(":"));
  if (secs < 10) {BT.print("0");}
  BT.print(secs);BT.print(F("."));
  BT.print(ms);
}


String getBatteryVoltage() {
    int sensorValue = analogRead(V_MEAS);          // read the input on analog pin A2 for battery voltage reading:
    float voltage   = (float)sensorValue / 155;      // Convert the analog reading to a voltage adjusted to Pieters multimeter
    String batMsg   = "Good";
    if (voltage > BATT_GOOD)  {                   // If Voltage higher than 3.8 display "Good" on LCD
        batMsg    = "Good";
        flashLed  = LOW;
    } else if ( voltage > BATT_LOW ) {            // If Voltage lower than 3.8 display "Turn off POD" on LCD and flash lights. POD will work until 3.5V but will stop working after that.
        batMsg = "Batt Low";
        if ( flashLed == LOW ) {                 
          flashLed = HIGH;                        // toggle status led previouse state 
        } else {
          flashLed = LOW;
        }
    } else {
        batMsg = "PowerOff";             
        flashLed = LOW;
    }
    char volt[5];
    String messageBattery = dtostrf(voltage,1,2,volt);       // conver float to string
    messageBattery        = messageBattery + "V " + batMsg;  // append status 
//    digitalWrite(STATUS_LED, flashLed); 
    return messageBattery;
}

// Displays main Screen Time & Pod station ID
void updateLcdScreen()  {  
    String msg;
    extern unsigned char TinyFont[];      // for 5110 LCD display
    extern unsigned char SmallFont[];     // for 5110 LCD display
    extern unsigned char MediumNumbers[]; // for 5110 LCD display
    extern unsigned char BigNumbers[];    // for 5110 LCD display

    // 1st line of display
    myGLCD.invert (false); //turn oFF inverted screen
    myGLCD.setFont(SmallFont);
    myGLCD.invertText (true); //turn on inverted text
    myGLCD.print((thisPod.progname),CENTER,0);         // Displays POD ID 
    myGLCD.invertText (false); //turn off inverted text

    //2nd line  Display
    msg = "Hold Watch";
    myGLCD.print(msg,CENTER,10);          //Instruction

    // 3rd Line
    msg = "and don't move";
    myGLCD.print(msg,CENTER,20);          //Instruction

    // 4th Line
    int temp    = RTC.temperature();
    int mC      = ((temp*100) % 400 ) /100;
    msg         = String(temp/4);
    msg         = msg + "." + mC + "C";
    myGLCD.print(msg,LEFT,30);    
    msg = "Tags: ";
    myGLCD.print(msg,RIGHT,30);    
    myGLCD.print( String(tagCount),RIGHT,30);

    // 5th Line
    msg = getBatteryVoltage();  
    myGLCD.print(msg,LEFT,40);

    myGLCD.update();
    myGLCD.clrScr();     
            
}

void beepsLights()                                            // Beeps and leds for succesfull scan
{
  int melody[2] = {NOTE_A7, NOTE_C7 };        // notes in the melody:
  int noteDurations[2] = {8, 8};                        // note durations 4 = quarter note, 8 = eighth note, etc.::
    
    for (int thisNote = 0; thisNote < 2; thisNote++) {
        int noteDuration = 1000 / noteDurations[thisNote];      //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
        tone(BUZZER, melody[thisNote], noteDuration);
        digitalWrite(LED_TOP, LOW);                             // turn  LED on
        digitalWrite(LED_BOT, LOW);                             // turn  LED on

        int pauseBetweenNotes = noteDuration * 1.30;            // the note's duration + 30% seems to work well:
        delay(pauseBetweenNotes);
    
        noTone(BUZZER);                                         // stop the tone playing:
        digitalWrite(LED_TOP, HIGH);
        digitalWrite(LED_BOT, HIGH);
    }
}

// These functions are to define the POD role and sync ms with seconds

void configrePod() {
    // read last pod type index from eeprom and select that identiry as default
    typeIndex = EEPROM.read(ADDRESS_BTPROG);
    if ( typeIndex >= LIST_SIZE_BT) {
      typeIndex = 0; // reset indix if its biger than list size
    }
    unsigned long ConfigTimeoutMs = millis() + CONFIG_TIME_MS;
    unsigned long msLeft = CONFIG_TIME_MS;
    while ( millis() < ConfigTimeoutMs ) {
        // check for card read and incremetn typeIndex
        if ( mfrc522.PICC_IsNewCardPresent()) {    // Look for new cards
            typeIndex++;
        }
        if (typeIndex >= LIST_SIZE_BT) {
            typeIndex = 0;                          // if uninitilized set to 0
        }
        thisPod = BTProgList[typeIndex];
        // display curretn pod id and start countdown to applying config
        //Serial.println("array size = " + sizeof(thisPod));
        msLeft = ConfigTimeoutMs - millis();
        configDisplay (msLeft );
        delay (100);
//        Serial.println(String(typeIndex) + "   "+ String(msLeft));
//        Serial.println(BTProgList[typeIndex].progid);
//        Serial.println(BTProgList[typeIndex].progname);

    }
    // save pod type in EEPROM for next boot
    EEPROM.write(ADDRESS_BTPROG, typeIndex);

  if(BTProgList[typeIndex].progid == 2){         // Runs BT program 2
    // Display Column headings (only if bluetooth connected before configure() completes)
    BT.print(F("FIRST NAME \tSURNAME \tWATCH NUMBER \tSS 1 \tSS 2 \tSS 3 \tSS 4 \tSS 5 \tSS 6 \tTOTAL \tStages Done\t\n"));
    //BT.println();
  }
}


// Display fucntion during config loop
void configDisplay( unsigned long timeLeftMs)   {
    extern unsigned char TinyFont[];      // for 5110 LCD display
    extern unsigned char SmallFont[];     // for 5110 LCD display
    extern unsigned char MediumNumbers[]; // for 5110 LCD display
    extern unsigned char BigNumbers[];    // for 5110 LCD display

  // 1st line of display
    myGLCD.invert (true); //turn on inverted screen 
    myGLCD.setFont(SmallFont);
    String msg = VERSION;
    msg = msg + " Configure ";
    myGLCD.print(msg,LEFT,0);       

   //2nd line  Display
    msg = "DO NOT TAG!";
    myGLCD.setFont(SmallFont);          
    myGLCD.print(msg,CENTER,10);         

    //3rd line  Display
    myGLCD.setFont(SmallFont);          
    myGLCD.print((thisPod.progname),CENTER,20);        
 
    // 4th Line
    msg = "Swipe2Change";
    myGLCD.setFont(SmallFont);
    myGLCD.print(msg,CENTER,30);

    // 5th Line
    msg = "Done in ";
    unsigned long ms = (timeLeftMs % 1000)/100;
    msg = msg + (timeLeftMs/1000) + "." + ms + "s";
    myGLCD.setFont(SmallFont);
    myGLCD.print(msg,LEFT,40);

    myGLCD.update();
    myGLCD.clrScr();    
}

// All Genral Purpouse IO pin Setup stuffs 
void initIoPins() {
    // Input Pins
    pinMode(SQ_WAVE, INPUT);

    // Output Pins
    pinMode(LED_TOP, OUTPUT);
    pinMode(LED_BOT, OUTPUT);
//    pinMode(STATUS_LED, OUTPUT);
    // set pins start condition: LOW = LED on, HIGH = LED off
    digitalWrite(LED_TOP, HIGH);                       
    digitalWrite(LED_BOT, HIGH);                       
//    digitalWrite(STATUS_LED, LOW);                     
}

// All RFID init Stuffs
void initMfrc522() {
    SPI.begin();                                          // Init SPI bus
    mfrc522.PCD_Init();                                   // Init MFRC522 card
    mfrc522.PCD_SetAntennaGain(mfrc522.RxGain_max);       // Enhance the MFRC522 Receiver Gain to maximum value of some 48 dB (default is 33dB)

    for (byte i = 0; i < 6; i++) {                // Prepare the key (used both as key A and as key B) using FFFFFFFFFFFFh which is the default at chip delivery from the factory
        key.keyByte[i] = 0xFF;
    }
}
