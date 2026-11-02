# libdir.py LIB [NAME..]: list library modules with their publics/externs and segment sizes
import sys,struct
def idx(b,i):
    v=b[i]
    if v&0x80: return ((v&0x7f)<<8)|b[i+1], i+2
    return v,i+1
def walk(path):
    d=open(path,'rb').read(); pg=struct.unpack('<H',d[1:3])[0]+3; i=pg; out=[]
    while i<len(d) and d[i]!=0xF1:
        if d[i]!=0x80: i=(i//pg+1)*pg; continue
        j=i; m=dict(name=None,pubs=[],ext=[],segs=[],off=i); lnames=['']
        while True:
            t=d[j]; L=struct.unpack('<H',d[j+1:j+3])[0]; r=d[j+3:j+2+L]
            if t==0x80: m['name']=r[1:1+r[0]].decode('latin1')
            elif t==0x96:
                k=0
                while k<len(r): n=r[k]; lnames.append(r[k+1:k+1+n].decode('latin1')); k+=1+n
            elif t==0x98:
                ln=struct.unpack('<H',r[1:3])[0]; ni,q=idx(r,3); ci,q=idx(r,q); m['segs'].append((lnames[ni],ln))
            elif t==0x90:
                k=0; gi,k=idx(r,k); si,k=idx(r,k)
                if si==0: k+=2
                while k<len(r):
                    n=r[k]; m['pubs'].append(r[k+1:k+1+n].decode('latin1')); k+=3+n; _,k=idx(r,k)
            elif t==0x8c:
                k=0
                while k<len(r):
                    n=r[k]; m['ext'].append(r[k+1:k+1+n].decode('latin1')); k+=1+n; _,k=idx(r,k)
            j+=3+L
            if t in (0x8a,0x8b): break
        out.append(m); i=j if j%pg==0 else (j//pg+1)*pg
    return out
if __name__=='__main__':
    want=set(sys.argv[2:])
    for m in walk(sys.argv[1]):
        if want and not (want & ({m['name']}|set(m['pubs']))): continue
        print(m['name'],m['segs'],'PUB',m['pubs'],'EXT',m['ext'])
