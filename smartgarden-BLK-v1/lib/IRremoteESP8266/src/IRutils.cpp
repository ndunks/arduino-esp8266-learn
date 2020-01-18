// Copyright 2017 David Conran

#include "IRutils.h"
#ifndef UNIT_TEST
#include <Arduino.h>
#endif

#define __STDC_LIMIT_MACROS
#include <stdint.h>
#include <string.h>
#include <algorithm>
#ifndef ARDUINO
#include <string>
#endif
#include "IRrecv.h"
#include "IRremoteESP8266.h"


// Reverse the order of the requested least significant nr. of bits.
// Args:
//   input: Bit pattern/integer to reverse.
//   nbits: Nr. of bits to reverse.
// Returns:
//   The reversed bit pattern.
uint64_t reverseBits(uint64_t input, uint16_t nbits) {
  if (nbits <= 1) return input;  // Reversing <= 1 bits makes no change at all.
  // Cap the nr. of bits to rotate to the max nr. of bits in the input.
  nbits = std::min(nbits, (uint16_t)(sizeof(input) * 8));
  uint64_t output = 0;
  for (uint16_t i = 0; i < nbits; i++) {
    output <<= 1;
    output |= (input & 1);
    input >>= 1;
  }
  // Merge any remaining unreversed bits back to the top of the reversed bits.
  return (input << nbits) | output;
}

// Convert a uint64_t (unsigned long long) to a string.
// Arduino String/toInt/Serial.print() can't handle printing 64 bit values.
//
// Args:
//   input: The value to print
//   base:  The output base.
// Returns:
//   A string representation of the integer.
// Note: Based on Arduino's Print::printNumber()
String uint64ToString(uint64_t input, uint8_t base) {
  String result = "";
  // prevent issues if called with base <= 1
  if (base < 2) base = 10;
  // Check we have a base that we can actually print.
  // i.e. [0-9A-Z] == 36
  if (base > 36) base = 10;

  // Reserve some string space to reduce fragmentation.
  // 16 bytes should store a uint64 in hex text which is the likely worst case.
  // 64 bytes would be the worst case (base 2).
  result.reserve(16);

  do {
    char c = input % base;
    input /= base;

    if (c < 10)
      c += '0';
    else
      c += 'A' - 10;
    result = c + result;
  } while (input);
  return result;
}

#ifdef ARDUINO
// Print a uint64_t/unsigned long long to the Serial port
// Serial.print() can't handle printing long longs. (uint64_t)
//
// Args:
//   input: The value to print
//   base: The output base.
void serialPrintUint64(uint64_t input, uint8_t base) {
  Serial.print(uint64ToString(input, base));
}
#endif

