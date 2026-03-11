#include "MNL80A.h"

MNL80A::MNL80A(HardwareSerial& serial) : _serial(serial) {
    _buffer = "";
}

void MNL80A::begin(int8_t rxPin, int8_t txPin, unsigned long baud) {
    // 1. Initialize the hardware serial
    _serial.begin(baud, SERIAL_8E1, rxPin, txPin);

    // Give the module a tiny bit of time to wake up
    delay(100); 

    // 2. Set background defaults
    // Enable notifications (+ONLINE, +OFFLINE)
    setNotifyEn(true);

    // Disable whitelist (Accept data from any node by default)
    setWhitelistEn(false);
}

// Internal helper to extract values from module responses
String MNL80A::parseValue(String res, String key) {
    int idx = res.indexOf(key);
    if (idx != -1) {
        int start = idx + key.length();
        int end = res.indexOf("\r", start);
        String val = res.substring(start, end);
        val.trim();
        return val;
    }
    return "ERROR";
}

String MNL80A::getMac() { return parseValue(sendRaw("AT+MAC?"), "+MAC:"); }
String MNL80A::getCcoMac() { return parseValue(sendRaw("AT+CCOMAC?"), "+CCOMAC:"); }
String MNL80A::getRole() { return parseValue(sendRaw("AT+ROLE?"), "+ROLE:"); }

String MNL80A::getMode() { 
    String val = parseValue(sendRaw("AT+MODE?"), "+MODE:");
    if(val == "0") return "TRANSPARENT MODE";
    if(val == "2") return "AT MODE";
    return "UNKNOWN (" + val + ")";
}

String MNL80A::getUartConfig() { return parseValue(sendRaw("AT+UART?"), "+UART:"); }

String MNL80A::getTopoNum() {
    String res = parseValue(sendRaw("AT+TOPONUM?"), "+TOPONUM:");
    return (res == "ERROR") ? "It's STA..CCO can count node." : res;
}

String MNL80A::getTopoInfo() {
    // 1. Get the current number of nodes in the network
    String numStr = getTopoNum();
    int count = numStr.toInt();

    // 2. If there are no nodes or an error occurred, return a message
    if (count <= 0) {
        return "No nodes found in topology.";
    }

    // 3. Automatically call AT+TOPOINFO=1,<count>
    // Example: if count is 5, it sends "AT+TOPOINFO=1,5"
    return sendRaw("AT+TOPOINFO=1," + String(count));
}

// Whitelist & Notifications
bool MNL80A::setNotifyEn(bool enable) { return sendRaw("AT+NOTIFY=" + String(enable ? 1 : 0)).indexOf("OK") != -1; }
bool MNL80A::setWhitelistEn(bool enable) { return sendRaw("AT+WHSTATUS=" + String(enable ? 1 : 0)).indexOf("OK") != -1; }
//bool MNL80A::addWhitelist(String mac) { return sendRaw("AT+WHITELISTADD=" + mac).indexOf("OK") != -1; }
//bool MNL80A::removeWhitelist(String mac) { return sendRaw("AT+WHITELISTDEL=" + mac).indexOf("OK") != -1; }
//bool MNL80A::clearWhitelist() { return sendRaw("AT+WHITELISTCLR").indexOf("OK") != -1; }
//String MNL80A::getWhitelist() { return sendRaw("AT+WHITELIST?"); }

bool MNL80A::sendData(String mac, String data, bool isString) {
    String cmd = "AT+SEND=" + mac + "," + String(data.length()) + ",";
    cmd += isString ? "\"" + data + "\",1" : data + ",0";
    return sendRaw(cmd).indexOf("OK") != -1;
}

// Add a small delay or use readStringUntil to be safer
String MNL80A::sendRaw(String cmd, uint32_t timeout) {
    while(_serial.available()) _serial.read(); // Clear junk before sending
    _serial.print(cmd + "\r\n");
    
    String response = "";
    uint32_t start = millis();
    while (millis() - start < timeout) {
        if (_serial.available()) {
            response += (char)_serial.read();
            if (response.indexOf("OK\r\n") != -1 || response.indexOf("ERROR\r\n") != -1) break;
        }
    }
    return response;
}

bool MNL80A::available(PLCMessage &msg) {
    while (_serial.available()) {
        char c = _serial.read();
        if (c == '\n') {
            _buffer.trim();
            if (_buffer.length() == 0) continue;

            // Check if it's Data
            if (_buffer.startsWith("+RECV:")) {
                bool ok = parseIncoming(_buffer, msg);
                _buffer = "";
                return ok; 
            } 
            
            // If it's not data, it might be an event. 
            // We store it back into the buffer or handle it via a flag.
            // For simplicity, let's process events in a unified loop.
            _buffer = ""; 
        } else {
            if (c != '\r') _buffer += c;
        }
    }
    return false;
}

bool MNL80A::getEvent(PLCEvent &ev) {
    while (_serial.available()) {
        char c = _serial.read();
        if (c == '\n') {
            _buffer.trim();
            bool found = false;
            if (_buffer.startsWith("+ONLINE:")) {
                ev.type = EVENT_ONLINE;
                ev.mac = _buffer.substring(_buffer.indexOf(':') + 1);
                ev.mac.trim();
                found = true;
            } 
            else if (_buffer.startsWith("+OFFLINE:")) {
                ev.type = EVENT_OFFLINE;
                ev.mac = _buffer.substring(_buffer.indexOf(':') + 1);
                ev.mac.trim();
                found = true;
            }
            else if (_buffer.startsWith("+READY")) {
                ev.type = EVENT_READY;
                ev.mac = "";
                found = true;
            }
            _buffer = ""; 
            if (found) return true;
        } else {
            _buffer += c;
        }
    }
    return false;
}

bool MNL80A::parseIncoming(String line, PLCMessage &msg) {
    int c1 = line.indexOf(':') + 1;
    int c2 = line.indexOf(',', c1 + 1);
    int c3 = line.indexOf(',', c2 + 1);
    int c4 = line.lastIndexOf(',');
    if (c1 == 0 || c2 == -1 || c4 == -1) return false;

    msg.mac = line.substring(c1, c2);
    msg.mac.trim();
    msg.len = line.substring(c2 + 1, c3).toInt();
    
    String rawData = line.substring(c3 + 1, c4);
    if (rawData.startsWith("\"") && rawData.endsWith("\"")) {
        msg.data = rawData.substring(1, rawData.length() - 1);
    } else {
        msg.data = rawData;
    }
    msg.type = line.substring(c4 + 1).toInt();
    return true;
}

PLCResponse MNL80A::readPacket() {
    PLCResponse res;
    res.type = PACKET_NONE;

    if (_serial.available()) {
        String line = _serial.readStringUntil('\n');
        line.trim();
        if (line.length() == 0) return res;

        if (line.startsWith("+RECV:")) {
            res.type = PACKET_DATA;
            parseIncoming(line, res.message);
        } 
        else if (line.startsWith("+ONLINE:")) {
            res.type = PACKET_EVENT;
            res.event.type = EVENT_ONLINE;
            res.event.mac = line.substring(line.indexOf(':') + 1);
            res.event.mac.trim();
        }
        else if (line.startsWith("+OFFLINE:")) {
            res.type = PACKET_EVENT;
            res.event.type = EVENT_OFFLINE;
            res.event.mac = line.substring(line.indexOf(':') + 1);
            res.event.mac.trim();
        }
        else if (line.startsWith("+READY")) {
            res.type = PACKET_EVENT;
            res.event.type = EVENT_READY;
            res.event.mac = "";
        }
    }
    return res;
}

