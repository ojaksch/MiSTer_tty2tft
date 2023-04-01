
// needs the following librarys:
// JPEGDEC			https://github.com/bitbank2/JPEGDEC
// GFX_Library_for_Arduino	https://github.com/moononournation/Arduino_GFX
// ESP32Time			https://github.com/fbiego/ESP32Time
// CronAlarms			https://github.com/Martin-Laclaustra/CronAlarms
// U8g2				https://github.com/olikraus/u8g2
// ftpsrv			https://github.com/schreibfaul1/ESP32FTPServer
// Adafruit_FT6206_Library	https://github.com/adafruit/Adafruit_FT6206_Library	(for ILI9341SPI)
//
// edit JPEGDEC.h and change the values of
// JPEG_FILE_BUF_SIZE and MAX_BUFFERED_PIXELS to 4096
//
//
// Don't forget to set your display type - see line IMPORTANT: CHANGE THIS TO YOUR CORRESPONDING DEVICE below
//
// crc32: https://github.com/bakercp/CRC32
// swipe: https://www.codeproject.com/Articles/5287929/Swipe-Gestures-Using-a-TFT-Display
//	  https://www.hackster.io/rakshith_bk/esp32-touch-slider-217156


// Neuer wifi.txt Parameter: ORG/OWN


//#define XDEBUG													// Uncomment to receive debug messages by serial output
String BuildVersion = "230331";
int DispRotation = 1;													// 0=normal portrait, 1=landscape, 2=portrait 180°, 3=landscape 180°

// IMPORTANT: CHANGE THIS TO YOUR CORRESPONDING DEVICE
#define ILI9341														// Possible values are: HX8347D,ILI9341,ILI9486,ILI9488,ILI9481_18bit,ILI9341SPI

// SD card access
#ifdef ILI9341SPI
  #define SD_CS 17													// used for Adafruit 2.8 SPI based TFT shield
#elif defined(ILI9341SPIAZ)
  #define SD_CS 5													// used for AZ-Delivery 2.8 SPI based TFT shield
#else
  #define SD_CS 5													// used for all other shields
#endif

#include <SD.h>
#include <FS.h>
File filehandle;
/*
#include "SdFat.h"
#define SPI_SPEED SD_SCK_MHZ(25)
int SD_CS_PIN = 5;
SdFat32 SD;
File32 dir;
File32 file;
File32 root;
File32 filehandle;
*/

// WiFi
#include <WiFi.h>
#include <HTTPClient.h>

// Setup display and JPG libs
#include <Wire.h>
#include <U8g2lib.h>
#include <Arduino_GFX_Library.h>											// Hardware-specific library
#ifdef ILI9341SPI
  Arduino_DataBus *bus = new Arduino_ESP32SPI(13 /* DC */, 5 /* CS */, 18 /* SCK */, 23 /* MOSI */, 19 /* MISO */, VSPI /* spi_num */);		// used for Adafruit 2.8 SPI based TFT shield
#elif defined(ILI9341SPIAZ)
  Arduino_DataBus *bus = new Arduino_ESP32SPI( 4 /* DC */, 5 /* CS */, 18 /* SCK */, 23 /* MOSI */, 19 /* MISO */, VSPI /* spi_num */);		// used for AZ-Delivery 2.8 SPI based TFT shield
#else
  Arduino_DataBus *bus = new Arduino_ESP32PAR8(15 /* DC */, 33 /* CS */, 4 /* WR */, 2 /* RD */, 12 /* D0 */, 13 /* D1 */, 26 /* D2 */, 25 /* D3 */, 17 /* D4 */, 16 /* D5 */, 27 /* D6 */, 14 /* D7 */);
#endif

#ifdef ILI9341
  Arduino_GFX *gfx = new Arduino_ILI9341(bus, 32 /* RST */, DispRotation /* rotation */, false /* IPS */);
  const uint8_t *DEFAULT_FONT = u8g2_font_6x10_mf;									// Default font for 320x240 (HX8347x/ILI9341)
  #include "touch.h"													// TOUCH for AZ-Delivery 2.8 SPI based TFT shield
#elif defined(ILI9481_18bit)
  Arduino_GFX *gfx = new Arduino_ILI9481_18bit(bus, 32 /* RST */, DispRotation /* rotation */, false /* IPS */);
  const uint8_t *DEFAULT_FONT = u8g2_font_6x10_mf;									// Default font for 320x240 (HX8347x/ILI9341)
  #include "touch.h"													// TOUCH for AZ-Delivery 2.8 SPI based TFT shield
#elif defined(ILI9341SPI)
  Arduino_GFX *gfx = new Arduino_ILI9341(bus, 12 /* RST */, DispRotation /* rotation */, false /* IPS */);		// used for Adafruit 2.8 SPI based TFT shield
  const uint8_t *DEFAULT_FONT = u8g2_font_6x10_mf;									// Default font for 320x240 (HX8347x/ILI9341)
  #include <Adafruit_FT6206.h>												// TOUCH for Adafruit 2.8 SPI based TFT shield
  Adafruit_FT6206 ts = Adafruit_FT6206();
#elif defined(ILI9341SPIAZ)
  Arduino_GFX *gfx = new Arduino_ILI9341(bus, 22 /* RST */, DispRotation /* rotation */, false /* IPS */);		// used for Adafruit 2.8 SPI based TFT shield
  const uint8_t *DEFAULT_FONT = u8g2_font_6x10_mf;									// Default font for 320x240 (HX8347x/ILI9341)
  #include "touch.h"													// TOUCH for AZ-Delivery 2.8 SPI based TFT shield
#elif defined(ILI9486)
  Arduino_GFX *gfx = new Arduino_ILI9486(bus, 32 /* RST */, DispRotation /* rotation */, false /* IPS */);
  const uint8_t *DEFAULT_FONT = u8g2_font_9x15_mf;									// Default font for 480x320 (ILI9486/ILI9488)
  #include "touch.h"													// TOUCH for AZ-Delivery 2.8 SPI based TFT shield
#elif defined(ILI9488)
  Arduino_GFX *gfx = new Arduino_ILI9488(bus, 32 /* RST */, DispRotation /* rotation */, false /* IPS */);
  const uint8_t *DEFAULT_FONT = u8g2_font_9x15_mf;									// Default font for 480x320 (ILI9486/ILI9488)
  #include "touch.h"													// TOUCH for AZ-Delivery 2.8 SPI based TFT shield
#elif defined(HX8347C)
  Arduino_GFX *gfx = new Arduino_HX8347C(bus, 32 /* RST */, DispRotation /* rotation */, true /* IPS */);
  const uint8_t *DEFAULT_FONT = u8g2_font_6x10_mf;									// Default font for 320x240 (HX8347x/ILI9341)
  #include "touch.h"													// TOUCH for AZ-Delivery 2.8 SPI based TFT shield
#elif defined(HX8347D)
  Arduino_GFX *gfx = new Arduino_HX8347D(bus, 32 /* RST */, DispRotation /* rotation */, true /* IPS */);
  const uint8_t *DEFAULT_FONT = u8g2_font_6x10_mf;									// Default font for 320x240 (HX8347x/ILI9341)
  #include "touch.h"													// TOUCH for AZ-Delivery 2.8 SPI based TFT shield
