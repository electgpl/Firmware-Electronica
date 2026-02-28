/**
 * @file    ThinkNodeM5_LoRaComm_v2_0.ino
 * @brief   LoRa predefined-message communicator v2.0 — Elecrow ThinkNode M5.
 *
 * @details New features vs v1.3:
 *            - ACK protocol: receiver auto-replies with a 4-byte ACK packet.
 *              Transmitter waits up to ACK_TIMEOUT_MS for confirmation and
 *              shows "ACK OK" (✓✓) or "NO ACK" on the EPD.
 *            - Heartbeat LED: blue LED (PCA9557 P1) pulses 80 ms every
 *              HEARTBEAT_MS (default 5 s) to confirm device is alive.
 *            - EPD backlight blink on new RX: PCA_EINK_EN blinks 5× after
 *              a received-message refresh so the user notices the alert even
 *              when not looking at the screen. Blink is non-blocking (uses
 *              a millis() state machine, not delay()).
 *            - Battery percentage in every screen header (ADC GPIO8, cached
 *              every BAT_CACHE_MS to avoid flooding the ADC).
 *            - Message history: last 3 received messages stored in a circular
 *              RAM buffer (RxHistory). Accessible by pressing BTN1 past the
 *              last TX message slot — enters HISTORY_VIEW. BTN2 returns to
 *              IDLE_MENU. Each history entry shows sender, message text,
 *              RSSI and RTC timestamp.
 *
 * @note    Protocol v2 packet layout (4 bytes):
 *            [0] sender_id    — DEVICE_ID of originator
 *            [1] pkt_type     — PKT_TYPE_MSG (0x00) or PKT_TYPE_ACK (0xFF)
 *            [2] msg_idx      — message index (MSG) / echoed index (ACK)
 *            [3] CRC-8/MAXIM  — of bytes [0..2]
 *          Protocol is backward-INcompatible with v1.x (pkt_type byte added).
 *          Both units must run the same version.
 *
 * @note    ACK timing at SF12/BW125/CR4-8:
 *            ToA(4-byte payload) ≈ 2.793 s TX + 2.793 s ACK = 5.6 s round trip.
 *            ACK_TIMEOUT_MS should be > 2 × ToA + processing margin = ~6.5 s.
 *            Default is 7000 ms.
 *
 * @note    EPD backlight blink uses PCA_EINK_EN gating. The GDEH0154D67
 *          panel holds its image without power (bistable e-paper). Gating
 *          the power rail after a completed refresh is safe and is how the
 *          HW test sketch also manages it. Do NOT gate PCA_EINK_EN during
 *          an active EPD transaction (while display.nextPage() loop runs).
 *
 * @note    All struct/enum declared before any function referencing them
 *          (Arduino IDE ctags forward-declaration limitation).
 *          No <avr/pgmspace.h> — ESP32-S3 MMU maps PROGMEM to linear space.
 *
 * Required libraries:
 *   RadioLib  >= 6.6.0   (Jan Gromes)
 *   GxEPD2    >= 1.5.8   (Jean-Marc Zingg)
 *   RTClib    >= 2.1.4   (Adafruit)
 *
 * Board: ESP32S3 Dev Module | Flash 8MB | USB CDC off | PSRAM off | 240 MHz
 *
 * @author  Electgpl
 * @date    2026-02-28
 * @version 2.0
 */

// ============================================================
// 1. INCLUDES
// ============================================================
#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include "RTClib.h"
#include <RadioLib.h>
#include <GxEPD2_BW.h>
#include <Fonts/FreeMonoBold9pt7b.h>
#include <Fonts/FreeMono9pt7b.h>

// ============================================================
// 2. USER CONFIGURATION
// ============================================================

/** @brief Device ID. Unit A = 0x01, Unit B = 0x02. */
#define DEVICE_ID           0x01

/** @brief LoRa frequency MHz. 915.0=US/AU/BR  868.0=EU  433.0=EU433/CN */
#define LORA_FREQ_MHZ       915.0f

/** @brief Spreading Factor. SF12 = max range, ~2.8 s ToA per packet. */
#define LORA_SF             12

/** @brief Bandwidth kHz. */
#define LORA_BW_KHZ         125.0f

/** @brief Coding Rate denominator (RadioLib). 8 = CR4/8. */
#define LORA_CR             8

/** @brief Preamble symbols. */
#define LORA_PREAMBLE       16

/** @brief TX power dBm. SX1262 max = 22. */
#define LORA_POWER_DBM      22

/** @brief TCXO DIO3 supply voltage (V). */
#define LORA_TCXO_V         3.3f

/** @brief TCXO stabilisation ticks (1 tick = 15.625 µs). 500 = 7.8 ms. */
#define LORA_TCXO_TICKS     500

/** @brief LoRa sync word. 0x12 = private network. */
#define LORA_SYNC_WORD      RADIOLIB_SX126X_SYNC_WORD_PRIVATE

/** @brief EPD rotation. 0=0° 1=90°CW 2=180° 3=270°CW. */
#define EPD_ROTATION        0

/**
 * @brief ACK wait timeout in ms.
 * @details Must exceed round-trip ToA: 2×ToA(SF12/BW125/4B) + margin.
 *          2 × 2793 ms + 900 ms margin = 6486 ms → use 7000 ms.
 */
#define ACK_TIMEOUT_MS      7000UL

/** @brief Duration (ms) the RX / ACK result screen shows before auto-dismiss. */
#define RESULT_DISPLAY_MS   8000UL

/** @brief Heartbeat LED pulse interval (ms). */
#define HEARTBEAT_MS        5000UL

/** @brief Heartbeat LED pulse on-time (ms). Keep short to save power. */
#define HEARTBEAT_PULSE_MS  80

/** @brief Number of RX blink cycles after a new message arrives. */
#define RX_BLINK_COUNT      5

/** @brief EPD backlight on-time per blink cycle (ms). */
#define RX_BLINK_ON_MS      120

/** @brief EPD backlight off-time between blink cycles (ms). */
#define RX_BLINK_OFF_MS     120

/** @brief Battery ADC cache refresh interval (ms). */
#define BAT_CACHE_MS        30000UL

/** @brief Number of received messages kept in history buffer. */
#define HISTORY_SIZE        3

/** @brief Number of predefined messages. */
#define MSG_COUNT           10

