#include <WiFi.h>
#include <PubSubClient.h>
#include <WiFiClientSecure.h>
#include <MNL80A.h>  // Your updated library

// WiFi settings
const char* ssid = "CHSL_2.4G";
const char* password = "CHSL@2025";

// MQTT Broker settings
const char* mqtt_server = "53fba7-------------.s1.eu.hivemq.cloud"; // user 
const char* mqtt_username = "-----"; // user
const char* mqtt_password = "-----"; // user
const int mqtt_port = 8883;

// Pin and PLC Constants
const int sta1load = 13;
#define RX_PIN 16
#define TX_PIN 17
#define TARGET_MAC "0D0103078794" 

// Initialize Library
MNL80A plc(Serial1); 

WiFiClientSecure espClient;
PubSubClient client(espClient);

const char* CONTROL_LOAD_TOPIC = "control-load";

// ISRG Root X1 Certificate (HiveMQ Cloud)
static const char *root_ca PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIFazCCA1OgAwIBAgIRAIIQz7DSQONZRGPgu2OCiwAwDQYJKoZIhvcNAQELBQAw
TzELMAkGA1UEBhMCVVMxKTAnBgNVBAoTIEludGVybmV0IFNlY3VyaXR5IFJlc2Vh
cmNoIEdyb3VwMRUwEwYDVQQDEwxJU1JHIFJvb3QgWDEwHhcNMTUwNjA0MTEwNDM4
WhcNMzUwNjA0MTEwNDM4WjBPMQswCQYDVQQGEwJVUzEpMCcGA1UEChMgSW50ZXJu
ZXQgU2VjdXJpdHkgUmVzZWFyY2ggR3JvdXAxFTATBgNVBAMTDElTUkcgUm9vdCBY
MTCCAiIwDQYJKoZIhvcNAQEBBQADggIPADCCAgoCggIBAK3oJHP0FDfzm54rVygc
h77ct984kIxuPOZXoHj3dcKi/vVqbvYATyjb3miGbESTtrFj/RQSa78f0uoxmyF+
0TM8ukj13Xnfs7j/EvEhmkvBioZxaUpmZmyPfjxwv60pIgbz5MDmgK7iS4+3mX6U
A5/TR5d8mUgjU+g4rk8Kb4Mu0UlXjIB0ttov0DiNewNwIRt18jA8+o+u3dpjq+sW
T8KOEUt+zwvo/7V3LvSye0rgTBIlDHCNAymg4VMk7BPZ7hm/ELNKjD+Jo2FR3qyH
B5T0Y3HsLuJvW5iB4YlcNHlsdu87kGJ55tukmi8mxdAQ4Q7e2RCOFvu396j3x+UC
B5iPNgiV5+I3lg02dZ77DnKxHZu8A/lJBdiB3QW0KtZB6awBdpUKD9jf1b0SHzUv
KBds0pjBqAlkd25HN7rOrFleaJ1/ctaJxQZBKT5ZPt0m9STJEadao0xAH0ahmbWn
OlFuhjuefXKnEgV4We0+UXgVCwOPjdAvBbI+e0ocS3MFEvzG6uBQE3xDk3SzynTn
jh8BCNAw1FtxNrQHusEwMFxIt4I7mKZ9YIqioymCzLq9gwQbooMDQaHWBfEbwrbw
qHyGO0aoSCqI3Haadr8faqU9GY/rOPNk3sgrDQoo//fb4hVC1CLQJ13hef4Y53CI
rU7m2Ys6xt0nUW7/vGT1M0NPAgMBAAGjQjBAMA4GA1UdDwEB/wQEAwIBBjAPBgNV
HRMBAf8EBTADAQH/MB0GA1UdDgQWBBR5tFnme7bl5AFzgAiIyBpY9umbbjANBgkq
hkiG9w0BAQsFAAOCAgEAVR9YqbyyqFDQDLHYGmkgJykIrGF1XIpu+ILlaS/V9lZL
ubhzEFnTIZd+50xx+7LSYK05qAvqFyFWhfFQDlnrzuBZ6brJFe+GnY+EgPbk6ZGQ
3BebYhtF8GaV0nxvwuo77x/Py9auJ/GpsMiu/X1+mvoiBOv/2X/qkSsisRcOj/KK
NFtY2PwByVS5uCbMiogziUwthDyC3+6WVwW6LLv3xLfHTjuCvjHIInNzktHCgKQ5
ORAzI4JMPJ+GslWYHb4phowim57iaztXOoJwTdwJx4nLCgdNbOhdjsnvzqvHu7Ur
TkXWStAmzOVyyghqpZXjFaH3pO3JLF+l+/+sKAIuvtd7u+Nxe5AW0wdeRlN8NwdC
jNPElpzVmbUq4JUagEiuTDkHzsxHpFKVK7q4+63SM1N95R1NbdWhscdCb+ZAJzVc
oyi3B43njTOQ5yOf+1CceWxG1bQVs5ZufpsMljq4Ui0/1lvh+wjChP4kqKOJ2qxq
4RgqsahDYVvTH9w7jXbyLeiNdd8XM2w9U/t7y0Ff/9yi0GE44Za4rF2LN9d11TPA
mRGunUHBcnWEvgJBQl9nJEiU0Zsnvgc/ubhPgXRR4Xq37Z0j4r7g1SgEEzwxA57d
emyPxgcYxn/eR44/KJ4EBs+lVDR3veyJm+kXQ99b21/+jh5Xos1AnX5iItreGCc=
-----END CERTIFICATE-----
)EOF";