#endif

// RTC functions
#include <ESP32Time.h>
ESP32Time rtc;

// cron functions
#include <time.h>
#include "CronAlarms.h"
CronId cronid1;
CronId cronid2;

// jpg and mjpeg routines
#include "JpegFunc.h"
#include "MjpegClass.h"
static MjpegClass mjpeg;

#include "ESP32FtpServer.h"
#define FTP_USERNAME		"esp32"											// user and pw in FTP Client
#define FTP_PASSWORD		"esp32"
FtpServer ftpSrv;

// OTA update
#include <Update.h>

#include "espmake.h"

// ------------ Variables ----------------

int baudrate = 115200;													// 115200, 921600
int menuloadad = 0;
int ScreenSaverSet = false;
String Dummystring = "";
String ScreenSaverAMPM = "no";
String ScreenSaverClock = "yes";
String ScreenSaverText = "no";
String ScreenSaverPict = "yes";
String ScreenSaverMove = "yes";
String ScreenSaverType = "1";
int ScreenSaverDotCol = WHITE;
int randomold =0;
int randomnew =0;
String hour1 = "";
String hour2 = "";
String minute1 = "";
String minute2 = "";
String second1 = "";
String second2 = "";
int filesize = 0;
int DispWidth = 0;
int DispHeight = 0;
int OffsetX = 0;
int OffsetY = 0;
float ExtraOffsetX = 1;
float ExtraOffsetY = 1;
String folderjpg = "";
String foldermjpeg = "";
String dirletter = "";
String newCommand = "";
String prevCommand = "";
String actCorename = "No Core loaded";											// Actual Received Corename
String oldCorename = "";
String corefilename = "";												// Actual Path+actCorename+Fileextension
String picfnam = "";													// Filename with extension
String fileext = "";													// Fileextension
String displaytype ="";
unsigned int stringlength = 0;
String text = "";
String randNumber = "";
String videoplay = "yes";
String wifiMacString = "";
char spinner = 45;
File myOrigFile;
File myDestFile;
char cron1[15];														// Cron timer #1
char cron2[15];														// Cron timer #2
String WSSID = "SSID";
String WPASS = "PW";
String WIFItimeout = "20";
char *WIFICountry[2];
String OwnMenu = "ORG";
int touchcoordX;
int touchcoordY;
String divisorX;
String divisorY;

String URL = "https://127.0.0.1";
bool USE_WIFI = false;

uint8_t *mjpeg_buf;													// <<== For malloc in Setup

// ================ SETUP ==================
void setup(void) {
  Serial.begin(baudrate);												// 115200 for MiSTer ttyUSBx Device UART Chip on ESP32
  Serial.flush();													// Wait for empty Send Buffer
  Serial.setTimeout(500);												// Set max. Serial "Waiting Time", default = 1000ms

  randomSeed(analogRead(34));												// Init Random Generator with empty Port Analog value

  rtc.setTime(1640995200);												// UTC UNIX time (2022-01-01)
  /*
  // Set all chip selects high to avoid bus contention during initialisation of each peripheral
  digitalWrite(22, HIGH);												// Touch controller chip select (if used)
  digitalWrite(15, HIGH);												// TFT screen chip select
  digitalWrite( 5, HIGH);												// SD card chips select, must use GPIO 5 (ESP32 SS)
  */

  // Initialise the TFT
  gfx->begin();
  gfx->begin(80000000);		// 80 MHz
  gfx->fillScreen(BLACK);
  gfx->setUTF8Print(true);												// enable UTF8 support for the Arduino print() function
  DispWidth = gfx->width();
  DispHeight = gfx->height();
  folderjpg = "/jpg/" + String(DispWidth) + "x" + String(DispHeight) + "/";
  foldermjpeg = "/mjpeg/" + String(DispWidth) + "x" + String(DispHeight) + "/";
  #define MJPEG_BUFFER_SIZE (gfx->width() * gfx->height() * 2 / 4)							// memory for a single JPEG frame
  //if (DispWidth == 320) playpicture("000-sorgcat", 135, 95 ,0);

  if (DispWidth == 480) {												// Sorgcat begins at 200x130
    OffsetX = 60; // 65
    OffsetY = 34; // 35
    ExtraOffsetX = 1.5;
    ExtraOffsetY = 1.333;
  }

  // Initialise SD card, set SPI to 80 MHz
  if (!SD.begin(SD_CS, SPI, 80000000)) {
  //if (!SD.begin(SD_CS_PIN, SPI_SPEED)) {										// SdFat
    Serial.println(F("SD.begin failed!"));
    writetext("Error while initialising SD. Halting.", 1, 0, 20, DEFAULT_FONT, 0, BLUE, false, "clear");
    while (1) delay(0);
  }

  wifiMacString = WiFi.macAddress();

  // Read WiFi Credentials from SD
  filehandle = SD.open("/wifi.txt", "r");
  if (!filehandle) {
    writetext("Could not open the file wifi.txt", 1, 20, 30, DEFAULT_FONT, 0, YELLOW, false, "");
    while (1) delay(0);
  } else {
    WSSID = filehandle.readStringUntil('\n');
    WPASS = filehandle.readStringUntil('\n');
    URL = filehandle.readStringUntil('\n');

    WIFItimeout = filehandle.readStringUntil('\n');
    if (WIFItimeout.isEmpty()) WIFItimeout = "20";

    String WIFIcountry = filehandle.readStringUntil('\n');
    if (WIFIcountry.isEmpty()) {
      WIFIcountry = "US";
    } else {
      WIFIcountry.toCharArray(*WIFICountry, 2);
    }

    if (WSSID == "SSID" && WPASS == "PW") {
      writetext("No valid WiFi credentials, WiFi disabled.", 1, 20, 30, DEFAULT_FONT, 0, YELLOW, false, "");
      delay(1000);
    } else {
      USE_WIFI = true;													// valid SSID/PW found
    }

    OwnMenu = filehandle.readStringUntil('\n');
    if (OwnMenu != "ORG" && OwnMenu != "OWN") OwnMenu = "ORG";

    String rot = filehandle.readStringUntil('\n');
    if (rot.isEmpty()) rot == "1";
    gfx->setRotation(rot.toInt());
    DispRotation = rot.toInt();

    divisorX = filehandle.readStringUntil('\n');
    if (divisorX.isEmpty()) divisorX = "3";
    divisorY = filehandle.readStringUntil('\n');
    if (divisorY.isEmpty()) divisorY = "1.5";
  }
  filehandle.close();

  mjpeg_buf = (uint8_t *) malloc(MJPEG_BUFFER_SIZE);									// Video buffer

  showarcade();														// Initial screen
  if (OwnMenu == "ORG") {
    writetext("- tty2tft -", 0, 127, 85, DEFAULT_FONT, 0, random(0xFFFF), false, "");
    writetext("Welcome!", 0, 138, 98, DEFAULT_FONT, 0, random(0xFFFF), false, "");
  }
  writetext(BuildVersion, 0, 275, 233, DEFAULT_FONT, 3, YELLOW, false, "");

  #ifdef ILI9341SPI
    if (!ts.begin(160)) {												// TOUCH - The bigger the threshold, the more sensible is the touch
      writetext("NT", 0, 10, 233, DEFAULT_FONT, 3, RED, false, "");
    } else {
      writetext("T", 0, 10, 233, DEFAULT_FONT, 3, RED, false, "");
    }   
  #endif

  if (USE_WIFI) {
    // https://github.com/esp8266/Arduino/blob/master/tools/sdk/include/user_interface.h#L750-L760
    //wifi_country_t WIFIcountry;		// https://github.com/esp8266/Arduino/issues/7083
    WiFi.mode(WIFI_STA);												// Explicit use of STA mode
    WiFi.setAutoConnect(true);
    WiFi.setAutoReconnect(true);

    for (int i = 1; i <= 4; i++) {
      int startTime = millis();
      WiFi.disconnect();												// Disconnect from AP if it was previously connected
      WiFi.begin(WSSID.c_str(), WPASS.c_str(), WIFI_ALL_CHANNEL_SCAN);
      WiFi.persistent(true);												// Store Wifi configuration in Flash?
      if (OwnMenu == "ORG") writetext("Waiting for WiFi", 0, 105, 125, DEFAULT_FONT, 0, YELLOW, false, "");
      if (i == 2 && OwnMenu == "ORG") writetext("(" + String(i) + "nd try)", 0, 135, 138, DEFAULT_FONT, 0, YELLOW, false, "");
      if (i == 3 && OwnMenu == "ORG") writetext("(" + String(i) + "rd try)", 0, 135, 138, DEFAULT_FONT, 0, YELLOW, false, "");
      while (WiFi.status() != WL_CONNECTED && (millis() - startTime) <= WIFItimeout.toInt() * 1000) {			// Try to connect for 5 seconds
	if (OwnMenu == "ORG") {
	  rotatespinner();
	  rectfill(210, 117, 15, 15, BLACK);
	  writetext(String(spinner), 0, 210, 125, DEFAULT_FONT, 0, YELLOW, false, "");
	}
	#ifdef XDEBUG
	  Serial.print("WiFi status: ");
	  Serial.println(WiFi.status());
	#endif
	delay(250);
      }
      if (WiFi.status() == WL_CONNECTED) break;
    }
    if (OwnMenu == "ORG") rectfill(100, 115, 125, 25, BLACK);								// Clear the row of "Establishing connection"

    if (WiFi.localIP().toString() == "0.0.0.0") {
      USE_WIFI = false;
      if (OwnMenu == "ORG") {
        writetext("Couldn't connect", 0, 115, 117, DEFAULT_FONT, 0, RED, false, "");
        writetext("Continuing w/o WiFi", 0, 105, 127, DEFAULT_FONT, 0, RED, false, "");
      }
    } else {
      if (OwnMenu == "ORG") writetext("IP: " + WiFi.localIP().toString(), 0, 110, 122, DEFAULT_FONT, 0, random(0xFFFF), false, "");
    }

  }
  if (OwnMenu == "ORG") {
    writetext("Waiting for", 0, 128, 153, DEFAULT_FONT, 0, random(0xFFFF), false, "");
    writetext("Menu or Core", 0, 125, 163, DEFAULT_FONT, 0, random(0xFFFF), false, "");
  }
  menuloadad = 1;

  if (USE_WIFI) ftpSrv.begin(SD, FTP_USERNAME, FTP_PASSWORD);								//username, password for ftp.
}

