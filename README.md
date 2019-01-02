# Description

This is a convenience library for handling ISO7816-4 application protocol data
units (APDUs), a protocol used to communicate with devices like smartcards and,
incidentally, the Ledger Nano S and Ledger Blue hardware wallets. APDUs are
used for sending commands to the wallet, for example, generating a new public
key, or signing a transaction.

The format of a command APDU is:

    +-----+-----+----+----+----+-------------+
    | CLA | INS | P1 | P2 | Lc | ... data... |
    +-----+-----+----+----+----+-------------+

Here, the CLA byte represents the CLASS of the command, the INS byte the
INSTRUCTION, P1 and P2 the PARAMETERS to the command, and Lc the LENGTH of
the command data.

Generally, Lc may have a variable length, however, the Ledger Bitcoin app
assumes it to have one byte, therefore the APDUs are limited to 260 bytes
in length (5 header and 255 data bytes). 

# Building

Run `make` in the project root. This will create a `build` directory with the
statically compiled library. Optionally, run `make test` for the unit tests.

# Additional information

A description of the APDUs implemented by the Ledger Bitcoin app can be found
[in the technical specification][1].

Generic information about APDUs in the Ledger SDK is available [here][2].

[1]: https://ledgerhq.github.io/btchip-doc/bitcoin-technical-beta.html

[2]: https://ledger.readthedocs.io/en/latest/userspace/application_structure.html
