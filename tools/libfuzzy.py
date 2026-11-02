# libfuzzy.py LIB EXE : for every library module, best-aligned placement in the EXE code with similarity
import sys,struct,re
sys.path.insert(0,__file__.rsplit('/',1)[0]); import libmatch
lib,exe=sys.argv[1],sys.argv[2]; minr=float(sys.argv[3]) if len(sys.argv)>3 else 0.5
d=open(exe,'rb').read(); h=struct.unpack('<14H',d[:28]); img=d[h[4]*16:]
ds=struct.unpack('<H',img[1:3])[0]*16
code=img[:ds]
res=[]
for name,c,m in libmatch.modules(lib):
    if len(c)<12: continue
    # anchor: try every 6-byte unmasked window among first 24 bytes
    best=(0,None)
    cands=set()
    for k in range(0,min(len(c)-6,40)):
        if any(m[k:k+6]): continue
        for mm in re.finditer(re.escape(c[k:k+6]),code): cands.add(mm.start()-k)
    for p in cands:
        if p<0 or p+len(c)>len(code): continue
        s=sum(1 for i in range(len(c)) if m[i] or code[p+i]==c[i])/len(c)
        if s>best[0]: best=(s,p)
    if best[0]>=minr: res.append((best[1],name,len(c),best[0]))
for p,n,l,s in sorted(res): print('%04x-%04x %-10s %4d %.2f'%(p,p+l,n,l,s))