// ============================================================
// PIN DEFINITIONS  (verified ThinkNodeM5_HWtest_v1.4)
// ============================================================

#define PIN_I2C_SCL         47
#define PIN_I2C_SDA         48
#define PIN_RTC_SCL          1
#define PIN_RTC_SDA          2

#define PCA9557_ADDR        0x18
#define PCA9557_REG_OUT     0x01
#define PCA9557_REG_CFG     0x03
#define PCA_LED_USER         1   ///< Blue LED
#define PCA_LED_ENABLE       2   ///< LED rail enable
#define PCA_LED_POWER        3   ///< Red LED (charger)
#define PCA_POWER_EN         4   ///< Peripheral power rail
#define PCA_EINK_EN          5   ///< EPD power enable

#define PIN_BUZZER           9
#define PIN_BTN1            21   ///< Cycle / history
#define PIN_BTN2            14   ///< Send / back

#define PIN_LORA_SCK        16
#define PIN_LORA_MISO        7
#define PIN_LORA_MOSI       15
#define PIN_LORA_CS         17
#define PIN_LORA_RST         6
#define PIN_LORA_BUSY        5
#define PIN_LORA_DIO1        4
#define PIN_LORA_PWR_EN     46

#define PIN_EPD_CS          39
#define PIN_EPD_BUSY        42
#define PIN_EPD_DC          40
#define PIN_EPD_RST         41
#define PIN_EPD_SCK         38
#define PIN_EPD_MOSI        45

#define PIN_BATTERY_ADC      8
#define ADC_MULTIPLIER       2.11f

// ============================================================
// PROTOCOL v2
// ============================================================

#define PKT_SIZE            4       ///< v2 packet: 4 bytes
#define PKT_OFFSET_ID       0       ///< Byte 0: sender Device ID
#define PKT_OFFSET_TYPE     1       ///< Byte 1: packet type
#define PKT_OFFSET_IDX      2       ///< Byte 2: message index
#define PKT_OFFSET_CRC      3       ///< Byte 3: CRC-8/MAXIM of bytes 0-2
#define PKT_TYPE_MSG        0x00    ///< Normal message packet
#define PKT_TYPE_ACK        0xFF    ///< Acknowledgement packet

// ============================================================
// 3. PROGMEM MESSAGE TABLE
// ============================================================

static const char msg_0[]  PROGMEM = "Estoy saliendo";
static const char msg_1[]  PROGMEM = "Ya llegue";
static const char msg_2[]  PROGMEM = "Llamame";
static const char msg_3[]  PROGMEM = "Voy para alla";
static const char msg_4[]  PROGMEM = "Donde estas?";
static const char msg_5[]  PROGMEM = "Todo bien";
static const char msg_6[]  PROGMEM = "Necesito ayuda";
static const char msg_7[]  PROGMEM = "Ya vuelvo";
static const char msg_8[]  PROGMEM = "Nos vemos en casa";
static const char msg_9[]  PROGMEM = "Bateria baja";

static const char* const MSG_TABLE[MSG_COUNT] PROGMEM = {
  msg_0, msg_1, msg_2, msg_3, msg_4,
  msg_5, msg_6, msg_7, msg_8, msg_9
};

// ============================================================
// 4. HARDWARE OBJECTS
// ============================================================

SPIClass spi_lora(FSPI);
SPIClass spi_epd(HSPI);

SX1262 radio = new Module(PIN_LORA_CS, PIN_LORA_DIO1,
                           PIN_LORA_RST, PIN_LORA_BUSY, spi_lora);

GxEPD2_BW<GxEPD2_154_D67, GxEPD2_154_D67::HEIGHT> display(
  GxEPD2_154_D67(PIN_EPD_CS, PIN_EPD_DC, PIN_EPD_RST, PIN_EPD_BUSY)
);

RTC_PCF8563 rtc;
TwoWire     Wire1_rtc = TwoWire(1);

// ============================================================
// 5. ENUMS AND STRUCTS  (all declared before any function)
// ============================================================

/**
 * @brief Main application FSM states.
 */
enum class AppState : uint8_t {
  BOOT_INIT    = 0,  ///< Hardware initialisation
  IDLE_MENU    = 1,  ///< TX message selector, RX armed
  TX_WAIT_ACK  = 2,  ///< Packet sent, waiting for ACK
  RX_RECEIVED  = 3,  ///< Incoming message being displayed
  HISTORY_VIEW = 4   ///< Browsing received message history
};

/**
 * @brief ACK result after a transmission attempt.
 */
enum class AckResult : uint8_t {
  PENDING  = 0,  ///< Waiting
  OK       = 1,  ///< ACK received within timeout
  TIMEOUT  = 2   ///< No ACK before ACK_TIMEOUT_MS
};

/**
 * @brief One received message record (used in history buffer and last_rx).
 */
struct RxRecord {
  uint8_t  sender_id;   ///< Remote device ID
  uint8_t  msg_idx;     ///< Message index
  float    rssi;        ///< RSSI in dBm
  float    snr;         ///< SNR in dB
  uint8_t  rx_hh;       ///< RTC hour at reception
  uint8_t  rx_mm;       ///< RTC minute
  uint8_t  rx_ss;       ///< RTC second
  bool     valid;       ///< true = slot occupied
};

/**
 * @brief Metadata of the last transmitted message.
 */
struct TxRecord {
  uint8_t  msg_idx;
  uint8_t  tx_hh;
  uint8_t  tx_mm;
  uint8_t  tx_ss;
};

/**
 * @brief Circular history buffer for received messages.
 * @details Oldest entry is overwritten when full.
 *          head points to the next write slot.
 *          count tracks how many valid entries exist (max HISTORY_SIZE).
 */
struct RxHistory {
  RxRecord slots[HISTORY_SIZE];
  uint8_t  head;   ///< Next write index (0..HISTORY_SIZE-1)
  uint8_t  count;  ///< Number of valid entries (0..HISTORY_SIZE)
};

/**
 * @brief EPD backlight blink state machine context.
 * @details Drives PCA_EINK_EN non-blocking blink after RX display.
 */
struct BlinkCtx {
  bool     active;      ///< true = blink sequence running
  uint8_t  remaining;   ///< Blink cycles left
  bool     phase;       ///< true = ON phase, false = OFF phase
  uint32_t phase_ms;    ///< millis() at start of current phase
};