// Convert a C-style str to a decode_type_t
//
// Args:
//   str:  A C-style string containing a protocol name or number.
// Returns:
//  A decode_type_t enum.
decode_type_t strToDecodeType(const char * const str) {
  if (!strcasecmp(str, "Unknown"))
    return decode_type_t::UNKNOWN;
  else if (!strcasecmp(str, "UNUSED"))
    return decode_type_t::UNUSED;
  else if (!strcasecmp(str, "AIWA_RC_T501"))
    return decode_type_t::AIWA_RC_T501;
  else if (!strcasecmp(str, "AMCOR"))
    return decode_type_t::AMCOR;
  else if (!strcasecmp(str, "ARGO"))
    return decode_type_t::ARGO;
  else if (!strcasecmp(str, "CARRIER_AC"))
    return decode_type_t::CARRIER_AC;
  else if (!strcasecmp(str, "COOLIX"))
    return decode_type_t::COOLIX;
  else if (!strcasecmp(str, "DAIKIN"))
    return decode_type_t::DAIKIN;
  else if (!strcasecmp(str, "DAIKIN128"))
    return decode_type_t::DAIKIN128;
  else if (!strcasecmp(str, "DAIKIN152"))
    return decode_type_t::DAIKIN152;
  else if (!strcasecmp(str, "DAIKIN160"))
    return decode_type_t::DAIKIN160;
  else if (!strcasecmp(str, "DAIKIN176"))
    return decode_type_t::DAIKIN176;
  else if (!strcasecmp(str, "DAIKIN2"))
    return decode_type_t::DAIKIN2;
  else if (!strcasecmp(str, "DAIKIN216"))
    return decode_type_t::DAIKIN216;
  else if (!strcasecmp(str, "DENON"))
    return decode_type_t::DENON;
  else if (!strcasecmp(str, "DISH"))
    return decode_type_t::DISH;
  else if (!strcasecmp(str, "ELECTRA_AC"))
    return decode_type_t::ELECTRA_AC;
  else if (!strcasecmp(str, "FUJITSU_AC"))
    return decode_type_t::FUJITSU_AC;
  else if (!strcasecmp(str, "GICABLE"))
    return decode_type_t::GICABLE;
  else if (!strcasecmp(str, "GLOBALCACHE"))
    return decode_type_t::GLOBALCACHE;
  else if (!strcasecmp(str, "GOODWEATHER"))
    return decode_type_t::GOODWEATHER;
  else if (!strcasecmp(str, "GREE"))
    return decode_type_t::GREE;
  else if (!strcasecmp(str, "HAIER_AC"))
    return decode_type_t::HAIER_AC;
  else if (!strcasecmp(str, "HAIER_AC_YRW02"))
    return decode_type_t::HAIER_AC_YRW02;
  else if (!strcasecmp(str, "HITACHI_AC"))
    return decode_type_t::HITACHI_AC;
  else if (!strcasecmp(str, "HITACHI_AC1"))
    return decode_type_t::HITACHI_AC1;
  else if (!strcasecmp(str, "HITACHI_AC2"))
    return decode_type_t::HITACHI_AC2;
  else if (!strcasecmp(str, "HITACHI_AC424"))
    return decode_type_t::HITACHI_AC424;
  else if (!strcasecmp(str, "INAX"))
    return decode_type_t::INAX;
  else if (!strcasecmp(str, "JVC"))
    return decode_type_t::JVC;
  else if (!strcasecmp(str, "KELVINATOR"))
    return decode_type_t::KELVINATOR;
  else if (!strcasecmp(str, "LEGOPF"))
    return decode_type_t::LEGOPF;
  else if (!strcasecmp(str, "LG"))
    return decode_type_t::LG;
  else if (!strcasecmp(str, "LG2"))
    return decode_type_t::LG2;
  else if (!strcasecmp(str, "LASERTAG"))
    return decode_type_t::LASERTAG;
  else if (!strcasecmp(str, "LUTRON"))
    return decode_type_t::LUTRON;
  else if (!strcasecmp(str, "MAGIQUEST"))
    return decode_type_t::MAGIQUEST;
  else if (!strcasecmp(str, "MIDEA"))
    return decode_type_t::MIDEA;
  else if (!strcasecmp(str, "MITSUBISHI"))
    return decode_type_t::MITSUBISHI;
  else if (!strcasecmp(str, "MITSUBISHI2"))
    return decode_type_t::MITSUBISHI2;
  else if (!strcasecmp(str, "MITSUBISHI_AC"))
    return decode_type_t::MITSUBISHI_AC;
  else if (!strcasecmp(str, "MITSUBISHI136"))
    return decode_type_t::MITSUBISHI136;
  else if (!strcasecmp(str, "MITSUBISHI112"))
    return decode_type_t::MITSUBISHI112;
  else if (!strcasecmp(str, "MITSUBISHI_HEAVY_88"))
    return decode_type_t::MITSUBISHI_HEAVY_88;
  else if (!strcasecmp(str, "MITSUBISHI_HEAVY_152"))
    return decode_type_t::MITSUBISHI_HEAVY_152;
  else if (!strcasecmp(str, "MWM"))
    return decode_type_t::MWM;
  else if (!strcasecmp(str, "NEOCLIMA"))
    return decode_type_t::NEOCLIMA;
  else if (!strcasecmp(str, "NEC"))
    return decode_type_t::NEC;
  else if (!strcasecmp(str, "NEC_LIKE") ||
           !strcasecmp(str, "NEC (NON-STRICT)"))
    return decode_type_t::NEC_LIKE;
  else if (!strcasecmp(str, "NIKAI"))
    return decode_type_t::NIKAI;
  else if (!strcasecmp(str, "PANASONIC"))
    return decode_type_t::PANASONIC;
  else if (!strcasecmp(str, "PANASONIC_AC"))
    return decode_type_t::PANASONIC_AC;
  else if (!strcasecmp(str, "PIONEER"))
    return decode_type_t::PIONEER;
  else if (!strcasecmp(str, "PRONTO"))
    return decode_type_t::PRONTO;
  else if (!strcasecmp(str, "RAW"))
    return decode_type_t::RAW;
  else if (!strcasecmp(str, "RC5"))
    return decode_type_t::RC5;
  else if (!strcasecmp(str, "RC5X"))
    return decode_type_t::RC5X;
  else if (!strcasecmp(str, "RC6"))
    return decode_type_t::RC6;
  else if (!strcasecmp(str, "RCMM"))
    return decode_type_t::RCMM;
  else if (!strcasecmp(str, "SAMSUNG"))
    return decode_type_t::SAMSUNG;
  else if (!strcasecmp(str, "SAMSUNG36"))
    return decode_type_t::SAMSUNG36;
  else if (!strcasecmp(str, "SAMSUNG_AC"))
    return decode_type_t::SAMSUNG_AC;
  else if (!strcasecmp(str, "SANYO"))
    return decode_type_t::SANYO;
  else if (!strcasecmp(str, "SANYO_LC7461"))
    return decode_type_t::SANYO_LC7461;
  else if (!strcasecmp(str, "SHARP"))
    return decode_type_t::SHARP;
  else if (!strcasecmp(str, "SHARP_AC"))
    return decode_type_t::SHARP_AC;
  else if (!strcasecmp(str, "SHERWOOD"))
    return decode_type_t::SHERWOOD;
  else if (!strcasecmp(str, "SONY"))
    return decode_type_t::SONY;
  else if (!strcasecmp(str, "TCL112AC"))
    return decode_type_t::TCL112AC;
  else if (!strcasecmp(str, "TECO"))
    return decode_type_t::TECO;
  else if (!strcasecmp(str, "TOSHIBA_AC"))
    return decode_type_t::TOSHIBA_AC;
  else if (!strcasecmp(str, "TROTEC"))
    return decode_type_t::TROTEC;
  else if (!strcasecmp(str, "VESTEL_AC"))
    return decode_type_t::VESTEL_AC;
  else if (!strcasecmp(str, "WHIRLPOOL_AC"))
    return decode_type_t::WHIRLPOOL_AC;
  else if (!strcasecmp(str, "WHYNTER"))
    return decode_type_t::WHYNTER;
  // Handle integer values of the type by converting to a string and back again.
  decode_type_t result = strToDecodeType(
      typeToString((decode_type_t)atoi(str)).c_str());
  if (result > 0)
    return result;
  else
    return decode_type_t::UNKNOWN;
}

