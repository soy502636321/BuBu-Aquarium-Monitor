# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "D:/Espressif/frameworks/esp-idf-v5.3.1/components/bootloader/subproject"
  "D:/BuBu-Aquarium-Monitor/firmware/esp32-s3/build/bootloader"
  "D:/BuBu-Aquarium-Monitor/firmware/esp32-s3/build/bootloader-prefix"
  "D:/BuBu-Aquarium-Monitor/firmware/esp32-s3/build/bootloader-prefix/tmp"
  "D:/BuBu-Aquarium-Monitor/firmware/esp32-s3/build/bootloader-prefix/src/bootloader-stamp"
  "D:/BuBu-Aquarium-Monitor/firmware/esp32-s3/build/bootloader-prefix/src"
  "D:/BuBu-Aquarium-Monitor/firmware/esp32-s3/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "D:/BuBu-Aquarium-Monitor/firmware/esp32-s3/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "D:/BuBu-Aquarium-Monitor/firmware/esp32-s3/build/bootloader-prefix/src/bootloader-stamp${cfgdir}") # cfgdir has leading slash
endif()
