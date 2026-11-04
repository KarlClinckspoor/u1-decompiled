#!/usr/bin/env python3
"""Post-link step for ULTIMA.EXE: put back the remains of the protection envelope.

    python3 postlink.py BUILT.EXE ORIGINAL.EXE

The program LINK makes is the original's load image up to its end (image 88E4). The envelope
added after it: filler, at 88F0 a stub that restores SS:SP and jumps to the startup (the header's
entry point, 088F:0000, with its own stack 09E1:0064), and 0x500 bytes that look encrypted. They
are not built from source: this copies them from the original at build time (the repository
holds none of them) and sets the header fields the envelope changed: file size, SS:SP and CS:IP.
The checksum is LINK's own and the envelope left it alone.
"""
import struct, sys


def main():
    built, orig = sys.argv[1], sys.argv[2]
    b = bytearray(open(built, 'rb').read())
    o = open(orig, 'rb').read()
    hb, ho = struct.unpack('<14H', b[:28]), struct.unpack('<14H', o[:28])
    ib, io = b[hb[4] * 16:], o[ho[4] * 16:]
    if ib != io[:len(ib)]:
        raise SystemExit('postlink: the linked image differs from the original; not adding the envelope')
    b += io[len(ib):]                         # filler, stub and encrypted tail
    n = len(b)
    struct.pack_into('<HH', b, 2, n % 512, (n + 511) // 512)
    struct.pack_into('<HH', b, 0x0E, ho[7], ho[8])     # SS, SP
    struct.pack_into('<HH', b, 0x14, ho[10], ho[11])   # IP, CS
    open(built, 'wb').write(b)
    print('postlink: envelope remains appended (%d bytes), entry %04X:%04X' % (len(io) - len(ib), ho[11], ho[10]))


if __name__ == '__main__':
    main()
