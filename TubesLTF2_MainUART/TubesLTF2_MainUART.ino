#include <Bluepad32.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include <WiFi.h>

#define ENA  13
#define IN1  26
#define IN2  25
#define IN3  33
#define IN4  32
#define ENB  27

#define UART_RX   17
#define UART_TX   16
#define UART_BAUD 115200
#define PACKET_HEADER 0xAB

typedef struct {
    float x;
    float y;
    bool  roller_in;   // X (silang)  = tangkap = mundur
    bool  roller_out;  // kotak       = tembak  = maju
    bool  servo;       // lingkaran   = servo
} JoyMessage;

volatile int  g_vkiri  = 0;
volatile int  g_vkanan = 0;
volatile bool g_maju   = false;
volatile bool g_mundur = false;

JoyMessage g_robotData = {0.0f, 0.0f, false, false, false};

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

    int vk = map(axisX + (throttle - brake), -512, 511, 0, 255);
    int vn = map(-axisX + (throttle - brake), -512, 511, 0, 255);

    vk = constrain(vk, 0, 255);
    vn = constrain(vn, 0, 255);

    if (xSemaphoreTake(xMotorSemaphore, pdMS_TO_TICKS(5)) == pdTRUE) {
        g_vkiri   = vk;
        g_vkanan  = vn;
        g_maju    = (throttle > brake);
        g_mundur  = (brake > throttle);

        g_robotData.x          = (float)axisX / 512.0f;
        g_robotData.y          = (float)(throttle - brake) / 1023.0f;
        g_robotData.roller_in  = (ctl->buttons() & 0x0001) ? true : false;
        g_robotData.roller_out = (ctl->buttons() & 0x0004) ? true : false;
        g_robotData.servo      = (ctl->buttons() & 0x0002) ? true : false;

        xSemaphoreGive(xMotorSemaphore);
    }
}

void setup() {
    Serial.begin(115200);
    Serial1.begin(UART_BAUD, SERIAL_8N1, UART_RX, UART_TX);

    int motorPins[] = {ENA, IN1, IN2, IN3, IN4, ENB};
    for (int pin : motorPins) {
        pinMode(pin, OUTPUT);
        digitalWrite(pin, LOW);
    }

    xSerialSemaphore = xSemaphoreCreateMutex();
    xMotorSemaphore  = xSemaphoreCreateMutex();

    BP32.setup(&onConnectedController, &onDisconnectedController);
    BP32.forgetBluetoothKeys();

    WiFi.mode(WIFI_OFF);

    xTaskCreatePinnedToCore(TaskConnectionPS4,              "PS4_Task",   4096, NULL, 7, NULL, 1);
    xTaskCreatePinnedToCore(TaskMotorMove,                  "Motor_Task", 4096, NULL, 6, NULL, 1);
    xTaskCreatePinnedToCore(TaskConnectionESP32Transmitter, "UART_Task",  4096, NULL, 5, NULL, 0);

    safePrint("Transmitter Ready! (UART Mode)");
}

void loop() { vTaskDelay(pdMS_TO_TICKS(1000)); }

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
            vk     = g_vkiri;
            vn     = g_vkanan;
            maju   = g_maju;
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
    uint8_t header = PACKET_HEADER;

    for (;;) {
        if (xSemaphoreTake(xMotorSemaphore, pdMS_TO_TICKS(10)) == pdTRUE) {
            dataToSend = g_robotData;
            xSemaphoreGive(xMotorSemaphore);
        }

        Serial1.write(&header, 1);
        Serial1.write((uint8_t *)&dataToSend, sizeof(JoyMessage));
        Serial1.flush();

        vTaskDelay(pdMS_TO_TICKS(50));
    }
}