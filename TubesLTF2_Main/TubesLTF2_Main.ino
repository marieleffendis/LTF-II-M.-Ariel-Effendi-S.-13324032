#include <Bluepad32.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include <esp_now.h>
#include <WiFi.h>

#define ENA  33
#define IN1  25
#define IN2  26
#define IN3  27
#define IN4  14
#define ENB  13

uint8_t receiverMAC[] = {0x3C, 0x8A, 0x1F, 0xD4, 0xB0, 0xB4}; 

typedef struct {
    float x;
    float y;
    bool flywheel;
    bool servo;
} JoyMessage;

volatile int  g_vkiri  = 0;
volatile int  g_vkanan = 0;
volatile bool g_maju   = false;
volatile bool g_mundur = false;

JoyMessage g_robotData = {0.0f, 0.0f, false, false};

SemaphoreHandle_t xSerialSemaphore;  
SemaphoreHandle_t xMotorSemaphore;   

ControllerPtr myControllers[BP32_MAX_GAMEPADS];

void TaskConnectionPS4(void *pvParameters);
void TaskMotorMove(void *pvParameters);
void TaskConnectionESP32Transmitter(void *pvParameters);

void safePrint(const char* msg) {
    if (xSemaphoreTake(xSerialSemaphore, pdMS_TO_TICKS(10)) == pdTRUE) {
        Serial.println(msg);
        xSemaphoreGive(xSerialSemaphore);
    }
}

void onConnectedController(ControllerPtr ctl) {
    for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
        if (myControllers[i] == nullptr) {
            myControllers[i] = ctl;
            safePrint("[BP32] Controller Connected!");
            break;
        }
    }
}

void onDisconnectedController(ControllerPtr ctl) {
    for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
        if (myControllers[i] == ctl) {
            myControllers[i] = nullptr;
            safePrint("[BP32] Controller Disconnected");
            break;
        }
    }
}

void processGamepad(ControllerPtr ctl) {
    int axisX    = ctl->axisX();     
    int throttle = ctl->throttle();  
    int brake    = ctl->brake();     

    // Mixing untuk Drive Motor Lokal
    int vk = map(axisX + (throttle - brake), -512, 511, 0, 255);
    int vn = map(-axisX + (throttle - brake), -512, 511, 0, 255);
    
    vk = constrain(vk, 0, 255);
    vn = constrain(vn, 0, 255);

    if (xSemaphoreTake(xMotorSemaphore, pdMS_TO_TICKS(5)) == pdTRUE) {
        g_vkiri   = vk;
        g_vkanan  = vn;
        g_maju    = (throttle > brake);
        g_mundur  = (brake > throttle);
        
        // Data untuk dikirim ke ESP32 Kedua
        g_robotData.x = (float)axisX / 512.0f;
        g_robotData.y = (float)(throttle - brake) / 1023.0f;
        g_robotData.flywheel = (ctl->buttons() & 0x0001); // Tombol Cross/A
        g_robotData.servo    = (ctl->buttons() & 0x0002); // Tombol Circle/B

        xSemaphoreGive(xMotorSemaphore);
    }
}

// --- Callback ESP-NOW (Perbaikan Kurung Kurawal) ---
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
    // Kosongkan saja untuk efisiensi, atau gunakan Serial untuk debug
}

void setup() {
    Serial.begin(115200);

    // Pin Motor (Hanya pin drive, pin shooter/servo dihilangkan dari sini)
    int motorPins[] = {ENA, IN1, IN2, IN3, IN4, ENB};
    for (int pin : motorPins) {
        pinMode(pin, OUTPUT);
        digitalWrite(pin, LOW);
    }

    xSerialSemaphore = xSemaphoreCreateMutex();
    xMotorSemaphore  = xSemaphoreCreateMutex();

    BP32.setup(&onConnectedController, &onDisconnectedController);
    BP32.forgetBluetoothKeys();

    WiFi.mode(WIFI_STA);
    WiFi.disconnect();

    if (esp_now_init() != ESP_OK) {
        safePrint("ESP-NOW Init Failed");
        return;
    }
    esp_now_register_send_cb(OnDataSent);

    esp_now_peer_info_t peerInfo = {};
    memcpy(peerInfo.peer_addr, receiverMAC, 6);
    peerInfo.channel = 0;
    peerInfo.encrypt = false;
    esp_now_add_peer(&peerInfo);

    xTaskCreatePinnedToCore(TaskConnectionPS4, "PS4_Task", 4096, NULL, 7, NULL, 1);
    xTaskCreatePinnedToCore(TaskMotorMove, "Motor_Task", 4096, NULL, 6, NULL, 1);
    xTaskCreatePinnedToCore(TaskConnectionESP32Transmitter, "ESPNOW_Task", 4096, NULL, 5, NULL, 0);

    safePrint("Transmitter Ready!");
}

void loop() {
    vTaskDelay(pdMS_TO_TICKS(1000));
}

void TaskConnectionPS4(void *pvParameters) {
    for (;;) {
        BP32.update();
        for (auto ctl : myControllers) {
            if (ctl && ctl->isConnected() && ctl->hasData()) {
                processGamepad(ctl);
            }
        }
        vTaskDelay(pdMS_TO_TICKS(15));
    }
}

void TaskMotorMove(void *pvParameters) {
    int vk, vn;
    bool maju, mundur;

    for (;;) {
        if (xSemaphoreTake(xMotorSemaphore, pdMS_TO_TICKS(10)) == pdTRUE) {
            vk = g_vkiri;
            vn = g_vkanan;
            maju = g_maju;
            mundur = g_mundur;
            xSemaphoreGive(xMotorSemaphore);
        }

        if (maju) {
            digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);
            digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);
            analogWrite(ENA, vk);    analogWrite(ENB, vn);
        } else if (mundur) {
            digitalWrite(IN1, LOW);  digitalWrite(IN2, HIGH);
            digitalWrite(IN3, LOW);  digitalWrite(IN4, HIGH);
            analogWrite(ENA, vk);    analogWrite(ENB, vn);
        } else {
            digitalWrite(IN1, LOW);  digitalWrite(IN2, LOW);
            digitalWrite(IN3, LOW);  digitalWrite(IN4, LOW);
            analogWrite(ENA, 0);     analogWrite(ENB, 0);
        }
        vTaskDelay(pdMS_TO_TICKS(20));
    }
}

void TaskConnectionESP32Transmitter(void *pvParameters) {
    JoyMessage dataToSend;
    for (;;) {
        if (xSemaphoreTake(xMotorSemaphore, pdMS_TO_TICKS(10)) == pdTRUE) {
            dataToSend = g_robotData;
            xSemaphoreGive(xMotorSemaphore);
        }
        esp_now_send(receiverMAC, (uint8_t *)&dataToSend, sizeof(JoyMessage));
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}