// Convert a protocol type (enum etc) to a human readable string.
// Args:
//   protocol: Nr. (enum) of the protocol.
//   isRepeat: A flag indicating if it is a repeat message of the protocol.
// Returns:
//   A string containing the protocol name.
String typeToString(const decode_type_t protocol, const bool isRepeat) {
  String result = "";
  switch (protocol) {
    case UNUSED:
      result = F("UNUSED");
      break;
    case AIWA_RC_T501:
      result = F("AIWA_RC_T501");
      break;
    case AMCOR:
      result = F("AMCOR");
      break;
    case ARGO:
      result = F("ARGO");
      break;
    case CARRIER_AC:
      result = F("CARRIER_AC");
      break;
    case COOLIX:
      result = F("COOLIX");
      break;
    case DAIKIN:
      result = F("DAIKIN");
      break;
    case DAIKIN128:
      result = F("DAIKIN128");
      break;
    case DAIKIN152:
      result = F("DAIKIN152");
      break;
    case DAIKIN160:
      result = F("DAIKIN160");
      break;
    case DAIKIN176:
      result = F("DAIKIN176");
      break;
    case DAIKIN2:
      result = F("DAIKIN2");
      break;
    case DAIKIN216:
      result = F("DAIKIN216");
      break;
    case DENON:
      result = F("DENON");
      break;
    case DISH:
      result = F("DISH");
      break;
    case ELECTRA_AC:
      result = F("ELECTRA_AC");
      break;
    case FUJITSU_AC:
      result = F("FUJITSU_AC");
      break;
    case GICABLE:
      result = F("GICABLE");
      break;
    case GLOBALCACHE:
      result = F("GLOBALCACHE");
      break;
    case GOODWEATHER:
      result = F("GOODWEATHER");
      break;
    case GREE:
      result = F("GREE");
      break;
    case HAIER_AC:
      result = F("HAIER_AC");
      break;
    case HAIER_AC_YRW02:
      result = F("HAIER_AC_YRW02");
      break;
    case HITACHI_AC:
      result = F("HITACHI_AC");
      break;
    case HITACHI_AC1:
      result = F("HITACHI_AC1");
      break;
    case HITACHI_AC2:
      result = F("HITACHI_AC2");
      break;
    case HITACHI_AC424:
      result = F("HITACHI_AC424");
      break;
    case INAX:
      result = F("INAX");
      break;
    case JVC:
      result = F("JVC");
      break;
    case KELVINATOR:
      result = F("KELVINATOR");
      break;
    case LEGOPF:
      result = F("LEGOPF");
      break;
    case LG:
      result = F("LG");
      break;
    case LG2:
      result = F("LG2");
      break;
    case LASERTAG:
      result = F("LASERTAG");
      break;
    case LUTRON:
      result = F("LUTRON");
      break;
    case MAGIQUEST:
      result = F("MAGIQUEST");
      break;
    case MIDEA:
      result = F("MIDEA");
      break;
    case MITSUBISHI:
      result = F("MITSUBISHI");
      break;
    case MITSUBISHI2:
      result = F("MITSUBISHI2");
      break;
    case MITSUBISHI_AC:
      result = F("MITSUBISHI_AC");
      break;
    case MITSUBISHI136:
      result = F("MITSUBISHI136");
      break;
    case MITSUBISHI112:
      result = F("MITSUBISHI112");
      break;
    case MITSUBISHI_HEAVY_88:
      result = F("MITSUBISHI_HEAVY_88");
      break;
    case MITSUBISHI_HEAVY_152:
      result = F("MITSUBISHI_HEAVY_152");
      break;
    case MWM:
      result = F("MWM");
      break;
    case NEOCLIMA:
      result = F("NEOCLIMA");
      break;
    case NEC:
      result = F("NEC");
      break;
    case NEC_LIKE:
      result = F("NEC (non-strict)");
      break;
    case NIKAI:
      result = F("NIKAI");
      break;
    case PANASONIC:
      result = F("PANASONIC");
      break;
    case PANASONIC_AC:
      result = F("PANASONIC_AC");
      break;
    case PIONEER:
      result = F("PIONEER");
      break;
    case PRONTO:
      result = F("PRONTO");
      break;
    case RAW:
      result = F("RAW");
      break;
    case RC5:
      result = F("RC5");
      break;
    case RC5X:
      result = F("RC5X");
      break;
    case RC6:
      result = F("RC6");
      break;
    case RCMM:
      result = F("RCMM");
      break;
    case SAMSUNG:
      result = F("SAMSUNG");
      break;
    case SAMSUNG36:
      result = F("SAMSUNG36");
      break;
    case SAMSUNG_AC:
      result = F("SAMSUNG_AC");
      break;
    case SANYO:
      result = F("SANYO");
      break;
    case SANYO_LC7461:
      result = F("SANYO_LC7461");
      break;
    case SHARP:
      result = F("SHARP");
      break;
    case SHARP_AC:
      result = F("SHARP_AC");
      break;
    case SHERWOOD:
      result = F("SHERWOOD");
      break;
    case SONY:
      result = F("SONY");
      break;
    case TCL112AC:
      result = F("TCL112AC");
      break;
    case TECO:
      result = F("TECO");
      break;
    case TOSHIBA_AC:
      result = F("TOSHIBA_AC");
      break;
    case TROTEC:
      result = F("TROTEC");
      break;
    case VESTEL_AC:
      result = F("VESTEL_AC");
      break;
    case WHIRLPOOL_AC:
      result = F("WHIRLPOOL_AC");
      break;
    case WHYNTER:
      result = F("WHYNTER");
      break;
    case UNKNOWN:
    default:
      result = F("Unknown");
      break;
  }
  if (isRepeat) {
    result += ')';
  }
  return result;
}