// ================ MAIN LOOP ===================
void loop(void) {
  if (Serial.available()) {
    newCommand = Serial.readStringUntil('\n');										// Read string from serial until NewLine "\n" (from MiSTer's echo command) is detected or timeout (1000ms) happens.
    #ifdef XDEBUG
      Serial.printf("New command: ", newCommand);
      Serial.printf("Received Corename or Command: %s\n", (char*)newCommand.c_str());
    #endif
  }
  Cron.delay();

  if (USE_WIFI) ftpSrv.handleFTP();
  // reset, user, osd

  #ifdef ILI9341SPI
    if (ts.touched()) touchfunctions();
//    if (ts.touched() && TOUCHED == false) touchfunction1();
//    if (ts.touched() && TOUCHED == true) touchfunction1();
  #endif

  if (newCommand != prevCommand) {											// Proceed only if Core Name changed

    // -- First Transmission --
    if (newCommand.endsWith("QWERTZ")) {										// Process first Transmission after PowerOn/Reboot.
      // Do nothing, just receive one string to clear the buffer.
    }                    

    // -- Test Commands --
    else if (newCommand == "CMDCLS")		gfx->fillScreen(BLACK);
    else if (newCommand == "CMDCRED")		gfx->fillScreen(RED);
    else if (newCommand == "CMDDON")		bus->sendCommand(ILI9341_SLPOUT);					// Turn display on
    else if (newCommand == "CMDDOFF")		bus->sendCommand(ILI9341_SLPIN);					// Turn display off
    else if (newCommand == "CMDDINVON")		bus->sendCommand(ILI9341_INVON);					// Invert screen colors on
    else if (newCommand == "CMDDINVOFF")	bus->sendCommand(ILI9341_INVOFF);					// Invert screen colors off
    else if (newCommand == "CMDRESET")		ESP.restart();								// Reset/Restart ESP
    else if (newCommand == "CMDSNAM") {											// Show actual loaded corename
      footbanner(actCorename);
    }
    else if (newCommand.startsWith("CMDSETTIME,")) {									// Set date and time
      int timestamp = (newCommand.substring(11)).toInt();
      rtc.setTime(timestamp);
    }
    else if (newCommand == "CMDHWINF") {										// Send hardware info, BuildVersion and MAC address
      Serial.println("TFTESP32;" + BuildVersion + ";" + wifiMacString + ";");
    }

    else if (newCommand.startsWith("CMDOTAUPDATE,")) {									// Start OTA update for display xxx
      displaytype = newCommand.substring(13);
      OTAupdate(displaytype);
    }

    else if (newCommand.startsWith("CMDVIDEOPLAY,")) {									// Play a video or not
      videoplay = newCommand.substring(13);
    }
    else if (newCommand.startsWith("CMDROT,")) {									// Rotate the screen
      if (ScreenSaverSet == true) Cron.disable(cronid2);
      int rot = (newCommand.substring(7)).toInt();
      gfx->setRotation(rot);
      DispRotation = rot;
      newCommand = "CMDCOR," + actCorename;
      showpic(corefilename);
      if (ScreenSaverSet == true) Cron.enable(cronid1);
    }
    else if (newCommand == "CMDDUPD") {											// ReNew picture
      if (ScreenSaverSet == true) Cron.disable(cronid2);
      newCommand = "CMDCOR," + actCorename;
      playpicture(picfnam, 0, 0, 0);
      if (ScreenSaverSet == true) Cron.enable(cronid1);
    }
    else if (newCommand == "CMDSAVER,0,0") {										// Disable screensaver
      Cron.free(cronid1);
      Cron.free(cronid2);
      playpicture(picfnam, 0, 0, 0);
    }
    else if (newCommand.startsWith("CMDSAVER,")) {									// Set screensaver
      String t1 = "0 */" + newCommand.substring(newCommand.indexOf(',') +1, newCommand.lastIndexOf(',')) + " * * * *";
      String t2 = "*/" + newCommand.substring(newCommand.lastIndexOf(',') +1) + " * * * * *";
      t1.toCharArray(cron1,15);
      t2.toCharArray(cron2,15);
      #ifdef XDEBUG
	Serial.println("t1 " + t1);
	Serial.println("t2 " + t2);
	Serial.println(cron1);
	Serial.println(cron2);
      #endif
      if (ScreenSaverSet == true) {
	Cron.free(cronid1);
	Cron.free(cronid2);
      }
      cronid1 = Cron.create(cron1, setscreensaver, false);
      cronid2 = Cron.create(cron2, startscreensaver, false);
      Cron.disable(cronid2);
      ScreenSaverSet = true;
      #ifdef XDEBUG
	Serial.println("cronid1:");
	Serial.println(cronid1);
	Serial.println("cronid2:");
	Serial.println(cronid2);
      #endif
    }
    else if (newCommand.startsWith("CMDSAVEROPTS,")) {									// Set screensaver options
      int d1 = newCommand.indexOf(',');
      int d2 = newCommand.indexOf(',', d1+1 );
      int d3 = newCommand.indexOf(',', d2+1 );
      int d4 = newCommand.indexOf(',', d3+1 );
      int d5 = newCommand.indexOf(',', d4+1 );
      int d6 = newCommand.indexOf(',', d5+1 );
      int d7 = newCommand.indexOf(',', d6+1 );
      ScreenSaverAMPM = newCommand.substring(d1 + 1, d2);
      ScreenSaverClock = newCommand.substring(d2 + 1, d3);
      ScreenSaverText = newCommand.substring(d3 + 1, d4);
      ScreenSaverPict = newCommand.substring(d4 + 1, d5);
      ScreenSaverMove = newCommand.substring(d5 + 1, d6);
      ScreenSaverType = newCommand.substring(d6 + 1, d7);
      Dummystring = newCommand.substring(d7 + 1);
      ScreenSaverDotCol = Dummystring.toInt();
    }
    else if (newCommand =="CMDCOR,MENU") {										// Show arcade cabinet and/or cat
      if (ScreenSaverSet == true) Cron.disable(cronid2);
      showmenu();
      newCommand = "";
      if (ScreenSaverSet == true) Cron.enable(cronid1);
    }
    else if (newCommand == "CMDDELETE" && menuloadad == 0) {								// Delete the loaded picture from SD
      if (SD.exists(corefilename)) {
	SD.remove(corefilename);
	picfnam = corefilename.substring(corefilename.lastIndexOf("/") +1);						// filename
	footbanner(picfnam + " deleted");
      }
    } else if (newCommand.startsWith("CMDTXT,")) {									// Command from Serial to write Text
      String TextIn = newCommand.substring(7);										// Get Command Text from "newCommand"

      //Searching for the "," delimiter
      int d1 = TextIn.indexOf(',');											// Find location of first ","
      int d2 = TextIn.indexOf(',', d1+1 );										// Find location of second ","
      int d3 = TextIn.indexOf(',', d2+1 );										// Find location of third ","
      int d4 = TextIn.indexOf(',', d3+1 );										// Find location of fourth ","
      int d5 = TextIn.indexOf(',', d4+1 );										// Find location of fifth ","
  
      //Create Substrings
      int fT = (TextIn.substring(0, d1).toInt());									// Get String for Font-Type
      int cT = (TextIn.substring(d1+1, d2).toInt());									// Get String for Draw Color
      int bT = (TextIn.substring(d2+1, d3).toInt());									// Get String for Background Color
      int xT = (TextIn.substring(d3+1, d4).toInt());									// Get String for X-Position
      int yT = (TextIn.substring(d4+1, d5).toInt());									// Get String for Y-Position
      String TextOut = TextIn.substring(d5+1);										// Get String for Text

      if (fT == 0) {
	writetext(TextOut, 0, xT, yT, u8g2_font_5x7_mf, 0, cT, bT, "");
      } else if (fT == 1) {
	writetext(TextOut, 0, xT, yT, u8g2_font_luBS08_tf, 0, cT, bT, "");
      } else if (fT == 2) {
	writetext(TextOut, 0, xT, yT, u8g2_font_luBS10_tf, 0, cT, bT, "");
      } else if (fT == 3) {
	writetext(TextOut, 0, xT, yT, u8g2_font_luBS14_tf, 0, cT, bT, "");
      } else if (fT == 4) {
	writetext(TextOut, 0, xT, yT, u8g2_font_luBS18_tf, 0, cT, bT, "");
      } else if (fT == 5) {
	writetext(TextOut, 0, xT, yT, u8g2_font_luBS24_tf, 0, cT, bT, "");
      } else if (fT == 6) {
	writetext(TextOut, 0, xT, yT, u8g2_font_lucasarts_scumm_subtitle_o_tf, 0, cT, bT, "");
      } else if (fT == 7) {
	writetext(TextOut, 0, xT, yT, u8g2_font_tenfatguys_tr, 0, cT, bT, "");
      } else if (fT == 8) {
	writetext(TextOut, 0, xT, yT, u8g2_font_7Segments_26x42_mn, 0, cT, bT, "");
      } else {
	writetext(TextOut, 0, xT, yT, u8g2_font_tenfatguys_tr, 0, cT, bT, "");
      }
    } else if (newCommand.startsWith("CMDUPLOAD,")) {									// Upload a picture to SD
      if (ScreenSaverSet == true) Cron.disable(cronid2);
	if (USE_WIFI) {
	  menuloadad = 0;
	  String fetchURL = newCommand.substring(10);
	  actCorename = newCommand.substring(newCommand.lastIndexOf("/") +1, newCommand.lastIndexOf("."));		// Extract corename,
	  picfnam = newCommand.substring(newCommand.lastIndexOf("/") +1);						// complete filename
	  fileext = picfnam.substring(picfnam.lastIndexOf(".") +1);							// and fileextension
	  setfnamvars(actCorename, fileext);
	  if (fileext == "jpg") {
	    if (SD.exists(corefilename)) SD.remove(corefilename);							// If already existing, overwrite
	    fetchfile(fetchURL, corefilename);
	  }
	    if (fileext == "mjpeg") {
	      if (SD.exists(corefilename)) SD.remove(corefilename);							// If already existing, overwrite
	      fetchfile(fetchURL, corefilename);
	  }
	} else {
	  playpicture("000-nowifi", 0, 0, 0);
	  footbanner("No WiFi enabled");
	}
	if (ScreenSaverSet == true) Cron.enable(cronid1);
    } else if (newCommand.startsWith("CMDCOR,")) {
      if (ScreenSaverSet == true) Cron.disable(cronid2);
      menuloadad = 0;
      actCorename = newCommand.substring(7);										// CORENAME
      setfnamvars(actCorename, "jpg");
      stringlength = actCorename.length();

      while (!SD.exists(corefilename)) {										// First try to find a JPG, trimming filename if neccessary
	stringlength--;
	if (stringlength == 2) {
	  actCorename = newCommand.substring(7);									// Reset CORENAME
	  showpic(corefilename);
	  return;
	}
	actCorename = actCorename.substring(0,stringlength);
	setfnamvars(actCorename, "jpg");
      }

      setfnamvars(actCorename, "mjpeg");										// If JPG found also look for MJPG
      if (SD.exists(corefilename)) {
	randNumber = "";
	if (videoplay == "yes" || (videoplay == "may"&& random(2) == 1)) {
	  if (SD.exists(foldermjpeg + dirletter + actCorename + "_alt1.mjpeg")) {					// If there is an _alt1 one, there are maybe more
	    while (!SD.exists(foldermjpeg + dirletter + actCorename + "_alt" + randNumber + ".mjpeg")) {
	      randNumber = random(6);
	      if (randNumber == "0") break;
	    }
	    if (randNumber != "0") actCorename = actCorename + "_alt" + randNumber;
	  }
	  if (SD.exists(foldermjpeg + dirletter + actCorename + ".mjpeg")) playvideo(actCorename, 0, 0);
	}
      }

      setfnamvars(actCorename, "jpg");
      randNumber = "";
      if (SD.exists(folderjpg + dirletter + actCorename + "_alt1.jpg")) {						// If there is an _alt1 one, there are maybe more
	while (!SD.exists(folderjpg + dirletter + actCorename + "_alt" + randNumber + ".jpg")) {
	  randNumber = random(6);
	  if (randNumber == "0") break;
	}
        if (randNumber != "0") actCorename = actCorename + "_alt" + randNumber;
      } // end if_alt
      showpic(corefilename);
      if (ScreenSaverSet == true) Cron.enable(cronid1);
    } // end CMDCOR
    prevCommand = newCommand;
    Serial.print("ttyack;");												// Handshake with delimiter; MiSTer: "read -d ";" ttyresponse < ${TTYDEVICE}"
  } // end newCommand
} // end Main Loop

