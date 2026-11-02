# libmatch.py LIB EXE : exact match (fixups masked) of each library module's code segment in the exe
import sys,struct,re
def idx(b,i):
    v=b[i]
    if v&0x80: return ((v&0x7f)<<8)|b[i+1], i+2
    return v,i+1
def fixlocs(r):
    out=[];k=0;threads={}
    while k<len(r):
        b=r[k]
        if b&0x80==0:  # thread
            k+=1; meth=(b>>2)&7
            if b&0x40==0:
                if meth<3: _,k=idx(r,k)
            else:
                if meth<4: _,k=idx(r,k)
            continue
        loc=((b&3)<<8)|r[k+1]; ltype=(b>>2)&0xf; k+=2
        fd=r[k]; k+=1
        if not fd&0x80:
            fm=(fd>>4)&7
            if fm<3: _,k=idx(r,k)
        if not fd&0x08:
            tm=fd&3
            _,k=idx(r,k)
        if not fd&0x04: k+=2
        n={0:1,1:2,2:2,3:4,4:1,5:2}.get(ltype&7 if ltype<8 else ltype,2)
        out.append((loc,n))
    return out
def modules(path):
    d=open(path,'rb').read(); pg=struct.unpack('<H',d[1:3])[0]+3; i=pg
    while i<len(d) and d[i]!=0xF1:
        if d[i]!=0x80: i=(i//pg+1)*pg; continue
        j=i; lnames=['']; segs=[]; data={}; mask={}; name=None; last=None
        while True:
            t=d[j]; L=struct.unpack('<H',d[j+1:j+3])[0]; r=d[j+3:j+2+L]
            if t==0x80: name=r[1:1+r[0]].decode('latin1')
            elif t==0x96:
                k=0
                while k<len(r): n=r[k]; lnames.append(r[k+1:k+1+n].decode('latin1')); k+=1+n
            elif t==0x98:
                ln=struct.unpack('<H',r[1:3])[0]; ni,q=idx(r,3); ci,q=idx(r,q); segs.append((lnames[ni],lnames[ci]))
                data[len(segs)]=bytearray(ln); mask[len(segs)]=bytearray(ln)
            elif t==0xa0:
                si,q=idx(r,0); off=struct.unpack('<H',r[q:q+2])[0]; b=r[q+2:]; data[si][off:off+len(b)]=b; last=(si,off)
            elif t==0x9c and last:
                for loc,n in fixlocs(r):
                    for z in range(n):
                        p=last[1]+loc+z
                        if p<len(mask[last[0]]): mask[last[0]][p]=1
            j+=3+L
            if t in (0x8a,0x8b): break
        for k,(sn,cn) in enumerate(segs,1):
            if cn in('PROG','CODE'):
                yield name,bytes(data[k]),bytes(mask[k])
        i=j if j%pg==0 else (j//pg+1)*pg
def find(img,code,mask):
    pat=b''.join(re.escape(bytes([c])) if not m else b'.' for c,m in zip(code,mask))
    return [m.start() for m in re.finditer(pat,img,re.S)]
if __name__=='__main__':
    lib=sys.argv[1]; d=open(sys.argv[2],'rb').read(); h=struct.unpack('<14H',d[:28]); img=d[h[4]*16:]
    n=f=0
    for name,code,mask in modules(lib):
        if len(code)<8: continue
        n+=1; pos=find(img,code,mask)
        if pos: f+=1; print('  %-10s %5d @%s'%(name,len(code),','.join('%04x'%p for p in pos)))
    print(lib, 'modules',n,'exact',f)
