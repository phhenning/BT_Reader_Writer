

//LIBRARIES
#include <SPI.h>
#include <Wire.h>
#include <MFRC522.h>
#include <MFRC522Extended.h>  //Library extends MFRC522.h to support RATS for ISO-14443-4 PICC.
#include <LCD5110_Graph.h> 
#include <DS3232RTC.h>
#include <TimeLib.h>
#include <Streaming.h>      //not sure this is needed? Dan
#include "pitches.h"     //if error message involving pitches.h comes up then make sure pitches.h file is in the same directory as the sketch
#include "iTimeAfricaV2.h"
#include <SoftwareSerial.h>
#include <stdlib.h>
#include <EEPROM.h>

//GLOBAL DEFINITIONS
String messageDebug;
String messageBattery;
int tagCount = 0;
int flashLed = LOW;                                           //PH PWR good , led on, power low, led flashes
int typeIndex = 0xFF;
BTProg thisPod ;

// RFID COMPONENT Create MFRC522 instance
MFRC522 mfrc522(RFID_CS, LCD_RST);                            // PH get pins from PCB header file 
MFRC522::MIFARE_Key key;

// LCD COMPONENT
// LCD5110 myGLCD(4,5,6,A1,7); V2.0 green pcb
// LCD5110 myGLCD(4,5,6,7,8); (v1.0 blue PCB)
LCD5110 myGLCD(LCD_CLK, LCD_DIN , LCD_COM , LCD_RST, LCD_CE);   // PH get pins from PCB header file
extern unsigned char TinyFont[];      // for 5110 LCD display
extern unsigned char SmallFont[];     // for 5110 LCD display
extern unsigned char MediumNumbers[]; // for 5110 LCD display
extern unsigned char BigNumbers[];    // for 5110 LCD display

// BLUETOOTH COMPONENT
SoftwareSerial BT(UART_RX, UART_TX); // creates a "virtual" serial port/UART 
int progid = 0;      //define program ID for bluetooth programs defined in global definations (1,2,3,4 etc)           

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
      writeRiderData();   //If value is 1 then invoke writeriderdata function
    }   
    if(BTProgList[typeIndex].progid == 2){         // Runs BT program 2
      readCardData();     //If value is 2 then invoke readcard function
    }    
    if(BTProgList[typeIndex].progid == 3){         // Runs  BT program 3
      wipeCard();    //If value is 3 then invoke wipecard function
    }
    if(BTProgList[typeIndex].progid == 4){         // Runs  BT program 4
      wipeTimes();    //If value is 4 then invoke wipetimes function
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
  return STATUS_OK;
}


/* reads timestamp from tag and converts to ms
*  timestamo is passes as unsugned ging mstimestamp
*  error status is returened 
*/
void readStageTime(byte block, unsigned long *msTimesTamp){
  byte buffer[TAG_RD_BUF_SIZE];
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
  status = mfrc522.MIFARE_Read(block, buffer, &size);
  if (status != MFRC522::STATUS_OK){
    BT.println(F("Error-ReScan"));
    BT.println();  
    return status;   
  }

  // Write buffer to console
  for (int i =0; i < 18; i++){
    Serial.print(readBuffer[i], HEX);
  }
  Serial.println();  

  // Convert buffer type byte to unsigned long and store
  int msSize = 4;
  unsigned long ms;
  for (int i=0; i < msSize ; i++ ){
   ms = ms + ((unsigned long)readBuffer[i]  << (i*8));// & 0xFF
  }
  //Serial.println(ms, HEX);
  *msTimesTamp = ms;
  return STATUS_OK  ;

}

unsigned long getStageResutls(int stageNo){
  unsigned long startMs = 0;
  unsigned long endMs   = 0;
  unsigned long stageMs = 0;

  byte startListIndex  = stageNo*2 - 2;
  byte finisListhIndex = stageNo*2 - 1;
  serial.println(stageNo);
  serial.println(podTypeList[startListIndex].podID);
  serial.println(podTypeList[finisListhIndex].podID);

  readStageTime(podTypeList[startListIndex].block,  &startMs);
  readStageTime(podTypeList[finisListhIndex].block, &endMs);

  if (startMs > endMs) {                                    // if ss1Start bigger than ss1Finish, then
    BT.print("ERROR") ;                                         // write ERROR to BT
    stageMs = 0;                                                  // set stage time to zero for total count
  } else if (startMs != 0 && endMs != 0)  { 
    stageMs = endMs - startMs;
    printTimeFromMs(stageMs);
    //TODO log start end and stage time to excel
  } else {
    BT.print("DNS/DNF") ;
    stageMs = 0;
  }
  BT.print("\t") ;             //tab separator
  return stageMs;
}

void readCardData() {
  int StageCount = 0
  unsigned long totalRaceTimeMS = 0;

  // read name, surname and tag number
  for (int i = 0; i < LIST_SIZE_ID; i++){
    readTagIdentities(tagIdList[i].block);
  }

  int noStages = 6;
  for ( int i = 1; i = noStages; i++ ){
    unsigned long stageMs = getStageResutls(i);
    totalRaceTimeMS = totalRaceTimeMS + stageMs;
    if (stageMs != 0) {
      StageCount++;
    }
  }


  printTimeFromMs(totalRaceTimeMS);
  // printTimeFromS(totalRaceTime);
  BT.print("\t") ;             //tab separator  
     
  BT.print(StageCount);
  BT.print("\t") ;             //tab separator
  BT.println();
  beepsLights();  
}


/*
 * Inserts leading zero if required and prints HH:MM:SS.000 format
 */
