import time
from mnl80a.client import MNL80AClient
from mnl80a.commands import MNL80ACommands

def main():
    # Replace '/dev/ttyUSB0' with your actual serial port (e.g., COM3 on Windows)
    client = MNL80AClient(port='/dev/ttyUSB0', baudrate=115200)
    
    # Initialize the command set wrapper
    mnl80a = MNL80ACommands(client)
    
    # Example callbacks for asynchronous messages
    client.on_ready = lambda: print("[EVENT] Module is READY!")
    client.on_online = lambda mac: print(f"[EVENT] Device ONLINE: {mac}")
    client.on_offline = lambda mac: print(f"[EVENT] Device OFFLINE: {mac}")
    client.on_recv = lambda mac, leng, data, flag: print(f"[EVENT] RECV from {mac}: {data} (Flag: {flag})")
    
    # Start the listening thread for the background async events
    client.start_listening()
    
    try:
        # Check software version
        version = mnl80a.get_software_version()
        print(f"Software Version: {version}")
        
        # Get topology
        topo = mnl80a.get_topology_node_count()
        print(f"Topology Count Response: {topo}")
        
        # Keep example running to listen for background events
        print("Listening for 10 seconds. Check module interaction...")
        time.sleep(10)
        
    finally:
        client.close()

if __name__ == "__main__":
    main()
