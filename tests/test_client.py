import unittest
from unittest.mock import MagicMock
from mnl80a.client import MNL80AClient
from mnl80a.commands import MNL80ACommands

class TestMNL80ACommands(unittest.TestCase):
    def setUp(self):
        self.mock_client = MagicMock(spec=MNL80AClient)
        self.commands = MNL80ACommands(self.mock_client)

    def test_get_software_version(self):
        self.commands.get_software_version()
        self.mock_client.send_command.assert_called_with("AT+GMR?")

    def test_add_whitelist(self):
        self.commands.add_whitelist(["0000C0A80110", "0000C0A80120"])
        self.mock_client.send_command.assert_called_with("AT+WHADD=2,\"0000C0A80110;0000C0A80120\"")

    def test_send_data(self):
        self.commands.send_data("0000C0A80110", "12345", 1)
        self.mock_client.send_command.assert_called_with("AT+SEND=0000C0A80110,5,\"12345\",1")

if __name__ == '__main__':
    unittest.main()
