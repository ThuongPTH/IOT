#include "WiFi.h"
#include "HTTPClient.h"
#include "DHT.h"

#define DHTPIN 4        // DATA của DHT11 nối vào GPIO4
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

const char* ssid = "HuyenThuong";      
const char* password = "1111111";  

String writeApiKey = "7ZTD0OI0GLXL6CQP";   // Key ghi dữ liệu
String readApiKey  = "47K7FFC2I3O2TS66";    // Key đọc dữ liệu
String channelID   = "3071948";      // ID channel

const char* server = "http://api.thingspeak.com/update";

void setup() {
  Serial.begin(115200);
  dht.begin();

  Serial.println("Ket noi WiFi...");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nDa ket noi WiFi!");
}

void loop() {
  // Đọc dữ liệu từ DHT11
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t)) {
    Serial.println("Khong doc duoc du lieu tu DHT11!");
    delay(2000);
    return;
  }

  Serial.print("Nhiet do: "); Serial.print(t);
  Serial.print(" *C, Do am: "); Serial.print(h); Serial.println(" %");

  // ====== Gửi dữ liệu lên ThingSpeak ======
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    String url = String(server) + "?api_key=" + writeApiKey +
                 "&field1=" + String(t) +
                 "&field2=" + String(h);

    http.begin(url);
    int httpCode = http.GET();

    if (httpCode > 0) {
      Serial.println("Gui du lieu len ThingSpeak thanh cong");
    } else {
      Serial.print("Loi khi gui du lieu: ");
      Serial.println(http.errorToString(httpCode));
    }
    http.end();
  }

  delay(20000); // chờ 20s (giới hạn ThingSpeak ≥ 15s)

  // ====== Đọc lại dữ liệu từ ThingSpeak ======
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String url = "http://api.thingspeak.com/channels/" + channelID +
                 "/fields/1/last.json?api_key=" + readApiKey;

    http.begin(url);
    int httpCode = http.GET();

    if (httpCode > 0) {
      String payload = http.getString();
      Serial.println("Du lieu lay ve tu ThingSpeak:");
      Serial.println(payload);
    } else {
      Serial.print("Loi khi doc du lieu: ");
      Serial.println(http.errorToString(httpCode));
    }
    http.end();
  }

  delay(20000); // lần tiếp theo
}
