#pragma GCC optimize("O3","unroll-loops")
#include <cstddef>
#include <limits>
#include <algorithm>
#include <vector>
#include <stdexcept>
#include <string>
#include <cstdlib>
#include <iosfwd>
#include <array>
#include <charconv>
#include <iomanip>
#include <istream>
#include <ostream>
#include <string_view>
#include <cmath>
#include <sstream>
#include <cstdint>
#include <optional>
#include <atomic>
#include <memory>
#include <utility>
#include <random>
#include <chrono>
#include <queue>
#include <map>
#include <fstream>
#include <iostream>
#include <numeric>
#include <functional>
#include <tuple>
#define la std::size_t
#define lb const auto
#define lc std::vector<
#define G std::
#define A .size()
#define lf G chrono::steady_clock::now()
#define lg G numeric_limits<double>::infinity(
#define R )return
#define li ;for(
#define lj }namespace bs::ez{
#define lk )const
#define ll static_cast<
#define lm G optional
#define X .push_back(
#define z ;if(
#define lq .begin()
#define lr (const
#define ls ;return
#define N ;double
#define lu double>
#define lv ,const
#define lw continue;
#define lx {return
#define ly G uint64_t
#define lz const double
#define lA G chrono::duration_cast<e1::duration>(G chrono::duration<lu
#define lB G nullopt
#define F ){if(
#define lD lc la
#define lE li la
#define lF li auto
#define lG const bool
#define lH .end()
#define M ac.
#define lJ .W()
#define I ){lb
#define H ,la
#define e ;lb
#define lN R false
#define o =0;
#define B );}
#define lQ )if(all||h==first||h==second)jn.add(cD(ac,b3,r,h,M bk(h)
#define lR G make_unique
#define lS [h]
#define lT G min(
#define lU G chrono::
#define lV .empty()
#define lW =true
#define lX {public:
#define lY lH,[](lb&a,lb&b)lx a.
#define U ;++
#define Q (la
#define mb (void)
#define mc loc.
#define md ll int>(
#define me G shared_ptr<const aP>
#define mf ab&ac
#define D )z
#define mh ;bool
#define mi =false
#define mj bk(h)
#define mk if(
#define ml unsigned char
#define mm )break
#define mn in.cH(
#define mo G max(
#define mp M av(
#define mq M aj()
#define mr ,double
#define ms final:public c0 lX lD>h1(ab&H,G mt19937_64&lk override;};lj lD>
#define mt h D!b3.hK()R e aG=M bk(bg).bp()e&dm=M aT();lG iM=dm&&dm->hQ(r D iM F all){static const ak fV li lb&J:dm->fR(r)F M co
#define mu private:
#define mv .resize(
#define mw ;auto&loc=bm[f];mc h=md h D L==aE(f))mc ax=p;else mc bH=
#define mx mh aR mi;while(lb aO=jn.dp()I J=aO->J e bg=aO->bg e eq=J.h;lG cM=M co(bg);lG cN=M co(eq);lG iB=bg==eq;lG gx=iB?M
#define my G numeric_limits
#define mz insert(
#define mA G move(
#define mB h lv ak&
#define mC );read(ed,
#define mD ,0,0,*K},bg);break;}}for(lb h:M cB()lQ,aG),bg B else{for Q h o h<M aH()U h lQ,aG),bg B}e gr=[&]{jn.clear
#define O h,
#define mF b3 H r H mB C,ly aG lk{lb bg=ll la>(mp r).h D h!=bg&&!b3.dP()R lB ls fJ.get(r,O M aH(),aG,C.bp(),b3.cF(),h
#define mG true,0,0 B;gr()mx cE(J.f,J.ax,J.au):(M g2(J.f)&&M mz J.f,eq,J.ax,J.au));aR lW z M co(bg)!=cM||
#define S [r]
#define mI ;void
#define mJ lr az&
#define mK ;struct
#define mL lk lx
#define mM B void
#define mN ;mutable
#define mO .erase(
#define mP for Q
#define mQ G isfinite(
#define mR lE h o h
#define mS in.
#define mT auto&
#define mU B return
#define mV first H
#define mW A U
#define mX final:public jt lX bool cb(
#define mY lr lc bu>&al H Y H E lv lc int>&aU
#define mZ first,
#define na lj class
#define nb (f)
#define nd lv lD>&
#define ne =G min_element(hO lq,hO lY second.Z<b.second
#define nf lm<lu
#define ng ll la>(
#define nh c1{bool operator()lr bb&lv bb&lk;};using iK=G priority_queue<bb,lc bb>,c1>mN const
#define ni af.dn(
#define nj lU steady_clock::time_point
#define nk lm<an>
#define nl lE r o r
#define nm .duration
#define nn .h
#define no }return
#define np h1(mf H count,G mt19937_64
#define nq =mS Y A,n=mS aw A,m=mS aX A
#define ns g9);fa.cC(0,f2 A);fb.cC(0,f3 A)e gQ=fa.a0(),gR=fb.a0(D!gQ||!gR lN;lz K=
#define nt ;const
#define nu e1::now()+lU seconds
#define nw ;}double
#define nx (in,
#define ny lF r:
#define nz ls true;}
#define nA Q mB C H
#define nB {double
#define nC lc lu
#define nD .clear(
#define nE M ah()
#define nF G lower_bound(aD lq,aD lH
#define nG lr mf H f)mh hK(mL hz;}bool dP(mL bI.has_value(B double cF(mL bB;}la b8 Q h
#define nH namespace bs
#define nI li lb
#define nJ ;else
#define nK while(
#define nL .first
#define nM R lB
#define nN ->K
#define nO ));
#define nP mp r)nn
#define nQ [&](auto a,auto
#define nR (ac,
#define nS +1e-8<mo 1e-9,bo?bo nN:1e-9))lw lb
#define nT aA.
#define nU (*ag,
#define nV lc G pair<double lv fF*>>eh nI&[key,c]:dh lS)nB e0=c.Z ny key)e0-=
#define V )e
#define nX .reserve(
#define nY G accumulate(
#define nZ )==0&&lf>=
#define oa lk{
#define ob }void
#define oc aV S
#define od lk override;
#define oe oa mk!hK()||h>=ci.aH
#define of value=
#define og mJ in H
#define oh lr ab&
#define oi lr mf H
#define oj C mO C lq+mc bH);C mO C lq+mc ax
#define ok W mO W lq+mc bH);W mO W lq+mc ax
#define ol [pos]
#define om i o i<
#define on ,eh A);G partial_sort(eh lq,eh lq+count,eh lY first>b nL;}
#define oo for(
#define op Q f
#define oq .E)
#define os Y lS.bd+1e-8
#define ot op oa lb n=ag->aw A ls f<n?ag->
#define ou lG iB=h==b6 e ch=C lJ A-(iB?
#define ov bP.gJ nU O C);aQ af nU O bP);af.cC(0,C A V cH=af.a0(D!
#define ow U r F nP<0)lw lb iy=ng nP);dI(r,iy==bg||iy==eq,bg,eq B no
#define ox namespace{
#define oy ,bool
#define oz 1e-9)
#define oA [f]
#define oB !M mz J->f,J->O J->ax,J->au nO add nR J->O false
#define oC lD>
#define oD Y A
#define oE lj struct
#define oF O C
#define oG mZ second
#define oH aS am
#define oI jN+0x9e3779b97f4a7c15ULL*
#define oJ ;}}
#define oK ::b8 Q h oa lb size=ci.mj lJ A ls h==b6?size-2:size;
#define oL .J
#define oM ::cb(mf oa fH jn e dI=[&]Q r oy all H mV second F nP<0
#define oN data.
#define oO ;}mk
#define oP ,0)
#define oQ +lU milliseconds(
#define oR .Z
#define oS [i]
#define oT ;class
#define oU .second
#define oV ){auto
#define oW .reset(
#define oX mp f)nn
#define oY lE j o j
#define oZ lJ,mc ax,mc
#define pa C.mz C lq+
#define pb cx[f*er+h]
#define pc lq,
#define pd aw A
#define pe G shared_ptr<a9
#define pf bi(aI&af H mV end
#define pg lE f o f<mq U f
#define ph h)
#define pi ab::
#define pj )/ll aS::duration::rep
#define pk ;lc
#define pl ;la
#define pm mJ ah
#define pn const c2 eU(al,Y,E);e3 kg(eU);kg.kd(am);
#define po ;hm.i0(am);nT fk+=hm.fk z hm.bU){nT bU lW ls
#define pp h==b6?2:0)||(h!=b6&&!bI)nM nt d6 jQ{C lJ
#define pq false)
#define pr mS aX
#define ps ;auto
#define pt B else
#define pu al A
#define pv ::max(
#define pw )lw lb
#define px lf>=am
#define py [&]Q
#define pz );af.
#define pA oo auto r:
#define pB dd A&&!aZ U
#define pC L>=k+2*n&&L<k+2*n+
#define pD b3 nR r V bg=ng mp r).mt(J nn)pw K=b3.K(J
#define pE assign(
#define pF [j]
#define pG G unique_ptr<
#define pH G sort(
#define pI M ao(
#define pJ [c nn]
#define pK bv.eN(lR<
#define pL mT C=cP;C=mj
#define pM lE pos o pos<=b8 U pos
#define pN <ao(D a>b||b>C A||(aJ&&a!=b)lN e aB=
#define pO &mF==bg,[&]{nk bo e b8=C lJ A-(h==bg?2
#define pP U r)dI(r,mG M co(eq)!=cN){gr();lw}jn
#define pQ D!
#define pR cv->
#define pS G istream&
#define pT ()=default
#define pU G min<la>(
#define pV ah.
#define pW )mk
#define pX h nd
#define pY +1e-8
#define pZ ag->
#define qa inline double
#define qb dh A
#define qc value-=
#define qd al lS
#define qe ;}const
#define qf .data()
#define qg e1::now(
#define qh );G iota
#define qi C.aB nx
#define qj nl<nr U
#define qk cK X lR<
#define ql L-k-2*
#define qm cx[bT*er+f]
#define qn lE u o u<nv U u
#define qo ll lu(jZ S)-1
#define qp oC bC;bC nX count);nK bC A<count){la
#define qq >=0
#define qr =lT
#define qs bK.
#define qt =M
#define qu )continue
#define qv ;break
#define qw ;}la
#define qx bw H a2
#define qy )nz bool pi
#define qz ;G mt19937_64 rng
#define qA ci.ah(
#define qB (G ostream&out lv
#define qC iB)bi(af,
#define qD lE bD o bD<10000 U bD F(bD&7 nZ am)
#define qE ::cb(mf oa bool aR mi;nK lb J=d7(ac
#define qF lH,
#define qG oa lb
#define qH lE om
#define qI oa mk
#define qJ B bool
#define qK )||
#define qL ny c oq
#define qM .bA()
#define qN .cH()
#define qO ->hQ nb mR<er U ph mk pb.bp!=M mj.
#define qP loc=
#define qQ lJ A
#define qR bX oL
#define qS G function<void
#define qT =nY aV pc aV qF 0.0)
#define qU z second<first)G swap(oG V a=
#define qV second D a nn<0||b nn<0||a nn==b
#define qW oJ mk J==mq||!M g2(J)mm;bC X
#define qX lF id:aU pW id qq)nT Z+=al[id]oR
#define qY bX.
#define qZ mb 0
#define ra ;};
#define rb [first]
#define rc =mo 0.0,
#define rd mX ab&od mu mutable ic fJ;nk cD oh
#define re ]pW ds(a8)){hB+=a8 oR qv oO hB<=
#define rf qI h>=ci.aH()nM ls K(O ci.mj
#define rg L<k+2*n?(L-k)%n:n+(ql n)%m
#define rh }bool
#define ri mV second
#define rj mu struct
#define rk oo lb
#define rl ->second
#define rm count=
#define rn br S
#define ro mn be,
#define rp .pop_back(
#define rq G stable_sort(
#define rr op H h H
#define rs ;lG aJ=
#define rt e&in=
#define ru mb(!M
#define rv mS aW nb:mS a1(f
#define rw .h1 nR bC,rng)nJ
#define rx ::c1::operator()lr bb&a lv bb
#define ry dj nD)lF&gk:dJ)gk nD);jh o gv
#define rz D at<C A I fE=C.L(at);fC
#define rA -iO+(pos<b8?mn au,W ol):0.0
#define rB hO A>gT I kx ne oR;});hO mO kx)
#define rC oX<0 R e&qP mp f);b6=ng mc ph
#define rD b6);bB=bE(nE,b6,C oZ bH B la
#define rE *0xbf58476d1ce4e5b9ULL;j6
#define rF D K&&(!bo||*K>bo nN))bo=
#define rG ,0 D K)jn.add(an{r,J nn mD
#define rH .ax
#define rI mc ax
#define rJ z!
#define rK lU duration<lu
#define rL z K<=1e-9 lN;
#define rM r pW nP qq)b9 X r
#define rN G min_element(bS pc bS qF kP
#define rO >nT Z+oz{nT Z
#define rP I be=at?C.L(at-1):h N
#define rQ ,b8 D af qM)iZ(aA,{f,O a,
#define rR z cX[a8]qq I r=ng cX[a8]);
#define rS lk;nf
#define rT [a]
#define rU ax)+mn ax,
#define rV h:M cB()
#define rW lk;mu const ab&ci pl dE,b6,
#define rX oi f):ci(ac),dE nb,b6(0)
#define rY append(
#define rZ lg)
#define sa o bool
#define sb I&
#define sc mc h
#define sd n_ U
#define se nT bf
#define sf aO oL
#define sg =0,
#define sh =de.
#define si .bp(
#define sj Q h H
#define sk ;}else
#define sl pk int>
#define sm ny a8 oq
#define sn >=aj(qK
#define so rng()%
#define sp aR;}lj
#define sq mh aR mi li;;){
#define sr dr Q b0,ak g8 H b1,ak g9 mr
#define ss nC em lr lc bu>&al H Y H E
#define st (nO
#define su .K
#define sv G shuffle(E pc E qF rng
#define sw qL mk iz S qq&&!hR[iz S]
#define sx ;oC
#define sy )mI
#define sz =nullptr
#define sA aM lS
#define sB ==md
#define sC lr mf
#define sD bQ->
#define sE )lw
#define sF aL::
#define sG aU lS
#define sH o}mK
#define sI ;s.
#define sJ (ag,
#define sK O jQ,
#define sL pk ml>
#define sM <al mW
#define sN <count U
#define sO <Y U
#define sP pQ af qM nM
#define sQ *nv+q
#define sR +lU seconds(
#define sS (oF,a
#define sT og h lv d6&C
#define sU ,qs dx*0.05)
#define sV >K){K=
#define sW (gc+1,Z
#define sX bz empty;empty.cf mv pV
#define sY nK cq<iS&&ll ml>(hk[cq
#define sZ mS cp nb:mS df(f-n V au
#define ta ;lz jM[]={1.0,0.8,1.2
#define tb D ek<end)ni ek+2,end+
#define tc ?(L-dt)%n:n+(L-dt-
#define td B}
#define te [t]
#define tf mC p rH.cS mC p rH.ee mC p
#define tg ]lF&ks:aV)ks*=jp;qZ
#define th a<pB a)mP b=a+1;b<pB b
#define ti fj[b]ls x!=y?x>y:a<b
#define tj value)
#define tk (mL
#define tl aX A
#define tm nx oF
#define tn B mk
#define to mS pd
#define tp mN lc
#define tq mc bH
#define tr B lb
#define ts nT dp
#define tt qt mj
#define tu 1e-9 R
#define tv lW ls
#define tw aq::
#define tx =0 N
#define ty -1)
#define tz f H mB
#define tA ,true)
#define tB G abs(
#define tC <=b8 U
#define tD pk lD>>
#define tE +2*n
#define tF fE[g7]
#define tG [Y pF]
#define tH sE bool j4 lW
#define tI sb c=al
#define tJ G max<la>(
#define tK a3(fe.aT st
#define tL db[row*n_+
#define tM :pr[f-n].
#define tN =W[b4+1];iC+=mn au
#define tO pa b,cg nb);pa a,aE nb
#define tP +ao(B rJ aB(a qK!aB(
#define tQ pos=b8+1;pos-->0;)
#define tR ;no nw hB=Z lE d=gc;d
#define tS mm;
#define tT L)
#define tU .au
#define tV F first
#define tW nI L:
#define tX e h=
#define tY nI f:
#define tZ lJ lq+
#define ua cb(mf
#define ub (nr oP
#define uc mS b7[
#define ud 1e-9&&(
#define ue qF nQ b
#define uf add nR O
#define ug G fill(
#define uh aR lW;
#define ui unsigned long long>
#define uj pV oD>48
#define uk nl<mq U
#define ul nc=pR pu
#define um *my<lu::epsilon()
#define un D first<gj)ni mZ gj
#define uo bk(b nn)qN-*gQ-*gR
#define up template<
#define uq )ls
#define ur pm H
#define us ;lz
#define ut (f
#define uu J nN
#define uv N of
#define uw mu const az
#define ux a H b
#define uy aH()
#define uz c7 Q b0 H fT H b1 H
#define uA tp lc eF>>bL mN iK dH
#define uB a8 qG ul z a8>=nc R
#define uC ,n=ao(),m=pZ tl e
#define uD ),p rH.cS,p rH.ee,
#define uE .aF
#define uF lm<
#define uG =mo
#define uH )lx
#define uI -1]
#define uJ =1;
#define uK cu Q O ak&eu mr K
#define uL mh hz mi;nf bI N bB
#define uM H tz,ly aG lk ra na
#define uN 31 nZ bt mm e J=bG
#define uO F aZ R U ef z(ef
#define uP oJ mk cJ==nc+n_ R
#define uQ dH lV sb bX=dH.top
#define uR );bs::eE(G cout
up class T,class P>void hJ(lc T>&fq,P jI){fq mO G remove_if(fq pc fq qF jI),fq lH B inline la hA Q tj lx value?my<ui::digits-__builtin_clzll(ll ui(tj):0;}nH{struct iX nB cS tx ee sH kX nB bd sH ju nB aF o iX ax N dw sH kz nB aF tx dq o iX ax;iX au N dw sH az{lc kX>Y pk ju>aw pk kz>aX sx fA;nC b7;nC es pl gG tk oD+2*(pd+tl B la iH Q L mL fA lV?L+1:fA[L]qw cp Q i mL oD+i qw aW Q i mL oD+pd+i qw df Q j mL oD+2*pd+j qw a1 Q j mL oD+2*pd+tl+j nw jq Q u H v mL b7[u*gG()+v]nw cH Q u H v mL es[u*gG()+v]oJ;}nH{struct hM{bool bA mi N c3 o G string kY;}mK bz{lc lD>>cf;hM fw pm lk ra}nH{hM bz::fw pm qG n=pV pd,m=pV tl e dt=pV oD pk bool>b9(n+m,pq N c3=0 lE k o k<dt U k){la be=k tW cf[k]){c3-=pV cH(be,L D L<dt+n)b9[L-dt]lW nJ mk L>=dt tE&&L<dt tE+m)b9[n+L-dt-2*n]lW;be=L oJ mP om n+m U i pW b9 oS)c3+=i<n?pV aw oS uE:pV aX[i-n]uE ls{true,c3,{}}oJ nH{az gq(pS in sy jR qB az&ah B nH{ox class dU{pS j7;G array<char,65536>hk{}pl cq sg iS o G string d3 mh ke(){iS=ng j7.rdbuf()->sgetn(hk qf,hk A nO cq=0 ls iS!=0;}public:explicit dU(pS ed):j7(ed){}G string_view fE(){d3 nD)li;;F cq==iS&&!ke()R{};sY])<=' ')++cq z cq<iS tS}oo;;I ha=cq;sY])>' ')++cq nt G string_view jH(hk qf+ha,cq-ha D cq<iS F d3 lV R jH;d3.rY jH qf,jH A uq d3;}d3.rY jH qf,jH A pQ ke()R d3 oJ};up typename T>void read(dU&ed,T&value oV jr=ed.fE(pQ jr lV&&jr.front()=='+')jr.remove_prefix(1);G from_chars(jr qf,jr qf+jr A,value mM read(dU&ed mr&tj{of G strtod(G string(ed.fE()).c_str(),nullptr td az gq(pS in){dU ed(in)pl k sg n sg m o read(ed,k mC n mC m)nt la v=k+2*(n+m);az ah;pV Y mv k);pV aw mv n);pV aX mv m);pV fA mv v V iG=py tT{mT id=pV fA[L];read(ed,id B mR<k U ph{iG(h mC pV Y lS.bd B mP om n U i){mT p=pV aw oS;iG(pV cp(i nO iG(pV aW(i)tf.dw mC p uE B mP j o j<m U j){mT p=pV aX pF;iG(pV df(j nO iG(pV a1(j)mC p.dq tf tU.cS mC p tU.ee mC p.dw mC p uE B oC h6(v)lE L o L<v U tT h6[pV iH(tT uI=L lF*eU:{&pV b7,&pV es}){eU->resize(v*v)lE u o u<v U u)mP w o w<v U w)read(ed,(*eU)[h6[u]*v+h6[w]]mU ah oJ nH{bz jX(pS in lv az&ah sy eE qB bz&bZ lv az&ah B nH{void eE qB bz&bZ lv az&ah I gC=bZ.fw(ah);out<<G setprecision(17)<<gC.c3<<'\n' nI&C:bZ.cf){out<<C A tW C)out<<' '<<pV iH(tT;out<<'\n' oJ oE bV{la first sg g7 tx duration tx hI=-rZ N bj=rZ N cH tx bR sg dZ sg dY sa bA lW ra bV b5 ur tT;bV kG pm lv bV&k2 lv bV&k0)oT ak lX ak pT;ak ur pX C sy gJ ur pX C sy clear(sy cO lr ak&de);bV dn ur mV g7 lk mh aB ur ba lk N cH tk jh;}ly bp tk gv qe oC&W tk ef;}mu oC ef pk bV>dj pk lc bV>>dJ N jh o ly gv o}oT aQ lX aQ ur mB bk sy cC Q mV end sy fQ lr bV&eo sy ft Q tT;nf a0(lk;uw&ag pl iF nt ak&fJ;uF bV>dl ra lj ox G atomic<ly>gn{1}N dw og L I k nq z L<k tE R mS aw[(L-k)%n].dw ls pr[(ql n)%m].dw nw jj og L I k nq z L<k tE R 0 e j=(ql n)%m ls L<k tE+m?pr pF.dq:-pr pF.dq oJ bV b5 og tT{bV s sI bj=my<lu pv);s nL=s.g7=L;s nm=dw nx L V k nq nt iX*h0 sz z L<k+n)h0=&mS aw[L-k]rH nJ mk pC m)h0=&pr[ql n]rH nJ mk L>=k tE+m)h0=&pr[ql n-m]tU z h0){s.hI=h0->cS+s nm sI bj=h0->ee;}s.bR=jj nx tT sI dZ qr 0.0,s.bR)sI dY rc s.bR uq s;}bV kG mJ in lv bV&a lv bV&b){lz eY=mS jq(a.g7,b nL);bV s;s nL=a nL sI g7=b.g7;s nm=a nm+eY+b nm sI hI uG a.hI+eY+b nm,b.hI)sI bj qr a.bj,b.bj-eY-a nm)sI bA=a.bA&&b.bA&&a.hI+eY<=b.bj sI cH=a.cH+mn a.g7,b nL)+b.cH sI bR=a.bR+b.bR sI dZ qr a.dZ,a.bR+b.dZ)sI dY uG a.dY,a.bR+b.dY uq s;}ak::ak og pX C){gJ tm mM ak::clear(){ef nD);ry o ob ak::cO lr ak&de F this==&de R;ef sh ef;dj sh dj z dJ A<de.dJ A)dJ mv de.dJ A)qH de.dJ mW i)dJ oS sh dJ oS lE i sh dJ A;i<dJ mW i)dJ oS nD);jh sh jh;gv sh gv;ob ak::gJ og pX C F&C!=&ef)ef=C;ry=gn.fetch_add(1,G memory_order_relaxed V size=C A;dj nX size)pl be=h tW C){dj X b5 nx L nO jh+=ro tT;be=L qw f1=0 lE kF uJ kF<size;kF*=2,++f1 F f1==dJ A)dJ.emplace_back();mT gk=dJ[f1];gk mv size)nt la kB=kF*2 lE ha o ha<size;ha+=kB I mid qr ha+kF,size V end qr ha+kB,size D mid>ha){gk[mid uI=dj[mid uI lF i=mid-1;i>ha;--i)gk[i uI=kG nx dj[i uI,gk oS tn mid<end){gk[mid]=dj[mid]lF i=mid+1;i<end U i)gk oS=kG nx gk[i uI,dj oS td mk kF>size/2 mm oJ bV ak::dn og mV g7 qI first==g7 R dj rb e gk=hA(first^g7)-1 ls kG nx dJ[gk]rb,dJ[gk][g7]qJ ak::aB og pos qI pos>ef A lN z pos==0 R true e fl=ef[pos uI,k=mS oD ls fl<k||fl>=k+to;}aQ::aQ og mB bk):ag(in),iF(ph,fJ(bk){ob aQ::cC Q mV end tV==end R;fQ(fJ.dn sJ mZ end ty mM aQ::fQ lr bV&eo){dl=dl?kG sJ*dl,eo):eo;ob aQ::ft Q tT{fQ(b5 sJ tT B nf aQ::a0(qI!dl R 0.0 e&s=*dl us e5=ag.jq(iF,s nL pQ s.bA||e5>s.bj||s.dZ<-1e-8||s.dY>ag.Y[iF].bd pY nM ls ag.cH(iF,s nL)+s.cH;}na aP oT a9 mK e4{int h=-1 pl ax sg bH o}oT ab lX ab pm,bz bZ,pe>bO={}sy cO oh de);bz bZ(lk nt ak&bk Q h lk nt oC&cB tk aD;rh co Q h mL h<aM A&&!sA lJ.empty(B la aH tk pZ oD qe e4&av op mL bm oA qw aj tk bm A qw ao tk pZ pd qw g3(lk N c3 tk dW;}a9&bO tk*da qe pe>&bc tk da nw aF op lk nt az&ah tk*ag qw aE op lk nt me&aT tk cI;ob a3(me dm){cI=mA dm B nf a6 op rS fL rr qx lk mh g2 op)mh ej Q pX E)mh insert rr qx)mh cE op H qx)mh fn Q a5 H aC H qx)mh gt Q ri)mh uz fU);uw*ag;pe>da pk ak>aM sx aD pk e4>bm mK ie{ly d1 o nf K;}tp ie>dD;me cI N dW o oC cP;ak bP mI di Q pX C sy uK sy sr K)pl cg op lk ra na a9 lX struct kk nB*ax N*au N*jT N*kM ra explicit a9 Q E):gM(hC(E)),jd(4*gM){}la jD tk gM-2;}kk gA Q kt oV*data=jd qf ls{data,data+gM,data+2*gM,data+3*gM};}mu static la hC Q E uH 2*E+2 qw gM;nC jd ra na aI lX aI og mB C):in_(in),iF(ph,kh(C){ob dn Q mV end tV<end)rY kh.dn(in_,mZ end ty mM L Q id){rY b5(in_,id)mM eo lr bV&fE){rY fE qJ bA(qI!dl R true e&s=*dl ls s.bA&&in_.jq(iF,s nL)<=s.bj&&s.dZ>=-1e-8&&s.dY<=in_.Y[iF].bd pY;}mu void append lr bV&fE F!dl){dl=fE ls;}mT s=*dl us eY=in_.jq(s.g7,fE nL)us gH=s.bR us gX=s nm us ix=s.hI sI bA=s.bA&&fE.bA&&ix+eY<=fE.bj sI bj qr s.bj,fE.bj-eY-gX);s nm=gX+eY+fE nm sI hI uG ix+eY+fE nm,fE.hI)sI dZ qr s.dZ,gH+fE.dZ)sI dY uG s.dY,gH+fE.dY)sI bR=gH+fE.bR sI g7=fE.g7 qe az&in_ pl iF nt ak&kh;uF bV>dl ra oE d6{const oC&W pl hF sg g5 sa bC mi pl size tk W A-(bC?2:0 B la L Q ba qI!bC R W[ba]z ba<hF R W[ba]z ba<g5-1 R W[ba+1]ls W[ba+2]oJ;qa bE og pX W H dg H b4 I ax=W[dg],au=W[b4]e dV=dg?W[dg uI:h z b4==dg+1)nB iC=mn dV,rU au D b4+1<W A I kA tN,kA)-mn dV,kA mU iC;}lb gO=W[dg+1]e e8=W[b4 uI N iC=mn dV,rU gO)-mn dV,gO)+mn e8,au D b4+1<W A I fI tN,fI)-mn e8,fI mU iC;}qa bY sT H at H L rP fC=ro L rz+=mn L,fE)-ro fE mU fC;}qa bF sT H at H ax H au rP fC=ro rU au rz+=mn au,fE)-ro fE mU fC;}qa cZ sT H dg H b4 H ax H au F dg==b4 R bF tm,dg,ax,au uq bY tm,dg,ax)+bY tm,b4,au B lj pi ab mJ in,bz bZ,pe>bO):ag(&in),da(bO?mA bO):G make_shared<a9>(to+pr A)),aM(mS oD),bm(to+pr A),dD(bm A){aD nX mS oD V gC=bZ.fw(in);dW=gC.c3 lE k o k<bZ.cf mW k pW!bZ.cf[k]lV)di(k,bZ.cf[k]mM pi cO oh de F this==&de R mR<aM mW ph mk sA si)!=de.sA si))sA.cO(de.sA);aD nX aH st aD sh aD;bm sh bm;dD sh dD;cI sh cI;dW sh dW;}bz pi bZ(oa bz aA;nT cf mv pZ oD)nI h:aD)nT cf lS=mj lJ ls aA qe ak&pi bk Q h oa static const ak empty ls h<aM A?sA:empty qw pi g3 tk G count_if(bm pc bm qF[](lb&av uH av nn qq;}B double pi aF ot aw oA uE:pZ aX[f-n]uE qw pi aE ot cp nb:pZ df ut-n B la pi cg ot aW nb:pZ a1 ut-n mM pi di Q pX C sb in=*ag e n=to,m=pr A,dt=mS oD tW mj lJ I f=L<dt tE tc 2*n)%m;bm oA={};}mT jY=sA;lG fB=co(h D C lV F fB){jY nD);aD mO nF,ph td else{jY.gJ tm pQ fB)aD.mz nF,ph,h B mP p o p<C mW p I L=C[p]rs L<dt tE e f=aJ tc 2*n)%m mw p oJ nf pi a6 op qG qP av ut D sc<0 nM tX ng sc V&fh=mj;mT bX=dD oA z qY d1==fh si)R qY K e size=fh qQ;aI af nU O fh);ni 0,rI);ni rI+1,tq);ni tq+1,size);qY d1=fh si);qY K=af qM?nf(bE nU O fh oZ bH)-aF nb):lB ls qY K;}nf pi fL rr ux qI f sn av nb nn qq||h>=uy nM e&el=mj e size=el qQ rs f<pZ pd z a>b||b>size||(aJ&&a!=b qK!el.aB nU a qK!el.aB nU b)nM rJ aJ&&pZ aX[f-pZ pd].dq>pZ os nM;aI af nU O el);ni 0,a pz L(aE nb pQ aJ)ni a,b pz L(cg nb);ni b,size sP nt d6 jQ{el lJ}e cH=aJ?bF nU sK a,aE nb,cg nb):cZ nU sK a,b,aE nb,cg nb uq aF nb-cH;rh pi g2 op I K=a6 ut pQ K lN e qP av nb tX ng mc ph;pL lJ;oj);dW+=*K;di(oF qy ej Q pX E F h>=aH(qK E lV lN e&old=mj pk bool>ge(old qQ,pq N gU=0 tY E F f>=aj()lN e qP av ut D sc!=md ph||ge[rI]lN;ge[rI]=ge[tq]lW;gU+=aF ut B aQ af nU O old);mT C=cP;C nD)lE first o first<old qQ;F ge rb){++first;lw}auto end=first+1;nK end<old qQ&&!ge[end])++end;af.cC(mZ end);C.mz C qF old tZ mZ old tZ end);first=end;}lb i7=af.a0(pQ i7 lN;dW+=old qN-*i7-gU;di(oF qy insert rr ux I K=fL ut,O a,b pQ K lN;pL lJ;tO);dW+=*K;di(oF qy cE op H ux F f sn av nb nn<0 lN tX ng av nb nn V h9=mj qN;pL lJ e qP av nb;oj)rs f pN py ba F!ba R true e fl=C[ba uI,k=pZ oD ls fl<k||fl>=k tP b)lN rJ aJ&&pZ aX[f-ao()].dq>pZ os lN;tO);ov cH lN;cu(O bP,h9-*cH qy fn Q a5 H aC H ux F a5 sn aC sn av(a5)nn<0||av(aC)nn qq lN e qP av(a5 V h=ng sc V h9=mj qN;pL lJ;oj)rs aC pN py ba uH!ba||C[ba uI>=uy tP b)lN;pa b,cg(aC nO pa a,aE(aC nO ov cH lN us K=aF(aC)-aF(a5)+h9-*cH rL cu(O bP,K)nz void pi uK I k=uy uC hW=py tT lx rg ra mT jY=sA tW jY lJ)bm[hW(tT]={};lG fB=!jY lJ lV;lG hN=!eu lJ lV z fB!=hN I iW=nF,h D hN)aD.mz iW,ph nJ aD mO iW B G swap(jY,eu pQ hN)jY nD)lE p o p<jY lJ mW p I L=jY lJ[p],f=hW(tT mw p;}dW+=K;rh pi gt Q ri tV sn second>=aj()lN qU av(first),b=av(qV nn lN e hS=py a5 H aC I qP av(a5)ps W=bk(mc ph lJ;ok V a2=aC<ao()?rI:tq-1;W.mz W lq+a2,cg(aC nO W.mz W lq+rI,aE(aC)uq W;}e f2=hS(oG),f3=hS(second,first);ak g8 nU a.O f2),g9 nU b.O f3);aQ fa nU a.O g8),fb nU b.O ns bk(a nn)qN+uo rL dr(a.O mA g8),b.O mA g9),K qy uz fU F b0==b1||b0>=aH(qK b1>=uy lN e&jl=bk(b0 V&jm=bk(b1 D fT>jl qQ||fU>jm qQ lN nI&loc:bm F(sc sB b0)&&rI<fT&&tq>=fT qK(sc sB b1)&&rI<fU&&tq>=fU)lN;}oC f2(jl lJ pc jl tZ fT);f2.mz f2 qF jm tZ fU,jm lJ lH)sx f3(jm lJ pc jm tZ fU);f3.mz f3 qF jl tZ fT,jl lJ lH);ak g8 nU b0,f2),g9 nU b1,f3);aQ fa nU b0,g8),fb nU b1,ns jl qN+jm qN-*gQ-*gR rL dr(b0,mA g8),b1,mA g9),K)nz void pi sr K){oo mT loc:bm pW sc sB b0 qK sc sB b1))qP{}e assign=py O ak eu oV iW=nF,ph;lG hN=iW!=aD lH&&*iW==h z eu lJ lV F hN)aD mO iW);sA nD pt{mk!hN)aD.mz iW,ph;sA=mA eu tr k=pZ oD uC&W=mj lJ lE p o p<W mW p I L=W[p]e f=rg mw p oJ;pE b0,mA g8 nO pE b1,mA g9 nO dW+=K;}oE js{ly ki=1 N dx=59.0 mh bv lW mh d8 lW mh eK lW mh hs lW mh d4 mi pl eH=1000000 pl eX=400 pl ht=400 mh f8 lW N fY=2 N gg=140 pl eB=2 pl eR=3 mh eZ lW ra oE jv{bz bZ ra jv kL pm lv js&bK);na c0 lX virtual~c0 pT;virtual oC np&rng lk o};na jt lX virtual~jt pT;virtual bool ua lk o};na hf lX void eN(pG jt>move){gm X mA move)qJ hZ(mf,nj am=nj pv)lk;mu lc pG jt>>gm ra lj using aS=nj mK an{la f sg h sg ax sg au tx K sH dL{nk dp N en=-lg B mK e2{la h tx K sH cz{bool aJ;bV ax,au N aF,dq;}oT aP lX explicit aP pm)mh hQ op,oH=aS pv)lk mh fv op mL ga oA qe lc e2>&fR op mL fu oA qe cz&dQ op mL cL oA;}uw*ag tp lc e2>>fu mN oC hU tp bool>ga pk cz>cL ra me ct oh fW,oH);nk bG oi tz C mr gW=-lg nO class aN lX const nk&dp oh H f sy jJ oh nd E sy g1 oh H pX E)nt nk&dp op mL qm oL;}rj jS{ly bp=my<ly>pv);nk J ra me i4 pl er sg bT o lc jS>cx mI gF oh sy d0 oh H f H ph mI d2 op B;na ic lX up class kW>nk get rr aH,ly aG,ly cQ mr cn oy dv,kW&&k1 F cx A<=f)cx mv f+1);mT row=cx oA z row A<aH)row mv aH);mT bX=row lS;lG hr=qY hK&&qY cQ==cQ&&qY dv==dv;lG kT=hr&&(dv?qY aG==aG:(qR.has_value(qK cn<=qY cn)pQ kT){qR=k1();qY aG=aG;qY cQ=cQ;qY cn=cn;qY dv=dv;qY hK lW oO qR&&!dv oV iY=*qR;iY su+=cn-qY cn ls iY;no qR;}rj jS{nk J;ly aG sg cQ tx cn sa dv mi mh hK mi;}pk lc jS>>cx ra oE dR{an J pl bg;}oT fH lX void add(nk J H bg F J&&uu>oz gm X{*J,bg}B uF dR>dp(qI gm lV nM ls*G max_element(gm pc gm lY J su<b oL su;}mM fK Q ri){hJ(gm,[&](lb&move uH move.bg==first||move.bg==second||move oL nn==first||move oL nn==second;}mM clear(){gm nD B mu lc dR>gm ra na aq lX aq nG rS K sj qx rS K nA qx rS bh nA qx lk;uF aI>eS nA bw rS dM nA qx lv aI&ew rW d5,cY uL o bV jG,il pl cd Q ba lk mh cm lr ak&C H bq lk mI pf lk;}oT dy rd lv aq&uM as lX as nG rS K sj bq rS K nA bq rS bh nA bq rW cV uL o la cd Q ba lk mI pf lk;}oT cG rd lv as&uM gw mX mf od mu mutable aN fJ mN oC bT ra na eM mX mf od};na dK mX mf od mu};oE bN{la a5;an aY N K;}oT a7 final:public jt lX a7 pT mh cb(ab&od uF bN>d7 oh lk;rj Row{ly fm o ak dO N a6 o}tp Row>kK mK eF{ly ca o uF bN>J;}mK bb{bN J;ly d1,ca;}mK nh az*gs sz tp ly>ce tp bool>et uA mI cU oh H pX a5 nd aC lk mI ep oh lk mI gd(lk mI fX sj aC lk mI f9(lk ra oE bl{la oG N K ra nf eJ oh H ri)oT cA final:public jt lX cA pT mh cb(ab&od uF bl>d7 oh lk;rj eF{ly ca o uF bl>J;}mK bb{bl J pl b0,b1;ly jO,jP,ca;}mK nh az*ag sz tp ly>cr uA mI ep oh lk mI fX Q b0 H b1 lk ra na aL lX explicit aL ur jC=200,ly ki=1 sy ia sC sy h5(oH H ex=50000 oy gY mi);bz ff(oH,me dm={}lv bz*cc sz oy kf lW oy ij lW H ei=2 oy dN lW H eI=3 sy dF(oH lv ab&fe sy iD Q jC)mh fD tk cR==qb;rh ua,oH);rj fF{oC E,C N Z sH Arc{la f N bW;}nt az&ag pl gT,eP sg ef sa aZ mi pl cR o nC fZ;ly jN uJ ly ip o ly fP o ly eG o lc G map<oC,fF>>dh pk ly>cr pk lc Arc>>kH;nC fc mh c9 mi;ly dz o mutable ly eW=my<ly>pv)mN nC eA mI add oh H O bool ck sy fs Q pX E nd C mr Z oy ck);nC c6(oH=aS::min()lk ra lj jv kL pm lv js&bK){using e1=lU steady_clock e ha=qg V am=ha+lA(qs dx nO lz hG=qs eK?1.0:0.0 e fo=am-lA(lT hG sU nO hf bv;pK eM>st pK gw>st pK cG>st pK dy>()D qs d8 oV move=lR<dK>();bv.eN(mA move)B pK cA>st pK a7>st sX oD);ab c5(ah,mA empty)V hD qr am,qg)+lA(lT 1.0 sU)nO c5.a3(ct(c5,hD nO pG aL>bQ mh d4=qs d4 z qs eK){bQ=lR<aL>(ah,qs eX,qs ki D qs hs||qs d4 I g6=qg V f0 qr fo,g6+lA(lT qs fY,qs dx*0.2))nO sD h5(f0,qs eH);lG cy=sD fD();lG jL=qs f8&&(!cy||uj D jL I jg=qg V gL=lA(lT 3.0 sU)V cj qr fo-gL,jg+lA(qs gg)nO la kr o do{lb hy=uj?lT cj,nu(6)):cj e cc=c5.bZ();ab ir(ah,sD ff(hy,c5.aT(),&cc,qs bv,qs d8,qs eB,qs eZ,qs eR),c5.bc st ir.a3(c5.aT st c5.cO(ir D uj&&nu(2)<cj I iv qr cj,nu(6 nO sD cb(c5,iv)U kr;qZ z nu(2)<cj I fO qr cj,nu(3)D kr%2)sD dF(fO,c5)nJ sD h5(fO,qs eH tA;qZ rJ(kr%2))qZ;qZ oJ}nK uj&&qg)<cj);d4 lW oO cy){d4 lW rJ jL)sD iD(lT qs eX,qs ht)B qZ oJ ab dp=c5 z bQ)sD ia(c5 D bQ){sD cb(dp,am);qZ z d4&&qs bv&&qg)oQ 500)<am I h2=dp.c3();bv.hZ(dp,am D dp.c3()>h2+oz sD ia(dp);qZ oJ jv aA;nT bZ=dp.bZ(uq aA;}na f5 ms f5::np&rng oa oC E(mq qh(E pc E lH oP;sv)sx bC;bC nX count)tY E F bC A>=count mm z M g2 nb)bC X f mU bC;}na gN ms gN::np&oa qp J=mq N c4=-rZ uk r I K qt a6(r D K&&*K>c4){J=r;c4=*K qW J mU bC;}na fG ms fG::np&rng oa oC b9 uk rM D b9 lV R{}e j0=b9[so b9 A]rt nE e hh qt aE(j0 V cS=py r uH r<to?mS aw S rH.cS:pr[r-to]rH.cS ra lz iL=cS(j0);qp J=mq N f7=rZ uk r F nP<0||!M a6(r)pw L qt aE(r)us dC=mS jq(hh,tT+mS jq(L,hh)+tB iL-cS(r)D dC<f7){f7=dC;J=r qW J mU bC;}lj ox cz fg og f I n=to rs f<n e ax=aJ?sZ=aJ?rv-n uq{aJ,b5 nx ax),b5 nx au),aJ?mS aw oA uE tM aF,aJ?0 tM dq}oJ aP::aP pm):ag(&ah),fu(pV pd+pV tl),hU(fu A oP,ga(fu A,pq,cL(fu A){mP f o f<cL mW f)cL oA=fg(ah,f qJ aP::hQ op,oH qI ga oA R true rt*ag e n=to e ax=f<n?sZ=f<n?rv-n V aF=f<n?mS aw oA uE tM aF nt ak empty;mT fR=fu oA lF&h=hU oA;h<mS Y mW h F am!=aS pv)&&px lN;aQ af nx O empty pz ft(ax pz ft(au V cH=af.a0(D cH)fR X{O aF-*cH}B pH fR pc fR lY K>b su||(a su==b su&&a nn<b nn B);ga oA lW nz me ct oh fW,oH oV dm=G make_shared<aP>(fW.ah())lE f o f<fW.aj()U f F!dm->hQ ut,am)tS no dm;}ox void iZ(dL&aA,an J F!ts||J su>ts nN||(J su==ts nN&&J nn<ts->ph F ts)nT en uG nT en,ts nN);ts=J sk nT en uG nT en,J su B cz dQ oi f sb in=nE e ao=pI)rs f<ao ls{aJ,b5 nx M aE nb),b5 nx aJ?rv-ao)),M aF nb,aJ?0:pr[f-ao].dq};}dL hY oi tz C lv cz&data,a9&ku mr gh=-rZ oy jx mi){dL aA e&in qt ah(D oN dq>mS os R aA e b8=C qQ e ax=oN ax.mZ au=oN au nL e&W=C lJ e gA=ku.gA(b8)ps*ev=gA rH ps*dA=gA tU ps*cs=gA.jT pM I be=pos?W[pos uI:h e iO=pos<b8?ro W ol):0.0;ev ol=ro ax)-iO+(pos<b8?mn ax,W ol):0.0);dA ol=ro au)rA);cs ol=ro rU au)rA tr gE=gh N dc=rZ N eC=rZ pM){dc qr dc,cs ol pQ oN aJ){dc qr dc,eC+dA ol);eC qr eC,ev ol td mk oN aF-dc pY<gE R aA ps*bx=gA.kM rJ oN aJ){bx[b8+1]=rZ lE tQ bx ol qr dA ol,bx[pos+1]B mP a o a tC a F!qi a)pw fp=[&]lx mo gh,jx&&ts?ts nN:nT en B e gy=oN aJ?cs rT:lT cs rT,ev rT+bx[a+1]D oN aF-gy pY<fp()sE aI ew tm);ew.dn(0,a);ew.eo(oN ax pQ ew qM qu z oN aJ oV af=ew;af.eo(oN au);ni a rQ a,oN aF-cs rT}pt{lb fw=py b mr fC oV af=ew;ni a,b pz eo(oN au);ni b rQ b,oN aF-fC}B lE b=a;b tC b F!qi b)pw fC=b==a?cs rT:ev rT+dA[b]z oN aF-fC pY<fp()sE fw(b,fC td no aA oJ nk bG oi tz C mr gW F f>=mq||h>=M aH(qK oX qq nM ls hY nR f,oF,dQ nR f),M bO(),gW tA.dp;ob aN::d0 oi f H ph{mT bX=pb e&C tt;qR oW D M co(ph){qR=hY nR f,oF,M aT()->dQ nb,M bO(),1e-9 tA.dp z qR&&qR nN<=oz qR oW pt{rk&empty:M aT()->fR nb pW empty nn==h F empty su>oz qR=an{f,O 0,0,empty su}qv oJ qY bp=C si mM aN::d2 op){mT jf=qm oL;jf oW)mR<er U h sb J=pb oL z J&&(!jf||uu>jf nN))jf=J;}ob aN::gF sC sb dm qt aT(D i4!=dm||er!=M aH(qK bT!=mq){i4=dm;er qt uy;bT=mq;cx.pE bT*(er+1),jS{}td const nk&aN::dp oi f){gF(ac D oX qq){qm oL oW uq dp ut B i4 qO bp()){d0 nR f,h B d2 nb ls dp ut mM aN::jJ sC nd E){gF(ac V&dm=i4 tY E F oX qq sE dm qO bp())d0 nR f,ph;d2 ut B ob aN::g1 oi pX E){rk f:E F oX qq pw be=dp nb;d0 nR f,h V&eu=pb oL z be&&be->h==ph d2 nb nJ mk eu&&(!be||eu nN>be nN||(eu nN==be nN&&h<be->ph))qm oL=eu oJ oE bu{la h sx E N Z;}mK cW{lc int>bf;nC bM N Z sg hX o la W sg fk sa bU mi mh fv mi ra ss,oH)mh bJ();cW dk mY,oH oy iU lW);cW dT mY,oH,ly ki=1 lv nC&bM={});lj sF aL ur jC,ly ki):ag(ah),gT(jC),fZ(pV oD,-rZ),jN(ki),dh(pV oD),cr(qb oP{ob sF add oi O bool ck sb W tt lJ z W lV R e k qt uy,n=pI)sx E N aF=0 tW W){la f z L<k+n)f=L-k nJ mk pC ag.tl)f=n+ql n nJ lw E X f);aF+=M aF ut B pH E pc E lH V Z=aF-M mj qN;fs(O E,W,Z,ck mM sF fs Q pX E nd W mr Z oy ck F!mQ Z qK Z<=tu;mT hO=dh lS z ck&&hO A>=gT&&Z<fZ lS R e jB=hO.find(E D jB!=hO lH&&jB rl oR>=Z R;hO[E]=fF{E,W,Z}U dz rJ ck)fZ lS=-lg D ck&&rB;fZ lS ne oR;})rl oR;}ob sF h5(oH H ex oy gY){cR=0 z px R rt ag e n=to,nr=n+pr A,nv=mS gG()ps aV=gY?c6(am):nC ub ta}z gY I jp=jM[eG%3 tg;}lb hp=oI eG++mK kl{la ax,au N hI,ee,ji,hn,dw,aF,dq;}pk kl>E qj r F r<n sb p=mS aw S;E X{mS cp(r),mS aW(r uD 0,rZ,p.dw,p uE-oc,0}pt{lb&p=pr[r-n];E X{mS df(r-n),mS a1(r-n uD p tU.cS,p tU.ee,p.dw,p uE-oc,p.dq}td lb m=pr A rJ c9 I eV=lf e gp qr 0.6,0.25*rK(am-eV).count()V eg=eV+lU duration_cast<aS::duration>(rK(mo 0.0,gp)nO lc lc double>>is;nC jc(nv,lg nO lc ml>gu(nv oP pl gi=0 lE l o l<pU 8,nv)U l F lf>=eg tS nC dC(mS b7 lq+gi*nv,mS b7 lq+(gi+1)*nv);dC[gi]o lc ml>iV(nv oP mh fv lW lE iJ o iJ<nv U iJ F(iJ&63 nZ eg){fv mi qv qw v=nv uv rZ qn pW!iV[u]&&dC[u]<tj{v=u;of dC[u]oO v==nv tS iV[v]=1 qn pW!iV[u])dC[u]qr dC[u],value+uc v*nv+u]tn!fv tS gu[gi]=1 qn)jc[u]qr jc[u],dC[u]);is X mA dC))N i2=-1 qn pW!gu[u]&&jc[u]>i2){i2=jc[u];gi=u oJ nC dB(nv*m oP lE L o L<nv U tT mP r o r<m U r){mT jk=dB[L*m+r]e kQ=mS a1(r)nI&d:is){lz kp=1e-8+64 um*nv*mo{1.0,d[kQ],d[L]});jk uG jk,d[kQ]-d[L]-kp td qZ pk lc Arc>>fE(nv)lE L o L<nv U L F px R;mT kS=fE[L]qj r I eY=uc L*nv+E S rH]e kp=8 um*mo{1.0,tB E S.ee),tB eY)}V ee=E S.ee-eY pY+kp z ee>=-1e-8)kS X{r,ee}B pH kS pc kS lY bW>b.bW||(a.bW==b.bW&&a.f<b.f B B kH=mA fE);fc=mA dB);c9 lW;qZ;}lb&fE=kH e&dB=fc;cR o lc ml>jF ub,hV ub sx C,b9;C nX 2*nr);b9 nX nr)mR<dh mW h F px mm e now=lf e bt=am==aS pv)?am:now+(am-now pj>(qb-ph pl fz o G map<oC,G pair<double,fF>>bS N g0=-rZ pl fN=ex ps e9=bt mh ea mi,gD mi pl c8=0 nI&arc:fE lS sb q=E[arc.f]z q.dq<=mS os&&uc h sQ rH]<=q.ee pY)++c8;}lb hL=[&](auto&&ky H g7 mr jq mr cH mr aF mr load H fy)->void{mk ea R z fz>=fN||((fz&63 nZ e9)){ea tv;}++fz ny b9 pW hV S&&jq+dB[g7*m+r-n]>E S.hn pY R rJ fy&&aF-cH>ud gY?(bS A<gT||aF-cH>=g0):(dh lS A<gT||aF-cH>=fZ lS))oV key=b9;pH key pc key lH pQ gY)fs(O key,C,aF-cH tA nJ{lb e0=aF-cH ps jB=bS.find(key D jB==bS lH||jB rl nL<e0)nB Z=-cH ny key)Z+=r<n?mS aw S uE:pr[r-n]uE;bS[key]={e0,fF{key,C,Z}}z bS A>gT I kP=[](lb&a,lb&b uH a oU nL<b oU nL ra bS mO rN nO g0=rN)rl nL oJ}}mk!mQ jq)R ps rm tF A;ly j6=0 z gY I jU=G partition_point(tF pc tF qF[&](lb&arc uH jq<=arc.bW;});rm ng jU-tF lq);j6=hp+h*0x94d049bb133111ebULL+g7 rE=(j6^(j6>>30))rE^=j6>>27;}lb d9=b9 A,i1=d9+count e ka=i1&&(j6&1)?ng j6%i1):0 qH i1 U i I ko=ka?(i+ka)%i1:i z ko<d9 I r=b9[ko]rJ hV S pw&q=E S us e5=jq+uc g7 sQ tU]rJ mQ e5 qK e5>q.hn pY sE hV S o C X q tU);ky(ky,q tU,mo e5,q.ji)+q.dw,cH+mS es[g7 sQ tU],aF,load-q.dq,fy ty;C rp);hV S=1 z ea R;lw}lb&arc=tF[ko-d9]rJ gY&&jq>arc.bW mm e r=arc.f z jF S pw&q=E S z load+q.dq>mS os sE lz e5=jq+uc g7 sQ rH]z e5>q.ee pY sE lG first=gY&&b9 lV z first I gK=lf z fz>=ex||gK>=bt){gD tv;}fN=fz+tJ 1,(ex-fz)/c8);e9=bt==aS pv)?bt:gK+(bt-gK pj>(c8 B lz im uG e5,q.hI)+q.dw us jA=cH+mS es[g7 sQ rH];jF S uJ b9 X r);C X q rH D r<n){C X q tU V gS=im+uc q rH sQ tU]z mQ gS))ky(ky,q tU,gS+q.dw,jA+mS es[q rH sQ tU],aF+q uE,load,fy);C rp pt{hV S uJ ky(ky,q rH,im,jA,aF+q uE,load+q.dq,fy+1);hV S o}C rp);b9 rp);jF S=0 z first){gD|=ea;ea mi;--c8 sk mk ea R oJ;hL(hL,O 0,0,0,0 oP;cR+=!ea&&!gD nI&[key,c]:bS)fs(O key,c oU.C,c oU oR,false td nC sF c6(oH qG nr=ag.pd+ag.tl z eW==dz&&eA A==nr){qZ ls eA;}lc const fF*>al sx Y lE t o t<dh mW t)rk&[key,c]:dh te){al X&c);Y X t B oC dd(pu qh(dd pc dd lH oP;pH dd pc dd ue uH al rT->Z>al[b]->Z;})sL br ub,f6(qb oP N jk=0 lF j:dd F f6 tG tH ny al pF->E)j4&=!rn rJ j4 sE f6 tG=1 ny al pF->E)rn uJ jk+=al pF->Z;}nC aV ub,cT=aV N dp=rZ lE bD o bD<192 U bD){nC cw(qb oP pk const fF*>dS(qb,nullptr)oY sM j sb c=*al pF uv c oR qL qc oc z value>cw tG){cw tG=value;dS tG=&c oJ lb i0 qT+nY cw pc cw qF 0.0 D i0<dp){dp=i0;cT=aV;}oC jZ ub nI c:dS pW c)pA c->E)++jZ S N kq=0 qj r pW oc>0||jZ S>1 I g=qo;kq+=g*g oO kq==0||i0<=jk+1e-7||!mQ i0)mm e iJ=0.7*(i0-jk)/kq qj r)oc rc oc+iJ*(qo)tn(bJ(qK qb<=96)&&lf oQ 250)<am){lc bu>kj mR<dh mW ph{nV cT S;eh X{e0,&c}tr rm bJ()?eh A:pU 64 on)oY sN j)kj X{O eh pF oU->E,eh pF oU->Z}tr iN qr am,lf sR 2 nO auto hd=em(kj,qb,nr,iN)uv nY hd pc hd qF 0.0)nI&hO:dh)nB cw=0 nI&[key,c]:hO)nB e0=c oR ny key)e0-=hd S;cw uG cw,e0 B value+=cw;}qZ z value<dp)cT=mA hd mU cT;}bz sF ff(oH,me dm lv bz*cc oy kf oy ij H ei oy dN H eI I nr=ag.pd+ag.tl;bz km z cc)km=*cc nJ km.cf mv qb);ab empty sJ km pQ dm)dm=ct(empty,lT am,lf sR 1)nO empty.a3(mA dm nO oC E(nr qh(E pc E lH oP qz(oI ip++)pl hl o ab ac=empty,dp=empty;aN bk;f5 hH;fG hu;gN ib pk pG jt>>cK;qk eM>st qk gw>()pQ dN){qk cG>st qk dy>()tn ij)qk dK>()pQ dN)qk cA>st qk a7>st nK lf<am F hl F hl%8==0||M c3()<dp.c3()*0.995)M cO(dp V bC=tJ 4,M g3()/12 D hl%4==1)hH rw mk hl%4==2)hu rw mk hl%4==3)ib rw mP i sg rm 2+so tJ 2,qb/12);i sN i I h=so qb sx bC ny E pW nP sB ph)bC X r);mb(!bC lV&&!M ej(O bC)td sv)ny E F px mm z nP qq pw J=bk.dp nR r pQ J||uu<=oz lw mb(oB B oo;;F px tS nk J ny E pW nP<0 sb i8=bk.dp nR r D i8&&i8 nN>ud!J||i8 nN>uu))J=i8 oO!J tS mb(oB tn kf&&hl%ei==0&&lf oQ 250)<am){mP jy o jy<eI U jy){bool aR mi oY<cK mW j){aR=cK pF->cb(ac qK aR oO!aR tS}oo auto rV)uf false tn M c3()>dp.c3()+oz{dp.cO(ac);qZ;}++hl;}qZ ls dp.bZ(mM sF dF(oH lv ab&fe oV aV=c6(am)ta}e jp=jM[fP%3 tg e nr=fe.aj(),n=fe.ao(),k=qb qz(oI fP++)pl iw=0 mR<k U h I now=lf z now>=am mm e bt=now+(am-now pj>(k-ph;bz cc;cc.cf mv k);cc.cf lS=fe.mj lJ;nV oc;eh X{e0,&c}tr rm pU 3 on)tD kv{cc.cf lS}oY sN j)kv X eh pF oU->C);ab ac sJ cc,fe.bc st M tK la jb=0 lE bD o bD<48&&lf<bt U bD)nB K=1e-8;nk aY;uF la>a5 sx bn qj r pW nP<0 F(r&uN nR r,O M mj,K+oc D J&&uu-oc sV uu-oc;aY=J;a5 oW td oC b9 qj rM)ny b9 I iR qt a6(r D iR&&*iR+oc sV*iR+oc;a5=r;aY oW);bn nD tn lf>=bt mm e qP mp r)ps W tt lJ;ok);ak dO sJ O W V hi=bE sJ O M mj oZ bH)-M aF(r)lE aC o aC<nr U aC pW mp aC)nn<0 F(aC&uN nR aC,O dO,K-hi-oc+aV[aC]D J&&uu+hi+oc-aV[aC]sV uu+hi+oc-aV[aC];a5=r;aY=J;bn=W oJ}mk K<=1e-8 F++jb>8 tS cc.cf lS=kv[jb%kv A];ab jK sJ cc,fe.bc st jK.tK M cO(jK);b9 nD)qj rM pQ b9 lV){G shuffle(b9 pc b9 qF rng);b9 mv lT b9 A,ng 1+so 3)nO M ej(O b9 B lw}mk a5&&aY I r=aY->f e kQ=r<n?ag.aW(r):ag.a1(r-n);bn.mz bn lq+aY->au,kQ);bn.mz bn lq+aY->ax,M aE(r nO bz af;af.cf mv k pz cf lS=bn;ab aR sJ af,fe.bc st aR.tK M cO(aR pt mk a5){ru g2(*a5)pt ru mz aY->f,O aY->ax,aY->au nO uf pq U iw oJ qZ;ob sF ia sC){rk rV F cr lS==M mj si)sE cr lS tt si);uf true B ob sF iD Q jC){gT=jC;ug fZ pc fZ qF-rZ)lF&hO:dh)nK rB U dz oJ bool sF ua,oH){rk rV)uf pq pk lc fF>>al(qb)sl bf(qb,ty,aU(qb,ty sL br(mq oP sx iq(mq oP,dd;eP tx b2=0 mR<dh mW ph{mT row=qd nI&[key,a8]:dh lS)row X a8);pH row pc row lY Z>b oR||(a oR==b oR&&a.E<b.E B);eP+=row A sL iP(mq oP qH row mW i){pA row oS oq iP S=1 mh c5=!row oS.E lV ny row oS oq c5&=nP sB h D c5){la rm 0 uk r)count+=nP sB h D count==row oS.E A){sG=md i);b2+=row oS oR oJ}mP r o r<iP mW r)iq S+=iP S;}oC h4(qb oP mR sM ph rk&a8:qd)pA a8 oq h4 lS+=iq S-1 mR sM ph mk!qd lV)dd X ph;rq dd pc dd ue uH h4 rT>h4[b];}V ds=[&]lr fF&a8){pA a8 oq mk rn lN nz;ef o aZ mi e iI=lf e fr=dd A>48?lT am,iI sR 2)):am e fM=[&]lr oC&eb){ug br pc br lH oP mR<aU mW h F sG<0||G find(eb pc eb qF ph!=eb lH qu ny qd[sG]oq rn=1 nw fx o lc int>dS(eb A,ty,gz lF h:eb){gz X sG D sG qq)fx+=qd[sG]oR nw dG=fx;qS Q mr)>hL=py gc mr Z uO&63 nZ fr){aZ tv oO gc==eb A F Z>dG+oz{dG=Z;gz=dS tR<eb mW d)rk&a8:al[eb[d]re dG+tu tX eb[gc]qH qd mW i sb a8=qd oS rJ ds(a8)sE dS[gc]=md i)sm rn uJ hL sW+a8 oR)sm rn=0 z aZ tS}dS[gc]=-1 rJ aZ)hL sW B z lf<fr)hL(0 oP nJ aZ lW z dG<=fx+1e-9 lN qH eb mW i)aU[eb oS]=gz oS;b2+=dG-fx nz mh uh nK aR&&!aZ){aR mi lE a o th)aR=fM({dd rT,dd[b]}qK aR;}qZ lE a o dd A<=48&&th)mP c=b+1;c<pB c)fM({dd rT,dd[b],dd[c]});qZ z eP>2000){lc bu>i6 pk G pair<la H>>iu sl cc(aU A,ty mR sM ph mP om qd mW i F sG sB i))cc lS=md i6 A V&c=qd oS;i6 X{O c.E,c oR});iu.emplace_back(O i tr kE=pu>48&&!bJ()?c6():nC{}e eL=pu>48?dT(i6,pu,mq,cc,am,jN,kE):dk(i6,pu,mq,cc,am D eL.bM A==mq){eA=eL.bM;eW=dz;}b2=eL oR;aZ=eL.bU;ef+=eL.W;ug aU pc aU qF ty lF id:eL.bf pW id qq I[O dm]=iu[id];sG=md dm B qZ sk{nC aV(mq oP,cT=aV N ii=rZ pk lc double>>dO(pu)tD h3(pu)lE bD o bD<192&&!aZ U bD F px){aZ lW qv nw i0 qT sx jZ(aV A oP lF h:dd)nB cw o int dS=-1 qH qd mW i sb a8=qd oS uv a8 oR sm qc oc z value>cw){cw=value;dS=md i td i0+=cw z dS qq)pA qd[dS]oq++jZ S oO i0<ii){ii=i0;cT=aV nw kq=0 nl<aV mW r pW oc>0||jZ S>1){lz i3=qo;kq+=i3*i3 oO!kq||i0<=b2+1e-7 tS lz iJ=0.9*(i0-b2)/kq nl<aV mW r)oc rc oc+iJ*(qo)B qZ;aV=mA cT)us iQ qT lF h:dd){mT fq=dO lS nI&a8:qd)nB of a8 oR sm qc oc;fq X value B mT hq=h3 lS;hq mv fq A qh(hq pc hq lH oP;rq hq pc hq ue uH fq rT>fq[b];}B ug br pc br lH oP;qS Q mr mr)>hZ=py gc mr Z mr gI uO&63 nZ am){aZ tv oO gc==dd A F Z>b2+oz{b2=Z;aU=bf tR<dd mW d)rk&a8:al[dd[d]re b2+tu N gZ=Z+iQ-gI lE d=gc;d<dd mW d I h=dd[d]N cw=0 lF i:h3 lS F dO lS oS<=0 mm z ds(qd oS)){cw=dO lS oS qv oJ gZ+=cw oO gZ<b2-1e-7 R tX dd[gc]lF i:h3 lS sb a8=qd oS rJ ds(a8)sE bf lS=md i)N kN=gI sm{rn uJ kN+=oc;}hZ sW+a8 oR,kN)sm rn=0 z aZ tS}bf lS=-1 rJ aZ)hZ sW,gI B z lf<am)hZ(0,0 oP nJ aZ lW oO b2<=M c3()+1e-9 lN;bz af;af.cf mv M uy)mR<aU mW ph mk sG qq)af.cf lS=qd[sG].C e kD=af.fw(ag);ab i9 sJ mA af),M bc st i9.a3(M aT st M cO(i9)nz lj ox struct c2{const lc bu>&al tD kJ sx hw pl io sg Y;c2 lr lc bu>&c H k H E):al(c),kJ(c A),Y(k){lc int>dm(k+E,ty oY<c mW j I add=py r F dm S<0){dm S=md io++);hw X r B kJ pF X dm S B;add(c pF nn)ny c pF oq add(k+r td}oT e3{const c2*cv pl n_;nC db,e6,eT,kR,iE sx e7 sL ec sl cX N fS Q row H uB tL a8-nc]uv 0 ny pR kJ[a8])value+=tL r]rR value+=iE S*tL r];no value nw dO Q uB-eT[a8-nc]uv pR al[a8]oR ny pR kJ[a8])qc eT S rR qc iE S*eT S;no value;ob kI Q cl H cJ){nC fd(n_),row(n_)qH sd i)fd oS=fS(i,cJ V jz=fd[cl]us iJ=e6[cl]/jz,K=dO(cJ)nl<sd r){row S=db[cl*n_+r]/jz;eT S+=K*row S;}mP om sd i pW i!=cl){e6 oS-=iJ*fd oS z fd oS==0 qu nl<sd r)db[i*n_+r]-=fd oS*row S;}G copy(row pc row qF db lq+cl*n_);e6[cl]=iJ;ec[e7[cl]]o e7[cl]=cJ;ec[cJ]=1 U fk;}public:la fk sa bU mi;explicit e3 lr c2&eU):cv(&eU),n_(eU.io),db(n_*n_ oP,e6(n_,1),eT(n_ oP,kR(n_,1),iE(n_ oP,e7(n_),ec(eU.pu+n_ oP,cX(eU.pu,ty{mP om sd i){db[i*n_+i]uJ e7 oS=eU.pu+i;ec[e7 oS]uJ}ob kd(oH I ul pl gB=0 qD{bU tv qw cJ=nc+n_ N K=1e-8 oY<nc+sd j F ec pF sE lz of dO(j D value sV value;cJ=j z gB>n_ mm uP pl cl=n_ N iJ=rZ qH sd i I d=fS(i,cJ D d<=oz lw lb jo rc e6 oS)/d z jo<iJ-1e-9||(tB jo-iJ)<=ud cl==n_||e7 oS<e7[cl]))){cl=i;iJ=jo oJ mk cl==n_||!mQ iJ)R;gB=iJ<1e-9?gB+1:0;kI(cl,cJ B ob ik Q a8 oy include){lz kU=include?-1:1,rhs=include?-1:0 e old=n_++;nC hb(n_*n_ oP qH old U i)G copy_n(db lq+i*old,old,hb lq+i*n_)uv 0 qH old U i pW e7 oS==a8){of e6 oS nl<old U r)hb[old*n_+r]=-kU*db[i*old+r]qv;}hb[old*n_+old]uJ db=mA hb);e6 X rhs-kU*tj;eT X 0);kR X rhs);iE X kU);e7 X pR pu+old);ec X 1);cX[a8]=md old);fk o ob i0(oH I ul qD{bU tv qw cl=n_ N jE=-1e-7 qH sd i pW e6 oS<jE){jE=e6 oS;cl=i oO cl==n_ R pl cJ=nc+n_ N jo=rZ oY<nc+sd j F ec pF pw d=fS(cl,j D d>=-oz lw lz af=dO(j)/d z af<jo-oz{cJ=j;jo=af uP;kI(cl,cJ td nC fq(oa nC x(pR pu oP qH sd i pW e7 oS<x A&&mQ e6 oS))x[e7 oS]=G clamp(e6 oS,0.0,1.0 uq x nw iT(oa auto aV=eT N hB sg gl=0 nl<sd r F!mQ oc)R rZ;oc rc oc);hB+=oc*kR S;gl+=tB oc*kR S B nC he(pR Y oP oY<pR al mW j sb c=pR al pF uv c oR ny pR kJ pF)qc oc z cX pF qq I r=ng cX pF);qc iE S*oc;}he pJ uG he pJ,value tr iA=nY he pc he qF 0.0);hB+=iA;gl+=iA rJ mQ hB qK!mQ gl)R rZ ls hB+1e-7+1e-10*gl;}nC bM Q E oa nC aA(E oP lE row o row<pR hw mW row I ja=pR hw[row]z ja>=pR Y&&mQ eT[row]))aA[ja-pR Y]rc eT[row]mU aA oJ;}ss,oH){pn qZ ls kg.bM(E qJ bJ(uH false;}cW dk mY,oH oy iU){cW aA;se=aU qX z al lV){nT fv tv aA;}pn nT fk=kg.fk z kg.bU){nT bU tv aA;}nT hX=kg.iT(D iU)qZ mh i5 lW nI&c:al)i5&=c oR==G round(c oR)sL jV(Y oP,br(E oP,ho(pu oP sl bf(Y,-1 V ds=py id tI[id]z jV pJ||ho[id]lN qL mk rn lN nz;qS lr e3&mr)>hL=[&]lr e3&lp mr Z F nT bU R z px){nT bU tv;}++nT W N iT=lp.iT(D i5)iT=G floor(iT D iT<=nT Z+tu e x=lp.fq()sx du oY sM j pW ds(j))du X j D du lV F Z rO=Z;se=bf;no;}rq du pc du ue uH x rT!=x[b]?x rT>x[b]:al rT oR>al[b]oR;})ps kN=br,f6=jV ps fe=bf N af=Z lF id:du tI[id]z f6 pJ tH qL j4&=!kN S rJ j4 sE f6 pJ=1 qL kN S uJ fe pJ=md id);af+=c oR oO af rO=af;se=mA fe D iU)qZ oO iT<=nT Z+tu pl hj=du.front()N gf=-1 lF j:du F x pF<=1e-7||x pF>=1-1e-7 pw e0 qr x pF,1-x pF D e0>gf){gf=e0;hj=j oJ lb&c=al[hj];{auto hm=lp;hm.ik(hj tA po;}jV pJ=1 qL rn uJ bf pJ=md hj);hL(hm,Z+c oR);bf pJ=-1;jV pJ=0 qL rn o}mk!nT bU oV hm=lp;hm.ik(hj,pq po;}ho[hj]uJ hL(hm,Z);ho[hj]o}};hL(kg oP;nT fv=!nT bU ls aA;}cW dT mY,oH,ly ki lv nC&bM){cW aA;se=aU;nT hX=rZ qX tD ih(Y)oY sM j)ih[al pF nn]X j);nC fj(pu,0 pQ bM lV)mP j o j sM j)pA al pF oq fj pF+=bM S li lz jp:{0.0,0.5,1.0,1.5}){oC du(pu qh(du pc du lH oP;pH du pc du ue I x=al rT oR-jp*fj rT,y=al[b]oR-jp*ti;})sL br(E oP sl fe(Y,ty N Z=0 lF j:du tI pF z fe pJ qq tH qL j4&=!rn rJ j4 sE fe pJ=md j)qL rn uJ Z+=c oR oO Z rO=Z;se=mA fe);qZ oJ oo mT hO:ih)pH hO pc hO ue I x=al rT oR-fj rT,y=al[b]oR-ti;})qz(ki V ha=lf pl hx sg it o nK lf<am){lc int>iz(E,ty lE t o t sO t pW se te qq)pA al[se te]oq iz S=md t)nt la kO qr Y,ng 8+4*((hx/Y)%4)nO oC eb sL hR(Y,0 V add=py t F!hR te){eb X t);hR te=1 oJ e first=hx%Y;add(first pQ ih rb lV I kZ=hx%3==0?so pU 24,ih rb A):0 ny al[ih rb[kZ]]oq mk iz S qq&&eb A<kO)add(iz S B nK eb A<kO){nC e0(Y oP lF t:eb I old=se te qq?al[se te]oR:0 e rm pU 32,ih te A)oY sN j tI[ih te pF]e of mo 1.0,c oR-old)sw)e0[iz S]+=value oJ la dS=Y N cw=0 lE t o t sO t pW!hR te&&e0 te>cw){cw=e0 te;dS=t oO dS==Y||hx%5==4){do{dS=so Y;}nK hR[dS]B add(dS B lc bu>jW sx j5 sl cc(eb A,ty lE t o t<eb mW t)oo auto id:ih[eb te]tI[id]mh ig lW sw){ig mi qv oO!ig qu z se[eb te]sB id))cc te=md jW A);jW X{t,c.E,c oR});j5 X id tr now=lf e hE qr am,now oQ 150)V eL=dk(jW,eb A,E,cc,hE,pq;nT W+=eL.W;nT fk+=eL.fk N old=0 lF id:cc pW id qq)old+=jW[id]oR z eL oR>old+oz{mP t o t<eb mW t)se[eb te]=eL.bf te qq?md j5[eL.bf te]):-1;nT Z+=eL oR-old;it=hx;qZ;}++hx z hx-it>12*Y tS}qZ;nT bU=px ls aA;}lj bool hf::hZ(mf,nj am oa bool aR mi li int jy o jy<2 U jy){bool dX mi nI&move:gm F px R aR||dX;dX=move->cb(ac qK dX oO!dX tS uh no sp bool gw::ua qI bT A!=mq){bT mv mq qh(bT pc bT qF 0 B fJ.jJ nR bT)sq nk dp tY bT F oX qq pw move=fJ.dp ut D move&&move nN>ud!dp||move nN>dp nN))dp=move oO!dp tS ru mz dp->f,dp->O dp->ax,dp->au nO uh fJ.g1 nR dp->O bT mU sp bool eM::ua oa lc uF double>>iw(mq)pg)iw oA qt a6 nb sq uF la>dp N c4=1e-9 pg F iw oA&&*iw oA>c4){dp=f;c4=*iw oA oJ mk!dp mm tX ng mp*dp)nn);ru g2(*dp nO uh iw[*dp]oW)pg pW oX sB ph)iw oA qt a6 ut mU sp as::as rX,cV(0 F f>=pI qK rC;cV=rI;hz lW;bI qt a6 nb e&C qt bk(rD as oK}la as::cd Q ba mL ba<cV?ba:ba+2;ob as::pf qG gj qr end,cV un V ek uG mZ cV tb 2 B nf as::K sj bq rf,bq B nf as::bh nA bq oe(qK bq>C qQ-(pp,cV,cV+1,h==b6}e fC=bF(qA),sK bq,ci.aE(dE),qA).aW(dE)uq bB-fC;}nf as::K nA bq oe(qK bq>C qQ-(h==b6?2:0)nM rt qA);ou 2:0 D iB F bq&&!qi cd(bq ty+1)nM sk mk!bI||!qi bq)nM;aI af tm D qC 0,bq)nJ ni 0,bq pz L(ci.aE(dE nO af.L(mS aW(dE)D qC bq,ch)nJ ni bq,ch sP ls bh(oF,bq B nk cG::cD sC lv as pO:0)lE a o a tC a I go=b3.bh sS pQ go||*go nS K=b3 su sS rF an{r,O a,a,*K};no bo;}qJ cG oM R;as pD.O fV rG()nl<pI)pP.fK(bg,eq)nl<pI)ow sp tw aq rX,d5(0),cY(0 F f<pI qK f>=mq||rC;d5=rI;cY=tq;hz lW;bI qt a6 nb;jG=b5(nE,M aE nb);il=b5(nE,nE.a1 ut-pI))V&C qt bk(rD aq oK}la tw cd Q ba qI ba<d5 R ba z ba<cY-1 R ba+1 ls ba+2;rh tw cm lr ak&C H bq mL!bq||C.aB(qA),cd(bq ty+1 mM tw pf qG gj qr end,d5 un V gV uG mZ d5 V hT qr end,cY-1 D gV<hT)ni gV+1,hT+1 V ek uG mZ cY-1 tb 2 B nf tw K sj ux rf,a,b B nf tw bh nA ux oe(qK a>b||b>C qQ-(pp,d5,cY,h==b6}e fC=cZ(qA),sK a,b,ci.aE(dE),qA).a1(dE-ci.ao())uq bB-fC;}nf tw K nA ux qG ew=eS sS uq ew?dM sS,b,*ew):lB;}uF aI>tw eS nA a oe()nM;ou 2:0 D a>ch nM rt qA V dq=pr[dE-ci.ao()].dq z dq>mS os nM z iB F!cm(C,a)nM sk mk!bI||!qi a)nM;aI af tm D qC 0,a)nJ ni 0,a pz eo(jG sP ls af;}nf tw dM nA ux lv aI&ew oa ou 2:0 D a>b||b>ch nM rt qA D iB F!cm(C,b)nM sk mk!qi b)nM ps af=ew z qC a,b)nJ ni a,b pz eo(il D qC b,ch)nJ ni b,ch sP ls bh sS,b B nk dy::cD sC lv aq pO:0 V&in=nE e qP mp r)nt d6 jQ{C oZ bH,h==bg}e aE qt aE(r V gb=mS a1(r-pI nO nC f4(b8+1),eQ(b8+1),eO(b8+1),bx(b8+2,rZ)pM){f4 ol=bY nx sK pos,aE);eQ ol=bY nx sK pos,gb);eO ol=bF nx sK pos,aE,gb B mP tQ bx ol qr eQ ol,bx[pos+1])lE a o a tC a I gy qr eO rT,f4 rT+bx[a+1]D b3.cF()-gy nS ew=b3.eS sS pQ ew qu lE b=a;b tC b I fC=b==a?eO rT:f4 rT+eQ[b]e go=b3.cF()-fC z go nS K=b3.dM sS,b,*ew rF an{r,O a,b,*K};}no bo;}qJ dy oM R;aq pD.O fV,0 rG()lE r=pI);r<mq pP.fK(bg,eq)lE r=pI);r<mq ow sp ox struct jw{la h sx hP ra oC g4 oi h sb in=nE e&C tt e k nq pl fi o oC kV{0}lE pos o pos<C lJ mW pos I L=C lJ ol z pC m)++fi nJ mk L>=k tE+m)--fi rJ fi&&qi pos+1))kV X pos+1 mU kV oJ bool dK::ua qG&in=nE sq lc jw>cf;cf nX M cB()A)nI rV)cf X{O g4 nR ph})mh gx mi qH cf A&&!gx U i){mP j=i+1;j<cf A&&!gx U j I b0=cf oS.O b1=cf pF nn e&a qt bk(b0 V&b qt bk(b1 V h7=a qQ,h8=b qQ N c4=1e-9 pl j1 sg j2=0 nI fT:cf oS.hP){rk fU:cf pF.hP I kb=fT?a lJ[fT uI:b0 e kc=fU?b lJ[fU uI:b1 N K=0 z fT<h7 I j8=a lJ[fT];K+=mn kb,j8)-mn kc,j8 tn fU<h8 I j9=b lJ[fU];K+=mn kc,j9)-mn kb,j9 tn K<=c4 pY sE aI g8 nx b0,a);g8.dn(0,fT D fU<h8)g8.eo(b.dn nx fU,h8 ty pQ g8 qM sE aI g9 nx b1,b);g9.dn(0,fU D fT<h7)g9.eo(a.dn nx fT,h7 ty pQ g9 qM sE c4=K;j1=fT;j2=fU oJ mk c4>oz{ru c7(b0,j1,b1,j2 nO aR=gx lW oJ}mk!gx R aR oJ lj uF bN>a7::d7 sC oa ep(ac);gd(uq dH lV?lB:lm(dH.top()oL mM a7::gd(oa nK!uQ(V h=qR.aY nn e aC=qR.aY.f z et[aC]&&qY d1==ce lS&&qY ca==bL lS[aC].ca tS dH.pop(B rh a7 rx&b qI a oL su!=b oL su R a oL su<b oL su e key=[]lr bN&c uH G tuple(c.a5,c.aY.f,c.aY rH,c.aY tU B ls key(a oL)>key(b oL mM a7::cU oi pX a5 nd aC qI aC lV R;mT row=bL lS tY aC){++row oA.ca;row oA oL oW tr&hc tt sx W tY a5 I qP mp f);mT gP=kK oA z gP.fm!=hc si)){W=hc lJ;ok);gP.dO.gJ(nE,O W);gP.a6=bE(nE,O hc oZ bH)-M aF nb;gP.fm=hc si B rk hv:aC){mT aO=row[hv]us fp=sf?sf nN:1e-9 e aY=bG nR hv,O gP.dO,fp-gP.a6 D aY&&aY nN+gP.a6>fp)sf=bN{f,*aY,aY nN+gP.a6}oJ rk f:aC)fX(O f mM a7::fX sj aC qG&aO=bL lS[aC]z sf){dH.push({*sf,ce lS,aO.ca}B ob a7::ep sC qG Y qt uy,E qt aj(D gs!=&nE||ce A!=Y||et A!=E){gs=&nE;ce.pE Y,my<ly>pv nO et.pE E,pq;bL.pE Y,lc eF>(E nO dH=iK{};kK mv E B lc lD>>a5(Y)sx aC,eD lE f o f<E U f I qP mp f);lG je=sc<0 z je){aC X f pQ et oA)eD X f pt a5[sc]X f);et oA=je;}mP h o h sO h I bp tt si D ce lS!=bp){ce lS=bp;cU nR O a5 lS,aC pt mk!eD lV){cU nR O a5 lS,eD td mk dH A>2*Y*E+32)f9(mM a7::f9(oa dH=iK{}mR<bL mW ph mP aC o aC<et mW aC F et[aC])fX(O aC B rh a7 qE)){ru fn(J->a5,J->aY.f,J->aY rH,J->aY tU nO uh no sp ox bool j3 lr bl&a lv bl&b uH a su>b su||(a su==b su&&G tie(a.mZ a oU)<G tie(b.mZ b oU)B nf ey oi a5 H aC I qP mp a5 V h=ng sc V&C tt rt nE rs aC<pI V au=aJ?mS aW(aC):mS a1(aC-pI nO aQ af tm pz cC(0,rI pz ft(M aE(aC)D aJ)af.ft(au pz cC(rI+1,tq pQ aJ)af.ft(au pz cC(tq+1,C qQ uq af.a0(td nf eJ oi ri tV>=mq||second>=mq nM qU mp first),b=mp qV nn nM e gQ=ey nR oG pQ gQ nM e gR=ey nR second,first pQ gR nM ls M bk(a nn)qN+M uo;rh cA rx&b mL j3(b oL,a oL mM cA::fX Q b0 H b1 qG&aO=bL[b0][b1]z sf)dH.push({*sf,b0,b1,cr[b0],cr[b1],aO.ca}mM cA::ep sC qG Y qt aH(D ag!=&nE||cr A!=Y){ag=&nE;cr.pE Y,my<ly>pv nO bL.pE Y,lc eF>(Y nO dH=iK{};}oC aR pk bool>kC(Y,pq mR sO h I bp tt si D cr lS!=bp){cr lS=bp;aR X ph;kC lS lW oJ mk aR lV R tD E(Y)pg I h=oX z h qq)E lS X f B rk h:aR){mP de o de sO de F de==h||(kC[de]&&de<ph pw b0 qr O de),b1 uG O de);mT aO=bL[b0][b1]U aO.ca;sf oW)nI first:E[b0]){rk second:E[b1]I K=eJ nR oG pQ K||*K<=oz lw const bl J{lT oG),mo oG),*K}rJ sf||j3(J,*sf))sf=J oJ fX(b0,b1 td mk dH A>2*Y*Y+32){dH=iK{}lE a o a sO a)mP b=a+1;b sO b)fX(a,b td uF bl>cA::d7 sC oa ep(ac);nK!uQ(D qY jO==cr[qY b0]&&qY jP==cr[qY b1]&&qY ca==bL[qY b0][qY b1].ca R qR;dH.pop(mU lB;rh cA qE)){ru gt(J->mZ J rl nO uh no aR oJ int main(I kw=lf;G ios::sync_with_stdio(pq;G cin.tie(nullptr V ah=bs::gq(G cin);bs::ez::js bK;qs ki=1ULL us kn=rK(lf-kw).count()us hg=300 us reserve=hg>2?1.0:hg*0.1;qs dx=hg-kn-reserve z qs dx<=0){bs::sX oD uR,empty,ah pt{lb aA=bs::ez::kL(ah,bK uR,nT bZ,ah mU 0;}
