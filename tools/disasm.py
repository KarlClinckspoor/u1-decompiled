#!/usr/bin/env python3
"""Disassemble the code segment of an EXE: tools/dis.py EXE OFFSET [COUNT]"""
import sys,capstone,struct
f=sys.argv[1]; off=int(sys.argv[2],16); n=int(sys.argv[3]) if len(sys.argv)>3 else 60
d=open(f,'rb').read(); hdr=struct.unpack('<H',d[8:10])[0]*16
md=capstone.Cs(capstone.CS_ARCH_X86,capstone.CS_MODE_16)
code=d[hdr+off:hdr+off+0x800]
for i,ins in enumerate(md.disasm(code,off)):
    if i>=n: break
    print('%05x: %-20s %s %s'%(ins.address,ins.bytes.hex(),ins.mnemonic,ins.op_str))
