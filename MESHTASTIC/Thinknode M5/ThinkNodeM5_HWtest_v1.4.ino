/**
 * @file    ThinkNodeM5_HWTest.ino
 * @brief   Hardware self-test for the Elecrow ThinkNode M5 (ESP32-S3FN8).
 *
 * @details Tests all on-board peripherals in sequence at startup, then
 *          enters an interactive info-browser navigated via the two buttons.
 *
 *          Peripherals tested:
 *            - PCA9557  GPIO expander  (I2C0 — GPIO48/47)
 *            - PCF8563  RTC            (I2C1 — GPIO2/1)
 *            - SX1262   LoRa           (FSPI — GPIO16/7/15/17)
 *            - GooDisplay 1.54" EPD    (HSPI — GPIO38/45/39)
 *            - Quectel L76K GPS        (UART1 — GPIO20/19)
 *            - Passive buzzer          (GPIO9  LEDC)
 *            - Button 1 / Button 2     (GPIO21 / GPIO14)
 *            - Battery ADC             (GPIO8  ADC1_CH7)
 *
 *          Navigation (post-test loop):
 *            BTN1 (GPIO21) — next page
 *            BTN2 (GPIO14) — previous page  /  LoRa ping on page 3
 *
 *          Pages:
 *            0  Summary
 *            1  PCA9557
 *            2  RTC PCF8563
 *            3  LoRa SX1262  (BTN2 triggers ping-pong)
 *            4  GPS L76K
 *            5  Battery ADC
 *
 * @note    Pinout source:
 *          meshtastic/firmware  variants/esp32s3/ELECROW-ThinkNode-M5/variant.h
 *          IMPORTANT: variant.h lists I2C on GPIO1/2. Physical measurement on
 *          this PCB revision shows PCA9557 on GPIO47/48 and RTC on GPIO1/2.
 *          Two independent I2C buses are required.
 *
 * @note    EPD rotation: adjust EPD_ROTATION (0-3) if display is sideways.
 *          0=0deg  1=90CW  2=180  3=270CW
 *
 * @note    LoRa frequency: set LORA_FREQ_MHZ to 868.0 for EU_868 region.
 *
 * Required libraries (Arduino Library Manager):
 *   RadioLib    >= 6.6.0   by Jan Gromes
 *   GxEPD2      >= 1.5.8   by Jean-Marc Zingg
 *   TinyGPSPlus >= 1.0.3   by Mikal Hart
 *   RTClib      >= 2.1.4   by Adafruit
 *
 * Board settings (Arduino IDE):
 *   Board             : ESP32S3 Dev Module
 *   Flash Size        : 8MB (64Mb)
 *   USB CDC on Boot   : Disabled  (serial via CH340K)
 *   PSRAM             : Disabled
 *   CPU Frequency     : 240MHz
 *
 * @author  Electgpl
 * @date    2026-02-27
 * @version 1.4
 */

// ============================================================
// INCLUDES
// ============================================================
#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include "RTClib.h"
#include <RadioLib.h>
#include <GxEPD2_BW.h>
#include <Fonts/FreeMonoBold9pt7b.h>   // header font  (~14px tall)
#include <Fonts/FreeMono9pt7b.h>       // body font    (~12px tall, narrower)
#include <TinyGPSPlus.h>

// ============================================================
// USER CONFIGURATION
// ============================================================

/** @brief EPD display rotation. Try 0,1,2,3 until text reads upright. */
#define EPD_ROTATION    0

/** @brief LoRa centre frequency in MHz. Use 868.0 for EU_868. */
#define LORA_FREQ_MHZ   915.0f

// ============================================================
// PCA9557 REGISTER MAP  (NXP UM10318)
// ============================================================
#define PCA9557_ADDR    0x18  ///< I2C address (A2=A1=A0=GND)
#define PCA9557_REG_IN  0x00  ///< Input  port register (read-only)
#define PCA9557_REG_OUT 0x01  ///< Output port register
#define PCA9557_REG_CFG 0x03  ///< Config register  0=output 1=input

// ============================================================
// PIN DEFINITIONS  (verified by PCB trace measurement)
// ============================================================

// I2C bus 0 — PCA9557  (variant.h lists 1/2, PCB measurement: 47/48)
#define PIN_I2C_SCL     47
#define PIN_I2C_SDA     48

// I2C bus 1 — PCF8563 RTC  (variant.h: 1/2, confirmed by measurement)
#define PIN_RTC_SCL      1
#define PIN_RTC_SDA      2

// PCA9557 internal pin numbers (P0..P7)
#define PCA_LED_USER     1   ///< Blue LED
#define PCA_LED_ENABLE   2   ///< LED rail power (OR with VBUS)
#define PCA_LED_POWER    3   ///< Red LED (hardware blink while charging)
#define PCA_POWER_EN     4   ///< Main peripheral power enable
#define PCA_EINK_EN      5   ///< EPD backlight enable

// Power / ADC
#define PIN_BATTERY_ADC  8   ///< ADC1_CH7 — Li-Ion voltage divider
#define PIN_VBUS_DETECT  12  ///< HIGH when USB 5V present
#define ADC_MULTIPLIER   2.11f  ///< Empirical divider correction (variant.h)

// Buzzer
#define PIN_BUZZER       9   ///< Passive buzzer — driven by LEDC

// Buttons (active-low, internal pull-up)
#define PIN_BTN1         21  ///< Next page
#define PIN_BTN2         14  ///< Previous page / LoRa ping

// GPS — Quectel L76K
#define PIN_GPS_SWITCH   10  ///< GPS power switch
#define PIN_GPS_RESET    13  ///< Active-low reset (>100 ms per datasheet)
#define PIN_GPS_STANDBY  11  ///< LOW=allow sleep, HIGH=force wake
#define PIN_GPS_TX_CPU   20  ///< UART1 RX on ESP side (GPS → CPU)
#define PIN_GPS_RX_CPU   19  ///< UART1 TX on ESP side (CPU → GPS)
#define GPS_BAUD         9600

