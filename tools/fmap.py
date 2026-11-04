# fmap.py EXE [LIB]: function map: start end frame lib-name calls datarefs
import sys,struct,re,json
sys.path.insert(0,__file__.rsplit('/',1)[0])
from rdis import analyse
from capstone import x86
import libmatch
exe=sys.argv[1]
img,ce,ds,funcs,allins=analyse(exe)
names={}
if len(sys.argv)>2:
    for name,c,m in libmatch.modules(sys.argv[2]):
        if len(c)<8: continue
        for p in libmatch.find(img[:ce],c,m): names.setdefault(p,[]).append(name)
starts=sorted(s for s in funcs if funcs[s])
dsize=len(img)-ce
out=[]
for k,s in enumerate(starts):
    ins=funcs[s]; nxt=starts[k+1] if k+1<len(starts) else ce
    end=max(a+i.size for a,i in ins.items())
    calls=sorted({int(i.op_str,16)&0xffff for i in ins.values() if i.mnemonic=='call' and i.op_str.startswith('0x')})
    refs=set()
    for i in ins.values():
        for op in i.operands:
            if op.type==x86.X86_OP_MEM and op.mem.base==0 and op.mem.index==0 and op.mem.segment in (0,x86.X86_REG_DS): refs.add(op.mem.disp&0xffff)
            if op.type==x86.X86_OP_MEM and op.mem.base!=0 and op.mem.disp>0x100: refs.add(op.mem.disp&0xffff)
            if op.type==x86.X86_OP_IMM and i.mnemonic in('mov','push') and 0x74<=op.imm<dsize+0x2000 and op.size==2: refs.add(('i',op.imm))
    out.append(dict(s=s,e=end,n=nxt,fr=img[s:s+3] in(b'\x55\x83\xec',b'\x55\x8b\xec'),lib=names.get(s,[]),calls=calls,refs=sorted(r for r in refs if isinstance(r,int)),imm=sorted(r[1] for r in refs if not isinstance(r,int))))
import os; os.makedirs('build',exist_ok=True); json.dump(out,open(os.path.join('build',exe.rsplit('/',1)[-1]+'.map.json'),'w'))
for f in out:
    print('%04x-%04x(%04x) %s %-10s calls %s | data %s | imm %s'%(f['s'],f['e'],f['n'],'F' if f['fr'] else 'a',','.join(f['lib'])[:10],' '.join('%x'%c for c in f['calls']),' '.join('%x'%r for r in f['refs']),' '.join('%x'%r for r in f['imm'])))
