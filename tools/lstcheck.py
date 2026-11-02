# lstcheck.py FILE.LST CODE0: first places where MASM's offset differs from the original address comment
import re,sys
L=open(sys.argv[1],'rb').read().decode('latin1').replace('\r','').split('\n')
c0=int(sys.argv[2],16); last=None; shown=0; seg=None
for k,l in enumerate(L):
    if 'PROG' in l and 'SEGMENT' in l: seg='PROG'
    if re.search(r'\b(DATA|UDATA)\s+SEGMENT',l): seg='D'
    m=re.match(r'\s?([0-9A-F]{4})\s+[0-9A-F]',l)
    if m and seg=='PROG': last=(int(m.group(1),16),k)
    mm=re.search(r';\s*([0-9A-F]{4})\s',l)
    if mm and last and seg=='PROG':
        o=int(mm.group(1),16)
        if o-c0!=last[0]:
            print('\n'.join(L[max(0,last[1]-6):k+1])); print('-----'); shown+=1
            if shown>=2: break
        last=None
