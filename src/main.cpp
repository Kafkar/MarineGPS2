#include <Arduino.h>
#include <TFT_eSPI.h> // Hardware-specific library
#include <SPI.h>
#include <TinyGPS++.h>

TFT_eSPI tft = TFT_eSPI();       // Invoke custom library
TinyGPSPlus gps;

#define RXD2 16
#define TXD2 17

String receivedData = ""; // Define receivedData as a global variable
static int prevHour = -1;
static int prevMinute = -1;
static int prevSecond = -1;

void displayInfo() {
  //tft.fillScreen(TFT_BLACK);
  tft.setTextSize(3);
  tft.setTextColor(TFT_WHITE);

  // Draw main frame
  tft.drawRect(0, 0, tft.width(), tft.height(), TFT_WHITE);

  // Draw boxes for each value
  int boxWidth = tft.width() / 2 - 5;
  int boxHeight = tft.height() / 3 - 5;

  for (int i = 0; i < 2; i++) {
    for (int j = 0; j < 3; j++) {
      tft.drawRect(5 + i * (boxWidth + 5), 5 + j * (boxHeight + 5), boxWidth, boxHeight, TFT_WHITE);
    }
  }

  // Display UTC Time
  if (gps.time.isValid()) {
    int currentHour = gps.time.hour();
    int currentMinute = gps.time.minute();
    int currentSecond = gps.time.second();

    if (currentHour != prevHour || currentMinute != prevMinute || currentSecond != prevSecond) {
      // Clear previous time by writing it in background color
      tft.setTextColor(TFT_BLACK);
      tft.setCursor(30, 30);
      if (prevHour < 10) tft.print('0');
      tft.print(prevHour);
      tft.print(':');
      if (prevMinute < 10) tft.print('0');
      tft.print(prevMinute);
      tft.print(':');
      if (prevSecond < 10) tft.print('0');
      tft.print(prevSecond);
      
      // Write new time in white
      tft.setTextColor(TFT_WHITE);
      tft.setCursor(30, 30);
      if (currentHour < 10) tft.print('0');
      tft.print(currentHour);
      tft.print(':');
      if (currentMinute < 10) tft.print('0');
      tft.print(currentMinute);
      tft.print(':');
      if (currentSecond < 10) tft.print('0');
      tft.print(currentSecond);
    }

    // Update previous time
    prevHour = currentHour;
    prevMinute = currentMinute;
    prevSecond = currentSecond;
  }

    // Display number of satellites
  tft.setCursor(tft.width() - 40, 5);
  tft.print("Sats:");
  
  tft.fillRect(tft.width() - 20, 15, boxWidth - 20, 20, TFT_BLACK);

  tft.setCursor(tft.width() - 20, 15);
  tft.print(gps.satellites.value());

  // Display Latitude
  if (gps.location.isValid()) {
    tft.setCursor(10, boxHeight + 10);
    tft.print("Lat:");
    tft.setCursor(10, boxHeight + 30);
    tft.print(gps.location.lat(), 6);
  }

  // Display Longitude
  if (gps.location.isValid()) {
    tft.setCursor(10, 2 * boxHeight + 15);
    tft.print("Lon:");
    tft.setCursor(10, 2 * boxHeight + 35);
    tft.print(gps.location.lng(), 6);
  }

  // Display Speed
  if (gps.speed.isValid()) {
    tft.setCursor(boxWidth + 15, 10);
    tft.print("Speed:");
    tft.setCursor(boxWidth + 15, 30);
    tft.print(gps.speed.kmph());
    tft.print(" km/h");
  }

  // Display Direction
  if (gps.course.isValid()) {
    tft.setCursor(boxWidth + 15, boxHeight + 10);
    tft.print("Dir:");
    tft.setCursor(boxWidth + 15, boxHeight + 30);
    tft.print(gps.course.deg());
    tft.print(" deg");
  }
}



void setup() {
//Setup the serial port
  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);

//Setup the display
  tft.begin();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);

    // Print display sizes
  Serial.println("Display sizes:");
  Serial.print("Width: ");
  Serial.println(tft.width());
  Serial.print("Height: ");
  Serial.println(tft.height());

  // Display startup text
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextSize(3);
  tft.setTextDatum(MC_DATUM);
  tft.drawString("Kafkar GPS 2.0", tft.width() / 2, tft.height() / 2);

  // Wait for 5 seconds
  delay(5000);

  // Clear the screen
  tft.fillScreen(TFT_BLACK);
}

void loop() { //Choose Serial1 or Serial2 as required
  receivedData = ""; // Clear previous data
  while (Serial2.available()) {
    char c = Serial2.read();
    receivedData += c;
    gps.encode(c);
    Serial.print(c);
  }

   if (gps.charsProcessed() > 0) {
    displayInfo();
        // Print received data at the bottom of the screen
    tft.setTextSize(1);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setCursor(0, tft.height() - 10);
    tft.print(receivedData);
  }

}