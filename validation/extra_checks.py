import itertools, pathlib, random, re, subprocess, tempfile
ROOT=pathlib.Path(__file__).resolve().parents[1]
def run(args):
    p=subprocess.run(args,cwd=ROOT,text=True,stdout=subprocess.PIPE,stderr=subprocess.STDOUT,timeout=35)
    assert p.returncode == 0, p.stdout
    return p.stdout
rng=random.Random(25303060076)
with tempfile.TemporaryDirectory() as td:
    trace=pathlib.Path(td)/'random.trace'
    trace.write_text(''.join(f' {rng.choice("ILSM")} {rng.getrandbits(64):x},1\n' for _ in range(1000)))
    for s,b,E in [(0,64,1),(1,63,2),(0,0,5),(6,0,4)]+[(rng.randrange(7),rng.randrange(57),rng.randrange(1,9)) for _ in range(96)]:
        args=['-v','-s',str(s),'-E',str(E),'-b',str(b),'-t',str(trace)]
        assert run(['./csim']+args)==run(['./csim-ref']+args),(s,b,E)
print('Csim: 100 randomized/boundary differential cases PASS',flush=True)
count=0; max_probes=0
for b,s,E in itertools.product((3,7),(1,9),(1,2,3,4,5,6,8,12,16)):
    for policy,wa in itertools.product(('LRU',) if E==1 else ('LRU','FIFO','RANDOM'),(0,1)):
        out=run(['./test-solver','-c',f'{b},{s},{E},{policy},{wa}'])
        m=re.search(r'Answer: b=(\d+) s=(\d+) E=(\d+) (LRU|FIFO|RANDOM) wa=(\d+) \((\d+) probes\)',out)
        assert m,out
        assert tuple(map(int,m.group(1,2,3,5)))==(b,s,E,wa),out
        assert E==1 or m[4]==policy,out
        max_probes=max(max_probes,int(m[6]))
        count+=1
print(f'Bonus: {count} geometry-boundary/policy/write-allocate combinations PASS; max probes {max_probes}',flush=True)
for sid in ('00000000000','11111111111','12345678901','99999999999','25303060076'):
    out=run(['./test-solver','-q','-i',sid])
    assert 'TEST_SOLVER_RESULTS=1:8:8:6:6:8' in out,out
print('Bonus: 5 student IDs, 40 practice caches PASS',flush=True)