// SX1262 — FSPI bus
#define PIN_LORA_SCK     16
#define PIN_LORA_MISO     7
#define PIN_LORA_MOSI    15
#define PIN_LORA_CS      17
#define PIN_LORA_RST      6
#define PIN_LORA_BUSY     5
#define PIN_LORA_DIO1     4
#define PIN_LORA_PWR_EN  46  ///< External LDO enable for SX1262

// EPD GooDisplay 1.54" — HSPI bus (independent from LoRa SPI)
#define PIN_EPD_CS       39
#define PIN_EPD_BUSY     42
#define PIN_EPD_DC       40
#define PIN_EPD_RST      41
#define PIN_EPD_SCK      38
#define PIN_EPD_MOSI     45  ///< EPD SDI — MISO not used (write-only)

// ============================================================
// GLOBAL OBJECTS
// ============================================================

SPIClass spi_lora(FSPI);   ///< SPI bus for SX1262
SPIClass spi_epd(HSPI);    ///< SPI bus for EPD (separate from LoRa)

/// SX1262: constructor (cs, dio1, reset, busy, &spi)
SX1262 radio = new Module(PIN_LORA_CS, PIN_LORA_DIO1,
                           PIN_LORA_RST, PIN_LORA_BUSY, spi_lora);

/// EPD: GxEPD2_154_D67 = GDEH0154D67 (1.54" BW).
/// Try GxEPD2_154_M09 if display shows artefacts.
GxEPD2_BW<GxEPD2_154_D67, GxEPD2_154_D67::HEIGHT> display(
  GxEPD2_154_D67(PIN_EPD_CS, PIN_EPD_DC, PIN_EPD_RST, PIN_EPD_BUSY)
);

TinyGPSPlus       gps;
HardwareSerial    gpsSerial(1);          ///< UART1 for L76K
RTC_PCF8563       rtc;
TwoWire           Wire1_rtc = TwoWire(1); ///< Second I2C bus for RTC

// ============================================================
// TEST RESULT STRUCTURE
// ============================================================

/** @brief Holds pass/fail result for each peripheral test. */
struct TestResult {
  bool pca9557  = false;
  bool rtc      = false;
  bool lora     = false;
  bool epd      = false;
  bool gps_uart = false; ///< NMEA frames received
  bool gps_fix  = false; ///< Valid position fix obtained
  bool buzzer   = false;
  bool buttons  = false;
  bool battery  = false;
};
TestResult results;

// ============================================================
// PERIPHERAL INFO STRUCTURES  (populated during tests)
// ============================================================

/** @brief LoRa test metrics and ping-pong statistics. */
struct LoraInfo {
  int   init_state  = -999;
  bool  tx_ok       = false;
  float freq        = LORA_FREQ_MHZ;
  int   ping_count  = 0;
  int   pong_count  = 0;
  float last_rssi   = 0.0f;
  float last_snr    = 0.0f;
  int   last_tx_state = -999;
};

/** @brief GPS measurement data. */
struct GpsInfo {
  uint32_t chars = 0;
  uint8_t  sats  = 0;
  float    lat   = 0.0f;
  float    lon   = 0.0f;
  float    alt   = 0.0f;
};

/** @brief Battery ADC measurement data. */
struct BatInfo {
  uint32_t raw  = 0;
  float    vadc = 0.0f;
  float    vbat = 0.0f;
  int      pct  = 0;
};

/** @brief RTC date/time snapshot. */
struct RtcInfo {
  int  y=0, mo=0, d=0, h=0, mi=0, s=0;
  bool lost = false; ///< true if RTC lost power (coin cell dead)
};

LoraInfo lora_info;
GpsInfo  gps_info;
BatInfo  bat_info;
RtcInfo  rtc_info;

// ============================================================
// NAVIGATION STATE
// ============================================================
#define PAGE_COUNT  6
int current_page = 0;

// ============================================================
// PCA9557 LOW-LEVEL DRIVER  (direct Wire, no external library)
// ============================================================

uint8_t pca_out = 0x00; ///< Shadow copy of PCA9557 output register

/**
 * @brief Initialise PCA9557: set P1-P5 as outputs, P0/P6/P7 as inputs.
 * @return true if I2C transaction succeeded.
 */
bool pca_init() {
  // Config register: bit=0 → output, bit=1 → input
  // P0=in(1)  P1..P5=out(0)  P6=in(1)  P7=in(1)  → 0b11000001 = 0xC1
  Wire.beginTransmission(PCA9557_ADDR);
  Wire.write(PCA9557_REG_CFG);
  Wire.write(0xC1);
  if (Wire.endTransmission() != 0) return false;

  pca_out = 0x00;
  Wire.beginTransmission(PCA9557_ADDR);
  Wire.write(PCA9557_REG_OUT);
  Wire.write(pca_out);
  return (Wire.endTransmission() == 0);
}

/**
 * @brief Write a single pin on the PCA9557 expander.
 * @param pin   Pin number (0-7) on the expander.
 * @param val   true = HIGH, false = LOW.
 */
void pca_pin_write(uint8_t pin, bool val) {
  if (val) pca_out |=  (1 << pin);
  else     pca_out &= ~(1 << pin);
  Wire.beginTransmission(PCA9557_ADDR);
  Wire.write(PCA9557_REG_OUT);
  Wire.write(pca_out);
  Wire.endTransmission();
}

// ============================================================
// BUZZER  (ESP32 Arduino Core v3.x LEDC API)
// ============================================================

/**
 * @brief Play a tone on the buzzer for a given duration.
 * @param freq_hz   Frequency in Hz. 0 = silence.
 * @param ms        Duration in milliseconds.
 */
void buzzer_tone(uint16_t freq_hz, uint16_t ms) {
  ledcWriteTone(PIN_BUZZER, freq_hz);
  delay(ms);
  ledcWriteTone(PIN_BUZZER, 0);
}

/** @brief Three-note ascending melody — played on overall PASS. */
void buzzer_ok() {
  buzzer_tone(1047, 80); delay(20);
  buzzer_tone(1319, 80); delay(20);
  buzzer_tone(1568, 120);
}

/** @brief Two-note descending tone — played on FAIL. */
void buzzer_fail() {
  buzzer_tone(400, 200); delay(50);
  buzzer_tone(300, 350);
}

