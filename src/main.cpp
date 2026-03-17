/*=============================================================================
 * Temperature Controlled Air Conditioning System
 * Author: Daggolu Hari Krishna
 * Platform: Arduino Uno / Nano
 *
 * Automatically controls fan/compressor relay based on temperature readings.
 * Features PID-like control with hysteresis, LCD display, and mode selection.
 *
 * Components:
 *  - DHT11 Temperature & Humidity Sensor
 *  - 16x2 I2C LCD Display
 *  - 2-Channel Relay Module (Fan + Compressor)
 *  - Potentiometer for set-point adjustment
 *  - Mode selection button (Auto/Manual/Off)
 *  - Status LEDs
 *=============================================================================*/

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>
#include <EEPROM.h>

// ==================== Pin Definitions ====================
#define DHT_PIN        2     // DHT11 data
#define DHT_TYPE       DHT11
#define FAN_RELAY      8     // Fan relay control
#define COMP_RELAY     9     // Compressor relay control
#define POT_PIN        A0    // Setpoint potentiometer
#define MODE_BTN       3     // Mode selection button
#define LED_COOL       5     // Cooling indicator (Blue)
#define LED_IDLE       6     // Idle indicator (Green)
#define LED_WARN       7     // Warning indicator (Red)

// ==================== Configuration ====================
#define HYSTERESIS     2.0   // Temperature hysteresis (°C)
#define MIN_SETPOINT   18.0  // Minimum setpoint (°C)
#define MAX_SETPOINT   32.0  // Maximum setpoint (°C)
#define COMP_DELAY     180000 // Compressor restart delay (3 min) for protection
#define FAN_RUN_AFTER  30000  // Fan runs 30s after compressor stops
#define READ_INTERVAL  2000   // Sensor read interval (ms)
#define EEPROM_ADDR    0      // EEPROM address for saved setpoint

// ==================== Objects ====================
LiquidCrystal_I2C lcd(0x27, 16, 2);
DHT dht(DHT_PIN, DHT_TYPE);

// ==================== Enums ====================
enum SystemMode {
    MODE_AUTO,
    MODE_MANUAL_COOL,
    MODE_OFF
};

enum CoolingState {
    COOLING_OFF,
    FAN_ONLY,
    FULL_COOLING,
    FAN_RUNDOWN  // Fan runs after compressor stops
};

// ==================== Global Variables ====================
float currentTemp      = 0.0;
float currentHumidity  = 0.0;
float setpoint         = 25.0;  // Default 25°C

SystemMode  systemMode     = MODE_AUTO;
CoolingState coolingState  = COOLING_OFF;

unsigned long lastRead       = 0;
unsigned long lastCompStop   = 0;
unsigned long fanRundownStart = 0;
bool compressorLocked        = false;
bool lastBtnState            = HIGH;
unsigned long lastDebounce   = 0;

// Custom LCD characters
byte thermChar[8] = {0x04, 0x0A, 0x0A, 0x0A, 0x0A, 0x1F, 0x1F, 0x0E};
byte snowChar[8]  = {0x04, 0x15, 0x0E, 0x04, 0x0E, 0x15, 0x04, 0x00};
byte fanChar[8]   = {0x00, 0x11, 0x0A, 0x04, 0x0A, 0x11, 0x00, 0x00};