void showpic(String corefilename) {
  setfnamvars(actCorename, "jpg");
  if (SD.exists(corefilename)) {
    gfx->fillScreen(BLACK);
    playpicture(actCorename, -1, -1, 0);
  } else {
    if (!USE_WIFI) playpicture("000-notavailable", 0, 0, 0);
    if (USE_WIFI) fetchfile(URL + folderjpg + dirletter + picfnam, folderjpg + dirletter + picfnam);
  }
}

void fetchfile(String fetchURL, String fetchfilename) {
  if (USE_WIFI) {
    playpicture("000-board", 0, 0, 0);
    actCorename = fetchURL.substring(fetchURL.lastIndexOf("/") +1, fetchURL.lastIndexOf("."));
    picfnam = fetchURL.substring(fetchURL.lastIndexOf("/") +1);
    fileext = fetchURL.substring(fetchURL.lastIndexOf(".") +1);
    writetext("Fetching " + picfnam + " from", 1, 30, 100, DEFAULT_FONT, 0, YELLOW, false, "");				// Info that we're fetching online
    writetext(fetchURL, 1, 10, 115, DEFAULT_FONT, 0, YELLOW, false, "");
    Serial.println(fetchURL);

    int loaded_ok = getFile(fetchURL, fetchfilename);
    #ifdef XDEBUG
      Serial.println(loaded_ok);
    #endif
    if (loaded_ok == 200 && filesize != 0) {										// Copy received file to SD card
      if (fileext == "jpg") {
	playpicture(actCorename, 0, 0, 0);
      } else if (fileext == "mjpeg") {
	playvideo(actCorename, 0, 0);
      }
    } else {
      writetext("Missing picture or another error", 1, 20, 40, DEFAULT_FONT, 0, random(0xFFFF), false, "clear");
      writetext(picfnam, 1, 20, 80, DEFAULT_FONT, 0, YELLOW, false, "");
      writetext("(http status: " + String(loaded_ok) + ")", 1, 20, 120, DEFAULT_FONT, 0, random(0xFFFF), false, "");
      playpicture("000-smiley-oooh", 190, 120, 0);
      newCommand = prevCommand;
    } // end ifs
  }
}