/** @brief Short click feedback for button presses. */
void buzzer_click() { buzzer_tone(1200, 40); }

// ============================================================
// EPD RENDERING HELPERS
// ============================================================

/**
 * @brief Apply rotation and set the header font (FreeMonoBold9pt7b).
 *        Must be called before drawing page content.
 */
void epd_begin() {
  display.setRotation(EPD_ROTATION);
  display.setFont(&FreeMonoBold9pt7b);
  display.setTextColor(GxEPD_BLACK);
}

/**
 * @brief Draw the standard page header: title + page index + separator line.
 * @param title   Title string (max ~14 chars in FreeMonoBold9pt7b).
 * @param pg      Zero-based page index.
 */
void epd_header(const char* title, int pg) {
  display.fillScreen(GxEPD_WHITE);
  epd_begin();                          // bold 9pt for header
  display.setCursor(4, 16);
  display.print(title);
  char buf[8];
  snprintf(buf, 8, "%d/%d", pg + 1, PAGE_COUNT);
  display.setCursor(162, 16);
  display.print(buf);
  display.drawLine(0, 21, 200, 21, GxEPD_BLACK);
  // Switch to smaller body font for all rows below the header
  display.setFont(&FreeMono9pt7b);
}

/**
 * @brief Draw one data row: label at x=4, value at x=100.
 * @param y     Baseline Y coordinate.
 * @param lbl   Left column string (label).
 * @param val   Right column string (value).
 */
void epd_row(int y, const char* lbl, const char* val) {
  display.setCursor(4,   y); display.print(lbl);
  display.setCursor(100, y); display.print(val);
}

// ============================================================
// INFO PAGES
// ============================================================

/** @brief Page 0 — overall pass/fail summary of all tests. */
void page_summary() {
  epd_header("HW Test  v1.4", 0);
  int y = 36;
  epd_row(y, "PCA9557:",  results.pca9557  ? "OK" : "FAIL"); y += 17;
  epd_row(y, "RTC:",      results.rtc      ? "OK" : "FAIL"); y += 17;
  epd_row(y, "LoRa:",     results.lora     ? "OK" : "FAIL"); y += 17;
  epd_row(y, "EPD:",      results.epd      ? "OK" : "FAIL"); y += 17;
  epd_row(y, "GPS UART:", results.gps_uart ? "OK" : "FAIL"); y += 17;
  epd_row(y, "Buzzer:",   results.buzzer   ? "OK" : "----"); y += 17;
  epd_row(y, "Buttons:",  results.buttons  ? "OK" : "WARN"); y += 17;
  epd_row(y, "Battery:",  results.battery  ? "OK" : "FAIL");
}

/** @brief Page 1 — PCA9557 GPIO expander details. */
void page_pca() {
  char buf[12];
  epd_header("PCA9557", 1);
  int y = 36;
  epd_row(y, "Addr:",    "0x18");                             y += 17;
  epd_row(y, "Status:",  results.pca9557 ? "OK" : "FAIL");   y += 17;
  epd_row(y, "I2C0:",    "GPIO48/47");                        y += 17;
  epd_row(y, "PWR_EN:",  "P4");                               y += 17;
  epd_row(y, "EINK:",    "P5");                               y += 17;
  epd_row(y, "LED_BLU:", "P1");                               y += 17;
  epd_row(y, "LED_RED:", "P3");                               y += 17;
  snprintf(buf, 12, "0x%02X", pca_out);
  epd_row(y, "OUT reg:", buf);
}

/** @brief Page 2 — PCF8563 RTC current date/time. */
void page_rtc() {
  char buf[20];
  epd_header("PCF8563  RTC", 2);
  int y = 36;
  epd_row(y, "Addr:",    "0x51");                           y += 17;
  epd_row(y, "Status:",  results.rtc ? "OK" : "FAIL");     y += 17;
  epd_row(y, "I2C1:",    "GPIO2/1");                        y += 17;
  snprintf(buf, 20, "%04d-%02d-%02d", rtc_info.y, rtc_info.mo, rtc_info.d);
  epd_row(y, "Date:",    buf);                              y += 16;
  snprintf(buf, 20, "%02d:%02d:%02d", rtc_info.h, rtc_info.mi, rtc_info.s);
  epd_row(y, "Time:",    buf);                              y += 17;
  epd_row(y, "PwrLoss:", rtc_info.lost ? "YES" : "NO");    y += 17;
  epd_row(y, "Bus clk:", "400 kHz");
}

/** @brief Page 3 — SX1262 LoRa config and ping-pong results. */
void page_lora() {
  char buf[20];
  epd_header("SX1262  LoRa", 3);
  int y = 36;
  snprintf(buf, 20, "%.1f MHz", lora_info.freq);
  epd_row(y, "Freq:",    buf);                                        y += 17;
  epd_row(y, "Init:",    lora_info.init_state == 0 ? "OK" : "FAIL"); y += 17;
  epd_row(y, "TX test:", lora_info.tx_ok ? "OK" : "FAIL");           y += 17;
  snprintf(buf, 20, "%d sent", lora_info.ping_count);
  epd_row(y, "Pings:",   buf);                                        y += 17;
  snprintf(buf, 20, "%d recv", lora_info.pong_count);
  epd_row(y, "Pongs:",   buf);                                        y += 17;
  if (lora_info.pong_count > 0) {
    snprintf(buf, 20, "%.1f dBm", lora_info.last_rssi);
    epd_row(y, "RSSI:",  buf);                                        y += 17;
    snprintf(buf, 20, "%.1f dB",  lora_info.last_snr);
    epd_row(y, "SNR:",   buf);
  } else {
    epd_row(y, "BTN2:",  "send ping");                                y += 16;
    epd_row(y, "Mode:",  "SF12 CR4/8");
  }
}