// ============================================================
// 6. GLOBAL VARIABLES
// ============================================================

volatile AppState app_state  = AppState::BOOT_INIT;
volatile bool     rx_flag    = false;

uint8_t   selected_msg_idx   = 0;
uint8_t   pca_out            = 0x00;

RxRecord  last_rx;
TxRecord  last_tx;
AckResult last_ack_result    = AckResult::PENDING;

RxHistory history            = { {}, 0, 0 };
uint8_t   history_view_idx   = 0;   ///< Which history slot is on screen

BlinkCtx  epd_blink          = { false, 0, false, 0 };

// Battery cache
uint8_t   bat_pct            = 0;
uint32_t  bat_cache_ms       = 0;

// ============================================================
// 7. ISR
// ============================================================

/**
 * @brief DIO1 ISR — signals RX-done or TX-done to the main loop.
 * @note  IRAM_ATTR mandatory on ESP32-S3 (must reside in IRAM).
 *        No RadioLib calls inside ISR.
 */
void IRAM_ATTR isr_radio_done() {
  rx_flag = true;
}

// ============================================================
// 8. HELPERS
// ============================================================

// ---- CRC-8/MAXIM (poly 0x31, init 0x00, reflected) ----

/**
 * @brief Compute CRC-8/MAXIM over a byte buffer.
 * @param data  Input buffer.
 * @param len   Byte count.
 * @return CRC-8 value.
 */
static uint8_t crc8_maxim(const uint8_t* data, size_t len) {
  uint8_t crc = 0x00;
  for (size_t i = 0; i < len; i++) {
    crc ^= data[i];
    for (uint8_t b = 0; b < 8; b++)
      crc = (crc & 1) ? (crc >> 1) ^ 0x8C : crc >> 1;
  }
  return crc;
}

// ---- PCA9557 ----

/**
 * @brief Initialise PCA9557: P1..P5 outputs, P0/P6/P7 inputs.
 * @return true on success.
 */
static bool pca_init() {
  Wire.beginTransmission(PCA9557_ADDR);
  Wire.write(PCA9557_REG_CFG);
  Wire.write(0xC1);   // P0=in P1..P5=out P6=in P7=in
  if (Wire.endTransmission() != 0) return false;
  pca_out = 0x00;
  Wire.beginTransmission(PCA9557_ADDR);
  Wire.write(PCA9557_REG_OUT);
  Wire.write(pca_out);
  return (Wire.endTransmission() == 0);
}

/**
 * @brief Write one PCA9557 output pin (shadow-register pattern).
 * @param pin  0-7 (only P1..P5 are outputs).
 * @param val  true=HIGH false=LOW.
 */
static void pca_pin_write(uint8_t pin, bool val) {
  if (val) pca_out |=  (1u << pin);
  else     pca_out &= ~(1u << pin);
  Wire.beginTransmission(PCA9557_ADDR);
  Wire.write(PCA9557_REG_OUT);
  Wire.write(pca_out);
  Wire.endTransmission();
}

// ---- Buzzer ----

/** @brief Play tone, blocking. freq_hz=0 = silence. */
static void buzzer_tone(uint16_t freq_hz, uint16_t ms) {
  ledcWriteTone(PIN_BUZZER, freq_hz);
  delay(ms);
  ledcWriteTone(PIN_BUZZER, 0);
}

/** @brief Short UI click. */
static void buzzer_click()      { buzzer_tone(1200, 35); }

/** @brief TX-sent ascending melody. */
static void buzzer_tx_sent()    { buzzer_tone(1047,60); delay(15); buzzer_tone(1568,90); }

/** @brief ACK-received double beep. */
static void buzzer_ack_ok()     { buzzer_tone(1760,50); delay(30); buzzer_tone(1760,50); }

/** @brief ACK timeout — low descending tone. */
static void buzzer_ack_fail()   { buzzer_tone(400,200); delay(40); buzzer_tone(300,280); }

/** @brief Incoming message alert — descending two-note. */
static void buzzer_rx_notify()  { buzzer_tone(1568,80); delay(15); buzzer_tone(1047,120); }

/** @brief Error / fatal — two long low tones. */
static void buzzer_error()      { buzzer_tone(400,180); delay(40); buzzer_tone(300,280); }

// ---- Battery ADC ----

/**
 * @brief Read battery percentage, cached every BAT_CACHE_MS.
 * @details Averages 16 samples. Formula from HW test v1.4:
 *          Vbat = (raw/4095) * 3.1 * ADC_MULTIPLIER.
 *          SoC approximation: linear 3.0 V (0%) to 4.2 V (100%).
 * @return Battery percentage 0–100.
 */
static uint8_t bat_read_pct() {
  if (millis() - bat_cache_ms < BAT_CACHE_MS && bat_cache_ms != 0)
    return bat_pct;
  uint32_t s = 0;
  for (int i = 0; i < 16; i++) { s += analogRead(PIN_BATTERY_ADC); delay(1); }
  float vadc = (s / 16 / 4095.0f) * 3.1f;
  float vbat = vadc * ADC_MULTIPLIER;
  bat_pct    = (uint8_t)constrain((vbat - 3.0f) / (4.2f - 3.0f) * 100.0f, 0, 100);
  bat_cache_ms = millis();
  return bat_pct;
}

// ---- RTC ----

/**
 * @brief Read PCF8563 current time.
 * @param[out] hh  Hours.
 * @param[out] mm  Minutes.
 * @param[out] ss  Seconds.
 */
static void rtc_now_hms(uint8_t& hh, uint8_t& mm, uint8_t& ss) {
  DateTime now = rtc.now();
  hh = now.hour(); mm = now.minute(); ss = now.second();
}

// ---- Message access ----

/**
 * @brief Copy PROGMEM message string to RAM buffer (ESP32: direct pointer).
 * @param idx  0..MSG_COUNT-1, clamped.
 * @param buf  Destination RAM buffer.
 * @param len  Buffer size including null terminator.
 * @return buf pointer.
 */
static char* msg_get(uint8_t idx, char* buf, size_t len) {
  if (idx >= MSG_COUNT) idx = 0;
  strlcpy(buf, MSG_TABLE[idx], len);
  return buf;
}

// ---- History buffer ----

/**
 * @brief Push a new RxRecord into the circular history buffer.
 * @param r  Record to store. Oldest entry is overwritten when full.
 */