// Does the given protocol use a complex state as part of the decode?
bool hasACState(const decode_type_t protocol) {
  switch (protocol) {
    case AMCOR:
    case ARGO:
    case DAIKIN:
    case DAIKIN128:
    case DAIKIN152:
    case DAIKIN160:
    case DAIKIN176:
    case DAIKIN2:
    case DAIKIN216:
    case ELECTRA_AC:
    case FUJITSU_AC:
    case GREE:
    case HAIER_AC:
    case HAIER_AC_YRW02:
    case HITACHI_AC:
    case HITACHI_AC1:
    case HITACHI_AC2:
    case HITACHI_AC424:
    case KELVINATOR:
    case MITSUBISHI136:
    case MITSUBISHI112:
    case MITSUBISHI_AC:
    case MITSUBISHI_HEAVY_88:
    case MITSUBISHI_HEAVY_152:
    case MWM:
    case NEOCLIMA:
    case PANASONIC_AC:
    case SAMSUNG_AC:
    case SHARP_AC:
    case TCL112AC:
    case TOSHIBA_AC:
    case TROTEC:
    case WHIRLPOOL_AC:
      return true;
    default:
      return false;
  }
}

// Return the corrected length of a 'raw' format array structure
// after over-large values are converted into multiple entries.
// Args:
//   results: A ptr to a decode result.
// Returns:
//   A uint16_t containing the length.
uint16_t getCorrectedRawLength(const decode_results * const results) {
  uint16_t extended_length = results->rawlen - 1;
  for (uint16_t i = 0; i < results->rawlen - 1; i++) {
    uint32_t usecs = results->rawbuf[i] * kRawTick;
    // Add two extra entries for multiple larger than UINT16_MAX it is.
    extended_length += (usecs / (UINT16_MAX + 1)) * 2;
  }
  return extended_length;
}