// ==================== Setup ====================
void setup() {
    Serial.begin(9600);
    Serial.println(F("========================================"));
    Serial.println(F("  Temperature Controlled AC System"));
    Serial.println(F("  Author: Daggolu Hari Krishna"));
    Serial.println(F("========================================"));

    // Pin initialization
    pinMode(FAN_RELAY, OUTPUT);
    pinMode(COMP_RELAY, OUTPUT);
    pinMode(LED_COOL, OUTPUT);
    pinMode(LED_IDLE, OUTPUT);
    pinMode(LED_WARN, OUTPUT);
    pinMode(MODE_BTN, INPUT_PULLUP);

    // Relays OFF initially (active LOW)
    digitalWrite(FAN_RELAY, HIGH);
    digitalWrite(COMP_RELAY, HIGH);

    // Sensor & display init
    dht.begin();
    lcd.init();
    lcd.backlight();
    lcd.createChar(0, thermChar);
    lcd.createChar(1, snowChar);
    lcd.createChar(2, fanChar);

    // Load saved setpoint from EEPROM
    float savedSetpoint;
    EEPROM.get(EEPROM_ADDR, savedSetpoint);
    if (savedSetpoint >= MIN_SETPOINT && savedSetpoint <= MAX_SETPOINT) {
        setpoint = savedSetpoint;
    }

    // Splash
    lcd.setCursor(0, 0);
    lcd.print("Temp Ctrl AC");
    lcd.setCursor(0, 1);
    lcd.print("Hari Krishna");
    delay(2000);
    lcd.clear();

    Serial.println(F("[OK] System ready"));
}

// ==================== Main Loop ====================
void loop() {
    unsigned long now = millis();

    // Handle mode button
    handleModeButton();

    // Read setpoint from potentiometer
    readSetpoint();

    // Read sensors at interval
    if (now - lastRead >= READ_INTERVAL) {
        readSensor();
        controlLogic();
        updateDisplay();
        updateLEDs();
        logData();
        lastRead = now;
    }

    // Handle compressor protection timer
    handleCompressorProtection();

    // Handle fan rundown
    handleFanRundown();
}

// ==================== Mode Button ====================
void handleModeButton() {
    bool btnState = digitalRead(MODE_BTN);

    if (btnState == LOW && lastBtnState == HIGH && (millis() - lastDebounce > 250)) {
        lastDebounce = millis();

        // Cycle: AUTO → MANUAL_COOL → OFF → AUTO
        switch (systemMode) {
            case MODE_AUTO:        systemMode = MODE_MANUAL_COOL; break;
            case MODE_MANUAL_COOL: systemMode = MODE_OFF; break;
            case MODE_OFF:         systemMode = MODE_AUTO; break;
        }

        Serial.print(F("[MODE] "));
        switch (systemMode) {
            case MODE_AUTO:        Serial.println(F("AUTO")); break;
            case MODE_MANUAL_COOL: Serial.println(F("MANUAL")); break;
            case MODE_OFF:         Serial.println(F("OFF")); break;
        }
    }
    lastBtnState = btnState;
}

// ==================== Setpoint Reading ====================
void readSetpoint() {
    int potValue = analogRead(POT_PIN);
    float newSetpoint = map(potValue, 0, 1023, MIN_SETPOINT * 10, MAX_SETPOINT * 10) / 10.0;

    // Only update & save if changed significantly
    if (abs(newSetpoint - setpoint) > 0.5) {
        setpoint = newSetpoint;
        EEPROM.put(EEPROM_ADDR, setpoint);
    }
}

// ==================== Sensor Reading ====================
void readSensor() {
    float t = dht.readTemperature();
    float h = dht.readHumidity();

    if (!isnan(t) && !isnan(h)) {
        currentTemp = t;
        currentHumidity = h;
    } else {
        Serial.println(F("[WARN] DHT read failed!"));
    }
}

// ==================== Control Logic ====================
void controlLogic() {
    switch (systemMode) {
        case MODE_OFF:
            setCompressor(false);
            setFan(false);
            coolingState = COOLING_OFF;
            break;

        case MODE_MANUAL_COOL:
            if (!compressorLocked) {
                setCompressor(true);
                setFan(true);
                coolingState = FULL_COOLING;
            }
            break;

        case MODE_AUTO:
            autoControl();
            break;
    }
}

