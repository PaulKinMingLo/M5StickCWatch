#include <M5StickC.h>
#include "bat_icon.h"
#include "DHT12.h"
#include "Adafruit_Sensor.h"
#include "WiFi.h"

int batLevel;
int count;
uint8_t i = 0;
uint8_t mode_num = 0;
uint8_t lightToggle = 0;
bool startScan = 0;

DHT12 dht12;
RTC_DateTypeDef RTC_DateStruct;

void setup() {
  // put your setup code here, to run once:
  M5.begin();
  //init env hat module
  Wire.begin(0, 26);
  M5.Axp.ScreenBreath(8);
  M5.Lcd.setRotation(3);
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.drawXBitmap(128, 0, batIconFull, batIconWidth, batIconHeight, TFT_WHITE);
  count = 0;
}

void batLevelAnimation() {
  if (M5.Axp.GetBatChargeCurrent() > 0) {
    Serial.println("Charging...");
    if (count > 10) {
      count = 0;
    }
    switch(count) {
      case 4:
        M5.Lcd.fillRect(128, 0, 156, 9, BLACK);
        M5.Lcd.drawXBitmap(128, 0, batIcon25, batIconWidth, batIconHeight, TFT_WHITE);
        break;
      case 6:
        M5.Lcd.fillRect(128, 0, 156, 9, BLACK);
        M5.Lcd.drawXBitmap(128, 0, batIcon50, batIconWidth, batIconHeight, TFT_WHITE);
        break;
      case 8:
        M5.Lcd.fillRect(128, 0, 156, 9, BLACK);
        M5.Lcd.drawXBitmap(128, 0, batIcon75, batIconWidth, batIconHeight, TFT_WHITE);
        break;
      case 10:
        M5.Lcd.fillRect(128, 0, 156, 9, BLACK);
        M5.Lcd.drawXBitmap(128, 0, batIconFull, batIconWidth, batIconHeight, TFT_WHITE);
        break;
    }
    count++;
  } else {
    batLevel = M5.Axp.GetBatVoltage() / 5 * 100;
      if (batLevel > 80) {
        M5.Lcd.fillRect(128, 0, 156, 9, BLACK);
        M5.Lcd.drawXBitmap(128, 0, batIconFull, batIconWidth, batIconHeight, TFT_WHITE);
      } else if (batLevel > 73) {
        M5.Lcd.fillRect(128, 0, 156, 9, BLACK);
        M5.Lcd.drawXBitmap(128, 0, batIcon75, batIconWidth, batIconHeight, TFT_WHITE);
      } else if (batLevel > 70) {
        M5.Lcd.fillRect(128, 0, 156, 9, BLACK);
        M5.Lcd.drawXBitmap(128, 0, batIcon50, batIconWidth, batIconHeight, TFT_WHITE);
      } else if (batLevel > 65) {
        M5.Lcd.fillRect(128, 0, 156, 9, BLACK);
        M5.Lcd.drawXBitmap(128, 0, batIcon25, batIconWidth, batIconHeight, TFT_WHITE);
      } else {
        M5.Lcd.fillRect(128, 0, 156, 9, BLACK);
        M5.Lcd.drawXBitmap(128, 0, batIconEmpty, batIconWidth, batIconHeight, TFT_WHITE);
      }
  }
}

void screenBrightnessLv() {
  if (i <= 3) {
    i++;
  }
  if (i > 3) {
    i = 0;
  }
  //delay(200);
  switch(i) {
    case 0:
      M5.Axp.ScreenBreath(7);
      break;
    case 1:
      M5.Axp.ScreenBreath(8);
      break;
    case 2:
      M5.Axp.ScreenBreath(10);
      break;
    case 3:
      M5.Axp.ScreenBreath(12);
      break;
    default:
      M5.Axp.ScreenBreath(7);
      break;
  }
}

