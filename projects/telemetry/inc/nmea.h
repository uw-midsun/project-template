#pragma once

// This module is used to parse and represent NMEA data. Used in the GPS module

#include <stdint.h>
#include <stdio.h>
#include "status.h"

// This enum contains the list of NMEA sentences (not all of then are supported yet)
// because they do not contain useful information
typedef enum {
  NMEA_UNKNOWN = 0,
  NMEA_GGA,
  NMEA_GLL,
  NMEA_GSA,
  NMEA_GSV,
  NMEA_RMC,
  NMEA_VTG,
  NUM_NMEA_MESSAGE_ID_TYPES
} NmeaMessageID;

// An enum to represent a position fix's validity (defined by 1, 2, or 6 as described in
// https://www.linxtechnologies.com/wp/wp-content/uploads/rxm-gps-f4.pdf
// on page 14)

typedef enum {
  NMEA_PF_INVALID = 0,
  NMEA_PF_GPS_SPS = 1,
  NMEA_PF_DIFFERENTIAL_GPS_SPS = 2,
  NMEA_PF_DEAD_RECKONING_MODE = 6,
  NUM_NMEA_POSITION_FIX_TYPES = 4
} NmeaPositionFix;

typedef struct {
  uint8_t hh;    // Hours
  uint8_t mm;    // Minutes
  uint8_t ss;    // Seconds
  uint16_t sss;  // Milliseconds
} nmea_utc_time;

// Representation of longitude or latitude
// https://en.wikipedia.org/wiki/Longitude
typedef struct {
  uint16_t degrees;
  uint16_t minutes;
  uint16_t fraction;
} nmea_coord;

// Info passed from the GPS chip should be dropped into this struct (more fields
// coming soon)

typedef struct {
  NmeaPositionFix position_fix;  // True if this struct has valid data
  uint16_t satellites_used;
  uint16_t adc;                        // Age of diff. corr. in seconds
  uint16_t drs;                        // Diff. Ref. Station. Not sure what it is yet
  uint16_t hdop_integer;               // Horizontal dilution of precision, characteristic
  uint16_t hdop_fraction;              // Horizontal dilution of precision, mantissa
  uint16_t msl_altitude_integer;       // In meters, characteristic
  uint16_t msl_altitude_fraction;      // In meters, mantissa
  uint16_t geoid_seperation_integer;   // In meters, characteristic
  uint16_t geoid_seperation_fraction;  // In meters, mantissa
  nmea_coord latitude;
  nmea_coord longitude;
  nmea_utc_time time;
  char east_west;               // East or West. E for East, W for West
  char north_south;             // North or South. N for North, S for South
  uint16_t units_msl_altitude;  // Indicated units of above, should be M for meters.
  char units_geoid_seperation;  // Indicates units of above, should be M for meters.
  NmeaMessageID message_id;
} nmea_gga_sentence;

typedef struct {
  uint16_t measure_heading_degrees_integer;   // Whole number of degrees representing the heading
  uint16_t measure_heading_degrees_fraction;  // Decimal part of degrees representing the heading
  uint16_t speed_kmh_integer;                 // Speed in km/h
  uint16_t speed_kmh_fraction;                // Speed in km/h, fractional part (after the decimal)
} nmea_vtg_sentence;

// Parsing functions for NMEA sentences

// Tests if the given string is a valid NMEA message. Must be null terminated, will return
// STATUS_CODE_OK if the message is valid.
StatusCode nmea_valid(const char *to_check);

// Returns the NMEA sentence type. The result will be stored in the second parameter.
// The function will return a StatusCode indicating success or failure.
StatusCode nmea_sentence_type(const char *nmea_input, NmeaMessageID *result);

// These functions parse NMEA sentences. The result will be stored in the second parameter.
// The function will return a StatusCode indicating success or failure.
StatusCode nmea_get_gga_sentence(const char *nmea_input, nmea_gga_sentence *result);
StatusCode nmea_get_vtg_sentence(const char *nmea_input, nmea_vtg_sentence *result);