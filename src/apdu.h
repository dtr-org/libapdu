// Copyright (c) 2018 The Unit-e developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

//! \file apdu.h
//! \brief Library for handling application protocol data units (APDUs)
//!
//! The Ledger Nano S and Ledger Blue wallet SDK expects the commands to be
//! wrapped in ISO7816-4-like protocol units. The format of a command APDU is:
//!
//!    +-----+-----+----+----+----+-------------+
//!    | CLA | INS | P1 | P2 | Lc | ... data... |
//!    +-----+-----+----+----+----+-------------+
//!
//! Here, the CLA byte represents the CLASS of the command, the INS byte the
//! INSTRUCTION, P1 and P2 the PARAMETERS to the command, and Lc the LENGTH of
//! the command data. Generally, Lc may have a variable length, however, the
//! Ledger bitcoin application assumes it to have one byte, therefore the APDUs
//! are limited to 260 bytes in length (5 header and 255 data bytes).

#ifndef APDU_H_INCLUDED
#define APDU_H_INCLUDED

#include <stddef.h>
#include <stdint.h>

#ifndef IO_APDU_BUFFER_SIZE
#define IO_APDU_BUFFER_SIZE (5 + 255)
#endif

#define APDU_OK 0
#define APDU_ERR_INVALID_FORMAT -1
#define APDU_ERR_SIZE_MISMATCH -2

typedef struct {
  size_t size;
  size_t max_size;
  uint8_t *buffer;
} APDU;

#ifdef __cplusplus
extern "C" {
#endif

//! \brief Initialize an APDU structure.
//!
//! Sets the initial size and backing buffer of the APDU, as well as the header
//! bytes. The buffer must be allocated by the caller.
//! \returns APDU_OK on success, or APDU_ERR_SIZE_MISMATCH if the buffer is too
//! small.
int ApduInit(APDU *apdu, uint8_t cla, uint8_t ins, uint8_t p1, uint8_t p2,
             uint8_t *buffer, size_t buffer_size);

//! \brief Write one or more values to an APDU structure.
//!
//! Converts the variadic arguments to bytes and append them to the APDU buffer
//! in order, based on the provided format string. Updates the APDU size byte.
//!
//! The varargs are written based on the characters in \a fmt as follows:
//! \li 'b', 'B' - as a single byte
//! \li 'w', 'W' - as a little-endian or big-endian word (2 bytes), respectively
//! \li 'd', 'D' - as a little-endian or big-endian dword (4 bytes)
//! \li 'q', 'Q' - as a little-endian or big-endian qword (8 bytes)
//! \li 'x', 'X' - interprets the vararg as an ASCII string of hex values, and
//!                writes them to the buffer as-is ('X') or in reverse order
//!                ('x')
//! \li 'r', 'R' - writes a raw binary sequence. Expects two arguments: the
//!                first one (N) interpreted as \c size_t , the second one as a
//!                pointer to N bytes to be written.
//!
//! As an example, the following call to ApduWrite:
//!    ApduWrite(apdu, "bdX", 5, 1024, "010203")
//! would result in apdu->buffer looking as follows:
//!    { 0x05, 0x00, 0x04, 0x00, 0x00, 0x01, 0x02, 0x03 }
//! \returns Number of bytes written on success, an error code otherwise.
int ApduWrite(APDU *apdu, const char *fmt, ...);

//! \brief Convenience function to display an APDU on stdout
void ApduDump(const APDU *apdu);

//! \brief Convenience function to display any byte buffer on stdout
void BufferDump(const uint8_t *buffer, size_t size);

#ifdef __cplusplus
}
#endif

#endif  // APDU_H_INCLUDED
