from typing import List, Optional

class MNL80ACommands:
    """
    Wrapper for Jiaxun MNL80A PLC Series Module AT Commands.
    """
    def __init__(self, client):
        self.client = client

    def get_software_version(self) -> str:
        """AT+GMR: Query Module Software Version."""
        return self.client.send_command("AT+GMR?")

    def restart(self) -> str:
        """AT+RST: Restart the module."""
        return self.client.send_command("AT+RST")

    def get_mac_address(self) -> str:
        """AT+MAC?: Get MAC Address."""
        return self.client.send_command("AT+MAC?")

    def set_mac_address(self, mac: str) -> str:
        """AT+MAC=<mac>: Set MAC Address."""
        return self.client.send_command(f"AT+MAC={mac}")
        
    def get_topology_node_count(self) -> str:
        """AT+TOPONUM?: Get network topology node number."""
        return self.client.send_command("AT+TOPONUM?")

    def get_topology_info(self, req_start: int, req_cnt: int) -> str:
        """AT+TOPOINFO=<req_start>,<req_cnt>: Get topology info."""
        return self.client.send_command(f"AT+TOPOINFO={req_start},{req_cnt}")

    def add_whitelist(self, macs: List[str]) -> str:
        """AT+WHADD=<cnt>,<macs>: Add Whitelist."""
        mac_str = ";".join(macs)
        return self.client.send_command(f"AT+WHADD={len(macs)},\"{mac_str}\"")
        
    def remove_whitelist(self, macs: List[str]) -> str:
        """AT+WHDEL=<cnt>,<macs>: Delete Whitelist."""
        mac_str = ";".join(macs)
        return self.client.send_command(f"AT+WHDEL={len(macs)},\"{mac_str}\"")

    def clear_whitelist(self) -> str:
        """AT+WHCLR: Clear Whitelist."""
        return self.client.send_command("AT+WHCLR")

    def set_whitelist_status(self, enabled: bool) -> str:
        """AT+WHSTATUS=<status>: Set Whitelist Status."""
        status_val = 1 if enabled else 0
        return self.client.send_command(f"AT+WHSTATUS={status_val}")

    def send_data(self, mac: str, data: str, data_type: int = 1) -> str:
        """AT+SEND=<mac>,<len>,<data>,[flag]: Data Transmission (default string)."""
        # data_type = 0 for hex, 1 for string
        return self.client.send_command(f"AT+SEND={mac},{len(data)},\"{data}\",{data_type}")
        
    def set_io_param(self, pin: int, pin_type: int, state: int, pwm_freq: int = 5, pwm_ctl_mode: int = 0, pwm_adjust_time: int = 0) -> str:
        """AT+IOPARAM: Set GPIO Primary Configuration."""
        return self.client.send_command(f"AT+IOPARAM={pin},{pin_type},{state},{pwm_freq},{pwm_ctl_mode},{pwm_adjust_time}")
        
    def set_io_ctrl(self, mac: str, pin: int, state: Optional[int] = None) -> str:
        """AT+IOCTRL: GPIO State Control on another node."""
        if state is not None:
            return self.client.send_command(f"AT+IOCTRL={mac},{pin},{state}")
        return self.client.send_command(f"AT+IOCTRL={mac},{pin}")