void autoControl() {
    float upperThreshold = setpoint + HYSTERESIS;
    float lowerThreshold = setpoint - HYSTERESIS;

    switch (coolingState) {
        case COOLING_OFF:
            if (currentTemp > upperThreshold) {
                if (!compressorLocked) {
                    setFan(true);
                    setCompressor(true);
                    coolingState = FULL_COOLING;
                    Serial.println(F("[COOL] Started cooling"));
                } else {
                    // Compressor locked, fan only
                    setFan(true);
                    coolingState = FAN_ONLY;
                }
            }
            break;

        case FAN_ONLY:
            if (!compressorLocked && currentTemp > upperThreshold) {
                setCompressor(true);
                coolingState = FULL_COOLING;
            } else if (currentTemp <= setpoint) {
                setFan(false);
                coolingState = COOLING_OFF;
            }
            break;

        case FULL_COOLING:
            if (currentTemp <= lowerThreshold) {
                setCompressor(false);
                lastCompStop = millis();
                compressorLocked = true;
                fanRundownStart = millis();
                coolingState = FAN_RUNDOWN;
                Serial.println(F("[COOL] Target reached, stopping compressor"));
            }
            break;

        case FAN_RUNDOWN:
            if (millis() - fanRundownStart >= FAN_RUN_AFTER) {
                setFan(false);
                coolingState = COOLING_OFF;
                Serial.println(F("[COOL] Fan rundown complete"));
            }
            break;
    }
}

// ==================== Relay Control ====================
void setFan(bool state) {
    digitalWrite(FAN_RELAY, state ? LOW : HIGH);
}

void setCompressor(bool state) {
    if (state && compressorLocked) return; // Protection
    digitalWrite(COMP_RELAY, state ? LOW : HIGH);
}

// ==================== Compressor Protection ====================
void handleCompressorProtection() {
    if (compressorLocked && (millis() - lastCompStop >= COMP_DELAY)) {
        compressorLocked = false;
        Serial.println(F("[COMP] Protection timer expired — unlocked"));
    }
}

// ==================== Fan Rundown ====================
void handleFanRundown() {
    // Handled in autoControl switch
}

// ==================== LCD Display ====================
void updateDisplay() {
    lcd.setCursor(0, 0);
    lcd.write(0); // Thermometer
    lcd.print(currentTemp, 1);
    lcd.print((char)223); // Degree symbol
    lcd.print("C ");
    lcd.print(currentHumidity, 0);
    lcd.print("%");

    lcd.setCursor(0, 1);
    // Mode
    switch (systemMode) {
        case MODE_AUTO:
            lcd.print("A");
            break;
        case MODE_MANUAL_COOL:
            lcd.print("M");
            break;
        case MODE_OFF:
            lcd.print("X");
            break;
    }
    lcd.print(" Set:");
    lcd.print(setpoint, 1);
    lcd.print((char)223);

    // Cooling state indicator
    lcd.setCursor(15, 1);
    switch (coolingState) {
        case FULL_COOLING: lcd.write(1); break; // Snowflake
        case FAN_ONLY:     lcd.write(2); break; // Fan
        case FAN_RUNDOWN:  lcd.write(2); break;
        default:           lcd.print(" "); break;
    }
}

// ==================== LED Indicators ====================
void updateLEDs() {
    digitalWrite(LED_COOL, (coolingState == FULL_COOLING) ? HIGH : LOW);
    digitalWrite(LED_IDLE, (coolingState == COOLING_OFF)  ? HIGH : LOW);
    digitalWrite(LED_WARN, (currentTemp > MAX_SETPOINT)   ? HIGH : LOW);
}

// ==================== Serial Logging ====================
void logData() {
    Serial.print(F("[DATA] Temp="));
    Serial.print(currentTemp, 1);
    Serial.print(F("°C | Hum="));
    Serial.print(currentHumidity, 1);
    Serial.print(F("% | Set="));
    Serial.print(setpoint, 1);
    Serial.print(F("°C | Mode="));
    switch (systemMode) {
        case MODE_AUTO:        Serial.print(F("AUTO")); break;
        case MODE_MANUAL_COOL: Serial.print(F("MANUAL")); break;
        case MODE_OFF:         Serial.print(F("OFF")); break;
    }
    Serial.print(F(" | State="));
    switch (coolingState) {
        case COOLING_OFF:  Serial.println(F("IDLE")); break;
        case FAN_ONLY:     Serial.println(F("FAN")); break;
        case FULL_COOLING: Serial.println(F("COOLING")); break;
        case FAN_RUNDOWN:  Serial.println(F("RUNDOWN")); break;
    }
}