void printTimeFromMs(unsigned long timeMs) {

  int ms = timeMs % 1000;
  unsigned long timeS = timeMs /1000;
  hours = (timeS/60/60);
  mins = (timeS-(hours*60*60))/60;
  secs = timeS-(hours*60*60)-(mins*60);

  Serial.println(String(timeMs));
  Serial.println(String(hours)+ ":"+ String(mins) + ":" + String(secs) + "." + String(ms));

  if (hours < 10) {BT.print("0");}       
  BT.print(hours);BT.print(F(":"));
  if (mins < 10) {BT.print("0");}
  BT.print(mins);BT.print(F(":"));
  if (secs < 10) {BT.print("0");}
  BT.print(secs);BT.print(F("."));
  BT.print(ms);
}



/*
 * Inserts leading zero if required and prints HH:MM:SS.000 format
 */
void printTimeFromS(unsigned long sec) {
  unsigned long hours = (sec/60/60);
  unsigned long mins = (sec-(hours*60*60))/60;
  unsigned long secs = sec-(hours*60*60)-(mins*60);

  if (hours < 10) {BT.print("0");}       
  BT.print(hours);BT.print(F(":"));
  if (mins < 10) {BT.print("0");}
  BT.print(mins);BT.print(F(":"));
  if (secs < 10) {BT.print("0");}
  BT.print(secs);

  Serial.println("print time: " + String(hours)+ ":"+ String(mins) + ";" + String(secs));
}


  
/* 
 *  Converts type byte to unsigned long int.
 *  Reads each digit from the buffer to the temp placeholders
 */
void buffer2epoch() {
//byte in buffer
      unsigned long temp0 = buffer[0]-'0';          //writes 1st digit of epoch time from buffer to temp0
      unsigned long temp1 = buffer[1]-'0';          //writes 2nd digit of epoch time from buffer to temp1
      unsigned long temp2 = buffer[2]-'0';          //writes 3rd digit of epoch time from buffer to temp2
      unsigned long temp3 = buffer[3]-'0';          //writes 4th digit of epoch time from buffer to temp3
      unsigned long temp4 = buffer[4]-'0';          //writes 5th digit of epoch time from buffer to temp4
      unsigned long temp5 = buffer[5]-'0';          //writes 6th digit of epoch time from buffer to temp5
      unsigned long temp6 = buffer[6]-'0';          //writes 7th digit of epoch time from buffer to temp6
      unsigned long temp7 = buffer[7]-'0';          //writes 8th digit of epoch time from buffer to temp7
      unsigned long temp8 = buffer[8]-'0';          //writes 9th digit of epoch time from buffer to temp8
      unsigned long temp9 = buffer[9]-'0';          //writes 10th digit of epoch time from buffer to temp9
      temp10 = ((temp0*1000000000) + (temp1*100000000) + (temp2*10000000) + (temp3*1000000) + (temp4*100000) + (temp5*10000) + (temp6*1000) + (temp7*100) + (temp8*10) + temp9);
      Serial.println("Timestamp: " + String(temp10));
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
void updateLcdScreen()    
{
    // 1st line of display
    myGLCD.invert (false); //turn oFF inverted screen
    myGLCD.setFont(SmallFont);
    myGLCD.invertText (true); //turn on inverted text
    myGLCD.print((thisPod.progname),CENTER,0);         // Displays POD ID 
    myGLCD.invertText (false); //turn off inverted text

    //2nd line  Display

    myGLCD.print("Hold Watch",CENTER,10);          //Instruction

    // 3rd Line
    myGLCD.print("and don't move",CENTER,20);          //Instruction

    // 4th Line
    int temp    = RTC.temperature();
    int mC      = ((temp*100) % 400 ) /100;
    String msg  = String(temp/4);
    msg         = msg + "." + mC + "C";
    myGLCD.print(msg,LEFT,30);     
    myGLCD.print("Tags: ",RIGHT,30);    
    myGLCD.print( String(tagCount),RIGHT,30);

    // 5th Line
    String messageBattery = getBatteryVoltage();  
    myGLCD.print(messageBattery,LEFT,40);

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
    BT.print("FIRST NAME "); BT.print("\t");
    BT.print("SURNAME "); BT.print("\t");
    BT.print("WATCH NUMBER "); BT.print("\t");
    BT.print("SS 1 "); BT.print("\t");
    BT.print("SS 2 "); BT.print("\t");
    BT.print("SS 3 "); BT.print("\t");
    BT.print("SS 4 "); BT.print("\t");
    BT.print("SS 5 "); BT.print("\t");
    BT.print("SS 6 "); BT.print("\t");
    BT.print("TOTAL"); BT.print("\t");
    BT.print("Stages Done"); BT.print("\t");
    BT.println();
  }
}


// Display fucntion during config loop
void configDisplay( unsigned long timeLeftMs)   {
     // 1st line of display
    myGLCD.invert (true); //turn on inverted screen 
    myGLCD.setFont(SmallFont);
    String msg = VERSION;
    msg = msg + " Configure ";
    myGLCD.print(msg,LEFT,0);       

   //2nd line  Display
    myGLCD.setFont(SmallFont);          
    myGLCD.print(("DO NOT TAG!"),CENTER,10);         

    //3rd line  Display
    myGLCD.setFont(SmallFont);          
    myGLCD.print((thisPod.progname),CENTER,20);        
 
    // 4th Line
    myGLCD.setFont(SmallFont);
    myGLCD.print("Swipe2Change",CENTER,30);

    // 5th Line
    String timeMsg = "Done in ";
    unsigned long ms = (timeLeftMs % 1000)/100;
    timeMsg = timeMsg + (timeLeftMs/1000) + "." + ms + "s";
    myGLCD.setFont(SmallFont);
    myGLCD.print(timeMsg,LEFT,40);

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
