/*******************************************************************
    Tetris clock that fetches its time Using the EzTimeLibrary

    NOTE: THIS IS CURRENTLY CRASHING!

    For use with an ESP8266
 *                                                                 *
    Written by Brian Lough
    YouTube: https://www.youtube.com/brianlough
    Tindie: https://www.tindie.com/stores/brianlough/
    Twitter: https://twitter.com/witnessmenow
 *******************************************************************/

// ----------------------------
// Standard Libraries - Already Installed if you have ESP32 set up
// ----------------------------

#include <SPI.h>
#include <Wire.h>
#include <ESP8266WiFi.h>

// ----------------------------
// Display Libraries - each one of these will need to be installed.
// ----------------------------
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Adafruit GFX library is a dependancy for the PxMatrix Library
// Can be installed from the library manager
// https://github.com/adafruit/Adafruit-GFX-Library
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// SCL GPIO5
// SDA GPIO4
#define OLED_RESET     -1  // NOT USED
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

uint16_t scale = 5;
bool getTime = true; // false to test without wifi

// @todo adjust offsets automatically by scale or calculate alignments
// uint16_t xoffset = -6;
// uint16_t xoffset = -16;
uint16_t xoffset = -21;
uint16_t yoffset = scale*13;

#include <TetrisMatrixDraw.h>
// This library :)
// https://github.com/toblum/TetrisAnimation

#include <ezTime.h>
// Library used for getting the time and adjusting for DST
// Search for "ezTime" in the Arduino Library manager
// https://github.com/ropg/ezTime

// ---- Stuff to configure ----

// Initialize Wifi connection to the router
char ssid[] = "****";     // your network SSID (name)
char password[] = "****"; // your network key

// Set a timezone using the following list
// https://en.wikipedia.org/wiki/List_of_tz_database_time_zones
#define MYTIMEZONE "Europe/Dublin"

// Sets whether the clock should be 12 hour format or not.
bool twelveHourFormat = true;

// If this is set to false, the number will only change if the value behind it changes
// e.g. the digit representing the least significant minute will be replaced every minute,
// but the most significant number will only be replaced every 10 minutes.
// When true, all digits will be replaced every minute.
bool forceRefresh = true;

// -----------------------------

TetrisMatrixDraw tetris(display); // Main clock
TetrisMatrixDraw tetris2(display); // The "M" of AM/PM
TetrisMatrixDraw tetris3(display); // The "P" or "A" of AM/PM

Timezone myTZ;
unsigned long oneSecondLoopDue = 0;

bool showColon         = true;
bool finishedAnimating = false;
bool displayIntro      = true; // false to disable intro
bool animateFlag       = true;

String lastDisplayedTime = "";
String lastDisplayedAmPm = "";

// This method is needed for driving the display
void display_updater() {
  display.display();
}

void setAnimateFlag() {
  animateFlag = true;
}

// This method is for controlling the tetris library draw calls
void  animationHandler()
{
  uint16_t offsetscale = scale-1;
  // Not clearing the display and redrawing it when you
  // dont need to improves how the refresh rate appears
  if (!finishedAnimating) {
    display.clearDisplay();
    if (displayIntro) {
      tetris.scale = 1;
      finishedAnimating = tetris.drawText(10, 30);
      if(finishedAnimating) delay(2000);
    }
    else
    {
      tetris.scale = scale;
      if (twelveHourFormat) {
        // 12 hr display
        
        // Place holders for checking are any of the tetris objects
        // currently still animating.
        bool tetris1Done = false; // Time
        bool tetris2Done = false; // A
        bool tetris3Done = false; // P/M

        // @todo scale*xypos offsets to adjust x,y for scales, or auto center etc.
        tetris1Done = tetris.drawNumbers(xoffset, yoffset, showColon);

        if(scale<4)tetris2Done = tetris2.drawText(56*offsetscale, 25*offsetscale);
        else if (tetris1Done) tetris2Done = tetris2.drawText(120,10);

        // Only draw the top letter once the bottom letter is finished.
        if (tetris2Done) {
          if(scale<4) tetris3Done = tetris3.drawText(56*offsetscale, 15*offsetscale);
          else tetris3Done = tetris3.drawText(110,10);
        }

        finishedAnimating = tetris1Done && tetris2Done && tetris3Done;
      } else {
        // 24h display
        finishedAnimating = tetris.drawNumbers(2, 46, showColon);
      }
    }
  }
  display.display(); // update display
}

void drawIntro(int x = 0, int y = 0)
{
  Serial.println("draw intro");
  tetris.drawChar("W", x, y,      WHITE);
  tetris.drawChar("r", x + 5, y,  WHITE);
  tetris.drawChar("i", x + 11, y, WHITE);
  tetris.drawChar("t", x + 17, y, WHITE);
  tetris.drawChar("t", x + 22, y, WHITE);
  tetris.drawChar("e", x + 27, y, WHITE);
  tetris.drawChar("n", x + 32, y, WHITE);
  tetris.drawChar(" ", x + 37, y, WHITE);
  tetris.drawChar("b", x + 42, y, WHITE);
  tetris.drawChar("y", x + 47, y, WHITE);
}

