# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "E:/SoftWare/Esp_idf/esp-idf/Espressif/frameworks/esp-idf-v5.3.1/components/bootloader/subproject"
  "E:/Documents/ESP32/CAN_Bus/TWAI_MotorControl/build/bootloader"
  "E:/Documents/ESP32/CAN_Bus/TWAI_MotorControl/build/bootloader-prefix"
  "E:/Documents/ESP32/CAN_Bus/TWAI_MotorControl/build/bootloader-prefix/tmp"
  "E:/Documents/ESP32/CAN_Bus/TWAI_MotorControl/build/bootloader-prefix/src/bootloader-stamp"
  "E:/Documents/ESP32/CAN_Bus/TWAI_MotorControl/build/bootloader-prefix/src"
  "E:/Documents/ESP32/CAN_Bus/TWAI_MotorControl/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "E:/Documents/ESP32/CAN_Bus/TWAI_MotorControl/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "E:/Documents/ESP32/CAN_Bus/TWAI_MotorControl/build/bootloader-prefix/src/bootloader-stamp${cfgdir}") # cfgdir has leading slash
endif()
