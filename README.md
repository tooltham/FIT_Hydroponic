# FIT_Hydroponic System V.1.6
## 🥬 Smart Hydroponic Control System by IoTES LAB

![IoT LAB](https://i0.wp.com/fit.rmuti.ac.th/wp-content/uploads/2019/06/FIT2-1.png?fit=992%2C1162&ssl=1)

---

### 📝 Overview
**FIT_Hydroponic** เป็นระบบควบคุมและมอนิเตอร์การปลูกพืชไฮโดรโปนิกส์อัจฉริยะ ออกแบบมาเพื่อควบคุมการหมุนเวียนของน้ำผ่านปั๊ม 2 ตัวสลับกัน และแจ้งเตือนอาการผิดปกติ เช่น น้ำร้อนเกินไป หรือตรวจไม่พบการไหลของน้ำ (Dry Run Protection) ผ่าน Line Notify

### 🚀 Key Features (Audited & Improved)
- **Security:** ระบบแยกส่วน Credential ไปยัง `secrets.h` ป้องกันข้อมูลรั่วไหล
- **Stability:** ระบบเชื่อมต่อ WiFi แบบ Non-blocking พร้อม Auto-reconnect
- **Hardware Safety:** 
    - **Dry Run Protection:** ตรวจสอบการไหลผ่าน Flow Switch ทุกครั้งที่ปั๊มทำงาน
    - **Sensor Health Check:** แจ้งเตือนทันทีหากเซ็นเซอร์วัดอุณหภูมิหลุดหรือชำรุด
- **Performance:** ใช้ `snprintf` และ `millisDelay` เพื่อประสิทธิภาพสูงสุดและลดปัญหา Memory Fragmentation

### 🔌 Hardware Configuration
| Component | Pin (ESP32) | Function |
| :--- | :--- | :--- |
| **Relay 1** | GPIO 26 | Water Pump 1 |
| **Relay 2** | GPIO 25 | Water Pump 2 |
| **DS18B20** | GPIO 4 | Water Temperature |
| **Flow SW 1** | GPIO 18 | Flow Detection 1 |
| **Flow SW 2** | GPIO 19 | Flow Detection 2 |

### 🛠️ Setup Instructions
1. **Libraries Required:**
   - `millisDelay`
   - `OneWire` & `DallasTemperature`
   - `TridentTD_LineNotify`
2. **Configuration:**
   - สร้างไฟล์ `secrets.h` ในโฟลเดอร์โครงการ
   - ระบุ `WIFI_SSID`, `WIFI_PASSWORD` และ `LINE_NOTIFY_TOKEN`
3. **Deployment:**
   - Compile และ Upload ไปยัง ESP32 บอร์ด

---
*Maintained and Audited by Mori Minami (Mina) - Engineering Advisor to IoTES LAB*