/** @brief Page 4 — GPS L76K NMEA / fix status. */
void page_gps() {
  char buf[20];
  epd_header("L76K  GPS", 4);
  int y = 36;
  epd_row(y, "UART1:",   "9600 8N1");                       y += 17;
  epd_row(y, "RX/TX:",   "IO20/19");                        y += 17;
  epd_row(y, "NMEA:",    results.gps_uart ? "OK" : "FAIL"); y += 17;
  snprintf(buf, 20, "%lu ch", gps_info.chars);
  epd_row(y, "Chars:",   buf);                              y += 17;
  snprintf(buf, 20, "%d", gps_info.sats);
  epd_row(y, "Sats:",    buf);                              y += 17;
  if (results.gps_fix) {
    snprintf(buf, 20, "%.4f", gps_info.lat);
    epd_row(y, "Lat:",   buf);                              y += 17;
    snprintf(buf, 20, "%.4f", gps_info.lon);
    epd_row(y, "Lon:",   buf);
  } else {
    epd_row(y, "Fix:",   "NO FIX");                         y += 17;
    epd_row(y, "Tip:",   "go outdoors");
  }
}

/** @brief Page 5 — Battery ADC voltage and charge estimate. */
void page_battery() {
  char buf[20];
  epd_header("Battery  ADC", 5);
  int y = 36;
  epd_row(y, "GPIO:",    "GPIO8 ADC");                      y += 17;
  snprintf(buf, 20, "x %.2f", ADC_MULTIPLIER);
  epd_row(y, "Divider:", buf);                              y += 17;
  snprintf(buf, 20, "%lu", bat_info.raw);
  epd_row(y, "ADC raw:", buf);                              y += 17;
  snprintf(buf, 20, "%.3f V", bat_info.vadc);
  epd_row(y, "Vadc:",    buf);                              y += 17;
  snprintf(buf, 20, "%.2f V", bat_info.vbat);
  epd_row(y, "Vbat:",    buf);                              y += 17;
  snprintf(buf, 20, "%d %%", bat_info.pct);
  epd_row(y, "Level:",   buf);                              y += 17;
  epd_row(y, "Status:",  results.battery ? "OK" : "RANGE!");
}

// ============================================================
// PAGE DISPATCH TABLE
// ============================================================
typedef void (*PageFunc)();
PageFunc pages[PAGE_COUNT] = {
  page_summary, page_pca, page_rtc, page_lora, page_gps, page_battery
};

/**
 * @brief Render the given page index on the EPD (full refresh).
 *        Turns backlight on; caller is responsible for turning it off.
 * @param p  Page index (0..PAGE_COUNT-1).
 */
void show_page(int p) {
  pca_pin_write(PCA_EINK_EN, true);
  epd_begin();
  display.setFullWindow();
  display.firstPage();
  do { pages[p](); } while (display.nextPage());
}

// ============================================================
// LORA PING-PONG TEST
// ============================================================

/**
 * @brief Transmit a PING packet with maximum-range settings and wait for PONG.
 *
 * @details Radio is reconfigured to SF12/BW125/CR4-8/22dBm/preamble-16
 *          (link budget ~157 dB, sensitivity ~-137 dBm).
 *          After TX the function blocks in RX for 10 seconds.
 *          Results (RSSI, SNR) are stored in lora_info and displayed on EPD.
 *          Radio is restored to SF9/BW125/CR4-7 after the exchange.
 *
 * @note    The companion sketch LoRa_PONG_Responder.ino must be running on
 *          the second node. It echoes any packet starting with "PING"
 *          with a "PONG" reply after a 50 ms guard time.
 *
 * @note    SX1262 DIO3 TCXO voltage = 3.3V (hardware requirement).
 *          DIO2 configured as RF switch (hardware requirement).
 */