void showarcade() {
  playvideo("000-arcade", 0, 0);
  if (OwnMenu == "ORG") clearmenu();
}

void showmenu() {
  if (SD.exists(folderjpg + "M/MENU.jpg")) {
    menuloadad = 0;
    if (OwnMenu == "ORG") {
      actCorename = "MENU";
      playpicture("MENU", 0, 0, 0);
    }
  } else {
    if (menuloadad == 0) showarcade();											// Arcade cabinet only if not already loaded
    menuloadad = 1;
    if (OwnMenu == "ORG") { 
      clearmenu();
      actCorename = "000-sorgcat";
      playpicture("000-sorgcat", 135 * ExtraOffsetX, 95 * ExtraOffsetY, 0);
    }
  }
}

void clearmenu() {
  rectfill(95, 75, 132, 93, BLACK);
}

void writetext(String text, int fixedpos, int textposX, int textposY, const uint8_t *fontname, int textrotation, int fontcolor, int backcolor, String clear) {
  if (clear == "clear") gfx->fillScreen(BLACK);
  if (fixedpos == 1) gfx->setCursor(textposX, textposY);								// fixed position
  if (fixedpos == 0) gfx->setCursor(textposX * ExtraOffsetX, textposY * ExtraOffsetY);					// relative position
  //if (fontname != "") gfx->setFont(fontname);
  //gfx->setFontMode(0);
  gfx->setFont(fontname);
  //u8f.setFontDirection(textrotation);
  if (backcolor == false) {
    gfx->setTextColor(fontcolor);																				// Transparent background
  } else {
    gfx->setTextColor(fontcolor, backcolor);										// Background color
  }
  //u8f.setForegroundColor(random(0xFFFF));
  //u8f.setForegroundColor(fontcolor);
  //gfx->setTextSize();					// x*y*pixel_margin
  gfx->print(text);
}

void footbanner(String bannertext) {
  rectfill(50, 200, 220, 25, BLACK);
  writetext(bannertext, 1, DispWidth/2-((bannertext.length()*10)/2), 217 * ExtraOffsetY, u8g2_font_10x20_mf, 0, RED, false, "");	// 207
}

void setscreensaver() {
  #ifdef XDEBUG
    Serial.println("setscreensaver");
  #endif
  Cron.disable(cronid1);
  Cron.enable(cronid2);
}

