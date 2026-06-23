#include <Arduino.h>
#include <WiFi.h>
#include <SinricPro.h>
#include <SinricProSwitch.h>
#include <IRremote.hpp>

// --- Credentials ---
#define WIFI_SSID          "CRANIUM"
#define WIFI_PASS          "craniumashwin"
#define APP_KEY            "4038e8f9-e6ee-44e9-afca-ff5054968380"
#define APP_SECRET         "ee30460b-2dcc-4711-8213-4d4bc28c37d9-fee325dc-18e4-42fb-abd2-55197e76193d"

#define SWITCH_ID_1        "696b472f92e412384abddefe"
#define SWITCH_ID_2        "696b4749f52b46f1590b9e32"
#define SWITCH_ID_3        "696b475af52b46f1590b9e62"

const int FIRE_THRESHOLD = 4100; 

// --- Pins ---
const int IR_RECEIVE_PIN = 15;
const int BUZZER_PIN     = 13;
const int GAS_SENSOR_PIN = 34;

const int LED_PINS[]         = {2, 4, 5};         // Relay Pins
const int WALL_SWITCH_PINS[] = {14, 27, 26};      // Physical Switch Pins

// --- Global Variables ---
unsigned long lastTempRequest = 0;
const unsigned long tempInterval = 5000;
bool ledStates[] = {false, false, false};
bool lastWallSwitchStates[] = {HIGH, HIGH, HIGH};

// --- Emergency Logic Variables ---
bool isAlarmActive = false;
unsigned long alarmStartTime = 0;
const unsigned long ALARM_DURATION = 30000; // 30 seconds
const uint32_t REMOTE_OK_CODE = 0xEA15FF00; // <--- UPDATE THIS with your OK button hex code

SinricProSwitch& mySwitch1 = SinricPro[SWITCH_ID_1];
SinricProSwitch& mySwitch2 = SinricPro[SWITCH_ID_2];
SinricProSwitch& mySwitch3 = SinricPro[SWITCH_ID_3];

// --- Helper Functions ---
void beep(int duration) {
    digitalWrite(BUZZER_PIN, HIGH);
    delay(duration);
    digitalWrite(BUZZER_PIN, LOW);
}

void masterOff() {
    Serial.println("Executing Master Off...");
    for (int i = 0; i < 3; i++) {
        ledStates[i] = false;
        digitalWrite(LED_PINS[i], LOW);
        if (WiFi.status() == WL_CONNECTED && SinricPro.isConnected()) {
            if(i==0) mySwitch1.sendPowerStateEvent(false);
            if(i==1) mySwitch2.sendPowerStateEvent(false);
            if(i==2) mySwitch3.sendPowerStateEvent(false);
        }
    }
}

void readGasLevel() {
    int gasValue = analogRead(GAS_SENSOR_PIN);
    // Trigger alarm if gas is high and alarm isn't already running
    if (gasValue > FIRE_THRESHOLD && !isAlarmActive) {
        Serial.println("!!! GAS DETECTED: 30s COUNTDOWN STARTED !!!");
        isAlarmActive = true;
        alarmStartTime = millis();
    }
}

void manageAlarmState() {
    if (!isAlarmActive) return;

    unsigned long elapsed = millis() - alarmStartTime;

    if (elapsed < ALARM_DURATION) {
        // Warning Beep Pattern (500ms intervals)
        if ((elapsed / 500) % 2 == 0) digitalWrite(BUZZER_PIN, HIGH);
        else digitalWrite(BUZZER_PIN, LOW);
    } 
    else {
        // TIME EXPIRED: Safety Shutdown
        Serial.println("!!! EMERGENCY SHUTDOWN: NO USER RESPONSE !!!");
        isAlarmActive = false;
        digitalWrite(BUZZER_PIN, LOW);
        masterOff();
        
        // Long continuous beep to signal shutdown completion
        digitalWrite(BUZZER_PIN, HIGH);
        delay(2000);
        digitalWrite(BUZZER_PIN, LOW);
    }
}

bool onPowerState(const String &deviceId, bool &state, int index) {
    ledStates[index] = state;
    digitalWrite(LED_PINS[index], state ? HIGH : LOW); 
    Serial.printf("Sinric: Relay %d turned %s\n", index + 1, state ? "ON" : "OFF");
    return true;
}

