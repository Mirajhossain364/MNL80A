Demo Video : https://youtube.com/shorts/mq8GhsHjBIU?feature=share
Buy Link : https://www.alibaba.com/product-detail/PLC-Module-2KM-Signal-Reach-Power_1601511420527.html?spm=a2700.galleryofferlist.wending_top.i0.2f7b13a0cGKjlB
Mqtt Broker : https://www.hivemq.com/agent/

## MNL80A PLC Library for ESP32

A lightweight C++ library for interfacing the **MNL80A Power Line Communication (PLC)** module with the **ESP32** using Hardware Serial. 
This library simplifies the use of AT commands to manage network roles (CCO/STA), data transmission, and device configuration.

---

### Features

* **Hardware Serial Optimized:** Designed specifically for ESP32 `HardwareSerial`.
* **Automatic Configuration:** Handles the specific `SERIAL_8E1` parity bit requirements of the MNL80A.
* **Network Management:** Functions to retrieve Local MAC, CCO MAC, and network topology (Node Count).
* **Data Transmission:** Supports both String and Hexadecimal data modes.
* **Background Handling:** Built-in logic to enable notifications (`AT+NOTIFY=1`) and disable whitelist status (`AT+WHSTATUS=0`) automatically.

---

### Hardware Connection

When using an ESP32, it is recommended to use **UART2** to keep the primary USB-Serial port free for debugging.

| MNL80A Pin | ESP32 Pin (Default UART2) |
| ---        | --- |
| **TX**     | GPIO 16 (RX2) |
| **RX**     | GPIO 17 (TX2) |
| **GND**    | GND |
| **VCC**    | 3.3V / 5V (Check specific module rating) |

---

### Installation

1. Download this repository as a `.zip` file.
2. In the Arduino IDE, go to **Sketch > Include Library > Add .ZIP Library...**
3. Select the downloaded file.

---

### Quick Start

The library is configured to handle the MNL80A startup sequence in the background, ensuring notifications are active without manual user input.

---
Operating Mode Description
The PLC-IoT module firmware provides two working modes, including: AT mode and
transparent transmission mode.
 AAT Command Mode: In this mode, users can control or configure the module through AT
commands. For example, you can obtain the PLC network topology, set the CCO whitelist, set
the GPIO working mode, etc.
 Transparent Transmission Mode: In this mode, users can automatically send serial data
through the CCO via the PLC network to the STA and transparently transmit it to the STA
serial port. Similarly, serial data can be sent through the STA via the PLC network to the CCO
and transparently transmitted to the CCO serial port, achieving data transparent transmission
between the CCO and the STA.

### Technical Specifications
After the PLC-IoT module is powered on, it defaults to working in AT command mode.
Users can connect to the UART0 serial port of the module and switch to other working modes
through serial commands. The default UART parameter configuration of the PLC-IoT module is
as follows:

* **Default Baud Rate:** 115200 bps
* **Communication Protocol:** AT Command Set
* **Serial Config:** 8 Data bits, **Even Parity**, 1 Stop bit (`SERIAL_8E1`)
* **Operating Modes:** Supports both `AT_MODE` and `TRANSPARENT_MODE`

---

### License

This project is licensed under the MIT License.

---
