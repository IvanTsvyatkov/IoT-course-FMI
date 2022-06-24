#define BLYNK_TEMPLATE_ID "TMPL5wPwG0RU"
#define BLYNK_DEVICE_NAME "Humidity Notifyer"
#define BLYNK_AUTH_TOKEN "QcRw2tVnDs57RM8ig6EcDIvurCgvNu1y"
#define BLYNK_PRINT Serial


#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "pic-livingroom";
char pass[] = "xxlovech72";

Adafruit_BME280 bme;
const int bmeVinPin = 12;


const float MAX_HUMIDITY_LEVEL_ALLOWED = 65.0;

bool isHighNotificationSent = false;
bool isLowNotificationSent = false;
float previousHumidity = 0.0;


void writeToLCD(float humidity) {
  if (humidity > MAX_HUMIDITY_LEVEL_ALLOWED) {
    lcd.setCursor (0, 0);
    lcd.print("Humidity reached");
    lcd.setCursor (0, 1);
    lcd.print("dangerous levels!");
  } else {
    lcd.setCursor (0, 1);
    lcd.print("Humidity: ");
    lcd.print(humidity);
    lcd.print("%");
  }
}



void writeToBlynk(float humidity)
{
  Blynk.virtualWrite(V4, humidity);

  if (humidity > MAX_HUMIDITY_LEVEL_ALLOWED && previousHumidity < humidity && !isHighNotificationSent) {
    isHighNotificationSent = true;
    Blynk.logEvent("high_humidity", String("Humidity is reaching danger high levels for people with asthma. Current humidity is: ") + humidity + String("%"));
  }

  if (humidity < MAX_HUMIDITY_LEVEL_ALLOWED && previousHumidity > MAX_HUMIDITY_LEVEL_ALLOWED && !isLowNotificationSent) {
    isLowNotificationSent = true;
    Blynk.logEvent("humidity_lowered", String("Humidity is now within normals levels. Current humidity is: ") + humidity + String("%"));
  }
}

void setup()
{
  pinMode(bmeVinPin, OUTPUT);
  digitalWrite(bmeVinPin, HIGH);
  bool status;

  status = bme.begin(0x76);
  if (!status) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while (1);
  }

  lcd.init();
  lcd.backlight();

  Serial.begin(115200);

  Blynk.begin(auth, ssid, pass);
}

void loop()
{
  float currHumidity = bme.readHumidity();
  writeToBlynk(currHumidity);

  lcd.clear();
  writeToLCD(currHumidity);

  previousHumidity = currHumidity;

  Blynk.run();
}
