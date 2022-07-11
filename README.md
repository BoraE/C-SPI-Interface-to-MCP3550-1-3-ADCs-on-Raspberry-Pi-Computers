# C-SPI-Interface-to-MCP3550-1-3-ADCs-on-Raspberry-Pi-Computers
A C++ class to communicate with the MCP3550/1/3 analog-to-digital (A/D) converters over the SPI protocol on Raspberry Pi (Zero, 4, 400) computers.

## Synopsis
[MCP3550/1/3](https://www.microchip.com/en-us/product/MCP3553) is a 22-bit, single-channel analog-to-digital converter that can be operated using the SPI communication protocol available on Raspberry Pi (Zero, 4, 400) computers.

## Requirements
To be able to compile this code, you need to install the latest version of the [C library for Broadcom BCM 2835 as used in Raspberry Pi](http://www.airspayce.com/mikem/bcm2835/) by Mike McCauley.

## Compilation
Use the Makefile to compile the test example or modify it to suit your needs:
- To compile the test example, run `make all`
- To generate documentation, run `make doc`. You need to install [Doxygen](https://www.doxygen.nl/manual/index.html) to generate documentation.

## Test Code
The file `test_mcp3553.cpp` constains an example. After building the code, run it as `sudo ./build/test_mcp3553` or `make test`.
