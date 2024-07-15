# IOT Tank Sensor Module

Radiation, temperature and conductivity monitoring via ESP32 module

## Hardware Requirements

- ESP32 (WROOM32 model used for prototyping)
- USB Cable
- 0.96 inch OLED I2C RGB display
- HiLetgo BH1750 I2C light sensor
- HiLetgo DS18B20 OneWire temperature sensor
- 4.7kΩ resistor
- Breadboard (Optional: for prototyping/testing)

## Recommended Development Environment

Visual Studio Code with PlatformIO extension installed. From here, all developing, compiling, uploading, and testing can take place in one window.

## Folder Structure

```makefile
project_root/
│
├── include/               # Contains header files
│   ├── Settings_Template.h  # Template for settings
│   ├── Settings.h           # Actual settings file, YOU MUST CREATE THIS!
│   ├── splash_image.h
│   └── splash_image2.h
│
├── lib/                   # Library files for custom, private libraries
│   └── README.md          # Information about the libraries
│
├── src/                   # Source files
│   └── main.cpp           # Main program source file
│
└── platformio.ini         # PlatformIO configuration file where dependencies are managed

```

Note 1: Pins for the BH1750 light sensor are the exact same (shared), except for the address wire (yellow) which can remain disconencted.  
Note 2: The temperature sensor uses the OneWire interface so its data line cannot be shared with the LCD or light sensor, so a separate data pin is configured for this. Power however can still be shared across all peripherals.  
Note 3: A 4.7kΩ "pull-up" resistor must be used between the power rail and the data line of the DS18B20 temperature sensor to ensure accurate reading.

## Getting Started

### Set Configuration Variables

Duplicate the `Settings_Template.h` file in the `include` folder and rename it `Settings.h`, then the following variables can be configured:

```cpp
// Wi-Fi Settings (2.4 GHz only)
const char *ssid = "Your Network Here";         // Your Wi-Fi SSID
const char *password = "Network Password Here"; // Your Wi-Fi Password

// Application Settings (modify these values as needed)
const String tankName = "Q2";
const unsigned long sensorReadInterval = 2 * SECOND;      // 2 seconds (Rolling average over 20 seconds)
const unsigned long notificationDelay = 15 * MINUTE;      // 15 minutes
const unsigned int notificationParThreshold = 100;        // PAR threshold for triggering notification
const unsigned int notificationTemperatureThreshold = 30; // Temperature threshold for triggering notification
```

### Building, Running & Testing

You can compile, upload, test and clean the source code all from the button in the top-right of the VSCode window. To run the program on the ESP32, simply press upload and let it do its thing! Once uploaded, open the serial monitor (the socket icon next to the upload button) to debug. Simple!

### Troubleshooting

- If compilation failed and is saying something is undefined, did you ensure you created the `Settings.h` file?
- If the upload fails but the computer recognizes the ESP32 and compilation was successful, you may consider reducing the upload speed as the default can be too fast sometimes. This can be done by simply adding the line `upload_speed = 9600` under "monitor_speed" in the `platformio.ini` file.
- If your computer will not recognize the ESP32, ensure the USB cable you're using supports data transfer and is in good shape.

## Example Images

Coming soon!
