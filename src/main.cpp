#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <TFT_eSPI.h>
#include <time.h>
#include <SPI.h>
#include <SD.h>

// =======================
// KONFIGURASI
// =======================
#define WIFI_SSID     "isi nama wifi kamu"
#define WIFI_PASS     "isi password wifi kamu"

#define REFRESH_MS    300000      // 5 menit
#define TIME_UPDATE_MS 1000       // 1 detik

// NTP WIB (UTC+7)
#define TIMEZONE_OFFSET 7 * 3600
#define NTP_SERVER1 "pool.ntp.org"
#define NTP_SERVER2 "time.nist.gov"

// SD Card CS pin (sesuai contoh anda)
#define SD_CS 5

TFT_eSPI tft;

float currentRate = 0;
float prevRate    = 0;
String lastUpdate = "--:--";
float history[10];
int historyCount = 0;

char timeStr[9] = "00:00:00";
char dateStr[24] = "Day, 00 Jan 0000";
bool timeSynced = false;

bool sdOk = false;      // status SD Card

// =======================
// SD Card functions
// =======================
void initSD() {
  if (!SD.begin(SD_CS)) {
    sdOk = false;
    return;
  }
  sdOk = true;
}

void saveHistoryToSD() {
  if (!sdOk) return;
  File file = SD.open("/rate_history.csv", FILE_WRITE);
  if (!file) return;
  for (int i = 0; i < historyCount; i++) {
    file.print(history[i]);
    if (i < historyCount - 1) file.print(",");
  }
  file.println();
  file.close();
}

void loadHistoryFromSD() {
  if (!sdOk) return;
  File file = SD.open("/rate_history.csv", FILE_READ);
  if (!file) return;
  String data = file.readString();
  file.close();
  data.trim();
  if (data.length() == 0) return;

  int idx = 0;
  int start = 0;
  historyCount = 0;
  while (idx < data.length() && historyCount < 100) {
    int comma = data.indexOf(',', idx);
    if (comma == -1) comma = data.length();
    String valStr = data.substring(idx, comma);
    float val = valStr.toFloat();
    if (val > 0) {
      history[historyCount++] = val;
    }
    idx = comma + 1;
    if (comma >= data.length()) break;
  }
}

// =======================
// WiFi
// =======================
void connectWiFi() {
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_CYAN);
  tft.setTextSize(2);
  tft.setCursor(80, 100);
  tft.print("Connecting WiFi");

  WiFi.begin(WIFI_SSID, WIFI_PASS);
  int dot = 0;
  while (WiFi.status() != WL_CONNECTED) {
    tft.setCursor(120 + (dot * 10), 130);
    tft.print(".");
    dot = (dot + 1) % 6;
    if (dot == 0) tft.fillRect(120, 130, 60, 20, TFT_BLACK);
    delay(500);
  }
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_GREEN);
  tft.setCursor(100, 110);
  tft.print("WiFi Connected");
  delay(1000);
}

// =======================
// NTP
// =======================
void initNTP() {
  configTime(TIMEZONE_OFFSET, 0, NTP_SERVER1, NTP_SERVER2);
  struct tm timeinfo;
  int attempts = 0;
  while (!getLocalTime(&timeinfo) && attempts < 10) {
    delay(500);
    attempts++;
  }
  if (attempts < 10) timeSynced = true;
}

void updateTimeDisplay() {
  if (!timeSynced) return;
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) return;

  // Update Jam (baris sendiri, pojok kanan, y=44)
  char newTime[9];
  strftime(newTime, sizeof(newTime), "%H:%M:%S", &timeinfo);
  if (strcmp(timeStr, newTime) != 0) {
    strcpy(timeStr, newTime);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setTextSize(1);
    tft.setCursor(260, 44);
    tft.print("        "); // hapus area
    tft.setCursor(260, 44);
    tft.print(timeStr);
  }

  // Update Tanggal (baris sendiri, kiri, y=28)
  char newDate[24];
  strftime(newDate, sizeof(newDate), "%A, %d %B %Y", &timeinfo);
  if (strcmp(dateStr, newDate) != 0) {
    strcpy(dateStr, newDate);
    tft.setTextColor(TFT_YELLOW, TFT_BLACK);
    tft.setTextSize(1);
    tft.setCursor(10, 28);
    tft.print("                                ");
    tft.setCursor(10, 28);
    tft.print(dateStr);
  }
}

// =======================
// API & Grafik
// =======================
float fetchRate() {
  WiFiClientSecure client;
  client.setInsecure();
  HTTPClient http;
  http.begin(client, "https://open.er-api.com/v6/latest/USD");
  int httpCode = http.GET();
  if (httpCode != 200) {
    http.end();
    return 0;
  }
  String payload = http.getString();
  http.end();
  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, payload);
  if (error) return 0;
  return doc["rates"]["IDR"].as<float>();
}

void addHistory(float value) {
  if (historyCount < 100) {
    history[historyCount++] = value;
  } else {
    for (int i = 0; i < 9; i++) history[i] = history[i+1];
    history[9] = value;
  }
  // Simpan ke SD card setiap ada perubahan
  saveHistoryToSD();
}