void startscreensaver() {
  #ifdef XDEBUG
    Serial.println("startscreensaver");
  #endif
  int x, y, z;
  x = DispWidth / 4;
  y = DispHeight / 4;
  // getTime("%A, %B %d %Y %H:%M:%S")   // (String) returns time with specified format       http://www.cplusplus.com/reference/ctime/strftime/
  if (ScreenSaverAMPM == "yes") {
    hour1 = rtc.getTime("%I").substring(0,1);
    hour2 = rtc.getTime("%I").substring(1,2);
  } else {
    hour1 = rtc.getTime("%H").substring(0,1);
    hour2 = rtc.getTime("%H").substring(1,2);
  }
  minute1 = rtc.getTime("%M").substring(0,1);
  minute2 = rtc.getTime("%M").substring(1,2);
  second1 = rtc.getTime("%S").substring(0,1);
  second2 = rtc.getTime("%S").substring(1,2);

  while (randomnew == randomold) randomnew = random(4);
  randomold = randomnew;
  gfx->fillScreen(BLACK);

  if (ScreenSaverPict == "yes" && randomnew == 1) {									// Show core picture
    if (ScreenSaverMove == "yes") {
      playpicture(actCorename, random(DispWidth / 2), random(DispHeight / 2), 2);
    } else {
      playpicture(actCorename, 0, 0, 0);
    }
  } else if (ScreenSaverPict == "yes" && randomnew == 2) {								// Show MENU
    oldCorename = actCorename;
    if (ScreenSaverMove == "yes") {
      playpicture("000-MENU", random(DispWidth / 2), random(DispHeight / 2), 2);
    } else {
      playpicture("000-MENU", 0, 0, 0);
    }
    actCorename = oldCorename;
  } else if (ScreenSaverPict == "yes" && randomnew == 3) {								// Show MENU
    if (ScreenSaverMove == "yes") {
      playpicture("000-tty2tft", random(DispWidth / 2), random(DispHeight / 2), 2);
    } else {
      playpicture("000-tty2tft", 0, 0, 0);
    }
  } else if (ScreenSaverClock == "yes") {										// Show time
    if (ScreenSaverText == "no") {
      if (DispWidth == 320) {
	if (ScreenSaverMove == "yes") {
	  x=random(DispWidth / 2);											// TextWidth = 5*8 Chars
	  y=random(DispHeight - 105);											// TextHeight = 7*2 Lines
	  gfx->fillRoundRect(x+78, y+68, 5, 5, 2, ScreenSaverDotCol);
	  gfx->fillRoundRect(x+78, y+85, 5, 5, 2, ScreenSaverDotCol);
	  playpicture("000-clock" + ScreenSaverType + "-" + hour1, x, y+48, 2);
	  playpicture("000-clock" + ScreenSaverType + "-" + hour2, x+37, y+48, 2);
	  playpicture("000-clock" + ScreenSaverType + "-" + minute1, x+85, y+48, 2);
	  playpicture("000-clock" + ScreenSaverType + "-" + minute2, x+122, y+48, 2);
	} else {
	  gfx->fillRoundRect(158,  95, 10, 10, 5, ScreenSaverDotCol);
	  gfx->fillRoundRect(158, 135, 10, 10, 5, ScreenSaverDotCol);
	  playpicture("000-clock" + ScreenSaverType + "-" + hour1, 0, 57, 0);
	  playpicture("000-clock" + ScreenSaverType + "-" + hour2, 75, 57, 0);
	  playpicture("000-clock" + ScreenSaverType + "-" + minute1, 170, 57, 0);
	  playpicture("000-clock" + ScreenSaverType + "-" + minute2, 245, 57, 0);
	}
      }
      if (DispWidth == 480) {
	if (ScreenSaverMove == "yes") {
	  x=random(DispWidth / 2);											// TextWidth = 5*8 Chars
	  y=random(DispHeight - 165);											// TextHeight = 7*2 Lines
	  gfx->fillRoundRect(x+118, y+93, 7, 7, 3, ScreenSaverDotCol);
	  gfx->fillRoundRect(x+118, y+118, 7, 7, 3, ScreenSaverDotCol);
	  playpicture("000-clock" + ScreenSaverType + "-" + hour1, x, y+64, 2);
	  playpicture("000-clock" + ScreenSaverType + "-" + hour2, x+56, y+64, 2);
	  playpicture("000-clock" + ScreenSaverType + "-" + minute1, x+128, y+64, 2);
	  playpicture("000-clock" + ScreenSaverType + "-" + minute2, x+184, y+64, 2);
	} else {
	  gfx->fillRoundRect(236, 125, 15, 15, 7, ScreenSaverDotCol);
	  gfx->fillRoundRect(236, 180, 15, 15, 7, ScreenSaverDotCol);
	  playpicture("000-clock" + ScreenSaverType + "-" + hour1, 0,66, 0);
	  playpicture("000-clock" + ScreenSaverType + "-" + hour2, 113, 66, 0);
	  playpicture("000-clock" + ScreenSaverType + "-" + minute1, 254, 66, 0);
	  playpicture("000-clock" + ScreenSaverType + "-" + minute2, 367, 66, 0);
	}
      }
    }
    if (ScreenSaverText == "yes") {
      z=random(0xFFFF);
      if (DispWidth == 320) {
	if (ScreenSaverMove == "yes") {
	  x=random(DispWidth - 152);		// TextWidth = 5*8 Chars
	  y=random(DispHeight - 30);		// TextHeight = 7*2 Lines
        }
	writetext("- tty2tft -", 1, x + 37, y, u8g2_font_6x10_mf, 0, z, false, "clear");
	writetext(hour1 + hour2 + ":" + minute1 + minute2 + ":" + second1 + second2, 1, x, y + 5, u8g2_font_7_Seg_33x19_mn, 0, z, false, "");
      }
      if (DispWidth == 480) {
	if (ScreenSaverMove == "yes") {
	  x=random(DispWidth - 168);		// TextWidth = 5*8 Chars
	  y=random(DispHeight - 40);		// TextHeight = 7*2 Lines
	}
	writetext("- tty2tft -", 1, x + 35, y, u8g2_font_9x15_mf, 0, z, false, "clear");
	writetext(hour1 + hour2 + ":" + minute1 + minute2 + ":" + second1 + second2, 1, x, y + 5, u8g2_font_7_Seg_41x21_mn, 0, z, false, "");
      }
    }
  } else {
    if (DispWidth == 320) {
      if (ScreenSaverMove == "yes") {
	x=random(DispWidth - 120);											// TextWidth = 5*8 Chars
	y=random(DispHeight - 58);											// TextHeight = 7*2 Lines
	playpicture("000-tty2tft", x, y, 2);
      } else {
	playpicture("000-tty2tft", 0, 0, 0);
      }
    }
    if (DispWidth == 480) {
      if (ScreenSaverMove == "yes") {
	x=random(DispWidth - 164);											// TextWidth = 5*8 Chars
	y=random(DispHeight - 73);											// TextHeight = 7*2 Lines
	playpicture("000-tty2tft", x, y, 2);
      } else {
	playpicture("000-tty2tft", 0, 0, 0);
      }
    }
  }
}

void setfnamvars(String actCorename, String fileext) {
  dirletter = actCorename.substring(0,1) + "/";
  dirletter.toUpperCase();
  if (isDigit(dirletter.charAt(0))) dirletter = "#/";
  if (fileext == "jpg") corefilename = folderjpg + dirletter + actCorename + "." + fileext;
  if (fileext == "mjpeg") corefilename = foldermjpeg + dirletter + actCorename + "." + fileext;
  picfnam = actCorename + "." + fileext;
  fileext = fileext;
}