// --- MQTT CALLBACK ---
void callback(char* topic, byte* payload, unsigned int length) {
  String message = "";
  for (unsigned int i = 0; i < length; i++) message += (char)payload[i];

  Serial.printf("MQTT [%s]: %s\n", topic, message.c_str());

  if (strcmp(topic, CONTROL_LOAD_TOPIC) == 0) {
    if (message == "sta1load off") {
      digitalWrite(sta1load, LOW);
      plc.sendData(TARGET_MAC, "STA1 LOAD OFF"); 
    } 
    else if (message == "sta1load on") {
      digitalWrite(sta1load, HIGH);
      plc.sendData(TARGET_MAC, "STA1 LOAD ON"); 
    }
  }
}

// --- NETWORK SETUP ---
void setup_wifi() {
  Serial.print("Connecting to WiFi");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected. IP: " + WiFi.localIP().toString());
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP32_PLC_Node", mqtt_username, mqtt_password)) {
      Serial.println("connected");
      client.subscribe(CONTROL_LOAD_TOPIC);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      delay(5000);
    }
  }
}

void setup() {
  pinMode(sta1load, OUTPUT);
  Serial.begin(115200);

  // 1. Initialize PLC Module via Library
  // Your updated begin() now handles Whitelist: OFF and Notify: ON automatically
  plc.begin(RX_PIN, TX_PIN); 
  delay(1000);
  
  Serial.println("--- PLC System Diagnostic ---");
  Serial.println("Role: " + plc.getRole());
  Serial.println("MAC:  " + plc.getMac());
  Serial.println("-----------------------------");

  // 2. Connect to WiFi and MQTT
  setup_wifi();
  espClient.setCACert(root_ca);
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // --- THE UNIFIED PACKET ENGINE ---
  // This replaces available() and getEvent() to prevent data loss
  PLCResponse res = plc.readPacket();

  if (res.type == PACKET_DATA) {
    Serial.printf("\n[PLC DATA] From %s: %s\n", res.message.mac.c_str(), res.message.data.c_str());
    
    // Publish PLC data to MQTT
    String mqttMsg = "From:" + res.message.mac + " Data:" + res.message.data;
    client.publish("plc/received_data", mqttMsg.c_str());
  } 
  
  else if (res.type == PACKET_EVENT) {
    if (res.event.type == EVENT_ONLINE) {
      Serial.println("[PLC EVENT] Node Joined: " + res.event.mac);
      client.publish("plc/events", ("ONLINE: " + res.event.mac).c_str());
    } 
    else if (res.event.type == EVENT_OFFLINE) {
      Serial.println("[PLC EVENT] Node Left: " + res.event.mac);
      client.publish("plc/events", ("OFFLINE: " + res.event.mac).c_str());
    }
  }
}