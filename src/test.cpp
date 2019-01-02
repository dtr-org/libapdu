#include <cassert>
#include <cstdint>
#include <cstring>

#include "apdu.h"

int main() {
  APDU apdu;
  uint8_t buffer[260];
  int result;

  result = ApduInit(&apdu, 0, 0, 0, 0, buffer, 3);
  assert(result == APDU_ERR_SIZE_MISMATCH);

  result = ApduInit(&apdu, 0, 0, 0, 0, buffer, 6);
  assert(result == APDU_OK);

  result = ApduWrite(&apdu, "");
  assert(result == 0);

  result = ApduWrite(&apdu, "%", 1);
  assert(result == APDU_ERR_INVALID_FORMAT);

  result = ApduWrite(&apdu, "b", 1);
  assert(result == 1);

  result = ApduWrite(&apdu, "b", 2);
  assert(result == APDU_ERR_SIZE_MISMATCH);

  result = ApduInit(&apdu, 1, 2, 3, 4, buffer, 200);
  assert(result == APDU_OK);

  // Test little- and big-endian serialization
  uint32_t value = 0x0a0b0c0d;
  uint8_t big_endian[]{0x0a, 0x0b, 0x0c, 0x0d};
  ApduWrite(&apdu, "D", value);
  assert(memcmp(apdu.buffer + 5, big_endian, 4) == 0);

  ApduInit(&apdu, 1, 2, 3, 4, buffer, 200);
  uint8_t little_endian[]{0x0d, 0x0c, 0x0b, 0x0a};
  ApduWrite(&apdu, "d", value);
  assert(memcmp(apdu.buffer + 5, little_endian, 4) == 0);

  // Test length calculation, hex and raw binary serialization
  ApduInit(&apdu, 1, 2, 3, 4, buffer, 200);
  uint8_t bytes[]{0x01, 0x02, 0x03};
  result = ApduWrite(&apdu, "DXr", 0xcafebabe, "deadbeef", (size_t)2, bytes);
  assert(result == 10);
  assert(apdu.size == 15);

  uint8_t expected[] = {0x01, 0x02, 0x03, 0x04, 0x0a, 0xca, 0xfe, 0xba,
                        0xbe, 0xde, 0xad, 0xbe, 0xef, 0x01, 0x02};
  assert(memcmp(apdu.buffer, expected, apdu.size) == 0);

  return 0;
}
