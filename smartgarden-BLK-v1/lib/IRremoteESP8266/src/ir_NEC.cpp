// Copyright 2009 Ken Shirriff
// Copyright 2017 David Conran

// NEC originally added from https://github.com/shirriff/Arduino-IRremote/

#define __STDC_LIMIT_MACROS
#include "ir_NEC.h"
#include <stdint.h>
#include <algorithm>
#include "IRrecv.h"
#include "IRutils.h"

#if (DECODE_NEC || DECODE_SHERWOOD || DECODE_AIWA_RC_T501 || DECODE_SANYO)
// Decode the supplied NEC message.
//
// Args:
//   results: Ptr to the data to decode and where to store the decode result.
//   nbits:   The number of data bits to expect. Typically kNECBits.
//   strict:  Flag indicating if we should perform strict matching.
// Returns:
//   boolean: True if it can decode it, false if it can't.
//
// Status: STABLE / Known good.
//
// Notes:
//   NEC protocol has three varients/forms.
//     Normal:   an 8 bit address & an 8 bit command in 32 bit data form.
//               i.e. address + inverted(address) + command + inverted(command)
//     Extended: a 16 bit address & an 8 bit command in 32 bit data form.
//               i.e. address + command + inverted(command)
//     Repeat:   a 0-bit code. i.e. No data bits. Just the header + footer.
//
// Ref:
//   http://www.sbprojects.com/knowledge/ir/nec.php
bool IRrecv::decodeNEC(decode_results *results, uint16_t nbits, bool strict) {
  if (results->rawlen < 2 * nbits + kHeader + kFooter - 1 &&
      results->rawlen != kNecRptLength)
    return false;  // Can't possibly be a valid NEC message.
  if (strict && nbits != kNECBits)
    return false;  // Not strictly an NEC message.

  uint64_t data = 0;
  uint16_t offset = kStartOffset;

  // Header
  if (!matchMark(results->rawbuf[offset++], kNecHdrMark)) return false;
  // Check if it is a repeat code.
  if (results->rawlen == kNecRptLength &&
      matchSpace(results->rawbuf[offset], kNecRptSpace) &&
      matchMark(results->rawbuf[offset + 1], kNecBitMark)) {
    results->value = kRepeat;
    results->decode_type = NEC;
    results->bits = 0;
    results->address = 0;
    results->command = 0;
    results->repeat = true;
    return true;
  }

  // Match Header (cont.) + Data + Footer
  if (!matchGeneric(results->rawbuf + offset, &data,
                    results->rawlen - offset, nbits,
                    0, kNecHdrSpace,
                    kNecBitMark, kNecOneSpace,
                    kNecBitMark, kNecZeroSpace,
                    kNecBitMark, kNecMinGap, true)) return false;
  // Compliance
  // Calculate command and optionally enforce integrity checking.
  uint8_t command = (data & 0xFF00) >> 8;
  // Command is sent twice, once as plain and then inverted.
  if ((command ^ 0xFF) != (data & 0xFF)) {
    if (strict) return false;  // Command integrity failed.
    command = 0;  // The command value isn't valid, so default to zero.
  }

  // Success
  results->bits = nbits;
  results->value = data;
  results->decode_type = NEC;
  // NEC command and address are technically in LSB first order so the
  // final versions have to be reversed.
  results->command = reverseBits(command, 8);
  // Normal NEC protocol has an 8 bit address sent, followed by it inverted.
  uint8_t address = (data & 0xFF000000) >> 24;
  uint8_t address_inverted = (data & 0x00FF0000) >> 16;
  if (address == (address_inverted ^ 0xFF))
    // Inverted, so it is normal NEC protocol.
    results->address = reverseBits(address, 8);
  else  // Not inverted, so must be Extended NEC protocol, thus 16 bit address.
    results->address = reverseBits((data >> 16) & UINT16_MAX, 16);
  return true;
}
#endif  // DECODE_NEC || DECODE_SHERWOOD || DECODE_AIWA_RC_T501 || DECODE_SANYO
