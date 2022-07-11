/**
 * Driver code for the MCP 3550/1/3 analog-to-digital converters.
 *
 * Copyright (C) 2022 Bora Eryilmaz
 */

#include <mcp3553.h>
#include <stdio.h>
#include <stdexcept>

using namespace mcp3553;

MCP3553::MCP3553(uint8_t ce_pin, IC ic) : _ce_pin{ce_pin} {
  // Conversion times of chips vary.
  switch (ic) {
  case IC::MCP3550_50:
    _tCONV = tCONV_MCP3550_50;
    break;
  case IC::MCP3550_60:
    _tCONV = tCONV_MCP3550_60;
    break;
  case IC::MCP3551:
    _tCONV = tCONV_MCP3551;
    break;
  case IC::MCP3553:
    _tCONV = tCONV_MCP3553;
    break;
  }

  spi_begin();
  startup();
}

MCP3553::~MCP3553() {
  spi_end();
}


/**
 * Wait for power up during startup.
 */
void MCP3553::startup() {
  bcm2835_delayMicroseconds(300); // start-up
  bcm2835_delayMicroseconds(tPUCSL);
}


/**
 * Returns the input voltage as a floating-point value between -Vref and Vref
 * volts, allowing positive and negative overflows of about 12%.
 */
double MCP3553::get_value() const {
  // Value read as a right-aligned 24-bit number (xxFFFFFFh).
  const int32_t reg = read_register();
  return get_value(reg);
}


/**
 * Returns the voltage value with respect to Vref for a given digital
 * output code read from the device.
 */
double MCP3553::get_value(int32_t reg) {
  const double LSB = Vref / (double)0x200000;
  const double voltage = (double)get_code(reg) * LSB;
  return voltage;
}


/**
 * Returns the 32-bit, signed decimal code corresponding to a 24-bit digital
 * output code read from the device, taking into account positive and negative
 * overflow conditions.
 */
int32_t MCP3553::get_code(int32_t reg) {
  const bool ovl = reg & (1 << 23);
  const bool ovh = reg & (1 << 22);

  int32_t code = reg;
  if (ovh) {
    // Positive overflow.
    code &= 0x3FFFFF; // Keep lower 22 bits, which also removes the OVH bit.
  } else if (ovl) {
    // Negative overflow.
    code &= 0x3FFFFF; // Keep lower 22 bits, which also removes the OVL bit.
    code = -((1 << 22) - code); // Convert to two's complement at 32-bit.
  } else if (code & (1 << 21)) {
    // Negative value.
    code = -((1 << 22) - code); // Convert to two's complement at 32-bit.
  }
  return code;
}


void MCP3553::spi_begin() {
  if (!bcm2835_spi_begin()) {
    throw std::runtime_error("Could not configure SPI for communicating with MCP3553.");
  }

  bcm2835_spi_setBitOrder(BCM2835_SPI_BIT_ORDER_MSBFIRST);
  bcm2835_spi_setDataMode(BCM2835_SPI_MODE3); // CPOL = 1, CPHA = 1
  bcm2835_spi_setClockDivider(CLK_DIVIDER); // 50 kHz clock.
  bcm2835_spi_chipSelect(BCM2835_SPI_CS_NONE); // No CS. Manual chip select.

  // Set CSPOL to LOW.
  volatile uint32_t* paddr = bcm2835_spi0 + BCM2835_SPI0_CS/4;
  bcm2835_peri_set_bits(paddr, 0, BCM2835_SPI0_CS_CSPOL); // CS is active low.
}


void MCP3553::spi_end() {
  bcm2835_spi_end();
}


int32_t MCP3553::read_register() const {
  spi_end();

  // Temporarily switch MISO and CE1 pins to be normal GPIO pins.
  bcm2835_gpio_fsel(_ce_pin,  BCM2835_GPIO_FSEL_OUTP);
  bcm2835_gpio_fsel(MISO_PIN, BCM2835_GPIO_FSEL_INPT);

  // Go into shutdown mode.
  bcm2835_gpio_write(_ce_pin, HIGH);
  bcm2835_delayMicroseconds(tCSHSD);

  // Trigger single conversion.
  bcm2835_gpio_write(_ce_pin, LOW);
  bcm2835_delayMicroseconds(_tCONV/2);
  bcm2835_gpio_write(_ce_pin, HIGH);
  bcm2835_delayMicroseconds(_tCONV/2);

  // Switch back to SPI mode.
  spi_begin();

  // Read the bytes in.
  char buffer[3] = {0, 0, 0};
  bcm2835_gpio_fsel(_ce_pin, BCM2835_GPIO_FSEL_OUTP);
  bcm2835_gpio_write(_ce_pin, LOW); // Chip select.
  bcm2835_spi_transfern(buffer, 3);
  bcm2835_gpio_write(_ce_pin, HIGH); // Chip deselect.
  int32_t value = ((int32_t)buffer[0] << 16) | ((int32_t)buffer[1] << 8) | (uint32_t)buffer[2];
  return value;
}
