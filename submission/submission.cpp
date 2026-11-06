#pragma GCC optimize("O3","unroll-loops")
#include <cstddef>
#include <limits>
#include <algorithm>
#include <vector>
#include <iostream>
#include <string>
#include <cstdlib>
#include <array>
#include <charconv>
#include <iomanip>
#include <string_view>
#include <cmath>
#include <cstdint>
#include <optional>
#include <atomic>
#include <memory>
#include <utility>
#include <chrono>
#include <map>
#include <random>
#include <queue>
#include <numeric>
#include <functional>
#include <iterator>
#include <tuple>
#define la std::size_t
#define lb const auto
#define lc std::vector<
#define H std::
#define B .size()
#define lf H chrono::steady_clock::now()
#define lg H numeric_limits<double>::infinity(
#define lh }namespace bk::eu{
#define R )return
#define lj ;for(
#define lk )const
#define ll static_cast<
#define Y .push_back(
#define ln .begin()
#define A ;if(
#define lq H optional
#define K ;double
#define ls (const
#define lt ;return
#define lu double>
#define lv ,const
#define lw continue;
#define lx {return
#define ly H uint64_t
#define lz H chrono::duration_cast<eY::duration>(H chrono::duration<lu
#define D ){if(
#define lB ;const
#define lC R H nullopt
#define lD lc la
#define lE lj la
#define lF lj auto
#define lG .end()
#define N ac.
#define J ){lb
#define lJ .W()
#define E ,la
#define h ;lb
#define lM R false
#define C );}
#define z =0;
#define lP )if(all||e==first||e==second)hE.add(cB(ac,b2,r,e,N bu(e)
#define Z [e]
#define lR H chrono::
#define lS H make_unique
#define lT H min(
#define lU .empty()
#define lV {public:
#define U ;++
#define lX (void)
#define Q (la
#define lZ =true
#define ma ls ab&
#define mb H shared_ptr<const aQ>
#define F )A
#define md ll int>(
#define me loc.
#define mf if(
#define mg unsigned char>
#define mh gq();bool aS=false;while(lb aP=hE.dp()J I=aP->I h bg=aP->bg h ep=I.e lB bool cL=N ck(bg)lB bool cM=N ck(ep)lB bool iF=bg==ep lB bool
#define mi )break
#define mj lB double
#define mk N au(
#define ml in.cG(
#define mm H max(
#define mn N aj()
#define mo ,double
#define mp final:public c2 lV lD>h2(ab&E,H mt19937_64&lk override;};lh lD>
#define mq =false
#define mr r)h bg=ll la>(mk r).e F!b2.hN()R h aF=N bu(bg).bo()h&dm=N aT()lB bool iQ=dm&&dm->hS(r F iQ D all){static const ak fU lj
#define ms private:
#define mt .resize(
#define mu bu(e)
#define mv lG,[](lb&a,lb&b)lx a.
#define mw H numeric_limits
#define mx ;bool
#define my insert(
#define mz H move(
#define mA e lv ak&
#define mB );read(ec,
#define mC ,0 F L)hE.add(ao{r,I.e,0,0,*L},bg);break;}}for(lb e:N cz()lP,aF),bg C else{for Q e z e<N aJ()U e lP,
#define mD 0 C;mh gw=iF?N cC(I.o,I.aw,I.as):(N g3(I.o)&&N my I.o,ep,I.aw,I.as));aS lZ A N ck(bg)!=cL||N ck
#define mE ,e,
#define mF &b2 E r E mA f,ly aF lk{lb bg=ll la>(mk r).e F e!=bg&&!b2.dO()lC lt fI.get(r mE N aJ(),aF,f.bo(),b2.cD(),e==
#define O [r]
#define mH ;struct
#define mI lk lx
#define mJ auto&
#define mK C return
#define mL ls aB&
#define mM ;void
#define mN ;mutable
#define mO H isfinite(
#define mP lE e z e
#define mQ in.
#define mR B U
#define mS .erase(
#define mT first,
#define mU final:public jx lV bool b7(
#define mV (o)
#define mW ls lc br>&al E X E G lv lc int>&aU
#define mX .clear(
#define mY C void
#define mZ first E
#define na const double
#define nb lh class
#define nd lv lD>&
#define ne lc lu
#define nf for Q
#define ng ->second
#define nh .e
#define ni .reserve(
#define nj c3{bool operator()ls ba&lv ba&lk;};using iO=H priority_queue<ba,lc ba>,c3>mN const
#define nk af.dn(
#define nl lR steady_clock::time_point
#define nm lq<ao>
#define nn lE r z r
#define no .duration
#define np lB bool
#define nq }return
#define ns ll la>(
#define nt ab&ac E count,H mt19937_64
#define nu eY::now()+lR seconds
#define nw =mQ X B,n=mQ av B,m=mQ aZ B
#define nx ,ha);e9.cA(0,f1 B);fa.cA(0,f2 B)h gR=e9.a0(),gS=fa.a0(F!gR||!gS lM mj L=
#define ny ;}double
#define nz lq<lu
#define nA lF r:
#define nB lt true;}
#define nC Q mA f E
#define nD N ag()
#define nE ::b7(ab&ac lk{
#define nF ;else
#define nG H lower_bound(aD ln,aD lG
#define V .aq
#define nI namespace bk
#define nJ {double
#define nK lj lb
#define nL while(
#define nM ->L
#define nN ma ac E
#define nO mk r)nh
#define nP [&](auto a,auto
#define nQ ));
#define nR +1e-8<mm 1e-9,bn?bn nM:1e-9))lw lb
#define nS }void
#define nT namespace{
#define S )h
#define nV .first
#define nW H accumulate(
#define nX )==0&&lf>=
#define nY ,bool
#define nZ aV O
#define oa lk override;
#define ob .G)
#define oc value=
#define od mL in E
#define oe f mS f ln+me bT);f mS f ln+me aw
#define of W mS W ln+me bT);W mS W ln+me aw
#define og [pos]
#define oh i z i<
#define oi lk{mf!hN()||e>=c0.
#define oj nN o)mx hN(mI hB;}bool dO(mI bE.has_value(C double cD(mI bw
#define ok ,eg B);H partial_sort(eg ln,eg ln+count,eg mv first>b nV;}
#define ol for(
#define om Q o
#define on assign(
#define oo X Z.bd+1e-8
#define op b2(ac,mr lb&I:dm->fQ(r)D N ck(I nh))lw lb L=b2.L(I
#define oq U r D nO<0)lw lb iC=ns nO);dH(r,iC==bg||iC==ep,bg,ep C nq
#define os lD>
#define ot 1e-9)
#define ou [o]
#define ov om lk{lb n=ah->av B lt o<n?ah
#define ow iF=e==ch h cd=f lJ B-(iF?2
#define ox bL.gJ(*ah mE f);aR af(*ah mE bL);af.cA(0,f B S cG=af.
#define oy az.
#define oz jQ+0x9e3779b97f4a7c15ULL*
#define oA .second
#define oB ->o,I->e,I->aw,I->as nQ add(ac,I->e,false
#define oC U r)dH(r,true,0,mD(ep)!=cM){gq();lw}hE.fJ(bg,ep
#define oD .I
#define oE ;}}
#define oF ln,
#define oG lh struct
#define oH ab::
#define oI mT second
#define oJ aO am
#define oK data.
#define oL ,0)
#define oM +lR milliseconds(
#define oN f.aA(in,
#define oO [i]
#define oP ;class
#define oQ ){auto
#define oR (e,
#define oS ;}mf
#define oT .reset(
#define oU ag.X B
#define oV =[&]
#define oW (in mE f
#define oX mk o)nh
#define oY lE j z j
#define oZ lJ,me aw,me
#define pa f.my f ln+
#define pb cu[o*eq+e]
#define pc ;lc
#define pd e)
#define pe H shared_ptr<a7
#define pf bi(aH&af E mZ end
#define pg lE o z o<mn U o
#define ph ;la
#define pi av B
#define pj mL ag
#define pk (ac,
#define pl )/ll aO::duration::rep
#define pm lk{
#define pn const c4 eT(al,X,G);e1 kg(eT);kg.kd(am);
#define po ;ho.i5(am);oy fk+=ho.fk A ho.bQ){oy bQ lZ lt
#define pp e==ch?2:0)||(e!=ch&&!bE)lC lB d5 jT{f lJ
#define pq false)
#define pr mZ second
#define ps C else
#define pt al B
#define pu mQ aZ[
#define pv ::max(
#define pw lf>=am
#define px H sort(
#define py );af.
#define pz ol auto r:
#define pA de B&&!aY U
#define pB M>=k+2*n&&M<k+2*n+
#define pC ap(F a>b||b>f B||(aM&&a!=b)lM h aA oV Q
#define pD bs.eM(lS<
#define pE F!
#define pF lE pos z pos<=cw U pos
#define pG mF bg,[&]{nm bn h cw=f lJ B-(e==bg?2:0
#define pH ;auto
#define pI [j]
#define pJ N ap(
#define pK [c nh]
#define pL mJ f=cO;f=mu
#define pM nI{void eE(H ostream&out lv bA&bY lv aB&
#define pN cs->
#define pO >=0)
#define pP ;}else
#define pQ ()=default
#define pR lX(!N
#define pS H min<la>(
#define pT )mf
#define pU e nd
#define pV +1e-8
#define pW inline double
#define pX c1 B
#define pY value-=
#define pZ al Z
#define qa ;}const
#define qb aJ()
#define qc (*ah
#define qd .data()
#define qe eY::now(
#define qf );H iota
#define qg nn<nr U
#define qh ob bp O=
#define qi cJ Y lS<
#define qj cu[bP*eq+o]
#define qk lE u z u<nv U u
#define ql ll lu(jZ O)-1
#define qm os bx;bx ni count);nL bx B<count){la
#define qn =lT
#define qo loc=
#define qp =N
#define qq )continue
#define qr ;break
#define qs ;}la
#define qt )nB bool oH
#define qu ;H mt19937_64 rng
#define qv c0.ag(
#define qw iF)bi(af,
#define qx lE by z by<10000 U by D(by&7 nX am)
#define qy nE fG hE h dH oV Q r nY all E pr D nO<0
#define qz .bv()
#define qA .cG()
#define qB ->hS mV mP<eq U pd mf pb.bo!=N mu.
#define qC ah->
#define qD lJ B
#define qE bD oD
#define qF (in,
#define qG ,bV.
#define qH H unique_ptr<jx
#define qI H function<void
#define qJ =nW aV oF aV lG,0.0
#define qK pm lb
#define qL lE oh
#define qM C bool
#define qN A second<first)H swap(oI S a=
#define qO F at<f B J next=f.M(at);fD
#define qP second F a nh<0||b nh<0||a nh==b
#define qQ lh void ff(ab&cr,aG&hE lv ig&bV,
#define qR oE mf I==mn||!N g3(I)mi;bx Y
#define qS nE bool aS mq;nL lb I=d6(ac)){pR
#define qT bD.
#define qU lX 0
#define qV [first]
#define qW =mm 0.0,
#define qX M-k-2*
#define qY mU ab&oa ms mutable ih fI;nm cB ma
#define qZ ]pT ds(a2)){hF+=a2 V qr oS hF<=
#define ra }bool
#define rb me aw
#define rc ms struct
#define rd )lw
#define re ol lb
#define rf count=
#define rg template<class
#define rh H istream&
#define ri ml be,
#define rj bO E bb
#define rk .pop_back(
#define rl H stable_sort(
#define rm om E e E
#define rn >=aj()
#define ro G oF G lG
#define rp aZ B
#define rq h&in=
#define rr next[g8]
#define rs oR f,a
#define rt &[key,c]
#define ru ){using eY=lR steady_clock h
#define rv mQ aW mV:mQ a1(o
#define rw dQ=c V nA key)dQ
#define rx .h2 pk bx,rng)nF
#define ry ::c3::operator()ls ba&a lv ba
#define rz dj mX)lF&gj:dI)gj mX);jn z gu
#define rA -iS+(pos<cw?ml as,W og):0.0
#define rB nA c ob mf iD O>=0&&!hT[iD O])
#define rC ||oX<0 R h&qo mk o);ch=ns me
#define rD ch);bw=bz(nD,ch,f oZ bT C la
#define rE *0xbf58476d1ce4e5b9ULL;j6
#define rF F L&&(!bn||*L>bn nM))bn=
#define rG ag.
#define rH .aw
#define rI me e
#define rJ A!
#define rK lR duration<lu
#define rL H pair<double
#define rM A L<=1e-9 lM;
#define rN >az V+ot{az V
#define rO J be=at?f.M(at-1):e K
#define rP A cX[a2]>=0 J r=ns cX[a2])
#define rQ next.
#define rR J&
#define rS H nullopt
#define rT ;};
#define rU ab&ac
#define rV [a]
#define rW aw)+ml aw,
#define rX lG,nP b)lx
#define rY X B
#define rZ e:N cz()
#define sa lk;ms const ab&c0 ph dD,ch,
#define sb +ap(C rJ aA(a)||!aA(b)lM
#define sc nN o):c0(ac),dD mV,ch(0)
#define sd append(
#define se C}
#define sf lg)
#define sg z bool
#define sh pm mf
#define si n_ U
#define sj oy bf
#define sk aP oD
#define sl =0,
#define sm =df.
#define sn .bo(
#define so pc int>
#define sp rng()%
#define sq rR c=al
#define sr aS;}lh
#define ss mx aS mq lj;;){
#define st dr Q bZ,ak g9 E b0,ak ha mo
#define su ne el ls lc br>&al E X E G
#define sv lF id:aU pT id pO az V+=al[
#define sw (nQ
#define sx .L
#define sy mC aF),bg se h gq oV{hE mX
#define sz ;os
#define sA oV Q
#define sB =nullptr
#define sC C mf
#define sD aL Z
#define sE me bT
#define sF ==md
#define sG ma ac
#define sH add(
#define sI aG::
#define sJ aU Z
#define sK ;s.
#define sL mE jT,
#define sM <al mR
#define sN <count U
#define sO <X U
#define sP pE af qz lC
#define sQ *nv+q
#define sR +lR seconds(
#define sS ,-1)
#define sT od e lv d5&f
#define sU r pT nO pO bW Y
#define sV >L){L=
#define sW (ga+1,aq
#define sX bA empty;empty.cb mt oU
#define sY ;nL cm<iW&&ll mg(hm[cm
#define sZ mQ cl mV:mQ dg(o-n S as
#define ta cw F af qz)i4(az,{o mE
#define tb mj jP[]={1.0,0.8,1.2}
#define tc F ej<end)nk ej+2,end+
#define td [t]
#define te mB p rH.cR mB p rH.ed mB p
#define tf ]lF&ks:aV)ks*=ju;qU
#define tg a<pA a)nf b=a+1;b<pA b
#define th fj[b]lt x!=y?x>y:a<b
#define ti value)
#define tj (mI
#define tk )mM
#define tl Q e
#define tm mN lc
#define tn oy dp
#define to qp mu
#define tp 1e-9 R
#define tq lZ lt
#define tr =0 K
#define ts o E mA
#define tt ,true)
#define tu H shuffle(
#define tv H abs(
#define tw <=cw U
#define tx pc lD>>
#define ty [X pI]
#define tz bool j4 lZ nA
#define tA H max<la>(
#define tB a3(fd.aT sw
#define tC dc[row*n_+
#define tD od M J k nw A M<k+2*
#define tE =W[b3+1];iG+=ml as
#define tF pa b,cc mV);pa a,aI mV
#define tG ?mQ av ou.aE:pu o-n]
#define tH pos=cw+1;pos-->0;)
#define tI ;nq ny hF=aq lE d=ga;d
#define tJ lG,
#define tK )lt
#define tL M)
#define tM .as
#define tN first==
#define tO z}mH
#define tP nK M:
#define tQ h e=
#define tR np aM=
#define tS nK o:
#define tT lJ ln+
#define tU (nr oL
#define tV mQ b5[
#define tW 1e-9&&(
#define tX H fill(
#define tY aS lZ;
#define tZ unsigned long long>
#define ua nn<mn U
#define ub nc=pN pt
#define uc ,next)-ri next mK fD
#define ud lz(lT 1.0 qG dw*0.05
#define ue *mw<lu::epsilon()
#define uf F first<gi)nk mT gi
#define ug bu(b nh)qA-*gR-*gS
#define uh al rV V>al[b]V;})
#define ui mi;
#define uj pj E
#define uk mQ pi
#define ul lk mM
#define um (o
#define un I nM
#define uo rd lb
#define up C lb
#define uq K oc
#define ur ar::
#define us ms const aB
#define ut (ah,
#define uu .emplace_hint(gd
#define uv c8 Q bZ E fS E b0 E
#define uw tm lc eF>>bG mN iO dG
#define ux a2 qK ub A a2>=nc R
#define uy nF mf M>=k+2*n+m)
#define uz ),p rH.cR,p rH.ed,
#define uA .aE
#define uB lq<
#define uC =mm
#define uD ;mJ
#define uE =md
#define uF -1]
#define uG mx hB mq;nz bE K bw
#define uH E ts,ly aF lk rT nb
#define uI ::h2(nt&rng pm os
#define uJ ob j4&=!bp O rJ j4
#define uK 31 nX bq mi h I=bC
#define uL D aY R U ee A(ee
#define uM oE mf cI==nc+n_ R
#define uN nC bR oi qb||bR>f
#define uO dG lU rR bD=dG.top
#define uP );bk::eE(H cout
rg T,class P>void hM(lc T>&fp,P jL){fp mS H remove_if(fp oF fp tJ jL),fp lG C inline la hC Q ti lx value?mw<tZ::digits-__builtin_clzll(ll tZ(ti):0;}nI{struct i2 nJ cR tr ed tO kY nJ bd tO jy nJ aE z i2 aw K dv tO kz nJ aE tr dq z i2 aw;i2 as K dv tO aB{lc kY>X pc jy>av pc kz>aZ sz fB;ne b5;ne er ph gG tj rY+2*(pi+rp C la iL Q M mI fB lU?M+1:fB[M]qs cl Q i mI rY+i qs aW Q i mI rY+pi+i qs dg Q j mI rY+2*pi+j qs a1 Q j mI rY+2*pi+rp+j ny jv Q u E v mI b5[u*gG()+v]ny cG Q u E v mI er[u*gG()+v]oE;}nI{struct hP{bool bv mq K c5 z H string kZ;}mH bA{lc lD>>cb;hP fv pj lk rT}nI{hP bA::fv pj qK n=rG pi,m=rG rp h gF=oU pc bool>bW(n+m,pq K c5=0 lE k z k<gF U k){la be=k tP cb[k]){c5-=rG cG(be,M F M<gF+n)bW[M-gF]lZ nF mf M>=gF+2*n&&M<gF+2*n+m)bW[n+M-gF-2*n]lZ;be=M oE nf oh n+m U i pT bW oO)c5+=i<n?rG av oO uA:rG aZ[i-n]uA lt{true,c5,{}}oE nI{aB gp(rh in C nI{nT class dU{rh j7;H array<char,65536>hm{}ph cm sl iW z H string d3 mx ke(){iW=ns j7.rdbuf()->sgetn(hm qd,hm B nQ cm=0 lt iW!=0;}public:explicit dU(rh ec):j7(ec){}H string_view next(){d3 mX)lj;;D cm==iW&&!ke()R{}sY])<=' ')++cm A cm<iW ui}ol;;J hc=cm sY])>' ')++cm lB H string_view jK(hm qd+hc,cm-hc F cm<iW D d3 lU R jK;d3.sd jK qd,jK B tK d3;}d3.sd jK qd,jK B pE ke()R d3 oE};template<typename T>void read(dU&ec,T&value oQ jw=ec.next(pE jw lU&&jw.front()=='+')jw.remove_prefix(1);H from_chars(jw qd,jw qd+jw B,value mY read(dU&ec mo&ti{oc H strtod(H string(ec.next()).c_str(),nullptr se aB gp(rh in){dU ec(in)ph k sl n sl m z read(ec,k mB n mB m)lB la v=k+2*(n+m);aB ag;rG X mt k);rG av mt n);rG aZ mt m);rG fB mt v S iK sA tL{mJ id=rG fB[M];read(ec,id C mP<k U pd{iK(e mB rG X Z.bd C nf oh n U i){mJ p=rG av oO;iK(rG cl(i nQ iK(rG aW(i)te.dv mB p uA C nf j z j<m U j){mJ p=rG aZ pI;iK(rG dg(j nQ iK(rG a1(j)mB p.dq te tM.cR mB p tM.ed mB p.dv mB p uA C os h7(v)lE M z M<v U tL h7[rG iL(tL uF=M lF*eT:{&rG b5,&rG er}){eT->resize(v*v)lE u z u<v U u)nf w z w<v U w)read(ec,(*eT)[h7[u]*v+h7[w]]mK ag oE pM ag C pM ag J gB=bY.fv(ag);out<<H setprecision(17)<<gB.c5<<'\n' nK&f:bY.cb){out<<f B tP f)out<<' '<<rG iL(tL;out<<'\n' oE oG bS{la first sl g8 tr duration tr hL=-sf K bj=sf K cG tr bM sl dZ sl dY sg bv lZ rT bS b4 uj tL;bS kG pj lv bS&k3 lv bS&k1)oP ak lV ak pQ;ak uj pU f tk gJ uj pU f tk clear(tk cN ls ak&df);bS dn uj mZ g8 lk mx aA uj a8 lk K cG tj jn;}ly bo tj gu qa os&W tj ee;}ms os ee pc bS>dj pc lc bS>>dI K jn z ly gu z}oP aR lV aR uj mA bu tk cA Q mZ end tk fP ls bS&en tk fs Q tL;nz a0(lk;us&ah ph iJ lB ak&fI;uB bS>dl rT lh nT H atomic<ly>gm{1}K dv tD n R mQ av[(M-k)%n].dv lt pu(qX n)%m].dv ny jp tD n R 0 h j=(qX n)%m lt M<k+2*n+m?pu j].dq:-pu j].dq oE bS b4 od tL{bS s sK bj=mw<lu pv);s nV=s.g8=M;s no=dv qF M S k nw lB i2*h1 sB A M<k+n)h1=&mQ av[M-k]rH nF mf pB m)h1=&pu qX n]rH uy h1=&pu qX n-m]tM A h1){s.hL=h1->cR+s no sK bj=h1->ed;}s.bM=jp qF tL sK dZ qn 0.0,s.bM)sK dY qW s.bM tK s;}bS kG mL in lv bS&a lv bS&b){na eX=mQ jv(a.g8,b nV);bS s;s nV=a nV sK g8=b.g8;s no=a no+eX+b no sK hL uC a.hL+eX+b no,b.hL)sK bj qn a.bj,b.bj-eX-a no)sK bv=a.bv&&b.bv&&a.hL+eX<=b.bj sK cG=a.cG+ml a.g8,b nV)+b.cG sK bM=a.bM+b.bM sK dZ qn a.dZ,a.bM+b.dZ)sK dY uC a.dY,a.bM+b.dY tK s;}ak::ak od pU f){gJ oW mY ak::clear(){ee mX);rz z nS ak::cN ls ak&df D this==&df R;ee sm ee;dj sm dj A dI B<df.dI B)dI mt df.dI B)qL df.dI mR i)dI oO sm dI oO lE i sm dI B;i<dI mR i)dI oO mX);jn sm jn;gu sm gu;nS ak::gJ od pU f D&f!=&ee)ee=f;rz=gm.fetch_add(1,H memory_order_relaxed S size=f B;dj ni size)ph be=e tP f){dj Y b4 qF M nQ jn+=ri tL;be=M qs f0=0 lE kF=1;kF<size;kF*=2,++f0 D f0==dI B)dI.emplace_back()uD gj=dI[f0];gj mt size)lB la kB=kF*2 lE hc z hc<size;hc+=kB J mid qn hc+kF,size S end qn hc+kB,size F mid>hc){gj[mid uF=dj[mid uF lF i=mid-1;i>hc;--i)gj[i uF=kG qF dj[i uF,gj oO sC mid<end){gj[mid]=dj[mid]lF i=mid+1;i<end U i)gj oO=kG qF gj[i uF,dj oO se mf kF>size/2 mi oE bS ak::dn od mZ g8 sh tN g8 R dj qV h gj=hC(first^g8)-1 lt kG qF dI[gj]qV,dI[gj][g8]qM ak::aA od pos sh pos>ee B lM A pos==0 R true h fl=ee[pos uF,k=mQ rY lt fl<k||fl>=k+uk;}aR::aR od mA bu):ah(in),iJ(pd,fI(bu){nS aR::cA Q mZ end D tN end R;fP(fI.dn ut mT end-1)mY aR::fP ls bS&en){dl=dl?kG ut*dl,en):en;nS aR::fs Q tL{fP(b4 ut tL C nz aR::a0(sh!dl R 0.0 h&s=*dl mj e4=ah.jv(iJ,s nV pE s.bv||e4>s.bj||s.dZ<-1e-8||s.dY>ah.X[iJ].bd pV lC lt ah.cG(iJ,s nV)+s.cG;}nb aQ oP a7 mH e2{int e=-1 ph aw sl bT z}oP ab lV ab pj,bA bY,pe>bJ={}tk cN ma df);bA bY(lk lB ak&bu tl lk lB os&cz tj aD;ra ck tl mI e<aL B&&!sD lJ.empty(C la aJ tj qC rY qa e2&au om mI bK ou qs aj tj bK B qs ap tj qC pi qs g4(lk K c5 tj dW;}a7&bJ tj*db qa pe>&bc tj db ny aE om lk lB aB&ag tj*ah qs aI om lk lB mb&aT tj cH;nS a3(mb dm){cH=mz dm C nz a5 om lk;nz fK rm rj lk mx g3 om)mx ei Q pU G)mx insert rm rj)mx cC om E rj)mx fn Q a4 E aC E rj)mx gs Q pr)mx uv fT);us*ah;pe>db pc ak>aL sz aD pc e2>bK mH ii{ly d1 z nz L;}tm ii>dC;mb cH K dW z os cO;ak bL mM di Q pU f tk cq tl,ak&et mo L tk st L)ph cc om lk ph fz Q M ul ev tl C;nb a7 lV struct kk nJ*aw K*as K*jV K*kM rT explicit a7 Q G):gM(hG(G)),jj(4*gM){}la jG tj gM-2;}kk gz Q kt oQ*data=jj qd lt{data,data+gM,data+2*gM,data+3*gM};}ms static la hG Q G)lx 2*G+2 qs gM;ne jj rT nb aH lV aH od mA f):in_(in),iJ(pd,kh(f){nS dn Q mZ end D first<end)sd kh.dn(in_,mT end-1)mY M Q id){sd b4(in_,id)mY en ls bS&next){sd next qM bv(sh!dl R true h&s=*dl lt s.bv&&in_.jv(iJ,s nV)<=s.bj&&s.dZ>=-1e-8&&s.dY<=in_.X[iJ].bd pV;}ms void append ls bS&next D!dl){dl=next lt;}mJ s=*dl mj eX=in_.jv(s.g8,next nV)mj gH=s.bM mj gY=s no mj iB=s.hL sK bv=s.bv&&rQ bv&&iB+eX<=rQ bj sK bj qn s.bj,rQ bj-eX-gY);s no=gY+eX+next no sK hL uC iB+eX+next no,rQ hL)sK dZ qn s.dZ,gH+rQ dZ)sK dY uC s.dY,gH+rQ dY)sK bM=gH+rQ bM sK g8=rQ g8 qa aB&in_ ph iJ lB ak&kh;uB bS>dl rT oG d5{const os&W ph hJ sl g6 sg bx mq ph size tj W B-(bx?2:0 C la M Q a8 sh!bx R W[a8]A a8<hJ R W[a8]A a8<g6-1 R W[a8+1]lt W[a8+2]oE;pW bz od pU W E dh E b3 J aw=W[dh],as=W[b3]h dV=dh?W[dh uF:e A b3==dh+1)nJ iG=ml dV,rW as F b3+1<W B J kA tE,kA)-ml dV,kA mK iG;}lb gP=W[dh+1]h e7=W[b3 uF K iG=ml dV,rW gP)-ml dV,gP)+ml e7,as F b3+1<W B J fH tE,fH)-ml e7,fH mK iG;}pW bX sT E at E M rO fD=ri M qO+=ml M uc;}pW bB sT E at E aw E as rO fD=ri rW as qO+=ml as uc;}pW cZ sT E dh E b3 E aw E as D dh==b3 R bB oW,dh,aw,as tK bX oW,dh,aw)+bX oW,b3,as C lh oH ab mL in,bA bY,pe>bJ):ah(&in),db(bJ?mz bJ):H make_shared<a7>(uk+mQ rp)),aL(mQ rY),bK(uk+mQ rp),dC(bK B){aD ni mQ rY S gB=bY.fv(in);dW=gB.c5 lE k z k<bY.cb mR k pT!bY.cb[k]lU)di(k,bY.cb[k]mY oH cN ma df D this==&df R mP<aL mR pd mf sD sn)!=df.sD sn))sD.cN(df.sD);aD ni aJ sw aD sm aD;bK sm bK;dC sm dC;cH sm cH;dW sm dW;}bA oH bY(pm bA az;oy cb mt qC rY)nK e:aD)oy cb Z=mu lJ lt az qa ak&oH bu tl pm static const ak empty lt e<aL B?sD:empty qs oH g4 tj H count_if(bK oF bK tJ[](lb&au)lx au nh>=0;}C double oH aE ov->av ou uA:qC aZ[o-n]uA qs oH aI ov->cl mV:qC dg um-n C la oH cc ov->aW mV:qC a1 um-n mY oH di Q pU f rR in=*ah tP mu lJ)bK[fz(tL]={}uD kw=sD np fC=ck(e F f lU D fC){kw mX);aD mS nG,pd se else{kw.gJ oW pE fC)aD.my nG,pd,e C ev(e C la oH fz Q M qK k=qb,n=ap(),m=qC rp lt M<k+2*n?(M-k)%n:n+(qX n)%m;nS oH ev tl rR f=mu lJ lE p z p<f mR p J M=f[p],o=fz(tL uD qo bK ou;rI uE e F M==aI mV)rb=p nF sE=p oE nz oH a5 om qK qo au um F rI<0 lC tQ ns rI S&fh=mu uD bD=dC ou A qT d1==fh sn)R qT L h size=fh qD;aH af qc mE fh);nk 0,rb);nk rb+1,sE);nk sE+1,size);qT d1=fh sn);qT L=af qz?nz(bz qc mE fh oZ bT)-aE mV):rS lt qT L;}nz oH fK rm a E b sh o rn||au mV nh>=0||e>=qb lC h&ek=mu h size=ek qD tR o<qC pi A a>b||b>size||(aM&&a!=b)||!ek.aA qc,a)||!ek.aA qc,b)lC rJ aM&&qC aZ[o-qC pi].dq>qC oo lC;aH af qc mE ek);nk 0,a py M(aI mV pE aM)nk a,b py M(cc mV);nk b,size sP lB d5 jT{ek lJ}h cG=aM?bB qc sL a,aI mV,cc mV):cZ qc sL a,b,aI mV,cc mV tK aE mV-cG;ra oH g3 om J L=a5 um pE L lM h qo au mV tQ ns me pd;pL lJ;oe);dW+=*L;di oR f qt ei Q pU G D e>=qb||G lU lM h&old=mu pc bool>gc(old qD,pq K gV=0 tS G D o rn lM h qo au um F rI!=md pd||gc[rb]lM;gc[rb]=gc[sE]lZ;gV+=aE um C aR af qc mE old)uD f=cO;f mX)lE first z first<old qD;D gc qV){++first;lw}auto end=first+1;nL end<old qD&&!gc[end])++end;af.cA(mT end);f.my f tJ old tT mT old tT end);first=end;}lb jd=af.a0(pE jd lM;dW+=old qA-*jd-gV;di oR f qt insert rm a E b J L=fK um mE a,b pE L lM;pL lJ;tF);dW+=*L;di oR f qt cC om E a E b D o rn||au mV nh<0 lM tQ ns au mV nh S ia=mu qA;pL lJ h qo au mV;oe)tR o<pC a8 D!a8 R true h fl=f[a8 uF,k=qC rY lt fl<k||fl>=k sb rJ aM&&qC aZ[o-ap()].dq>qC oo lM;tF);ox a0(pE cG lM;cq oR bL,ia-*cG qt fn Q a4 E aC E a E b D a4 rn||aC rn||au(a4)nh<0||au(aC)nh>=0 lM h qo au(a4 S e=ns rI S ia=mu qA;pL lJ;oe)tR aC<pC a8)lx!a8||f[a8 uF>=qb sb;pa b,cc(aC nQ pa a,aI(aC nQ ox a0(pE cG lM mj L=aE(aC)-aE(a4)+ia-*cG rM cq oR bL,L)nB void oH cq tl,ak&et mo L){mJ kw=sD tP kw lJ)bK[fz(tL]={}np fC=!kw lJ lU np hQ=!et lJ lU A fC!=hQ J i0=nG,e F hQ)aD.my i0,pd nF aD mS i0 C H swap(kw,et pE hQ)kw mX);ev(pd;dW+=L;ra oH gs Q pr D first rn||second rn lM qN au(first),b=au(qP nh lM h hU sA a4 E aC J qo au(a4)pH W=bu(me pd lJ;of S bb=aC<ap()?rb:sE-1;W.my W ln+bb,cc(aC nQ W.my W ln+rb,aI(aC)tK W;}h f1=hU(oI),f2=hU(second,first);ak g9 qc,a nh,f1),ha qc,b nh,f2);aR e9 qc,a nh,g9),fa qc,b nh nx bu(a nh)qA+ug rM dr(a nh,mz g9),b nh,mz ha),L qt uv fT D bZ==b0||bZ>=qb||b0>=qb lM h&jr=bu(bZ S&js=bu(b0 F fS>jr qD||fT>js qD lM nK&loc:bK D(rI sF bZ)&&rb<fS&&sE>=fS)||(rI sF b0)&&rb<fT&&sE>=fT)lM;}os f1(jr lJ oF jr tT fS);f1.my f1 tJ js tT fT,js lJ lG)sz f2(js lJ oF js tT fT);f2.my f2 tJ jr tT fS,jr lJ lG);ak g9 qc,bZ,f1),ha qc,b0,f2);aR e9 qc,bZ,g9),fa qc,b0 nx jr qA+js qA-*gR-*gS rM dr(bZ,mz g9),b0,mz ha),L)nB void oH st L){ol mJ loc:bK pT rI sF bZ)||rI sF b0))qo{}h assign sA e,ak et oQ i0=nG,pd np hQ=i0!=aD lG&&*i0==e A et lJ lU D hQ)aD mS i0);sD mX ps{mf!hQ)aD.my i0,pd;sD=mz et C ev(e C;on bZ,mz g9 nQ on b0,mz ha nQ dW+=L;}oG ig{ly ki=1 K dw=59.0 mx bs lZ mx d7 lZ ph eH=1000000 ph eW=400 ph hv=400 K fX=2 K gf=140 ph eB=2 ph eQ=3 mx eZ lZ rT oG jz{bA bY rT jz kL pj lv ig&bV);lh using aO=nl mH ao{la o sl e sl aw sl as tr L tO dK{nm dp K em=-lg C mH e0{la e tr L tO cx{bool aM;bS aw,as K aE,dq;}oP aQ lV explicit aQ pj)mx hS om,oJ=aO pv)lk mx fu om mI f8 ou qa lc e0>&fQ om mI ft ou qa cx&dP om mI cK ou;}us*ah tm lc e0>>ft mN os hW tm bool>f8 pc cx>cK rT mb cp ma fV,oJ);nm bC nN ts f mo gX=-lg nQ class aN lV const nm&dp ma E o tk jM ma nd G tk g2 ma E pU G)lB nm&dp om mI qj oD;}rc jU{ly bo=mw<ly>pv);nm I rT mb ja ph eq sl bP z lc jU>cu mM gE ma tk d0 ma E o E pd mM d2 om C;nb aG lV explicit aG uj jF=200,ly ki=1 tk ib sG tk h6(oJ E ey=50000 nY gZ mq);bA fe(oJ,mb dm={}lv bA*b8 sB nY kf lZ nY im lZ E eh=2 nY dM lZ E eI=3 tk dE(oJ lv ab&fd tk iH Q jF)mx fE tj cQ==pX;ra b7(rU,oJ);rc ie{os f K aq z};using kP=H map<os,ie>mH Arc{la o K bU;}lB aB&ah ph gU,eO sl ee sg aY mq ph cQ z ne fY;ly jQ=1;ly is z ly fO z ly eG z lc kP>c1 pc ly>cn pc lc Arc>>kH;ne fb mx da mq;ly dy z mutable ly eV=mw<ly>pv)mN ne eA sz hb mM add ma E e nY cf tk fr Q pU G nd f mo aq nY cf);ne c7(oJ=aO::min()lk rT qQ aO cE);nb c2 lV virtual~c2 pQ;virtual os h2(nt&rng lk z};nb jx lV virtual~jx pQ;virtual bool b7(rU lk z};nb hh lV void eM(qH>move){gl Y mz move)qM h0(rU,nl am=nl pv)lk;ms lc qH>>gl rT nb ih lV rg kX>nm get rm aJ,ly aF,ly cP mo cj nY du,kX&&k2 D cu B<=o)cu mt o+1)uD row=cu ou A row B<aJ)row mt aJ)uD bD=row Z np hu=qT hN&&qT cP==cP&&qT du==du np kU=hu&&(du?qT aF==aF:(qE.has_value()||cj<=qT cj)pE kU){qE=k2();qT aF=aF;qT cP=cP;qT cj=cj;qT du=du;qT hN lZ oS qE&&!du oQ i3=*qE;i3 sx+=cj-qT cj lt i3;nq qE;}rc jU{nm I;ly aF sl cP tr cj sg du mq mx hN mq;}pc lc jU>>cu rT oG dR{ao I ph bg;}oP fG lV void sH nm I E bg D I&&un>ot gl Y{*I,bg}C uB dR>dp(sh gl lU lC lt*H max_element(gl oF gl mv I sx<b oD sx;}mY fJ Q pr){hM(gl,[&](lb&move)lx move.bg==first||move.bg==second||move oD nh==first||move oD nh==second;}mY clear(){gl mX C ms lc dR>gl rT nb ar lV ar oj;}nz L nC rj lk;nz bh nC rj lk;uB aH>eR nC bO lk;nz dL nC rj lv aH&ex sa d4,cY uG z bS jJ,ip ph b9 Q a8 lk mx ci ls ak&f E bR ul pf lk;}oP dx qY lv ar&uH ax lV ax oj;}nz L nC bR lk;nz bh nC bR sa cV uG z la b9 Q a8 ul pf lk;}oP cF qY lv ax&uH gv mU rU oa ms mutable aN fI mN os bP rT nb eL mU rU oa};nb dJ mU rU oa ms};oG bI{la a4;ao aX K L;}oP a6 final:public jx lV a6 pQ mx b7(ab&oa uB bI>d6 ma lk;rc Row{ly fm z ak dN K a5 z}tm Row>kK mH eF{ly b6 z uB bI>I;}mH ba{bI I;ly d1,b6;}mH nj aB*gr sB tm ly>ca tm bool>es uw mM cU ma E pU a4 nd aC ul eo ma ul gb(ul fW tl E aC ul f7(lk rT oG bl{la oI K L rT nz eJ ma E pr)oP cy final:public jx lV cy pQ mx b7(ab&oa uB bl>d6 ma lk;rc eF{ly b6 z uB bl>I;}mH ba{bl I ph bZ,b0;ly jR,jS,b6;}mH nj aB*ah sB tm ly>cn uw mM eo ma ul fW Q bZ E b0 lk rT lh jz kL pj lv ig&bV ru hc=qe S am=hc+lz(bV.dw)S cE=am-ud)nQ hh bs;pD eL>sw pD gv>sw pD cF>sw pD dx>()F bV.d7 oQ move=lS<dJ>();bs.eM(mz move)C pD cy>sw pD a6>sw sX);ab cr(ag,mz empty)S hH qn am,qe)+ud))nQ cr.a3(cp(cr,hH nQ aG fA(ag qG eW qG ki);ff(cr,fA,bV,cE);ab dp=cr;fA.ib(cr);fA.b7(dp,am);qU A bV.bs&&qe)oM 500)<am J h3=dp.c5();bs.h0(dp,am F dp.c5()>h3+ot fA.ib(dp);qU;}jz az;oy bY=dp.bY(tK az;}qQ aO cE ru&ag=cr.ag(S g7=qe S fZ qn cE,g7+lz(lT bV.fX qG dw*0.2))nQ hE.h6(fZ qG eH)np cv=hE.fE()np jO=!cv||oU>48 A jO J jm=qe S gL=lz(lT 3.0 qG dw*0.05))S ce qn cE-gL,jm+lz(bV.gf)nQ la kr z do{lb hA=oU>48?lT ce,nu(6)):ce h b8=cr.bY();ab iv(ag,hE.fe(hA,cr.aT(),&b8 qG bs qG d7 qG eB qG eZ qG eQ),cr.bc sw iv.a3(cr.aT sw cr.cN(iv F oU>48&&nu(2)<ce J iz qn ce,nu(6 nQ hE.b7(cr,iz)U kr;qU A nu(2)<ce J fN qn ce,nu(3)F kr%2)hE.dE(fN,cr)nF hE.h6(fN qG eH tt;qU rJ(kr%2))qU;qU oE}nL oU>48&&qe)<ce sC cv D!jO)hE.iH(lT bV.eW qG hv)C qU;}nb f4 mp f4 uI G(mn qf(ro oL;tu ro,rng)sz bx;bx ni count)tS G D bx B>=count mi A N g3 mV)bx Y o mK bx;}nb gO mp gO::h2(nt&pm qm I=mn K c6=-sf ua r J L qp a5(r F L&&*L>c6){I=r;c6=*L qR I mK bx;}nb fF mp fF uI bW ua sU r F bW lU R{}h j0=bW[sp bW B]rq nD h hj qp aI(j0 S cR sA r)lx r<uk?mQ av O rH.cR:pu r-uk]rH.cR;}mj iP=cR(j0);qm I=mn K f6=sf ua r D nO<0||!N a5(r)uo M qp aI(r)mj dB=mQ jv(hj,tL+mQ jv(M,hj)+tv iP-cR(r)F dB<f6){f6=dB;I=r qR I mK bx;}lh nT cx fg od o J n=uk tR o<n h aw=aM?sZ=aM?rv-n tK{aM,b4 qF aw),b4 qF as),aM tG uA,aM?0:pu o-n].dq}oE aQ::aQ pj):ah(&ag),ft(rG pi+rG rp),hW(ft B oL,f8(ft B,pq,cK(ft B){nf o z o<cK mR o)cK ou=fg(ag,o qM aQ::hS om,oJ sh f8 ou R true rq*ah h n=uk h aw=o<n?sZ=o<n?rv-n S aE=o<n tG uA lB ak empty uD fQ=ft ou lF&e=hW ou;e<mQ X mR e D am!=aO pv)&&pw lM;aR af(in mE empty py fs(aw py fs(as S cG=af.a0(F cG)fQ Y{e,aE-*cG}C px fQ oF fQ mv L>b sx||(a sx==b sx&&a nh<b nh C);f8 ou lZ nB mb cp ma fV,oJ oQ dm=H make_shared<aQ>(fV.ag())lE o z o<fV.aj()U o D!dm->hS um,am)ui nq dm;}nT void i4(dK&az,ao I D!tn||I sx>tn nM||(I sx==tn nM&&I nh<tn->pd D tn)oy em uC oy em,tn nM);tn=I pP oy em uC oy em,I sx C cx dP nN o rR in=nD h ap=pJ)tR o<ap lt{aM,b4 qF N aI mV),b4 qF aM?rv-ap)),N aE mV,aM?0:pu o-ap].dq};}dK hZ nN ts f lv cx&data,a7&ku mo gg=-sf nY jB mq){dK az h&in qp ag(F oK dq>mQ oo R az h cw=f qD h aw=oK aw.mT as=oK as nV h&W=f lJ h gz=ku.gz(cw)pH*ew=gz rH pH*dz=gz tM pH*co=gz.jV pF J be=pos?W[pos uF:e h iS=pos<cw?ri W og):0.0;ew og=ri aw)-iS+(pos<cw?ml aw,W og):0.0);dz og=ri as)rA);co og=ri rW as)rA up gD=gg K dd=sf K eC=sf pF){dd qn dd,co og pE oK aM){dd qn dd,eC+dz og);eC qn eC,ew og se mf oK aE-dd pV<gD R az pH*bt=gz.kM rJ oK aM){bt[cw+1]=sf lE tH bt og qn dz og,bt[pos+1]C nf a z a tw a D!oN a)uo fo oV lx mm gg,jB&&tn?tn nM:oy em C h gx=oK aM?co rV:lT co rV,ew rV+bt[a+1]F oK aE-gx pV<fo()rd aH ex oW);ex.dn(0,a);ex.en(oK aw pE ex qz qq A oK aM oQ af=ex;af.en(oK as);nk a,ta a,a,oK aE-co rV}ps{lb fv sA b mo fD oQ af=ex;nk a,b py en(oK as);nk b,ta a,b,oK aE-fD}C lE b=a;b tw b D!oN b)uo fD=b==a?co rV:ew rV+dz[b]A oK aE-fD pV<fo()rd fv(b,fD se nq az oE nm bC nN ts f mo gX D o>=mn||e>=N qb||oX>=0 lC lt hZ pk o mE f,dP pk o),N bJ(),gX tt.dp;nS aN::d0 nN o E pd{mJ bD=pb h&f to;qE oT F N ck(pd){qE=hZ pk o mE f,N aT()->dP mV,N bJ(),1e-9 tt.dp A qE&&qE nM<=ot qE oT ps{re&empty:N aT()->fQ mV pT empty nh==e D empty sx>ot qE=ao{o mE 0,0,empty sx}qr oE qT bo=f sn mY aN::d2 om){mJ jl=qj oD;jl oT)mP<eq U e rR I=pb oD A I&&(!jl||un>jl nM))jl=I;}nS aN::gE sG rR dm qp aT(F ja!=dm||eq!=N qb||bP!=mn){ja=dm;eq qp qb;bP=mn;cu.on bP*(eq+1),jU{}se const nm&aN::dp nN o){gE(ac F oX pO{qj oD oT tK dp um C ja qB bo()){d0 pk o,e C d2 mV lt dp um mY aN::jM sG nd G){gE(ac S&dm=ja tS G D oX pO lw dm qB bo())d0 pk o,pd;d2 um C nS aN::g2 nN pU G){re o:G D oX pO lw lb be=dp mV;d0 pk o,e S&et=pb oD A be&&be->e==pd d2 mV nF mf et&&(!be||et nM>be nM||(et nM==be nM&&e<be->pd))qj oD=et oE oG br{la e sz G K aq;}mH cW{lc int>bf;ne bH K aq sl hY z la W sl fk sg bQ mq mx fu mq rT su,oJ)mx bF();cW dk mW,oJ nY iY lZ);cW dT mW,oJ,ly ki=1 lv ne&bH={});lh nT rg kP,class kS>double hq(kP&hD,kS dQ oQ i1=hD oF second=hD lG lF it=H next(i1);it!=hD lG U it D dQ(it ng)<dQ(i1 ng)){second=i1;i1=it pP mf second==hD lG||dQ(it ng)<dQ(second ng)){second=it oE lb floor=dQ(second ng);hD mS i1 tK floor oE sI aG uj jF,ly ki):ah(ag),gU(jF),fY(oU,-sf),jQ(ki),c1(oU),cn(pX oL{nS sI add nN e nY cf rR W to lJ A W lU R h k qp qb,n=pJ)uD G=hb;G mX)K aE=0 tP W){la o A M<k+n)o=M-k nF mf pB ah.rp)o=n+qX n nF lw G Y o);aE+=N aE um C px ro S aq=aE-N mu qA;fr oR G,W,aq,cf mY sI fr Q pU G nd W mo aq nY cf D!mO aq)||aq<=tp uD hD=c1 Z A cf&&hD B>=gU&&aq<fY Z R h gd=hD.lower_bound(G F gd!=hD lG&&gd->tN G D gd ng V>=aq R;gd ng.f.on W oF W lG);gd ng V=aq pP{hD uu,G,ie{W,aq}C++dy rJ cf)fY Z=-lg F cf&&hD B>gU){fY Z=hq(hD,[](lb&a2)lx a2 V;}C nS sI h6(oJ E ey nY gZ){cQ=0 A pw R rq ah h n=uk,nr=n+mQ rp,nv=mQ gG()pH aV=gZ?c7(am):ne tU tb A gZ J ju=jP[eG%3 tf;}lb hs=oz eG++mH kl{la aw,as K hL,ed,jo,hp,dv,aE,dq;}pc kl>G;G ni nr)qg r D r<n rR p=mQ av O;G Y{mQ cl(r),mQ aW(r uz 0,sf,p.dv,p uA-nZ,0}ps{lb&p=pu r-n];G Y{mQ dg(r-n),mQ a1(r-n uz p tM.cR,p tM.ed,p.dv,p uA-nZ,p.dq}se lb m=mQ rp rJ da J eU=lf h go qn 0.6,0.25*rK(am-eU).count()S ef=eU+lR duration_cast<aO::duration>(rK(mm 0.0,go)nQ lc lc double>>iw;ne ji(nv,lg nQ lc mg gt(nv oL ph gh=0 lE l z l<pS 8,nv)U l D lf>=ef ui ne dB(mQ b5 ln+gh*nv,mQ b5 ln+(gh+1)*nv);dB[gh]z lc mg iZ(nv oL mx fu lZ lE iN z iN<nv U iN D(iN&63 nX ef){fu mq qr qs v=nv uq sf qk pT!iZ[u]&&dB[u]<ti{v=u;oc dB[u]oS v==nv ui iZ[v]=1 qk pT!iZ[u])dB[u]qn dB[u],value+tV v*nv+u]sC!fu ui gt[gh]=1 qk)ji[u]qn ji[u],dB[u]);iw Y mz dB))K i8=-1 qk pT!gt[u]&&ji[u]>i8){i8=ji[u];gh=u oE ne dA(nv*m oL lE M z M<nv U tL nf r z r<m U r){mJ jq=dA[M*m+r]h kQ=mQ a1(r)nK&d:iw){na kp=1e-8+64 ue*nv*mm{1.0,d[kQ],d[M]});jq uC jq,d[kQ]-d[M]-kp se qU pc lc Arc>>next(nv)lE M z M<nv U M D pw R uD kT=next[M]qg r J eX=tV M*nv+G O rH]h kp=8 ue*mm{1.0,tv G O.ed),tv eX)}S ed=G O.ed-eX pV+kp A ed>=-1e-8)kT Y{r,ed}C px kT oF kT mv bU>b.bU||(a.bU==b.bU&&a.o<b.o C C kH=mz next);fb=mz dA);da lZ;qU;}lb&next=kH h&dA=fb;cQ z lc mg jI tU,hX tU sz f,bW,key;f ni 2*nr);bW ni nr);key ni nr)mP<c1 mR e D pw mi h now=lf h bq=am==aO pv)?am:now+(am-now pl>(pX-pd ph fy z H map<os,rL,ie>>cT K g1=-sf ph fM=ey pH e8=bq mx d9 mq,gC mq ph c9=0 nK&arc:next Z rR q=G[arc.o]A q.dq<=mQ oo&&tV e sQ rH]<=q.ed pV)++c9;}lb hO oV(auto&&ky E g8 mo jv mo cG mo aE mo load E fx)->void{mf d9 R A fy>=fM||((fy&63 nX e8)){d9 tq;}++fy nA bW pT hX O&&jv+dA[g8*m+r-n]>G O.hp pV R rJ fx&&aE-cG>tW gZ?(cT B<gU||aE-cG>=g1):(c1 Z B<gU||aE-cG>=fY Z))){key.on bW oF bW lG);px key oF key lG pE gZ)fr oR key,f,aE-cG tt nF{lb dQ=aE-cG h gd=cT.lower_bound(key)np i7=gd!=cT lG&&gd->tN key rJ i7||gd ng nV<dQ)nJ aq=-cG nA key)aq+=r<n?mQ av O uA:pu r-n]uA A i7){gd ng nV=dQ;gd ng oA.f.on f oF f lG);gd ng oA V=aq pP cT uu,key,H make_pair(dQ,ie{f,aq})F cT B>gU){g1=hq(cT,[](lb&bD)lx bD nV;}se}}mf!mO jv)R pH rf rr B;ly j6=0 A gZ J jW=H partition_point(rr oF rr tJ[&](lb&arc)lx jv<=arc.bU;});rf ns jW-rr ln);j6=hs+e*0x94d049bb133111ebULL+g8 rE=(j6^(j6>>30))rE^=j6>>27;}lb d8=bW B,i6=d8+count h ka=i6&&(j6&1)?ns j6%i6):0 qL i6 U i J ko=ka?(i+ka)%i6:i A ko<d8 J r=bW[ko]rJ hX O uo&q=G O mj e4=jv+tV g8 sQ tM]rJ mO e4)||e4>q.hp pV rd hX O z f Y q tM);ky(ky,q tM,mm e4,q.jo)+q.dv,cG+mQ er[g8 sQ tM],aE,load-q.dq,fx-1);f rk);hX O=1 A d9 R;lw}lb&arc=rr[ko-d8]rJ gZ&&jv>arc.bU mi h r=arc.o A jI O uo&q=G O A load+q.dq>mQ oo rd na e4=jv+tV g8 sQ rH]A e4>q.ed pV rd const bool first=gZ&&bW lU A first J gK=lf A fy>=ey||gK>=bq){gC tq;}fM=fy+tA 1,(ey-fy)/c9);e8=bq==aO pv)?bq:gK+(bq-gK pl>(c9 C na iq uC e4,q.hL)+q.dv mj jE=cG+mQ er[g8 sQ rH];jI O=1;bW Y r);f Y q rH F r<n){f Y q tM S gT=iq+tV q rH sQ tM]A mO gT))ky(ky,q tM,gT+q.dv,jE+mQ er[q rH sQ tM],aE+q uA,load,fx);f rk ps{hX O=1;ky(ky,q rH,iq,jE,aE+q uA,load+q.dq,fx+1);hX O z}f rk);bW rk);jI O=0 A first){gC|=d9;d9 mq;--c9 pP mf d9 R oE;hO(hO mE 0,0,0,0 oL;cQ+=!d9&&!gC nK rt:cT)fr oR key,c oA.f,c oA V,false se ne sI c7(oJ qK nr=ah.pi+ah.rp A eV==dy&&eA B==nr){qU lt eA;}struct gN{const os&G K aq;}pc gN>al sz X lE t z t<c1 mR t)re rt:c1 td){al Y{key,c V});X Y t C os de(pt qf(de oF de lG oL;px de oF de rX uh pc mg bp tU,f5(pX oL K jq=0 lF j:de D f5 ty rd tz al pI uJ rd f5 ty=1 nA al pI qh 1;jq+=al pI V;}ne aV tU,cS=aV K dp=sf lE by z by<192 U by){ne ct(pX oL pc const gN*>dS(pX,nullptr)oY sM j sq pI uq c V nA c ob pY nZ A value>ct ty){ct ty=value;dS ty=&c oE lb i5 qJ)+nW ct oF ct tJ 0.0 F i5<dp){dp=i5;cS=aV;}os jZ tU nK c:dS pT c)pz c->G)++jZ O K kq=0 qg r pT nZ>0||jZ O>1 J g=ql;kq+=g*g oS kq==0||i5<=jq+1e-7||!mO i5)mi h iN=0.7*(i5-jq)/kq qg r)nZ qW nZ+iN*(ql)sC(bF()||pX<=96)&&lf oM 250)<am){lc br>kj mP<c1 mR pd{lc rL lv kP::value_type*>>eg nK&bD:c1 Z J rt=bD K rw-=cS O;eg Y{dQ,&bD}up rf bF()?eg B:pS 64 ok)oY sN j)kj Y{e,eg pI oA->mT eg pI oA ng V}up iR qn am,lf sR 2 nQ auto hf=el(kj,pX,nr,iR)uq nW hf oF hf tJ 0.0)nK&hD:c1)nJ ct=0 nK rt:hD)nJ rw-=hf O;ct uC ct,dQ C value+=ct;}qU A value<dp)cS=mz hf mK cS;}bA sI fe(oJ,mb dm lv bA*b8 nY kf nY im E eh nY dM E eI J nr=ah.pi+ah.rp;bA km A b8)km=*b8 nF km.cb mt pX);ab empty ut km pE dm)dm=cp(empty,lT am,lf sR 1)nQ empty.a3(mz dm nQ os G(nr qf(ro oL qu(oz is++)ph hn z ab ac=empty,dp=empty;aN bu;f4 hK;fF hw;gO ic pc qH>>cJ;qi eL>sw qi gv>()pE dM){qi cF>sw qi dx>()sC im)qi dJ>()pE dM)qi cy>sw qi a6>sw nL lf<am D hn D hn%8==0||N c5()<dp.c5()*0.995)N cN(dp S bx=tA 4,N g4()/12 F hn%4==1)hK rx mf hn%4==2)hw rx mf hn%4==3)ic rx nf i sl rf 2+sp tA 2,pX/12);i sN i J e=sp pX sz bx nA G pT nO sF pd)bx Y r);lX(!bx lU&&!N ei oR bx)se tu ro,rng)nA G D pw mi A nO pO lw lb I=bu.dp pk r pE I||un<=ot lw pR my I oB C ol;;D pw ui nm I nA G pT nO<0 rR je=bu.dp pk r F je&&je nM>tW!I||je nM>un))I=je oS!I ui pR my I oB sC kf&&hn%eh==0&&lf oM 250)<am){nf jC z jC<eI U jC){bool aS mq oY<cJ mR j){aS=cJ pI->b7(ac)||aS oS!aS ui}ol auto rZ)sH ac mE false sC N c5()>dp.c5()+ot{dp.cN(ac);qU;}++hn;}qU lt dp.bY(mY sI dE(oJ lv ab&fd oQ aV=c7(am)tb h ju=jP[fO%3 tf h nr=fd.aj(),n=fd.ap(),k=pX qu(oz fO++)ph iA=0 mP<k U e J now=lf A now>=am mi h bq=now+(am-now pl>(k-pd;bA b8;b8.cb mt k);b8.cb Z=fd.mu lJ pc rL lv ie*>>eg nK rt:c1 Z)nJ rw-=nZ;eg Y{dQ,&c}up rf pS 3 ok)tx kv{b8.cb Z}oY sN j)kv Y eg pI oA->f);ab ac ut b8,fd.bc sw N tB la jh=0 lE by z by<48&&lf<bq U by)nJ L=1e-8;nm aX;uB la>a4 sz bm qg r pT nO<0 D(r&uK pk r mE N mu,L+nZ F I&&un-nZ sV un-nZ;aX=I;a4 oT se os bW qg sU r)nA bW J iV qp a5(r F iV&&*iV+nZ sV*iV+nZ;a4=r;aX oT);bm mX sC lf>=bq mi h qo mk r)pH W to lJ;of);ak dN(ah mE W S hk=bz(ah mE N mu oZ bT)-N aE(r)lE aC z aC<nr U aC pT mk aC)nh<0 D(aC&uK pk aC mE dN,L-hk-nZ+aV[aC]F I&&un+hk+nZ-aV[aC]sV un+hk+nZ-aV[aC];a4=r;aX=I;bm=W oE}mf L<=1e-8 D++jh>8 ui b8.cb Z=kv[jh%kv B];ab jN ut b8,fd.bc sw jN.tB N cN(jN);bW mX)qg sU r pE bW lU){tu bW oF bW tJ rng);bW mt lT bW B,ns 1+sp 3)nQ N ei oR bW C lw}mf a4&&aX J r=aX->o h kQ=r<n?ah.aW(r):ah.a1(r-n);bm.my bm ln+aX->as,kQ);bm.my bm ln+aX->aw,N aI(r nQ bA af;af.cb mt k py cb Z=bm;ab aS ut af,fd.bc sw aS.tB N cN(aS ps mf a4){pR g3(*a4)ps pR my aX->o mE aX->aw,aX->as nQ sH ac mE pq U iA oE qU;nS sI ib sG){re rZ D cn Z==N mu sn)rd cn Z to sn);sH ac mE true C nS sI iH Q jF){gU=jF;tX fY oF fY tJ-sf)lF&hD:c1)nL hD B>gU J i1=H min_element(hD oF hD mv second V<b oA V;});hD mS i1)U dy oE bool sI b7(rU,oJ){re rZ)sH ac mE pq mH e3{os G,f K aq;}pc lc e3>>al(pX)so bf(pX sS,aU(pX sS pc mg bp(mn oL sz iu(mn oL,de;eO tr b1=0 mP<c1 mR pd{mJ row=pZ;row ni c1 Z B)nK&[key,a2]:c1 Z)row Y{key,a2.f,a2 V});px row oF row mv aq>b V||(a V==b V&&a.G<b.G C);eO+=row B pc mg iT(mn oL qL row mR i){pz row oO ob iT O=1 mx cr=!row oO.G lU nA row oO ob cr&=nO sF e F cr){la rf 0 ua r)count+=nO sF e F count==row oO.G B){sJ uE i);b1+=row oO V oE}nf r z r<iT mR r)iu O+=iT O;}os h5(pX oL mP sM pd re&a2:pZ)pz a2 ob h5 Z+=iu O-1 mP sM pd mf!pZ lU)de Y pd;rl de oF de rX h5 rV>h5[b];}S ds oV ls e3&a2){pz a2 ob mf bp O lM nB;ee z aY mq h iM=lf h fq=de B>48?lT am,iM sR 2)):am h fL oV ls os&ea){tX bp oF bp lG oL mP<aU mR e D sJ<0||H find(ea oF ea tJ pd!=ea lG qq nA pZ[sJ]qh 1 ny fw z lc int>dS(ea B sS,gy lF e:ea){gy Y sJ F sJ pO fw+=pZ[sJ]V ny dF=fw;qI Q mo)>hO sA ga mo aq uL&63 nX fq){aY tq oS ga==ea B D aq>dF+ot{dF=aq;gy=dS tI<ea mR d)re&a2:al[ea[d]qZ dF+tp tQ ea[ga]qL pZ mR i rR a2=pZ oO rJ ds(a2)rd dS[ga]uE i)nA a2 qh 1;hO sW+a2 V)nA a2 qh 0 A aY ui}dS[ga]=-1 rJ aY)hO sW C A lf<fq)hO(0 oL nF aY lZ A dF<=fw+1e-9 lM qL ea mR i)aU[ea oO]=gy oO;b1+=dF-fw nB mx tY nL aS&&!aY){aS mq lE a z tg)aS=fL({de rV,de[b]})||aS;}qU lE a z de B<=48&&tg)nf c=b+1;c<pA c)fL({de rV,de[b],de[c]});qU A eO>2000){lc br>jc pc H pair<la E>>iy so b8(aU B sS mP sM pd nf oh pZ mR i D sJ sF i))b8 Z uE jc B S&c=pZ oO;jc Y{e,c.G,c V});iy.emplace_back oR i up kE=pt>48&&!bF()?c7():ne{}h eK=pt>48?dT(jc,pt,mn,b8,am,jQ,kE):dk(jc,pt,mn,b8,am F eK.bH B==mn){eA=eK.bH;eV=dy;}b1=eK V;aY=eK.bQ;ee+=eK.W;tX aU oF aU lG sS lF id:eK.bf pT id>=0 J[e,dm]=iy[id];sJ uE dm C qU pP{ne aV(mn oL,cS=aV K il=sf pc lc double>>dN(pt)tx h4(pt)lE by z by<192&&!aY U by D pw){aY lZ qr ny i5 qJ)sz jZ(aV B oL lF e:de)nJ ct z int dS=-1 qL pZ mR i rR a2=pZ oO uq a2 V nA a2 ob pY nZ A value>ct){ct=value;dS uE i se i5+=ct A dS pO pz pZ[dS]ob++jZ O oS i5<il){il=i5;cS=aV ny kq=0 nn<aV mR r pT nZ>0||jZ O>1){na i9=ql;kq+=i9*i9 oS!kq||i5<=b1+1e-7 mi mj iN=0.9*(i5-b1)/kq nn<aV mR r)nZ qW nZ+iN*(ql)C qU;aV=mz cS)mj iU qJ)lF e:de){mJ fp=dN Z nK&a2:pZ)nJ oc a2 V nA a2 ob pY nZ;fp Y value C mJ ht=h4 Z;ht mt fp B qf(ht oF ht lG oL;rl ht oF ht rX fp rV>fp[b];}C tX bp oF bp lG oL;qI Q mo mo)>h0 sA ga mo aq mo gI uL&63 nX am){aY tq oS ga==de B D aq>b1+ot{b1=aq;aU=bf tI<de mR d)re&a2:al[de[d]qZ b1+tp K g0=aq+iU-gI lE d=ga;d<de mR d J e=de[d]K ct=0 lF i:h4 Z D dN Z oO<=0 mi A ds(pZ oO)){ct=dN Z oO qr oE g0+=ct oS g0<b1-1e-7 R tQ de[ga]lF i:h4 Z rR a2=pZ oO rJ ds(a2)rd bf Z uE i)K kN=gI nA a2 ob{bp O=1;kN+=nZ;}h0 sW+a2 V,kN)nA a2 qh 0 A aY ui}bf Z=-1 rJ aY)h0 sW,gI C A lf<am)h0(0,0 oL nF aY lZ oS b1<=N c5()+1e-9 lM;bA af;af.cb mt N qb)mP<aU mR pd mf sJ pO af.cb Z=pZ[sJ].f h kD=af.fv(ah);ab jf ut mz af),N bc sw jf.a3(N aT sw N cN(jf)nB lh nT struct c4{const lc br>&al tx kJ sz hy ph ir sl X;c4 ls lc br>&c E k E G):al(c),kJ(c B),X(k){lc int>dm(k+G sS oY<c mR j J add sA r D dm O<0){dm O uE ir++);hy Y r C kJ pI Y dm O C;sH c pI nh)nA c pI ob sH k+r se}oP e1{const c4*cs ph n_;ne dc,e5,eS,kR,iI sz e6 pc mg eb so cX K fR Q row E ux tC a2-nc]uq 0 nA pN kJ[a2])value+=tC r]rP;value+=iI O*tC r];nq value ny dN Q ux-eS[a2-nc]uq pN al[a2]V nA pN kJ[a2])pY eS O rP;pY iI O*eS O;nq value;nS kI Q cg E cI){ne fc(n_),row(n_)qL si i)fc oO=fR(i,cI S jD=fc[cg]mj iN=e5[cg]/jD,L=dN(cI)nn<si r){row O=dc[cg*n_+r]/jD;eS O+=L*row O;}nf oh si i pT i!=cg){e5 oO-=iN*fc oO A fc oO==0 qq nn<si r)dc[i*n_+r]-=fc oO*row O;}H copy(row oF row tJ dc ln+cg*n_);e5[cg]=iN;eb[e6[cg]]z e6[cg]=cI;eb[cI]=1 U fk;}public:la fk sg bQ mq;explicit e1 ls c4&eT):cs(&eT),n_(eT.ir),dc(n_*n_ oL,e5(n_,1),eS(n_ oL,kR(n_,1),iI(n_ oL,e6(n_),eb(eT.pt+n_ oL,cX(eT.pt sS{nf oh si i){dc[i*n_+i]=1;e6 oO=eT.pt+i;eb[e6 oO]=1;}nS kd(oJ J ub ph gA=0 qx{bQ tq qs cI=nc+n_ K L=1e-8 oY<nc+si j D eb pI rd na oc dN(j F value sV value;cI=j A gA>n_ mi uM ph cg=n_ K iN=sf qL si i J d=fR(i,cI F d<=ot lw lb jt qW e5 oO)/d A jt<iN-1e-9||(tv jt-iN)<=tW cg==n_||e6 oO<e6[cg]))){cg=i;iN=jt oE mf cg==n_||!mO iN)R;gA=iN<1e-9?gA+1:0;kI(cg,cI C nS io Q a2 nY include){na kV=include?-1:1,rhs=include?-1:0 h old=n_++;ne hd(n_*n_ oL qL old U i)H copy_n(dc ln+i*old,old,hd ln+i*n_)uq 0 qL old U i pT e6 oO==a2){oc e5 oO nn<old U r)hd[old*n_+r]=-kV*dc[i*old+r]qr;}hd[old*n_+old]=1;dc=mz hd);e5 Y rhs-kV*ti;eS Y 0);kR Y rhs);iI Y kV);e6 Y pN pt+old);eb Y 1);cX[a2]uE old);fk z nS i5(oJ J ub qx{bQ tq qs cg=n_ K jH=-1e-7 qL si i pT e5 oO<jH){jH=e5 oO;cg=i oS cg==n_ R ph cI=nc+n_ K jt=sf oY<nc+si j D eb pI uo d=fR(cg,j F d>=-ot lw na af=dN(j)/d A af<jt-ot{cI=j;jt=af uM;kI(cg,cI se ne fp(pm ne x(pN pt oL qL si i pT e6 oO<x B&&mO e5 oO))x[e6 oO]=H clamp(e5 oO,0.0,1.0 tK x ny iX(pm auto aV=eS K hF sl gk=0 nn<si r D!mO nZ)R sf;nZ qW nZ);hF+=nZ*kR O;gk+=tv nZ*kR O C ne hg(pN X oL oY<pN al mR j rR c=pN al pI uq c V nA pN kJ pI)pY nZ A cX pI>=0 J r=ns cX pI);pY iI O*nZ;}hg pK uC hg pK,value up iE=nW hg oF hg tJ 0.0);hF+=iE;gk+=iE rJ mO hF)||!mO gk)R sf lt hF+1e-7+1e-10*gk;}ne bH Q G pm ne az(G oL lE row z row<pN hy mR row J jg=pN hy[row]A jg>=pN X&&mO eS[row]))az[jg-pN X]qW eS[row]mK az oE;}su,oJ){pn qU lt kg.bH(G qM bF()lx false;}cW dk mW,oJ nY iY){cW az;sj=aU sv id]V A al lU){oy fu tq az;}pn oy fk=kg.fk A kg.bQ){oy bQ tq az;}oy hY=kg.iX(F iY)qU mx jb lZ nK&c:al)jb&=c V==H round(c V)pc mg jX(X oL,bp(G oL,hr(pt oL so bf(X,-1 S ds sA id sq[id]A jX pK||hr[id]lM nA c ob mf bp O lM nB;qI ls e1&mo)>hO oV ls e1&lp mo aq D oy bQ R A pw){oy bQ tq;}++oy W K iX=lp.iX(F jb)iX=H floor(iX F iX<=az V+tp h x=lp.fp()sz dt oY sM j pT ds(j))dt Y j F dt lU D aq rN=aq;sj=bf;nq;}rl dt oF dt rX x rV!=x[b]?x rV>x[b]:uh pH kN=bp,f5=jX pH fd=bf K af=aq lF id:dt sq[id]A f5 pK rd tz c ob j4&=!kN O rJ j4 rd f5 pK=1 nA c ob kN O=1;fd pK uE id);af+=c V oS af rN=af;sj=mz fd F iY)qU oS iX<=az V+tp ph hl=dt.front()K ge=-1 lF j:dt D x pI<=1e-7||x pI>=1-1e-7 uo dQ qn x pI,1-x pI F dQ>ge){ge=dQ;hl=j oE lb&c=al[hl];{auto ho=lp;ho.io(hl tt po;}jX pK=1 nA c qh 1;bf pK uE hl);hO(ho,aq+c V);bf pK=-1;jX pK=0 nA c ob bp O z}mf!oy bQ oQ ho=lp;ho.io(hl,pq po;}hr[hl]=1;hO(ho,aq);hr[hl]z}};hO(kg oL;oy fu=!oy bQ lt az;}cW dT mW,oJ,ly ki lv ne&bH){cW az;sj=aU;oy hY=sf sv id]V tx ik(X)oY sM j)ik[al pI nh]Y j);ne fj(pt,0 pE bH lU)nf j z j sM j)pz al pI ob fj pI+=bH O lj na ju:{0.0,0.5,1.0,1.5}){os dt(pt qf(dt oF dt lG oL;px dt oF dt tJ nP b J x=al rV V-ju*fj rV,y=al[b]V-ju*th;})pc mg bp(G oL so fd(X sS K aq=0 lF j:dt sq pI A fd pK pO lw tz c uJ rd fd pK uE j)nA c qh 1;aq+=c V oS aq rN=aq;sj=mz fd);qU oE ol mJ hD:ik)px hD oF hD tJ nP b J x=al rV V-fj rV,y=al[b]V-th;})qu(ki S hc=lf ph hz sl ix z nL lf<am){lc int>iD(G sS lE t z t sO t pT sj td pO pz al[sj td]ob iD O uE t)lB la kO qn X,ns 8+4*((hz/X)%4)nQ os ea pc mg hT(X,0 S add sA t D!hT td){ea Y t);hT td=1 oE h first=hz%X;sH first pE ik qV lU J k0=hz%3==0?sp pS 24,ik qV B):0 nA al[ik qV[k0]]ob mf iD O>=0&&ea B<kO)sH iD O C nL ea B<kO){ne dQ(X oL lF t:ea J old=sj td>=0?al[sj td]V:0 h rf pS 32,ik td B)oY sN j sq[ik td pI]h oc mm 1.0,c V-old)rB dQ[iD O]+=value oE la dS=X K ct=0 lE t z t sO t pT!hT td&&dQ td>ct){ct=dQ td;dS=t oS dS==X||hz%5==4){do{dS=sp X;}nL hT[dS]C sH dS C lc br>jY sz j5 so b8(ea B sS lE t z t<ea mR t)ol auto id:ik[ea td]sq[id]mx ij lZ rB{ij mq qr oS!ij qq A sj[ea td]sF id))b8 td uE jY B);jY Y{t,c.G,c V});j5 Y id up now=lf h hI qn am,now oM 150)S eK=dk(jY,ea B,G,b8,hI,pq;oy W+=eK.W;oy fk+=eK.fk K old=0 lF id:b8 pT id pO old+=jY[id]V A eK V>old+ot{nf t z t<ea mR t)sj[ea td]=eK.bf td>=0?md j5[eK.bf td]):-1;az V+=eK V-old;ix=hz;qU;}++hz A hz-ix>12*X ui}qU;oy bQ=pw lt az;}lh bool hh::h0(rU,nl am pm bool aS mq lj int jC z jC<2 U jC){bool dX mq nK&move:gl D pw R aS||dX;dX=move->b7(ac)||dX oS!dX ui tY nq sr bool gv nE mf bP B!=mn){bP mt mn qf(bP oF bP tJ 0 C fI.jM pk bP)ss nm dp tS bP D oX pO lw lb move=fI.dp um F move&&move nM>tW!dp||move nM>dp nM))dp=move oS!dp ui pR my dp->o,dp->e,dp->aw,dp->as nQ tY fI.g2 pk dp->e,bP mK sr bool eL nE lc uB double>>iA(mn)pg)iA ou qp a5 mV ss uB la>dp K c6=1e-9 pg D iA ou&&*iA ou>c6){dp=o;c6=*iA ou oE mf!dp mi tQ ns mk*dp)nh);pR g3(*dp nQ tY iA[*dp]oT)pg pT oX sF pd)iA ou qp a5 um mK sr ax::ax sc,cV(0 D o>=pJ)rC pd;cV=rb;hB lZ;bE qp a5 mV h&f qp bu(rD ax::b9 Q a8 mI a8<cV?a8:a8+2;nS ax::pf qK gi qn end,cV uf S ej uC mT cV tc 2 C nz ax::bh uN qD-(pp,cV,cV+1,e==ch}h fD=bB(qv)sL bR,c0.aI(dD),qv).aW(dD)tK bw-fD;}nz ax::L uN qD-(e==ch?2:0)lC rq qv)np ow:0 F iF D bR&&!oN b9(bR-1)+1)lC pP mf!bE||!oN bR)lC;aH af oW F qw 0,bR)nF nk 0,bR py M(c0.aI(dD nQ af.M(mQ aW(dD)F qw bR,cd)nF nk bR,cd sP lt bh oR f,bR C nm cF::cB sG lv ax pG)lE a z a tw a J gn=b2.bh rs pE gn||*gn nR L=b2 sx rs rF ao{r mE a,a,*L};nq bn;}qM cF qy R;ax op nh,fU sy)nn<pJ)oC)nn<pJ)oq sr ur ar sc,d4(0),cY(0 D o<pJ)||o>=mn rC pd;d4=rb;cY=sE;hB lZ;bE qp a5 mV;jJ=b4(nD,N aI mV);ip=b4(nD,nD.a1 um-pJ))S&f qp bu(rD ur b9 Q a8 sh a8<d4 R a8 A a8<cY-1 R a8+1 lt a8+2;ra ur ci ls ak&f E bR mI!bR||f.aA(qv),b9(bR-1)+1 mY ur pf qK gi qn end,d4 uf S gW uC mT d4 S hV qn end,cY-1 F gW<hV)nk gW+1,hV+1 S ej uC mT cY-1 tc 2 C nz ur bh nC a E b oi qb||a>b||b>f qD-(pp,d4,cY,e==ch}h fD=cZ(qv)sL a,b,c0.aI(dD),qv).a1(dD-c0.ap())tK bw-fD;}nz ur L nC a E b qK ex=eR rs tK ex?dL rs,b,*ex):rS;}uB aH>ur eR nC a oi qb lC np ow:0 F a>cd lC rq qv S dq=pu dD-c0.ap()].dq A dq>mQ oo lC A iF D!ci(f,a)lC pP mf!bE||!oN a)lC;aH af oW F qw 0,a)nF nk 0,a py en(jJ sP lt af;}nz ur dL nC a E b lv aH&ex pm const bool ow:0 F a>b||b>cd lC rq qv F iF D!ci(f,b)lC pP mf!oN b)lC pH af=ex A qw a,b)nF nk a,b py en(ip F qw b,cd)nF nk b,cd sP lt bh rs,b C nm dx::cB sG lv ar pG S&in=nD h qo mk r)lB d5 jT{f oZ bT,e==bg}h aI qp aI(r S f9=mQ a1(r-pJ nQ ne f3(cw+1),eP(cw+1),eN(cw+1),bt(cw+2,sf)pF){f3 og=bX(in sL pos,aI);eP og=bX(in sL pos,f9);eN og=bB(in sL pos,aI,f9 C nf tH bt og qn eP og,bt[pos+1])lE a z a tw a J gx qn eN rV,f3 rV+bt[a+1]F b2.cD()-gx nR ex=b2.eR rs pE ex qq lE b=a;b tw b J fD=b==a?eN rV:f3 rV+eP[b]h gn=b2.cD()-fD A gn nR L=b2.dL rs,b,*ex rF ao{r mE a,b,*L};}nq bn;}qM dx qy R;ar op nh,fU,0 sy)lE r=pJ);r<mn oC)lE r=pJ);r<mn oq sr nT struct jA{la e sz hR rT os g5 nN e rR in=nD h&f to h k nw ph fi z os kW{0}lE pos z pos<f lJ mR pos J M=f lJ og A pB m)++fi uy--fi rJ fi&&oN pos+1))kW Y pos+1 mK kW oE bool dJ nE lb&in=nD ss lc jA>cb;cb ni N cz()B)nK rZ)cb Y{e,g5 pk pd})mx gw mq qL cb B&&!gw U i){nf j=i+1;j<cb B&&!gw U j J bZ=cb oO nh,b0=cb pI nh h&a qp bu(bZ S&b qp bu(b0 S h8=a qD,h9=b qD K c6=1e-9 ph j1 sl j2=0 nK fS:cb oO.hR){re fT:cb pI.hR J kb=fS?a lJ[fS uF:bZ h kc=fT?b lJ[fT uF:b0 K L=0 A fS<h8 J j8=a lJ[fS];L+=ml kb,j8)-ml kc,j8 sC fT<h9 J j9=b lJ[fT];L+=ml kc,j9)-ml kb,j9 sC L<=c6 pV rd aH g9 qF bZ,a);g9.dn(0,fS F fT<h9)g9.en(b.dn qF fT,h9-1)pE g9 qz rd aH ha qF b0,b);ha.dn(0,fT F fS<h8)ha.en(a.dn qF fS,h8-1)pE ha qz rd c6=L;j1=fS;j2=fT oE mf c6>ot{pR c8(bZ,j1,b0,j2 nQ aS=gw lZ oE}mf!gw R aS oE lh uB bI>a6::d6 sG pm eo(ac);gb(tK dG lU?rS:lq(dG.top()oD mY a6::gb(pm nL!uO(S e=qE.aX nh h aC=qE.aX.o A es[aC]&&qT d1==ca Z&&qT b6==bG Z[aC].b6 ui dG.pop(C ra a6 ry&b sh a oD sx!=b oD sx R a oD sx<b oD sx h key=[]ls bI&c)lx H tuple(c.a4,c.aX.o,c.aX rH,c.aX tM C lt key(a oD)>key(b oD mY a6::cU nN pU a4 nd aC sh aC lU R uD row=bG Z tS aC){++row ou.b6;row ou oD oT up&he to sz W tS a4 J qo mk o)uD gQ=kK ou A gQ.fm!=he sn)){W=he lJ;of);gQ.dN.gJ(nD mE W);gQ.a5=bz(nD mE he oZ bT)-N aE mV;gQ.fm=he sn C re hx:aC){mJ aP=row[hx]mj fo=sk?sk nM:1e-9 h aX=bC pk hx mE gQ.dN,fo-gQ.a5 F aX&&aX nM+gQ.a5>fo)sk=bI{o,*aX,aX nM+gQ.a5}oE re o:aC)fW oR o mY a6::fW tl E aC qK&aP=bG Z[aC]A sk){dG.push({*sk,ca Z,aP.b6}C nS a6::eo sG qK X qp qb,G qp aj(F gr!=&nD||ca B!=X||es B!=G){gr=&nD;ca.on X,mw<ly>pv nQ es.on G,pq;bG.on X,lc eF>(G nQ dG=iO{};kK mt G C lc lD>>a4(X)sz aC,eD lE o z o<G U o J qo mk o)np jk=rI<0 A jk){aC Y o pE es ou)eD Y o ps a4[rI]Y o);es ou=jk;}nf e z e sO e J bo to sn F ca Z!=bo){ca Z=bo;cU(ac mE a4 Z,aC ps mf!eD lU){cU(ac mE a4 Z,eD se mf dG B>2*X*G+32)f7(mY a6::f7(pm dG=iO{}mP<bG mR pd nf aC z aC<es mR aC D es[aC])fW oR aC C ra a6 qS fn(I->a4,I->aX.o,I->aX rH,I->aX tM nQ tY nq sr nT bool j3 ls bl&a lv bl&b)lx a sx>b sx||(a sx==b sx&&H tie(a.mT a oA)<H tie(b.mT b oA)C nz ez nN a4 E aC J qo mk a4 S e=ns rI S&f to rq nD tR aC<pJ S as=aM?mQ aW(aC):mQ a1(aC-pJ nQ aR af oW py cA(0,rb py fs(N aI(aC)F aM)af.fs(as py cA(rb+1,sE pE aM)af.fs(as py cA(sE+1,f qD tK af.a0(se nz eJ nN pr D first>=mn||second>=mn lC qN mk first),b=mk qP nh lC h gR=ez pk oI pE gR lC h gS=ez pk second,first pE gS lC lt N bu(a nh)qA+N ug;ra cy ry&b mI j3(b oD,a oD mY cy::fW Q bZ E b0 qK&aP=bG[bZ][b0]A sk)dG.push({*sk,bZ,b0,cn[bZ],cn[b0],aP.b6}mY cy::eo sG qK X qp aJ(F ah!=&nD||cn B!=X){ah=&nD;cn.on X,mw<ly>pv nQ bG.on X,lc eF>(X nQ dG=iO{};}os aS pc bool>kC(X,pq mP sO e J bo to sn F cn Z!=bo){cn Z=bo;aS Y pd;kC Z lZ oE mf aS lU R tx G(X)pg J e=oX A e pO G Z Y o C re e:aS){nf df z df sO df D df==e||(kC[df]&&df<pd uo bZ qn e,df),b0 uC e,df)uD aP=bG[bZ][b0]U aP.b6;sk oT)nK first:G[bZ]){re second:G[b0]J L=eJ pk oI pE L||*L<=ot lw const bl I{lT oI),mm oI),*L}rJ sk||j3(I,*sk))sk=I oE fW(bZ,b0 se mf dG B>2*X*X+32){dG=iO{}lE a z a sO a)nf b=a+1;b sO b)fW(a,b se uB bl>cy::d6 sG pm eo(ac);nL!uO(F qT jR==cn[qT bZ]&&qT jS==cn[qT b0]&&qT b6==bG[qT bZ][qT b0].b6 R qE;dG.pop(mK rS;ra cy qS gs(I->mT I ng nQ tY nq aS oE int main(J kx=lf;H ios::sync_with_stdio(pq;H cin.tie(nullptr S ag=bk::gp(H cin);bk::eu::ig bV;bV.ki=1ULL mj kn=rK(lf-kx).count()mj hi=300 mj reserve=hi>2?1.0:hi*0.1;bV.dw=hi-kn-reserve A bV.dw<=0){bk::sX uP,empty,ag ps{lb az=bk::eu::kL(ag,bV uP,oy bY,ag mK 0;}
