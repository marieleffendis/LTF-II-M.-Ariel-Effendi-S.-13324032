#include <ESP32Servo.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include <esp_now.h>
#include <WiFi.h>

#define PWMA      19    
#define AI1       17    
#define AI2       18    

#define SERVO_PIN 32

typedef struct {
    float  x;        
    float  y;        
    bool   roller;   
    bool   servo;    
} JoyMessage;

JoyMessage g_rxData   = {0.0f, 0.0f, false, false};
volatile uint32_t   g_lastRecvMs = 0;      

SemaphoreHandle_t xSerialSemaphore; 
SemaphoreHandle_t xRxMutex;         

Servo PWMServo;

void safePrint(const char *msg) {
    if (xSerialSemaphore && xSemaphoreTake(xSerialSemaphore, pdMS_TO_TICKS(10)) == pdTRUE) {
        Serial.println(msg);
        xSemaphoreGive(xSerialSemaphore);
    }
}

void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len) {
    if (len != sizeof(JoyMessage)) return;

    JoyMessage incoming;
    memcpy(&incoming, incomingData, sizeof(JoyMessage));

    if (xRxMutex && xSemaphoreTake(xRxMutex, pdMS_TO_TICKS(5)) == pdTRUE) {
        g_rxData     = incoming;
        g_lastRecvMs = millis(); 
        xSemaphoreGive(xRxMutex);
    }
}

void setup() {
    Serial.begin(115200);

    pinMode(PWMA, OUTPUT);
    pinMode(AI1, OUTPUT);
    pinMode(AI2, OUTPUT);
    digitalWrite(AI1, LOW);
    digitalWrite(AI2, LOW);
    analogWrite(PWMA, 0);

    PWMServo.setPeriodHertz(50);
    PWMServo.attach(SERVO_PIN, 500, 2400); 
    PWMServo.write(0);

    xSerialSemaphore = xSemaphoreCreateMutex();
    xRxMutex         = xSemaphoreCreateMutex();

    WiFi.mode(WIFI_STA);
    WiFi.disconnect();

    if (esp_now_init() != ESP_OK) {
        Serial.println("ESP-NOW Gagal!");
        esp_restart();
    }

    esp_now_register_recv_cb(OnDataRecv);

    xTaskCreatePinnedToCore(TaskConnectionESP32Receiver, "Safety_Task", 2048, NULL, 7, NULL, 0);
    xTaskCreatePinnedToCore(TaskFlywheelMove, "Flywheel_Task", 4096, NULL, 6, NULL, 1);
    xTaskCreatePinnedToCore(TaskServoMove, "Servo_Task", 2048, NULL, 5, NULL, 1);

    safePrint("System Receiver Siap!");
}

void loop() { vTaskDelay(pdMS_TO_TICKS(1000)); }

// ==================== TASK IMPLEMENTATIONS ====================

void TaskConnectionESP32Receiver(void *pvParameters) {
    for (;;) {
        if (millis() - g_lastRecvMs > 500) { // Timeout 0.5 detik
            if (xSemaphoreTake(xRxMutex, pdMS_TO_TICKS(10)) == pdTRUE) {
                g_rxData = {0.0f, 0.0f, false, false}; // Reset data
                xSemaphoreGive(xRxMutex);
            }
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

void TaskFlywheelMove(void *pvParameters) {
    for (;;) {
        bool runRoller = false;
        
        if (xSemaphoreTake(xRxMutex, pdMS_TO_TICKS(10)) == pdTRUE) {
            runRoller = g_rxData.roller;
            xSemaphoreGive(xRxMutex);
        }

        if (runRoller) {
            digitalWrite(AI1, HIGH);
            digitalWrite(AI2, LOW);
            analogWrite(PWMA, 200); 
        } else {
            digitalWrite(AI1, LOW);
            digitalWrite(AI2, LOW);
            analogWrite(PWMA, 0);
        }
        vTaskDelay(pdMS_TO_TICKS(20));
    }
}

void TaskServoMove(void *pvParameters) {
    bool lastState = false;
    for (;;) {
        bool currentState = false;

        if (xSemaphoreTake(xRxMutex, pdMS_TO_TICKS(10)) == pdTRUE) {
            currentState = g_rxData.servo;
            xSemaphoreGive(xRxMutex);
        }

        if (currentState != lastState) {
            PWMServo.write(currentState ? 90 : 0);
            lastState = currentState;
            safePrint(currentState ? "Servo: BUKA" : "Servo: TUTUP");
        }
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}