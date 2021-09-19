#ifndef TETRIS_CLOCK_SETTINGS
#define TETRIS_CLOCK_SETTINGS
// Initialize Wifi connection to the router
char ssid[] = "SSID";     // your network SSID (name)
char password[] = "password"; // your network key

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

#endif  // TETRIS_CLOCK_SETTINGS