// Convert the decode_results structure's value/state to simple hexadecimal.
//
String resultToHexidecimal(const decode_results * const result) {
  String output = F("0x");
  // Reserve some space for the string to reduce heap fragmentation.
  output.reserve(2 * kStateSizeMax + 2);  // Should cover worst cases.
  if (hasACState(result->decode_type)) {
#if DECODE_AC
    for (uint16_t i = 0; result->bits > i * 8; i++) {
      if (result->state[i] < 0x10) output += '0';  // Zero pad
      output += uint64ToString(result->state[i], 16);
    }
#endif  // DECODE_AC
  } else {
    output += uint64ToString(result->value, 16);
  }
  return output;
}

namespace irutils {
 
  uint8_t bcdToUint8(const uint8_t bcd) {
    if (bcd > 0x99) return 255;  // Too big.
    return (bcd >> 4) * 10 + (bcd & 0xF);
  }

  uint8_t uint8ToBcd(const uint8_t integer) {
    if (integer > 99) return 255;  // Too big.
    return ((integer / 10) << 4) + (integer % 10);
  }

  // Return the value of `position`th bit of `data`.
  // Args:
  //   data: Value to be examined.
  //   position: Nr. of the nth bit to be examined. `0` is the LSB.
  //   size: Nr. of bits in data.
  bool getBit(const uint64_t data, const uint8_t position, const uint8_t size) {
    if (position >= size) return false;  // Outside of range.
    return data & (1ULL << position);
  }

  // Return the value of `position`th bit of `data`.
  // Args:
  //   data: Value to be examined.
  //   position: Nr. of the nth bit to be examined. `0` is the LSB.
  bool getBit(const uint8_t data, const uint8_t position) {
    if (position >= 8) return false;  // Outside of range.
    return data & (1 << position);
  }

  // Return the value of `data` with the `position`th bit changed to `on`
  // Args:
  //   data: Value to be changed.
  //   position: Nr. of the bit to be changed. `0` is the LSB.
  //   on: Value to set the position'th bit to.
  //   size: Nr. of bits in data.
  uint64_t setBit(const uint64_t data, const uint8_t position, const bool on,
                  const uint8_t size) {
    if (position >= size) return data;  // Outside of range.
    uint64_t mask = 1ULL << position;
    if (on)
      return data | mask;
    else
      return data & ~mask;
  }

  // Return the value of `data` with the `position`th bit changed to `on`
  // Args:
  //   data: Value to be changed.
  //   position: Nr. of the bit to be changed. `0` is the LSB.
  //   on: Value to set the position'th bit to.
  uint8_t setBit(const uint8_t data, const uint8_t position, const bool on) {
    if (position >= 8) return data;  // Outside of range.
    uint8_t mask = 1 << position;
    if (on)
      return data | mask;
    else
      return data & ~mask;
  }

