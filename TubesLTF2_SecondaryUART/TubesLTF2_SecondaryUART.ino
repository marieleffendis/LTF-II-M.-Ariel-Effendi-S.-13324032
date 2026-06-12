#include <ESP32Servo.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include <WiFi.h>          
#include <WebServer.h>     

#define RPWM_A    33
#define LPWM_A    25
#define R_EN_A    26
#define L_EN_A    27

#define RPWM_B    14
#define LPWM_B    12
#define R_EN_B    13
#define L_EN_B    15

#define CHANNEL_A  4
#define CHANNEL_B  5

#define SERVO_PIN  18

#define UART_TX   16
#define UART_RX   17
#define UART_BAUD 115200
#define PACKET_HEADER 0xAB

// TAMBAHAN
// #define WIFI_SSID  "NamaWiFiKamu"
// #define WIFI_PASS  "PasswordWiFi"

typedef struct {
    float x;
    float y;
    bool  roller_in;   // X (silang)  = tangkap = mundur
    bool  roller_out;  // kotak       = tembak  = maju
    bool  servo;       // lingkaran   = servo
} JoyMessage;

JoyMessage        g_rxData     = {0.0f, 0.0f, false, false, false};
volatile uint32_t g_lastRecvMs = 0;

SemaphoreHandle_t xSerialSemaphore;
SemaphoreHandle_t xRxMutex;

Servo PWMServo;
WebServer server(80); 

void TaskConnectionESP32Receiver(void *pvParameters);
void TaskFlywheelMove(void *pvParameters);
void TaskServoMove(void *pvParameters);
void TaskHTTPServer(void *pvParameters);  

void safePrint(const char *msg) {
    if (xSerialSemaphore && xSemaphoreTake(xSerialSemaphore, pdMS_TO_TICKS(10)) == pdTRUE) {
        Serial.println(msg);
        xSemaphoreGive(xSerialSemaphore);
    }
}

void setup() {
    Serial.begin(115200);
    Serial1.begin(UART_BAUD, SERIAL_8N1, UART_RX, UART_TX);

    btStop();

    // Motor A
    pinMode(RPWM_A, OUTPUT);
    pinMode(LPWM_A, OUTPUT);
    pinMode(R_EN_A, OUTPUT);
    pinMode(L_EN_A, OUTPUT);
    digitalWrite(R_EN_A, HIGH);
    digitalWrite(L_EN_A, HIGH);

    ledcSetup(CHANNEL_A, 1000, 8);
    ledcSetup(CHANNEL_B, 1000, 8);  // channel B untuk LPWM_A arah mundur

    // Motor B
    pinMode(RPWM_B, OUTPUT);
    pinMode(LPWM_B, OUTPUT);
    pinMode(R_EN_B, OUTPUT);
    pinMode(L_EN_B, OUTPUT);
    digitalWrite(R_EN_B, HIGH);
    digitalWrite(L_EN_B, HIGH);

    // Definisikan 4 channel: A_maju, A_mundur, B_maju, B_mundur
    ledcSetup(0, 1000, 8); ledcAttachPin(RPWM_A, 0); // Motor A maju
    ledcSetup(1, 1000, 8); ledcAttachPin(LPWM_A, 1); // Motor A mundur
    ledcSetup(2, 1000, 8); ledcAttachPin(RPWM_B, 2); // Motor B maju
    ledcSetup(3, 1000, 8); ledcAttachPin(LPWM_B, 3); // Motor B mundur

    // Pastikan semua 0 di awal
    ledcWrite(0, 0); ledcWrite(1, 0);
    ledcWrite(2, 0); ledcWrite(3, 0);

    // --- Setup Servo ---
    PWMServo.setPeriodHertz(50);
    PWMServo.attach(SERVO_PIN, 500, 2400);
    PWMServo.write(0);

    xSerialSemaphore = xSemaphoreCreateMutex();
    xRxMutex         = xSemaphoreCreateMutex();

    // WiFi.begin(WIFI_SSID, WIFI_PASS);
    // Serial.print("Connecting to WiFi");
    // while (WiFi.status() != WL_CONNECTED) { delay(500); Serial.print("."); }
    // Serial.println("\nIP: " + WiFi.localIP().toString());
    // server.on("/status", []() {
    //     bool ps4ok = (millis() - g_lastRecvMs) < 1000;
    //     server.send(200, "application/json", ps4ok ? "{\"connected\":true}" : "{\"connected\":false}");
    // });
    // server.begin();

    xTaskCreatePinnedToCore(TaskConnectionESP32Receiver, "UART_Task",     2048, NULL, 7, NULL, 0);
    xTaskCreatePinnedToCore(TaskFlywheelMove,            "Flywheel_Task", 4096, NULL, 6, NULL, 1);
    xTaskCreatePinnedToCore(TaskServoMove,               "Servo_Task",    2048, NULL, 5, NULL, 1);
    // xTaskCreatePinnedToCore(TaskHTTPServer,              "HTTP_Task",     4096, NULL, 4, NULL, 0);  

    safePrint("Receiver Siap! (Dual BTS7960)");
}