static void history_push(const RxRecord& r) {
  history.slots[history.head] = r;
  history.slots[history.head].valid = true;
  history.head = (history.head + 1) % HISTORY_SIZE;
  if (history.count < HISTORY_SIZE) history.count++;
}

/**
 * @brief Get history slot by display index (0 = most recent).
 * @param display_idx  0..HISTORY_SIZE-1 (most recent first).
 * @return Pointer to RxRecord, or nullptr if slot empty.
 */
static const RxRecord* history_get(uint8_t display_idx) {
  if (display_idx >= history.count) return nullptr;
  // head-1 = most recent; head-2 = second most recent, etc.
  int slot = ((int)history.head - 1 - (int)display_idx + HISTORY_SIZE * 2)
             % HISTORY_SIZE;
  return &history.slots[slot];
}

// ---- LED helpers ----

/**
 * @brief Blocking LED blink (used for TX/RX feedback where delay is acceptable).
 */
static void led_blink(uint8_t count, uint16_t on_ms, uint16_t off_ms) {
  for (uint8_t i = 0; i < count; i++) {
    pca_pin_write(PCA_LED_USER, true);  delay(on_ms);
    pca_pin_write(PCA_LED_USER, false);
    if (i < count - 1u) delay(off_ms);
  }
}

// ---- EPD blink state machine (non-blocking) ----

/**
 * @brief Start a non-blocking EPD backlight blink sequence.
 * @details Arms the BlinkCtx state machine. tick_epd_blink() must be
 *          called every loop iteration to advance the state.
 *          IMPORTANT: call only AFTER the EPD refresh is fully complete
 *          (after display.nextPage() loop exits). Never during refresh.
 * @param count  Number of blink cycles (typically RX_BLINK_COUNT).
 */
static void epd_blink_start(uint8_t count) {
  epd_blink.active    = true;
  epd_blink.remaining = count;
  epd_blink.phase     = true;   // Start with ON phase
  epd_blink.phase_ms  = millis();
  pca_pin_write(PCA_EINK_EN, true);  // First ON
}

/**
 * @brief Advance the EPD backlight blink state machine.
 * @details Call once per main loop iteration.
 *          Manages PCA_EINK_EN transitions based on elapsed time.
 *          Leaves PCA_EINK_EN LOW (off) when sequence completes.
 */
static void tick_epd_blink() {
  if (!epd_blink.active) return;
  uint32_t elapsed = millis() - epd_blink.phase_ms;
  uint32_t target  = epd_blink.phase ? RX_BLINK_ON_MS : RX_BLINK_OFF_MS;
  if (elapsed < target) return;

  // Phase expired — toggle
  epd_blink.phase = !epd_blink.phase;
  epd_blink.phase_ms = millis();

  if (epd_blink.phase) {
    // Rising: start of new ON phase = new blink cycle starting
    pca_pin_write(PCA_EINK_EN, true);
  } else {
    // Falling: end of ON phase = count down
    pca_pin_write(PCA_EINK_EN, false);
    if (--epd_blink.remaining == 0) {
      epd_blink.active = false;   // Sequence complete, EPD EN stays LOW
    }
  }
}

// ---- EPD drawing ----

/**
 * @brief Power on EPD and set rotation + default font.
 */
static void epd_begin() {
  pca_pin_write(PCA_EINK_EN, true);
  delay(5);
  display.setRotation(EPD_ROTATION);
  display.setFont(&FreeMonoBold9pt7b);
  display.setTextColor(GxEPD_BLACK);
}

/**
 * @brief Draw standard header: bold title + battery % + separator rule.
 * @details Battery percentage right-aligned in the header row.
 *          Separator rule at Y=21. Switches font to FreeMono9pt7b after.
 * @param title  Header string (max ~10 chars to leave room for battery).
 */
static void epd_header(const char* title) {
  display.fillScreen(GxEPD_WHITE);
  display.setFont(&FreeMonoBold9pt7b);
  display.setTextColor(GxEPD_BLACK);
  display.setCursor(4, 16);
  display.print(title);

  // Battery % — right-aligned in header
  char bat_buf[8];
  snprintf(bat_buf, sizeof(bat_buf), "%3d%%", bat_read_pct());
  display.setCursor(155, 16);
  display.print(bat_buf);

  display.drawLine(0, 21, 200, 21, GxEPD_BLACK);
  display.setFont(&FreeMono9pt7b);
}

/**
 * @brief Draw the TX message selection menu.
 * @details Layout:
 *            Y=16  "LoRa MSG"  |  bat%  (header)
 *            Y=21  rule
 *            Y=36  "ID:XX" small
 *            Y=55  "[N/10]" bold
 *            Y=78  message text
 *            Y=152 "BTN1:next/hist"
 *            Y=170 "BTN2:send"
 * @param idx  Currently selected message index.
 */
static void epd_draw_menu(uint8_t idx) {
  char buf[32], msg[32];
  epd_begin();
  display.setFullWindow();
  display.firstPage();
  do {
    epd_header("LoRa MSG");

    display.setFont(&FreeMono9pt7b);
    snprintf(buf, sizeof(buf), "ID:0x%02X", DEVICE_ID);
    display.setCursor(4, 36);
    display.print(buf);

    display.setFont(&FreeMonoBold9pt7b);
    snprintf(buf, sizeof(buf), "[%d / %d]", idx + 1, MSG_COUNT);
    display.setCursor(4, 58);
    display.print(buf);

    display.setFont(&FreeMono9pt7b);
    msg_get(idx, msg, sizeof(msg));
    display.setCursor(4, 80);
    display.print(msg);

    display.setCursor(4, 152);
    display.print("BTN1:next/hist");
    display.setCursor(4, 170);
    display.print("BTN2:send");
  } while (display.nextPage());
  pca_pin_write(PCA_EINK_EN, false);
}

/**
 * @brief Draw the "transmitting — waiting for ACK" screen.
 * @param idx  Message being sent.
 */
