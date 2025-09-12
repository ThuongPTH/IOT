#include <WiFi.h>
#include <HTTPClient.h>
#include "DHT.h"

// Chân DATA của DHT11 nối vào GPIO4 (bạn có thể đổi nếu muốn)
#define DHTPIN 4     
#define DHTTYPE DHT11   

const char* ssid = "HuyenThuong";          // WiFi của bạn
const char* password = "1111111";      // Mật khẩu WiFi
const char* apiKey = "7ZTD0OI0GLXL6CQP"; // API Key ThingSpeak

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  dht.begin();

  WiFi.begin(ssid, password);
  Serial.print("Đang kết nối WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nĐã kết nối WiFi");
}

void loop() {
  float t = dht.readTemperature();  // Nhiệt độ (°C)
  float h = dht.readHumidity();     // Độ ẩm (%)

  if (isnan(t) || isnan(h)) {
    Serial.println("Lỗi đọc cảm biến DHT11!");
  } else {
    Serial.printf("Nhiệt độ: %.2f °C, Độ ẩm: %.2f %%\n", t, h);

    if (WiFi.status() == WL_CONNECTED) {
      HTTPClient http;

      String url = "http://api.thingspeak.com/update?api_key=" + String(apiKey) +
                   "&field1=" + String(t, 2) + "&field2=" + String(h, 2);

      http.begin(url);
      int httpCode = http.GET();

      if (httpCode > 0) {
        Serial.printf("Gửi thành công! HTTP code: %d\n", httpCode);
      } else {
        Serial.printf("Lỗi HTTP: %s\n", http.errorToString(httpCode).c_str());
      }

      http.end();
    }
  }

  delay(20000); // Đợi 20 giây (ThingSpeak quy định >= 15s/lần gửi)
}