void loop() { vTaskDelay(pdMS_TO_TICKS(1000)); }

void TaskConnectionESP32Receiver(void *pvParameters) {
    for (;;) {
        if (Serial1.available() > 0) {
            uint8_t incoming = Serial1.read();

            if (incoming != PACKET_HEADER) {
                safePrint("[UART] Misaligned, cari header...");
                vTaskDelay(pdMS_TO_TICKS(2));
                continue;
            }

            uint32_t waitStart = millis();
            while (Serial1.available() < (int)sizeof(JoyMessage)) {
                if (millis() - waitStart > 100) {
                    safePrint("[UART] Timeout paket tidak lengkap");
                    break;
                }
                vTaskDelay(pdMS_TO_TICKS(1));
            }

            if (Serial1.available() >= (int)sizeof(JoyMessage)) {
                JoyMessage msg;
                Serial1.readBytes((uint8_t *)&msg, sizeof(JoyMessage));

                if (xSemaphoreTake(xRxMutex, pdMS_TO_TICKS(5)) == pdTRUE) {
                    g_rxData     = msg;
                    g_lastRecvMs = millis();
                    xSemaphoreGive(xRxMutex);
                }

                char buf[80];
                snprintf(buf, sizeof(buf), "[UART] x=%.2f y=%.2f in=%d out=%d srv=%d",
                         msg.x, msg.y, msg.roller_in, msg.roller_out, msg.servo);
                safePrint(buf);
            }
        }

        if (millis() - g_lastRecvMs > 500) {
            if (xSemaphoreTake(xRxMutex, pdMS_TO_TICKS(10)) == pdTRUE) {
                g_rxData = {0.0f, 0.0f, false, false, false};
                xSemaphoreGive(xRxMutex);
            }
        }

        vTaskDelay(pdMS_TO_TICKS(5));
    }
}

void TaskFlywheelMove(void *pvParameters) {
    for (;;) {
        bool runIn  = false;
        bool runOut = false;

        if (xSemaphoreTake(xRxMutex, pdMS_TO_TICKS(10)) == pdTRUE) {
            runIn  = g_rxData.roller_in;
            runOut = g_rxData.roller_out;
            xSemaphoreGive(xRxMutex);
        }

        if (runOut) {
            // Tembak: RPWM aktif, LPWM = 0
            ledcWrite(0, 150); ledcWrite(1, 0);
            ledcWrite(2, 150); ledcWrite(3, 0);
        } else if (runIn) {
            // Tangkap: LPWM aktif, RPWM = 0
            ledcWrite(0, 0); ledcWrite(1, 150);
            ledcWrite(2, 0); ledcWrite(3, 150);
        } else {
            // Stop: semua 0
            ledcWrite(0, 0); ledcWrite(1, 0);
            ledcWrite(2, 0); ledcWrite(3, 0);
        }

        vTaskDelay(pdMS_TO_TICKS(20));
    }
}

void TaskServoMove(void *pvParameters) {
    bool lastButton  = false;
    bool servoToggle = false;

    for (;;) {
        bool currentButton = false;
        if (xSemaphoreTake(xRxMutex, pdMS_TO_TICKS(10)) == pdTRUE) {
            currentButton = g_rxData.servo;
            xSemaphoreGive(xRxMutex);
        }

        // Print terus setiap 500ms supaya kelihatan nilainya
        static uint32_t lastPrint = 0;
        if (millis() - lastPrint > 500) {
            char buf[50];
            snprintf(buf, sizeof(buf), "[SERVO] button=%d toggle=%d", currentButton, servoToggle);
            safePrint(buf);
            lastPrint = millis();
        }

        if (currentButton == true && lastButton == false) {
            servoToggle = !servoToggle;
            PWMServo.write(servoToggle ? 90 : 0);

            char buf[40];
            snprintf(buf, sizeof(buf), "Servo -> %d deg", servoToggle ? 90 : 0);
            safePrint(buf);
        }

        lastButton = currentButton;
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}

// TAMBAHAN
// void TaskHTTPServer(void *pvParameters) {
//     for (;;) {
//         server.handleClient();
//         vTaskDelay(pdMS_TO_TICKS(10));
//     }
// }