void rotatespinner() {
  switch (spinner) {
    case 47:
      spinner = 45;													// slash to minus
      break;
    case 124:
      spinner = 47;													// horizontal to slash
      break;
    case 92:
      spinner = 124;													// backslash to horizontal
      break;
    case 45:
      spinner = 92;													// minus to backslash
      break;
  }
}

//========================== End of main routines ================================

void playpicture(String actCorename, int pictureposX, int pictureposY, int scale) {
  setfnamvars(actCorename, "jpg");
  jpegDraw(corefilename.c_str(), jpegDrawCallback, true /* useBigEndian */, pictureposX, pictureposY, gfx->width(), gfx->height(), scale );	// NEW CLASS
}

void playvideo(String actCorename, int videoposX, int videoposY) {
  gfx->fillScreen(BLACK);
  setfnamvars(actCorename, "mjpeg");
  filehandle = SD.open(corefilename, "r");
  if (!filehandle || filehandle.isDirectory()) {
    footbanner("ERROR: Failed to open " + actCorename + " file for reading");
  } else {
    if (!mjpeg_buf) {
      footbanner("mjpeg_buf malloc failed!");
    } else {
      #ifdef XDEBUG
	Serial.println(F("MJPEG start"));
      #endif
      mjpeg.setup(&filehandle, mjpeg_buf, jpegDrawCallback, true, 0, 0, gfx->width(), gfx->height() );
      while (filehandle.available()) {
	mjpeg.readMjpegBuf();												// Read video
	mjpeg.drawJpg();												// Play video
      }
      #ifdef XDEBUG
	Serial.println(F("MJPEG end"));
      #endif
	filehandle.close();
    }
  }
}

void rectfill(int posX, int posY, int rectwidth, int rectheight, int fillcolor) {
  gfx->fillRect(posX * ExtraOffsetX, posY  * ExtraOffsetY, rectwidth * ExtraOffsetX, rectheight * ExtraOffsetY, fillcolor);
}

// pixel drawing callback
static int jpegDrawCallback(JPEGDRAW *pDraw) {
  gfx->draw16bitBeRGBBitmap(pDraw->x, pDraw->y, pDraw->pPixels, pDraw->iWidth, pDraw->iHeight);
  return 1;
}


// Fetch a file from the URL given and save it to SD
// Return 1 if a web fetch was needed or 0 if file already exists
int getFile(String url, String corefilename) {
  int errornum = 0;
  footbanner("Downloading... ");

  #ifdef XDEBUG
    Serial.println("Downloading "  + corefilename.substring(corefilename.lastIndexOf("/") +1) + " from " + url);
  #endif

  if ((WiFi.status() == WL_CONNECTED)) {										// Check WiFi connection
    #ifdef XDEBUG
      Serial.println("[HTTP] begin...\n");
    #endif

    HTTPClient http;
    http.begin(url);													// Configure server and url
    http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);								// Follow automatically after a "302 redirect"

    #ifdef XDEBUG
      Serial.println("[HTTP] GET...\n");
    #endif

    int httpCode = http.GET();												// Start connection and send HTTP header
    if (httpCode > 0) {													// HTTP header has been send and Server response header has been handled
      filehandle = SD.open(corefilename, "w+");
      if (!filehandle) {
	Serial.println("file open failed");
	return 0;
      }
      #ifdef XDEBUG
	Serial.printfln("[HTTP] GET... code: %d\n", httpCode);
      #endif
      errornum = httpCode;

      if (httpCode == HTTP_CODE_OK) {											// File found at server
	int total = http.getSize();											// Get length of document (is -1 when Server sends no Content-Length header)
	int len = total;
	uint8_t buff[1024] = { 0 };											// Create buffer for read
	WiFiClient * stream = http.getStreamPtr();									// Get tcp stream

	while (http.connected() && (len > 0 || len == -1)) {								// Read all data from server
	  size_t size = stream->available();										// Get available data size
	  if (size) {
	    rotatespinner();
	    rectfill(250, 205, 15, 15, BLACK);
	    writetext(String(spinner), 0, 250, 215, DEFAULT_FONT, 0, RED, false, "");
	    int c = stream->readBytes(buff, ((size > sizeof(buff)) ? sizeof(buff) : size));				// Read up to 128 bytes
	    filehandle.write(buff, c);											// Write it to file
	    if (len > 0) len -= c;											// Calculate remaining bytes
	  }
	  yield();
	}
	#ifdef XDEBUG
	  Serial.println();
	  Serial.print("[HTTP] connection closed or file end.\n");
	#endif
      }
      filehandle.close();

      filehandle = SD.open(corefilename, "r");
      filesize = filehandle.size();											// Check filesize
      filehandle.close();
      if (filesize == 0) SD.remove(corefilename);									// If filesize = 0 delete it
    } else {
      #ifdef XDEBUG
	Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
      #endif
    }
    http.end();
  }
  return errornum; 													// File was fetched from web
}

/*
void ftp_debug(const char* info) {
  if(startsWith(info, "File Name")) return;
  SerialPrintfln("ftpServer:   %s", info);
}
*/

// OTA: https://github.com/espressif/arduino-esp32/blob/master/libraries/Update/examples/SD_Update/SD_Update.ino
void OTAupdate(String displaytype) {
  if (USE_WIFI) {
    Cron.disable(cronid1);
    Cron.disable(cronid2);
    playpicture("000-board", 0, 0, 0);
    writetext("OTA Update", 1, 10, 90, u8g2_font_luBS14_tf, 0, RED, false, "");

    int loaded_ok_ver = getFile("https://www.tty2tft.de/MiSTer_tty2tft-installer/buildver", "/tty2tft-buildver");
    if (loaded_ok_ver == 200) {
      filehandle = SD.open("/tty2tft-buildver", "r");
      String NewBuildVersion = filehandle.readStringUntil('\n');
      filehandle.close();
      if (NewBuildVersion == BuildVersion && displaytype != "TESTOTA") {
	writetext("No update available", 1, 10, 115, u8g2_font_luBS10_tf, 0, RED, false, "");
	delay(3000);
	playpicture(actCorename, 0, 0, 0);
	if (ScreenSaverSet == true) Cron.enable(cronid1);
	return;
      }

      int loaded_ok_mac = getFile("https://www.tty2tft.de/MiSTer_tty2tft-installer/MAC.html?" + wifiMacString, "/tty2tft-mac");
      int loaded_ok_bin = getFile("https://www.tty2tft.de/MiSTer_tty2tft-installer/esp32de_" + NewBuildVersion +"_" + ""+ displaytype + ".bin", "/tty2tft-update.bin");
      if (loaded_ok_bin == 200) {
	WiFi.disconnect();												// | - Disable background tasks
	USE_WIFI = false;												// |
	Serial.end();													// /
	footbanner("Download done");
	delay(1000);
	footbanner("Updating...");
	File updateBin = SD.open("/tty2tft-update.bin", "r");
	size_t updateSize = updateBin.size();
	performUpdate(updateBin, updateSize);
	updateBin.close();
	SD.remove("/tty2tft-buildver");
	SD.remove("/tty2tft-mac");
	SD.remove("/tty2tft-update.bin");
	delay(3000);
	ESP.restart();
      }
    }
    if (ScreenSaverSet == true) Cron.enable(cronid1);
  } else {
    playpicture("000-nowifi", 0, 0, 0);
    footbanner("No WiFi enabled");
  }
}

