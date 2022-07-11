/*
 * Class and variable definitions for the MCP 3550/1/3 analog-to-digital converters.
 *
 * Copyright (C) 2022 Bora Eryilmaz
 */

#ifndef MCP3553_H
#define MCP3553_H

#include <bcm2835.h>
#include <stdint.h>

namespace mcp3553 {
  // Device identifiers
  enum class IC {MCP3550_50, MCP3550_60, MCP3551, MCP3553};

  // Clock Divider (uncomment for current RPi device)
  //constexpr uint16_t CLK_DIVIDER = 8000;  // 50 kHz for 400 MHz core clock of RPi Zero.
  //constexpr uint16_t CLK_DIVIDER = 11000; // 50 kHz for ??? MHz core clock of RPi 400.
  constexpr uint16_t CLK_DIVIDER = 10000; // 50 kHz for ??? MHz core clock of RPi 4.

  // GPIO pins
  constexpr uint8_t MOSI_PIN = RPI_V2_GPIO_P1_19; ///< GPIO 10 (MOSI)
  constexpr uint8_t MISO_PIN = RPI_V2_GPIO_P1_21; ///< GPIO 09 (MISO)
  constexpr uint8_t CLK_PIN  = RPI_V2_GPIO_P1_23; ///< GPIO 11 (CLK)
  constexpr uint8_t CE1_PIN  = RPI_V2_GPIO_P1_26; ///< GPIO 07 (CE1)
  constexpr uint8_t CE2_PIN  = RPI_V2_GPIO_P1_15; ///< GPIO 22 (User-selected CE pin)

  // Timing
  constexpr uint32_t tPUCSL = 20; // 10 us typical.
  constexpr uint32_t tCSHSD = 20; // 10 us typical.

  constexpr uint32_t tCONV_MCP3550_50 = 83000; // 80000 us typical for MCP3550_50. (not tested)
  constexpr uint32_t tCONV_MCP3550_60 = 69000; // 66670 us typical for MCP3550_60. (not tested)
  constexpr uint32_t tCONV_MCP3551 = 75000; // 73100 us typical for MCP3551.
  constexpr uint32_t tCONV_MCP3553 = 19455; // 16670 us typical for MCP3553.

  // Voltage reference
  constexpr double Vref = 2.500; // V
}

class MCP3553 {
public:
  MCP3553(uint8_t ce_pin, mcp3553::IC ic);
  ~MCP3553();

  static void startup();

  double get_value() const;
  static double get_value(int32_t reg);
  static int32_t get_code(int32_t reg);

private:
  static void spi_begin();
  static void spi_end();

  int32_t read_register() const;

  uint8_t _ce_pin;
  uint32_t _tCONV;
};

#endif
