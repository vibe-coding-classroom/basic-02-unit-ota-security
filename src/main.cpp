#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <Update.h>
#include <Preferences.h>
#include "version.h"

// --- 配置資訊 ---
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

WebServer server(80);
Preferences preferences;

// OTA 更新頁面 HTML (簡化版)
const char* updateIndex = 
"<form method='POST' action='/update' enctype='multipart/form-data'>"
"  <input type='file' name='update'>"
"  <input type='submit' value='Update'>"
"</form>";

// --- 任務 1: 數位守門員 (HTTP Basic Auth) ---
bool authenticateUser() {
    // TODO: 從 NVS (Preferences) 讀取密碼並進行驗證
    // 提示: 使用 server.authenticate(username, password)
    // 如果是第一次開機，請設定預設密碼或導向設定頁面
    return true; // 暫時回傳 true 供開發測試
}

// --- 任務 2 & 3: 版本檢查與硬體適配 ---
void handleUpdate() {
    HTTPUpload& upload = server.upload();
    
    if (upload.status == UPLOAD_FILE_START) {
        Serial.printf("Update: %s\n", upload.filename.c_str());
        
        // TODO: 任務 2 - 解析標頭前 1K 檢查版本 (Anti-rollback)
        // TODO: 任務 3 - 檢查硬體型號 (Hardware Affinity)
        
        if (!Update.begin(UPDATE_SIZE_UNKNOWN)) {
            Update.printError(Serial);
        }
    } else if (upload.status == UPLOAD_FILE_WRITE) {
        if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
            Update.printError(Serial);
        }
    } else if (upload.status == UPLOAD_FILE_END) {
        if (Update.end(true)) {
            Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
        } else {
            Update.printError(Serial);
        }
    }
}

void setup() {
    Serial.begin(115200);
    
    // 初始化 NVS
    preferences.begin("ota-auth", false);
    
    // 連接 WiFi
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWiFi connected");
    Serial.println(WiFi.localIP());

    // 路由設定
    server.on("/", HTTP_GET, []() {
        server.send(200, "text/html", updateIndex);
    });

    // OTA 處理路由 (需要進行身分驗證)
    server.on("/update", HTTP_POST, []() {
        server.sendHeader("Connection", "close");
        server.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
        ESP.restart();
    }, []() {
        // 在此處實作攔截邏輯
        if (!authenticateUser()) {
            return server.requestAuthentication();
        }
        handleUpdate();
    });

    server.begin();
    Serial.printf("Current Version: %s\n", APP_VERSION);
    Serial.printf("Hardware Target: %s\n", HARDWARE_TARGET);
    Serial.printf("Actual Chip Model: %s\n", ESP.getChipModel());
}

void loop() {
    server.handleClient();
    delay(1);
}
