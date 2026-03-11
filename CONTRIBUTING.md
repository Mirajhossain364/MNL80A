# Contributing to MNL80A

Thank you for your interest in contributing to the MNL80A library!

## How to Contribute

1. **Fork the Repository**: Create your own fork and clone it to your machine.
2. **Create a Branch**: Always branch out from `main`.
3. **Make Changes**: Fix a bug, add a feature, or improve the documentation.
   - If adding a new AT command wrapper, ensure you follow the existing style in `mnl80a/commands.py`.
   - Update `tests/test_client.py` as necessary to cover any new logic.
4. **Test**: Run the test suite:
   ```bash
   python -m unittest tests/test_client.py
   ```
5. **Create a Pull Request**: Submit a detailed Pull Request explaining your changes and the issues they resolve.

## Reporting Issues

If you find a bug or have a request for a new feature, please open an issue in the GitHub issue tracker and provide as much detail as possible, including your OS, Python version, and a minimal reproducible example.