static void epd_draw_tx_waiting(uint8_t idx) {
  char buf[32], msg[32];
  epd_begin();
  display.setFullWindow();
  display.firstPage();
  do {
    epd_header("SENDING...");
    display.setFont(&FreeMono9pt7b);
    snprintf(buf, sizeof(buf), "Msg %d:", idx + 1);
    display.setCursor(4, 50);  display.print(buf);
    msg_get(idx, msg, sizeof(msg));
    display.setCursor(4, 68);  display.print(msg);
    display.setCursor(4, 95);  display.print("Waiting ACK...");
    snprintf(buf, sizeof(buf), "TX %02d:%02d:%02d",
             last_tx.tx_hh, last_tx.tx_mm, last_tx.tx_ss);
    display.setCursor(4, 170); display.print(buf);
  } while (display.nextPage());
  pca_pin_write(PCA_EINK_EN, false);
}

/**
 * @brief Draw TX result screen with ACK status.
 * @param idx     Message index sent.
 * @param result  AckResult::OK or AckResult::TIMEOUT.
 */
static void epd_draw_tx_result(uint8_t idx, AckResult result) {
  char buf[32], msg[32];
  epd_begin();
  display.setFullWindow();
  display.firstPage();
  do {
    bool ok = (result == AckResult::OK);
    epd_header(ok ? "TX OK  / ACK" : "TX OK / NO ACK");

    display.setFont(&FreeMono9pt7b);
    snprintf(buf, sizeof(buf), "Msg %d:", idx + 1);
    display.setCursor(4, 50);  display.print(buf);
    msg_get(idx, msg, sizeof(msg));
    display.setCursor(4, 68);  display.print(msg);

    // ACK status indicator — large and obvious
    display.setFont(&FreeMonoBold9pt7b);
    display.setCursor(4, 105);
    display.print(ok ? "ACK: OK  vv" : "ACK: NO  --");

    display.setFont(&FreeMono9pt7b);
    snprintf(buf, sizeof(buf), "TX %02d:%02d:%02d",
             last_tx.tx_hh, last_tx.tx_mm, last_tx.tx_ss);
    display.setCursor(4, 170); display.print(buf);
  } while (display.nextPage());
  pca_pin_write(PCA_EINK_EN, false);
}

/**
 * @brief Draw TX RadioLib error screen.
 * @param err  RadioLib error code.
 */
static void epd_draw_tx_err(int err) {
  char buf[32];
  epd_begin();
  display.setFullWindow();
  display.firstPage();
  do {
    epd_header("TX ERROR");
    display.setFont(&FreeMono9pt7b);
    snprintf(buf, sizeof(buf), "RadioLib: %d", err);
    display.setCursor(4, 55);  display.print(buf);
    display.setCursor(4, 80);  display.print("Check antenna");
    display.setCursor(4, 98);  display.print("& LoRa power.");
  } while (display.nextPage());
  pca_pin_write(PCA_EINK_EN, false);
}

/**
 * @brief Draw the incoming-message display screen.
 * @param r  Const reference to the decoded RxRecord.
 */
static void epd_draw_rx(const RxRecord& r) {
  char buf[32], msg[32];
  epd_begin();
  display.setFullWindow();
  display.firstPage();
  do {
    epd_header("INCOMING MSG");
    display.setFont(&FreeMono9pt7b);
    snprintf(buf, sizeof(buf), "From: 0x%02X", r.sender_id);
    display.setCursor(4, 40);  display.print(buf);
    snprintf(buf, sizeof(buf), "Msg %d:", r.msg_idx + 1);
    display.setCursor(4, 58);  display.print(buf);
    msg_get(r.msg_idx, msg, sizeof(msg));
    display.setCursor(4, 76);  display.print(msg);
    snprintf(buf, sizeof(buf), "RSSI:%.0fdBm SNR:%.1f", r.rssi, r.snr);
    display.setCursor(4, 110); display.print(buf);
    snprintf(buf, sizeof(buf), "RX %02d:%02d:%02d",
             r.rx_hh, r.rx_mm, r.rx_ss);
    display.setCursor(4, 128); display.print(buf);
    display.setCursor(4, 158); display.print("BTN2: dismiss");
  } while (display.nextPage());
  pca_pin_write(PCA_EINK_EN, false);
}

/**
 * @brief Draw the message history view.
 * @details Shows all history slots (up to HISTORY_SIZE), most recent first.
 *          Empty slots show "---". Highlighted slot index shown in header.
 * @param view_idx  Which slot to highlight (0 = most recent). Currently
 *                  all slots are shown simultaneously on one screen.
 */
static void epd_draw_history() {
  char buf[32], msg[32];
  epd_begin();
  display.setFullWindow();
  display.firstPage();
  do {
    epd_header("RX HISTORY");
    display.setFont(&FreeMono9pt7b);

    int y = 36;
    for (uint8_t i = 0; i < HISTORY_SIZE; i++) {
      const RxRecord* r = history_get(i);
      if (r == nullptr) {
        snprintf(buf, sizeof(buf), "%d: ---", i + 1);
        display.setCursor(4, y); display.print(buf);
        y += 50;
        continue;
      }
      // Slot header: index + sender + time
      snprintf(buf, sizeof(buf), "%d:0x%02X %02d:%02d",
               i + 1, r->sender_id, r->rx_hh, r->rx_mm);
      display.setCursor(4, y);  display.print(buf);
      y += 16;
      // Message text
      msg_get(r->msg_idx, msg, sizeof(msg));
      // Truncate to 18 chars to fit screen width
      msg[18] = '\0';
      display.setCursor(4, y);  display.print(msg);
      y += 16;
      // RSSI
      snprintf(buf, sizeof(buf), "  %.0fdBm", r->rssi);
      display.setCursor(4, y);  display.print(buf);
      y += 20;
    }
    display.setCursor(4, 190);
    display.print("BTN2:back");
  } while (display.nextPage());
  pca_pin_write(PCA_EINK_EN, false);
}

// ---- LoRa helpers ----

/**
 * @brief Arm SX1262 for continuous reception (non-blocking).
 * @details Clears rx_flag before arming to discard any stale ISR trigger.
 * @return RADIOLIB_ERR_NONE on success.
 */
static int lora_start_rx() {
  rx_flag = false;
  return radio.startReceive();
}

/**
 * @brief Transmit a MSG packet (blocking, ~2.8 s at SF12/BW125).
 * @param msg_idx  Message index 0..MSG_COUNT-1.
 * @return RADIOLIB_ERR_NONE on success.
 */
