/*******************************************************************
    Tetris clock that fetches its time Using the EzTimeLibrary

    For use with an ESP8266

    Written by Brian Lough
    YouTube: https://www.youtube.com/brianlough
    Tindie: https://www.tindie.com/stores/brianlough/
    Twitter: https://twitter.com/witnessmenow
 *******************************************************************/

// ----------------------------
// Standard Libraries - Already Installed if you have ESP32 set up
// ----------------------------

#include <ESP8266WiFi.h>

// ----------------------------
// Additional Libraries - each one of these will need to be installed.
// ----------------------------


#include <PxMatrix.h>
// The library for controlling the LED Matrix
// Can be installed from the library manager
// https://github.com/2dom/PxMatrix

// Adafruit GFX library is a dependancy for the PxMatrix Library
// Can be installed from the library manager
// https://github.com/adafruit/Adafruit-GFX-Library

#include <TetrisMatrixDraw.h>
// This library :)
// https://github.com/toblum/TetrisAnimation

#include <ezTime.h>
// Library used for getting the time and adjusting for DST
// Search for "ezTime" in the Arduino Library manager
// https://github.com/ropg/ezTime

// ---- Stuff to configure ----

// moved into settings.h for version control reasons
#include "settings.h"


// ----- Wiring -------
#define P_LAT 16
#define P_A 5
#define P_B 4
#define P_C 15
#define P_OE 2
#define P_D 12
#define P_E 0
// ---------------------

// PxMATRIX display(32,16,P_LAT, P_OE,P_A,P_B,P_C);
// PxMATRIX display(64,32,P_LAT, P_OE,P_A,P_B,P_C,P_D);
PxMATRIX display(64, 32, P_LAT, P_OE, P_A, P_B, P_C, P_D, P_E);

TetrisMatrixDraw tetris(display); // Main clock
TetrisMatrixDraw tetris2(display); // The "M" of AM/PM
TetrisMatrixDraw tetris3(display); // The "P" or "A" of AM/PM

Timezone myTZ;

void setMatrixTime() {
  static String lastDisplayedTime;
  static String lastDisplayedAmPm;

  String timeString;
  String AmPmString;
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
      tetris3.setText(AmPmString.substring(0, 1), forceRefresh);
    }
  } else {
    // Get time in format "01:15" or "22:15"(24 hour with leading 0)
    timeString = myTZ.dateTime("H:i");
  }

  // Only update Time if its different
  if (lastDisplayedTime != timeString) {
    Serial.println(timeString);
    lastDisplayedTime = timeString;
    tetris.setTime(timeString, forceRefresh);
  }
}

// declare all available animations
void animateIntro(bool showColon);
void animateTwelveHour(bool showColon);
void animateTwentyFourHour(bool showColon);

// set function pointer to the active animation
auto activeAnimation = animateIntro;

void animateIntro(bool showColon)
{
  static unsigned long first_call = millis();  // remember the time of the first call

  tetris.drawText(1, 21);

  if(millis() > first_call + 20000)   // switch to clock animation 20 seconds after first call
  {
    activeAnimation = twelveHourFormat ? animateTwelveHour : animateTwentyFourHour;
    tetris.scale = 2;
  }
}

void animateTwelveHour(bool showColon)
{
  setMatrixTime();

  // Place holders for checking are any of the tetris objects
  // currently still animating.
  bool tetris1Done = false;
  bool tetris2Done = false;
  bool tetris3Done = false;

  tetris1Done = tetris.drawNumbers(-6, 26, showColon);
  tetris2Done = tetris2.drawText(56, 25);

  // Only draw the top letter once the bottom letter is finished.
  if (tetris2Done) {
    tetris3Done = tetris3.drawText(56, 15);
  }

  tetris1Done && tetris2Done && tetris3Done;
}

void animateTwentyFourHour(bool showColon)
{
  setMatrixTime();

  tetris.drawNumbers(2, 26, showColon);
}

void drawIntro(int x = 0, int y = 0)
{
  tetris.drawChar("W", x, y, tetris.tetrisCYAN);
  tetris.drawChar("r", x + 5, y, tetris.tetrisMAGENTA);
  tetris.drawChar("i", x + 11, y, tetris.tetrisYELLOW);
  tetris.drawChar("t", x + 17, y, tetris.tetrisGREEN);
  tetris.drawChar("t", x + 22, y, tetris.tetrisBLUE);
  tetris.drawChar("e", x + 27, y, tetris.tetrisRED);
  tetris.drawChar("n", x + 32, y, tetris.tetrisWHITE);
  tetris.drawChar(" ", x + 37, y, tetris.tetrisMAGENTA);
  tetris.drawChar("b", x + 42, y, tetris.tetrisYELLOW);
  tetris.drawChar("y", x + 47, y, tetris.tetrisGREEN);
}

void setup() {
  Serial.begin(115200);

  // Attempt to connect to Wifi network:

  // Set WiFi to station mode and disconnect from an AP if it was Previously
  // connected
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED) {
    if(0 == attempts % 10) {
      Serial.println();
      Serial.print(F("Connecting to Wifi '"));
      Serial.print(ssid);
      Serial.print(F("' using password '"));
      Serial.print(password);
      Serial.print("' ");
    }
    delay(500);
    Serial.print(".");
    attempts++;
  }

  Serial.println();
  Serial.println(F("WiFi connected"));
  Serial.print(F("IP address: "));
  Serial.println(WiFi.localIP());

  // Do not set up display before WiFi connection
  // as it will crash!

  // Intialise display library
  display.begin(16);
  display.clearDisplay();
  display.display(70);

  // Setup EZ Time
  setDebug(INFO);
  waitForSync();

  Serial.println();
  Serial.println("UTC:             " + UTC.dateTime());

  myTZ.setLocation(F(MYTIMEZONE));
  Serial.print(F("Time in your set timezone:         "));
  Serial.println(myTZ.dateTime());

   //"Powered By"
  drawIntro(6, 12);
  unsigned long now = millis();
  unsigned long start_time = now;
  while(now < start_time + 2000) {
    now = millis();
    if(0 == now % (unsigned long)2)
    {
      display.display(70);
    }
  }

  // Start the Animation Timer
  tetris.setText("B. LOUGH");
}

void loop()
{
  static unsigned int colonCounter = 0;
  unsigned long now = millis();

  if(0 == now % (unsigned long)100)
  {
    colonCounter++;
    unsigned int colonFraction = colonCounter / 5;
    bool showColon = colonFraction % 2;
    display.clearDisplay();
    activeAnimation(showColon);
  }

  if(0 == now % (unsigned long)2)
  {
    display.display(70);
  }
}
