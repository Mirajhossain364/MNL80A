#ifndef MNL80A_H
#define MNL80A_H

#include <Arduino.h>

// 1. Fundamental Enums
enum PLCPacketType {
    PACKET_NONE,
    PACKET_DATA,
    PACKET_EVENT
};

enum PLCEventType {
    EVENT_NONE,
    EVENT_DATA,
    EVENT_ONLINE,
    EVENT_OFFLINE,
    EVENT_READY
};

// 2. Data Structures
struct PLCMessage {
    String mac;
    String data;
    int len;
    int type; // 0: Hex, 1: String
};

struct PLCEvent {
    PLCEventType type;
    String mac;
};

// 3. Response Wrapper (Must be AFTER Message and Event)
struct PLCResponse {
    PLCPacketType type;
    PLCMessage message;
    PLCEvent event;
};

class MNL80A {
public:
    MNL80A(HardwareSerial& serial);
    
    // Setup (Default 115200, 8E1)
    void begin(int8_t rxPin, int8_t txPin, unsigned long baud = 115200);

    // --- Information Commands ---
    String getMac();                
    String getCcoMac();             
    String getRole();               
    String getMode();               
    String getUartConfig();         
    String getTopoNum();            
    String getTopoInfo();           
    
    // --- Configuration ---
    bool setNotifyEn(bool enable);         
    bool setWhitelistEn(bool enable);      

    // --- Communication ---
    bool sendData(String mac, String data, bool isString = true);
    String sendRaw(String cmd, uint32_t timeout = 1000);

    // --- The Packet Engine ---
    // Use readPacket() as your primary loop handler
    PLCResponse readPacket();

    // Legacy support (optional - you can keep these if you want)
    bool available(PLCMessage &msg); 
    bool getEvent(PLCEvent &ev);     

private:
    HardwareSerial& _serial;
    String _buffer;
    bool parseIncoming(String line, PLCMessage &msg);
    String parseValue(String res, String key);
};

#endif