import serial
import time
import threading
from typing import Callable, Optional

class MNL80AClient:
    """
    Client for interacting with the Jiaxun MNL80A PLC Module via Serial AT Commands.
    """
    def __init__(self, port: str, baudrate: int = 115200, timeout: float = 2.0):
        self.port = port
        self.baudrate = baudrate
        self.timeout = timeout
        self.serial = serial.Serial(port, baudrate, timeout=timeout)
        self._running = False
        self._read_thread = None
        
        # Callbacks for asynchronous events from the module
        self.on_ready: Optional[Callable[[], None]] = None
        self.on_online: Optional[Callable[[str], None]] = None
        self.on_offline: Optional[Callable[[str], None]] = None
        self.on_recv: Optional[Callable[[str, int, str, int], None]] = None

    def start_listening(self):
        """Starts a background thread to listen for event notifications like +READY, +ONLINE, +RECV."""
        if not self._running:
            self._running = True
            self._read_thread = threading.Thread(target=self._read_loop, daemon=True)
            self._read_thread.start()

    def stop_listening(self):
        """Stops the background listening thread."""
        self._running = False
        if self._read_thread:
            self._read_thread.join()

    def _read_loop(self):
        while self._running and self.serial.is_open:
            if self.serial.in_waiting > 0:
                try:
                    line = self.serial.readline().decode('utf-8').strip()
                    if line:
                        self._handle_async_event(line)
                except Exception as e:
                    print(f"Error reading serial: {e}")
            time.sleep(0.01)

    def _handle_async_event(self, line: str):
        if line.startswith("+READY") and self.on_ready:
            self.on_ready()
        elif line.startswith("+ONLINE:") and self.on_online:
            mac = line.split(":")[1].strip()
            self.on_online(mac)
        elif line.startswith("+OFFLINE:") and self.on_offline:
            mac = line.split(":")[1].strip()
            self.on_offline(mac)
        elif line.startswith("+RECV:") and self.on_recv:
            # +RECV:<mac>,<len>,<data>,<flag>
            parts = line[6:].split(',', 3)
            if len(parts) == 4:
                mac, length, data, flag = parts
                self.on_recv(mac.strip(), int(length), data.strip(), int(flag))

    def send_command(self, cmd: str, wait_for_ok: bool = True, wait_time: float = 0.5) -> str:
        """Sends an AT command to the module and optionally waits for OK/ERROR."""
        full_cmd = f"{cmd}\r\n"
        self.serial.write(full_cmd.encode('utf-8'))
        
        if not wait_for_ok:
            return ""

        time.sleep(wait_time)
        response = []
        while self.serial.in_waiting > 0:
            try:
                line = self.serial.readline().decode('utf-8').strip()
                if line:
                    response.append(line)
            except Exception:
                pass
        return "\n".join(response)

    def close(self):
        """Closes the serial connection."""
        self.stop_listening()
        if self.serial.is_open:
            self.serial.close()