void lora_ping_pong() {
  if (lora_info.init_state != RADIOLIB_ERR_NONE) {
    Serial.println("[LORA] Not initialised — aborting ping");
    return;
  }

  // Reconfigure for maximum range
  radio.setSpreadingFactor(12);
  radio.setBandwidth(125.0);
  radio.setCodingRate(8);       // 4/8
  radio.setPreambleLength(16);
  radio.setOutputPower(22);
  lora_info.ping_count++;

  Serial.printf("[LORA] TX PING #%d  SF12 BW125 CR4/8 22dBm\n",
                lora_info.ping_count);

  // Show TX screen on EPD
  pca_pin_write(PCA_EINK_EN, true);
  display.setRotation(EPD_ROTATION);
  display.setFullWindow(); display.firstPage();
  do {
    display.fillScreen(GxEPD_WHITE);
    display.setFont(&FreeMonoBold9pt7b);
    display.setTextColor(GxEPD_BLACK);
    display.setCursor(4, 16); display.print("LoRa Ping-Pong");
    display.drawLine(0, 21, 200, 21, GxEPD_BLACK);
    display.setFont(&FreeMono9pt7b);
    display.setCursor(4, 42);  display.print("SF12 BW125 CR4/8");
    display.setCursor(4, 59);  display.print("22 dBm  915 MHz");
    char buf[24];
    snprintf(buf, 24, "TX #%d ...", lora_info.ping_count);
    display.setCursor(4, 76);  display.print(buf);
    display.setCursor(4, 93);  display.print("Waiting 10s...");
  } while(display.nextPage());
  delay(1500); pca_pin_write(PCA_EINK_EN, false);

  // Build PING packet: "PING" + 2-byte seq + 4-byte timestamp + 0xDEAD
  uint8_t pkt[12];
  pkt[0]='P'; pkt[1]='I'; pkt[2]='N'; pkt[3]='G';
  pkt[4] = (lora_info.ping_count >> 8) & 0xFF;
  pkt[5] =  lora_info.ping_count       & 0xFF;
  uint32_t ts = millis();
  pkt[6]=(ts>>24)&0xFF; pkt[7]=(ts>>16)&0xFF;
  pkt[8]=(ts>> 8)&0xFF; pkt[9]= ts     &0xFF;
  pkt[10]=0xDE; pkt[11]=0xAD;

  buzzer_tone(800, 80);
  lora_info.last_tx_state = radio.transmit(pkt, sizeof(pkt));

  if (lora_info.last_tx_state != RADIOLIB_ERR_NONE) {
    Serial.printf("[LORA] TX FAIL: state=%d\n", lora_info.last_tx_state);
    buzzer_fail();
    lora_info.last_rssi = 0; lora_info.last_snr = 0;
    goto restore_config;
  }

  Serial.println("[LORA] TX OK — waiting PONG 10s...");
  buzzer_tone(1200, 60);

  {
    // RX with 10-second timeout
    uint8_t rx_buf[64];
    int rx_state = radio.receive(rx_buf, sizeof(rx_buf), 10000);

    if (rx_state == RADIOLIB_ERR_NONE) {
      lora_info.pong_count++;
      lora_info.last_rssi = radio.getRSSI();
      lora_info.last_snr  = radio.getSNR();
      int pkt_len = radio.getPacketLength();

      Serial.printf("[LORA] PONG #%d  RSSI=%.1f dBm  SNR=%.1f dB  len=%d\n",
                    lora_info.pong_count, lora_info.last_rssi,
                    lora_info.last_snr, pkt_len);
      Serial.print("[LORA] Payload: ");
      for (int i = 0; i < min(pkt_len, 12); i++)
        Serial.printf("%02X ", rx_buf[i]);
      Serial.println();

      buzzer_ok();

      pca_pin_write(PCA_EINK_EN, true);
      display.setFullWindow(); display.firstPage();
      do {
        display.fillScreen(GxEPD_WHITE);
        display.setFont(&FreeMonoBold9pt7b);
        display.setTextColor(GxEPD_BLACK);
        display.setCursor(4, 16); display.print("PONG received!");
        display.drawLine(0, 21, 200, 21, GxEPD_BLACK);
        display.setFont(&FreeMono9pt7b);
        char buf[22];
        snprintf(buf, 22, "%.1f dBm", lora_info.last_rssi);
        display.setCursor(4, 42); display.print("RSSI:"); display.setCursor(70,42); display.print(buf);
        snprintf(buf, 22, "%.1f dB",  lora_info.last_snr);
        display.setCursor(4, 59); display.print("SNR: "); display.setCursor(70,59); display.print(buf);
        snprintf(buf, 22, "%d / %d", lora_info.pong_count, lora_info.ping_count);
        display.setCursor(4, 76); display.print("RX/TX:"); display.setCursor(70,76); display.print(buf);
        snprintf(buf, 22, "%d bytes", pkt_len);
        display.setCursor(4, 93); display.print("Len: "); display.setCursor(70,93); display.print(buf);
        display.setCursor(4,118); display.print("BTN2 = new ping");
      } while(display.nextPage());
      delay(3000); pca_pin_write(PCA_EINK_EN, false);

    } else if (rx_state == RADIOLIB_ERR_RX_TIMEOUT) {
      Serial.println("[LORA] RX timeout — no response in 10s");
      buzzer_tone(600, 200);

      pca_pin_write(PCA_EINK_EN, true);
      display.setFullWindow(); display.firstPage();
      do {
        display.fillScreen(GxEPD_WHITE);
        display.setFont(&FreeMonoBold9pt7b);
        display.setTextColor(GxEPD_BLACK);
        display.setCursor(4, 16); display.print("No response");
        display.drawLine(0, 21, 200, 21, GxEPD_BLACK);
        display.setFont(&FreeMono9pt7b);
        display.setCursor(4, 42); display.print("TX OK, RX timeout");
        display.setCursor(4, 59); display.print("after 10 seconds");
        char buf[22];
        snprintf(buf, 22, "Pings sent: %d", lora_info.ping_count);
        display.setCursor(4, 76); display.print(buf);
        display.setCursor(4,101); display.print("Check other node");
        display.setCursor(4,118); display.print("BTN2 = retry");
      } while(display.nextPage());
      delay(3000); pca_pin_write(PCA_EINK_EN, false);

    } else {
      Serial.printf("[LORA] RX error: state=%d\n", rx_state);
      buzzer_fail();
    }
  }

restore_config:
  // Restore default test configuration
  radio.setSpreadingFactor(9);
  radio.setBandwidth(125.0);
  radio.setCodingRate(7);   // 4/7
  radio.setPreambleLength(8);
}

// ============================================================
// HARDWARE TESTS
// ============================================================

/**
 * @brief Test PCA9557 GPIO expander.
 * @details Probes I2C address 0x18, attempts bus recovery on failure,
 *          initialises the chip, enables peripheral power (P4) and
 *          blinks the blue LED as visual confirmation.
 */
void test_pca9557() {
  Serial.println("\n--- TEST PCA9557 ---");

  Wire.beginTransmission(PCA9557_ADDR);
  uint8_t err = Wire.endTransmission();
  Serial.printf("  I2C probe 0x18: err=%d\n", err);

  if (err != 0) {
    // I2C bus recovery: 9 SCL pulses to release a stuck slave
    Serial.println("  WARN: attempting I2C bus recovery (9 SCL pulses)...");
    Wire.end();
    delay(10);
    pinMode(PIN_I2C_SCL, OUTPUT);
    pinMode(PIN_I2C_SDA, INPUT_PULLUP);
    for (int i = 0; i < 9; i++) {
      digitalWrite(PIN_I2C_SCL, LOW);  delayMicroseconds(5);
      digitalWrite(PIN_I2C_SCL, HIGH); delayMicroseconds(5);
    }
    // Generate STOP condition
    pinMode(PIN_I2C_SDA, OUTPUT);
    digitalWrite(PIN_I2C_SDA, LOW);  delayMicroseconds(5);
    digitalWrite(PIN_I2C_SCL, HIGH); delayMicroseconds(5);
    digitalWrite(PIN_I2C_SDA, HIGH); delayMicroseconds(5);
    delay(10);
    Wire.begin(PIN_I2C_SDA, PIN_I2C_SCL);
    Wire.setClock(400000);
    delay(50);

    Wire.beginTransmission(PCA9557_ADDR);
    err = Wire.endTransmission();
    Serial.printf("  Post-recovery probe: err=%d\n", err);
  }

  if (err != 0) {
    Serial.println("FAIL: PCA9557 not found at 0x18");
    results.pca9557 = false;
    return;
  }

  results.pca9557 = pca_init();
  if (!results.pca9557) { Serial.println("FAIL: pca_init() error"); return; }

  Serial.println("OK: PCA9557 responded");

  // Enable all peripheral power and LED rail
  pca_pin_write(PCA_POWER_EN,   true);
  pca_pin_write(PCA_LED_ENABLE, true);
  delay(50);

  // Blue LED blink — visual confirmation
  pca_pin_write(PCA_LED_USER, true);  delay(400);
  pca_pin_write(PCA_LED_USER, false);

  // Red LED — skip if USB present (hardware blink during charging)
  if (digitalRead(PIN_VBUS_DETECT) == LOW) {
    pca_pin_write(PCA_LED_POWER, true);  delay(400);
    pca_pin_write(PCA_LED_POWER, false);
  }
}