void drawMiniGraph() {
  if (historyCount < 2) return;
  int gx = 20, gy = 160, gw = 280, gh = 40;
  float minV = history[0], maxV = history[0];
  for (int i = 1; i < historyCount; i++) {
    if (history[i] < minV) minV = history[i];
    if (history[i] > maxV) maxV = history[i];
  }
  if ((maxV - minV) < 50) {
    maxV += 25;
    minV -= 25;
  }
  float stepX = (float)gw / (historyCount - 1);
  for (int i = 0; i < historyCount - 1; i++) {
    int x1 = gx + (i * stepX);
    int x2 = gx + ((i+1) * stepX);
    int y1 = gy + gh - map(history[i] * 100, minV * 100, maxV * 100, 0, gh);
    int y2 = gy + gh - map(history[i+1] * 100, minV * 100, maxV * 100, 0, gh);
    uint16_t color = (history[i+1] >= history[i]) ? TFT_RED : TFT_GREEN;
    tft.drawLine(x1, y1, x2, y2, color);
    tft.fillCircle(x2, y2, 2, color);
  }
}

// =======================
// DRAW UI (Landscape 320x240)
// =======================
void drawUI() {
  tft.fillScreen(TFT_BLACK);

  // === BARIS 1 (y=8) ===
  tft.setTextColor(TFT_CYAN);
  tft.setTextSize(2);
  tft.setCursor(10, 8);
  tft.print("USD  ->  IDR");

  tft.setTextSize(1);
  if (WiFi.status() == WL_CONNECTED) {
    tft.setTextColor(TFT_GREEN);
    tft.setCursor(250, 8);
    tft.print("WiFi OK");
  } else {
    tft.setTextColor(TFT_RED);
    tft.setCursor(250, 8);
    tft.print("No WiFi");
  }
  // Tambahan indikator SD Card
  if (sdOk) {
    tft.setTextColor(TFT_GREEN);
    tft.setCursor(180, 8);
    tft.print("SD OK");
  } else {
    tft.setTextColor(TFT_RED);
    tft.setCursor(180, 8);
    tft.print("SD Fail");
  }

  // === BARIS 2 (y=28) : Tanggal (kiri) ===
  tft.setTextColor(TFT_YELLOW);
  tft.setCursor(10, 28);
  tft.print(dateStr);

  // === BARIS 3 (y=44) : Jam (kanan) ===
  tft.setTextColor(TFT_WHITE);
  tft.setCursor(260, 44);
  tft.print(timeStr);

  // Garis pemisah setelah header
  tft.drawFastHLine(0, 58, 320, TFT_DARKGREY);

  // Nilai tukar
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(4);
  tft.setCursor(20, 85);
  tft.printf("Rp %0.0f", currentRate);

  // Perubahan
  tft.setTextSize(2);
  if (prevRate > 0) {
    float diff = currentRate - prevRate;
    if (diff >= 0) {
      tft.setTextColor(TFT_RED);
      tft.setCursor(20, 135);
      tft.printf("^ +%0.0f", diff);
    } else {
      tft.setTextColor(TFT_GREEN);
      tft.setCursor(20, 135);
      tft.printf("v %0.0f", diff);
    }
  }

  // Grafik
  drawMiniGraph();

  // Footer
  tft.drawFastHLine(0, 215, 320, TFT_DARKGREY);
  tft.setTextColor(TFT_DARKGREY);
  tft.setTextSize(1);
  tft.setCursor(10, 225);
  tft.print("Source: open.er-api.com");
  tft.setCursor(220, 225);
  tft.print("Upd: " + lastUpdate);
}

// =======================
// SETUP
// =======================
void setup() {
  Serial.begin(115200);
  tft.init();
  tft.setRotation(0);          // Landscape: 320x240
  tft.fillScreen(TFT_BLACK);

  connectWiFi();
  initNTP();
  timeSynced = true;

  // Init SD Card dan load history
  initSD();
  if (sdOk) {
    loadHistoryFromSD();
  }

  float rate = fetchRate();
  if (rate > 0) {
    currentRate = rate;
    prevRate = rate;
    addHistory(rate);  // otomatis simpan ke SD
  }

  struct tm timeinfo;
  if (getLocalTime(&timeinfo)) {
    strftime(timeStr, sizeof(timeStr), "%H:%M:%S", &timeinfo);
    strftime(dateStr, sizeof(dateStr), "%A, %d %B %Y", &timeinfo);
  }

  drawUI();
}

// =======================
// LOOP
// =======================
void loop() {
  static unsigned long lastRateTime = 0;
  static unsigned long lastTimeUpdate = 0;
  unsigned long now = millis();

  if (now - lastTimeUpdate >= TIME_UPDATE_MS) {
    lastTimeUpdate = now;
    updateTimeDisplay();
  }

  if (now - lastRateTime >= REFRESH_MS) {
    lastRateTime = now;
    if (WiFi.status() != WL_CONNECTED) WiFi.reconnect();

    prevRate = currentRate;
    float rate = fetchRate();
    if (rate > 0) {
      currentRate = rate;
      addHistory(rate);   // otomatis simpan ke SD
      struct tm timeinfo;
      if (getLocalTime(&timeinfo)) {
        char buf[6];
        strftime(buf, sizeof(buf), "%H:%M", &timeinfo);
        lastUpdate = String(buf);
      } else lastUpdate = "--:--";
      drawUI();
    }
  }
  delay(100);
}