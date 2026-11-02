# libsim.py EXE LIB START END: best library module (by mnemonic-sequence similarity) for each function in range
import sys,json,difflib,capstone
sys.path.insert(0,__file__.rsplit('/',1)[0]); import omf
from rdis import analyse
md=capstone.Cs(capstone.CS_ARCH_X86,capstone.CS_MODE_16)
exe,lib,a0,a1=sys.argv[1],sys.argv[2],int(sys.argv[3],16),int(sys.argv[4],16)
img,ce,ds,funcs,allins=analyse(exe)
mods=[]
for m in omf.read_obj(lib):
    for k,(sn,cn,ln,at) in enumerate(m['segs'],1):
        if cn in('PROG','CODE') and ln>4:
            seq=[i.mnemonic for i in md.disasm(bytes(m['data'][k]),0)]
            mods.append((m['name'],ln,seq))
starts=sorted(s for s in funcs if funcs[s] and a0<=s<a1)
for k,s in enumerate(starts):
    e=starts[k+1] if k+1<len(starts) else a1
    seq=[i.mnemonic for i in md.disasm(img[s:e],s)]
    best=sorted(((difflib.SequenceMatcher(None,seq,ms,autojunk=False).ratio(),n,l) for n,l,ms in mods if abs(len(ms)-len(seq))<max(8,len(seq))),reverse=True)[:2]
    print('%04x-%04x %4d  %s'%(s,e,e-s,'  '.join('%s(%d) %.2f'%(n,l,r) for r,n,l in best)))