/**
 * @brief Test PCF8563 RTC via I2C bus 1.
 * @details If power-loss flag is set, adjusts RTC to compile time.
 *          Stores date/time in rtc_info for display.
 */
void test_rtc() {
  Serial.println("\n--- TEST RTC PCF8563 ---");
  if (!rtc.begin(&Wire1_rtc)) {
    Serial.println("FAIL: PCF8563 not found on Wire1 (GPIO1/GPIO2)");
    return;
  }
  rtc_info.lost = rtc.lostPower();
  if (rtc_info.lost) {
    Serial.println("  WARN: RTC lost power — setting compile time");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
  DateTime now    = rtc.now();
  rtc_info.y  = now.year();   rtc_info.mo = now.month();
  rtc_info.d  = now.day();    rtc_info.h  = now.hour();
  rtc_info.mi = now.minute(); rtc_info.s  = now.second();
  results.rtc = (rtc_info.y >= 2024);
  Serial.printf("OK: %04d-%02d-%02d %02d:%02d:%02d\n",
    rtc_info.y, rtc_info.mo, rtc_info.d,
    rtc_info.h, rtc_info.mi, rtc_info.s);
}

/**
 * @brief Test SX1262 LoRa transceiver.
 * @details Initialises with SF9/BW125/CR4-7 at LORA_FREQ_MHZ.
 *          Configures DIO2 as RF switch and DIO3 as TCXO source (3.3V).
 *          Transmits a 6-byte test packet to verify TX path.
 * @note    DIO3 TCXO configuration is mandatory for correct frequency
 *          accuracy. Ref: Semtech AN1200.21, SX1262 DS sec 13.3.6.
 */
void test_lora() {
  Serial.println("\n--- TEST SX1262 ---");
  lora_info.freq       = LORA_FREQ_MHZ;
  lora_info.init_state = radio.begin(
    lora_info.freq,
    125.0,  // BW kHz
    9,      // SF
    7,      // CR (4/7)
    RADIOLIB_SX126X_SYNC_WORD_PRIVATE,
    22,     // power dBm
    8       // preamble length
  );
  if (lora_info.init_state != RADIOLIB_ERR_NONE) {
    Serial.printf("FAIL: begin=%d\n", lora_info.init_state);
    return;
  }
  radio.setDio2AsRfSwitch(true);
  radio.setTCXO(3.3, 500);  // 500 ticks ~7.8 ms stabilisation delay

  uint8_t pkt[] = {0xAA, 0x55, 0xDE, 0xAD, 0xBE, 0xEF};
  lora_info.tx_ok = (radio.transmit(pkt, sizeof(pkt)) == RADIOLIB_ERR_NONE);
  results.lora    = (lora_info.init_state == RADIOLIB_ERR_NONE);
  Serial.printf("OK  TX: %s\n", lora_info.tx_ok ? "OK" : "FAIL");
}

/**
 * @brief Initialise EPD and show splash screen.
 * @details Uses the secondary SPI bus (HSPI GPIO38/45).
 *          SPI is injected via selectSPI() before init() to avoid
 *          the default SPI bus conflict with the LoRa transceiver.
 * @note    If display shows artefacts, change the GxEPD2_154_D67
 *          driver to GxEPD2_154_M09 in the object declaration.
 */
void test_epd() {
  Serial.println("\n--- TEST EPD ---");
  pca_pin_write(PCA_EINK_EN, true);
  delay(20);
  display.epd2.selectSPI(spi_epd, SPISettings(4000000, MSBFIRST, SPI_MODE0));
  display.init(115200, true, 2, false);

  display.setRotation(EPD_ROTATION);
  display.setFont(&FreeMonoBold9pt7b);
  display.setTextColor(GxEPD_BLACK);
  display.setFullWindow();
  display.firstPage();
  do {
    display.fillScreen(GxEPD_WHITE);
    display.setCursor(10, 30); display.print("  Electgpl  ");
    display.setCursor(10, 55); display.print("ThinkNode M5");
    display.setCursor(10, 80); display.print("HW Test v1.4");
    display.setFont(&FreeMono9pt7b);
    display.setCursor(10, 110); display.print("Running tests...");
    display.setCursor(10, 130); display.print("Please wait...");
  } while (display.nextPage());

  results.epd = true;
  Serial.println("OK");
  delay(1500);
}

/**
 * @brief Test battery ADC (GPIO8, ADC1_CH7).
 * @details Averages 64 samples to reduce ESP32-S3 ADC noise.
 *          Vbat = (raw / 4095) * 3.1V * ADC_MULTIPLIER.
 *          Valid range: 3.0V–4.35V (single-cell Li-Ion).
 * @note    ADC_MULTIPLIER=2.11 is empirical from variant.h.
 *          For higher accuracy use esp_adc_cal_characterize() with eFuse Vref.
 *          Ref: ESP32-S3 TRM Chapter 4.
 */
void test_battery() {
  Serial.println("\n--- TEST Battery ADC ---");
  uint32_t s = 0;
  for (int i = 0; i < 64; i++) { s += analogRead(PIN_BATTERY_ADC); delay(2); }
  bat_info.raw  = s / 64;
  bat_info.vadc = (bat_info.raw / 4095.0f) * 3.1f;
  bat_info.vbat = bat_info.vadc * ADC_MULTIPLIER;
  bat_info.pct  = (int)constrain(
    (bat_info.vbat - 3.0f) / (4.2f - 3.0f) * 100.0f, 0, 100);
  results.battery = (bat_info.vbat >= 3.0f && bat_info.vbat <= 4.35f);
  Serial.printf("  Vbat=%.2fV  %d%%  %s\n",
    bat_info.vbat, bat_info.pct, results.battery ? "OK" : "WARN");
}

/**
 * @brief Test buzzer by playing a short ascending/descending scale.
 * @note  Pass/fail cannot be determined programmatically — verify audibly.
 */
void test_buzzer() {
  Serial.println("\n--- TEST Buzzer ---");
  uint16_t freqs[] = {440, 660, 880, 1100, 880, 660, 440};
  for (auto f : freqs) { buzzer_tone(f, 100); delay(30); }
  results.buzzer = true;
  Serial.println("OK (verify audibly)");
}

/**
 * @brief Test both push buttons with an 8-second user interaction window.
 * @details Displays a prompt on the EPD. Detects press of BTN1 and/or BTN2.
 *          Result is PASS if at least one button was pressed.
 */
void test_buttons() {
  Serial.println("\n--- TEST Buttons (8s) ---");

  display.setRotation(EPD_ROTATION);
  display.setFullWindow(); display.firstPage();
  do {
    display.fillScreen(GxEPD_WHITE);
    display.setFont(&FreeMonoBold9pt7b);
    display.setTextColor(GxEPD_BLACK);
    display.setCursor(4, 16); display.print("Button Test");
    display.drawLine(0, 21, 200, 21, GxEPD_BLACK);
    display.setFont(&FreeMono9pt7b);
    display.setCursor(4, 42); display.print("BTN1 -> GPIO21");
    display.setCursor(4, 59); display.print("BTN2 -> GPIO14");
    display.setCursor(4, 84); display.print("Press any button");
    display.setCursor(4,101); display.print("within 8 seconds");
  } while(display.nextPage());

  bool b1 = false, b2 = false;
  uint32_t t = millis();
  while (millis() - t < 8000) {
    if (digitalRead(PIN_BTN1) == LOW) { b1 = true; buzzer_click(); delay(200); }
    if (digitalRead(PIN_BTN2) == LOW) { b2 = true; buzzer_click(); delay(200); }
    if (b1 && b2) break;
    delay(20);
  }
  results.buttons = b1 || b2;
  Serial.printf("  BTN1=%s  BTN2=%s\n", b1 ? "OK" : "no", b2 ? "OK" : "no");
}

/**
 * @brief Test Quectel L76K GPS module via UART1.
 * @details Powers on GPS, resets it (active-low >100 ms per L76K datasheet),
 *          then waits up to 15 seconds for valid NMEA frames.
 *          gps_uart is set true if any NMEA characters are received.
 *          gps_fix  is set true only if a valid position is obtained
 *          (requires outdoor clear-sky visibility).
 */
void test_gps() {
  Serial.println("\n--- TEST GPS L76K (15s) ---");

  digitalWrite(PIN_GPS_SWITCH,  HIGH);
  digitalWrite(PIN_GPS_STANDBY, HIGH); delay(10);
  digitalWrite(PIN_GPS_RESET,   LOW);  delay(150);  // >100ms reset pulse
  digitalWrite(PIN_GPS_RESET,   HIGH); delay(500);  // boot time

  gpsSerial.begin(GPS_BAUD, SERIAL_8N1, PIN_GPS_TX_CPU, PIN_GPS_RX_CPU);

  display.setRotation(EPD_ROTATION);
  display.setFullWindow(); display.firstPage();
  do {
    display.fillScreen(GxEPD_WHITE);
    display.setFont(&FreeMonoBold9pt7b);
    display.setTextColor(GxEPD_BLACK);
    display.setCursor(4, 16); display.print("GPS Test");
    display.drawLine(0, 21, 200, 21, GxEPD_BLACK);
    display.setFont(&FreeMono9pt7b);
    display.setCursor(4, 50); display.print("Waiting NMEA...");
    display.setCursor(4, 67); display.print("15 seconds max");
  } while(display.nextPage());

  uint32_t t = millis();
  while (millis() - t < 15000) {
    while (gpsSerial.available()) {
      char c = gpsSerial.read();
      gps.encode(c);
      gps_info.chars++;
      if (gps_info.chars < 512) Serial.write(c);
    }
    if (gps.location.isValid()) { results.gps_fix = true; break; }
    delay(50);
  }
  Serial.println();

  results.gps_uart = (gps_info.chars > 20);
  gps_info.sats    = gps.satellites.value();
  if (results.gps_fix) {
    gps_info.lat = gps.location.lat();
    gps_info.lon = gps.location.lng();
    gps_info.alt = gps.altitude.meters();
  }
  Serial.printf("  NMEA:%s  chars=%lu  sats=%d  fix=%s\n",
    results.gps_uart ? "OK" : "FAIL",
    gps_info.chars, gps_info.sats,
    results.gps_fix  ? "YES" : "NO");
}

// ============================================================
// SETUP
// ============================================================

/**
 * @brief Arduino setup: initialise peripherals and run all hardware tests.
 */
void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 3000) {}
  Serial.println("=== ThinkNode M5 HW Test v1.4 ===");

  // GPIO configuration
  pinMode(PIN_VBUS_DETECT, INPUT);
  pinMode(PIN_BTN1,        INPUT_PULLUP);
  pinMode(PIN_BTN2,        INPUT_PULLUP);
  pinMode(PIN_GPS_SWITCH,  OUTPUT); digitalWrite(PIN_GPS_SWITCH,  LOW);
  pinMode(PIN_GPS_RESET,   OUTPUT); digitalWrite(PIN_GPS_RESET,   HIGH);
  pinMode(PIN_GPS_STANDBY, OUTPUT); digitalWrite(PIN_GPS_STANDBY, LOW);
  pinMode(PIN_LORA_PWR_EN, OUTPUT); digitalWrite(PIN_LORA_PWR_EN, LOW);

  // LEDC buzzer (ESP32 Arduino Core v3.x unified API)
  ledcAttach(PIN_BUZZER, 2000, 8);

  // ADC — 12-bit, 11dB attenuation (0–3.3V range)
  analogReadResolution(12);
  analogSetAttenuation(ADC_11db);

  // I2C bus 0 — PCA9557 (GPIO48 SDA / GPIO47 SCL)
  Wire.begin(PIN_I2C_SDA, PIN_I2C_SCL);
  Wire.setClock(400000);

  // I2C bus 1 — PCF8563 RTC (GPIO2 SDA / GPIO1 SCL)
  Wire1_rtc.begin(PIN_RTC_SDA, PIN_RTC_SCL);
  Wire1_rtc.setClock(400000);

  // I2C discovery scan (debug)
  Serial.println("I2C0 scan (GPIO48/47):");
  for (uint8_t a = 1; a < 127; a++) {
    Wire.beginTransmission(a);
    if (Wire.endTransmission() == 0) Serial.printf("  0x%02X\n", a);
  }
  Serial.println("I2C1 scan (GPIO2/1):");
  for (uint8_t a = 1; a < 127; a++) {
    Wire1_rtc.beginTransmission(a);
    if (Wire1_rtc.endTransmission() == 0) Serial.printf("  0x%02X\n", a);
  }

  // SPI buses
  spi_lora.begin(PIN_LORA_SCK, PIN_LORA_MISO, PIN_LORA_MOSI, PIN_LORA_CS);
  spi_epd.begin(PIN_EPD_SCK, -1, PIN_EPD_MOSI, PIN_EPD_CS);

  // Startup tone
  buzzer_tone(800, 80); delay(40); buzzer_tone(1200, 80);
  delay(200);

  // ---- Test sequence ----
  // PCA9557 MUST be first — it enables power to all other peripherals
  test_pca9557(); delay(80);
  test_epd();     delay(200);
  test_rtc();     delay(50);
  test_battery(); delay(50);
  test_buzzer();  delay(50);
  digitalWrite(PIN_LORA_PWR_EN, HIGH); delay(20);  // enable SX1262 LDO
  test_lora();    delay(50);
  test_buttons(); delay(50);
  test_gps();     delay(50);

  // Serial summary
  Serial.println("\n=== RESULTS ===");
  Serial.printf("PCA9557 : %s\n", results.pca9557  ? "PASS" : "FAIL");
  Serial.printf("RTC     : %s\n", results.rtc      ? "PASS" : "FAIL");
  Serial.printf("LoRa    : %s\n", results.lora     ? "PASS" : "FAIL");
  Serial.printf("EPD     : %s\n", results.epd      ? "PASS" : "FAIL");
  Serial.printf("GPS UART: %s\n", results.gps_uart ? "PASS" : "FAIL");
  Serial.printf("GPS Fix : %s\n", results.gps_fix  ? "PASS" : "NO FIX");
  Serial.printf("Buzzer  : %s\n", results.buzzer   ? "PASS" : "----");
  Serial.printf("Buttons : %s\n", results.buttons  ? "PASS" : "WARN");
  Serial.printf("Battery : %s\n", results.battery  ? "PASS" : "FAIL");
  Serial.println("BTN1=next page   BTN2=prev page / LoRa ping (page 3)");

  bool all_ok = results.pca9557 && results.rtc && results.lora
             && results.epd    && results.battery;
  if (all_ok) buzzer_ok(); else buzzer_fail();

  // Show summary page on EPD
  current_page = 0;
  show_page(current_page);
  delay(3000);
  pca_pin_write(PCA_EINK_EN, false); // backlight off — e-paper image persists
}

