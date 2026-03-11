#include <MNL80A.h>

// Define the LED pin
#define LED_PIN 13

// Initialize the MNL80A library using Hardware Serial 1
MNL80A plc(Serial1);

void setup() {
  // Start the serial monitor for debugging at high speed
  Serial.begin(115200);
  
  // Set LED pin as output
  pinMode(LED_PIN, OUTPUT);

  // Initialize PLC communication on Pins 16 (RX) and 17 (TX)
  // Background: Library automatically handles 8E1 parity, 
  // enables notifications, and disables whitelist.
  plc.begin(16, 17);
  
  // Wait for module to stabilize
  delay(2000);
  
  // Print local info to confirm the module is responding
  Serial.println("My MAC Address: " + plc.getMac());
  Serial.println("Network Role:   " + plc.getRole());
  Serial.println("\nListening for PLC packets...\n");
}

void loop() {
  // --- UNIFIED PACKET HANDLING ---
  // Using readPacket() ensures we never miss an event while waiting for data
  PLCResponse res = plc.readPacket();

  // Handle incoming DATA packets
  if (res.type == PACKET_DATA) {
    Serial.println("--- New Message Received ---");
    Serial.print("Transmitter MAC : "); Serial.println(res.message.mac);
    Serial.print("Payload         : "); Serial.println(res.message.data);
    
    // Command Logic
    if (res.message.data == "STA1 LOAD OFF") {
      digitalWrite(LED_PIN, LOW);
      Serial.println("ACTION: LED turned OFF.");
    } 
    else if (res.message.data == "STA1 LOAD ON") {
      digitalWrite(LED_PIN, HIGH);
      Serial.println("ACTION: LED turned ON.");
    }
    
    Serial.println("----------------------------\n");
  }

  // Handle incoming SYSTEM events
  else if (res.type == PACKET_EVENT) {
    switch (res.event.type) {
      case EVENT_ONLINE:
        Serial.println("[SYSTEM] Node joined network: " + res.event.mac);
        break;
        
      case EVENT_OFFLINE:
        Serial.println("[SYSTEM] Node went offline: " + res.event.mac);
        break;
        
      case EVENT_READY:
        Serial.println("[SYSTEM] Module rebooted / Ready.");
        break;

      default:
        break;
    }
  }
}