void displayCurrentTime() {
  M5.Rtc.GetBm8563Time();
  M5.Rtc.GetData(&RTC_DateStruct);

  char* month;
  char* weekDay;

  switch(RTC_DateStruct.Month) {
    case 0:
      month = "Jan";
      break;
    case 1:
      month = "Feb";
      break;
    case 2:
      month = "Mar";
      break;
    case 3:
      month = "Apr";
      break;
    case 4:
      month = "May";
      break;
    case 5:
      month = "Jun";
      break;
    case 6:
      month = "July";
      break;
    case 7:
      month = "Aug";
      break;
    case 8:
      month = "Sep";
      break;
    case 9:
      month = "Oct";
      break;
    case 10:
      month = "Nov";
      break;
    case 11:
      month = "Dec";
      break;
    default:
      month ="ERR";
  }

  switch(RTC_DateStruct.WeekDay) {
    case 0:
      weekDay = "Sun";
      break;
    case 1:
      weekDay = "Mon";
      break;
    case 2:
      weekDay = "Tue";
      break;
    case 3:
      weekDay = "Wed";
      break;
    case 4:
      weekDay = "Thu";
      break;
    case 5:
      weekDay = "Fri";
      break;
    case 6:
      weekDay = "Sat";
      break;
    default:
      weekDay = "ERR";
      break;
  }
  M5.Lcd.setCursor(0, 0);
  M5.Lcd.setTextSize(1);
  M5.Lcd.printf("%s, %s %02d", weekDay, month, RTC_DateStruct.Date);
  M5.Lcd.setCursor(7, 29);
  M5.Lcd.setTextSize(3);
  M5.Lcd.printf("%02d:%02d:%02d", M5.Rtc.Hour, M5.Rtc.Minute, M5.Rtc.Second);
}

void flashlight() {
  if (lightToggle == 1) {
    M5.Lcd.fillScreen(WHITE);
    lightToggle = 0;
  }
  else {
    return;
  }
}

void envSampling() {
  float tmp = dht12.readTemperature();
  float hum = dht12.readHumidity();
  M5.Lcd.setTextSize(2);
  M5.Lcd.setCursor(0, 25);
  M5.Lcd.printf("Temp: %2.1fC\nHumi: %2.0f%%", tmp, hum);
  //Serial.printf("Temp: %2.1fC\nHumi: %2.0f%%\n", tmp, hum);
}

void scanNetwork() {
  //code
  if (startScan) {
    M5.Lcd.setTextSize(1);
    M5.Lcd.setCursor(0, 0);
    M5.Lcd.print("WiFi Scan\n");
    
    int numOfNetworks = WiFi.scanNetworks();
    
    M5.Lcd.printf("No. of AP found: %d", numOfNetworks);
    for (int i = 0; i < numOfNetworks; i++) {
      Serial.print("Network name: ");
      Serial.println(WiFi.SSID(i));
      if (i == 0) {
        M5.Lcd.setCursor(5, 25);
        M5.Lcd.print("                                ");
        M5.Lcd.setCursor(5, 25);
      }
      else {
        M5.Lcd.setCursor(5, 25 + (i * 10));
        M5.Lcd.print("                                ");
        M5.Lcd.setCursor(5, 25 + (i * 10));
      }
      M5.Lcd.print("SSID: ");
      M5.Lcd.println(WiFi.SSID(i));
    }
    startScan = 0;
  }
  else {
    return;
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  if (M5.BtnB.isPressed()) {
    if (mode_num <= 3) {
      mode_num++;
    }
    if (mode_num > 3) {
      mode_num = 0;
    }
    M5.Lcd.fillScreen(BLACK);
    if (lightToggle == 0) {
      digitalWrite(M5_LED, HIGH);
    }
  }
  if (M5.BtnA.isPressed()) {
    screenBrightnessLv();
    delay(200);
  }

  switch(mode_num) {
    case 0:
      M5.Lcd.setTextSize(3);
      batLevelAnimation();
      displayCurrentTime();
      lightToggle = 1;
      break;
    case 1:
      flashlight();
      break;
    case 2:
      envSampling();
      batLevelAnimation();
      startScan = 1;
      break;
    case 3:
      scanNetwork();
      break;
    default:
      break;
  }
  
  M5.update();
  delay(250);
}