static int lora_send_msg(uint8_t msg_idx) {
  uint8_t pkt[PKT_SIZE];
  pkt[PKT_OFFSET_ID]   = DEVICE_ID;
  pkt[PKT_OFFSET_TYPE] = PKT_TYPE_MSG;
  pkt[PKT_OFFSET_IDX]  = msg_idx;
  pkt[PKT_OFFSET_CRC]  = crc8_maxim(pkt, 3);
  return radio.transmit(pkt, PKT_SIZE);
}

/**
 * @brief Transmit an ACK packet in reply to a received MSG (blocking).
 * @param original_sender_id  Device ID of the unit we are ACKing.
 * @param msg_idx             Message index being acknowledged.
 * @return RADIOLIB_ERR_NONE on success.
 */
static int lora_send_ack(uint8_t original_sender_id, uint8_t msg_idx) {
  (void)original_sender_id;   // Not embedded in packet; reserved for future use
  uint8_t pkt[PKT_SIZE];
  pkt[PKT_OFFSET_ID]   = DEVICE_ID;
  pkt[PKT_OFFSET_TYPE] = PKT_TYPE_ACK;
  pkt[PKT_OFFSET_IDX]  = msg_idx;
  pkt[PKT_OFFSET_CRC]  = crc8_maxim(pkt, 3);
  return radio.transmit(pkt, PKT_SIZE);
}

/**
 * @brief Read and validate a packet from the SX1262 FIFO.
 * @details Validates CRC-8, checks msg_idx range (for MSG type).
 *          Populates out.rssi, out.snr, and RTC timestamp on success.
 * @param[out] out        Populated on success.
 * @param[out] pkt_type   Set to PKT_TYPE_MSG or PKT_TYPE_ACK on success.
 * @return true = packet valid.
 */
static bool lora_read_packet(RxRecord& out, uint8_t& pkt_type) {
  uint8_t buf[PKT_SIZE] = {0};
  int len = PKT_SIZE;
  int state = radio.readData(buf, len);
  if (state != RADIOLIB_ERR_NONE) {
    Serial.printf("[RX] readData err=%d\n", state);
    return false;
  }
  uint8_t crc_calc = crc8_maxim(buf, 3);
  if (crc_calc != buf[PKT_OFFSET_CRC]) {
    Serial.printf("[RX] CRC fail calc=0x%02X rx=0x%02X\n",
                  crc_calc, buf[PKT_OFFSET_CRC]);
    return false;
  }
  pkt_type         = buf[PKT_OFFSET_TYPE];
  out.sender_id    = buf[PKT_OFFSET_ID];
  out.msg_idx      = buf[PKT_OFFSET_IDX];
  out.rssi         = radio.getRSSI();
  out.snr          = radio.getSNR();
  out.valid        = true;
  rtc_now_hms(out.rx_hh, out.rx_mm, out.rx_ss);

  if (pkt_type == PKT_TYPE_MSG && out.msg_idx >= MSG_COUNT) {
    Serial.printf("[RX] MSG idx out of range: %d\n", out.msg_idx);
    return false;
  }
  Serial.printf("[RX] type=0x%02X from=0x%02X idx=%d RSSI=%.1f SNR=%.1f\n",
                pkt_type, out.sender_id, out.msg_idx, out.rssi, out.snr);
  return true;
}

// ============================================================
// 9a. SETUP
// ============================================================

/**
 * @brief Hardware initialisation. Peripheral power-on order is critical.
 * @details Order: GPIO → I2C0 → PCA9557 (power rail) → SPI → EPD (splash)
 *          → I2C1 → RTC → LDO → LoRa → ISR → startReceive.
 */
void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 3000) {}
  Serial.printf("=== ThinkNode M5 LoRa Comm v2.0  ID=0x%02X ===\n", DEVICE_ID);

  // GPIO
  pinMode(PIN_BTN1,        INPUT_PULLUP);
  pinMode(PIN_BTN2,        INPUT_PULLUP);
  pinMode(PIN_LORA_PWR_EN, OUTPUT);
  digitalWrite(PIN_LORA_PWR_EN, LOW);
  analogReadResolution(12);
  analogSetAttenuation(ADC_11db);
  ledcAttach(PIN_BUZZER, 2000, 8);

  // I2C0 — PCA9557
  Wire.begin(PIN_I2C_SDA, PIN_I2C_SCL);
  Wire.setClock(400000);

  if (!pca_init()) {
    Serial.println("[FATAL] PCA9557 not found. Halting.");
    ledcWriteTone(PIN_BUZZER, 300); delay(3000); ledcWriteTone(PIN_BUZZER, 0);
    while (true) delay(1000);
  }
  pca_pin_write(PCA_POWER_EN,   true);
  pca_pin_write(PCA_LED_ENABLE, true);
  delay(50);
  Serial.println("[PCA9557] OK");

  // SPI buses
  spi_lora.begin(PIN_LORA_SCK, PIN_LORA_MISO, PIN_LORA_MOSI, PIN_LORA_CS);
  spi_epd.begin(PIN_EPD_SCK, -1, PIN_EPD_MOSI, PIN_EPD_CS);

  // EPD — splash screen
  pca_pin_write(PCA_EINK_EN, true); delay(10);
  display.epd2.selectSPI(spi_epd, SPISettings(4000000, MSBFIRST, SPI_MODE0));
  display.init(115200, true, 2, false);
  display.setRotation(EPD_ROTATION);
  display.setFullWindow();
  display.firstPage();
  do {
    display.fillScreen(GxEPD_WHITE);
    display.setFont(&FreeMonoBold9pt7b);
    display.setTextColor(GxEPD_BLACK);
    display.setCursor(10, 40);  display.print("Electgpl");
    display.setCursor(10, 65);  display.print("LoRa Comm v2");
    display.setFont(&FreeMono9pt7b);
    display.setCursor(10, 95);  display.print("Initialising...");
    char b[20];
    snprintf(b, sizeof(b), "Device: 0x%02X", DEVICE_ID);
    display.setCursor(10, 115); display.print(b);
    snprintf(b, sizeof(b), "%.1fMHz SF%d", LORA_FREQ_MHZ, LORA_SF);
    display.setCursor(10, 133); display.print(b);
    display.setCursor(10, 151); display.print("ACK protocol v2");
  } while (display.nextPage());
  pca_pin_write(PCA_EINK_EN, false);
  Serial.println("[EPD] OK");

  // I2C1 — PCF8563 RTC
  Wire1_rtc.begin(PIN_RTC_SDA, PIN_RTC_SCL);
  Wire1_rtc.setClock(400000);
  if (!rtc.begin(&Wire1_rtc)) {
    Serial.println("[WARN] PCF8563 not found");
  } else {
    if (rtc.lostPower()) {
      rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
      Serial.println("[RTC] Adjusted to compile time");
    }
    DateTime now = rtc.now();
    Serial.printf("[RTC] %04d-%02d-%02d %02d:%02d:%02d\n",
      now.year(), now.month(), now.day(),
      now.hour(), now.minute(), now.second());
  }

  // SX1262 LDO + init
  digitalWrite(PIN_LORA_PWR_EN, HIGH); delay(20);
  int state = radio.begin(LORA_FREQ_MHZ, LORA_BW_KHZ, LORA_SF, LORA_CR,
                           LORA_SYNC_WORD, LORA_POWER_DBM, LORA_PREAMBLE);
  if (state != RADIOLIB_ERR_NONE) {
    Serial.printf("[FATAL] radio.begin()=%d\n", state);
    buzzer_error(); while (true) delay(1000);
  }
  radio.setDio2AsRfSwitch(true);
  radio.setTCXO(LORA_TCXO_V, LORA_TCXO_TICKS);
  Serial.println("[LoRa] OK");

  // ISR
  radio.setDio1Action(isr_radio_done);

  // Ready
  buzzer_tone(800, 70); delay(30); buzzer_tone(1200, 100);
  led_blink(2, 150, 100);

  // Initial ADC read
  bat_read_pct();

  // Show menu (blocking ~2 s EPD refresh)
  app_state = AppState::IDLE_MENU;
  epd_draw_menu(selected_msg_idx);

  // Arm RX AFTER all blocking inits (avoids stale rx_flag)
  state = lora_start_rx();
  if (state != RADIOLIB_ERR_NONE)
    Serial.printf("[WARN] startReceive()=%d\n", state);

  Serial.println("[INIT] Done. RX armed.");
}