void toggleLED(int index, SinricProSwitch& mySwitch) {
    ledStates[index] = !ledStates[index];
    digitalWrite(LED_PINS[index], ledStates[index] ? HIGH : LOW);
    beep(100);
    
    if (WiFi.status() == WL_CONNECTED && SinricPro.isConnected()) {
        mySwitch.sendPowerStateEvent(ledStates[index]);
    }
    Serial.printf("Toggled Relay %d: %s\n", index + 1, ledStates[index] ? "ON" : "OFF");
}

void handleWallSwitches() {
    for (int i = 0; i < 3; i++) {
        bool currentSwitchState = digitalRead(WALL_SWITCH_PINS[i]);
        if (currentSwitchState != lastWallSwitchStates[i]) {
            delay(50); // Debounce
            currentSwitchState = digitalRead(WALL_SWITCH_PINS[i]);
            if (currentSwitchState != lastWallSwitchStates[i]) {
                lastWallSwitchStates[i] = currentSwitchState;
                if (i == 0) toggleLED(0, mySwitch1);
                else if (i == 1) toggleLED(1, mySwitch2);
                else if (i == 2) toggleLED(2, mySwitch3);
            }
        }
    }
}

void handleButton(uint32_t code) {
    // Check for OK button to neglect/reset alarm
    if (isAlarmActive && code == REMOTE_OK_CODE) {
        isAlarmActive = false;
        digitalWrite(BUZZER_PIN, LOW);
        Serial.println("Alarm Neglected: User signaled safety.");
        beep(300); 
        return;
    }

    switch (code) {
        case 0xBA45FF00: toggleLED(0, mySwitch1); break;
        case 0xB946FF00: toggleLED(1, mySwitch2); break;
        case 0xB847FF00: toggleLED(2, mySwitch3); break;

        case 0xE916FF00: // Manual Master Off
            beep(400);
            masterOff();
            break;

        case 0xF20DFF00: // Master On
            beep(100); delay(50); beep(100);
            for (int i = 0; i < 3; i++) {
                ledStates[i] = true;
                digitalWrite(LED_PINS[i], HIGH);
                if (WiFi.status() == WL_CONNECTED && SinricPro.isConnected()) {
                    if(i==0) mySwitch1.sendPowerStateEvent(true);
                    if(i==1) mySwitch2.sendPowerStateEvent(true);
                    if(i==2) mySwitch3.sendPowerStateEvent(true);
                }
            }
            break;
        default: break;
    }
}

void setup() {
    Serial.begin(115200);
    for(int i = 0; i < 3; i++) {
        digitalWrite(LED_PINS[i], LOW);
        pinMode(LED_PINS[i], OUTPUT);
        pinMode(WALL_SWITCH_PINS[i], INPUT_PULLUP);
        lastWallSwitchStates[i] = digitalRead(WALL_SWITCH_PINS[i]);
    }
    pinMode(BUZZER_PIN, OUTPUT);
    analogReadResolution(12);
    IrReceiver.begin(IR_RECEIVE_PIN, DISABLE_LED_FEEDBACK);

    WiFi.begin(WIFI_SSID, WIFI_PASS);

    mySwitch1.onPowerState([](const String &id, bool &st) { return onPowerState(id, st, 0); });
    mySwitch2.onPowerState([](const String &id, bool &st) { return onPowerState(id, st, 1); });
    mySwitch3.onPowerState([](const String &id, bool &st) { return onPowerState(id, st, 2); });

    SinricPro.begin(APP_KEY, APP_SECRET);
}

void loop() {
    manageAlarmState(); // Constant background alarm check
    handleWallSwitches();

    if (IrReceiver.decode()) {
        uint32_t receivedCode = IrReceiver.decodedIRData.decodedRawData;
        if (receivedCode != 0) handleButton(receivedCode);
        IrReceiver.resume();
    }

    if (millis() - lastTempRequest >= tempInterval) {
        readGasLevel();
        lastTempRequest = millis();
    }

    if (WiFi.status() == WL_CONNECTED) {
        SinricPro.handle();
    }
}
