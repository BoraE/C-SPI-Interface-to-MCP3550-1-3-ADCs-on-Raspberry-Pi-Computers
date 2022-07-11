/**
 * Tests for the MCP 3553 analog-to-digital converter chip.
 *
 * Copyright (C) 2022 Bora Eryilmaz
 */

#include <mcp3553.h>

#include <stdio.h>
#include <assert.h>
#include <cmath>

void test() {
  using namespace mcp3553;
  MCP3553 adc{mcp3553::CE1_PIN, IC::MCP3553};

  /**
   * The 24-bit digital output codes read from the device correspond to
   * decimal codes encoding the 22-bit conversion data and the two
   * overflow conditions.
   */
  printf("Testing decimal codes corresponding to digital output codes...\n");
  // Overflow
  assert(adc.get_code(0x600001) == (int32_t)2097153);
  assert(adc.get_code(0x600000) == (int32_t)2097152);
  // Normal range
  assert(adc.get_code(0x1FFFFF) == (int32_t)2097151);
  assert(adc.get_code(0x000002) == (int32_t)2);
  assert(adc.get_code(0x000001) == (int32_t)1);
  assert(adc.get_code(0x000000) == (int32_t)0);
  assert(adc.get_code(0x3FFFFF) == (int32_t)-1);
  assert(adc.get_code(0x3FFFFE) == (int32_t)-2);
  assert(adc.get_code(0x200000) == (int32_t)-2097152);
  // Underflow
  assert(adc.get_code(0x9FFFFF) == (int32_t)-2097153);
  assert(adc.get_code(0x9FFFFE) == (int32_t)-2097154);
  printf("Done.\n\n");


  /**
   * Voltages, with respect to Vref, corresponding to the 24-bit digital
   * output codes read from the device cover an overrange of 12% below
   * and above the Vref.
   */
  printf("Testing voltages corresponding to digital output codes...\n");
  double LSB = Vref / (double)0x200000;
  // Overflow
  assert(adc.get_value(0x600001) == Vref + LSB);
  assert(adc.get_value(0x600000) == Vref);
  // Normal range
  assert(adc.get_value(0x1FFFFF) == Vref - LSB);
  assert(adc.get_value(0x000002) == 2.0 * LSB);
  assert(adc.get_value(0x000001) == 1.0 * LSB);
  assert(adc.get_value(0x000000) == 0.0);
  assert(adc.get_value(0x3FFFFF) == -1.0 * LSB);
  assert(adc.get_value(0x3FFFFE) == -2.0 * LSB);
  assert(adc.get_value(0x200000) == -1.0 * Vref);
  // Underflow
  assert(adc.get_value(0x9FFFFF) == -Vref - 1.0 * LSB);
  assert(adc.get_value(0x9FFFFE) == -Vref - 2.0 * LSB);
  printf("Done.\n\n");


  /**
   * With VIN+ pin connected to VREF and VIN- pin connected to VREF,
   * the converted value must be, within specifications, equal to 0 V.
   */
  printf("Testing the differential ADC value at 0 V...\n");
  printf("Connect both the VIN+ and VIN- pins to VREF and press Enter.");
  getchar();
  auto value = adc.get_value();
  printf("ADC value: %f\n", value);
  assert(std::fabs(value) < 0.0003);
  printf("Done.\n\n");


  /**
   * With VIN+ pin connected to GND and VIN- pin connected to VREF,
   * the converted value must be, within specifications, equal to -VREF.
   */
  printf("Testing the differential ADC value at -VREF...\n");
  printf("Connect VIN+ to GND and VIN- to VREF and press Enter.");
  getchar();
  value = adc.get_value();
  printf("ADC value: %f\n", value);
  assert(std::fabs(-Vref - value) < 0.001);
  printf("Done.\n\n");


  /**
   * With VIN+ pin connected to 5V and VIN- pin connected to VREF,
   * the converted value must be, within specifications, equal to 5.0-VREF.
   */
  printf("Testing the differential ADC value at 5.0-VREF...\n");
  printf("Connect VIN+ to 5V and VIN- to VREF and press Enter.");
  getchar();
  value = adc.get_value();
  printf("ADC value: %f\n", value);
  assert(std::fabs(5.22 - Vref - value) < 0.007);
  printf("Done.\n\n");


  /**
   * With VIN+ pin connected to 3.3V and VIN- pin connected to VREF,
   * the converted value must be, within specifications, equal to 3.3-VREF
   * volts.
   */
  printf("Testing the differential ADC value at 3.3-VREF...\n");
  printf("Connect VIN+ to 3.3V and VIN- to VREF and press Enter.");
  getchar();
  value = adc.get_value();
  printf("ADC value: %f\n", value);
  assert(std::fabs(3.30 - Vref - value) < 0.008);
  printf("Done.\n");
}


int main() {
  if (!bcm2835_init()) {
    printf("bcm2835_init failed. Are you running as root?\n");
    return 1;
  }

  // Run tests
  test();

  // Clean up
  bcm2835_close();
  return 0;
}
