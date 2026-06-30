#include <WiFi.h>          // Gunakan <ESP8266WiFi.h> jika kau pakai ESP8266
#include <PubSubClient.h>

// ==========================================
// 1. TETAPAN RANGKAIAN & BROKER
// ==========================================
const char* ssid = "Vikyolin_2.4Ghz";      // Masukkan nama WiFi/Hotspot kau
const char* password = "OsAke19_";   // Masukkan password WiFi
const char* mqtt_server = "192.168.0.143"; // IP laptop kau (EMQX Broker)
const int mqtt_port = 1883;


const char* mqtt_user = "soil_temp";          // Username yang kau buat kat EMQX tadi
const char* mqtt_password = "UTeM2026";
// ==========================================
// 2. TETAPAN PIN PERKAKASAN
// ==========================================
const int LED_PIN = 2;       // Pin LED atau Relay untuk pam air
const int SOIL_PIN = 34;     // Pin Analog untuk Soil Moisture Sensor

WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;

// ==========================================
// 3. FUNGSI: PENAPIS ARAHAN (ON/OFF)
// ==========================================
void callback(char* topic, byte* payload, unsigned int length) {
  String message = "";
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  
  // ESP32 hanya akan bertindak balas jika mesej tepat "ON" atau "OFF"
  if (message == "ON") {
    Serial.println("Instructions Arrived: LED ON ");
    digitalWrite(LED_PIN, HIGH);
  } else if (message == "OFF") {
    Serial.println("Instructions Arrived: LED OFF");
    digitalWrite(LED_PIN, LOW);
  }
}

// ==========================================
// 4. FUNGSI: SAMBUNGAN WIFI
// ==========================================
void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("CONNECTING TO Wi-Fi: ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected successfully!");
  Serial.print("IP ESP32: ");
  Serial.println(WiFi.localIP());
}

// ==========================================
// 5. FUNGSI: SAMBUNGAN MQTT EMQX
// ==========================================
void reconnect() {
  while (!client.connected()) {
    Serial.print("Connecting to EMQX Broker (192.168.0.143)...");
    
    // Guna nama peranti ESP32_Soil_1 seperti dalam Node-RED
    if (client.connect("ESP32_Soil_1")) { 
      Serial.println("successfully!");
      client.subscribe("soil_health/command"); // Langgan topik butang ON/OFF
    } else {
      Serial.print("Failed, error=");
      Serial.print(client.state());
      Serial.println(" Try again in 5 seconds....");
      delay(5000);
    }
  }
}

// ==========================================
// 6. SETUP UTAMA
// ==========================================
void setup() {
  Serial.begin(115200);
  
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW); // Pastikan pam/LED mati pada permulaan
  
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

// ==========================================
// 7. LOOP UTAMA (PENGHANTARAN DATA)
// ==========================================
void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  unsigned long now = millis();
  // Hantar data setiap 5 saat (5000 milisaat)
  if (now - lastMsg > 5000) {
    lastMsg = now;

    // Baca nilai sebenar dari Soil Moisture Sensor
    int soilValue = analogRead(SOIL_PIN);
    
    // Contoh bacaan Suhu & Kelembapan (Boleh ganti dengan fungsi DHT sebenar)
    float temp = 30.00; 
    float hum = 76.00;

    // Bina struktur JSON sama macam yang kau buat dalam Node-RED
    String payload = "{";
    payload += "\"sensor_id\":\"ESP32_Soil_1\",";
    payload += "\"temperature\":" + String(temp) + ",";
    payload += "\"humidity\":" + String(hum) + ",";
    payload += "\"soil_moisture\":" + String(soilValue);
    payload += "}";

    Serial.print("Sending Telemetry: ");
    Serial.println(payload);
    
    // Publish data ke topik Node-RED
    client.publish("soil_health/telemetry", payload.c_str());
  }
}