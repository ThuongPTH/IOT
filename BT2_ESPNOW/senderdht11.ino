#include <WiFi.h>
#include <esp_now.h>
#include "DHT.h"

// Chân DHT11 nối với ESP32
#define DHTPIN 14
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// Địa chỉ MAC của Receiver (thay bằng MAC thật của board nhận)
uint8_t broadcastAddress[] = {0x3c, 0xe9, 0x0E, 0x88, 0xd8, 0x14};

// Cấu trúc dữ liệu gửi đi
typedef struct struct_message {
  char a[32];
  int b;
  float temperature;
  float humidity;
} struct_message;

// Tạo biến dữ liệu
struct_message myData;

// Callback khi gửi xong
void OnDataSent(const wifi_tx_info_t *info, esp_now_send_status_t status) {
  Serial.print("Send Status: ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Success" : "Fail");
}

void setup() {
  Serial.begin(115200);
  dht.begin();

  // Cấu hình Wi-Fi mode
  WiFi.mode(WIFI_STA);

  // Khởi tạo ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Đăng ký callback gửi
  esp_now_register_send_cb(OnDataSent);

  // Thêm peer (receiver)
  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }
}

void loop() {
  // Đọc dữ liệu từ DHT11
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  // Kiểm tra lỗi
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    delay(2000);
    return;
  }

  // Gán dữ liệu vào struct
  strcpy(myData.a, "HELLO ESP-NOW");
  myData.b = random(0, 100);
  myData.temperature = t;
  myData.humidity = h;

  // Gửi dữ liệu
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));

  if (result == ESP_OK) {
    Serial.println("Sending data...");
  } else {
    Serial.println("Error sending the data");
  }

  delay(5000); // Gửi mỗi 5 giây
}
