/* Offline educational replay. Search rules follow RoutePoolRepair.cpp;
   comparison policies change only which upper bounds prune. */
'use strict';
const RoutePoolTree = (() => {
  const route = (id, profit, requests) => ({id, profit, requests});
  const examples = {
    cycle: {
      name: 'Four-taxi coordinated exchange', requests: 4,
      note: 'The warm start stays at 400. Replacing all four routes together reaches 440. Follow the green path, then inspect the cut branches.',
      pools: [0,1,2,3].map(t => [route('A',110,[(t+1)%4]), route('B',100,[t])]),
      initial: ['B','B','B','B']
    },
    competition: {
      name: 'Competing requests & price bounds', requests: 9,
      note: 'The warm start is 447; the global search reaches 533. With both bounds it visits 19 nodes, versus 39 with the simple bound alone. Jump to a cut to inspect the arithmetic.',
      pools: [
        [route('A',148,[0,3]), route('B',163,[0,6]), route('C',43,[4])],
        [route('A',113,[0,1]), route('B',108,[1,5]), route('C',66,[5])],
        [route('A',149,[1,2]), route('B',132,[0,2]), route('C',38,[6])],
        [route('A',138,[1,2]), route('B',157,[0,8]), route('C',25,[7])],
        [route('A',187,[2,3]), route('B',82,[1,8]), route('C',44,[8])]
      ], initial: ['C','C','C','C','C']
    }
  };
  const cmpRequests = (a,b) => {
    for (let i=0;i<Math.min(a.length,b.length);i++) if(a[i]!==b[i]) return a[i]-b[i];
    return a.length-b.length;
  };
  const fmt = x => x === null || x === undefined ? '—' : (Math.abs(x-Math.round(x))<1e-8 ? String(Math.round(x)) : x.toFixed(2));
  function prepare(input) {
    const data = JSON.parse(JSON.stringify(input));
    for(const row of data.pools) row.sort((a,b)=>b.profit-a.profit || cmpRequests(a.requests,b.requests));
    const frequency=Array(data.requests).fill(0);
    data.pools.forEach(row=>new Set(row.flatMap(c=>c.requests)).forEach(r=>frequency[r]++));
    const scores=data.pools.map(row=>row.reduce((sum,c)=>sum+c.requests.reduce((s,r)=>s+frequency[r]-1,0),0));
    const order=data.pools.map((_,t)=>t).filter(t=>data.pools[t].length).sort((a,b)=>scores[b]-scores[a] || a-b);
    let best=data.initial.map((id,t)=>data.pools[t].find(c=>c.id===id) || null);
    const total=selected=>selected.reduce((s,c)=>s+(c?c.profit:0),0);
    const initialProfit=total(best);
    // Exact small-group warm start, same pair passes and one triple pass as C++.
    function groupImprove(group) {
      const occupied=new Set();
      best.forEach((c,t)=>{if(c&&!group.includes(t))c.requests.forEach(r=>occupied.add(r));});
      let old=group.reduce((s,t)=>s+(best[t]?best[t].profit:0),0), value=old, groupBest=group.map(t=>best[t]);
      const chosen=[];
      function visit(d,p) {
        if(d===group.length){if(p>value+1e-9){value=p;groupBest=chosen.slice();}return;}
        let bound=p;
        for(let j=d;j<group.length;j++) bound+=(data.pools[group[j]].find(c=>c.requests.every(r=>!occupied.has(r)))?.profit || 0);
        if(bound<=value+1e-9)return;
        for(const c of data.pools[group[d]]){
          if(c.requests.some(r=>occupied.has(r)))continue;
          chosen[d]=c;c.requests.forEach(r=>occupied.add(r));visit(d+1,p+c.profit);c.requests.forEach(r=>occupied.delete(r));
        }
        chosen[d]=null;visit(d+1,p);
      }
      visit(0,0);
      if(value<=old+1e-9)return false;
      group.forEach((t,i)=>best[t]=groupBest[i]);return true;
    }
    let changed=true;
    while(changed){changed=false;for(let a=0;a<order.length;a++)for(let b=a+1;b<order.length;b++)changed=groupImprove([order[a],order[b]])||changed;}
    for(let a=0;a<order.length;a++)for(let b=a+1;b<order.length;b++)for(let c=b+1;c<order.length;c++)groupImprove([order[a],order[b],order[c]]);
    const warm=best.slice(), warmProfit=total(warm);
    let prices=Array(data.requests).fill(0), bestPrices=prices.slice(), bestDual=Infinity, updates=0;
    for(let iteration=0;iteration<192;iteration++){
      let dual=prices.reduce((s,p)=>s+p,0);const uses=Array(data.requests).fill(0);
      order.forEach(t=>{
        let maximum=0, chosen=null;
        data.pools[t].forEach(c=>{const p=c.profit-c.requests.reduce((s,r)=>s+prices[r],0);if(p>maximum){maximum=p;chosen=c;}});
        dual+=maximum;if(chosen)chosen.requests.forEach(r=>uses[r]++);
      });
      updates++;
      if(dual<bestDual){bestDual=dual;bestPrices=prices.slice();}
      const norm=uses.reduce((sum,u,r)=>sum+((prices[r]>0||u>1)?(u-1)**2:0),0);
      if(!norm||dual<=warmProfit+1e-7)break;
      const step=.9*(dual-warmProfit)/norm;
      prices=prices.map((p,r)=>Math.max(0,p+step*(uses[r]-1)));
    }
    prices=bestPrices;
    data.pools.forEach(row=>row.forEach(c=>c.reduced=c.profit-c.requests.reduce((s,r)=>s+prices[r],0)));
    const branches=data.pools.map(row=>row.slice().sort((a,b)=>b.reduced-a.reduced));
    return {data,frequency,scores,order,warm,warmProfit,initialProfit,prices,branches,bestDual,updates};
  }
  function search(input, policy='both') {
    const prep=prepare(input), {data,order,prices,branches}=prep;
    const nodes=[], events=[], occupied=new Set(), selected=Array(data.pools.length).fill(null);
    let best=prep.warm.slice(), bestProfit=prep.warmProfit;
    const priceTotal=prices.reduce((s,p)=>s+p,0);
    const compatible=c=>c.requests.every(r=>!occupied.has(r));
    function emit(type,node,extra={}) {
      const event={type,node:node.id,depth:node.depth,profit:node.profit,
        occupied:node.occupied.slice(),selected:node.selected.slice(),best:best.slice(),bestProfit,
        ub1:node.ub1??null,ub2:node.ub2??null,parts1:node.parts1||[],parts2:node.parts2||[],
        priceUsed:node.priceUsed,...extra};
      events.push(event);if(node.first===undefined)node.first=events.length-1;node.last=events.length-1;
    }
    function visit(depth,profit,priceUsed,parent,edge) {
      const node={id:nodes.length,depth,profit,priceUsed,parent,edge,children:[],
        occupied:[...occupied].sort((a,b)=>a-b),selected:selected.slice()};
      nodes.push(node);if(parent!==null)nodes[parent].children.push(node.id);
      emit('enter',node);
      if(depth===order.length){
        if(profit>bestProfit+1e-9){bestProfit=profit;best=selected.slice();emit('incumbent',node);}else emit('leaf',node);
        return node;
      }
      if(policy!=='none'){
        node.parts1=order.slice(depth).map(t=>({taxi:t,route:data.pools[t].find(compatible)||null}));
        node.ub1=profit+node.parts1.reduce((s,item)=>s+(item.route?item.route.profit:0),0);
        emit('bound1',node);
        if(node.ub1<=bestProfit+1e-9){emit('prune1',node);return node;}
      }
      if(policy==='both'){
        node.parts2=order.slice(depth).map(t=>({taxi:t,route:branches[t].find(c=>c.reduced>0&&compatible(c))||null}));
        node.ub2=profit+priceTotal-priceUsed+node.parts2.reduce((s,item)=>s+(item.route?item.route.reduced:0),0);
        emit('bound2',node);
        if(node.ub2<bestProfit-1e-7){emit('prune2',node);return node;}
      }
      const t=order[depth];
      for(const c of branches[t]){
        if(!compatible(c)){emit('conflict',node,{candidate:c,taxi:t,blocked:c.requests.filter(r=>occupied.has(r))});continue;}
        selected[t]=c;c.requests.forEach(r=>occupied.add(r));
        visit(depth+1,profit+c.profit,priceUsed+c.requests.reduce((s,r)=>s+prices[r],0),node.id,{taxi:t,route:c});
        c.requests.forEach(r=>occupied.delete(r));selected[t]=null;
        emit('backtrack',node,{returned:c,taxi:t});
      }
      selected[t]=null;visit(depth+1,profit,priceUsed,node.id,{taxi:t,route:null});
      emit('backtrack',node,{returned:null,taxi:t});emit('complete',node);
      return node;
    }
    const root=visit(0,0,0,null,null);emit('finished',root);
    return {...prep,nodes,events,policy,profit:bestProfit,best,prune1:events.filter(e=>e.type==='prune1').length,
      prune2:events.filter(e=>e.type==='prune2').length,conflicts:events.filter(e=>e.type==='conflict').length};
  }
  return {examples,prepare,search,fmt};
})();
if(typeof module!=='undefined'&&module.exports)module.exports=RoutePoolTree;
if(typeof document!=='undefined') (()=>{
  const $=id=>document.getElementById(id), esc=s=>String(s).replace(/[&<>"']/g,c=>({'&':'&amp;','<':'&lt;','>':'&gt;','"':'&quot;',"'":'&#39;'}[c]));
  const {fmt}=RoutePoolTree;
  let run,index=0,timer=null,zoom=1;
  const controls=['example','policy','reset','previous','play','next','next-cut','next-best','timeline','speed','zoom-out','zoom-in'];
  const nodeElements=new Map(), edgeElements=new Map();
  const requestLabel=r=>`R${r+1}`;
  const routeLabel=(t,c)=>`T${t+1} · ${c?c.id:'unused'}`;
  const phaseNames={enter:'Choose route / enter node',bound1:'Calculate simple upper bound',bound2:'Calculate request-price upper bound',
    prune1:'Prune · simple bound',prune2:'Prune · price bound',conflict:'Skip · request conflict',incumbent:'New best complete fleet',leaf:'Complete fleet',
    backtrack:'Backtrack · release requests',complete:'All branches explored',finished:'Search finished'};
  function stop(){if(timer)clearInterval(timer);timer=null;$('play').textContent='▶ Play';}
  function buildTree(){
    nodeElements.clear();edgeElements.clear();
    let row=0;
    function layout(id){const n=run.nodes[id];n.x=24+n.depth*182;if(!n.children.length)n.y=24+(row++)*77;else{n.children.forEach(layout);n.y=(run.nodes[n.children[0]].y+run.nodes[n.children.at(-1)].y)/2;}}
    layout(0);run.width=run.order.length*182+210;run.height=Math.max(410,row*77+55);
    const svg=$('tree');svg.replaceChildren();svg.setAttribute('viewBox',`0 0 ${run.width} ${run.height}`);
    const add=(name,attrs,parent)=>{const el=document.createElementNS('http://www.w3.org/2000/svg',name);Object.entries(attrs).forEach(([k,v])=>el.setAttribute(k,v));parent.appendChild(el);return el;};
    for(const n of run.nodes){
      if(n.parent!==null){const p=run.nodes[n.parent];const edge=add('path',{d:`M ${p.x+150} ${p.y+26} C ${p.x+166} ${p.y+26},${n.x-16} ${n.y+26},${n.x} ${n.y+26}`,class:'edge'},svg);edgeElements.set(n.id,edge);}
    }
    for(const n of run.nodes){
      const group=add('g',{class:'node',transform:`translate(${n.x},${n.y})`,'data-node':n.id,tabindex:-1,role:'button'},svg);
      add('rect',{width:150,height:56,rx:8},group);
      const title=add('text',{x:10,y:18},group);title.textContent=n.edge?routeLabel(n.edge.taxi,n.edge.route):'Root · no taxis decided';
      const profit=add('text',{x:10,y:34,class:'small'},group);profit.textContent=`Depth ${n.depth} · profit ${fmt(n.profit)}`;
      add('text',{x:10,y:48,class:'node-tag'},group);
      const tip=add('title',{},group);tip.textContent=`Node ${n.id}: occupied ${n.occupied.map(requestLabel).join(', ')||'none'}. Click to inspect.`;
      group.addEventListener('click',()=>inspect(n.id));group.addEventListener('keydown',e=>{if(e.key==='Enter'||e.key===' '){e.preventDefault();inspect(n.id);}});
      nodeElements.set(n.id,group);
    }
    sizeTree();
  }
  function inspect(id){
    const target=run.events.slice(0,index+1).findLastIndex(e=>e.node===id&&['enter','prune1','prune2','incumbent','leaf','complete'].includes(e.type));
    if(target>=0){stop();index=target;render();}
  }
  function sizeTree(){ $('tree').style.width=`${run.width*zoom}px`;$('tree').style.height=`${run.height*zoom}px`;$('zoom-label').textContent=`${Math.round(zoom*100)}%`; }
  function start(){stop();index=0;zoom=1;run=RoutePoolTree.search(RoutePoolTree.examples[$('example').value],$('policy').value);$('case-note').textContent=run.data.note;$('timeline').max=run.events.length-1;buildTree();render();document.body.dataset.ready='true';}
  function tell(e){
    const n=run.nodes[e.node], next=run.order[e.depth];
    switch(e.type){
      case 'enter':return n.edge?`${routeLabel(n.edge.taxi,n.edge.route)} selected. ${n.edge.route?`Mark ${n.edge.route.requests.map(requestLabel).join(', ')} occupied.`:'No requests are added.'} ${next!==undefined?`Taxi ${next+1} is next.`:'Every taxi is now decided.'}`:'Start from an empty partial fleet. The warm-start fleet remains the incumbent, but its requests are not locked.';
      case 'bound1':return `Independently take the best compatible route for each remaining taxi. This optimistic total is ${fmt(e.ub1)}; the incumbent is ${fmt(e.bestProfit)}.`;
      case 'bound2':return `Add available request prices once, then the best compatible adjusted profit for each remaining taxi. The priced upper bound is ${fmt(e.ub2)}.`;
      case 'prune1':return `${fmt(e.ub1)} ≤ ${fmt(e.bestProfit)}. Even the optimistic completion cannot improve the incumbent. Do not create this node’s children.`;
      case 'prune2':return `${fmt(e.ub2)} < ${fmt(e.bestProfit)}. Request competition tightens the bound enough to stop this whole branch.`;
      case 'conflict':return `Skip ${routeLabel(e.taxi,e.candidate)}: ${e.blocked.map(requestLabel).join(', ')} ${e.blocked.length===1?'is':'are'} already occupied. No child node is created.`;
      case 'incumbent':return `A complete compatible fleet reaches ${fmt(e.profit)}. Save it as the new incumbent. Future branches must beat this value.`;
      case 'leaf':return `Complete fleet: ${fmt(e.profit)}. It does not improve the incumbent ${fmt(e.bestProfit)}, so keep the incumbent.`;
      case 'backtrack':return `Return from ${routeLabel(e.taxi,e.returned)}. ${e.returned?`Release ${e.returned.requests.map(requestLabel).join(', ')}.`:'No requests to release.'} Restore this parent’s profit and occupied set, then try its next alternative.`;
      case 'complete':return `All compatible choices, including leaving taxi ${next+1} unused, have been explored at this node. Return to its parent.`;
      case 'finished':return `Finished: best fleet ${fmt(e.bestProfit)}, versus warm start ${fmt(run.warmProfit)}. The explored tree proves optimality within this example’s stored route pool.`;
      default:return '';
    }
  }
  function boundHtml(e,kind){
    const value=kind===1?e.ub1:e.ub2, parts=kind===1?e.parts1:e.parts2;
    const title=kind===1?'U₁ · Simple bound':'U₂ · Request-price bound';
    if(value===null){const reason=kind===2&&run.policy!=='both'?'Disabled for this comparison.':e.depth===run.order.length?'Leaf: all taxis already decided.':'Not evaluated at this step.';return `<strong>${title}</strong><div class="why">${reason}</div>`;}
    const terms=parts.map(p=>p.route?(kind===1?p.route.profit:p.route.reduced):0);
    const available=run.prices.reduce((s,p)=>s+p,0)-e.priceUsed;
    const expression=[e.profit,...(kind===2?[available]:[]),...terms].map(fmt).join(' + ');
    const breakdown=parts.map(p=>`<div><span>T${p.taxi+1}: ${p.route?`${p.route.id} · {${p.route.requests.map(requestLabel).join(', ')}}`:'unused / no positive compatible choice'}</span><span>${fmt(p.route?(kind===1?p.route.profit:p.route.reduced):0)}</span></div>`).join('');
    return `<strong>${title}</strong><div class="equation">${expression} = <b>${fmt(value)}</b></div><div class="why">${kind===1?'P + independent compatible maxima; overlap among future routes is ignored.':`P + available prices (${fmt(available)}) + independent adjusted maxima. Negative maxima become 0.`}</div><div class="breakdown">${breakdown}</div>`;
  }
  function render(){
    const e=run.events[index];$('timeline').value=index;$('step-counter').textContent=`${index+1} / ${run.events.length} events`;
    $('previous').disabled=index===0;$('next').disabled=index===run.events.length-1;
    $('next-cut').disabled=!run.events.slice(index+1).some(x=>x.type==='prune1'||x.type==='prune2');
    $('next-best').disabled=!run.events.slice(index+1).some(x=>x.type==='incumbent');
    $('phase').textContent=phaseNames[e.type];$('phase').style.color=e.type.startsWith('prune')||e.type==='conflict'?'var(--red)':'var(--green)';
    $('explain').textContent=tell(e);$('node-number').textContent=`Node ${e.node}`;
    $('depth').textContent=`${e.depth}/${run.order.length}`;$('profit').textContent=fmt(e.profit);$('incumbent').textContent=fmt(e.bestProfit);$('price-used').textContent=fmt(e.priceUsed);
    const occupied=new Set(e.occupied), blocked=new Set(e.blocked||[]);
    $('occupancy').innerHTML=run.prices.map((p,r)=>`<span class="chip ${occupied.has(r)?'busy':''} ${blocked.has(r)?'blocked':''}" title="Request price λ = ${fmt(p)}">${requestLabel(r)} ${occupied.has(r)?'●':'○'} · λ ${fmt(p)}</span>`).join('');
    for(const kind of [1,2]){const el=$(`bound${kind}`);el.innerHTML=boundHtml(e,kind);el.className=`bound ${e.type===`bound${kind}`?'active':''} ${e.type===`prune${kind}`?'pruned':''}`;}
    $('node-note').textContent=e.type==='finished'?`Price discovery: ${run.updates} evaluations. Best root price bound: ${fmt(run.bestDual)}.`:'P is actual selected profit. L is a complete feasible fleet. Upper bounds estimate the best possible completion.';
    const t=run.order[e.depth];$('route-title').textContent=t===undefined?'All taxis decided':`Next taxi: T${t+1} · route alternatives`;
    $('routes').innerHTML=t===undefined?'<p class="subtle">This leaf has no remaining route choices.</p>':`<div class="route-row head"><span>Route</span><span>Requests</span><span class="num">Profit</span><span class="num">Adjusted</span><span>Status</span></div>`+run.branches[t].map(c=>{
      const conflicts=c.requests.filter(r=>occupied.has(r)),highlight=e.candidate?.id===c.id&&e.taxi===t;
      return `<div class="route-row ${highlight?'highlight':''}"><strong>T${t+1} · ${esc(c.id)}</strong><span>${c.requests.map(requestLabel).join(', ')}</span><span class="num">${fmt(c.profit)}</span><span class="num">${fmt(c.reduced)}</span><span class="${conflicts.length?'blocked-text':'ok'}">${conflicts.length?'Blocked: '+conflicts.map(requestLabel).join(', '):'Compatible'}</span></div>`;
    }).join('')+'<div class="route-row"><strong>Unused</strong><span>∅</span><span class="num">0</span><span class="num">0</span><span class="ok">Tried last</span></div>';
    $('order').innerHTML=run.order.map((taxi,d)=>`<span class="order-item ${d===e.depth?'active':''} ${d<e.depth?'done':''}">T${taxi+1} · score ${run.scores[taxi]}</span>`).join('');
    $('best-total').textContent=`Total ${fmt(e.bestProfit)}`;
    $('best-fleet').innerHTML=e.best.map((c,taxi)=>`<div class="fleet-row"><strong>${routeLabel(taxi,c)}</strong><span>${c?c.requests.map(requestLabel).join(', '):'No requests'}</span><strong>${fmt(c?c.profit:0)}</strong></div>`).join('');
    $('best-note').textContent=`Initial fleet: ${fmt(run.initialProfit)} → pair/triple warm start: ${fmt(run.warmProfit)} → current best: ${fmt(e.bestProfit)}`;
    const statuses=new Map(),visited=new Set();let cuts1=0,cuts2=0,conflicts=0;
    for(let i=0;i<=index;i++){const x=run.events[i];if(x.type==='enter'){visited.add(x.node);statuses.set(x.node,'entered');}if(['prune1','prune2','incumbent','leaf'].includes(x.type))statuses.set(x.node,x.type);if(x.type==='complete'||x.type==='finished')statuses.set(x.node,'finished');if(x.type==='prune1')cuts1++;if(x.type==='prune2')cuts2++;if(x.type==='conflict')conflicts++;}
    const path=new Set();let id=e.node;while(id!==null){path.add(id);id=run.nodes[id].parent;}
    for(const [id,el]of nodeElements){const seen=visited.has(id);el.style.display=seen?'':'none';el.setAttribute('tabindex',seen?'0':'-1');const status=statuses.get(id)||'';el.setAttribute('class',`node ${status} ${id===e.node?'current':''}`);el.querySelector('.node-tag').textContent=status==='prune1'?'CUT · SIMPLE':status==='prune2'?'CUT · PRICED':status==='incumbent'?'NEW INCUMBENT':status==='leaf'?'LEAF':status==='finished'?'EXPLORED':id===e.node?'CURRENT NODE':'';}
    for(const[id,el]of edgeElements){el.style.display=visited.has(id)?'':'none';el.setAttribute('class',`edge ${path.has(id)?'on-path':''}`);}
    $('tree-stats').innerHTML=`<span>Visited <b>${visited.size}</b> nodes</span><span>Simple cuts <b>${cuts1}</b></span><span>Priced cuts <b>${cuts2}</b></span><span>Conflict skips <b>${conflicts}</b></span>`;
    if($('follow').checked){const node=run.nodes[e.node],viewport=$('viewport');viewport.scrollTo({left:Math.max(0,(node.x+75)*zoom-viewport.clientWidth/2),top:Math.max(0,(node.y+28)*zoom-viewport.clientHeight/2),behavior:'auto'});}
    document.body.dataset.event=e.type;document.body.dataset.step=String(index);document.body.dataset.best=String(e.bestProfit);
  }
  function step(delta){stop();index=Math.max(0,Math.min(run.events.length-1,index+delta));render();}
  function jump(types){stop();const next=run.events.findIndex((e,i)=>i>index&&types.includes(e.type));if(next>=0){index=next;render();}}
  $('example').addEventListener('change',start);$('policy').addEventListener('change',start);$('reset').addEventListener('click',()=>{stop();index=0;render();});
  $('previous').addEventListener('click',()=>step(-1));$('next').addEventListener('click',()=>step(1));
  $('next-cut').addEventListener('click',()=>jump(['prune1','prune2']));$('next-best').addEventListener('click',()=>jump(['incumbent']));
  $('timeline').addEventListener('input',()=>{stop();index=Number($('timeline').value);render();});
  function play(){if(timer){stop();return;}if(index===run.events.length-1)index=0;$('play').textContent='Ⅱ Pause';timer=setInterval(()=>{if(index>=run.events.length-1){stop();return;}index++;render();},Number($('speed').value));}
  $('play').addEventListener('click',play);$('speed').addEventListener('change',()=>{if(timer){stop();play();}});
  $('zoom-out').addEventListener('click',()=>{zoom=Math.max(.4,zoom-.15);sizeTree();render();});$('zoom-in').addEventListener('click',()=>{zoom=Math.min(2,zoom+.15);sizeTree();render();});
  document.addEventListener('keydown',e=>{if(['INPUT','SELECT','TEXTAREA','BUTTON'].includes(e.target.tagName))return;if(e.key==='ArrowRight'){e.preventDefault();step(1);}if(e.key==='ArrowLeft'){e.preventDefault();step(-1);}if(e.code==='Space'){e.preventDefault();play();}});
  window.routePoolViz={search:RoutePoolTree.search,examples:RoutePoolTree.examples,getRun:()=>run,getIndex:()=>index,seek:i=>{stop();index=Math.max(0,Math.min(run.events.length-1,i));render();}};
  try{start();}catch(error){$('warning').style.display='block';$('warning').textContent=`Could not initialize walkthrough: ${error.message}`;controls.forEach(id=>$(id).disabled=true);throw error;}
})();