  // Change the value at the location `data_ptr` with the `position`th bit
  //   changed to `on`
  // Args:
  //   data: Ptr to the data to be changed.
  //   position: Nr. of the bit to be changed. `0` is the LSB.
  //   on: Value to set the position'th bit to.
  void setBit(uint8_t * const data, const uint8_t position, const bool on) {
    uint8_t mask = 1 << position;
    if (on)
      *data |= mask;
    else
      *data &= ~mask;
  }

  // Change the value at the location `data_ptr` with the `position`th bit
  //   changed to `on`
  // Args:
  //   data: Ptr to the data to be changed.
  //   position: Nr. of the bit to be changed. `0` is the LSB.
  //   on: Value to set the position'th bit to.
  void setBit(uint32_t * const data, const uint8_t position, const bool on) {
    uint32_t mask = (uint32_t)1 << position;
    if (on)
      *data |= mask;
    else
      *data &= ~mask;
  }

  // Change the value at the location `data_ptr` with the `position`th bit
  //   changed to `on`
  // Args:
  //   data: Ptr to the data to be changed.
  //   position: Nr. of the bit to be changed. `0` is the LSB.
  //   on: Value to set the position'th bit to.
  void setBit(uint64_t * const data, const uint8_t position, const bool on) {
    uint64_t mask = (uint64_t)1 << position;
    if (on)
      *data |= mask;
    else
      *data &= ~mask;
  }

  // Change the uint8_t pointed to by `dst` starting at the `offset`th bit
  //   and for `nbits` bits, with the contents of `data`.
  // Args:
  //   dst: Ptr to the uint8_t to be changed.
  //   offset: Nr. of bits from the Least Significant Bit to be ignored.
  //   nbits: Nr of bits of `data` to be placed into the destination uint8_t.
  //   data: Value to be placed into dst.
  void setBits(uint8_t * const dst, const uint8_t offset, const uint8_t nbits,
               const uint8_t data) {
    if (offset >= 8 || !nbits) return;  // Short circuit as it won't change.
    // Calculate the mask for the supplied value.
    uint8_t mask = UINT8_MAX >> (8 - ((nbits > 8) ? 8 : nbits));
    // Calculate the mask & clear the space for the data.
    // Clear the destination bits.
    *dst &= ~(uint8_t)(mask << offset);
    // Merge in the data.
    *dst |= ((data & mask) << offset);
  }

  // Change the uint32_t pointed to by `dst` starting at the `offset`th bit
  //   and for `nbits` bits, with the contents of `data`.
  // Args:
  //   dst: Ptr to the uint32_t to be changed.
  //   offset: Nr. of bits from the Least Significant Bit to be ignored.
  //   nbits: Nr of bits of `data` to be placed into the destination uint32_t.
  //   data: Value to be placed into dst.
  void setBits(uint32_t * const dst, const uint8_t offset, const uint8_t nbits,
               const uint32_t data) {
    if (offset >= 32 || !nbits) return;  // Short circuit as it won't change.
    // Calculate the mask for the supplied value.
    uint32_t mask = UINT32_MAX >> (32 - ((nbits > 32) ? 32 : nbits));
    // Calculate the mask & clear the space for the data.
    // Clear the destination bits.
    *dst &= ~(mask << offset);
    // Merge in the data.
    *dst |= ((data & mask) << offset);
  }

  // Change the uint64_t pointed to by `dst` starting at the `offset`th bit
  //   and for `nbits` bits, with the contents of `data`.
  // Args:
  //   dst: Ptr to the uint64_t to be changed.
  //   offset: Nr. of bits from the Least Significant Bit to be ignored.
  //   nbits: Nr of bits of `data` to be placed into the destination uint64_t.
  //   data: Value to be placed into dst.
  void setBits(uint64_t * const dst, const uint8_t offset, const uint8_t nbits,
               const uint64_t data) {
    if (offset >= 64 || !nbits) return;  // Short circuit as it won't change.
    // Calculate the mask for the supplied value.
    uint64_t mask = UINT64_MAX >> (64 - ((nbits > 64) ? 64 : nbits));
    // Calculate the mask & clear the space for the data.
    // Clear the destination bits.
    *dst &= ~(mask << offset);
    // Merge in the data.
    *dst |= ((data & mask) << offset);
  }
}  // namespace irutils
