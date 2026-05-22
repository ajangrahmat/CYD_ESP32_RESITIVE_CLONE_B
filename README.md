# 📘 CYD LVGL Arduino

Proyek berbasis **ESP32** dengan dukungan:

* 📺 TFT Display 320×240
* 👆 Touchscreen XPT2046
* 💡 Sensor LDR
* 📡 WiFi & Cloud
* 💾 SD Card Storage

Cocok untuk:

* Dashboard IoT
* Monitoring sensor
* Human Machine Interface (HMI)
* Embedded GUI dengan LVGL

---

# 📋 Daftar Isi

1. [Hardware](#-hardware)
2. [Konfigurasi Pin](#-konfigurasi-pin)
3. [Setup Project](#-setup-project)
4. [Build & Upload](#-build--upload)
5. [Fitur](#-fitur)
6. [Testing](#-testing)
7. [Troubleshooting](#-troubleshooting)
8. [Referensi](#-referensi)

---

# 🔧 Hardware

## ESP32

| Spesifikasi | Nilai           |
| ----------- | --------------- |
| MCU         | ESP32 Dual Core |
| Clock       | 240 MHz         |
| RAM         | 520 KB SRAM     |
| Flash       | 4 MB            |
| ADC         | 12-bit          |

---

## TFT Display — ILI9342

| Spesifikasi | Nilai   |
| ----------- | ------- |
| Resolusi    | 320×240 |
| Driver      | ILI9342 |
| Interface   | SPI     |
| Tegangan    | 3.3V    |

### Koneksi TFT

| Pin TFT | GPIO ESP32 |
| ------- | ---------- |
| MISO    | 12         |
| MOSI    | 13         |
| SCLK    | 14         |
| CS      | 15         |
| DC      | 2          |
| BL      | 27         |

---

## Touchscreen — XPT2046

| Pin Touch | GPIO ESP32 |
| --------- | ---------- |
| CS        | 33         |
| DIN       | 13         |
| DOUT      | 12         |
| CLK       | 14         |

---

## Sensor LDR

| Pin    | GPIO |
| ------ | ---- |
| Signal | 34   |

---

## SD Card (Optional)

| Pin SD | GPIO ESP32 |
| ------ | ---------- |
| CS     | 5          |
| MOSI   | 13         |
| MISO   | 12         |
| CLK    | 14         |

---

# ⚙️ Konfigurasi Pin

## `platformio.ini`

```ini
[env:esp32dev]
platform = espressif32
board = esp32dev
framework = arduino

monitor_speed = 115200

lib_deps =
    bodmer/TFT_eSPI
    paulstoffregen/XPT2046_Touchscreen
    bblanchon/ArduinoJson

build_flags =
    -D USER_SETUP_LOADED=1
    -D ILI9342_DRIVER=1

    -D TFT_WIDTH=320
    -D TFT_HEIGHT=240

    -D TFT_MISO=12
    -D TFT_MOSI=13
    -D TFT_SCLK=14
    -D TFT_CS=15
    -D TFT_DC=2
    -D TFT_RST=-1

    -D TFT_BL=27
    -D TFT_BACKLIGHT_ON=1

    -D TOUCH_CS=33

    -D SPI_FREQUENCY=27000000
    -D SPI_READ_FREQUENCY=20000000
    -D SPI_TOUCH_FREQUENCY=2500000
```

---

## `src/main.cpp`

```cpp
#define LDR_PIN   34
#define SD_CS      5
#define TOUCH_CS  33
```

---

# 🚀 Setup Project

## 1. Install Software

Yang dibutuhkan:

* VS Code
* PlatformIO Extension
* Driver USB CH340 / CP210x

---

## 2. Clone Project

```bash
git clone https://github.com/username/CYD_LVGL_Arduino.git
cd CYD_LVGL_Arduino
```

---

## 3. Cek Board

```bash
pio device list
```

---

# 🔨 Build & Upload

## Build Project

```bash
pio run
```

## Upload Firmware

```bash
pio run -t upload
```

## Serial Monitor

```bash
pio device monitor --baud 115200
```

---

# ✨ Fitur

## 📺 TFT Display

Menggunakan library `TFT_eSPI`.

### Contoh

```cpp
#include <TFT_eSPI.h>

TFT_eSPI tft = TFT_eSPI();

void setup() {
    tft.init();
    tft.setRotation(1);
    tft.fillScreen(TFT_BLACK);

    tft.drawString("Hello World", 10, 10, 2);
}
```

---

## 👆 Touchscreen

Menggunakan `XPT2046_Touchscreen`.

### Contoh

```cpp
#include <XPT2046_Touchscreen.h>

XPT2046_Touchscreen ts(TOUCH_CS);

void setup() {
    ts.begin();
    ts.setRotation(1);
}

void loop() {
    if (ts.touched()) {
        TS_Point p = ts.getPoint();

        Serial.printf(
            "X: %d Y: %d\n",
            p.x,
            p.y
        );
    }
}
```

---

## 💡 Sensor LDR

### Contoh

```cpp
#define LDR_PIN 34

void loop() {
    int raw = analogRead(LDR_PIN);

    int percent = map(
        raw,
        0,
        4095,
        100,
        0
    );

    percent = constrain(percent, 0, 100);

    Serial.printf(
        "Light: %d%%\n",
        percent
    );
}
```

---

## 📡 WiFi

### Contoh

```cpp
#include <WiFi.h>

const char* ssid = "YOUR_SSID";
const char* password = "YOUR_PASSWORD";

void connectWiFi() {

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("\nWiFi Connected");
}
```

---

## 💾 SD Card

### Contoh

```cpp
#include <SD.h>

#define SD_CS 5

void setup() {

    if (!SD.begin(SD_CS)) {
        Serial.println("SD Card Failed");
        return;
    }

    Serial.println("SD Card OK");
}
```

---

# 🧪 Testing

Folder `test/` berisi beberapa contoh pengujian hardware.

## Test TFT

```bash
pio run -e esp32dev -t upload --project-dir test/1_test_tft
```

## Test Touchscreen

```bash
pio run -e esp32dev -t upload --project-dir test/2_test_touchscreen
```

## Kalibrasi Touchscreen

```bash
pio run -e esp32dev -t upload --project-dir test/3_kalibrasi_touchscreen
```

## Test LDR

```bash
pio run -e esp32dev -t upload --project-dir test/8_test_ldr_sensor
```

---

# 🔍 Troubleshooting

## ❌ Display Tidak Menyala

### Penyebab

* Pin salah
* Driver salah
* Backlight mati
* Kabel SPI longgar

### Solusi

```cpp
pinMode(27, OUTPUT);
digitalWrite(27, HIGH);
```

Pastikan:

```ini
-D ILI9342_DRIVER=1
```

---

## ❌ Touchscreen Tidak Responsif

### Solusi

* Pastikan `TOUCH_CS=33`
* Jalankan file kalibrasi
* Update library touchscreen

---

## ❌ LDR Tidak Stabil

Gunakan averaging:

```cpp
int total = 0;

for (int i = 0; i < 10; i++) {
    total += analogRead(LDR_PIN);
    delay(10);
}

int raw = total / 10;
```

---

## ❌ WiFi Tidak Connect

### Penyebab

* SSID/password salah
* Router 5GHz
* Signal lemah

### Solusi

```cpp
WiFi.begin(ssid, password);
```

Gunakan WiFi 2.4GHz.

---

## ❌ Upload Gagal

### Solusi

Cek port:

```bash
pio device list
```

Upload manual:

```bash
pio run -t upload --upload-port COM3
```

Jika gagal:

* Install driver CH340
* Ganti kabel USB
* Tekan tombol BOOT saat upload

---

# ⚡ Tips Optimasi

## Jangan Refresh Full Screen

❌ Lambat:

```cpp
tft.fillScreen(TFT_BLACK);
```

✅ Lebih cepat:

```cpp
tft.fillRect(
    20,
    60,
    280,
    24,
    TFT_BLACK
);
```

---

## Hindari String Besar

❌

```cpp
String text = "Hello";
```

✅

```cpp
char buffer[32];
sprintf(buffer, "Value: %d", value);
```

---

## Debug Macro

```cpp
#define DEBUG 1

#if DEBUG
    #define DEBUGLN(x) Serial.println(x)
#else
    #define DEBUGLN(x)
#endif
```

---

# 📚 Referensi

* [TFT_eSPI Library](https://github.com/Bodmer/TFT_eSPI?utm_source=chatgpt.com)
* [XPT2046 Touchscreen Library](https://github.com/PaulStoffregen/XPT2046_Touchscreen?utm_source=chatgpt.com)
* [ArduinoJson](https://arduinojson.org/?utm_source=chatgpt.com)
* [ESP32 Documentation](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/?utm_source=chatgpt.com)
* [PlatformIO](https://platformio.org/?utm_source=chatgpt.com)

---

# 📌 Catatan

* Gunakan power supply stabil 5V
* Hindari kabel SPI terlalu panjang
* Gunakan averaging ADC untuk hasil sensor lebih stabil
* LVGL lebih optimal jika menggunakan partial redraw

---

**Version:** 1.0
**Updated:** May 2026
**Board:** ESP32 + CYD Display
