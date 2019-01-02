// Copyright (c) 2018 The Unit-e developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <cstdarg>

#include <stdexcept>
#include <type_traits>
#include <vector>

#include "apdu.h"

int ApduInit(APDU *apdu, uint8_t cla, uint8_t ins, uint8_t p1, uint8_t p2,
             uint8_t *buffer, size_t buffer_size) {
  if (buffer_size > IO_APDU_BUFFER_SIZE) {
    buffer_size = IO_APDU_BUFFER_SIZE;
  }
  if (buffer_size < 5) {
    return APDU_ERR_SIZE_MISMATCH;
  }

  apdu->size = 5;
  apdu->max_size = buffer_size;
  apdu->buffer = buffer;
  buffer[0] = cla;
  buffer[1] = ins;
  buffer[2] = p1;
  buffer[3] = p2;
  buffer[4] = 0;

  return APDU_OK;
}

template <typename T>
static void FillLE(APDU *apdu, T n) {
  static_assert(std::is_integral<T>::value, "T must be integral");

  if (apdu->size + sizeof(n) > apdu->max_size) {
    throw std::logic_error("APDU size would exceed limit");
  }
  for (size_t i = 0; i < sizeof(n); i++) {
    apdu->buffer[apdu->size++] = (n >> (8 * i)) & 0xff;
  }
}

template <typename T>
static void FillBE(APDU *apdu, T n) {
  static_assert(std::is_integral<T>::value, "T must be integral");

  if (apdu->size + sizeof(n) > apdu->max_size) {
    throw std::logic_error("APDU size would exceed limit");
  }
  for (size_t i = 0; i < sizeof(n); i++) {
    apdu->buffer[apdu->size++] = (n >> (8 * (sizeof(n) - 1 - i))) & 0xff;
  }
}

static void FillRaw(APDU *apdu, size_t n, const uint8_t *bytes) {
  if (apdu->size + n > apdu->max_size) {
    throw std::logic_error("APDU size would exceed limit");
  }

  if (!bytes) {
    throw std::logic_error("Invalid argument");
  }

  std::copy(bytes, bytes + n, apdu->buffer + apdu->size);
  apdu->size += n;
}

static void FillHex(APDU *apdu, const char *s, bool reverse) {
  std::vector<uint8_t> temp;

  if (!s) {
    throw std::logic_error("Invalid argument");
  }

  for (; s[0] != '\0' && s[1] != '\0'; s += 2) {
    int b;
    sscanf(s, "%02x", &b);
    temp.emplace_back(static_cast<uint8_t>(b));
  }
  if (apdu->size + temp.size() > apdu->max_size) {
    throw std::logic_error("APDU size would exceed limit");
  }

  if (reverse) {
    std::copy(temp.rbegin(), temp.rend(), apdu->buffer + apdu->size);
  } else {
    std::copy(temp.begin(), temp.end(), apdu->buffer + apdu->size);
  }
  apdu->size += temp.size();
}

int ApduWrite(APDU *apdu, const char *fmt, ...) {
  size_t old_size = apdu->size;
  int result;

  std::va_list args;
  va_start(args, fmt);

  try {
    while (*fmt != 0) {
      switch (*fmt) {
        case 'b':
        case 'B':
          // va_arg does not support 8-bit and 16-bit types
          FillLE(apdu, static_cast<uint8_t>(va_arg(args, int)));
          break;
        case 'w':
          FillLE(apdu, static_cast<uint16_t>(va_arg(args, int)));
          break;
        case 'W':
          FillBE(apdu, static_cast<uint16_t>(va_arg(args, int)));
          break;
        case 'd':
          FillLE(apdu, va_arg(args, uint32_t));
          break;
        case 'D':
          FillBE(apdu, va_arg(args, uint32_t));
          break;
        case 'q':
          FillLE(apdu, va_arg(args, uint64_t));
          break;
        case 'Q':
          FillBE(apdu, va_arg(args, uint64_t));
          break;
        case 'r':
        case 'R': {
          size_t n = va_arg(args, size_t);
          const uint8_t *bytes = va_arg(args, uint8_t *);
          FillRaw(apdu, n, bytes);
        } break;
        case 'x':
        case 'X': {
          const char *s = va_arg(args, char *);
          FillHex(apdu, s, (*fmt == 'x'));
        } break;
        default:
          result = APDU_ERR_INVALID_FORMAT;
          goto cleanup;
      }
      ++fmt;
    }

    // Update APDU size
    apdu->buffer[4] = (uint8_t)(apdu->size - 5);

    // Return bytes written
    result = apdu->size - old_size;
  } catch (const std::logic_error &ex) {
    result = APDU_ERR_SIZE_MISMATCH;
  }

cleanup:
  va_end(args);
  return result;
}

void BufferDump(const uint8_t *buffer, size_t size) {
  for (size_t i = 0; i < size; i++) {
    printf("%02x ", buffer[i]);
  }
  printf("\n");
}

void ApduDump(const APDU *apdu) { BufferDump(apdu->buffer, apdu->size); }