// ============================================================
// 9b. LOOP
// ============================================================

/**
 * @brief Main FSM loop — ~50 Hz tick.
 *
 * @details Button debounce: falling edge captures timestamp; rising edge
 *          after >= DEBOUNCE_MS fires single-shot action. This avoids the
 *          v1.x bug where b_prev was updated before the action condition
 *          was checked in the same tick.
 *
 *          Heartbeat: blue LED pulses every HEARTBEAT_MS non-blocking.
 *
 *          EPD backlight blink: tick_epd_blink() advances the non-blocking
 *          blink state machine every iteration.
 *
 *          ACK flow:
 *            1. BTN2 → lora_send_msg() (blocking TX)
 *            2. State → TX_WAIT_ACK; show "waiting ACK" EPD
 *            3. lora_start_rx(); arm for ACK
 *            4. Loop polls rx_flag until ACK or timeout
 *            5. Show TX result; return to IDLE_MENU
 *
 *          ACK responder (in IDLE_MENU RX path):
 *            On receiving PKT_TYPE_MSG → lora_send_ack() immediately,
 *            then show incoming message screen.
 */
void loop() {
  static bool     b1_prev    = HIGH, b2_prev  = HIGH;
  static uint32_t b1_down    = 0,    b2_down  = 0;
  static uint32_t result_t   = 0;   ///< Timestamp for auto-dismiss
  static uint32_t hb_t       = 0;   ///< Heartbeat timer

  const uint32_t DEBOUNCE_MS = 60;

  bool b1 = digitalRead(PIN_BTN1);
  bool b2 = digitalRead(PIN_BTN2);

  if (b1 == LOW  && b1_prev == HIGH) b1_down = millis();
  if (b2 == LOW  && b2_prev == HIGH) b2_down = millis();

  bool b1_press = (b1 == HIGH && b1_prev == LOW && (millis()-b1_down) >= DEBOUNCE_MS);
  bool b2_press = (b2 == HIGH && b2_prev == LOW && (millis()-b2_down) >= DEBOUNCE_MS);

  // ---- Non-blocking heartbeat LED ----
  if (millis() - hb_t >= HEARTBEAT_MS) {
    hb_t = millis();
    pca_pin_write(PCA_LED_USER, true);
    delay(HEARTBEAT_PULSE_MS);        // 80 ms blocking — imperceptible to UX
    pca_pin_write(PCA_LED_USER, false);
    Serial.printf("[HB] state=%d sel=%d bat=%d%%\n",
                  (int)app_state, selected_msg_idx, bat_read_pct());
  }

  // ---- EPD blink tick ----
  tick_epd_blink();

  // ==========================================================
  // STATE: IDLE_MENU
  // ==========================================================
  if (app_state == AppState::IDLE_MENU) {

    // BTN1: cycle messages; when past last message → HISTORY_VIEW
    if (b1_press) {
      if (selected_msg_idx < MSG_COUNT - 1) {
        selected_msg_idx++;
        buzzer_click();
        char msg[32];
        Serial.printf("[BTN1] msg %d: %s\n", selected_msg_idx+1,
                      msg_get(selected_msg_idx, msg, sizeof(msg)));
        epd_draw_menu(selected_msg_idx);
      } else {
        // Wrap: go to history view
        buzzer_click();
        Serial.println("[BTN1] -> HISTORY_VIEW");
        app_state = AppState::HISTORY_VIEW;
        epd_draw_history();
      }
    }

    // BTN2: transmit selected message
    if (b2_press) {
      rtc_now_hms(last_tx.tx_hh, last_tx.tx_mm, last_tx.tx_ss);
      last_tx.msg_idx   = selected_msg_idx;
      last_ack_result   = AckResult::PENDING;
      app_state         = AppState::TX_WAIT_ACK;

      Serial.printf("[TX] Sending msg %d\n", selected_msg_idx+1);
      int tx_err = lora_send_msg(selected_msg_idx);   // Blocking ~2.8 s

      if (tx_err != RADIOLIB_ERR_NONE) {
        Serial.printf("[TX] Error %d\n", tx_err);
        buzzer_error();
        epd_draw_tx_err(tx_err);
        lora_start_rx();
        app_state = AppState::IDLE_MENU;
        result_t  = millis();
        // Return to menu after brief pause
        delay(2000);
        epd_draw_menu(selected_msg_idx);
      } else {
        Serial.println("[TX] Packet sent, arming ACK RX");
        buzzer_tx_sent();
        led_blink(2, 80, 60);
        lora_start_rx();              // Arm RX to catch ACK
        epd_draw_tx_waiting(selected_msg_idx);
        result_t = millis();          // Start ACK timeout timer
        // Stay in TX_WAIT_ACK; polling happens below on next iterations
      }
    }

    // RX while in menu
    if (rx_flag) {
      noInterrupts(); rx_flag = false; interrupts();
      uint8_t pkt_type = 0;
      RxRecord r; r.valid = false;
      if (lora_read_packet(r, pkt_type)) {
        if (pkt_type == PKT_TYPE_MSG) {
          // Store in history
          history_push(r);
          last_rx = r;

          // Auto-send ACK immediately
          Serial.printf("[ACK] Sending ACK for msg %d from 0x%02X\n",
                        r.msg_idx, r.sender_id);
          int ack_err = lora_send_ack(r.sender_id, r.msg_idx);
          if (ack_err != RADIOLIB_ERR_NONE)
            Serial.printf("[ACK] TX err=%d\n", ack_err);

          lora_start_rx();   // Re-arm after ACK TX

          // Notify user
          led_blink(4, 80, 60);
          buzzer_rx_notify();
          epd_draw_rx(last_rx);
          epd_blink_start(RX_BLINK_COUNT);   // Non-blocking backlight blink
          app_state = AppState::RX_RECEIVED;
          result_t  = millis();
        }
        // Ignore stale ACKs in IDLE_MENU
      } else {
        lora_start_rx();
      }
    }
  }

  // ==========================================================
  // STATE: TX_WAIT_ACK
  // ==========================================================
  else if (app_state == AppState::TX_WAIT_ACK) {
    bool timed_out = (millis() - result_t) > ACK_TIMEOUT_MS;

    if (rx_flag) {
      noInterrupts(); rx_flag = false; interrupts();
      uint8_t pkt_type = 0;
      RxRecord r; r.valid = false;
      if (lora_read_packet(r, pkt_type)) {
        if (pkt_type == PKT_TYPE_ACK &&
            r.msg_idx == last_tx.msg_idx) {
          // Correct ACK received
          last_ack_result = AckResult::OK;
          Serial.printf("[ACK] OK from 0x%02X RSSI=%.1f\n",
                        r.sender_id, r.rssi);
          buzzer_ack_ok();
          led_blink(3, 60, 50);
          lora_start_rx();
          epd_draw_tx_result(last_tx.msg_idx, AckResult::OK);
          app_state = AppState::IDLE_MENU;
          result_t  = millis();
          delay(RESULT_DISPLAY_MS);
          epd_draw_menu(selected_msg_idx);
          return;
        } else if (pkt_type == PKT_TYPE_MSG) {
          // Someone else sent us a message while we waited for ACK
          // Store it in history for later; do not display now
          history_push(r);
          Serial.println("[RX] MSG received during ACK wait — stored in history");
          lora_start_rx();
        } else {
          lora_start_rx();
        }
      } else {
        lora_start_rx();
      }
    }

    if (timed_out) {
      last_ack_result = AckResult::TIMEOUT;
      Serial.println("[ACK] TIMEOUT");
      buzzer_ack_fail();
      lora_start_rx();
      epd_draw_tx_result(last_tx.msg_idx, AckResult::TIMEOUT);
      app_state = AppState::IDLE_MENU;
      delay(RESULT_DISPLAY_MS);
      epd_draw_menu(selected_msg_idx);
    }
  }

  // ==========================================================
  // STATE: RX_RECEIVED
  // ==========================================================
  else if (app_state == AppState::RX_RECEIVED) {
    bool timed_out    = (millis() - result_t) > RESULT_DISPLAY_MS;
    bool btn2_dismiss = b2_press;

    if (timed_out || btn2_dismiss) {
      if (btn2_dismiss) buzzer_click();
      // EPD blink sequence may still be running — stop it cleanly
      epd_blink.active = false;
      pca_pin_write(PCA_EINK_EN, false);
      app_state = AppState::IDLE_MENU;
      epd_draw_menu(selected_msg_idx);
    }

    // New message arriving while displaying current one
    if (rx_flag) {
      noInterrupts(); rx_flag = false; interrupts();
      uint8_t pkt_type = 0;
      RxRecord r; r.valid = false;
      if (lora_read_packet(r, pkt_type) && pkt_type == PKT_TYPE_MSG) {
        history_push(r);
        last_rx = r;
        int ack_err = lora_send_ack(r.sender_id, r.msg_idx);
        if (ack_err != RADIOLIB_ERR_NONE)
          Serial.printf("[ACK] TX err=%d\n", ack_err);
        lora_start_rx();
        buzzer_rx_notify();
        epd_draw_rx(last_rx);
        // Restart blink sequence for the new message
        epd_blink_start(RX_BLINK_COUNT);
        result_t = millis();
      } else {
        lora_start_rx();
      }
    }
  }

  // ==========================================================
  // STATE: HISTORY_VIEW
  // ==========================================================
  else if (app_state == AppState::HISTORY_VIEW) {
    // BTN1 in history: wrap back to msg index 0
    if (b1_press) {
      selected_msg_idx = 0;
      buzzer_click();
      app_state = AppState::IDLE_MENU;
      epd_draw_menu(selected_msg_idx);
    }
    // BTN2: back to menu (keep current message selection)
    if (b2_press) {
      buzzer_click();
      app_state = AppState::IDLE_MENU;
      epd_draw_menu(selected_msg_idx);
    }
    // New RX while in history view — store silently
    if (rx_flag) {
      noInterrupts(); rx_flag = false; interrupts();
      uint8_t pkt_type = 0;
      RxRecord r; r.valid = false;
      if (lora_read_packet(r, pkt_type) && pkt_type == PKT_TYPE_MSG) {
        history_push(r);
        int ack_err = lora_send_ack(r.sender_id, r.msg_idx);
        if (ack_err != RADIOLIB_ERR_NONE)
          Serial.printf("[ACK] TX err=%d\n", ack_err);
        lora_start_rx();
        buzzer_rx_notify();
        // Refresh history display with new entry
        epd_draw_history();
        epd_blink_start(RX_BLINK_COUNT);
      } else {
        lora_start_rx();
      }
    }
  }

  b1_prev = b1;
  b2_prev = b2;
  delay(20);
}