void drawConnecting(int x = 0, int y = 0)
{
  Serial.println("drawconnecting");
  tetris.drawChar("C", x, y,      WHITE);
  tetris.drawChar("o", x + 5, y,  WHITE);
  tetris.drawChar("n", x + 11, y, WHITE);
  tetris.drawChar("n", x + 17, y, WHITE);
  tetris.drawChar("e", x + 22, y, WHITE);
  tetris.drawChar("c", x + 27, y, WHITE);
  tetris.drawChar("t", x + 32, y, WHITE);
  tetris.drawChar("i", x + 37, y, WHITE);
  tetris.drawChar("n", x + 42, y, WHITE);
  tetris.drawChar("g", x + 47, y, WHITE);
}

void setup() {
  // Serial.begin(115200);

  // Attempt to connect to Wifi network:
  Serial.print("Connecting Wifi: ");
  Serial.println(ssid);

  // Set WiFi to station mode and disconnect from an AP if it was Previously
  // connected
  
  // WiFi.mode(WIFI_OFF);
  WiFi.begin(ssid, password);
  
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 64x48
    Serial.println(F("SSD1306 allocation failed"));
  }

  display.clearDisplay();
  display.setTextSize(1);             // Normal 1:1 pixel scale
  display.setTextColor(WHITE);        // Draw white text
  display.setCursor(0,0);             // Start at top-left corner
  display.println(F("Connecting..."));
  display.display();

  delay(2000);
  // Setup EZ Time
  setDebug(INFO);
  if(getTime){
    if(WiFi.status()==WL_CONNECTED) waitForSync();

    Serial.println();
    Serial.println("UTC:             " + UTC.dateTime());

    myTZ.setLocation(F(MYTIMEZONE));
    Serial.print(F("Time in your set timezone:         "));
    Serial.println(myTZ.dateTime());
  }

  display.clearDisplay();

  if(displayIntro){
    // tetris.scale = 1;
    drawIntro(6, 12);
    display.display();
    delay(2000);
    tetris.setText("B. LOUGH");
  }

  tetris.scale = scale;
  setupMonochrome();
}

void setMatrixTime() {
  Serial.println("setMatrixTime");
  String timeString = "";
  String AmPmString = "";

  if (twelveHourFormat) {
    // Get the time in format "1:15" or 11:15 (12 hour, no leading 0)
    // Check the EZTime Github page for info on
    // time formatting
    timeString = myTZ.dateTime("g:i");

    //If the length is only 4, pad it with
    // a space at the beginning
    if (timeString.length() == 4) {
      timeString = " " + timeString;
    }

    //Get if its "AM" or "PM"
    AmPmString = myTZ.dateTime("A");
    if (lastDisplayedAmPm != AmPmString) {
      Serial.println(AmPmString);
      lastDisplayedAmPm = AmPmString;
      // Second character is always "M"
      // so need to parse it out
      tetris2.setText("M", forceRefresh);

      // Parse out first letter of String
      // tetris3.setText(getTime ? AmPmString.substring(0, 1) : "A", forceRefresh);
      tetris3.setText("A", forceRefresh);
    }
  } else {
    // Get time in format "01:15" or "22:15"(24 hour with leading 0)
    timeString = myTZ.dateTime("H:i");
  }

  // Only update Time if its different
  if (lastDisplayedTime != timeString) {
    // Serial.println(timeString);
    lastDisplayedTime = timeString;
    tetris.setTime(timeString, forceRefresh);
    // Must set this to false so animation knows
    // to start again
    finishedAnimating = false;
  }
}

void setupMonochrome(){
  // @todo setup tetris colors
  // 
  // you can adjust these for different block appearance, colon not supported in library atm
  // 
  // tetris.drawOutline   = true; // individual blocks outline
  // tetris.outLineColour = BLACK; // outline color
}

void handleColonAfterAnimation() {
  // It will draw the colon every time, but when the colour is black it
  // should look like its clearing it.
  uint16_t colour =  showColon ? WHITE : BLACK;
  // The x position that you draw the tetris animation object
  int x = twelveHourFormat ? xoffset : 2;
  // The y position adjusted for where the blocks will fall from
  // (this could be better!)
  int y = yoffset - (TETRIS_Y_DROP_DEFAULT * tetris.scale);
  tetris.drawColon(x, y, colour);
}

void loop() {

  if(displayIntro){
    if(finishedAnimating) displayIntro = finishedAnimating = false;
  }
  else {
    unsigned long now = millis();
    if (now > oneSecondLoopDue) {
      // We can call this often, but it will only
      // update when it needs to
      setMatrixTime();
      showColon = !showColon;
      if (finishedAnimating) {
        handleColonAfterAnimation();
      }
      oneSecondLoopDue = now + 1000;
    }
  }
  if(animateFlag){
   animationHandler();
  }

}
