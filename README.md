# KeyMaster ESP32-S3

KeyMaster is an ESP32-S3-based keystroke injection tool inspired by popular devices like Rubber Ducky and Bash Bunny. It allows you to program, queue, and execute complex keyboard sequences through a web interface or with the press of a button. The device can emulate a keyboard and execute pre-defined or custom keystrokes, making it a powerful tool for automation, testing, or penetration testing tasks.

## Features:

- Web interface for easy setup and command input
- Queue commands for execution at the press of a physical button
- USB HID keyboard emulation
- Upload and execute sequences from files
- Easy step-by-step command creation and execution
- Supports common keyboard shortcuts and custom text commands

## List of Supported Shortcuts/Key Combos:

- win+r: Open Run dialog
- win+d: Show desktop
- win+l: Lock screen
- win+x: Open Quick Link menu
- ctrl+t: Open new tab (browser)
- ctrl+x: Cut
- ctrl+w: Close tab/window
- ctrl+a: Select all
- ctrl+c: Copy
- ctrl+v: Paste
- ctrl+alt+t: Open terminal (Linux)
- alt+f4: Close window

## How It Works:

1. **WiFi Access Point:**
   - Connect to the device via WiFi (SSID: EPS, Password: 12345678).
   
2. **Web Interface:**
   - Open a browser and navigate to the device’s IP (default: 192.168.4.1).
   - Use the intuitive interface to build keystroke sequences or upload pre-prepared command files.
   
3. **Execute Commands:**
   - Press the execute button (GPIO 14) to run the queued commands instantly.
   
4. **Programming Custom Sequences:**
   - Use the interface to add custom text or command-based actions (e.g., win+r, ctrl+v, sleep).

## Getting Started:

1. Flash the firmware to your ESP32-S3 using your preferred method.
2. Power up the device, connect to its WiFi, and access the web interface.
3. Create or upload your keystroke sequences and start automating!
