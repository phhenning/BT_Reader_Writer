/**
*header file for ITA v2 PCB
*/

#define VERSION "V3.1"
//GLOBAL DEFINITIONS         

#define DS3231_I2C_ADDRESS 	0x68

//PCB pin definition for V32
#define LCD_RST				A1
#define SQ_WAVE				A2
#define CONFIG 				A3
#define SDA					SDA
#define SCL					SCL
#define V_MEAS				A7
#define STATUS_LED	  1 //bluetooth also uses D1 so if BT required, all statusled commands must be taken out
#define BUZZER 				2
#define LED_TOP				3
#define LED_BOT				A0
#define LCD_CLK				4
#define LCD_DIN				5
#define LCD_COM 			6
#define LCD_CE 				7
#define UART_TX				8
#define UART_RX				9
#define RFID_CS				10 
#define RFID_MOSI			MOSI 
#define RFID_MISO			MISO
#define RFID_SCK			SCK 


// EEPROM stuffs
// Arduino have different amounts of EEPROM
// 1024 bytes on the ATmega328P

#define ADDRESS_POD_TYPE 0
#define ADDRESS_BTPROG 0

#define BATT_GOOD 3.8
#define BATT_LOW  3.5
// BELOW IS STATION/POD IDENTIFICATION!
#define LIST_SIZE 16
#define START_POD  0
#define FINISH_POD 1

// BELOW IS BT program TYPE IDENTIFICATION!
#define LIST_SIZE_BT 4

struct BTProg {
	int progid;
	char progname[16]; 
} ;


struct BTProg BTProgList [LIST_SIZE_BT] = {
	{ 1 , "BT WRITE2WATC)H"},
	{ 2 , "BT D/L RESULTS"}, 
	{ 3, "BT WIPE ALL" },
	{ 4, "BT WIPE TIMES"}
};

// BELOW IS STATION/POD IDENTIFICATION!

struct podType {
  int block;
  char podID[16]; 
} ;

#define LIST_SIZE_PODS 12
struct podType podTypeList [LIST_SIZE_PODS] = {
  { 8 , "Stage 1 Start"},
  { 9 , "Stage 1 Finish"}, 
  { 12, "Stage 2 Start" },
  { 13, "Stage 2 Finish"},
  { 16, "Stage 3 Start" },
  { 17, "Stage 3 Finish"},
  { 20, "Stage 4 Start" },
  { 21, "Stage 4 Finish"},
  { 24, "Stage 5 Start" },
  { 25, "Stage 5 Finish"},
  { 28, "Stage 6 Start" },
  { 29, "Stage 6 Finish"},
  };
       
// variouse values
#define CONFIG_TIME_MS 10000  



struct tagId {
  int block;
  char description[16];
};

#define LIST_SIZE_ID 3
struct tagId tagIdList [LIST_SIZE_ID] = {
  { 1, "First Name"},
  { 2, "Surname"   },
  { 4, "Tag Number"}
};

// read buffer has 2 crc bytes
#define TAG_RD_BUF_SIZE 18
#define TAG_WR_BUF_SIZE 16

#define OK 0
#define ERROR          1
//#define STATUS_COLLISION      2
//#define STATUS_TIMEOUT        3
//#define STATUS_NO_ROOM        4
//#define STATUS_INTERNAL_ERROR 5
//#define STATUS_INVALID        6
//#define STATUS_CRC_WRONG      7
//#define STATUS_MIFARE_NACK    8
