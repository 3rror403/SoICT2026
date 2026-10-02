#!/usr/bin/env python3
"""Write input-distribution and solo-feasibility observations for a generated set."""
import csv
import hashlib
import json
from pathlib import Path
import sys
import numpy as np

folder = Path(sys.argv[1]) if len(sys.argv)>1 else Path(__file__).resolve().parents[1]/'data/synthetic'
profile = json.loads((folder/'generation.json').read_text())
records = []
def quantiles(values):
    a = np.asarray(values).ravel()
    return None if not a.size else dict(zip(['min','p25','median','p75','max'],map(float,np.quantile(a,[0,.25,.5,.75,1]))))
for row in csv.DictReader((folder/'manifest.csv').open()):
    path = folder/row['file']
    with path.open() as f:
        k,n,m = map(int,f.readline().split()); v=k+2*(n+m)
        taxi = np.array([list(map(float,f.readline().split())) for _ in range(k)])
        passengers = np.array([list(map(float,f.readline().split())) for _ in range(n)]).reshape(n,6)
        parcels = np.array([list(map(float,f.readline().split())) for _ in range(m)]).reshape(m,9)
        t = np.array([np.fromstring(f.readline(),sep=' ') for _ in range(v)])
        c = np.array([np.fromstring(f.readline(),sep=' ') for _ in range(v)])
    origins = taxi[:,0].astype(int)-1
    record = dict(instance=path.name, requests=n+m,taxis=k,
                  hash_matches_manifest=hashlib.sha256(path.read_bytes()).hexdigest()==row['sha256'])
    offdiag = ~np.eye(v,dtype=bool)
    record['matrices'] = dict(time=quantiles(t[offdiag]),cost=quantiles(c[offdiag]),
        cost_time_ratio=quantiles(c[offdiag]/t[offdiag]),
        symmetric_time=bool(np.array_equal(t,t.T)),symmetric_cost=bool(np.array_equal(c,c.T)),
        zero_diagonals=bool(np.all(t.diagonal()==0) and np.all(c.diagonal()==0)))
    for kind,requests in [('passengers',passengers),('parcels',parcels)]:
        feasible = 0; ratios=[]; earliest=[]; widths=[]; deltas=[]
        for r in requests:
            p,d = int(r[0])-1,int(r[1])-1
            arrival=t[origins,p]
            if kind=='passengers':
                valid=arrival<=r[3]; revenue=r[5];e=r[2];width=r[3]-r[2]
            else:
                delivery=np.maximum(arrival,r[3])+r[7]+t[p,d]
                valid=(arrival<=r[4])&(delivery<=r[6])&(r[2]<=taxi[:,1])
                revenue=r[8];e=r[3];width=r[4]-r[3]
                deltas.append(r[5]-r[3]-r[7]-t[p,d])
            feasible+=int(valid.any());ratios.append(revenue/c[p,d]);earliest.append(e);widths.append(width)
        record[kind] = dict(count=len(requests),solo_feasible=feasible,
            earliest=quantiles(earliest),pickup_width=quantiles(widths),
            revenue_direct_cost_ratio=quantiles(ratios))
        if kind=='parcels': record[kind]['delivery_delta']=quantiles(deltas)
    records.append(record)
summary = dict(generation=profile['generation'],
    manifest_sha256=hashlib.sha256((folder/'manifest.csv').read_bytes()).hexdigest(),
    instances=len(records),requests=sum(r['requests'] for r in records),
    solo_feasible=sum(r[k]['solo_feasible'] for r in records for k in ['passengers','parcels']),
    scope='Input distribution, hashes, symmetry, and solo feasibility; no solver or integration tests.',
    triangle_inequality='Guaranteed by ceil of scaled Euclidean geometry in the empirical profile; not exhaustively enumerated.',
    records=records)
(folder/'validation.json').write_text(json.dumps(summary,indent=2)+'\n')
print(json.dumps({k:v for k,v in summary.items() if k!='records'},indent=2))
for r in records:
    if r['passengers']['count'] and r['passengers']['count']==r['parcels']['count']:
        print(r['instance'], 'time median',r['matrices']['time']['median'],
              'revenue/cost medians',r['passengers']['revenue_direct_cost_ratio']['median'],r['parcels']['revenue_direct_cost_ratio']['median'])
