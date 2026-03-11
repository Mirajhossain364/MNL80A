#include <MNL80A.h>

// Initialize the PLC module on Hardware Serial 1
MNL80A plc(Serial1);

void setup() {
  // Start PC communication for debugging
  Serial.begin(115200);
  
  // Initialize the PLC module. 
  // Background: This automatically sets up Serial pins, enables 
  // Network Notifications, and disables the Whitelist.
  plc.begin(16, 17);
  
  // Wait briefly for the module to stabilize and join the network
  delay(1000);

  Serial.println("--- MNL80A System Diagnostic ---");
  Serial.println("Module Role:  " + plc.getRole());    // STA (Node) or CCO (Master)
  Serial.println("Local MAC:    " + plc.getMac());     // This device's address
  Serial.println("Network CCO:  " + plc.getCcoMac());  // The Master's address
  Serial.println("Nodes Found:  " + String(plc.getTopoNum()));
  Serial.println("Serial Mode:  " + String(plc.getMode()));
  Serial.println("--------------------------------\n");
}

void loop() {
  // Continuously check for incoming data and network status changes
  processIncomingTraffic(); 

  // Handle commands sent from the Serial Monitor (PC)
  if (Serial.available()) {
    String input = Serial.readStringUntil('\n');
    input.trim();

    // Type 'scan' to see the full network map
    if (input == "scan") {
        Serial.println("Current Topology:\n" + plc.getTopoInfo());
    } 
    // Type any text to send a message to a specific node
    else if (input.length() > 0) {
       // Replace this MAC with your target destination
       String targetMAC = "0D0103091676"; 
       if(plc.sendData(targetMAC, input, true)) {
         Serial.println(">> Message Sent: " + input);
       } else {
         Serial.println(">> Send Failed!");
       }
    }
  }
}

/**
 * Monitors the PLC module for incoming packets.
 * Separates standard data messages from system events (Node Join/Leave).
 */
void processIncomingTraffic() {
  PLCMessage msg;
  PLCEvent ev;

  // Check for incoming data messages (+RECV)
  if (plc.available(msg)) {
    Serial.println("\n[DATA RECEIVED]");
    Serial.println("From:    " + msg.mac);
    Serial.println("Message: " + msg.data);
    Serial.println("Format:  " + String(msg.type == 1 ? "String" : "Hex"));
    Serial.println("---------------");
  }

  // Check for network status updates (+ONLINE / +OFFLINE)
  if (plc.getEvent(ev)) {
    if (ev.type == EVENT_ONLINE) {
      Serial.println("\n[NETWORK EVENT] Node joined: " + ev.mac);
    } else if (ev.type == EVENT_OFFLINE) {
      Serial.println("\n[NETWORK EVENT] Node disconnected: " + ev.mac);
    }
  }
}