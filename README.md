# 📘 Panduan Proyek CYD LVGL Arduino

## 📋 Daftar Isi
1. [Pengenalan Proyek](#pengenalan-proyek)
2. [Hardware yang Digunakan](#hardware-yang-digunakan)
3. [Setup dan Instalasi](#setup-dan-instalasi)
4. [Konfigurasi Pin](#konfigurasi-pin)
5. [Fitur Utama](#fitur-utama)
6. [Cara Menggunakan](#cara-menggunakan)
7. [Troubleshooting](#troubleshooting)

---

## 🎯 Pengenalan Proyek

**CYD LVGL Arduino** adalah proyek berbasis **ESP32** yang mengintegrasikan:
- **TFT Display** 320×240 (ILI9342) untuk tampilan visual
- **Touchscreen** XPT2046 untuk input pengguna
- **Sensor LDR** untuk membaca intensitas cahaya
- **WiFi & Cloud Connectivity** untuk sinkronisasi data
- **SD Card Support** untuk penyimpanan data lokal

Proyek ini cocok untuk:
- Dashboard IoT portabel
- Monitoring sensor real-time
- Aplikasi interaktif embedded

---

## 🔧 Hardware yang Digunakan

### 1. ESP32 Dev Board
- Microcontroller: ESP32 (Dual-core 240MHz)
- RAM: 520KB SRAM
- Flash: 4MB
- GPIO: 36 pin (configurable)
- ADC: 12-bit, resolusi 4096 step

### 2. TFT Display (ILI9342)
| Spesifikasi | Nilai |
|------------|-------|
| Resolusi | 320 × 240 pixel |
| Driver | ILI9342 |
| Interface | SPI |
| Voltage | 3.3V |

**Pin Koneksi TFT:**
| Pin TFT | GPIO ESP32 | Fungsi |
|---------|-----------|--------|
| MISO | 12 | Data In |
| MOSI | 13 | Data Out |
| SCLK | 14 | Clock |
| CS | 15 | Chip Select |
| DC | 2 | Data/Command |
| RST | - | Reset (tidak digunakan) |
| BL | 27 | Backlight Control |
| GND | GND | Ground |
| VCC | 3.3V | Power |

### 3. Touchscreen (XPT2046)
| Pin Touchscreen | GPIO ESP32 | Fungsi |
|-----------------|-----------|--------|
| CS | 33 | Chip Select |
| DIN | 13 | Data In (MOSI) |
| DOUT | 12 | Data Out (MISO) |
| CLK | 14 | Clock (SCLK) |
| GND | GND | Ground |
| VCC | 3.3V | Power |

### 4. Sensor LDR
| Pin LDR | GPIO ESP32 | Fungsi |
|---------|-----------|--------|
| Signal | 34 | ADC0 - Membaca intensitas cahaya |
| GND | GND | Ground |

### 5. SD Card Module (Optional)
| Pin SD | GPIO ESP32 | Fungsi |
|--------|-----------|--------|
| CS | 5 | Chip Select |
| MOSI | 13 | Data Out |
| MISO | 12 | Data In |
| CLK | 14 | Clock |

---

## 🚀 Setup dan Instalasi

### Prasyarat
1. **VS Code** dengan extension PlatformIO
2. **PlatformIO CLI** terinstal
3. **USB Cable** untuk upload kode

### Langkah 1: Persiapan Lingkungan

```bash
# Buka folder proyek
cd CYD_LVGL_Arduino

# Verifikasi board terhubung
pio device list
```

### Langkah 2: Instalasi Library

Library sudah terdaftar di `platformio.ini`, akan otomatis terinstall saat build:
- ✅ `bodmer/TFT_eSPI` - Driver display
- ✅ `paulstoffregen/XPT2046_Touchscreen` - Touch controller
- ✅ `bblanchon/ArduinoJson` - JSON processing

### Langkah 3: Build & Upload

```bash
# Build proyek (cek syntax)
pio run

# Upload ke board
pio run -t upload

# Monitor Serial Output
pio device monitor --baud 115200
```

---

## ⚙️ Konfigurasi Pin

### File Konfigurasi Utama

#### 1. `platformio.ini` - Build Configuration
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
    -D TFT_RGB_ORDER=TFT_RGB
    -D SPI_FREQUENCY=27000000
    -D SPI_READ_FREQUENCY=20000000
    -D SPI_TOUCH_FREQUENCY=2500000
```

**Keterangan:**
- `ILI9342_DRIVER=1` → Driver display yang digunakan
- `TFT_BL=27` → GPIO untuk backlight
- `TOUCH_CS=33` → GPIO untuk touchscreen chip select
- `SPI_FREQUENCY` → Kecepatan komunikasi (27MHz untuk display, 2.5MHz untuk touch)

#### 2. `src/main.cpp` - Pin Definition
```cpp
#define LDR_PIN 34        // Sensor LDR
#define SD_CS 5           // SD Card chip select
#define TOUCH_CS 33       // Touchscreen chip select
```

### Modifikasi Pin (Jika Diperlukan)

**Untuk mengubah pin:**

1. Edit `platformio.ini` - ubah `build_flags`
   ```ini
   -D TFT_MISO=12    # Ubah ke GPIO yang diinginkan
   ```

2. Edit `src/main.cpp` - ubah `#define`
   ```cpp
   #define LDR_PIN 34    // Ubah ke GPIO yang tersedia
   ```

3. Rebuild dan upload
   ```bash
   pio run -t upload
   ```

---

## ✨ Fitur Utama

### 1. 📊 Display TFT (320×240)
- **Fungsi:** Menampilkan informasi visual real-time
- **Update Rate:** ~200ms (5 FPS)
- **Output:** Nilai sensor, grafik, status sistem
- **Library:** TFT_eSPI (Bodmer)

**Contoh Kode:**
```cpp
#include <TFT_eSPI.h>
TFT_eSPI tft = TFT_eSPI();

void setup() {
    tft.init();
    tft.setRotation(1);
    tft.fillScreen(TFT_BLACK);
}

void loop() {
    tft.drawString("Hello World", 10, 10, 2);
}
```

### 2. 🖱️ Touchscreen Input
- **Fungsi:** Deteksi sentuhan untuk interaksi user
- **Resolusi:** 12-bit (4096 step)
- **Kalibrasi:** Ada di `test/3_kalibrasi_touchscreen.cpp`
- **Library:** XPT2046_Touchscreen

**Contoh Kode:**
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
        Serial.printf("X: %d, Y: %d\n", p.x, p.y);
    }
}
```

### 3. 💡 Sensor LDR
- **Fungsi:** Membaca intensitas cahaya
- **Pin:** GPIO 34 (ADC0)
- **Range:** 0-4095 (12-bit)
- **Mapping:** 0-4095 → 0-100% atau 100-0% (inverse)

**Contoh Kode:**
```cpp
#define LDR_PIN 34

void loop() {
    int raw = analogRead(LDR_PIN);
    int percent = map(raw, 0, 4095, 100, 0);  // Inverse
    percent = constrain(percent, 0, 100);
    Serial.printf("Light: %d%%\n", percent);
}
```

### 4. 📡 WiFi & Cloud
- **Fungsi:** Koneksi internet untuk sinkronisasi data
- **NTP Time:** Sinkronisasi waktu ke server (UTC+7 WIB)
- **HTTP Client:** Fetch data dari cloud
- **JSON Parsing:** Proses data dari API

**Konfigurasi WiFi:**
```cpp
const char* ssid = "YOUR_SSID";
const char* password = "YOUR_PASSWORD";
const char* ntpServer = "pool.ntp.org";

void connectWiFi() {
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWiFi Connected!");
}
```

### 5. 💾 SD Card Storage
- **Fungsi:** Penyimpanan data lokal
- **Format:** FAT32
- **Pin CS:** GPIO 5
- **Operasi:** Read/Write file data

**Contoh Kode:**
```cpp
#include <SD.h>
#define SD_CS 5

void setup() {
    if (!SD.begin(SD_CS)) {
        Serial.println("SD Card failed!");
    }
}

void writeToSD(String filename, String data) {
    File file = SD.open(filename, FILE_WRITE);
    if (file) {
        file.println(data);
        file.close();
    }
}
```

---

## 💻 Cara Menggunakan

### Quick Start

**1. Persiapan Hardware**
```
Koneksikan:
├── ESP32 → USB Cable → Komputer
├── TFT Display → SPI Pins (12,13,14,15,2)
├── Touchscreen → GPIO 33 (+ MISO/MOSI/SCLK shared)
├── LDR Sensor → GPIO 34 (+ GND)
└── (Optional) SD Card → GPIO 5 (+ MISO/MOSI/SCLK shared)
```

**2. Edit Konfigurasi WiFi** (jika diperlukan)
```cpp
// Di src/main.cpp
const char* ssid = "YOUR_SSID";
const char* password = "YOUR_PASSWORD";
```

**3. Build & Upload**
```bash
pio run -t upload
```

**4. Monitor Serial Output**
```bash
pio device monitor --baud 115200
```

### Testing Individual Features

Gunakan file test yang tersedia di folder `test/`:

```bash
# Test TFT Display
cd test
pio run -c platformio.ini -e esp32dev --project-dir . -c 1_test_tft.cpp

# Test Touchscreen
pio run -c platformio.ini -e esp32dev --project-dir . -c 2_test_touchscreen.cpp

# Kalibrasi Touchscreen
pio run -c platformio.ini -e esp32dev --project-dir . -c 3_kalibrasi_touchscreen.cpp

# Test LDR Sensor
pio run -c platformio.ini -e esp32dev --project-dir . -c 8_test_ldr_sensor.cpp
```

---

## 🔍 Troubleshooting

### ❌ Display Tidak Menyala

**Kemungkinan Penyebab:**
1. **Pin salah** → Verifikasi pin di `platformio.ini`
2. **Driver salah** → Pastikan `ILI9342_DRIVER=1`
3. **Backlight off** → Cek GPIO 27 dan tegangan
4. **Koneksi longgar** → Periksa kabel SPI

**Solusi:**
```cpp
// Di setup(), cek backlight
pinMode(27, OUTPUT);
digitalWrite(27, HIGH);  // Nyalakan backlight

// Cek inisialisasi
void setup() {
    Serial.begin(115200);
    delay(1000);
    tft.init();
    tft.setRotation(1);
    Serial.println("TFT Initialized");
}
```

### ❌ Touchscreen Tidak Responsif

**Kemungkinan Penyebab:**
1. **Belum dikalibrasi** → Jalankan calibration test
2. **Pin CS salah** → Verifikasi GPIO 33
3. **Library versi lama** → Update XPT2046_Touchscreen

**Solusi:**
```bash
# Jalankan kalibrasi
pio run -c platformio.ini -e esp32dev -c 3_kalibrasi_touchscreen.cpp
```

### ❌ LDR Memberikan Nilai Aneh

**Kemungkinan Penyebab:**
1. **ADC belum dikonfigurasi** → Set resolution
2. **Noise dari rangkaian** → Tambah capacitor filter
3. **Pin salah** → Verifikasi GPIO 34

**Solusi:**
```cpp
void setup() {
    analogReadResolution(12);  // Set 12-bit resolution
    analogSetPinAttenuation(ADC_11db, LDR_PIN);  // Set attenuation
}

void loop() {
    // Read multiple samples & average
    int total = 0;
    for (int i = 0; i < 10; i++) {
        total += analogRead(LDR_PIN);
        delay(10);
    }
    int raw = total / 10;
}
```

### ❌ WiFi Tidak Terhubung

**Kemungkinan Penyebab:**
1. **SSID/Password salah** → Verifikasi konfigurasi
2. **WiFi 5GHz** → Ubah ke 2.4GHz
3. **Signal lemah** → Dekat dengan router

**Solusi:**
```cpp
void connectWiFi() {
    Serial.println("Connecting to WiFi...");
    WiFi.begin(ssid, password);
    
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 20) {
        delay(500);
        Serial.print(".");
        attempts++;
    }
    
    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\nWiFi Connected!");
        Serial.print("IP: ");
        Serial.println(WiFi.localIP());
    } else {
        Serial.println("\nFailed to connect");
    }
}
```

### ❌ Upload Gagal

**Kemungkinan Penyebab:**
1. **Driver USB hilang** → Install CH340 driver
2. **Port salah** → Gunakan `pio device list` untuk cek
3. **Baud rate salah** → Sesuaikan di platformio.ini

**Solusi:**
```bash
# List port yang tersedia
pio device list

# Upload dengan verbose mode
pio run -t upload -v

# Upload ke port spesifik
pio run -t upload --upload-port COM3
```

---

## 📚 Referensi dan Resources

| Resource | Link |
|----------|------|
| TFT_eSPI Library | https://github.com/Bodmer/TFT_eSPI |
| XPT2046 Library | https://github.com/PaulStoffregen/XPT2046_Touchscreen |
| ESP32 Documentation | https://docs.espressif.com/ |
| Arduino JSON | https://arduinojson.org/ |
| PlatformIO | https://platformio.org/ |

---

## 🎓 Tips & Tricks

### 1. Optimasi Display Update
```cpp
// ❌ LAMBAT - Full screen refresh setiap loop
tft.fillScreen(TFT_BLACK);  // Jangan lakukan ini!

// ✅ CEPAT - Hanya update area yang berubah
tft.fillRect(20, 60, 280, 24, TFT_BLACK);  // Clear only bar area
tft.fillRect(22, 62, fillW, 20, TFT_GREEN);  // Draw bar
```

### 2. Reduce Memory Usage
```cpp
// Gunakan PROGMEM untuk string konstant
const char* text PROGMEM = "Hello";
Serial.println(FPSTR(text));

// Hindari String objects yang besar
char buffer[50];
sprintf(buffer, "Value: %d", value);
tft.drawString(buffer, 10, 10, 2);
```

### 3. Debugging dengan Serial
```cpp
#define DEBUG 1
#if DEBUG
    #define DEBUGLN(x) Serial.println(x)
    #define DEBUG_PRINTF(...) Serial.printf(__VA_ARGS__)
#else
    #define DEBUGLN(x)
    #define DEBUG_PRINTF(...)
#endif

// Usage
DEBUGLN("Starting setup...");
DEBUG_PRINTF("Raw ADC: %d\n", raw);
```

### 4. Kalibrasi LDR
```cpp
int raw_min = 0;     // Minimum ADC reading (dalam gelap)
int raw_max = 4095;  // Maximum ADC reading (cahaya penuh)

int calibratedPercent = map(raw, raw_min, raw_max, 0, 100);
calibratedPercent = constrain(calibratedPercent, 0, 100);
```

---

## 📞 Support & Issues

Jika menemukan masalah:
1. Cek **Serial Monitor** untuk error messages
2. Verifikasi **pin configuration** di platformio.ini
3. Test dengan **file test** yang tersedia
4. Gunakan **debug output** untuk trace code execution

---

**Last Updated:** May 2026  
**Version:** 1.0  
**Project:** CYD_LVGL_Arduino
