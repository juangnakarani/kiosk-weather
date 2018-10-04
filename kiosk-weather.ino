//WeatherToday 1.0
#include "DHT.h"
#include <Wire.h>
#include "RTClib.h"
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <SD.h>


#define DHTPIN 7     // what digital pin we're connected to
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x3F, 20, 4); // set the LCD address to 0x27 for a 16 chars and 2 line display

RTC_DS1307 rtc;
File myFile;
char daysOfTheWeek[7][12] = {"mgu", "sen", "sel", "rab", "kam", "jum", "sab"};
String location = "Bakso Pionir";

void writeToDisk(String location, String datex, String timex, float t, float h) {
  Serial.println(datex);
  Serial.println(timex);
  Serial.println(t);
  Serial.println(h);
  String str = location + "," + datex + "," + timex + "," + t + "," + h;
  Serial.println(str);
  myFile = SD.open("whtoday.csv", FILE_WRITE);
  if (myFile) {

    digitalWrite(8, HIGH);   // turn the LED on (HIGH is the voltage level)
    delay(1000);                       // wait for a second
    digitalWrite(8, LOW);    // turn the LED off by making the voltage LOW

    lcd.setCursor(0, 3);
    lcd.print("Writing to csv...");
    delay(2000);
    myFile.println(str);
    myFile.close();

    lcd.setCursor(0, 3);
    lcd.print("Writing to csv done!");
    delay(1000);
  } else {
    // if the file didn't open, print an error:
    lcd.setCursor(0, 3);
    lcd.print("error writing csv");
    delay(2000);

  }
}

void setup()
{
  Serial.begin(9600);
  while (!Serial);             // Leonardo: wait for serial monitor
  Serial.println("\nWeather Today begin");

  dht.begin();
  lcd.init();                      // initialize the lcd
  //  lcd.init();
  // Print a message to the LCD.
  //  lcd.backlight();


  if (! rtc.begin()) {
    lcd.setCursor(0, 3);
    lcd.print("Couldn't find RTC");
    delay(1000);
    while (1);
  }

  if (! rtc.isrunning()) {
    lcd.setCursor(0, 3);
    lcd.print("init RTC...");
    delay(1000);
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }

  //  DateTime now = rtc.now();
  //  char bufferFileName[20] = "";
  //  //  sprintf(buffer, "%s,%s/%s/%s %02d:%02d:%02d", daysOfTheWeek[now.dayOfTheWeek()], now.day(),now.month(),now.year(), now.hour(), now.minute(), now.second());
  //  sprintf(bufferFileName, "%02d/%02d/%02d-%02d:%02d.txt", now.day(), now.month(), now.year(), now.hour(), now.minute());
  //  lcd.setCursor(0, 3);
  //  lcd.print(bufferFileName);
  //  delay(2000);

  lcd.setCursor(0, 3);
  lcd.print("init SD card...");
  delay(1000);
  if (!SD.begin(4)) {
    lcd.setCursor(0, 3);
    lcd.print("init SD failed!");
    while (1);
  } else {
    myFile = SD.open("whtoday.csv", FILE_WRITE);
    myFile.close();
    lcd.setCursor(0, 3);
    lcd.print("init SD done.");
  }

  pinMode(8, OUTPUT);
}


void loop()
{
  DateTime now = rtc.now();

  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  lcd.setCursor(0, 0);
  lcd.print(location);
  lcd.setCursor(0, 1);
  lcd.print("Suhu  :");
  lcd.setCursor(7, 1);
  lcd.print(t);
  lcd.setCursor(12, 1);
  lcd.print("*C");
  lcd.setCursor(0, 2);
  lcd.print("lembab:"); 
  lcd.setCursor(7, 2);
  lcd.print(h);
  lcd.setCursor(12, 2);
  lcd.print("%");
  char buffer[20] = "";

  //
  sprintf(buffer, "%s,%02d/%02d/%02d %02d:%02d", daysOfTheWeek[now.dayOfTheWeek()], now.day(), now.month(), now.year(), now.hour(), now.minute());
  lcd.setCursor(0, 3);
  lcd.print(buffer);

  char bufferDate[10] = "";
  char bufferTime[5] = "";
  sprintf(bufferDate, "%02d/%02d/%02d", now.day(), now.month(), now.year());
  sprintf(bufferTime, "%02d:%02d", now.hour(), now.minute());

  if (now.second() == 0) {
    writeToDisk(location, bufferDate, bufferTime, t, h);
  }
}
