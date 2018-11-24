#pragma once
// This will be the GPS driver. It will be responsive for initializing, and cleaning up the GPS
// as well as providing data from the GPS.

// On GPS data: the GPS sends NMEA (National Marine Electronics Association) messages which
// contain data about current time, position, speed, etc. The message we are most interested
// in is the GGA message (Global Positioning System Fix Data). It contains latitude and
// longitude.
#include <stdbool.h>
#include "nmea.h"
#include "status.h"
#include "uart.h"

// Just some constants so that the max length of raw data can be set.
// A GGA message will be around a hundred characters.
#define GPS_MAX_NMEA_LENGTH 128

// Messages to transmit to the GPS module to turn off unneeded messages
#define GPS_GLL_OFF "$PSRF103,01,00,00,01*27\r\n" // GLL: Geographic Position - Latitude/Longitude
#define GPS_GSA_OFF "$PSRF103,02,00,00,01*26\r\n" // GSA: GPS DOP and Active Satellites
#define GPS_GSV_OFF "$PSRF103,03,00,00,01*27\r\n" // GSV: GPS Satellites in View
#define GPS_RMC_OFF "$PSRF103,04,00,00,01*20\r\n" // RMC: Recommended Minimum Specific GPS Data

// This struct basically contains all the info about pins etc.
// Check this document on page 4:
// https://www.linxtechnologies.com/wp/wp-content/uploads/evm-gps-f4.pdf
typedef struct {
  UartPort port;
  UartSettings *uart_settings;    // The uart settings to be used
  GpioSettings *settings_power;   // Pin which provides power
  GpioSettings *settings_on_off;  // Pin to trigger power up
  GpioAddress *pin_rx;            // Pin addresses
  GpioAddress *pin_tx;
  GpioAddress *pin_power;
  GpioAddress *pin_on_off;
  UartStorage uart_storage;
  volatile char gga_data[GPS_MAX_NMEA_LENGTH];  // Stores raw NMEA messages sent by the chip
} GpsSettings;

// Initialized the GPS module
StatusCode gps_init();

// Gets GGA data (coordinates, time)
StatusCode gps_get_gga_data(NmeaGgaSentence *result);

// Gets VTG data (speed, heading)
StatusCode gps_get_vtg_data(NmeaVtgSentence *result);