// ============================================================
// LOOP — button navigation + continuous serial monitor
// ============================================================

/**
 * @brief Arduino loop: button navigation and background serial logging.
 *
 * @details BTN1 advances pages (edge-triggered).
 *          BTN2 goes back one page, except on page 3 (LoRa) where it
 *          triggers a ping-pong test instead.
 *          GPS NMEA is decoded continuously in the background.
 *          RTC, battery voltage and GPS fix are logged every 5 seconds.
 */
void loop() {
  static bool b1_last = HIGH, b2_last = HIGH;
  bool b1 = digitalRead(PIN_BTN1);
  bool b2 = digitalRead(PIN_BTN2);

  // BTN1 falling edge — next page
  if (b1 == LOW && b1_last == HIGH) {
    current_page = (current_page + 1) % PAGE_COUNT;
    Serial.printf("[NAV] page %d/%d\n", current_page + 1, PAGE_COUNT);
    buzzer_click();
    show_page(current_page);
    delay(3000);
    pca_pin_write(PCA_EINK_EN, false);
  }

  // BTN2 falling edge
  if (b2 == LOW && b2_last == HIGH) {
    if (current_page == 3) {
      // On LoRa page: trigger ping-pong test
      Serial.println("[BTN2] LoRa ping-pong");
      lora_ping_pong();
      show_page(current_page);   // refresh stats after test
      delay(3000);
      pca_pin_write(PCA_EINK_EN, false);
    } else {
      // On any other page: go back
      current_page = (current_page - 1 + PAGE_COUNT) % PAGE_COUNT;
      Serial.printf("[NAV] page %d/%d\n", current_page + 1, PAGE_COUNT);
      buzzer_click();
      show_page(current_page);
      delay(3000);
      pca_pin_write(PCA_EINK_EN, false);
    }
  }

  b1_last = b1;
  b2_last = b2;

  // Background GPS decoding
  while (gpsSerial.available()) {
    char c = gpsSerial.read();
    gps.encode(c);
  }

  // Serial monitor — log every 5 seconds
  static uint32_t t_log = 0;
  if (millis() - t_log > 5000) {
    t_log = millis();

    DateTime now = rtc.now();
    Serial.printf("[RTC] %04d-%02d-%02d %02d:%02d:%02d\n",
      now.year(), now.month(), now.day(),
      now.hour(), now.minute(), now.second());

    uint32_t adc = 0;
    for (int i = 0; i < 16; i++) adc += analogRead(PIN_BATTERY_ADC);
    float vbat = (adc / 16 / 4095.0f) * 3.1f * ADC_MULTIPLIER;
    Serial.printf("[BAT] Vbat=%.2fV\n", vbat);

    if (gps.location.isValid())
      Serial.printf("[GPS] Lat=%.6f Lon=%.6f Sats=%d\n",
        gps.location.lat(), gps.location.lng(), gps.satellites.value());
  }

  // Blue LED mirrors BTN1 state (visual feedback)
  pca_pin_write(PCA_LED_USER, (b1 == LOW));

  delay(50);
}