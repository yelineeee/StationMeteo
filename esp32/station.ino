#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>

//  WIFI
const char* ssid = "exemple_Wifi";
const char* password = "jeSuisUnMotDePasseTresFort123!";

//  MQTT 
const char* mqtt_server = "captain.dev0.pandor.cloud";
const int mqtt_port = 1884;
const char* mqtt_topic = "station/temperature";
const char* mqtt_client_id = "ESP32-Station";

// PINS 
const int BUTTON_PIN = 21;
const int LED_C_PIN  = 2;
const int LED_F_PIN  = 4;

const int DHT_PIN = 33;   
#define DHTTYPE DHT22

//  CAPTEUR 
DHT dht(DHT_PIN, DHTTYPE);

// VARIABLES
float celsius = 0.0;
float fahrenheit = 0.0;
float humidity = 0.0;

bool modeCelsius = true;
bool modeSimulation = false; 

unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 50;
bool lastReading = HIGH;
bool stableState = HIGH;

unsigned long lastPublish = 0;
const unsigned long publishInterval = 1000;

// MQTT objects
WiFiClient espClient;
PubSubClient client(espClient);

// WIFI SETUP
void setup_wifi() {
  Serial.print("Connexion WiFi");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(400);
    Serial.print(".");
  }

  Serial.println("\nWiFi connecté !");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
}

// MQTT RECONNECT
void reconnect_mqtt() {
  while (!client.connected()) {
    Serial.print("Connexion MQTT...");
    if (client.connect(mqtt_client_id)) {
      Serial.println("OK");
    } else {
      Serial.print("ECHEC (state=");
      Serial.print(client.state());
      Serial.println(")");
      delay(2000);
    }
  }
}

// JSON PAYLOAD
String buildPayload() {
  String payload = "{";
  payload += "\"temperature\":";
  payload += (modeCelsius ? String(celsius, 1) : String(fahrenheit, 1));
  payload += ",\"unit\":\"";
  payload += (modeCelsius ? "C" : "F");
  payload += "\",\"humidity\":";
  payload += String(humidity, 1);
  payload += ",\"simulation\":";
  payload += (modeSimulation ? "true" : "false");
  payload += "}";
  return payload;
}

void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(LED_C_PIN, OUTPUT);
  pinMode(LED_F_PIN, OUTPUT);

  Serial.begin(115200);
  delay(200);

  dht.begin();

  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);

  Serial.println("BOOT OK");
}

void loop() {
  if (!client.connected()) reconnect_mqtt();
  client.loop();
  bool reading = digitalRead(BUTTON_PIN);

  if (reading != lastReading) lastDebounceTime = millis();

  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading != stableState) {
      stableState = reading;
      if (stableState == LOW) modeCelsius = !modeCelsius;
    }
  }
  lastReading = reading;


  if (modeSimulation) {
    celsius = 45.0;
    humidity = 55.0;
  } else {
    float t = dht.readTemperature(); // °C
    float h = dht.readHumidity();    // %

    if (!isnan(t)) celsius = t;
    else Serial.println("DHT22: temp invalide");

    if (!isnan(h)) humidity = h;
    else Serial.println("DHT22: humidite invalide");
  }

  fahrenheit = (celsius * 9.0) / 5.0 + 32.0;

  // LEDs
  digitalWrite(LED_C_PIN, modeCelsius ? HIGH : LOW);
  digitalWrite(LED_F_PIN, modeCelsius ? LOW : HIGH);

  if (millis() - lastPublish >= publishInterval) {
    lastPublish = millis();

    String payload = buildPayload();
    Serial.println(payload);

    if (!client.publish(mqtt_topic, payload.c_str())) {
      Serial.println("Publish MQTT: ECHEC");
    }
  }
}