void performUpdate(Stream &updateSource, size_t updateSize) {
  if (Update.begin(updateSize)) {
    size_t written = Update.writeStream(updateSource);
    if (written == updateSize) {
      writetext("Successfully written: " + String(written) + " kb", 1, 10, 115, u8g2_font_luBS10_tf, 0, RED, false, "");
    } else {
      writetext("Written only: " + String(written) + "/" + String(updateSize) + ". Please Retry." , 1, 10, 115, u8g2_font_luBS10_tf, 0, RED, false, "");
    }
    if (Update.end()) {
      footbanner("OTA done!");
      if (Update.isFinished()) {
	writetext("Update completed. Rebooting." , 1, 10, 130, u8g2_font_luBS10_tf, 0, RED, false, "");
      } else {
	writetext("Something went wrong!" , 1, 10, 130, u8g2_font_luBS10_tf, 0, RED, false, "");
      }
    } else {
      writetext("Error Occurred. Error #: " + String(Update.getError()) , 1, 10, 130, u8g2_font_luBS10_tf, 0, RED, false, "");
    }
  } else {
    writetext("Not enough space for OTA!", 1, 10, 115, u8g2_font_luBS10_tf, 0, RED, false, "");
  }
}

void touchfunctions() {
  int x = 0;
  int y = 0;
  int xfirst = 0;
  int yfirst = 0;
  int xlast = 0;
  int ylast = 0;
  int touchscreen = 1;
  bool TOUCHED = false;
  int buttonpressed = 0;
  int buttonoffset = 0;

  if (ts.touched() && TOUCHED == false) {
    if (ScreenSaverSet == true) {
      Cron.disable(cronid1);
      Cron.disable(cronid2);
    }
    while (ts.touched()) delay(100);											// Wait until untouched
    playpicture("000-touchscreen" + String(touchscreen), -1, -1, 0);							// 000-board with rectangles
    TOUCHED = true;
  }

  while (TOUCHED == true) {
    while (!ts.touched()) delay(100);											// Wait until touched again
    gettouchcoord(ts.getPoint());
    xfirst = touchcoordX;												// Save starting point
    yfirst = touchcoordY;
    while (ts.touched()) gettouchcoord(ts.getPoint());									// Get coordinates ongoing
    xlast = touchcoordX;												// Save ending point after releasing
    ylast = touchcoordY;
    x = touchcoordX;
    y = touchcoordY;
    while (ts.touched()) delay(100);											// Wait until untouched

    if ((ylast - yfirst) > (DispHeight / divisorY.toFloat())) {
      touchscreen = 0;
      TOUCHED = false;
      if (ScreenSaverSet == true) Cron.enable(cronid1);
      playpicture(actCorename, 0, 0, 0);
      break;
    } else if ((ylast - yfirst) < (DispHeight / divisorY.toFloat()) * -1) {
      touchscreen = 0;
      TOUCHED = false;
      if (ScreenSaverSet == true) Cron.enable(cronid1);
      playpicture(actCorename, 0, 0, 0);
      break;
    }

    if ((xlast - xfirst) > (DispWidth / divisorX.toFloat())) {							// Continuous touch > a third of screen
      touchscreen--;
      if (touchscreen < 1) {
	touchscreen = 1;
      } else {
        x = 0; y = 0;
	playpicture("000-touchscreen" + String(touchscreen), -1, -1, 0);
      }
    } else if ((xlast - xfirst) < (DispWidth / divisorX.toFloat()) * -1) {
      touchscreen++;
      if (touchscreen > 3) {
	touchscreen = 3;
      } else {
        x = 0; y = 0;
	playpicture("000-touchscreen" + String(touchscreen), -1, -1, 0);
      }
    }

    if (touchscreen > 0 && x > 0 && y > 0) {
      if (touchscreen == 1) buttonoffset = 0;
      if (touchscreen == 2) buttonoffset = 9;
      if (touchscreen == 3) buttonoffset = 18;
      if (DispWidth == 320) {
	if (x >  20 && x < 100 && y > 20  && y <  70) buttonpressed = 1 + buttonoffset;
	if (x > 120 && x < 200 && y > 20  && y <  70) buttonpressed = 2 + buttonoffset;
	if (x > 220 && x < 300 && y > 20  && y <  70) buttonpressed = 3 + buttonoffset;
	if (x >  20 && x < 100 && y > 90  && y < 140) buttonpressed = 4 + buttonoffset;
	if (x > 120 && x < 200 && y > 90  && y < 140) buttonpressed = 5 + buttonoffset;
	if (x > 220 && x < 300 && y > 90  && y < 140) buttonpressed = 6 + buttonoffset;
	if (x >  20 && x < 100 && y > 160 && y < 210) buttonpressed = 7 + buttonoffset;
	if (x > 120 && x < 200 && y > 160 && y < 210) buttonpressed = 8 + buttonoffset;
	if (x > 220 && x < 300 && y > 160 && y < 210) buttonpressed = 9 + buttonoffset;
      } else if (DispWidth == 480) {
	if (x >  20 && x < 140 && y > 20  && y <  90) buttonpressed = 1 + buttonoffset;
	if (x > 180 && x < 300 && y > 20  && y <  90) buttonpressed = 2 + buttonoffset;
	if (x > 340 && x < 460 && y > 20  && y <  90) buttonpressed = 3 + buttonoffset;
	if (x >  20 && x < 140 && y > 120 && y < 190) buttonpressed = 4 + buttonoffset;
	if (x > 180 && x < 300 && y > 120 && y < 190) buttonpressed = 5 + buttonoffset;
	if (x > 340 && x < 460 && y > 120 && y < 190) buttonpressed = 6 + buttonoffset;
	if (x >  20 && x < 140 && y > 220 && y < 290) buttonpressed = 7 + buttonoffset;
	if (x > 180 && x < 300 && y > 220 && y < 290) buttonpressed = 8 + buttonoffset;
	if (x > 340 && x < 460 && y > 220 && y < 290) buttonpressed = 9 + buttonoffset;
      }
      Serial.print("touchpressed;button" + String(buttonpressed) + ";");

      // Exit if SysInfo was selected
      if (buttonpressed + buttonoffset == 8) {
	touchscreen = 0;
	TOUCHED = false;
	if (ScreenSaverSet == true) Cron.enable(cronid1);
	playpicture(actCorename, 0, 0, 0);
	break;
      }
    }
  }
}

void gettouchcoord(TS_Point p) {
  if (DispRotation == 1) {
    p.x = map(p.x, 0, gfx->height(), gfx->height(), 0);									// flip it around to match the screen.
    p.y = map(p.y, 0, gfx->width(), gfx->width(), 0);
  }
  if (DispRotation == 3) {
    p.x = map(p.x, 0, gfx->height(), 0, gfx->height());
    p.y = map(p.y, 0, gfx->width(), 0, gfx->width());									// flip it around to match the screen.
  }
  touchcoordX = p.y;
  touchcoordY = gfx->height() - p.x;
  delay(100);
}
