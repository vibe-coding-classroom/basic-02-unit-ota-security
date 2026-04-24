針對 **`basic-02-unit-ota-security` (OTA 安全性：密碼保護與版本檢查)** 單元，這是將物聯網裝置從「實驗室雛型」提升到「商業級產品」的必經之路。它要求學員具備 **防禦性程式設計 (Defensive Programming)** 的觀念，學習如何透過身份驗證、語義化版本控制與硬體型號鎖定，為裝置建立起牢不可破的安全護城河。

以下是在 **GitHub Classroom** 部署其作業 (Assignments) 的具體建議：

### 1. 範本倉庫 (Template Repo) 配置建議
安全作業的核心在於「攔截非法請求」，範本應包含完整的安全框架測試環境，建議包含：
*   **📂 `src/main.cpp`**：**安全邏輯實作對象**。提供一個具備基礎 OTA 但完全不設防的代碼，學員需在此實作 `server.authenticate()` 攔截器與版本比對邏輯。
*   **📂 `include/version_config.h`**：定義當前韌體的 `APP_VERSION` (例如 1.0.1) 與 `HARDWARE_TARGET` (例如 ESP32_S3)。
*   **📂 `data/access_denied.html`**：設計一個專業的「拒絕訪問」頁面，當學員模擬攻擊失敗時顯示。
*   **📂 `docs/SECURITY_AUDIT.md`**：**安全審計清單**。學員需在此記錄三項測試結果：
    1.  輸入錯誤密碼是否能成功攔截？
    2.  上傳舊版本 (.bin) 是否會自動觸發 `Update.abort()`？
    3.  若偵測到硬體型號不符，系統是否能停止更新？

---

### 2. 作業任務部署細節

#### 任務 1：「數位守門員」實作 OTA 密碼鎖 (Digital Gatekeeper Lab)
*   **目標**：實作 HTTP Basic Authentication，並利用 NVS (Non-Volatile Storage) 讓管理密碼具備重啟後的持久性。
*   **Classroom 部署建議**：
    *   **攔截器實作**：學員需在 `/update` 路由入口處插入驗證 logic，未登入者應收到 401 錯誤並彈出登入視窗。
    *   **NVS 整合**：利用 `Preferences` 函式庫，學員需實作一個功能：第一次開機時設定密碼，之後所有更新請求都必須匹配該密碼。
    *   **驗證要點**：提交一張截圖，展示瀏覽器彈出「需要使用者名稱與密碼」的標準認證對話框。

#### 任務 2：「防呆機制」版本檢查與降級攔截 (Integrity Guard: Version Pre-check Lab)
*   **目標**：解析二進位檔標頭資訊，實作「拒絕降級 (Anti-rollback)」策略，確保系統版本永遠向上提升。
*   **Classroom 部署建議**：
    *   **標頭解析實作**：學員需學會在 `Update.onStart` 時解析上傳流的前 1K Bytes，提取其中的版本字串。
    *   **邏輯裁決**：若 `New_Version < Current_Version`，必須立即調用 `Update.abort()` 並回傳錯誤訊息。
    *   **技術說明**：在 README 中解釋，為什麼在生產環境中，「防止版本退回」比「新增功能」更重要。

#### 任務 3：「硬體適配」晶片型號鎖定 (Hardware Affinity: Architecture Lock Lab)
*   **目標**：利用 SoC 內部指紋，防止「誤刷」導致的硬體毀損（如將 S3 韌體刷入 C3）。
*   **Classroom 部署建議**：
    *   **指紋提取**：調用 `ESP.getChipModel()` 讀取當前硬體架構。
    *   **嚴格綁定**：在更新標頭中加入型號檢查位元。若不符，立即強制關閉連線。
    *   **穩定性測試**：學員需證明：即便持有密碼且版本號正確，但若編譯目標架構不符，系統仍能守住最後一線防波堤。

---

### 3. 物聯網安全官點評標準 (IoT Security & Audit Standards)
此單元的價值在於 **「對異常路徑 (Negative Paths) 的處理能力」**：
*   [ ] **認證嚴謹度**：帳號密碼是否硬編碼 (Hardcoded) 在代碼中？（若是，則應扣分，引導使用 NVS）。
*   [ ] **版本管理邏輯**：是否能正確解讀語義化版本 (Semantic Versioning) 並執行正確的攔截判斷？
*   [ ] **安全訊息提示**：當更新被攔截時，前端介面是否能提供清晰的錯誤原因（如：Error 101 - Version Too Old）？

### 📁 推薦範本結構 (GitHub Classroom Template)：
```text
.
├── src/main.cpp         # 韌體：實作 authenticate() 與 Update.abort() 的安全核心
├── include/version.h    # 配置：定義當前設備的數位 ID 與版本號
├── docs/SecurityTest.md # 報告：詳列「密碼測試」、「降級測試」與「型號誤刷測試」日誌
├── README.md            # 總結：我如何將開放式 OTA 升級為工業級安全防禦系統
└── assets/auth_box.png  # 證據：瀏覽器顯示 Http Basic Auth 登入視窗的畫面
```

透過這種部署方式，學生能體驗到 **「好的安全工程師，是把便利性建立在絕對防火牆之後的人」**。掌握 OTA 安全性後，學員將擁有了管理「具備資安風險、大規模部署、長週期運行」物聯網專案最核心的資產保護能力！_
|
|
太精采了！我們已經將 **基礎 02**（安全性）單元的標準化 Classroom 定義完成了。這是一門真正教導學生「如何保護自己的作品不被駭客接管」的進階基礎課。恭喜！課程架構已經非常完整！
