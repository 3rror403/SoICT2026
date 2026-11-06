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
#define ld std::size_t
#define le const auto
#define lf std::vector<
#define H std::
#define z .size()
#define li H chrono::steady_clock::now()
#define lj H numeric_limits<double>::infinity(
#define lk }namespace bb::dV{
#define Q )return
#define lm ;for(
#define ln static_cast<
#define lo )const
#define lq .push_back(
#define f ;if(
#define ls .begin()
#define lt H optional
#define N ;double
#define lv (const
#define lw ;return
#define lx double>
#define ly ,const
#define lz {return
#define lA H uint64_t
#define lB )continue;
#define lC Q H nullopt
#define lD H chrono::
#define lE lf ld
#define lF lm ld
#define lG lm auto
#define lH const double
#define F ){if(
#define lJ .end()
#define lK lD duration_cast<eV::duration>(lD duration<lx
#define K ){le
#define L ad.
#define A );}
#define B ,ld
#define lP .X()
#define D =0;
#define lR [e]
#define o ;le
#define lT Q false
#define lU H make_unique
#define lV H min(
#define lW .empty()
#define lX {public:
#define lY ;const bool
#define lZ (void)
#define S ;++
#define mb H numeric_limits<
#define mc lv ab&
#define md H shared_ptr<const aL>
#define me (ld e ly ak&C B
#define mf =true
#define W if(
#define mh unsigned char>
#define mi ln int>(
#define mj )break
#define mk in.cu(
#define ml L ai()
#define U (ld
#define G )f
#define mo bI(e)
#define mp ,double
#define mq loc.
#define mr final:public cS lX lE>h4(ab&B,H mt19937_64&lo override;};lk lE>
#define M e,
#define mt =false
#define mu .resize(
#define mv H max(
#define mw H move(
#define mx lJ,[](le&a,le&b)lz a.
#define my );read(d6,
#define mz &bW B r B e ly ak&C,lA aC lo{le ca=ln ld>(L as(r).e G e!=ca&&!bW.eT()lC lw hk.get(r,M L aK(),aC,C.bn(),bW.
#define mA private:
#define mB lv az&
#define R [r]
#define mD (h)
#define mE ;bool
#define mF ;struct
#define mG lo lz
#define mH for U
#define mI A void
#define mJ auto&
#define mK A return
#define mL ;mutable
#define mM insert(
#define mN H isfinite(
#define mO lF e D e
#define mP Z z,n=in.ar z,m=in.a1 z
#define mQ second)
#define mR final:public jD lX bool b2(
#define mS lv lf bk>&al B Z B E ly lf int>&aN
#define mT clear()
#define mU ly lE>&
#define mV lf lx
#define mW ;void
#define mX z S
#define mY .reserve(
#define mZ first,
#define na lk class
#define nb .erase(
#define nd lD steady_clock::time_point
#define ne cT{bool operator()lv a2&ly a2&lo;};using iP=H priority_queue<a2,lf a2>,cT>mL const
#define nf lt<ao>
#define ng (in,
#define nh ;else
#define ni data.
#define nj ab&ad B count,H mt19937_64
#define nk eV::now()+lD seconds
#define nl g4);e7.cA(0,fV z);e8.cA(0,fW z)o gM=e7.a0(),gN=e8.a0(G!gM||!gN lT;lH O=
#define nm ;}double
#define nn (ad,
#define no lG r:
#define np (*ah,
#define nq );af.
#define ns lw true;}
#define nt }return
#define nu second.
#define nw as mD.e
#define nx while(
#define ny ;const
#define nz ln ld>(
#define nA lF r D r
#define nB in.
#define nC namespace bb
#define nD {double
#define nE lm le
#define nF ->O
#define nG lt<lx
#define nH L ag()
#define nI B h B e ly ak
#define nJ [&](auto a,auto
#define nK e==
#define nL ));
#define nM +1e-8
#define nN ax.
#define nO namespace{
#define nP mB in B
#define nQ lo override;
#define nR H accumulate(
#define nS )==0&&li>=
#define nT ::b2(ab&ad
#define nU .E)
#define nV L as(r).
#define nW ,ea z);H partial_sort(ea ls,ea ls+count,ea mx first>b.first;
#define nX for(
#define nY value=
#define nZ U h
#define oa aO R
#define ob C nb C ls+mq b0);C nb C ls+mq aw
#define oc X nb X ls+mq b0);X nb X ls+mq aw
#define Y )o
#define oe first B
#define of M C
#define og [pos]
#define oh i D i<
#define oi lo{W!ig()||e>=de.
#define oj assign(
#define ok template<
#define ol lE>
#define om lk struct
#define on }void
#define oo A else
#define op mc ad
#define oq ,bool
#define os nZ lo{le n=ah->ar z lw h<n?ah
#define ot !L mM I->h,I->M I->aw,I->aq nL add nn I->M false
#define ou >C lP z-(nK dc?2:0)||(e!=dc&&!cy)lC ny dY jY{C lP,cP,
#define ov [h]
#define ow jV+0x9e3779b97f4a7c15ULL*
#define ox iX=nK dc o b9=C lP z-(iX?2:
#define oy 1e-9)
#define oz ;}}
#define oA )W all||nK first||nK mQ iT.add(b6 nn bW,h,M L mo,aC
#define oB ::max(
#define oC ls,
#define oD .I
#define oE .V
#define oF ,0)
#define oG ab::
#define oH aI am
#define oI ;}W
#define oJ +lD milliseconds(
#define oK e)
#define oL [i]
#define oM lo{
#define oN first)
#define oO if constexpr(
#define oP ;}else
#define oQ .reset(
#define oR ag.Z z
#define oS lF j D j
#define oT C.mM C ls+
#define oU cn[h*eh+e]
#define oV nM<mv 1e-9,bh?bh nF:oy lB
#define oW ;lf
#define oX ar z
#define oY H shared_ptr<aZ
#define oZ lF h D h<ml S h
#define pa =L
#define pb =[&]
#define pc )/ln aI::duration::rep
#define pd bC.gD np of);bT af np M bC nq cA(0,C z Y cu=af.
#define pe const cU eQ(al,Z,E);eY kn(eQ);kn.kk(am);
#define pf ;hl.i6(am);nN fh+=hl.fh f hl.bF){nN bF mf lw
#define pg &bW nI&C,lA aC)lz b6(bJ,bW,h,of,aC A A lk
#define ph false)
#define pi next.
#define pj al z
#define pk ){auto
#define pl li>=am
#define pm af.dy(
#define pn nX auto r:
#define po c5 z&&!aS S
#define pp J>=k+2*n&&J<k+2*n+
#define pq aB(G a>b||b>C z||(bd&&a!=b)lT o ay pb U
#define pr ;ld
#define ps mB ag
#define pt [j]
#define pu H sort(
#define pv bl.eI(lU<
#define pw nA<nr S r)
#define px [c.e]=
#define py mz c9(),nK ca,[&]{nf bh o cp=C lP z-(nK ca
#define pz G!
#define pA e mU
#define pB A bool
#define pC lF pos D pos<=cp S pos
#define pD ;auto
#define pE mJ C=cC;C=mo
#define pF nC{void ey(H ostream&out ly bq&bQ ly az&
#define pG ;class
#define pH inline double
#define pI mq aw
#define pJ cR z
#define pK ;break
#define pL cl->
#define pM ()=default
#define pN H min<ld>(
#define pO value-=
#define pP al lR
#define pQ aK()
#define pR .data()
#define pS Z lR.a8
#define pT eV::now(
#define pU );H iota
#define pV C.ay ng
#define pW nU bi R=
#define pX cx lq lU<
#define pY cn[bE*eh+h]
#define pZ lF u D u<nv S u
#define qa ln lx(j4 R)-1
#define qb ol bo;bo mY count);nx bo z<count){ld
#define qc >=0
#define qd <=1e-9
#define qe )continue
#define qf ;}ld
#define qg )ns bool oG
#define qh de.ag(
#define qi ;H mt19937_64 rng
#define qj e:L cM()
#define qk iX)cf(af,
#define ql lF bp D bp<10000 S bp F(bp&7 nS am)
#define qm )W
#define qn .bG()
#define qo .cu()
#define qp ->iw mD mO<eh S oK W oU.bn!=L mo.
#define qq lJ,
#define qr Z z
#define qs a1 z
#define qt oM le
#define qu mq b0
#define qv bt.
#define qw bt oD
#define qx lB le
#define qy nX le
#define qz =0,
#define qA ,bN.
#define qB H unique_ptr<jD
#define qC H function<void
#define qD =nR aO oC aO qq 0.0)
#define qE G at<C z K next=C.J(at);fA
#define qF lk void fd(ab&bJ,aD&iT ly ij&bN,
#define qG oz W I==ml||!L hw(I)mj;bo lq
#define qH lG id:aN qm id qc)nN V+=al[id]oE
#define qI nT oM bool aP mt;nx le I=dZ(ad)){
#define qJ =lV
#define qK lZ 0
#define qL ;}const
#define qM [first]
#define qN loc pa as
#define qO =mv 0.0,
#define qP oh n_ S i
#define qQ J-k-2*
#define qR mR ab&nQ mA mutable ik hk;nf b6 mc
#define qS as(second G a.e<0||b.e<0||
#define qT ]qm dh(aV)){hI+=aV oE pK oI hI<=
#define qU .aw
#define qV }bool
#define qW ah->
#define qX mA struct
#define qY second->
#define qZ count=
#define ra H istream&
#define rb s.duration
#define rc mk a9,
#define rd bD B a3
#define re f9->nu
#define rf .pop_back(
#define rg H stable_sort(
#define rh nZ B e B
#define ri >=ai()
#define rj o&in=
#define rk next[hG]
#define rl lZ(!L
#define rm &[key,c]
#define rn ){using eV=lD steady_clock o
#define ro dJ=c oE no key)dJ
#define rp .h4 nn bo,rng)nh
#define rq ::cT::operator()lv a2&a ly a2
#define rr da.mT lG&ge:dA)ge.mT;jr D go
#define rs -iS+(pos<cp?mk aq,X og):0.0
#define rt *0xbf58476d1ce4e5b9ULL;kc
#define ru G O&&(!bh||*O>bh nF))bh=
#define rv ag.
#define rw lP z
#define rx f!
#define ry op B
#define rz =0 N
#define rA .bn(
#define rB lD duration<lx
#define rC H pair<double
#define rD >nN V+oy{nN V
#define rE nP J K k=nB mP f J<k+2*n Q
#define rF K a9=at?C.J(at-1):e N
#define rG ,cp G af qn)i5(ax,{h,M a,
#define rH f cN[aV]qc K r=nz cN[aV]);
#define rI (nL
#define rJ H nullopt
#define rK continue;
#define rL .O
#define rM ;};
#define rN ab&ad
#define rO [a]
#define rP no c nU
#define rQ aw)+mk aw,
#define rR +aB(A rx ay(a)||!ay(b)lT
#define rS lj)
#define rT D bool
#define rU oM W
#define rV K&
#define rW lower_bound(
#define rX nN ba
#define rY aR oD
#define rZ =c6.
#define sa loc=as
#define sb oW int>
#define sc rng()%
#define sd rV c=al
#define se mE aP mt lm;;){
#define sf dg U bR,ak g3 B bS,ak g4 mp
#define sg mV ee lv lf bk>&al B Z B E
#define sh (mG
#define si (h
#define sj H shuffle(E oC E qq rng
#define sk rP W iG R qc&&!hU[iG R]
#define sl ok class
#define sm ;ol
#define sn A}
#define so pb U
#define sp =nullptr
#define sq A W
#define sr aG lR
#define ss ,true
#define st L nw
#define su aN lR
#define sv nB a1[
#define sw nq dy(
#define sx M jY,
#define sy aB()
#define sz <al mX
#define sA <count S
#define sB <Z S
#define sC pz af qn lC
#define sD *nv+q
#define sE +lD seconds(
#define sF (of,a
#define sG nP e ly dY&C
#define sH e=nz mq e Y
#define sI nV e qc)bO lq
#define sJ >O){O=
#define sK (f5+1,V
#define sL f second<oN H swap(mZ
#define sM bq empty;empty.b5 mu oR
#define sN ;nx cg<iY&&ln mh(hi[cg
#define sO ;lH jU[]={1.0,0.8,1.2
#define sP .aq
#define sQ [t]
#define sR my p qU.cF my p qU.d7 my p
#define sS qo-*gM-*gN f O qd lT;
#define sT ]lG&kB:aO)kB*=jz;qK
#define sU a<po a)mH b=a+1;b<po b
#define sV fg[b]lw x!=y?x>y:a<b
#define sW value)
#define sX first=
#define sY ng of
#define sZ lF oh
#define ta mL lf
#define tb .e
#define tc nN df
#define td pa mo
#define te aD::
#define tf mf lw
#define tg ;}le
#define th oe second F
#define ti H abs(
#define tj <=cp S
#define tk oW lE>>
#define tl ;dX(M
#define tm [Z pt]
#define tn lB bool ka mf
#define to ,-1)
#define tp H max<ld>(
#define tq aW(fb.aU rI
#define tr c3[row*n_+
#define ts =X[bX+1];iI+=mk aq
#define tt oT b,b7 mD);oT a,aJ mD
#define tu pos=cp+1;pos-->0;)
#define tv ;nt nm hI=V lF d=f5;d
#define tw D}
#define tx mj;
#define ty )lw
#define tz next)
#define tA first==
#define tB nB oX
#define tC )lz
#define tD :public
#define tE +1e-9
#define tF nE J:
#define tG nE h:
#define tH lP ls+
#define tI A le
#define tJ nn h
#define tK I nF
#define tL nB bY
#define tM N nY
#define tN mj o
#define tO unsigned long long
#define tP ni aE-
#define tQ (nr oF
#define tR qq nJ b
#define tS add nn M
#define tT H fill(
#define tU aP mf;
#define tV nc+n_
#define tW mb lA>oB
#define tX nA<ml S
#define tY nc=pL pj
#define tZ dW.append(jP pR,jP
#define ua lK(lV 1.0 qA dm*0.05
#define ub *mb lx::epsilon()*
#define uc lP)bB[gE np J)]={}
#define ud al rO oE>al[b]oE;})
#define ue ){mJ
#define uf mA const az
#define ug .emplace_hint(f9
#define uh lP,pI,qu)
#define ui de.aJ(ep)
#define uj cZ U bR B fN B bS B
#define uk ta lf ez>>bw mL iP dx
#define ul aV qt tY f aV>=nc Q
#define um nh W J>=k+2*n+m)
#define un )mW
#define uo ;mJ
#define up -1]
#define uq ck U M ak&ek mp O
#define ur ,tz-rc next mK fA;
#define us nf bs op nI&C mp gS
#define ut ::h4(nj&rng oM ol
#define uu lF J D J<nv S J
#define uv ka&=!bi R rx ka lB
#define uw 31 nS bj tN I=bs nn
#define ux F aS Q S d8 f(d8
#define uy dx lW rV bt=dx.top
#define uz );bb::ey(H cout
sl T,class P>void hP(lf T>&fm,P jQ){fm nb H remove_if(fm oC fm qq jQ),fm lJ A inline ld hC U sW lz value?mb tO>::digits-__builtin_clzll(ln tO>(sW):0;}nC{struct i3 nD cF rz d7 tw mF k7 nD a8 tw mF eZ nD aE D i3 aw N dl tw mF kI nD aE rz dB D i3 aw;i3 aq N dl tw mF az{lf k7>Z oW eZ>ar oW kI>a1 sm fz;mV bY;mV ei pr gA sh qr+2*(oX+qs A ld iM U J mG fz lW?J+1:fz[J]qf dG U i mG qr+i qf bf U i mG qr+oX+i qf eN U j mG qr+2*oX+j qf bx U j mG qr+2*oX+qs+j nm jC U u B v mG bY[u*gA()+v]nm cu U u B v mG ei[u*gA()+v]oz;}nC{struct hR{bool bG mt N cW D H string k8;}mF bq{lf lE>>b5;hR fs ps lo rM}nC{hR bq::fs ps qt n=rv oX,m=rv qs o gz=oR oW bool>bO(n+m,ph N cW=0 lF k D k<gz S k){ld a9=k tF b5[k]){cW-=rv cu(a9,J G J<gz+n)bO[J-gz]mf nh W J>=gz+2*n&&J<gz+2*n+m)bO[n+J-gz-2*n]mf;a9=J oz mH oh n+m S i qm bO oL)cW+=i<n?rv ar oL.aE:rv a1[i-n].aE lw{true,cW,{}}oz nC{az gk(ra in A nC{nO class dN{ra kd;H array<char,65536>hi{}pr cg qz iY D H string dW mE kl(){iY=nz kd.rdbuf()->sgetn(hi pR,hi z nL cg=0 lw iY!=0;}public:explicit dN(ra d6):kd(d6){}H string_view next(){dW.mT lm;;F cg==iY&&!kl()Q{}sN])<=' ')++cg f cg<iY tx}nX;;K g6=cg sN])>' ')++cg ny H string_view jP(hi pR+g6,cg-g6 G cg<iY F dW lW Q jP;tZ z ty dW;}tZ z pz kl()Q dW oz};ok typename T>void read(dN&d6,T&value pk jB=d6.next(pz jB lW&&jB.front()=='+')jB.remove_prefix(1);H from_chars(jB pR,jB pR+jB z,value mI read(dN&d6 mp&sW{nY H strtod(H string(d6.next()).c_str(),nullptr sn az gk(ra in){dN d6(in)pr k qz n qz m D read(d6,k my n my m)ny ld v=k+2*(n+m);az ag;rv Z mu k);rv ar mu n);rv a1 mu m);rv fz mu v Y iL so J ue id=rv fz[J];read(d6,id A mO<k S oK{iL(e my rv pS A mH oh n S i ue p=rv ar oL;iL(rv dG(i nL iL(rv bf(i)sR.dl my p.aE A mH j D j<m S j ue p=rv a1 pt;iL(rv eN(j nL iL(rv bx(j)my p.dB sR sP.cF my p sP.d7 my p.dl my p.aE A ol h9(v)lF J D J<v S J)h9[rv iM(J)up=J lG*eQ:{&rv bY,&rv ei}){eQ->resize(v*v)lF u D u<v S u)mH w D w<v S w)read(d6,(*eQ)[h9[u]*v+h9[w]]mK ag oz pF ag A pF ag K gv=bQ.fs(ag);out<<H setprecision(17)<<gv.cW<<'\n' nE&C:bQ.b5){out<<C z tF C)out<<' '<<rv iM(J);out<<'\n' oz om bU{ld first qz hG rz duration rz jy=-rS N b8=rS N cu rz di qz fu qz ft rT bG mf rM bU cK ps B J);bU kZ ps ly bU&lc ly bU&la);ok bool fo>inline void fG mB in,bU&s ly bU&tz{lH hE=nB jC(s.hG,pi oN;lH gB=s.di,gT=rb,iF=s.jy;oO fo)s.cu=s.cu+mk s.hG,pi oN+pi cu;s.bG=s.bG&&pi bG&&iF+hE<=pi b8;s.b8 qJ s.b8,pi b8-hE-gT);rb=gT+hE+pi duration;s.jy=mv iF+hE+pi duration,pi jy);s.fu qJ s.fu,gB+pi fu);s.ft=mv s.ft,gB+pi ft);s.di=gB+pi di;s.hG=pi hG;}class ak lX ak pM;ak ps B pA C un gD ps B pA C un mT mW cB lv ak&c6);bU dy ps B oe hG lo mE ay ps B bu lo N cu sh jr;}lA bn sh go qL ol&X sh d8;}mA ol d8 oW bU>da oW lf bU>>dA N jr D lA go tw;ok bool fo>class cV lX cV nP e ly ak&C):in_(in),jA(oK,ko(C){on dy U oe end){oO fo F tA end Q oP{W first>=end Q;}ex(ko.dy(in_,mZ end-1)mI J U id){ex(cK(in_,id)mI ex lv bU&next F er)fG<fo>(in_,*er,tz nh er=next;on cA U oe end){dy(mZ end mI kt lv bU&tz{ex(next mI fZ U id){J(id pB bG(rU!er Q true o&s=*er;oO fo){lH e2=in_.jC(jA,s.oN lw!(!s.bG||e2>s.b8||s.fu<-1e-8||s.ft>in_.Z[jA].a8 nM oo lz s.bG&&in_.jC(jA,s.oN<=s.b8&&s.fu>=-1e-8&&s.ft<=in_.Z[jA].a8 nM oz nG a0(oM static_assert(fo pz bG()lC lw er?in_.cu(jA,er->oN+er->cu:0.0;}uf&in_ pr jA ny ak&ko;lt<bU>er rM using bT=cV<true>;lk nO H atomic<lA>gh{1}N dl rE nB ar[(J-k)%n].dl lw sv(qQ n)%m].dl nm jt rE 0 o j=(qQ n)%m lw J<k+2*n+m?nB a1 pt.dB:-nB a1 pt.dB oz bU cK nP J){bU s;s.b8=mb lx oB);s.sX s.hG=J;rb=dl ng J Y k=nB mP ny i3*h3 sp f J<k+n)h3=&nB ar[J-k]qU nh W pp m)h3=&sv qQ n]qU um h3=&sv qQ n-m]sP f h3){s.jy=h3->cF+rb;s.b8=h3->d7;}s.di=jt ng J);s.fu qJ 0.0,s.di);s.ft qO s.di ty s;}bU kZ mB in ly bU&a ly bU&b){bU s=a;fG<true>ng s,b ty s;}ak::ak nP pA C){gD sY mI ak::mT{d8.mT;rr D on ak::cB lv ak&c6 F this==&c6 Q;d8 rZ d8;da rZ da f dA z<c6.dA z)dA mu c6.dA z)sZ c6.dA mX i)dA oL rZ dA oL lF i rZ dA z;i<dA mX i)dA oL.mT;jr rZ jr;go rZ go;on ak::gD nP pA C F&C!=&d8)d8=C;rr=gh.fetch_add(1,H memory_order_relaxed Y size=C z;da mY size)pr a9=e tF C){da lq cK ng J nL jr+=rc J);a9=J qf fU=0 lF kO=1;kO<size;kO*=2,++fU F fU==dA z)dA.emplace_back()uo ge=dA[fU];ge mu size)ny ld kK=kO*2 lF g6 D g6<size;g6+=kK K mid qJ g6+kO,size Y end qJ g6+kK,size G mid>g6){ge[mid up=da[mid up lG i=mid-1;i>g6;--i)ge[i up=kZ ng da[i up,ge oL sq mid<end){ge[mid]=da[mid]lG i=mid+1;i<end S i)ge oL=kZ ng ge[i up,da oL sn W kO>size/2 mj oz bU ak::dy nP oe hG rU tA hG Q da qM o ge=hC(first^hG)-1 lw kZ ng dA[ge]qM,dA[ge][hG]pB ak::ay nP pos rU pos>d8 z lT f pos==0 Q true o fi=d8[pos up,k=nB qr lw fi<k||fi>=k+tB;}na aL pG aZ mF e0{int e=-1 pr aw qz b0 tw pG ab lX ab ps,bq bQ,oY>bA={}un cB mc c6);bq bQ(lo ny ak&bI U e lo ny ol&cM sh a7;qV eB U e mG e<aG z&&!sr lP.empty(A ld aK sh qW qr qL e0&as nZ mG bB ov qf ai sh bB z qf aB sh qW oX qf gZ(lo N cW sh dP;}aZ&bA sh*c2 qL oY>&a5 sh c2 nm aE nZ lo ny az&ag sh*ah qf aJ nZ lo ny md&aU sh cv;on aW(md dR){cv=mw dR A nG a6 nZ lo;nG fH rh rd lo mE hw nZ)mE ec U pA E)mE insert rh rd)mE dz nZ B rd)mE fk U aX B aA B rd)mE gm U oe mQ mE uj fO);uf*ah;oY>c2 oW ak>aG sm a7 oW e0>bB mF il{lA dT D nG O;}ta il>dt;md cv N dP D ol cC;ak bC mW c8 U pA C un uq un sf O un dX U M bool hS)pr b7 nZ lo mW el U e A;na aZ lX struct kr nD*aw N*aq N*j0 N*kU rM explicit aZ U E):gH(hJ(E)),jm(4*gH){}ld jL sh gH-2;}kr gs U kC pk*data=jm pR lw{data,data+gH,data+2*gH,data+3*gH};}mA static ld hJ U E tC 2*E+2 qf gH;mV jm rM lk using aT=cV<false>;om dY{const ol&X pr hN qz g1 rT bo mt pr size sh X z-(bo?2:0 A ld J U bu rU!bo Q X[bu]f bu<hN Q X[bu]f bu<g1-1 Q X[bu+1]lw X[bu+2]oz;pH bZ nP pA X B c7 B bX K aw=X[c7],aq=X[bX]o dO=c7?X[c7 up:e f bX==c7+1)nD iI=mk dO,rQ aq G bX+1<X z K kJ ts,kJ)-mk dO,kJ mK iI tg gK=X[c7+1]o e5=X[bX up N iI=mk dO,rQ gK)-mk dO,gK)+mk e5,aq G bX+1<X z K fF ts,fF)-mk e5,fF mK iI;}pH bP sG B at B J rF fA=rc J qE+=mk J ur}pH br sG B at B aw B aq rF fA=rc rQ aq qE+=mk aq ur}pH cQ sG B c7 B bX B aw B aq F c7==bX Q br sY,c7,aw,aq ty bP sY,c7,aw)+bP sY,bX,aq A om hc{ld aw qz aq rz jy qz d7 rz js qz hm rz dl qz aE qz dB tw;inline hc eq nP h K n=tB f h<n rV p=nB ar ov lw{nB dG mD,nB bf mD,p qU.cF,p qU.d7,0,rS,p.dl,p.aE,0}tg&p=sv h-n]lw{nB eN si-n),nB bx si-n),p qU.cF,p qU.d7,p sP.cF,p sP.d7,p.dl,p.aE,p.dB};}inline ld gE nP J K k=nB mP lw J<k+2*n?(J-k)%n:n+(qQ n)%m;}lk oG ab mB in,bq bQ,oY>bA):ah(&in),c2(bA?mw bA):H make_shared<aZ>(tB+nB qs)),aG(nB qr),bB(tB+nB qs),dt(bB z){a7 mY nB qr Y gv=bQ.fs(in);dP=gv.cW lF k D k<bQ.b5 mX k qm!bQ.b5[k]lW)c8(k,bQ.b5[k]mI oG cB mc c6 F this==&c6 Q mO<aG mX oK W sr rA)!=c6.sr rA))sr.cB(c6.sr);a7 mY aK rI a7 rZ a7;bB rZ bB;dt rZ dt;cv rZ cv;dP rZ dP;}bq oG bQ(oM bq ax;nN b5 mu qW qr)nE e:a7)nN b5 lR=mo lP lw ax qL ak&oG bI U e oM static const ak empty lw e<aG z?sr:empty qf oG gZ sh H count_if(bB oC bB qq[](le&as tC as tb qc;}A double oG aE os->ar ov.aE:qW a1[h-n].aE qf oG aJ os->dG mD:qW eN si-n A ld oG b7 os->bf mD:qW bx si-n mI oG c8 U pA C rV in=*ah tF mo uc uo kH=sr f C lW){kH.mT tl false oo{kH.gD sY)tl true A el(e mI oG dX U M bool hS K j5=H rW a7 oC a7 qq oK lY h2=j5!=a7 lJ&&*j5==e f hS&&!h2)a7.mM j5,oK nh W!hS&&h2)a7 nb j5 mI oG el U e rV C=mo lP lF p D p<C mX p K J=C[p],h=gE np J)uo loc=bB ov;mq e=mi e G J==aJ mD)pI=p nh qu=p oz nG oG a6 nZ qt sa si G mq e<0 lC o sH&fe=mo uo bt=dt ov f qv dT==fe rA)Q qv O o size=fe rw;aT af np M fe sw 0,pI sw pI+1,qu sw qu+1,size);qv dT=fe rA);qv O=af qn?nG(bZ np M fe uh-aE mD):rJ lw qv O;}nG oG fH rh a B b rU h ri||nw qc||e>=pQ lC o&ed=mo o size=ed rw lY bd=h<qW oX f a>b||b>size||(bd&&a!=b)||!ed.ay np a)||!ed.ay np b)lC rx bd&&qW a1[h-qW oX].dB>qW pS nM lC;aT af np M ed sw 0,a nq J(aJ mD pz bd)pm a,b nq J(b7 mD sw b,size sC ny dY jY{ed lP}o cu=bd?br np sx a,aJ mD,b7 mD):cQ np sx a,b,aJ mD,b7 mD ty aE mD-cu;qV oG hw nZ K O=a6 si pz O lT o sa mD o e=nz mq oK;pE lP;ob);dP+=*O;c8(of qg ec U pA E F e>=pQ||E lW lT o&old=mo oW bool>f7(old rw,ph N gQ=0 tG E F h ri lT o sa si G mq e!=mi oK||f7[pI]lT;f7[pI]=f7[qu]mf;gQ+=aE si A bT af np M old)uo C=cC;C.mT lF first D first<old rw;F f7 qM){++first;rK}auto end=first+1;nx end<old rw&&!f7[end])++end;af.cA(mZ end);C.mM C qq old tH mZ old tH end);sX end tg jf=af.a0(pz jf lT;dP+=old qo-*jf-gQ;c8(of qg insert rh a B b K O=fH si,M a,b pz O lT;pE lP;tt);dP+=*O;c8(of qg dz nZ B a B b F h ri||nw<0 lT o e=nz nw Y ic=mo qo;pE lP o sa mD;ob)lY bd=h<pq bu F!bu Q true o fi=C[bu up,k=qW qr lw fi<k||fi>=k rR rx bd&&qW a1[h-sy].dB>qW pS nM lT;tt);pd a0(pz cu lT;ck(M bC,ic-*cu qg fk U aX B aA B a B b F aX ri||aA ri||as(aX)tb<0||as(aA)tb qc lT o sa(aX Y sH ic=mo qo;pE lP;ob)lY bd=aA<pq bu tC!bu||C[bu up>=pQ rR;oT b,b7(aA nL oT a,aJ(aA nL pd a0(pz cu lT;lH O=aE(aA)-aE(aX)+ic-*cu f O qd lT;ck(M bC,O)ns void oG uq ue kH=sr tF kH uc lY hS=!ek lP lW;H swap(kH,ek)tl hS pz hS)kH.mT;el(oK;dP+=O;qV oG gm U th first ri||second ri lT sL mQ o a=as(oN,b=qS a.nK b tb lT o hV so aX B aA K sa(aX)pD X=bI(mq oK lP;oc Y a3=aA<sy?pI:qu-1;X.mM X ls+a3,b7(aA nL X.mM X ls+pI,aJ(aA)ty X;}o fV=hV(mZ mQ,fW=hV(second,oN;ak g3 np a.M fV),g4 np b.M fW);bT e7 np a.M g3),e8 np b.M nl bI(a.oK qo+bI(b.oK sS dg(a.M mw g3),b.M mw g4),O qg uj fO F bR==bS||bR>=pQ||bS>=pQ lT o&jv=bI(bR Y&jw=bI(bS G fN>jv rw||fO>jw rw lT nE&loc:bB F(mq nK mi bR)&&pI<fN&&qu>=fN)||(mq nK mi bS)&&pI<fO&&qu>=fO)lT;}ol fV(jv lP oC jv tH fN);fV.mM fV qq jw tH fO,jw lP lJ)sm fW(jw lP oC jw tH fO);fW.mM fW qq jv tH fN,jv lP lJ);ak g3 np bR,fV),g4 np bS,fW);bT e7 np bR,g3),e8 np bS,nl jv qo+jw sS dg(bR,mw g3),bS,mw g4),O)ns void oG sf O){nX mJ loc:bB qm mq nK mi bR)||mq nK mi bS))loc={}o assign so M ak ek F ek lP lW){sr.mT tl false oo{sr=mw ek)tl true A el(e A;oj bR,mw g3 nL oj bS,mw g4 nL dP+=O;}om ij{lA kp=1 N dm=59.0 mE bl mf mE d0 mf pr eD=1000000 pr eU=400 pr hu=400 N fR=2 N gb=140 pr eu=2 pr eM=3 mE eW mf rM om jE{bq bQ rM jE kT ps ly ij&bN);lk using aI=nd mF ao{ld h qz e qz aw qz aq rz O tw mF dD{nf df N ef=-lj A mF eX{ld e rz O tw mF cq{bool bd;bU aw,aq N aE,dB;}pG aL lX explicit aL ps)mE iw nZ,oH=aI oB)lo mE fr nZ mG f3 ov qL lf eX>&gt nZ mG fq ov qL cq&dI nZ mG cz ov;}uf*ah ta lf eX>>fq mL ol hX ta bool>f3 oW cq>cz rM md cj mc fP,oH);us=-lj nL class aH lX const nf&df mc B h un jR mc mU E un gX mc B pA E)ny nf&df nZ mG pY oD;}qX jZ{lA bn=tW);nf I rM md jb pr eh qz bE D lf jZ>cn mW gy mc un dS mc B h B oK mW dU nZ A;na aD lX explicit aD ps B jK=200,lA kp=1 un ie op un h8(oH B eo=50000 oq gU mt);bq fc(oH,md dR={}ly bq*b3 sp oq km mf oq iq mf B eb=2 oq dF mf B eE=3 un du(oH ly ab&fb un iJ U jK)mE fB sh cE==pJ;qV b2(rN,oH);qX ii{ol C N V tw;using kX=H map<ol,ii>mF Arc{ld h N bL;}ny az&ah pr gP,eK qz d8 rT aS mt pr cE D mV fS;lA jV=1;lA ix D lA fL D lA eA D lf kX>cR oW lA>ch oW lf Arc>>kP;mV e9 mE c1 mt;lA dp D mutable lA eS=tW)mL mV et sm g5 mW add mc B M bool cc un fp U pA E mU C mp V oq cc);mV cY(oH=aI::min()lo rM qF aI cs);na cS lX virtual~cS pM;virtual ol h4(nj&rng lo tw;na jD lX virtual~jD pM;virtual bool b2(rN lo tw;na hb lX void eI(qB>move){gg lq mw move)pB h1(rN,nd am=nd oB)lo;mA lf qB>>gg rM na ik lX sl k6>nf get rh aK,lA aC,lA cD mp ce oq dk,k6&&lb F cn z<=h)cn mu h+1)uo row=cn ov f row z<aK)row mu aK)uo bt=row lR lY ht=qv ig&&qv cD==cD&&qv dk==dk lY k3=ht&&(dk?qv aC==aC:(qw.has_value()||ce<=qv ce)pz k3){qw=lb();qv aC=aC;qv cD=cD;qv ce=ce;qv dk=dk;qv ig mf oI qw&&!dk pk i4=*qw;i4 rL+=ce-qv ce lw i4;nt qw;}qX jZ{nf I;lA aC qz cD rz ce rT dk mt mE ig mt;}oW lf jZ>>cn rM lk ok bool eZ>class bK lX bK ry h):de(ad),ep mD{oO eZ F h>=L sy Q oP{W h<L sy||h>=ml Q tg&qN si G mq e<0 Q;dc=nz mq oK;cP=pI;cO=qu;cy pa a6 mD;cG=bZ(nH,dc,L bI(dc)lP,cP,cO pB ig sh dc!=mb ld>oB pB eT sh cy.has_value(A double c9 sh cG;}protected:const ab&de pr ep,dc=mb ld>oB)pr cP qz cO D nG cy N cG D ld eC U bu rU bu<cP Q bu f bu<cO-1 Q bu+1 lw bu+2;qV dd lv ak&C B bM mG!bM||C.ay(qh),eC(bM-1)+1 mI cf(aT&af B oe end qt jd qJ end,cP G first<jd)pm mZ jd Y gR=mv mZ cP Y hW qJ end,cO-1 G gR<hW)pm gR+1,hW+1 Y hz=mv mZ cO-1 G hz<end)pm hz+2,end+2 sn;na au tD bK<false>lX au ry h);nG O me rd lo;nG bc me rd lo;lt<aT>eO me bD lo;nG dE me rd ly aT&en lo;mA bU jO,is;}pG dn qR ly au&nI&,lA aC lo rM na aM tD bK<true>lX using bK<true>::bK;nG O me bM lo;nG bc me bM lo;}pG ct qR ly aM&nI&,lA aC lo rM na gp mR rN nQ mA mutable aH hk mL ol bE rM na eH mR rN nQ};na dC mR rN nQ mA};om bz{ld aX;ao aQ N O;}pG aY final tD jD lX aY pM mE b2(ab&nQ lt<bz>dZ mc lo;qX Row{lA fj D ak dH N a6 tw ta Row>kS mF ez{lA b1 D lt<bz>I;}mF a2{bz I;lA dT,b1;}mF ne az*gl sp ta lA>b4 ta bool>ej uk mW cJ mc B pA aX mU aA lo mW eg mc lo mW f6(lo mW fQ U e B aA lo mW f2(lo rM om be{ld mZ second N O rM nG eF mc B oe mQ pG cr final tD jD lX cr pM mE b2(ab&nQ lt<be>dZ mc lo;qX ez{lA b1 D lt<be>I;}mF a2{be I pr bR,bS;lA jW,jX,b1;}mF ne az*ah sp ta lA>ch uk mW eg mc lo mW fQ U bR B bS lo rM lk jE kT ps ly ij&bN rn g6=pT Y am=g6+lK(bN.dm)Y cs=am-ua)nL hb bl;pv eH>rI pv gp>rI pv ct>rI pv dn>()G bN.d0 pk move=lU<dC>();bl.eI(mw move)A pv cr>rI pv aY>rI sM);ab bJ(ag,mw empty)Y hL qJ am,pT)+ua))nL bJ.aW(cj(bJ,hL nL aD fy(ag qA eU qA kp);fd(bJ,fy,bN,cs);ab df=bJ;fy.ie(bJ);fy.b2(df,am);qK f bN.bl&&pT)oJ 500)<am K h5=df.cW();bl.h1(df,am G df.cW()>h5+oy fy.ie(df);qK;}jE ax;nN bQ=df.bQ(ty ax;}qF aI cs rn&ag=bJ.ag(Y g2=pT Y fT qJ cs,g2+lK(lV bN.fR qA dm*0.2))nL iT.h8(fT qA eD)lY co=iT.fB()lY jT=!co||oR>48 f jT K jq=pT Y gG=lK(lV 3.0 qA dm*0.05))Y cb qJ cs-gG,jq+lK(bN.gb)nL ld kA D do{le hB=oR>48?lV cb,nk(6)):cb o b3=bJ.bQ();ab iz(ag,iT.fc(hB,bJ.aU(),&b3 qA bl qA d0 qA eu qA eW qA eM),bJ.a5 rI iz.aW(bJ.aU rI bJ.cB(iz G oR>48&&nk(2)<cb K iD qJ cb,nk(6 nL iT.b2(bJ,iD)S kA;qK f nk(2)<cb K fK qJ cb,nk(3)G kA%2)iT.du(fK,bJ)nh iT.h8(fK qA eD ss);qK rx(kA%2))qK;qK oz}nx oR>48&&pT)<cb sq co F!jT)iT.iJ(lV bN.eU qA hu)A qK;}na fY mr fY ut E(ml pU(E oC E lJ oF;sj)sm bo;bo mY count)tG E F bo z>=count mj f L hw mD)bo lq h mK bo;}na gJ mr gJ::h4(nj&oM qb I=ml N cX=-rS tX r K O pa a6(r G O&&*O>cX){I=r;cX=*O qG I mK bo;}na fE mr fE ut bO tX r qm sI r G bO lW Q{}o j6=bO[sc bO z]rj nH o he pa aJ(j6 Y cF so r tC r<tB?nB ar R qU.cF:sv r-tB]qU.cF rM lH iQ=cF(j6);qb I=ml N f1=rS tX r F nV e<0||!L a6(r)qx J pa aJ(r);lH ds=nB jC(he,J)+nB jC(J,he)+ti iQ-cF(r)G ds<f1){f1=ds;I=r qG I mK bo;}lk nO cq dw nP h K data=eq ng h ty{h<tB,cK ng ni aw),cK ng ni aq),ni aE,ni dB}oz aL::aL ps):ah(&ag),fq(rv oX+rv qs),hX(fq z oF,f3(fq z,ph,cz(fq z){mH h D h<cz mX h)cz ov=dw(ag,h pB aL::iw nZ,oH rU f3 ov Q true rj*ah o data=eq ng h)ny ak empty uo gt=fq ov lG&e=hX ov;e<nB Z mX e F am!=aI oB)&&pl lT;bT af ng M empty nq fZ(ni aw nq fZ(ni aq Y cu=af.a0(G cu)gt lq{M tP*cu}A pu gt oC gt mx O>b rL||(a rL==b rL&&a tb<b tb A);f3 ov mf ns md cj mc fP,oH pk dR=H make_shared<aL>(fP.ag())lF h D h<fP.ai()S h F!dR->iw si,am)tx nt dR;}nO void i5(dD&ax,ao I F!tc||I rL>tc nF||(I rL==tc nF&&I tb<tc->oK F tc)nN ef=mv nN ef,tc nF);tc=I oP nN ef=mv nN ef,I rL A cq dI ry h tC dw(nH,h A dD h0 op nI&C ly cq&data,aZ&kD mp gc=-rS oq jG mt){dD ax o&in pa ag(G ni dB>nB pS nM Q ax o cp=C rw o aw=ni aw.mZ aq=ni aq.first o&X=C lP o gs=kD.gs(cp)pD*em=gs qU pD*dq=gs sP pD*ci=gs.j0 pC K a9=pos?X[pos up:e o iS=pos<cp?rc X og):0.0;em og=rc aw)-iS+(pos<cp?mk aw,X og):0.0);dq og=rc aq)rs);ci og=rc rQ aq)rs tI gx=gc N c4=rS N ev=rS pC){c4 qJ c4,ci og pz ni bd){c4 qJ c4,ev+dq og);ev qJ ev,em og sn W tP c4 nM<gx Q ax pD*bm=gs.kU rx ni bd){bm[cp+1]=rS lF tu bm og qJ dq og,bm[pos+1]A mH a D a tj a F!pV a)qx fl pb lz mv gc,jG&&tc?tc nF:nN ef A o gq=ni bd?ci rO:lV ci rO,em rO+bm[a+1]G tP gq nM<fl()lB aT en sY);en.dy(0,a);en.ex(ni aw pz en qn qe f ni bd pk af=en;af.ex(ni aq sw a rG a,tP ci rO}oo{le fs so b mp fA pk af=en;pm a,b nq ex(ni aq sw b rG b,tP fA}A lF b=a;b tj b F!pV b)qx fA=b==a?ci rO:em rO+dq[b]f tP fA nM<fl()lB fs(b,fA sn nt ax oz us F h>=ml||e>=L pQ||st qc lC lw h0 tJ,of,dI tJ),L bA(),gS ss).df;on aH::dS ry h B oK{mJ bt=oU o&C td;qw oQ G L eB(oK){qw=h0 tJ,of,L aU()->dI mD,L bA(),1e-9 ss).df f qw&&qw nF<=oy qw oQ oo{qy&empty:L aU()->gt mD qm empty.nK e F empty rL>oy qw=ao{h,M 0,0,empty rL}pK oz qv bn=C rA mI aH::dU nZ ue jo=pY oD;jo oQ)mO<eh S e rV I=oU oD f I&&(!jo||tK>jo nF))jo=I;}on aH::gy op rV dR pa aU(G jb!=dR||eh!=L pQ||bE!=ml){jb=dR;eh pa pQ;bE=ml;cn.oj bE*(eh+1),jZ{}sn const nf&aH::df ry h){gy(ad G st qc){pY oD oQ ty df si A jb qp bn()){dS tJ,e A dU mD lw df si mI aH::jR op mU E){gy(ad Y&dR=jb tG E F st qc lB dR qp bn())dS tJ,oK;dU si A on aH::gX ry pA E){qy h:E F st qc qx a9=df mD;dS tJ,e Y&ek=oU oD f a9&&a9->nK oK dU mD nh W ek&&(!a9||ek nF>a9 nF||(ek nF==a9 nF&&e<a9->oK))pY oD=ek oz om bk{ld e sm E N V;}mF cL{lf int>ba;mV by N V qz hZ D ld X qz fh rT bF mt mE fr mt rM sg,oH)mE bv();cL db mS,oH oq i0 mf);cL dM mS,oH,lA kp=1 ly mV&by={});lk nO sl kX,class k1>double hp(kX&hF,k1 dJ pk i2=hF oC second=hF lJ lG it=H next(i2);it!=hF lJ S it F dJ(it->mQ<dJ(i2->mQ){second=i2;i2=it oP W second==hF lJ||dJ(it->mQ<dJ(qY mQ){second=it oz le floor=dJ(qY mQ;hF nb i2 ty floor oz te aD ps B jK,lA kp):ah(ag),gP(jK),fS(oR,-rS),jV(kp),cR(oR),ch(pJ oF{on te add ry M bool cc rV X td lP f X lW Q o k pa pQ,n pa sy uo E=g5;E.mT N aE=0 tF X){ld h f J<k+n)h=J-k nh W pp ah.qs)h=n+qQ n nh rK E lq h);aE+=L aE si A pu E oC E lJ Y V=aE-L mo qo;fp(M E,X,V,cc mI te fp U pA E mU X mp V oq cc F!mN V)||V qd Q uo hF=cR lR f cc&&hF z>=gP&&V<fS lR Q o f9=hF.rW E G f9!=hF lJ&&f9->tA E F re V>=V Q;re C.oj X oC X lJ);re V=V oP{hF ug,E,ii{X,V}A++dp rx cc)fS lR=-lj G cc&&hF z>gP){fS lR=hp(hF,[](le&aV tC aV oE;}A on te h8(oH B eo oq gU){cE=0 f pl Q rj ah o n=tB,nr=n+nB qs,nv=nB gA()pD aO=gU?cY(am):mV tQ sO}f gU K jz=jU[eA%3 sT tg hr=ow eA++oW hc>E;E mY nr)pw{E lq eq ng r nL E.back().aE-=oa tg m=nB qs rx c1 K eR=li o gj qJ 0.6,0.25*rB(am-eR).count()Y d9=eR+lD duration_cast<aI::duration>(rB(mv 0.0,gj)nL lf lf double>>iA;mV jl(nv,lj nL lf mh gn(nv oF pr gd=0 lF l D l<pN 8,nv)S l F li>=d9 tx mV ds(tL ls+gd*nv,tL ls+(gd+1)*nv);ds[gd]D lf mh i1(nv oF mE fr mf lF iO D iO<nv S iO F(iO&63 nS d9){fr mt pK qf v=nv tM rS pZ qm!i1[u]&&ds[u]<sW{v=u;nY ds[u]oI v==nv tx i1[v]=1 pZ qm!i1[u])ds[u]qJ ds[u],value+tL[v*nv+u]sq!fr tx gn[gd]=1 pZ)jl[u]qJ jl[u],ds[u]);iA lq mw ds))N i9=-1 pZ qm!gn[u]&&jl[u]>i9){i9=jl[u];gd=u oz mV dr(nv*m oF uu)mH r D r<m S r ue ju=dr[J*m+r]o kY=nB bx(r)nE&d:iA){lH kx=1e-8+64 ub nv*mv{1.0,d[kY],d[J]});ju=mv ju,d[kY]-d[J]-kx sn qK oW lf Arc>>next(nv)uu F pl Q uo k2=next[J]nA<nr S r K hE=tL[J*nv+E R qU]o kx=8 ub mv{1.0,ti E R.d7),ti hE)}Y d7=E R.d7-hE nM+kx f d7>=-1e-8)k2 lq{r,d7}A pu k2 oC k2 mx bL>b.bL||(a.bL==b.bL&&a.h<b.h A A kP=mw tz;e9=mw dr);c1 mf;qK tg&next=kP o&dr=e9;cE D lf mh jN tQ,hY tQ sm C,bO,key;C mY 2*nr);bO mY nr);key mY nr)mO<cR mX e F pl tN now=li o bj=am==aI oB)?am:now+(am-now pc>(pJ-oK pr fx D H map<ol,rC,ii>>cI N gW=-rS pr fJ=eo pD e6=bj mE d2 mt,gw mt pr c0=0 nE&arc:next lR rV q=E[arc.h]f q.dB<=nB pS nM&&tL[e sD qU]<=q.d7 nM)++c0 tg hQ pb(auto&&kG B hG mp jC mp cu mp aE mp load B fw)->void{W d2 Q f fx>=fJ||((fx&63 nS e6)){d2 tf;}++fx no bO qm hY R&&jC+dr[hG*m+r-n]>E R.hm nM Q rx fw&&aE-cu>1e-9&&(gU?(cI z<gP||aE-cu>=gW):(cR lR z<gP||aE-cu>=fS lR))){key.oj bO oC bO lJ);pu key oC key lJ pz gU)fp(M key,C,aE-cu ss)nh{le dJ=aE-cu o f9=cI.rW key)lY i8=f9!=cI lJ&&f9->tA key rx i8||re first<dJ)nD V=-cu no key)V+=r<n?nB ar R.aE:sv r-n].aE f i8){re sX dJ;re nu C.oj C oC C lJ);re nu V=V oP cI ug,key,H make_pair(dJ,ii{C,V})G cI z>gP){gW=hp(cI,[](le&bt tC qv first;}sn}}W!mN jC)Q pD qZ rk z;lA kc=0 f gU K j1=H partition_point(rk oC rk qq[&](le&arc tC jC<=arc.bL;});qZ nz j1-rk ls);kc=hr+e*0x94d049bb133111ebULL+hG rt=(kc^(kc>>30))rt^=kc>>27 tg d1=bO z,i7=d1+count o kg=i7&&(kc&1)?nz kc%i7):0 sZ i7 S i K kw=kg?(i+kg)%i7:i f kw<d1 K r=bO[kw]rx hY R qx&q=E R;lH e2=jC+tL[hG sD sP]rx mN e2)||e2>q.hm nM lB hY R D C lq q sP);kG(kG,q sP,mv e2,q.js)+q.dl,cu+nB ei[hG sD sP],aE,load-q.dB,fw-1);C rf);hY R=1 f d2 Q;rK}le&arc=rk[kw-d1]rx gU&&jC>arc.bL tN r=arc.h f jN R qx&q=E R f load+q.dB>nB pS nM lB lH e2=jC+tL[hG sD qU]f e2>q.d7 nM lB const bool sX gU&&bO lW f first K gF=li f fx>=eo||gF>=bj){gw tf;}fJ=fx+tp 1,(eo-fx)/c0);e6=bj==aI oB)?bj:gF+(bj-gF pc>(c0 A lH iu=mv e2,q.jy)+q.dl;lH jJ=cu+nB ei[hG sD qU];jN R=1;bO lq r);C lq q qU G r<n){C lq q sP Y gO=iu+tL[q qU sD sP]f mN gO))kG(kG,q sP,gO+q.dl,jJ+nB ei[q qU sD sP],aE+q.aE,load,fw);C rf oo{hY R=1;kG(kG,q qU,iu,jJ,aE+q.aE,load+q.dB,fw+1);hY R tw C rf);bO rf);jN R=0 f oN{gw|=d2;d2 mt;--c0 oP W d2 Q oz;hQ(hQ,M 0,0,0,0 oF;cE+=!d2&&!gw nE rm:cI)fp(M key,c.nu C,c.nu V,false sn mV te cY(oH qt nr=ah.oX+ah.qs f eS==dp&&et z==nr){qK lw et;}struct gI{const ol&E N V;}oW gI>al sm Z lF t D t<cR mX t)qy rm:cR sQ){al lq{key,c oE});Z lq t A ol c5(pj pU(c5 oC c5 lJ oF;pu c5 oC c5 tR tC ud oW mh bi tQ,f0(pJ oF N ju=0 lG j:c5 F f0 tm tn no al pt nU uv f0 tm=1 no al pt pW 1;ju+=al pt oE;}mV aO tQ,cH=aO N df=rS lF bp D bp<192 S bp){mV cm(pJ oF oW const gI*>dL(pJ,nullptr)oS sz j sd pt tM c oE rP pO oa f value>cm tm){cm tm=value;dL tm=&c oz le i6 qD+nR cm oC cm qq 0.0 G i6<df){df=i6;cH=aO;}ol j4 tQ nE c:dL qm c)pn c->E)++j4 R N kz=0 pw W oa>0||j4 R>1 K g=qa;kz+=g*g oI kz==0||i6<=ju+1e-7||!mN i6)tN iO=0.7*(i6-ju)/kz pw oa qO oa+iO*(qa)sq(bv()||pJ<=96)&&li oJ 250)<am){lf bk>kq mO<cR mX oK{lf rC ly kX::value_type*>>ea nE&bt:cR lR K rm=bt N ro-=cH R;ea lq{dJ,&bt}tI qZ bv()?ea z:pN 64 nW})oS sA j)kq lq{M ea pt.qY mZ ea pt.qY nu V}tI iR qJ am,li sE 2 nL auto g9=ee(kq,pJ,nr,iR)tM nR g9 oC g9 qq 0.0)nE&hF:cR)nD cm=0 nE rm:hF)nD ro-=g9 R;cm=mv cm,dJ A value+=cm;}qK f value<df)cH=mw g9 mK cH;}bq te fc(oH,md dR ly bq*b3 oq km oq iq B eb oq dF B eE K nr=ah.oX+ah.qs;bq ku f b3)ku=*b3 nh ku.b5 mu pJ);ab empty(ah,ku pz dR)dR=cj(empty,lV am,li sE 1)nL empty.aW(mw dR nL ol E(nr pU(E oC E lJ oF qi(ow ix++)pr hj D ab ad=empty,df=empty;aH bI;fY hO;fE hv;gJ ih oW qB>>cx;pX eH>rI pX gp>()pz dF){pX ct>rI pX dn>()sq iq)pX dC>()pz dF)pX cr>rI pX aY>rI nx li<am F hj F hj%8==0||L cW()<df.cW()*0.995)L cB(df Y bo=tp 4,L gZ()/12 G hj%4==1)hO rp W hj%4==2)hv rp W hj%4==3)ih rp mH i qz qZ 2+sc tp 2,pJ/12);i sA i K e=sc pJ sm bo no E qm nV nK mi oK)bo lq r);lZ(!bo lW&&!L ec(M bo)sn sj)no E F pl mj f nV e qc qx I=bI.df nn r pz I||tK qd lB lZ(ot A nX;;F pl tx nf I no E qm nV e<0 rV jg=bI.df nn r G jg&&jg nF>1e-9&&(!I||jg nF>tK))I=jg oI!I tx lZ(ot sq km&&hj%eb==0&&li oJ 250)<am){mH jH D jH<eE S jH){bool aP mt oS<cx mX j){aP=cx pt->b2(ad)||aP oI!aP tx}nX auto qj)tS false sq L cW()>df.cW()+oy{df.cB(ad);qK;}++hj;}qK lw df.bQ(mI te du(oH ly ab&fb pk aO=cY(am)sO}o jz=jU[fL%3 sT o nr=fb.ai(),n=fb.sy,k=pJ qi(ow fL++)pr iE=0 mO<k S e K now=li f now>=am tN bj=now+(am-now pc>(k-oK;bq b3;b3.b5 mu k);b3.b5 lR=fb.mo lP oW rC ly ii*>>ea nE rm:cR lR)nD ro-=oa;ea lq{dJ,&c}tI qZ pN 3 nW})tk kE{b3.b5 lR}oS sA j)kE lq ea pt.qY C);ab ad(ah,b3,fb.a5 rI L tq ld jj=0 lF bp D bp<48&&li<bj S bp)nD O=1e-8;nf aQ;lt<ld>aX sm bg pw W nV e<0 F(r&uw r,M L mo,O+oa G I&&tK-oa sJ tK-oa;aQ=I;aX oQ sn ol bO pw W sI r)no bO K iW pa a6(r G iW&&*iW+oa sJ*iW+oa;aX=r;aQ oQ);bg.clear(sq li>=bj tN qN(r)pD X td lP;oc);ak dH(ah,M X Y hg=bZ(ah,M L mo uh-L aE(r)lF aA D aA<nr S aA qm L as(aA)tb<0 F(aA&uw aA,M dH,O-hg-oa+aO[aA]G I&&tK+hg+oa-aO[aA]sJ tK+hg+oa-aO[aA];aX=r;aQ=I;bg=X oz}W O<=1e-8 F++jj>8 tx b3.b5 lR=kE[jj%kE z];ab jS(ah,b3,fb.a5 rI jS.tq L cB(jS);bO.mT pw W sI r pz bO lW){H shuffle(bO oC bO qq rng);bO mu lV bO z,nz 1+sc 3)nL L ec(M bO A continue oI aX&&aQ K r=aQ->h o kY=r<n?ah.bf(r):ah.bx(r-n);bg.mM bg ls+aQ->aq,kY);bg.mM bg ls+aQ->aw,L aJ(r nL bq af;af.b5 mu k nq b5 lR=bg;ab aP(ah,af,fb.a5 rI aP.tq L cB(aP oo W aX){rl hw(*aX)oo rl mM aQ->h,M aQ->aw,aQ->aq nL tS ph S iE oz qK;on te ie op){qy qj F ch lR==L mo rA)lB ch lR td rA);tS true A on te iJ U jK){gP=jK;tT fS oC fS qq-rS)lG&hF:cR)nx hF z>gP K i2=H min_element(hF oC hF mx nu V<b.nu V;});hF nb i2)S dp oz bool aD nT,oH){qy qj)tS ph mF e1{ol E,C N V;}oW lf e1>>al(pJ)sb ba(pJ to,aN(pJ to oW mh bi(ml oF sm iy(ml oF,c5;eK rz bV=0 mO<cR mX oK{mJ row=pP;row mY cR lR z)nE&[key,aV]:cR lR)row lq{key,aV.C,aV oE});pu row oC row mx V>b oE||(a oE==b oE&&a.E<b.E A);eK+=row z oW mh iU(ml oF sZ row mX i){pn row oL nU iU R=1 mE bJ=!row oL.E lW no row oL nU bJ&=nV nK mi e G bJ){ld qZ 0 tX r)count+=nV nK mi e G count==row oL.E z){su=mi i);bV+=row oL oE oz}mH r D r<iU mX r)iy R+=iU R;}ol h7(pJ oF mO sz oK qy&aV:pP)pn aV nU h7 lR+=iy R-1 mO sz oK W!pP lW)c5 lq oK;rg c5 oC c5 tR tC h7 rO>h7[b];}Y dh pb lv e1&aV){pn aV nU W bi R lT ns;d8 D aS mt o iN=li o fn=c5 z>48?lV am,iN sE 2)):am o fI pb lv ol&d3){tT bi oC bi lJ oF mO<aN mX e F su<0||H find(d3 oC d3 qq oK!=d3 lJ qe no pP[su]pW 1 nm fv D lf int>dL(d3 z to,gr lG e:d3){gr lq su G su qc)fv+=pP[su]oE nm dv=fv;qC U mp)>hQ so f5 mp V ux&63 nS fn){aS tf oI f5==d3 z F V>dv+oy{dv=V;gr=dL tv<d3 mX d)qy&aV:al[d3[d]qT dv tE Q o e=d3[f5]sZ pP mX i rV aV=pP oL rx dh(aV)lB dL[f5]=mi i)no aV pW 1;hQ sK+aV oE)no aV pW 0 f aS tx}dL[f5]=-1 rx aS)hQ sK A f li<fn)hQ(0 oF nh aS mf f dv<=fv tE lT sZ d3 mX i)aN[d3 oL]=gr oL;bV+=dv-fv ns mE tU nx aP&&!aS){aP mt lF a D sU)aP=fI({c5 rO,c5[b]})||aP;}qK lF a D c5 z<=48&&sU)mH c=b+1;c<po c)fI({c5 rO,c5[b],c5[c]});qK f eK>2000){lf bk>je oW H pair<ld B>>iC sb b3(aN z to mO sz oK mH oh pP mX i F su==mi i))b3 lR=mi je z Y&c=pP oL;je lq{M c.E,c oE});iC.emplace_back(M i tI kN=pj>48&&!bv()?cY():mV{}o eG=pj>48?dM(je,pj,ml,b3,am,jV,kN):db(je,pj,ml,b3,am G eG.by z==ml){et=eG.by;eS=dp;}bV=eG oE;aS=eG.bF;d8+=eG.X;tT aN oC aN qq-1)lG id:eG.ba qm id qc K[M dR]=iC[id];su=mi dR A qK oP{mV aO(ml oF,cH=aO N ip=rS oW lf double>>dH(pj)tk h6(pj)lF bp D bp<192&&!aS S bp F pl){aS mf pK nm i6 qD sm j4(aO z oF lG e:c5)nD cm D int dL=-1 sZ pP mX i rV aV=pP oL tM aV oE no aV nU pO oa f value>cm){cm=value;dL=mi i sn i6+=cm f dL qc)pn pP[dL]nU++j4 R oI i6<ip){ip=i6;cH=aO nm kz=0 nA<aO mX r qm oa>0||j4 R>1){lH ja=qa;kz+=ja*ja oI!kz||i6<=bV+1e-7 tx lH iO=0.9*(i6-bV)/kz nA<aO mX r)oa qO oa+iO*(qa)A qK;aO=mw cH);lH iV qD lG e:c5 ue fm=dH lR nE&aV:pP)nD nY aV oE no aV nU pO oa;fm lq value A mJ hs=h6 lR;hs mu fm z pU(hs oC hs lJ oF;rg hs oC hs tR tC fm rO>fm[b];}A tT bi oC bi lJ oF;qC U mp mp)>h1 so f5 mp V mp gC ux&63 nS am){aS tf oI f5==c5 z F V>bV+oy{bV=V;aN=ba tv<c5 mX d)qy&aV:al[c5[d]qT bV tE Q N gV=V+iV-gC lF d=f5;d<c5 mX d K e=c5[d]N cm=0 lG i:h6 lR F dH lR oL<=0 mj f dh(pP oL)){cm=dH lR oL pK oz gV+=cm oI gV<bV-1e-7 Q o e=c5[f5]lG i:h6 lR rV aV=pP oL rx dh(aV)lB ba lR=mi i)N kV=gC no aV nU{bi R=1;kV+=oa;}h1 sK+aV oE,kV)no aV pW 0 f aS tx}ba lR=-1 rx aS)h1 sK,gC A f li<am)h1(0,0 oF nh aS mf oI bV<=L cW()tE lT;bq af;af.b5 mu L pQ)mO<aN mX oK W su qc)af.b5 lR=pP[su].C o kM=af.fs(ah);ab jh(ah,mw af),L a5 rI jh.aW(L aU rI L cB(jh)ns lk nO struct cU{const lf bk>&al tk kR sm hy pr iv qz Z;cU lv lf bk>&c B k B E):al(c),kR(c z),Z(k){lf int>dR(k+E to oS<c mX j K add so r F dR R<0){dR R=mi iv++);hy lq r A kR pt lq dR R A;add(c pt.oK no c pt nU add(k+r sn}pG eY{const cU*cl pr n_;mV c3,e3,eP,k0,iK sm e4 oW mh d5 sb cN N fM U row B ul tr aV-nc]tM 0 no pL kR[aV])value+=tr r]rH value+=iK R*tr r];nt value nm dH U ul-eP[aV-nc]tM pL al[aV]oE no pL kR[aV])pO eP R rH pO iK R*eP R;nt value;on kQ U cd B cw){mV fa(n_),row(n_)lF qP)fa oL=fM(i,cw Y jI=fa[cd];lH iO=e3[cd]/jI,O=dH(cw)nA<n_ S r){row R=c3[cd*n_+r]/jI;eP R+=O*row R;}mH qP qm i!=cd){e3 oL-=iO*fa oL f fa oL==0 qe nA<n_ S r)c3[i*n_+r]-=fa oL*row R;}H copy(row oC row qq c3 ls+cd*n_);e3[cd]=iO;d5[e4[cd]]D e4[cd]=cw;d5[cw]=1 S fh;}public:ld fh rT bF mt;explicit eY lv cU&eQ):cl(&eQ),n_(eQ.iv),c3(n_*n_ oF,e3(n_,1),eP(n_ oF,k0(n_,1),iK(n_ oF,e4(n_),d5(eQ.pj+n_ oF,cN(eQ.pj to{mH qP){c3[i*n_+i]=1;e4 oL=eQ.pj+i;d5[e4 oL]=1;}on kk(oH K tY pr gu=0 ql{bF tf qf cw=tV N O=1e-8 oS<tV S j F d5 pt lB lH nY dH(j G value sJ value;cw=j f gu>n_ mj oz W cw==tV Q pr cd=n_ N iO=rS lF qP K d=fM(i,cw G d qd qx jx qO e3 oL)/d f jx<iO-1e-9||(ti jx-iO)qd&&(cd==n_||e4 oL<e4[cd]))){cd=i;iO=jx oz W cd==n_||!mN iO)Q;gu=iO<1e-9?gu+1:0;kQ(cd,cw A on ir U aV oq include){lH k4=include?-1:1,rhs=include?-1:0 o old=n_++;mV g7(n_*n_ oF sZ old S i)H copy_n(c3 ls+i*old,old,g7 ls+i*n_)tM 0 sZ old S i qm e4 oL==aV){nY e3 oL nA<old S r)g7[old*n_+r]=-k4*c3[i*old+r]pK;}g7[old*n_+old]=1;c3=mw g7);e3 lq rhs-k4*sW;eP lq 0);k0 lq rhs);iK lq k4);e4 lq pL pj+old);d5 lq 1);cN[aV]=mi old);fh D on i6(oH K tY ql{bF tf qf cd=n_ N jM=-1e-7 lF qP qm e3 oL<jM){jM=e3 oL;cd=i oI cd==n_ Q pr cw=tV N jx=rS oS<tV S j F d5 pt qx d=fM(cd,j G d>=-1e-9 lB lH af=dH(j)/d f af<jx-oy{cw=j;jx=af oz W cw==tV Q;kQ(cd,cw sn mV fm(oM mV x(pL pj oF lF qP qm e4 oL<x z&&mN e3 oL))x[e4 oL]=H clamp(e3 oL,0.0,1.0 ty x nm iZ(oM auto aO=eP N hI qz gf=0 nA<n_ S r F!mN oa)Q rS;oa qO oa);hI+=oa*k0 R;gf+=ti oa*k0 R A mV ha(pL Z oF oS<pL al mX j rV c=pL al pt tM c oE no pL kR pt)pO oa f cN pt qc K r=nz cN pt);pO iK R*oa;}ha px mv ha[c tb],value tI iH=nR ha oC ha qq 0.0);hI+=iH;gf+=iH rx mN hI)||!mN gf)Q rS lw hI+1e-7+1e-10*gf;}mV by U E oM mV ax(E oF lF row D row<pL hy mX row K ji=pL hy[row]f ji>=pL Z&&mN eP[row]))ax[ji-pL Z]qO eP[row]mK ax oz;}sg,oH){pe qK lw kn.by(E pB bv(tC false;}cL db mS,oH oq i0){cL ax;rX=aN qH f al lW){nN fr tf ax;}pe nN fh=kn.fh f kn.bF){nN bF tf ax;}nN hZ=kn.iZ(G i0)qK mE jc mf nE&c:al)jc&=c oE==H round(c oE)oW mh j2(Z oF,bi(E oF,hq(pj oF sb ba(Z,-1 Y dh so id sd[id]f j2[c tb]||hq[id]lT rP W bi R lT ns;qC lv eY&mp)>hQ pb lv eY&lp mp V F nN bF Q f pl){nN bF tf;}++nN X N iZ=lp.iZ(G jc)iZ=H floor(iZ G iZ<=nN V tE Q o x=lp.fm()sm dj oS sz j qm dh(j))dj lq j G dj lW F V rD=V;rX=ba;nt;}rg dj oC dj tR tC x rO!=x[b]?x rO>x[b]:ud pD kV=bi,f0=j2 pD fb=ba N af=V lG id:dj sd[id]f f0[c tb]tn rP ka&=!kV R rx ka lB f0 px 1 rP kV R=1;fb px mi id);af+=c oE oI af rD=af;rX=mw fb G i0)qK oI iZ<=nN V tE Q pr hh=dj.front()N ga=-1 lG j:dj F x pt<=1e-7||x pt>=1-1e-7 qx dJ qJ x pt,1-x pt G dJ>ga){ga=dJ;hh=j oz le&c=al[hh];{auto hl=lp;hl.ir(hh ss)pf;}j2 px 1 no c pW 1;ba px mi hh);hQ(hl,V+c oE);ba px-1;j2 px 0 rP bi R tw W!nN bF pk hl=lp;hl.ir(hh,ph pf;}hq[hh]=1;hQ(hl,V);hq[hh]tw};hQ(kn oF;nN fr=!nN bF lw ax;}cL dM mS,oH,lA kp ly mV&by){cL ax;rX=aN;nN hZ=rS qH tk io(Z)oS sz j)io[al pt tb]lq j);mV fg(pj,0 pz by lW)mH j D j sz j)pn al pt nU fg pt+=by R lm lH jz:{0.0,0.5,1.0,1.5}){ol dj(pj pU(dj oC dj lJ oF;pu dj oC dj tR K x=al rO oE-jz*fg rO,y=al[b]oE-jz*sV;})oW mh bi(E oF sb fb(Z to N V=0 lG j:dj sd pt f fb[c tb]qc tn rP uv fb px mi j)no c pW 1;V+=c oE oI V rD=V;rX=mw fb);qK oz nX mJ hF:io)pu hF oC hF tR K x=al rO oE-fg rO,y=al[b]oE-sV;})qi(kp Y g6=li pr hA qz iB D nx li<am){lf int>iG(E to lF t D t sB t qm rX sQ qc)pn al[rX sQ]nU iG R=mi t)ny ld kW qJ Z,nz 8+4*((hA/Z)%4)nL ol d3 oW mh hU(Z,0 Y add so t F!hU sQ){d3 lq t);hU sQ=1 oz o sX hA%Z;add(first pz io qM lW K k9=hA%3==0?sc pN 24,io qM z):0 no al[io qM[k9]]nU W iG R qc&&d3 z<kW)add(iG R A nx d3 z<kW){mV dJ(Z oF lG t:d3 K old=rX sQ qc?al[rX sQ]oE:0 o qZ pN 32,io sQ z)oS sA j sd[io sQ pt]o nY mv 1.0,c oE-old)sk)dJ[iG R]+=value oz ld dL=Z N cm=0 lF t D t sB t qm!hU sQ&&dJ sQ>cm){cm=dJ sQ;dL=t oI dL==Z||hA%5==4){do{dL=sc Z;}nx hU[dL]A add(dL A lf bk>j3 sm kb sb b3(d3 z to lF t D t<d3 mX t)nX auto id:io[d3 sQ]sd[id]mE im mf sk){im mt pK oI!im qe f rX[d3 sQ]==mi id))b3 sQ=mi j3 z);j3 lq{t,c.E,c oE});kb lq id tI now=li o hM qJ am,now oJ 150)Y eG=db(j3,d3 z,E,b3,hM,ph;nN X+=eG.X;nN fh+=eG.fh N old=0 lG id:b3 qm id qc)old+=j3[id]oE f eG oE>old+oy{mH t D t<d3 mX t)rX[d3 sQ]=eG.ba sQ qc?mi kb[eG.ba sQ]):-1;nN V+=eG oE-old;iB=hA;qK;}++hA f hA-iB>12*Z tx}qK;nN bF=pl lw ax;}lk bool hb::h1(rN,nd am oM bool aP mt lm int jH D jH<2 S jH){bool dQ mt nE&move:gg F pl Q aP||dQ;dQ=move->b2(ad)||dQ oI!dQ tx tU nt aP;}lk bool gp nT rU bE z!=ml){bE mu ml pU(bE oC bE qq 0 A hk.jR nn bE)se nf df tG bE F st qc qx move=hk.df si G move&&move nF>1e-9&&(!df||move nF>df nF))df=move oI!df tx rl mM df->h,df->M df->aw,df->aq nL tU hk.gX nn df->M bE mK aP;}lk bool eH nT oM lf lt<double>>iE(ml)oZ)iE ov pa a6 mD se lt<ld>df N cX=1e-9 oZ F iE ov&&*iE ov>cX){df=h;cX=*iE ov oz W!df tN e=nz L as(*df).oK;rl hw(*df nL tU iE[*df]oQ)oZ qm st==mi oK)iE ov pa a6 si mK aP;}om dK{ao I pr ca;}pG hH lX void add(nf I B ca F I&&tK>oy gg lq{*I,ca}A lt<dK>df(rU gg lW lC lw*H max_element(gg oC gg mx I rL<b oD rL;}mI hK U oe mQ{hP(gg,[&](le&move tC move.ca==first||move.ca==second||move oD.nK first||move oD.nK second;}mI mT{gg.clear(A mA lf dK>gg rM lk sl ks oq eZ,class jp>bool d4(rN B f8 B ho,jp b6){hH iT o gY pb nZ oq all B th st<0 Q;ks bW tJ Y ca=nz st pz bW.ig()Q o aC pa bI(ca)rA Y&dR pa aU()lY ky=dR&&dR->iw si G ky F all){static const ak hn nE&I:dR->gt mD F L eB(I.oK qx O pb{oO eZ Q bW rL(I.M hn oF nh return bW rL(I.M hn,0,0 A(G O)iT.add(ao{h,I.M 0,0,*O},ca)pK oz qy qj oA),ca oo{mH e D e<L pQ S e oA),ca sn o jk pb{iT.mT lF h=f8;h<ho S h)gY si ss,0,0 A;jk()mE aP mt;nx le aR=iT.df()K I=aR->I o ca=aR->ca o hD=I tb lY fC pa eB(ca)lY fD pa eB(hD)lY iX=ca==hD lY hf=iX?L dz(I.h,I qU,I sP):(L hw(I.h)&&L mM I.h,hD,I qU,I sP nL aP mf f L eB(ca)!=fC||L eB(hD)!=fD){jk();rK}iT.hK(ca,hD)lF h=f8;h<ho S h F st<0 qx kh=nz st);gY si,kh==ca||kh==hD,ca,hD A nt aP;}lk nG aM::bc me bM oi pQ||bM ou cP+1,nK dc}o fA=br(qh),sx bM,ui,qh).bf(ep)ty cG-fA;}nG aM::O me bM oi pQ||bM>C rw-(nK dc?2:0)lC rj qh)lY ox 0 G iX F bM&&!pV eC(bM-1)+1)lC oP W!cy||!pV bM)lC;aT af sY G qk 0,bM)nh pm 0,bM nq J(ui nq J(nB bf(ep)G qk bM,b9)nh pm bM,b9 sC lw bc(of,bM A nf ct::b6 op ly aM py?2:0)lF a D a tj a K gi=bW.bc sF pz gi||*gi oV le O=bW rL sF ru ao{r,M a,a,*O};nt bh;}pB ct nT mG d4<aM ss>nn 0,L sy,[this]mc bJ ly aM pg au::au ry h):bK<false>tJ F!ig()Q o data=eq(nH,h);jO=cK(nH,ni aw);is=cK(nH,ni aq A nG au::bc me a B b oi pQ||a>b||b ou cO,nK dc}o fA=cQ(qh),sx a,b,ui,qh).bx(ep-de.sy)ty cG-fA;}nG au::O me a B b qt en=eO sF ty en?dE sF,b,*en):rJ;}lt<aT>au::eO me a oi pQ lC lY ox 0 G a>b9 lC rj qh Y dB=sv ep-de.sy].dB f dB>nB pS nM lC f iX F!dd(C,a)lC oP W!cy||!pV a)lC;aT af sY G qk 0,a)nh pm 0,a nq ex(jO sC lw af;}nG au::dE me a B b ly aT&en oM const bool ox 0 G a>b||b>b9 lC rj qh G iX F!dd(C,b)lC oP W!pV b)lC pD af=en f qk a,b)nh pm a,b nq ex(is G qk b,b9)nh pm b,b9 sC lw bc sF,b A nf dn::b6 op ly au py?2:0 Y&in=nH o qN(r)ny dY jY{C lP,pI,qu,nK ca}o aJ pa aJ(r Y f4=nB bx(r-L aB rI mV fX(cp+1),eL(cp+1),eJ(cp+1),bm(cp+2,rS)pC){fX og=bP ng sx pos,aJ);eL og=bP ng sx pos,f4);eJ og=br ng sx pos,aJ,f4 A mH tu bm og qJ eL og,bm[pos+1])lF a D a tj a K gq qJ eJ rO,fX rO+bm[a+1]G bW.c9()-gq oV le en=bW.eO sF pz en qe lF b=a;b tj b K fA=b==a?eJ rO:fX rO+eL[b]o gi=bW.c9()-fA f gi oV le O=bW.dE sF,b,*en ru ao{r,M a,b,*O};}nt bh;}pB dn nT mG d4<au,false>nn L sy,ml,[this]mc bJ ly au pg nO struct jF{ld e sm hT rM ol g0 ry e rV in=nH o&C td o k=nB mP pr ff D ol k5{0}lF pos D pos<C lP mX pos K J=C lP og f pp m)++ff um--ff rx ff&&pV pos+1))k5 lq pos+1 mK k5 oz bool dC nT qt&in=nH se lf jF>b5;b5 mY L cM()z)nE qj)b5 lq{M g0 nn oK})mE hf mt sZ b5 z&&!hf S i){mH j=i+1;j<b5 z&&!hf S j K bR=b5 oL.M bS=b5 pt tb o&a pa bI(bR Y&b pa bI(bS Y ia=a rw,ib=b rw N cX=1e-9 pr j7 qz j8=0 nE fN:b5 oL.hT){qy fO:b5 pt.hT K ki=fN?a lP[fN up:bR o kj=fO?b lP[fO up:bS N O=0 f fN<ia K ke=a lP[fN];O+=mk ki,ke)-mk kj,ke sq fO<ib K kf=b lP[fO];O+=mk kj,kf)-mk ki,kf sq O<=cX nM lB aT g3 ng bR,a);g3.dy(0,fN G fO<ib)g3.ex(b.dy ng fO,ib-1)pz g3 qn lB aT g4 ng bS,b);g4.dy(0,fO G fN<ia)g4.ex(a.dy ng fN,ia-1)pz g4 qn lB cX=O;j7=fN;j8=fO oz W cX>oy{rl cZ(bR,j7,bS,j8 nL aP=hf mf oz}W!hf Q aP oz lk lt<bz>aY::dZ op oM eg(ad);f6(ty dx lW?rJ:lt(dx.top()oD mI aY::f6(oM nx!uy(Y e=qw.aQ tb o aA=qw.aQ.h f ej[aA]&&qv dT==b4 lR&&qv b1==bw lR[aA].b1 tx dx.pop(A qV aY rq&b rU a oD rL!=b oD rL Q a oD rL<b oD rL o key=[]lv bz&c tC H tuple(c.aX,c.aQ.h,c.aQ qU,c.aQ sP A lw key(a oD)>key(b oD mI aY::cJ ry pA aX mU aA rU aA lW Q uo row=bw lR tG aA){++row ov.b1;row ov oD oQ tI&g8 td sm X tG aX K qN mD uo gL=kS ov f gL.fj!=g8 rA)){X=g8 lP;oc);gL.dH.gD(nH,M X);gL.a6=bZ(nH,M g8 uh-L aE mD;gL.fj=g8 rA A qy hx:aA ue aR=row[hx];lH fl=rY?rY nF:1e-9 o aQ=bs nn hx,M gL.dH,fl-gL.a6 G aQ&&aQ nF+gL.a6>fl)rY=bz{h,*aQ,aQ nF+gL.a6}oz qy h:aA)fQ(M h mI aY::fQ U e B aA qt&aR=bw lR[aA]f rY){dx.push({*rY,b4 lR,aR.b1}A on aY::eg op qt Z pa pQ,E=ml f gl!=&nH||b4 z!=Z||ej z!=E){gl=&nH;b4.oj Z,tW nL ej.oj E,ph;bw.oj Z,lf ez>(E nL dx=iP{};kS mu E A lf lE>>aX(Z)sm aA,ew lF h D h<E S h K qN mD lY jn=mq e<0 f jn){aA lq h pz ej ov)ew lq h oo aX[mq e]lq h);ej ov=jn;}mH e D e sB e K bn td rA G b4 lR!=bn){b4 lR=bn;cJ nn M aX lR,aA oo W!ew lW){cJ nn M aX lR,ew sn W dx z>2*Z*E+32)f2(mI aY::f2(oM dx=iP{}mO<bw mX oK mH aA D aA<ej mX aA F ej[aA])fQ(M aA A qV aY qI rl fk(I->aX,I->aQ.h,I->aQ qU,I->aQ sP nL tU nt aP;}lk nO bool j9 lv be&a ly be&b tC a rL>b rL||(a rL==b rL&&H tie(a.mZ a.mQ<H tie(b.mZ b.mQ A nG es ry aX B aA K qN(aX Y sH&C td rj nH lY bd=aA<L aB(Y aq=bd?nB bf(aA):nB bx(aA-L aB rI bT af sY nq cA(0,pI nq fZ(L aJ(aA)G bd)af.fZ(aq nq cA(pI+1,qu pz bd)af.fZ(aq nq cA(qu+1,C rw ty af.a0(sn nG eF ry th first>=ml||second>=ml lC sL mQ o a pa as(oN,b pa qS a.nK b tb lC o gM=es nn mZ second pz gM lC o gN=es nn second,first pz gN lC lw L bI(a.oK qo+L bI(b.oK qo-*gM-*gN;qV cr rq&b mG j9(b oD,a oD mI cr::fQ U bR B bS qt&aR=bw[bR][bS]f rY)dx.push({*rY,bR,bS,ch[bR],ch[bS],aR.b1}mI cr::eg op qt Z pa aK(G ah!=&nH||ch z!=Z){ah=&nH;ch.oj Z,tW nL bw.oj Z,lf ez>(Z nL dx=iP{};}ol aP oW bool>kL(Z,ph mO sB e K bn td rA G ch lR!=bn){ch lR=bn;aP lq oK;kL lR mf oz W aP lW Q tk E(Z)oZ K e pa nw f e qc)E lR lq h A qy e:aP){mH c6 D c6 sB c6 F c6==e||(kL[c6]&&c6<oK qx bR qJ M c6),bS=mv M c6)uo aR=bw[bR][bS]S aR.b1;rY oQ)nE first:E[bR]){qy second:E[bS]K O=eF nn mZ second pz O||*O qd lB const be I{lV mZ mQ,mv mZ mQ,*O}rx rY||j9(I,*rY))rY=I oz fQ(bR,bS sn W dx z>2*Z*Z+32){dx=iP{}lF a D a sB a)mH b=a+1;b sB b)fQ(a,b sn lt<be>cr::dZ op oM eg(ad);nx!uy(G qv jW==ch[qv bR]&&qv jX==ch[qv bS]&&qv b1==bw[qv bR][qv bS].b1 Q qw;dx.pop(mK rJ;qV cr qI rl gm(I->mZ I->mQ);tU nt aP oz int main(K kF=li;H ios::sync_with_stdio(ph;H cin.tie(nullptr Y ag=bb::gk(H cin);bb::dV::ij bN;bN.kp=1ULL;lH kv=rB(li-kF).count();lH hd=300;lH reserve=hd>2?1.0:hd*0.1;bN.dm=hd-kv-reserve f bN.dm<=0){bb::sM uz,empty,ag oo{le ax=bb::dV::kT(ag,bN uz,nN bQ,ag mK 0;}
