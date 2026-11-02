# recursive-descent function finder for small-model MZ exes
import re,sys,struct,capstone,hashlib,json
from capstone import x86
md=capstone.Cs(capstone.CS_ARCH_X86,capstone.CS_MODE_16); md.detail=True
def load(f):
    d=open(f,'rb').read(); h=struct.unpack('<14H',d[:28]); img=d[h[4]*16:]
    return d,h,img
def analyse(f):
    d,h,img=load(f)
    e=next(md.disasm(img[0:3],0)); ds=int(e.op_str.split(',')[1],16)
    codeend=ds*16
    funcs={0:None}; work=[0]; seen=set()
    for m in re.finditer(rb'\x55(\x83\xec.\x8b\xec|\x8b\xec)',img[:codeend],re.S):
        funcs[m.start()]=None; work.append(m.start())
    def walk(start):
        insns={}; todo=[start]
        while todo:
            a=todo.pop()
            while a<codeend and a not in insns:
                try: ins=next(md.disasm(img[a:a+16],a))
                except StopIteration: break
                insns[a]=ins
                m=ins.mnemonic
                if m=='call' and ins.op_str.startswith('0x'):
                    t=int(ins.op_str,16)&0xffff
                    if t<codeend and t not in funcs: funcs[t]=None; work.append(t)
                if m.startswith('j') and ins.op_str.startswith('0x'):
                    t=int(ins.op_str,16)&0xffff
                    if m=='jmp':
                        a=t; continue
                    todo.append(t)
                if m in('ret','retf','iret','jmp') or (m=='int' and ins.op_str=='0x20'): break
                if m=='mov' and ins.op_str=='ax, 0x4c00': pass
                a+=ins.size
        return insns
    allins={}
    while work:
        s=work.pop()
        ins=walk(s); funcs[s]=ins; allins.update(ins)
    return img,codeend,ds,funcs,allins
if __name__=='__main__':
    out={}
    for f in sys.argv[1:]:
        img,ce,ds,funcs,allins=analyse(f)
        cov=sum(i.size for i in allins.values())
        name=f.split('/')[-1]
        res=[]
        for s in sorted(funcs):
            ins=funcs[s]
            if not ins: continue
            end=max(a+i.size for a,i in ins.items())
            fr=img[s:s+3] in (b'\x55\x83\xec',b'\x55\x8b\xec')
            # masked hash: opcode bytes with immediates/displacements for mem/imm operands zeroed when 16-bit abs
            hb=b''
            for a in sorted(ins):
                i=ins[a]; b=bytearray(i.bytes)
                if i.mnemonic in('call','jmp') or i.mnemonic.startswith('j'):
                    b=bytearray(b[:1])
                else:
                    for op in i.operands:
                        if op.type==x86.X86_OP_MEM and op.mem.base==0 and op.mem.index==0:
                            b=bytearray(b[:2])
                        if op.type==x86.X86_OP_IMM and op.size==2 and op.imm>0x40:
                            b=bytearray(b[:2])
                hb+=bytes(b)
            res.append(dict(start=s,end=end,n=len(ins),frame=fr,h=hashlib.md5(hb).hexdigest()[:12]))
        out[name]=dict(codeend=ce,ds=ds,cover=cov,funcs=res)
        print(name,'code',hex(ce),'covered',hex(cov),'%.0f%%'%(100*cov/ce),'funcs',len(res),'frame',sum(r['frame'] for r in res))
    json.dump(out,open('funcs.json','w'),indent=0)
