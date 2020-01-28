<!-- PROJECT SHIELDS -->
<!--
*** I'm using markdown "reference style" links for readability.
*** Reference links are enclosed in brackets [ ] instead of parentheses ( ).
*** See the bottom of this document for the declaration of the reference variables
*** for contributors-url, forks-url, etc. This is an optional, concise syntax you may use.
*** https://www.markdownguide.org/basic-syntax/#reference-style-links
-->
[![Contributors][contributors-shield]][contributors-url]
[![Forks][forks-shield]][forks-url]
[![Stargazers][stars-shield]][stars-url]
[![Issues][issues-shield]][issues-url]
[![MIT License][license-shield]][license-url]
[![LinkedIn][linkedin-shield]][linkedin-url]


<!-- PROJECT LOGO -->
<br />
<p align="center">
  <a href="https://github.com/dianlight/ESP_DFU_Firmware">
    <!--
    <img src="images/logo.png" alt="Logo" width="80" height="80">
    -->
  </a>

  <h3 align="center">Esp8266_DFU_Firmware</h3>

  <p align="center">
    A minimal DFU firmware for ESP8266 devices to allow OTA update for "over 50% size" firmware
    <br />
    <!--
    <a href="https://github.com/dianlight/ESP_DFU_Firmware"><strong>Explore the docs »</strong></a>
    <br />
    <br />
    -->
    <a href="https://github.com/dianlight/ESP_DFU_Firmware/blob/master/CHANGELOG.md">Changelog</a>
    ·
    <a href="https://github.com/dianlight/ESP_DFU_Firmware/issues">ESP_DFU_Firmware Bug</a>
    ·
    <a href="https://github.com/dianlight/ESP_DFU_Firmware/issues">Request Feature</a>
  </p>
</p>

<!-- Donations -->

<a href="https://www.buymeacoffee.com/ypKZ2I0" target="_blank"><img src="https://cdn.buymeacoffee.com/buttons/default-orange.png" alt="Buy Me A Coffee" style="height: 51px !important;width: 217px !important;" ></a>


<!-- TABLE OF CONTENTS -->
## Table of Contents

- [Table of Contents](#table-of-contents)
- [About The Project](#about-the-project)
  - [How it work](#how-it-work)
  - [Built With and for](#built-with-and-for)
    - [Tested Hardware](#tested-hardware)
- [Quick start](#quick-start)
  - [Platform IO integration](#platform-io-integration)
- [Features](#features)
- [Config options](#config-options)
  - [`custom_dfu_project_dir` (_REQUIRED_)](#customdfuprojectdir-required)
  - [`custom_dfu_wait_time` (_DEFAULT_ `35`)](#customdfuwaittime-default-35)
  - [`custom_dfu_build_flags' (_DEFAULT_ *see below*)](#customdfubuildflags-default-see-below)
    - [`SERIAL_DEBUG` (__OPTIONAL__)](#serialdebug-optional)
    - [`MDNS_NAME` (__REQUIRED__)](#mdnsname-required)
    - [`MESSAGE` (__DEFAULT__ `Ready for OTA:`)](#message-default-ready-for-ota)
    - [`USE_WIFIMANAGER` (__OPTIONAL__)](#usewifimanager-optional)
    - [`USE_DISPLAY` (__OPTIONAL__)](#usedisplay-optional)
    - [`I2C_OLED_ADDRESS` (__DEFAULT `0x3C`__)](#i2coledaddress-default-0x3c)
- [Roadmap](#roadmap)
- [Contributing](#contributing)
- [License](#license)
- [Contact](#contact)
- [Acknowledgements](#acknowledgements)



<!-- ABOUT THE PROJECT -->
## About The Project

<!-- [![Product Name Screen Shot][product-screenshot]](https://example.com) -->
ESP8266 MCU are fantastic microcontrollers with a lot of features but one limit can be very ugly.
If you use OTA firmware update only the ___half___ of the memory can be used.

This is a minimal __DFU__ firmware to allow ESP OTA update for ***big firmware!***

### How it work

The basic concept is quite simple. The OTA ESP copies the new firmware to the "free" part of the flash and then on boot "overwrites" the previous one before do the real flash booting.

___So the basic requirement is that there is enough free Flash to load the new firmware before booting....___ ***so only the 50% of the flash can be used!***

**Problem**: If on an ESP with 1Mb of memory your firmware is bigger than 500Kb the OTA goes wrong and you need to use serial programmer.

**Solution**: Before running your firmware OTA, a smaller firmware OTA was run with only the code strictly necessary, effectively freeing up space for the larger firmware OTA.

### Built With and for

The actual version is designed to work with **PlatformIO** on an Unix ( Linux / MacOS ) environment.

#### Tested Hardware
 
* ESP-01 (ESP8266 80MHz, 80KB RAM, 1MB Flash) __Chinese clone__

## Quick start

The project is meant to be used as a module for another project. The advice is to insert it as __git submodule__.

***Currently only integration with PlatformIO has been developed and tested!***

### [Platform IO](https://platformio.org) integration

- In your project directory checkout DFU project as module
  ```bash
  $ git submodule add https://github.com/dianlight/ESP_DFU_Firmware.git
  ```
  This command create a new subdirectory called ESP_DFU_Firmware
- **[Optional]** Configure your DFU firmware editing `${your ptoject dir}/ESP_DFU_Firmware/include/Config.h` @See [Config](#config-options) 
- Modify `platform.ini` in your project
  - Add a new section and configure it @See [Config](#config-options)
  ```ini
  [esp_dfu]
  custom_dfu_project_dir = ./ESP_DFU_Firmware
  custom_dfu_wait_time = 34
  custom_dfu_build_flags =
    ;-DSERIAL_DEBUG
    -DMDNS_NAME=smarttemp 
    "-DMESSAGE=OTA in progress..."
    -DUSE_WIFIMANAGER  
    ; Display are 0=SSD1306Wire | 1=SH1106Wire | 2=SSD1306Spi | 3=SH1106SPi
    -DUSE_DISPLAY=1 
    -DI2C_OLED_ADDRESS=0x3C

  ```
  - Add the script to your [env:..] under `extra_script`
  ```ini
  extra_scripts = 
    ${esp_dfu.custom_dfu_project_dir}/integrations/pio/dfu_trigger.py
  ```
-------

## Features

This are a list of features implemented in the DFU project

- [x] WiFi autoconnect ( use the last successfull connection )
- [x] mDNS autoregister
- [x] WifiManager integration [_Optional_]
- [x] Display [_Optional_]
  - [x] I2C
    - [x] SSD1306 based OLED ( ***Untested!*** )
    - [x] SH1106 based OLED
  - [x] SPI
    - [x] SSD1306 based OLED ( ***Untested!*** )
    - [x] SH1106 based OLED ( ***Untested!*** )

<!-- USAGE EXAMPLES -->
## Config options

All standard options can ben configured from "calling" project in the `[esp_dfu]` section:

### `custom_dfu_project_dir` (_REQUIRED_)

The relative path to the ESP_DFU_Firmware project. If you follow the [Quick start](#quick-start) instruction is _./ESP_DFU_Firmware_

### `custom_dfu_wait_time` (_DEFAULT_ `35`)

Number of seconds between the OTA of the DFU firmware and the ready running on MCU. ___For ESP-01 `35` secons should be ok___

### `custom_dfu_build_flags' (_DEFAULT_ *see below*)

Build dlags used to configure the DFU Firmware. Passwd in the form `-D<FLAG>=<VALUE>`

#### `SERIAL_DEBUG` (__OPTIONAL__)

Debug message the Serial of the MCU

#### `MDNS_NAME` (__REQUIRED__)

The name fo mDNS of the decide. It used to connect and upload the DFU and for register after DFU boot.

#### `MESSAGE` (__DEFAULT__ `Ready for OTA:`)

The messagge to display when DFU is running

#### `USE_WIFIMANAGER` (__OPTIONAL__)

Use the WiFiManager system if no WiFi can be connected after the boot. I use some Flash size but is usefull to _UNBRICK_ your device if there was WiFi issue during update process.

#### `USE_DISPLAY` (__OPTIONAL__)

Display status on display during the OTA process. The value is the display type/connection

0. SSD1306 I2C ( on ESP-01 the pin used are GPIO0->SDA, GPIO1->SDC)
1. SH1106 I2C  ( on ESP-01 the pin used are GPIO0->SDA, GPIO1->SDC)
2. SSD1306 SPI 
3. SH1106 SPI
    
#### `I2C_OLED_ADDRESS` (__DEFAULT `0x3C`__)    

If a I2C display is used is the address of the Display.

<!-- ROADMAP -->
## Roadmap

See the [open issues](https://github.com/dianlight/ESP_DFU_Firmware/issues) for a list of proposed features (and known issues).

<!-- CONTRIBUTING -->
## Contributing

Contributions are what make the open source community such an amazing place to be learn, inspire, and create. Any contributions you make are **greatly appreciated**.

1. Fork the Project
2. Create your Feature Branch (`git checkout -b feature/AmazingFeature`)
3. Commit your Changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to the Branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request



<!-- LICENSE -->
## License

Distributed under the GPL3 License. See `LICENSE` for more information.



<!-- CONTACT -->
## Contact

<!--
Your Name - [@twitter_handle](https://twitter.com/twitter_handle) - email
-->

Project Link: [https://github.com/dianlight/ESP_DFU_Firmware](https://github.com/dianlight/ESP_DFU_Firmware)



<!-- ACKNOWLEDGEMENTS - ->
## Acknowledgements

***!Work in progress!***

* [RobTillaart](https://github.com/RobTillaart/) for the Arduino libraries
* [soligen2010](https://github.com/soligen2010/) for the ClickEncoder fork
  -->



<!-- MARKDOWN LINKS & IMAGES -->
<!-- https://www.markdownguide.org/basic-syntax/#reference-style-links -->
[contributors-shield]: https://img.shields.io/github/contributors/dianlight/ESP_DFU_Firmware.svg?style=flat-square
[contributors-url]: https://github.com/dianlight/ESP_DFU_Firmware/graphs/contributors
[forks-shield]: https://img.shields.io/github/forks/dianlight/ESP_DFU_Firmware.svg?style=flat-square
[forks-url]: https://github.com/dianlight/ESP_DFU_Firmware/network/members
[stars-shield]: https://img.shields.io/github/stars/dianlight/ESP_DFU_Firmware.svg?style=flat-square
[stars-url]: https://github.com/dianlight/ESP_DFU_Firmware/stargazers
[issues-shield]: https://img.shields.io/github/issues/dianlight/ESP_DFU_Firmware.svg?style=flat-square
[issues-url]: https://github.com/dianlight/ESP_DFU_Firmware/issues
[license-shield]: https://img.shields.io/github/license/dianlight/ESP_DFU_Firmware.svg?style=flat-square
[license-url]: https://github.com/dianlight/ESP_DFU_Firmware/blob/master/LICENSE
[linkedin-shield]: https://img.shields.io/badge/-LinkedIn-black.svg?style=flat-square&logo=linkedin&colorB=555
[linkedin-url]: https://www.linkedin.com/in/lucio-tarantino-8ab9a3/
[product-screenshot]: images/screenshot.png
[buy-me-a-coffe]: https://www.buymeacoffee.com/ypKZ2I0