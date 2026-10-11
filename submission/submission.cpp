#pragma GCC optimize("O3","unroll-loops")
#include <cstddef>
#include <limits>
#include <algorithm>
#include <vector>
#include <stdexcept>
#include <cmath>
#include <string>
#include <iosfwd>
#include <iomanip>
#include <istream>
#include <ostream>
#include <sstream>
#include <cstdint>
#include <optional>
#include <atomic>
#include <memory>
#include <utility>
#include <random>
#include <string_view>
#include <chrono>
#include <queue>
#include <map>
#include <fstream>
#include <iostream>
#include <numeric>
#include <functional>
#include <tuple>
#define ko std::size_t
#define kp const auto
#define kq std::
#define kr kq vector
#define ks .size()
#define kt [[nodiscard]
#define ku )const
#define kv }namespace a4::du{
#define kw )return
#define kx kq optional
#define ky kq numeric_limits<double>::infinity(
#define kz ;if(
#define kA {return
#define kB ;for(
#define kC (const
#define kD .push_back(
#define kE static_cast<
#define kF )throw kq logic_error("error"
#define kG .begin()
#define kH const double
#define kI public:kq string_view hu(ku override
#define kJ kr<ko>
#define kK )throw kq invalid_argument("error"
#define kL ,const
#define kM kq chrono::steady_clock::now()
#define kN ;double
#define kO ;return
#define kP <double>
#define kQ kw kq nullopt
#define kR )continue;
#define kS ){if(
#define kT ac.
#define kU kq mt19937_64&
#define kV kq uint64_t
#define kW const bool
#define kX .aw()
#define kY .end()
#define kZ ab&ac
#define la ,ko
#define lb );}
#define lc ;kp
#define ld kB ko
#define le kq chrono::duration_cast<gy::duration>(kq chrono::duration kP(
#define lf )if(all||ae==first||ae==second)g0.add(cG(ac,b1,r,ae,kT bn(ae)
#define lg kq make_unique
#define lh kT ad()
#define li .empty()
#define lj (aa)
#define lk kw false
#define ll )kz
#define lm loc.
#define ln ){kp
#define lo final:public
#define lp =0;
#define lq (ko
#define lr )throw kq runtime_error("error"
#define ls private:
#define lt [ae]
#define lu bn(ae)
#define lv auto&
#define lw ku override
#define lx kq max(
#define ly };kv class
#define lz kq numeric_limits
#define lA ku kA
#define lB in.
#define lC .resize(
#define lD {public:
#define lE insert(
#define lF =true
#define lG =false
#define lH ,double
#define lI ae ll!b1.gc()kw lc aG=kT bn(bi).bl()lc&dH=kT az();kW cN=dH&&dH->gH(r ll cN kS all){static const ak fm kB kp&ah:dH->dF(r)kS kT cb
#define lJ kw{false,0,
#define lK ;lv loc=a6[aa];lm ae=kE int>(ae ll bz==aC lj)lm as=p;else lm bv=
#define lL true,0,0 lb;fU();bool a8 lG;while(kp aN=g0.cx()ln ah=aN->ah lc bi=aN->bi lc dk=ah.ae;kW cP=kT cb(bi);kW cQ=kT cb(dk);kW hV
#define lM ae,
#define lN ks;++
#define lO kq min(
#define lP ae kL ak&
#define lQ kT gq(ah.aa)&&kT lE ah.aa,dk,ah.as,ah.ar)ll!eJ kF);a8 lF kz kT cb(bi)!=cP||kT cb(dk)!=cQ){fU();continue;}g0.fc(bi,dk
#define lR );read(in,
#define lS ,*a3},bi);break;}}for(kp ae:kT bo()lf,aG),bi lb else{for lq ae lp ae<kT aA();++ae lf,aG),bi lb}lc fU=[&]{g0.clear()ld
#define lT ;bool
#define lU kq shared_ptr<
#define lV &b1 la r la lP an,kV aG ku{kp bi=kE ko>(kT ap(r).ae ll ae!=bi&&!b1.dI()kQ kO fb.get(r,lM kT aA(),aG,an.bl(),b1.cI()
#define lW ;void
#define lX ;mutable
#define lY ;struct
#define lZ }return
#define ma kC av&
#define mb ku{if(
#define mc kC kZ la aa);kt]bool gc(lA gW;}kt]bool dI(lA bJ.has_value(lb kt]double cI(lA bF;}kt]ko b6 lq ae ku;kt]kx kP a3
#define md lb void
#define me .erase(
#define mf kL kJ&
#define mg clear()
#define mh lB cM(
#define mi kE int>(
#define mj ap lj.ae
#define mk kY,[](kp&a,kp&b)kA a.
#define ml kt]const
#define mm .reserve(
#define mn kx<al>
#define mo kT ag()
#define mp kT ap(r).
#define mq )=default;virtual kq string_view hu(ku lp virtual
#define mr first,
#define ms lb return
#define mt ;}bool
#define mu kC kZ la aa
#define mv kr<
#define mw kB kp
#define mx .duration
#define my )break
#define mz ->a3
#define mA }void
#define mB first la
#define mC kq move(
#define mD (in,
#define mE (ac,
#define mF ld ae lp ae
#define mG af.dh(
#define mH namespace a4
#define mI }kJ ek(ab&la,kU lw;ly
#define mJ =lB bk ks,n=lB au ks,m=lB aS ks
#define mK ,ca;}lY c7{bool operator()kC be&kL be&ku;};using h1=kq priority_queue<be,mv be>
#define mL gu);eL.cF(0,fs ks);eM.cF(0,ft ks)lc gi=eL.aY(),gj=eM.aY(ll!gi||!gj lk;kH a3=
#define mM if(
#define mN is ln jE=kq min_element(iG kG,iG mk second.bh<b.second.bh;});iG me jE lb mA aR::
#define mO kt]ko
#define mP lq lP an la
#define mQ ;++
#define mR kB auto
#define mS kq isfinite(
#define mT kq lower_bound(ax kG,ax kY
#define mU ek(kZ la count,kU rng
#define mV for(kp
#define mW kT am()
#define mX =kE ko>(lm ae)
#define mY for lq
#define mZ ld r lp r<lh mQ r
#define na aa,ah->lM ah->as,ah->ar
#define nb a9.
#define nc (*ai,
#define nd namespace{
#define ne lq aa
#define nf ae==
#define ng [pos]
#define nh kq uniform_real_distribution kP
#define ni )lc
#define nj ;class
#define nk kx kP
#define nl lM an
#define nm ma in la
#define nn )mw ae:kT bo()
#define no an me an kG+lm bv);an me an kG+lm as
#define np aw me aw kG+lm bv);aw me aw kG+lm as
#define nq mb!gc()||ae>=bj.aA(
#define ns bf.fy nc nl);aL af nc lM bf);af.cF(0,an ks ni cM=af.aY(ll!cM
#define nt mQ r kS mp ae<0 kR kp hS=kE ko>(mp ae);dC(r,hS==bi||hS==dk,bi
#define nu +1e-8
#define nw ;}double
#define nx kC ab&
#define ny [aa]
#define nz ku;kt]
#define nA ::cC(kZ ku{
#define nB ne ku{kp n=ai->au ks kO aa<n?ai
#define nC kW hV=nf b5 lc ch=an kX ks-(hV?2
#define nD =kT
#define nE mQ r)dC(r,lL=bi==dk;kW eJ=hV?kT cH(ah.aa,ah.as,ah.ar
#define nF {double
#define nG bk ks
#define nH au ks
#define nI else mM
#define nJ swap(
#define nK ::max()
#define nL ;const
#define nM ae)
#define nN kv struct
#define nO while(
#define nP kq unique_ptr<
#define nQ kq ostream&out
#define nR )kO true mt ab::
#define nS mr second
#define nT kC kZ
#define nU lx 1e-9,bw?bw mz:1e-9)kR kp
#define nV ::b6 lq ae ku{kp size=bj.lu kX ks kO nf b5?size-2:size;
#define nW nA e8 g0 lc dC=[&]lq r,bool all la mB second kS mp ae<0 kw;
#define nX data.
#define nY r:bP.aj)
#define nZ mE aa,lM kT lu,data
#define oa first)
#define ob lU const aX>
#define oc kT mj>=0
#define od kX,lm as,lm
#define oe ;an.lE an kG
#define of ag.
#define og nb ah
#define oh aD.cx
#define oi kq chrono::duration kP(
#define oj bp(aI&af la mB end
#define ok ld aa lp aa<lh mQ aa
#define ol es.mg;dn.mg;h8.mg kB lv fN:dD)fN.mg;iz lp fZ
#define om ;}}mM ah==lh||!kT gq(ah)my;aT kD ah ms aT;}kv
#define on kS aV kw mQ dK kz(dK&63)==0&&kM>=a1){aV lF kO;}
#define oo b1 mE r ni bi=kE ko>(mp lI(ah.nM kR kp a3=b1.a3(
#define op false)
#define oq lb else
#define os [i]
#define ot lB aS[
#define ou .emplace_back(
#define ov {kI kA
#define ow .as.br
#define ox ld i lp i
#define oy );af.
#define oz gy::now(
#define oA <<kq setprecision(17)
#define oB eh ks&&!aV mQ
#define oC bz>=k+2*n&&bz<k+2*n+
#define oD [d]])mM el(bP)){iR+=bP.bh;break;}mM iR<=
#define oE an kX ks-(nf b5?2:0)||(ae!=b5&&!bJ)kQ nL dV
#define oF assign(
#define oG mv bool>
#define oH [r]
#define oI bL.eq(lg<
#define oJ ll!
#define oK aA()
#define oL ld pos lp pos<=b6 mQ pos
#define oM bool ij lF):fX(ij){}kq string_view hu(lw
#define oN <am(ll a>b||b>an ks||(aK&&a!=b)lk lc ay=
#define oO lV,nf bi,[&]{mn bw lc b6=an kX ks-(nf bi?2:
#define oP (lA
#define oQ bs.
#define oR mw aa:
#define oS [&]lq
#define oT .bl()
#define oU lv an=b3;an=lu
#define oV kG,
#define oW mv mv
#define oX ab::
#define oY ;}}
#define oZ kq shuffle(
#define pa lc&in=
#define pb throw kq
#define pc ma ag
#define pd lq ae
#define pe ai->
#define pf inline double
#define pg ;else
#define ph ls struct
#define pi 1e-9)
#define pj .reset(
#define pk loc nD ap
#define pl aN.ah
#define pm kq istream&in
#define pn lw;ls mutable
#define po an.ay mD
#define pp bz-k-2*
#define pq bT[aa*ec+ae]
#define pr kJ aT;aT mm count);nO aT ks<count){ko
#define ps ;kH
#define pt ls const
#define pu bS.
#define pv empty.
#define pw mE aa
#define px b0 lt
#define py bB la a0
#define pz kq abs(
#define pA kT az()
#define pB bj.ag(
#define pC hV)bp(af,
#define pD .bg()
#define pE .cM()
#define pF mv unsigned char>
#define pG aa)bT lC aa+1);lv row=bT ny kz row
#define pH lh){aU lC lh);kq iota(aU oV aU kY,
#define pI nA bool a8 lG;nO kp ah=dX(ac)kS!kT
#define pJ second)
#define pK ah.
template<class T,class P>void g7(mv T>&fY,P i1){fY me kq remove_if(fY oV fY kY,i1),fY kY lb inline ko gX lq value)kA value?lz<unsigned long long>::digits-__builtin_clzll(kE unsigned long long>(value)):0;}mH{struct hI nF br=0 kN db lp}lY kg nF ba lp}lY iI nF aH lp hI as kN co lp}lY jI nF aH=0 kN ci lp hI as;hI ar kN co lp}lY av{mv kg>bk;mv iI>au;mv jI>aS;kJ dt;kr kP cK;kr kP c0;mO dS oP nG+2*(nH+aS ks lb mO gZ lq bz lA dt li?bz+1:dt.at(bz lb mO cu lq i lA nG+i;}mO aP lq i lA nG+nH+i;}mO dd lq j lA nG+2*nH+j;}mO bb lq j lA nG+2*nH+aS ks+j;}kt]double hZ lq u la v lA cK.at(u*dS()+v lb kt]double cM lq u la v lA c0.at(u*dS()+v md hE(ku;};}mH{nd void cm(double x kL char*hu kS!mS x)||x<0)pb invalid_argument(hu md eZ(hI w){cm(w.br,"negative or nonfinite window start");cm(w.db,"negative or nonfinite window end" ll w.br>w.db kK lb mA av::hE(mb bk li kK ni v=dS(oJ dt li kS dt ks!=v kK);oG jT(v,op mw id:dt kS id==0||id>v||jT[id-1]kK);jT[id-1]lF oY mM v>lz<ko>nK/v||cK ks!=v*v||c0 ks!=v*v kK)mw&ae:bk)cm(ae.ba,"invalid taxi capacity")mw&p:au){cm(p.aH,"invalid passenger revenue");cm(p.co,"invalid passenger service duration");eZ(p.as lb mV&p:aS){cm(p.aH,"invalid parcel revenue" oJ mS p.ci)||p.ci<=0 kK);cm(p.co,"invalid parcel service duration");eZ(p.as);eZ(p.ar lb for(double x:cK)cm(x,"invalid travel time")kB double x:c0)cm(x,"invalid travel cost" lb}mH{struct g9{bool bg lG kN bY lp kq string iU;}lY cL{oW ko>>cS;kt]g9 ex pc ku;};}mH{g9 cL::ex pc mb cS ks!=of nG lJ"route count differs from taxi count"}lc n=of nH lc m=of aS ks lc cO=of nG lc jD=of dS();mv int>cg(n+m,-1);mv int>en(n+m,-1)kN bY lp constexpr double eps=1e-8 ld k lp k<cO mQ k){ko bd=k kN b9=0 kN load=0 lc&an=cS[k]ld pos lp pos<an lN pos ln bz=an ng kz bz<cO||bz>=jD lJ"route contains invalid service node"}ps bX=b9+of hZ(bd,bz oJ mS bX)lJ"nonfinite route time"};bY-=of cM(bd,bz ll bz<cO+n ln i=bz-cO kz cg os!=-1 lJ"passenger picked up more than once"}lc&p=of au os kz bX>p.as.db+eps lJ"passenger pickup is late"}kz pos+1==an ks||an[pos+1]!=of aP(i)lJ"passenger drop-off must immediately follow pickup"};cg os=mi k);b9=lx bX,p ow)+p.co;}nI bz<cO+2*n ln i=bz-cO-n kz cg os!=mi k)||en os!=-1 lJ"invalid passenger drop-off"};en os=mi k);b9=bX+of au os.co;}nI bz<cO+2*n+m ln j=bz-cO-2*n lc id=n+j kz cg[id]!=-1 lJ"parcel picked up more than once"}lc&p=of aS[j]kz bX>p.as.db+eps lJ"parcel pickup is late"};load+=p.ci kz load>of bk[k].ba+eps lJ"parcel capacity exceeded"};cg[id]=mi k);b9=lx bX,p ow)+p.co;}else{kp j=bz-cO-2*n-m lc id=n+j kz cg[id]!=mi k)||en[id]!=-1 lJ"invalid parcel drop-off"}lc&p=of aS[j]kz bX>p.ar.db+eps lJ"parcel delivery is late"};load-=p.ci;en[id]=mi k);b9=lx bX,p.ar.br)+p.co;}bd=bz oY mY i lp i<n+m mQ i kS cg os!=en os lJ"partially served request"}kz cg os!=-1)bY+=i<n?of au os.aH:of aS[i-n].aH;lZ{true,bY,{}}oY mH{av fS(pm)lW ja(nQ kL av&ag lb mH{nd template<typename T>void read(pm,T&value kL char*j4 kS!(in>>value))pb runtime_error(kq string("cannot read ")+j4 lb}av fS(pm){ko k=0,n=0,m lp read mD k,"taxi count" lR n,"passenger count" lR m,"parcel count" ll k==0||k>100000||n>20000||m>20000||n+m>20000 lr)nL ko v=k+2*(n+m ll v>50000 lr);av ag;of bk lC k);of au lC n);of aS lC m);of dt lC v);oG jT(v,false ni h0=oS bz){lv id=of dt[bz];read mD id,"vertex ID" ll id==0||id>v||jT[id-1]lr);jT[id-1]lF;}mF<k mQ nM{h0(ae lR of bk lt.ba,"taxi capacity" lb mY i lp i<n mQ i){lv p=of au os;h0(of cu(i));h0(of aP(i)lR p ow,"passenger earliest" lR p.as.db,"passenger latest" lR p.co,"passenger service" lR p.aH,"passenger revenue" lb mY j lp j<m mQ j){lv p=of aS[j];h0(of dd(j));h0(of bb(j)lR p.ci,"parcel weight" lR p ow,"parcel pickup earliest" lR p.as.db,"parcel pickup latest" lR p.ar.br,"parcel delivery earliest" lR p.ar.db,"parcel delivery latest" lR p.co,"parcel service" lR p.aH,"parcel revenue" lb kJ hq(v)ld bz lp bz<v mQ bz)hq[of gZ(bz)-1]=bz mR*jn:{&of cK,&of c0}){jn->resize(v*v)ld u lp u<v mQ u)mY w lp w<v mQ w)read mD(*jn)[hq[u]*v+hq[w]],"matrix entry" lb kq string j3 kz in>>j3 lr);of hE()kO ag oY mH{cL jf(pm kL av&ag)lW ei(nQ kL cL&b7 kL av&ag lb mH{void ei(nQ kL cL&b7 kL av&ag){of hE(ni dp=b7.ex(ag oJ dp.bg lr+dp.iU);out oA<<dp.bY<<'\n' mw&an:b7.cS){out<<an ks mw bz:an)out<<' '<<of gZ(bz);out<<'\n';}mM!out lr lb nN bR{ko first=0,g4=0 kN duration=0 kN gS=-ky)kN bA=ky)kN cM=0 kN bE=0,dP=0,dO lp bool bg lF;};bR b4 pc la bz);bR jR pc kL bR&kn kL bR&kk)nj ak lD ak()=default;ak pc la ae mf an)lW fy pc la ae mf an)lW mg lW dY kC ak&bS);kt]bR dh pc la mB g4 nz bool ay pc la aW nz double cM oP iz;}kt]kV bl oP fZ;}ml kJ&aw oP dK;}ml kr kP&bX oP es;}ml kr kP&b9 oP dn;}ml kr kP&load oP h8;}ls kJ dK;mv bR>cD;oW bR>>dD;kr kP es,dn,h8 kN iz lp kV fZ lp}nj aL lD aL pc la lP bn)lW cF lq mB end)lW fg kC bR&d4)lW e0 lq bz);kt]nk aY(nz double g2(ku;pt av&ai;ko hC nL ak&fb;kx<bR>cE;};kv nd kq atomic<kV>fP{1}kN co nm bz ln k mJ kz bz<k||bz>=lB dS()kF ll bz<k+2*n kw lB au[(bz-k)%n].co kO ot(pp n)%m].co nw iB nm bz ln k mJ kz bz<k+2*n kw 0 lc j=(pp n)%m kO bz<k+2*n+m?ot j].ci:-ot j].ci oY bR b4 nm bz){bR s;s.first=s.g4=bz;s mx=co mD bz ni k mJ nL hI*hk=nullptr kz bz<k+n)hk=&lB au[bz-k].as;nI oC m)hk=&ot pp n].as;nI bz>=k+2*n+m)hk=&ot pp n-m].ar kz hk){s.gS=hk->br+s mx;s.bA=hk->db;}s.bE=iB mD bz);s.dP=lO 0.0,s.bE);s.dO=lx 0.0,s.bE)kO s;}bR jR ma in kL bR&a kL bR&b){kH eE=lB hZ(a.g4,b.oa;bR s;s.first=a.first;s.g4=b.g4;s mx=a mx+eE+b mx;s.gS=lx a.gS+eE+b mx,b.gS);s.bA=lO a.bA,b.bA-eE-a mx);s.bg=a.bg&&b.bg&&a.gS+eE<=b.bA;s.cM=a.cM+mh a.g4,b.oa+b.cM;s.bE=a.bE+b.bE;s.dP=lO a.dP,a.bE+b.dP);s.dO=lx a.dO,a.bE+b.dO)kO s;}ak::ak nm ae mf an){fy mD nl md ak::mg{dK.mg;cD.mg;ol lp mA ak::dY kC ak&bS kS this==&bS kw;dK=pu dK;cD=pu cD;es=pu es;dn=pu dn;h8=pu h8 kz dD ks<pu dD ks)dD lC pu dD ks)ox<pu dD lN i)dD os=pu dD os ld i=pu dD ks;i<dD lN i)dD os.mg;iz=pu iz;fZ=pu fZ;mA ak::fy nm ae mf an kS&an!=&dK)dK=an;cD.mg;ol=fP.fetch_add(1,kq memory_order_relaxed ni size=an ks;cD mm size);es mm size);dn mm size);h8 mm size);ko bd=ae kN b9=0,load=0 mw bz:an){cD kD b4 mD bz))ps bX=b9+lB hZ(bd,bz);b9=lx bX+cD.back()mx,cD.back().gS);load+=cD.back().bE;iz+=mh bd,bz);es kD bX);dn kD b9);h8 kD load);bd=bz;}ko fq=0 ld jQ=1;jQ<size;jQ*=2,++fq kS fq==dD ks)dD ou);lv fN=dD[fq];fN lC size)nL ko jM=jQ*2 ld hA lp hA<size;hA+=jM ln mid=lO hA+jQ,size ni end=lO hA+jM,size ll mid>hA){fN[mid-1]=cD[mid-1]mR i=mid-1;i>hA;--i)fN[i-1]=jR mD cD[i-1],fN os lb mM mid<end){fN[mid]=cD[mid]mR i=mid+1;i<end mQ i)fN os=jR mD fN[i-1],cD os lb}mM jQ>size/2 my oY bR ak::dh nm mB g4 mb first>g4||g4>=dK ks)pb out_of_range("invalid route range" ll first==g4 kw cD[first]lc fN=gX(first^g4)-1 kO jR mD dD.at(fN)[first],dD.at(fN)[g4]lb bool ak::ay nm pos mb pos>dK ks lk kz pos==0 kw true lc eS=dK[pos-1],k=lB nG kO eS<k||eS>=k+lB nH;}aL::aL nm lP bn):ai(in),hC(nM,fb(bn){mA aL::cF lq mB end kS first==end kw;fg(fb.dh(ai,mr end-1)md aL::fg kC bR&d4){cE=cE?jR(ai,*cE,d4):d4;mA aL::e0 lq bz){fg(b4(ai,bz)lb nk aL::aY(mb!cE kw 0.0 lc&s=*cE ps bX=ai.hZ(hC,s.first oJ s.bg||bX>s.bA||s.dP<-1e-8||s.dO>ai.bk[hC].ba nu kQ kO ai.cM(hC,s.oa+s.cM nw aL::g2(mb!cE kw 0.0 kO ai.cM(hC,cE->oa+cE->cM;}kv class aX nj bc lY eI{int ae=-1;ko as=0,bv lp}nj ab lD ab pc,cL b7,lU bc>aO={})lW dY nx bS)lW nJ ab&bS)noexcept;kt]cL b7(ku;ml ak&bn pd ku;ml kJ&bo oP ax;}kt]bool cb pd lA ae<aE ks&&!aE lt kX.empty(lb mO aA oP pe nG;}ml eI&ap ne lA a6.at(aa lb mO ad oP a6 ks;}mO am oP pe nH;}mO eY(nz double bY oP cV;}kt]bc&aO oP*bD;}ml lU bc>&ep oP bD;}kt]double aH ne ku;ml av&ag oP*ai;}mO aC ne ku;ml ob&az oP bM;mA dM(ob dH){bM=mC dH lb kt]nk aQ ne nz nk fd ne la ae la py ku lT gq ne)lT d1 pd mf aj)lT insert ne la ae la py)lT cH ne la py)lT eV lq a2 la aF la py)lT fW lq mB pJ lT c9 lq bV la fk la bW la fl)lW hK(ku;pt av*ai;lU bc>bD;mv ak>aE;kJ ax;mv eI>a6 lY hH{kV by lp nk a3;}lX mv hH>cz;ob bM kN cV lp kJ b3;ak bf lW df pd mf an)lW cB lq lM ak&dg lH a3)lW dm lq bV,ak gt la bW,ak gu lH a3);mO cf ne ku;ly bc lD struct jx nF*as kN*ar kN*jb kN*jV;};explicit bc lq aj):aU(aj),gb(g1(aj)),hB(4*gb){}mO ad oP aU;}mO gN oP gb-2;}mO ki oP hB ks*sizeof(double lb kt]jx f3 lq ig kS ig>gN())pb out_of_range("insertion route exceeds fixed scratch capacity");auto*data=hB.data()kO{data,data+gb,data+2*gb,data+3*gb};}ls static ko g1 lq aj){constexpr auto ht=lz<ko>nK/sizeof(double)/4 kz aj>(ht-2)/2)pb length_error("insertion scratch size overflow")kO 2*aj+2;}ko aU,gb;kr kP hB;ly aI lD aI nm lP an):in_(in),hC(nM,jt(an){mA dh lq mB end kS first<end)iM(jt.dh(in_,mr end-1)md bz lq id){iM(b4(in_,id)md d4 kC bR&gO){iM(gO lb kt]bool bg(mb!cE kw true lc&s=*cE kO s.bg&&in_.hZ(hC,s.oa<=s.bA&&s.dP>=-1e-8&&s.dO<=in_.bk[hC].ba nu;}ls void iM kC bR&gO kS!cE){cE=gO kO;}lv s=*cE ps eE=in_.hZ(s.g4,gO.oa ps f6=s.bE ps go=s mx ps hP=s.gS;s.bg=s.bg&&gO.bg&&hP+eE<=gO.bA;s.bA=lO s.bA,gO.bA-eE-go);s mx=go+eE+gO mx;s.gS=lx hP+eE+gO mx,gO.gS);s.dP=lO s.dP,f6+gO.dP);s.dO=lx s.dO,f6+gO.dO);s.bE=f6+gO.bE;s.g4=gO.g4;}const av&in_;ko hC nL ak&jt;kx<bR>cE;};nN dV{const kJ&aw;ko g5=0,gs lp bool aT lG;mO size oP aw ks-(aT?2:0 lb mO bz lq aW mb!aT kw aw[aW]kz aW<g5 kw aw[aW]kz aW<gs-1 kw aw[aW+1]kO aw[aW+2]oY;pf bH nm ae mf aw la de la b2 ln as=aw[de],ar=aw[b2]lc dN=de?aw[de-1]:ae kz b2==de+1)nF hW=mh dN,as)+mh as,ar ll b2+1<aw ks ln jJ=aw[b2+1];hW+=mh ar,jJ)-mh dN,jJ ms hW;}kp gf=aw[de+1]lc eK=aw[b2-1]kN hW=mh dN,as)+mh as,gf)-mh dN,gf)+mh eK,ar ll b2+1<aw ks ln e9=aw[b2+1];hW+=mh ar,e9)-mh eK,e9 ms hW;}pf bU nm ae kL dV&an la at la bz ln bd=at?an.bz(at-1):ae kN em=mh bd,bz ll at<an ks ln gO=an.bz(at);em+=mh bz,gO)-mh bd,gO ms em;}pf bI nm ae kL dV&an la at la as la ar ln bd=at?an.bz(at-1):ae kN em=mh bd,as)+mh as,ar ll at<an ks ln gO=an.bz(at);em+=mh ar,gO)-mh bd,gO ms em;}pf c4 nm ae kL dV&an la de la b2 la as la ar kS de==b2 kw bI mD nl,de,as,ar)kO bU mD nl,de,as)+bU mD nl,b2,ar lb kv oX ab ma in,cL b7,lU bc>aO):ai(&in),bD(aO?mC aO):kq make_shared<bc>(lB nH+lB aS ks)),aE(lB nG),a6(lB nH+lB aS ks),cz(a6 ks kS bD->ad()!=a6 ks kF);ax mm lB nG ni dp=b7.ex(in oJ dp.bg kF+dp.iU);cV=dp.bY ld k lp k<b7.cS lN k)mM!b7.cS[k]li)df(k,b7.cS[k]md oX dY nx bS kS this==&bS kw kz ai!=pu ai kF)mF<aE lN nM mM aE lt oT!=pu aE lt oT)aE lt.dY(pu aE lt);ax mm oK);ax=pu ax;a6=pu a6;cz=pu cz;bM=pu bM;cV=pu cV;mA oX nJ ab&bS)noexcept{using kq swap;nJ ai,pu ai);bD.nJ pu bD);aE.nJ pu aE);ax.nJ pu ax);a6.nJ pu a6);cz.nJ pu cz);bM.nJ pu bM);nJ cV,pu cV);b3.nJ pu b3);nJ bf,pu bf lb cL oX b7(ku{cL aD;aD.cS lC pe nG)mw ae:ax)aD.cS lt=lu kX kO aD;}const ak&oX bn pd ku{static const ak empty kO ae<aE ks?aE lt:empty;}ko oX eY oP kq count_if(a6 oV a6 kY,[](kp&ap)kA ap.ae>=0;}lb double oX aH nB->au.at lj.aH:pe aS.at(aa-n).aH;}ko oX aC nB->cu lj:pe dd(aa-n lb ko oX cf nB->aP lj:pe bb(aa-n md oX df pd mf an ln&in=*ai lc n=lB nH,m=lB aS ks,cO=lB nG mw bz:lu kX ln aa=bz<cO+2*n?(bz-cO)%n:n+(bz-cO-2*n)%m;a6 ny={};}lv jh=aE.at(nM;kW e5=cb(ae ll an li kS e5){jh.mg;ax me mT,nM lb}else{jh.fy mD nl oJ e5)ax.lE mT,nM,ae lb mY p lp p<an lN p ln bz=an[p];kW aK=bz<cO+2*n lc aa=aK?(bz-cO)%n:n+(bz-cO-2*n)%m lK p oY nk oX aQ ne ku{kp loc=ap lj kz lm ae<0 kQ lc ae mX lc&eQ=lu;lv a9=cz ny kz nb by==eQ oT kw nb a3 lc size=eQ kX ks;aI af nc lM eQ);mG 0,lm as);mG lm as+1,lm bv);mG lm bv+1,size);nb by=eQ oT;nb a3=af pD?nk(bH nc lM eQ od bv)-aH lj):kq nullopt kO nb a3;}nk oX fd ne la ae la a la b mb aa>=ad()||mj>=0||ae>=oK kQ lc&d3=lu lc size=d3 kX ks;kW aK=aa<pe nH kz a>b||b>size||(aK&&a!=b)||!d3.ay nc a)||!d3.ay nc b)kQ kz!aK&&pe aS[aa-pe nH].ci>pe bk lt.ba nu kQ;aI af nc lM d3);mG 0,a oy bz(aC lj oJ aK)mG a,b oy bz(cf lj);mG b,size oJ af pD kQ nL dV i9{d3 kX}lc cM=aK?bI nc lM i9,a,aC lj,cf lj):c4 nc lM i9,a,b,aC lj,cf lj)kO aH lj-cM mt oX gq ne ln a3=aQ lj kz!a3 lk lc loc=ap lj lc ae mX;oU kX;no);cV+=*a3;df(nl nR d1 pd mf aj kS ae>=oK||aj li lk lc&old=lu;oG fJ(old kX ks,op kN gl=0 oR aj kS aa>=ad()lk lc loc=ap lj kz lm ae!=mi nM||fJ[lm as]lk;fJ[lm as]=fJ[lm bv]lF;gl+=aH(aa lb aL af nc lM old);lv an=b3;an.mg ld first lp first<old kX ks;kS fJ[first]){++first;continue;}auto end=first+1;nO end<old kX ks&&!fJ[end])++end;af.cF(mr end);an.lE an kY,old kX kG+mr old kX kG+end);first=end;}kp it=af.aY(oJ it lk;cV+=old pE-*it-gl;df(nl nR insert ne la ae la a la b ln a3=fd(aa,lM a,b oJ a3 lk;oU kX oe+b,cf lj)oe+a,aC lj);cV+=*a3;df(nl nR cH ne la a la b kS aa>=ad()||mj<0 lk lc ae=kE ko>(mj ni hv=lu pE;oU kX lc loc=ap lj;no);kW aK=aa oN oS aW kS!aW kw true lc eS=an[aW-1],k=pe nG kO eS<k||eS>=k+am(lb kz!ay(a)||!ay(b)lk kz!aK&&pe aS[aa-am()].ci>pe bk lt.ba nu lk oe+b,cf lj)oe+a,aC lj);ns lk;cB(lM bf,hv-*cM nR eV lq a2 la aF la a la b kS a2>=ad()||aF>=ad()||ap(a2).ae<0||ap(aF).ae>=0 lk lc loc=ap(a2 ni ae mX lc hv=lu pE;oU kX;no);kW aK=aF oN oS aW)kA!aW||an[aW-1]>=oK+am(lb kz!ay(a)||!ay(b)lk oe+b,cf(aF))oe+a,aC(aF));ns lk ps a3=aH(aF)-aH(a2)+hv-*cM kz a3<=1e-9 lk;cB(lM bf,a3)kO true;mA oX cB lq lM ak&dg lH a3 ln k=oK,n=am(),m=pe aS ks lc hg=oS bz)kA bz<k+2*n?(bz-k)%n:n+(pp n)%m;};lv jh=aE.at(nM mw bz:jh kX)a6[hg(bz)]={};kW e5=!jh kX li;kW ha=!dg kX li kz e5!=ha ln il=mT,ae ll ha)ax.lE il,nM pg ax me il lb kq nJ jh,dg oJ ha)jh.mg ld p lp p<jh kX lN p ln bz=jh kX[p],aa=hg(bz)lK p;}cV+=a3 mt oX fW lq mB second kS first>=ad()||second>=ad()lk kz second<oa kq nJ nS ni a=ap(oa,b=ap(second ll a.ae<0||b.ae<0||a.nf b.ae lk lc hd=oS a2 la aF ln loc=ap(a2);auto aw=bn(lm nM kX;np ni a0=aF<am()?lm as:lm bv-1;aw.lE aw kG+a0,cf(aF));aw.lE aw kG+lm as,aC(aF))kO aw;}lc fs=hd(nS),ft=hd(second,oa;ak gt nc a.lM fs),gu nc b.lM ft);aL eL nc a.lM gt),eM nc b.lM mL bn(a.nM pE+bn(b.nM pE-*gi-*gj kz a3<=1e-9 lk;dm(a.lM mC gt),b.lM mC gu),a3 nR c9 lq bV la fk la bW la fl kS bV==bW||bV>=oK||bW>=oK lk lc&iD=bn(bV ni&iE=bn(bW ll fk>iD kX ks||fl>iE kX ks lk mw&loc:a6 kS(lm nf mi bV)&&lm as<fk&&lm bv>=fk)||(lm nf mi bW)&&lm as<fl&&lm bv>=fl)lk;}kJ fs(iD kX oV iD kX kG+fk);fs.lE fs kY,iE kX kG+fl,iE kX kY);kJ ft(iE kX oV iE kX kG+fl);ft.lE ft kY,iD kX kG+fk,iD kX kY);ak gt nc bV,fs),gu nc bW,ft);aL eL nc bV,gt),eM nc bW,mL iD pE+iE pE-*gi-*gj kz a3<=1e-9 lk;dm(bV,mC gt),bW,mC gu),a3)kO true;mA oX dm lq bV,ak gt la bW,ak gu lH a3){for(lv loc:a6)mM lm nf mi bV)||lm nf mi bW))loc={}lc assign=oS lM ak dg){auto il=mT,nM;kW ha=il!=ax kY&&*il==ae kz dg kX li kS ha)ax me il);aE lt.clear(oq{mM!ha)ax.lE il,nM;aE lt=mC dg lb kp k=pe nG,n=am(),m=pe aS ks lc&aw=lu kX ld p lp p<aw lN p ln bz=aw[p]lc aa=bz<k+2*n?(bz-k)%n:n+(pp n)%m lK p oY;oF bV,mC gt));oF bW,mC gu));cV+=a3;mA oX hK(ku{kp aD=b7().ex(*ai oJ aD.bg||pz aD.bY-cV)>1e-6*lx 1.0,pz aD.bY))kF+aD.iU lb nN hF{ko eN=0,ga=0,fT lp}nj ff lD ff lq ee la aj,bool gx);hF jv(kU rng la c5)lW jw(hF ah lH eW);ml kr kP&ka oP fz;}ml kJ&j2 oP hc;}ph Arm nF ci=1,sum=0,hi lp ko count lp}lT hY;mv Arm>ho;oW Arm>>ix;kJ gR;kr kP fz;kJ hc;ko iS kC mv Arm>&jd,kU rng ku;};kv ff::ff lq ee la aj,bool gx):hY(gx),ho(ee),ix(ee),fz(ee,0),hc(ee,0 kS ee==0 kK)ld p=1;p<=aj;p*=2){gR kD p ll p>aj/2 my;}mM gR li)gR kD 1)kB lv jd:ix)jd lC gR ks lb ko ff::iS kC mv Arm>&jd,kU rng mb!hY)nF jW=0 mw&arm:jd)jW+=arm.ci;nh kj(0,jW)kN dk=kj(rng)ox<jd lN i){dk-=jd os.ci kz dk<=0 kw i;lZ jd ks-1;}constexpr double mu0=0,h7=0.01,jK=1,j1=100;ko ah=0 kN cx=-ky)ox<jd lN i ln&a=jd os ps count=kE double>(a.count)ps iX=h7+count ps j7=(h7*mu0+a.sum)/iX ps jZ=jK+count/2 ps kh=j1+0.5*(a.hi+h7*mu0*mu0-iX*j7*j7);kq gamma_distribution kP j5(jZ,1.0/lx kh,1e-12))ps i0=j5(rng);kq normal_distribution kP jS(j7,1.0/kq sqrt(lx iX*i0,1e-12)))ps ju=jS(rng ll ju>cx){cx=ju;ah=i;}lZ ah;}hF ff::jv(kU rng la c5){hF ah;pK eN=iS(ho,rng);pK ga=iS(ix[pK eN],rng);pK fT=lO c5,gR[pK ga])kO ah;mA ff::jw(hF ah lH eW){eW=lx 0.0,eW);auto hj=[eW](Arm&a){++a.count;a.sum+=eW;a.hi+=eW*eW;a.ci=0.9*a.ci+0.1*(0.01+eW lb;hj(ho[pK eN]);hj(ix[pK eN][pK ga]);fz[pK eN]+=eW mQ hc[pK eN];}nN iH{kV kd=1 kN dU=59.0 kN cJ=200.0 kN jz=kq exp(kq log(1e-4)/6000.0)lT hK lG lT bL lF lT fj lF lT fh lF lT hw lF lT hy lF lT fv lF lT dr lF lT gP lF lT cy lG;ko f7=50000;ko eD=200;ko gQ=48;ko gL=1 lT gx lG;kq string je;};nN iJ{cL b7 kN bY lp ko dR=0 kN i3 lp bool d8 lG lT cy lG;};iJ jU pc kL iH&bs);kv class bO lD virtual~bO(mq kJ mU ku lp ly fA lo bO ov"random";mI e7 lo bO ov"related";mI ge lo bO ov"worst";mI gd lo bO ov"route";mI d7 lo bO ov"opportunity";mI i8 lD virtual~i8(mq bool cC(kZ ku lp ly gz lD void eq(nP i8>move){fO kD mC move)lb bool hh(kZ ku;ls mv nP i8>>fO;ly cR lD virtual~cR(mq void ek(ab&mf aT,kU ku lp};kv using cU=kq chrono::steady_clock::time_point lY al{ko aa=0,ae=0,as=0,ar=0 kN a3 lp}lY aB{mn cx kN bQ=-ky lb lY eF{ko ae=0 kN a3 lp}lY b8{bool aK;bR as,ar kN aH,ci;}nj aX lD explicit aX pc)lT gH ne,cU a1=cU nK nz bool jX ne lA fE.at(aa lb ml mv eF>&dF ne lA e1.at(aa lb ml b8&bq ne lA cd.at(aa lb mO hU oP cd ks*sizeof(b8 lb pt av*ai lX oW eF>>e1 lX kJ hf lX oG fE;mv b8>cd;};ob di nx fn,cU a1);aB eo mu);mn dv mu);mn cj mu la lP an lH gn=-ky))nj c6 lD mn cx mu);ph hl{kV by=0 kN bu=-ky);mn cx;};oW hl>>bT;}nj a5 lD void ib nx mf aj)lW c2 nx la ae mf aj);ml mn&cx ne lA bT.at(aU*ec+aa).ah;}mO ji oP bT ks*sizeof(hl lb ph hl{kV bl=lz<kV>nK;mn ah;};ob jm;ko ec=0,aU lp mv hl>bT lW eA nx la aa la nM lW eB ne lb nj aJ lD kt]static bool h5 nT);explicit aJ nT);aJ nT mf aj);kt]aB cn ne ku lW c2 lq nM;pt ab&bj;using gB=kq pair<ko,aB>;oW gB>>cA;kJ aU;ly hG lD template<class kf>mn get ne la ae la aA,kV aG,kV cT lH cq,bool ds,kf&&kl kS bT ks<=pG ks<aA)row lC aA);lv a9=row lt;kW gM=nb gc&&nb cT==cT&&nb ds==ds;kW j9=gM&&(ds?nb aG==aG:(og.has_value()||cq<=nb cq)oJ j9){og=kl();nb aG=aG;nb cT=cT;nb cq=cq;nb ds=ds;nb gc lF;}mM og&&!ds){auto io=*og;io.a3+=cq-nb cq kO io;lZ og;}ph hl{mn ah;kV aG=0,cT=0 kN cq lp bool ds lG lT gc lG;};oW hl>>bT;};nN dJ{al ah;ko bi;}nj e8 lD void add(mn ah la bi kS ah&&ah mz>pi fO kD{*ah,bi}lb kt]kx<dJ>cx(mb fO li kQ kO*kq max_element(fO oV fO mk pK a3<b.pK a3;}md fc lq mB pJ{g7(fO,[&](kp&move)kA move.bi==first||move.bi==second||move.pK nf first||move.pK nf second;}md mg{fO.clear(lb ls mv dJ>fO;ly ao lD ao mc pd la py nz nk a3 mP py nz nk bm mP py nz kx<aI>ey mP bB nz nk dG mP py kL aI&ea ku;pt ab&bj;ko dy,b5,dT,c1 lT gW lG;nk bJ kN bF lp bR iZ,hL;mO cc lq aW nz bool cp kC ak&an la bx ku lW oj ku;}nj ed lo i8 ov"parcel_relocate" mt cC(ab&pn hG fb;kt]mn cG nx kL ao&la aa la lP,kV aG ku;ly aq lD aq mc pd la bx nz nk a3 mP bx nz nk bm mP bx ku;pt ab&bj;ko dy,b5,cZ lT gW lG;nk bJ kN bF lp mO cc lq aW ku lW oj ku;}nj dl lo i8 ov"passenger_relocate" mt cC(ab&pn hG fb;kt]mn cG nx kL aq&la aa la lP,kV aG ku;ly g8 lo i8 ov"add-request" mt cC(kZ pn a5 fb lX c6 dZ lX kJ aU;ly fB lo i8 ov"remove-request" mt cC(kZ lw;ly dE lo i8 ov"clean-cut-2opt-star" mt cC(kZ lw;mO j2 oP hc;}mO jc oP fa;}kt]double jG oP gY;}ls mutable ko hc=0,fa lp mutable double gY lp};nN aM{ko a2;al bt kN a3;}nj a7 lo i8 lD explicit a7(oM kA"replace-request" mt cC(ab&lw;kt]kx<aM>dX nx ku;ls bool fX;kt]kx<aM>dz nx ku lY hl{bool gc lG kN bu lp kx<aM>ah;}lY Row{kV by lp mv hl>h6;kV eU lp ak ez kN aQ lp}lX mv Row>jg lY ej{kV ca lp kx<aM>ah;}lY be{aM ah;kV by mK,c7>lX const av*fV=nullptr lX mv kV>ce lX oG d6 lX oW ej>>bK lX h1 dB lW cY nx la ae mf a2 mf aF ku lW d5 nx ku lW fI(ku lW fo pd la aF ku lW fD(ku;};nN aZ{ko nS kN a3;};nk dq nx la mB pJ nj cW lo i8 lD explicit cW(oM kA"swap-request" mt cC(ab&lw;kt]kx<aZ>dX nx ku;ph ej{kV ca lp kx<aZ>ah;}lY be{aZ ah;ko bV,bW;kV i6,i7 mK,c7>lT fX lX const av*ai=nullptr lX mv kV>cv lX oW ej>>bK lX h1 dB lW d5 nx ku lW fo lq bV la bW ku;ly eG lo cR ov"greedy";mA ek(ab&mf,kU lw;ly eH lo cR ov"profit";mA ek(ab&mf,kU pn a5 d0 lX c6 dZ;ly aR lD explicit aR pc la ht=200)lW hx nT)lW ip(cU a1 la eT=50000)lW hX lq ht);ko fK oP cl;}ko gG oP cr mt e6 oP cr==fx ks mt cC(kZ,cU a1);ko b0 oP et;}ko aw oP dK mt i5 oP aV;mA write(nQ ku;ph im{kJ aj,an kN bh lp}nL av&ai;ko is,et=0,dK lp bool aV lG;ko cl=0,cr lp mv kq map<kJ,im>>fx;mv kV>cv lW add nx la lM bool c8)lW f0 pd mf aj mf an lH bh,bool c8 lb;kv iJ jU pc kL iH&bs){using gy=kq chrono::steady_clock kz!oQ gL kK ll oQ cy&&!oQ dr kK ni hA=oz ni a1=hA+le oQ dU));kq mt19937_64 rng(oQ kd)ps g6=oQ dr?1.0:0.0 lc eX=a1-le lO g6,oQ dU*0.05)));mv nP bO>>ew;ew kD lg<fA>());ew kD lg<ge>());ew kD lg<e7>()ll oQ hy)ew kD lg<gd>()ll oQ fv)ew kD lg<d7>());mv nP cR>>gT;gT kD lg<eG>()ll oQ hw)gT kD lg<eH>())lY jH{ko jA,js;};mv jH>ee ld d lp d<ew lN d)mY r lp r<gT lN r)ee kD{d,r});ff iO(ee ks,of nH+of aS ks,oQ gx);gz bL;oI fB>());oI g8>());oI dl>());oI ed>());dE*iT=nullptr kz oQ fj){auto move=lg<dE>();iT=move.get();bL.eq(mC move)lb oI cW>());oI a7>());cL empty;pv cS lC of nG);ab ck(ag,mC empty)ni g3=lO a1,oz)+le lO 1.0,oQ dU*0.05))));ck.dM(di(ck,g3));nP aR>bN lT cy=oQ cy kz oQ dr){bN=lg<aR>(ag,oQ eD ll oQ gP||oQ cy ln eC=oz ni fp=lO eX,eC+le lO 2.0,oQ dU*0.2))));bN->ip(fp,oQ f7 ll bN->e6()){cy lF;bN->hX(lO oQ eD,oQ gQ)lb kq cerr<<"path_prefixes="<<bN->fK()<<" path_completed_taxis="<<bN->gG()<<" path_seconds="<<oi oz)-eC).count()<<'\n' oY mM!cy){kJ da(ck.ad());kq iota(da oV da kY,0);oZ da oV da kY,rng)oR da kS oz)>=a1 my lc ah=dv(ck,aa ll ah&&ah mz>pi ck.lE na lb}mM oQ hK)ck.hK();ab cx=ck;ab af=ck kz bN)bN->hx(ck)kN cJ=oQ cJ ps cX=oQ cJ*1e-4;ko dR lp nO!cy&&cJ>cX ln gK=oz ll gK>=eX my lc gV=iO.jv(rng,ck.eY()ni eN=ee[gV.eN];af.dY(ck ni aT=ew[eN.jA]->ek(af,gV.fT,rng ll oQ fh){kJ g0 ld r lp r<af.ad()mQ r)mM af.ap(r).ae<0)g0 kD r);oZ g0 oV g0 kY,rng);gT[eN.js]->ek(af,g0,rng oq gT[eN.js]->ek(af,aT,rng ll oQ bL&&dR%oQ gL==0)bL.hh(af ll oQ hK)af.hK(ll bN)bN->hx(af)ps em=af.bY()-ck.bY()ps eW=lx 0.0,em/lx 1.0,pz ck.bY())))lT iL=em>=0 kz!iL){nh ii(0,1);iL=ii(rng)<kq exp(em/cJ lb mM iL){ck.nJ af ll ck.bY()>cx.bY()+pi{cx.dY(ck lb}iO.jw(gV,eW)mQ dR;cJ=lx cX,cJ*oQ jz lb mM bN){bN->cC(cx,a1 lb mM oQ hK)cx.hK();iJ aD;aD.b7=cx.b7();aD.bY=cx.bY();aD.dR=dR;aD.i3=oi oz)-hA).count();aD.d8=cJ<=cX;aD.cy=cy&&!bN->i5()kO aD;}kv kJ fA::mU ku{kJ aj(lh);kq iota(aj oV aj kY,0);oZ aj oV aj kY,rng);kJ aT;aT mm count)oR aj kS aT ks>=count my kz kT gq lj)aT kD aa ms aT;}kv kJ ge::ek(kZ la count,kU ku{pr ah=lh kN bG=-ky)mZ ln a3 nD aQ(r ll a3&&*a3>bG){ah=r;bG=*a3 om kJ e7::mU ku{kJ c5 mZ)mM mp ae>=0)c5 kD r ll c5 li kw{}lc gg=c5[rng()%c5 ks]pa mo lc gD nD aC(gg ni br=oS r)kA r<lB nH?lB au oH ow:ot r-lB nH]ow;}ps h2=br(gg);pr ah=lh kN fC=ky)mZ kS mp ae<0||!kT aQ(r)kR kp bz nD aC(r)ps gv=lB hZ(gD,bz)+lB hZ(bz,gD)+pz h2-br(r)ll gv<fC){fC=gv;ah=r om kJ gd::mU ku{auto bk nD bo();oZ bk oV bk kY,rng);kJ aT mw ae:bk kS aT ks>=count my;kJ aj mZ)mM mp nf mi nM)aj kD r ni hT=count-aT ks kz aj ks<=hT kS kT d1(lM aj))aT.lE aT kY,aj oV aj kY oq{for(int iN lp iN<4 mQ iN){oZ aj oV aj kY,rng)nL kJ i4(aj oV aj kG+hT ll kT d1(lM i4)){aT.lE aT kY,i4 oV i4 kY);break oY}lZ aT;}kv kJ d7::mU mb!count kw{}pa mo lc&dH nD az();kJ ih,c5;kr kP ik mZ kS mp ae>=0){c5 kD r);continue nw fu nD aH(r ll dH){dH->gH(r ni&dF=dH->dF(r);fu=dF li?0:lx 0.0,dF.front().a3 lb mM fu>pi{ih kD r);ik kD fu lb}mM c5 li kw{};ko gg kz ih li)gg=c5[rng()%c5 ks]pg gg=ih[kq discrete_distribution<ko>(ik oV ik kY)(rng)]lc bv=oS r)kA r<mW?lB aP(r):lB bb(r-mW lb lc br=oS r)kA r<mW?lB au oH ow:ot r-mW]ow;}lc iu nD aC(gg),jl=bv(gg);mv kq pair<double la>>fR mR r:c5 ln as nD aC(r),kc=bv(r)ps gv=0.5*(lB hZ(iu,as)+lB hZ(as,iu)+lB hZ(jl,kc)+lB hZ(kc,jl))+pz br(gg)-br(r));fR ou gv,r lb kq sort(fR oV fR kY);nh ii(0,1);kJ aT;nO!fR li&&aT ks<count ln aW=lO fR ks-1,kE ko>(kq pow(ii(rng),3)*fR ks)ni r=fR[aW].second;fR me fR kG+aW ll kT gq(r))aT kD r ms aT;}kv nd b8 eP nm aa ln n=lB nH;kW aK=aa<n lc as=aK?lB cu lj:lB dd(aa-n ni ar=aK?lB aP lj:lB bb(aa-n)kO{aK,b4 mD as),b4 mD ar),aK?lB au ny.aH:ot aa-n].aH,aK?0:ot aa-n].ci}oY aX::aX pc):ai(&ag),e1(of nH+of aS ks),hf(e1 ks,0),fE(e1 ks,op,cd(e1 ks){mY aa lp aa<cd lN aa)cd ny=eP(ag,aa lb bool aX::gH ne,cU a1 mb fE.at lj kw true pa*ai lc n=lB nH lc as=aa<n?lB cu lj:lB dd(aa-n ni ar=aa<n?lB aP lj:lB bb(aa-n ni aH=aa<n?lB au ny.aH:ot aa-n].aH nL ak empty;lv dF=e1 ny kB lv ae=hf ny;ae<lB bk lN ae kS a1!=cU nK&&kM>=a1 lk;aL af mD lM empty oy e0(as oy e0(ar ni cM=af.aY(ll cM)dF kD{lM aH-*cM}lb kq sort(dF oV dF mk a3>b.a3||(a.a3==b.a3&&a.ae<b.ae lb);fE ny lF kO true;}ob di nx fn,cU a1){auto dH=kq make_shared<aX>(fn.ag())ld aa lp aa<fn.ad()mQ aa kS!dH->gH(aa,a1)my;lZ dH;}nd void fF(aB&aD,al ah kS!oh||pK a3>oh mz||(pK a3==oh mz&&pK ae<oh->nM kS oh)aD.bQ=lx aD.bQ,oh mz);oh=ah;}else aD.bQ=lx aD.bQ,pK a3 md h3(aB&aD kL aB&an kS an.cx)fF(aD,*an.cx);aD.bQ=lx aD.bQ,an.bQ lb bool dW(aB&aD kL kZ la aa ln&dH nD az();kW cN=dH&&dH->gH lj kz cN){ko hp=0 mw&empty:dH->dF lj kS kT cb(pv nM kR fF(aD,{aa,pv lM 0,0,pv a3}ll++hp==2 my;}lZ cN;}b8 bq mu ln&in=mo lc am=mW;kW aK=aa<am kO{aK,b4 mD kT aC lj),b4 mD aK?lB aP lj:lB bb(aa-am)),kT aH lj,aK?0:ot aa-am].ci};}aB c3 mu la lP an kL b8&data,bc&f9 lH fL=-ky),bool iP lG){aB aD lc&in nD ag(ll nX ci>lB bk lt.ba nu kw aD lc b6=an kX ks lc as=nX as.mr ar=nX ar.first lc&aw=an kX lc f3=f9.f3(b6);auto*d9=f3.as;auto*dw=f3.ar;auto*cw=f3.jb oL ln bd=pos?aw[pos-1]:ae lc h9=pos<b6?mh bd,aw ng):0.0;d9 ng=mh bd,as)-h9+(pos<b6?mh as,aw ng):0.0);dw ng=mh bd,ar)-h9+(pos<b6?mh ar,aw ng):0.0);cw ng=mh bd,as)+mh as,ar)-h9+(pos<b6?mh ar,aw ng):0.0 lb kp f4=fL kN dc=ky)kN ef=ky)oL){dc=lO dc,cw ng oJ nX aK){dc=lO dc,ef+dw ng);ef=lO ef,d9 ng lb}mM nX aH-dc nu<f4 kw aD;auto*bC=f3.jV kz!nX aK){bC[b6+1]=ky)ld pos=b6+1;pos-->0;)bC ng=lO dw ng,bC[pos+1]lb mY a lp a<=b6 mQ a kS!po a)kR kp bu=[&]kA lx fL,iP&&oh?oh mz:aD.bQ lb lc f1=nX aK?cw[a]:lO cw[a],d9[a]+bC[a+1]ll nX aH-f1 nu<bu()kR aI ea mD nl);ea.dh(0,a);ea.d4(nX as oJ ea pD)continue kz nX aK){auto af=ea;af.d4(nX ar);mG a,b6 ll af pD)fF(aD,{aa,lM a,a,nX aH-cw[a]}oq{kp ex=oS b lH em){auto af=ea;mG a,b oy d4(nX ar);mG b,b6 ll af pD)fF(aD,{aa,lM a,b,nX aH-em}lb ld b=a;b<=b6 mQ b kS!po b)kR kp em=b==a?cw[a]:d9[a]+dw[b]kz nX aH-em nu<bu()kR ex(b,em lb}lZ aD oY mn cj mu la lP an lH gn kS aa>=lh||ae>=kT oK||oc kQ kO c3 pw,nl,bq pw),kT aO(),gn,true).cx;}nd aB dj mu,bool fr){aB aD kz oc kw aD;kW cN=dW(aD,ac,aa ni data=bq pw);lv f9 nD aO(ni bu=[&]kA fr?aD.bQ:(oh?oh mz:-ky)lb kz cN){mV ae:kT bo())h3(aD,c3 nZ,f9,bu(),!fr)oq{mY ae lp ae<kT oK mQ nM h3(aD,c3 nZ,f9,bu(),!fr)ms aD oY aB eo mu)kA dj pw,true lb mn dv mu)kA dj pw,op.cx;}mn c6::cx mu kS oc kQ;aB aD kz!dW(aD,ac,aa)kw dv pw ll bT ks<=pG ks<kT oK)row lC kT oK ni&data nD az()->bq lj;lv f9 nD aO(nn ln&an nD lu;lv a9=row lt lc bu=oh?oh mz:-ky ll nb by!=an oT||bu<nb bu){nb cx=c3 pw,nl,data,f9,bu,true).cx;nb by=an oT;nb bu=bu;}mM nb cx)fF(aD,*nb cx ms oh;mA a5::eA mu la nM{lv a9=pq lc&an nD lu;og pj ll kT cb(nM){og=c3 pw,nl,pA->bq lj,kT aO(),1e-9,true).cx kz og&&og mz<=pi og pj oq{mV&empty:pA->dF lj)mM pv nf ae kS pv a3>pi og=al{aa,lM 0,0,pv a3};break oY nb bl=an.bl(md a5::eB ne){lv iy=bT[aU*ec+aa].ah;iy pj)mF<ec mQ ae ln&ah=pq.ah kz ah&&(!iy||ah mz>iy mz))iy=ah;}mA a5::ib nT mf aj ln&dH nD az(oJ dH kF ll jm!=dH||ec!=kT oK||aU!=lh){jm=dH;ec nD oK;aU=lh kz ec==lz<ko>nK||aU>lz<ko>nK/(ec+1))pb length_error("positive insertion cache size overflow");bT.oF aU*(ec+1),hl{}lb mV aa:aj kS oc kR dH->gH lj mF<ec mQ nM mM pq.bl!=kT lu oT)eA pw,nM;eB(aa lb mA a5::c2 nT la ae mf aj){mV aa:aj kS oc kR kp bd=cx lj;eA pw,ae ni&dg=pq.ah kz bd&&bd->nf nM eB lj;nI dg&&(!bd||dg mz>bd mz||(dg mz==bd mz&&ae<bd->nM))bT[aU*ec+aa].ah=dg oY bool aJ::h5 nT)kA pA&&lh-kT eY()>=4;}aJ::aJ nT):bj(ac),cA(pH 0 oJ pA kw oR aU kS oc kR kp data=bq pw);lv cn=cA ny;cn mm kT bo()ks nn)cn ou lM c3 nZ,kT aO())lb}aJ::aJ nT mf aj):bj(ac),cA(lh){oG ir(lh,op oR aj kS ir.at lj||oc kR ir ny lF;aU kD aa oJ pA kR kp data=bq pw nn)cA ny ou lM c3 nZ,kT aO())lb}aB aJ::cn ne ku{aB aD kz bj.mj>=0 kw aD kz!bj.az()kw eo(bj,aa);dW(aD,bj,aa)mw&[nl]:cA.at lj)h3(aD,an)kO aD;mA aJ::c2 pd kS!bj.az()kw lc&an=bj.lu oR aU kS bj.mj>=0 kR lv cn=cA ny lc hp=kq lower_bound(cn oV cn kY,lM[]kC gB&a9 la id)kA nb first<id;}ni data=bq(bj,aa);auto dg=c3(bj,aa,nl,data,bj.aO()ll hp==cn kY||hp->first!=nM cn.lE hp,{lM mC dg)})pg hp->second=mC dg lb}kv void eG::ek(kZ mf aT,kU ku{mV aa:aT ln ah=dv pw ll ah&&ah mz>pi kT lE na lb}kv class gA lo cR ov"regret2";mA ek(ab&mf,kU lw;};kv void gA::ek(kZ mf aT,kU ku{oG ia(lh,op oR aT)ia.at lj lF;aJ bn mE aT);nO true){mn ah kN fi=-1 oR aT kS!ia ny kR kp cn=bn.cn lj kz!cn.cx||cn.cx mz<=1e-9 kR kH i2=mS cn.bQ)?cn.cx mz-cn.bQ:cn.cx mz kz!ah||i2>fi||(i2==fi&&cn.cx mz>ah mz)){ah=cn.cx;fi=i2 oY mM!ah||!kT lE ah->na)kw;ia[ah->aa]lG;bn.c2(ah->ae lb}kv void eH::ek(kZ mf g0,kU ku{kW cN=bool(pA ll cN)d0.ib mE g0)kB;;){mn ah kN iW=0 oR g0 kS oc kR kp hR=cN?d0.cx lj:dZ.cx pw ll hR&&hR mz>1e-9&&hR mz>iW){ah=hR;iW=hR mz oY mM!ah kw kz!kT lE ah->na)kF ll cN)d0.c2 mE ah->lM g0 lb}kv aR::aR pc la ht):ai(ag),is(ht),fx(of nG),cv(fx ks,0 kS!ht kK md aR::add nT la lM bool c8 ln&aw nD lu kX kz aw li kw lc k nD oK,n=mW;kJ aj kN aH=0 mw bz:aw){ko aa kz bz<k+n)aa=bz-k;nI oC ai.aS ks)aa=n+pp n pg continue;aj kD aa);aH+=kT aH(aa lb kq sort(aj oV aj kY ni bh=aH-kT lu pE;f0(lM aj,aw,bh,c8 md aR::f0 pd mf aj mf aw lH bh,bool c8 kS!mS bh)||bh<=1e-9 kw;lv iG=fx lt lc hp=iG.find(aj ll hp!=iG kY&&hp->second.bh>=bh kw;iG[aj]=im{aj,aw,bh}kz c8&&iG ks>mN ip(cU a1 la eT kS!eT kK);cl=cr=0 kz kM>=a1 kw pa ai lc n=lB nH,nr=n+lB aS ks,nv=lB dS()lY jy{ko as,ar kN gS,db,iA,h4,co,aH,ci;};mv jy>aj ld r lp r<nr mQ r kS r<n ln&p=lB au oH;aj kD{lB cu(r),lB aP(r),p ow,p.as.db,0,ky),p.co,p.aH,0}oq{kp&p=ot r-n];aj kD{lB dd(r-n),lB bb(r-n),p ow,p.as.db,p.ar.br,p.ar.db,p.co,p.aH,p.ci}lb}struct Arc{ko aa kN ct;};oW Arc>>gO(nv)ld bz lp bz<nv mQ bz kS kM>=a1 kw;lv j0=gO[bz]ld r lp r<nr mQ r ln eE=lB cK[bz*nv+aj oH.as]lc j6=8*lz kP::epsilon()*lx{1.0,pz aj oH.db),pz eE)}ni db=aj oH.db-eE nu+j6 kz db>=-1e-8)j0 kD{r,db}lb kq sort(j0 oV j0 mk ct>b.ct||(a.ct==b.ct&&a.aa<b.aa lb lb cl=cr lp pF iY(nr,0),hQ(nr,0);kJ an,c5;an mm 2*nr);c5 mm nr)mF<fx lN ae kS kM>=a1 my lc now=kM lc hD=a1==cU nK?a1:now+(a1-now)/kE cU::duration::rep>(fx ks-nM;ko gw lp bool eO lG lc iF=[&](auto&&jF la g4 lH hZ lH cM lH aH lH load la e3)->void{mM eO kw kz gw>=eT||((gw&63)==0&&kM>=hD)){eO lF kO;}++gw kz!e3&&aH-cM>pi{auto key=c5;kq sort(key oV key kY);f0(lM key,an,aH-cM,true lb mM!mS hZ)kw ox<c5 lN i ln r=c5 os kz!hQ oH kR kp&q=aj oH ps bX=hZ+lB cK[g4*nv+q.ar]kz!mS bX)||bX>q.h4 nu kR hQ oH lp an kD q.ar);jF(jF,q.ar,lx bX,q.iA)+q.co,cM+lB c0[g4*nv+q.ar],aH,load-q.ci,e3-1);an.pop_back();hQ oH=1 kz eO kw;}mV&arc:gO[g4]kS hZ>arc.ct my lc r=arc.aa kz iY oH kR kp&q=aj oH kz load+q.ci>lB bk lt.ba nu kR kH bX=hZ+lB cK[g4*nv+q.as]kz bX>q.db nu kR kH b9=lx bX,q.gS)+q.co ps iV=cM+lB c0[g4*nv+q.as];iY oH=1;c5 kD r);an kD q.as ll r<n){an kD q.ar ni gk=b9+lB cK[q.as*nv+q.ar]kz mS gk))jF(jF,q.ar,gk+q.co,iV+lB c0[q.as*nv+q.ar],aH+q.aH,load,e3);an.pop_back(oq{hQ oH=1;jF(jF,q.as,b9,iV,aH+q.aH,load+q.ci,e3+1);hQ oH lp}an.pop_back();c5.pop_back();iY oH=0 kz eO kw oY;iF(iF,lM 0,0,0,0,0);cl+=gw;cr+=!eO;}mA aR::hx nT kS&mo!=&ai kF nn kS cv lt==kT lu oT kR cv lt nD lu oT;add mE lM true lb mA aR::hX lq ht kS!ht kK);is=ht kB lv iG:fx)nO iG ks>mN write(nQ ku{out<<"taxi,benefit,route\n" oA mF<fx lN nM mV&[key,bP]:fx lt){out<<ae+1<<','<<bP.bh<<",\"" ox<bP.an lN i kS i)out<<' ';out<<ai.gZ(bP.an os lb out<<"\"\n" oY bool aR::cC(kZ,cU a1 kS&mo!=&ai kF nn)add mE lM op;oW im>>b0(fx ks);mv int>hz(fx ks,-1),cs(fx ks,-1);pF dx(lh,0);kJ hN(lh,0),eh;et=0 kN bZ=0 mF<fx lN nM{lv row=px mw&[key,bP]:fx lt)row kD bP);kq sort(row oV row mk bh>b.bh||(a.bh==b.bh&&a.aj<b.aj lb);et+=row ks;pF ic(lh,0)ox<row lN i){for(auto r:row os.aj)ic oH=1 lT ck=!row os.aj li mR r:row os.aj)ck&=mp nf mi ae ll ck){ko count=0 mZ)count+=mp nf mi ae ll count==row os.aj ks){cs lt=mi i);bZ+=row os.bh oY}mY r lp r<ic lN r)hN oH+=ic oH;}kJ hn(fx ks,0)mF<b0 lN nM mV&bP:px)for(auto nY hn lt+=hN oH-1 mF<b0 lN nM mM!px li)eh kD nM;kq stable_sort(eh oV eh kY,[&](auto a,auto b)kA hn[a]>hn[b];}ni el=[&]kC im&bP){for(auto nY mM dx oH lk kO true;};dK lp aV lG lc fe=[&]kC kJ&gI){kq fill(dx oV dx kY,0)mF<cs lN ae kS cs lt<0||kq find(gI oV gI kY,nM!=gI kY)continue mR r:px[cs lt].aj)dx oH=1 nw e2 lp mv int>gF(gI ks,-1),f2 mR ae:gI){f2 kD cs lt ll cs lt>=0)e2+=px[cs lt].bh nw dA=e2;kq function<void lq lH)>iF=oS fH lH bh on mM fH==gI ks kS bh>dA+pi{dA=bh;f2=gF;lZ nw iR=bh ld d=fH;d<gI lN d)mV&bP:b0[gI oD dA+1e-9 kw lc ae=gI[fH]ox<px lN i ln&bP=px os kz!el(bP)kR gF[fH]=mi i)mR nY dx oH=1;iF(fH+1,bh+bP.bh)mR nY dx oH=0 kz aV my;}gF[fH]=-1 kz!aV)iF(fH+1,bh lb kz kM<a1)iF(0,0)pg aV lF kz dA<=e2+1e-9 lk ox<gI lN i)cs[gI os]=f2 os;bZ+=dA-e2 kO true;}lT a8 lF;nO a8&&!aV){a8 lG ld a lp a<oB a)mY b=a+1;b<oB b)a8=fe({eh[a],eh[b]})||a8;}mY a lp a<oB a)mY b=a+1;b<oB b)mY c=b+1;c<oB c)fe({eh[a],eh[b],eh[c]});kr kP dL(lh,0),gE=dL kN hJ=ky);oW double>>ez(b0 ks);oW ko>>hm(b0 ks)ld hO lp hO<192&&!aV mQ hO kS kM>=a1){aV lF;break nw jP=kq accumulate(dL oV dL kY,0.0);kJ jY(dL ks,0)mR ae:eh)nF f5 lp int gF=-1 ox<px lN i ln&bP=px os kN value=bP.bh mR nY value-=dL oH kz value>f5){f5=value;gF=mi i lb}jP+=f5 kz gF>=0)for(auto r:px[gF].aj)++jY oH;}mM jP<hJ){hJ=jP;gE=dL nw j8=0 ld r lp r<dL lN r)mM dL oH>0||jY oH>1){kH iq=kE double>(jY oH)-1;j8+=iq*iq;}mM!j8||jP<=bZ+1e-7 my ps km=0.9*(jP-bZ)/j8 ld r lp r<dL lN r)dL oH=lx 0.0,dL oH+km*(kE double>(jY oH)-1)lb dL=mC gE)ps ie=kq accumulate(dL oV dL kY,0.0)mR ae:eh){lv fY=ez lt mw&bP:px)nF value=bP.bh mR nY value-=dL oH;fY kD value lb lv gJ=hm lt;gJ lC fY ks);kq iota(gJ oV gJ kY,0);kq stable_sort(gJ oV gJ kY,[&](auto a,auto b)kA fY[a]>fY[b];}lb kq fill(dx oV dx kY,0);kq function<void lq lH lH)>hh=oS fH lH bh lH f8 on mM fH==eh ks kS bh>bZ+pi{bZ=bh;cs=hz;lZ nw iR=bh ld d=fH;d<eh lN d)mV&bP:b0[eh oD bZ+1e-9 kw kN gp=bh+ie-f8 ld d=fH;d<eh lN d ln ae=eh[d]kN f5=0 mR i:hm lt kS ez lt os<=0 my kz el(px os)){f5=ez lt os;break oY gp+=f5;}mM gp<bZ-1e-7 kw lc ae=eh[fH]mR i:hm lt ln&bP=px os kz!el(bP)kR hz lt=mi i)kN ke=f8 mR nY{dx oH=1;ke+=dL oH;}hh(fH+1,bh+bP.bh,ke)mR nY dx oH=0 kz aV my;}hz lt=-1 kz!aV)hh(fH+1,bh,f8 lb kz kM<a1)hh(0,0,0)pg aV lF kz bZ<=kT bY()+1e-9 lk;cL af;af.cS lC kT oK)mF<cs lN nM mM cs lt>=0)af.cS lt=px[cs lt].an lc hM=af.ex(ai oJ hM.bg||pz hM.bY-bZ)>1e-6 kF);ab iw(ai,mC af),kT ep());iw.dM(pA);kT dY(iw)kO true;}kv bool gz::hh(kZ ku{bool a8 lG kB int jN lp jN<2 mQ jN){bool eu lG mw&move:fO){eu=move->cC(ac)||eu;}mM!eu my;a8 lF;lZ a8;}kv bool g8 nA kW cN=bool(pA ll aU ks!=pH 0 lb mM cN)fb.ib mE aU)lT a8 lG kB;;){mn cx oR aU kS oc kR kp move=cN?fb.cx lj:dZ.cx pw ll move&&move mz>1e-9&&(!cx||move mz>cx mz))cx=move;}mM!cx my kz!kT lE cx->aa,cx->lM cx->as,cx->ar)kF);a8 lF kz cN)fb.c2 mE cx->lM aU ms a8;}kv bool fB nA mv kx<double>>iC(lh)ok)iC ny nD aQ lj lT a8 lG kB;;){kx<ko>cx kN bG=1e-9 ok kS iC ny&&*iC ny>bG){cx=aa;bG=*iC ny oY mM!cx my lc ae=kE ko>(kT ap(*cx).ae oJ kT gq(*cx)kF);a8 lF;iC[*cx]pj)ok)mM kT mj==mi nM)iC ny nD aQ(aa ms a8;}kv aq::aq mu):bj(ac),dy lj,b5(0),cZ(0 kS aa>=mW||kT mj<0 kw lc&pk lj;b5 mX;cZ=lm as;gW lF;bJ nD aQ lj lc&an nD bn(b5);bF=bH(mo,b5,an od bv lb ko aq nV}ko aq::cc lq aW lA aW<cZ?aW:aW+2;mA aq::oj ku{kp fM=lO end,cZ ll first<fM)mG mr fM ni d2=lx mr cZ ll d2<end)mG d2+2,end+2 lb nk aq::a3 pd la bx mb ae>=bj.oK kQ kO a3(lM bj.lu,bx lb nk aq::bm mP bx nq)||bx>oE i9{an kX,cZ,cZ+1,nf b5}lc em=bI(pB),lM i9,bx,bj.aC(dy),pB).aP(dy))kO bF-em;}nk aq::a3 mP bx nq)||bx>an kX ks-(nf b5?2:0)kQ pa pB);nC:0 ll hV kS bx&&!po cc(bx-1)+1)kQ;}nI!bJ||!po bx)kQ;aI af mD nl ll pC 0,bx)pg mG 0,bx oy bz(bj.aC(dy)oy bz(lB aP(dy)ll pC bx,ch)pg mG bx,ch oJ af pD kQ kO bm(nl,bx lb mn dl::cG nT kL aq oO 0)ld a lp a<=b6 mQ a ln fQ=b1.bm(nl,a oJ fQ||*fQ nu<nU a3=b1.a3(nl,a ll a3&&(!bw||*a3>bw mz))bw=al{r,lM a,a,*a3};lZ bw;}lb bool dl nW aq oo pK lM fm,0 ll a3)g0.add(al{r,pK lM 0,0 lS r lp r<mW nE):(lQ)ld r lp r<mW nt,dk lb lZ a8;}kv ao::ao mu):bj(ac),dy lj,b5(0),dT(0),c1(0 kS aa<mW||aa>=lh||kT mj<0 kw lc&pk lj;b5 mX;dT=lm as;c1=lm bv;gW lF;bJ nD aQ lj;iZ=b4(mo,kT aC lj);hL=b4(mo,mo.bb(aa-mW)ni&an nD bn(b5);bF=bH(mo,b5,an od bv lb ko ao nV}ko ao::cc lq aW mb aW<dT kw aW kz aW<c1-1 kw aW+1 kO aW+2 mt ao::cp kC ak&an la bx lA!bx||an.ay(pB),cc(bx-1)+1 md ao::oj ku{kp fM=lO end,dT ll first<fM)mG mr fM ni gm=lx mr dT ni he=lO end,c1-1 ll gm<he)mG gm+1,he+1 ni d2=lx mr c1-1 ll d2<end)mG d2+2,end+2 lb nk ao::a3 pd la a la b mb ae>=bj.oK kQ kO a3(lM bj.lu,a,b lb nk ao::bm mP a la b nq)||a>b||b>oE i9{an kX,dT,c1,nf b5}lc em=c4(pB),lM i9,a,b,bj.aC(dy),pB).bb(dy-bj.am()))kO bF-em;}nk ao::a3 mP a la b ku{kp ea=ey(nl,a)kO ea?dG(nl,a,b,*ea):kq nullopt;}kx<aI>ao::ey mP a nq)kQ;nC:0 ll a>ch kQ pa pB ni ci=ot dy-bj.am()].ci kz ci>lB bk lt.ba nu kQ kz hV kS!cp(an,a)kQ;}nI!bJ||!po a)kQ;aI af mD nl ll pC 0,a)pg mG 0,a oy d4(iZ oJ af pD kQ kO af;}nk ao::dG mP a la b kL aI&ea ku{nC:0 ll a>b||b>ch kQ pa pB ll hV kS!cp(an,b)kQ;}nI!po b)kQ;auto af=ea kz pC a,b)pg mG a,b oy d4(hL ll pC b,ch)pg mG b,ch oJ af pD kQ kO bm(nl,a,b lb mn ed::cG nT kL ao oO 0 ni&in=mo lc pk(r)nL dV i9{an od bv,nf bi}lc aC nD aC(r ni fG=lB bb(r-mW);kr kP fw(b6+1),ev(b6+1),er(b6+1),bC(b6+2,ky))oL){fw ng=bU mD lM i9,pos,aC);ev ng=bU mD lM i9,pos,fG);er ng=bI mD lM i9,pos,aC,fG lb mY pos=b6+1;pos-->0;)bC ng=lO ev ng,bC[pos+1])ld a lp a<=b6 mQ a ln f1=lO er[a],fw[a]+bC[a+1]ll b1.cI()-f1 nu<nU ea=b1.ey(nl,a oJ ea)continue ld b=a;b<=b6 mQ b ln em=b==a?er[a]:fw[a]+ev[b]lc fQ=b1.cI()-em kz fQ nu<nU a3=b1.dG(nl,a,b,*ea ll a3&&(!bw||*a3>bw mz))bw=al{r,lM a,b,*a3};}lZ bw;}lb bool ed nW ao oo pK lM fm,0,0 ll a3)g0.add(al{r,pK lM 0,0 lS r=mW;r<lh nE):(lQ)ld r=mW;r<lh nt,dk lb lZ a8;}kv nd struct iK{ko ae;kJ hb;};kJ gr nT la ae ln&in=mo lc&an nD lu lc k mJ;ko eR lp kJ kb{0}ld pos lp pos<an kX lN pos ln bz=an kX ng kz oC m)++eR;nI bz>=k+2*n+m)--eR kz!eR&&po pos+1))kb kD pos+1 ms kb oY bool dE nA++hc pa mo lT a8 lG kB;;){mv iK>cS;cS mm kT bo()ks nn)cS kD{lM gr mE nM})lT eJ lG ox<cS ks&&!eJ mQ i){mY j=i+1;j<cS ks&&!eJ mQ j ln bV=cS os.lM bW=cS[j].ae lc&a nD bn(bV ni&b nD bn(bW ni hr=a kX ks,hs=b kX ks kN bG=1e-9;ko jj=0,jk=0 mw fk:cS os.hb){mV fl:cS[j].hb ln jq=fk?a kX[fk-1]:bV lc jr=fl?b kX[fl-1]:bW kN a3=0 kz fk<hr ln jo=a kX[fk];a3+=mh jq,jo)-mh jr,jo lb mM fl<hs ln jp=b kX[fl];a3+=mh jr,jp)-mh jq,jp lb mM a3<=bG nu kR aI gt mD bV,a);gt.dh(0,fk ll fl<hs)gt.d4(b.dh mD fl,hs-1)oJ gt pD kR aI gu mD bW,b);gu.dh(0,fl ll fk<hr)gu.d4(a.dh mD fk,hr-1)oJ gu pD kR bG=a3;jj=fk;jk=fl oY mM bG>pi{kH jL nD bY(oJ kT c9(bV,jj,bW,jk)kF)mQ fa;gY+=kT bY()-jL;a8=eJ lF oY}mM!eJ kw a8 oY kv kx<aM>a7::dX nT mb!fX kw dz(ac);d5(ac);fI()kO dB li?kq nullopt:kx(dB.top().ah md a7::fI(ku{nO!dB li ln&a9=dB.top(ni ae=og.bt.ae lc aF=og.bt.aa kz d6[aF]&&nb by==ce lt&&nb ca==bK lt[aF].ca my;dB.pop(lb}bool a7::c7::operator()kC be&a kL be&b mb a.pK a3!=b.pK a3 kw a.pK a3<b.pK a3 lc key=[]kC aM&c)kA kq tuple(c.a2,c.bt.aa,c.bt.as,c.bt.ar lb kO key(a.ah)>key(b.ah md a7::cY nT la ae mf a2 mf aF mb aF li kw;lv row=bK lt oR aF){++row ny.ca;row ny.ah pj lb kp&dQ nD lu;kJ aw oR a2 ln pk lj;lv gh=jg ny kz gh.eU!=dQ oT){aw=dQ kX;np);gh.ez.fy(mo,lM aw);gh.aQ=bH(mo,lM dQ od bv)-kT aH lj;gh.eU=dQ.bl(lb mV gU:aF){lv aN=row[gU]ps bu=pl?pl mz:1e-9 lc bt=cj mE gU,lM gh.ez,bu-gh.aQ ll bt&&bt mz+gh.aQ>bu)pl=aM{aa,*bt,bt mz+gh.aQ}oY mV aa:aF)fo(lM aa md a7::fo pd la aF ku{kp&aN=bK lt[aF]kz pl){dB.push({*pl,ce lt,aN.ca}lb mA a7::d5 nT ku{kp bk nD oK,aj=lh kz fV!=&mo||ce ks!=bk||d6 ks!=aj){fV=&mo;ce.oF bk,lz<kV>nK);d6.oF aj,op;bK.oF bk,mv ej>(aj));dB=h1{};jg lC aj lb oW ko>>a2(bk);kJ aF,eg ld aa lp aa<aj mQ aa ln pk lj;kW e4=lm ae<0 kz e4){aF kD aa oJ d6 ny)eg kD aa oq a2[lm ae]kD aa);d6 ny=e4;}mY ae lp ae<bk mQ ae ln bl nD lu.bl(ll ce lt!=bl){ce lt=bl;cY mE lM a2 lt,aF lb nI!eg li){cY mE lM a2 lt,eg lb}mM dB ks>2*bk*aj+32)fD(md a7::fD(ku{dB=h1{}mF<bK lN nM mY aF lp aF<d6 lN aF kS d6[aF])fo(lM aF lb}kx<aM>a7::dz nT ku{kJ e4 mZ)mM mp ae<0)e4 kD r ll e4 li kQ;jg lC lh);kx<aM>cx kN bG=1e-9;kJ aw;ak ez ld a2 lp a2<lh mQ a2 ln pk(a2 ll lm ae<0 kR kp ae mX lc&dQ nD lu;lv row=jg[a2]kz row.by!=dQ oT){row.by=dQ oT;row.h6.mg;row.h6 lC lh lb kH aQ=bH(mo,lM dQ od bv)-kT aH(a2)lT iv lG mw aF:e4){lv a9=row.h6[aF]kz!nb gc||(!og&&bG<nb bu)kS!iv){aw=dQ kX;np);ez.fy(mo,lM aw);iv lF;}nb gc lF;nb bu=bG;og pj ni bt=cj mE aF,lM ez,bG-aQ ll bt&&bt mz+aQ>bG)og=aM{a2,*bt,bt mz+aQ};}mM og&&og mz>bG){cx=og;bG=cx mz oY lZ cx mt a7 pI eV(ah->a2,ah->bt.aa,ah->bt.as,ah->bt.ar)kF);a8 lF;lZ a8;}kv nd bool iQ kC aZ&a kL aZ&b)kA a.a3>b.a3||(a.a3==b.a3&&kq tie(a.mr a.pJ<kq tie(b.mr b.pJ lb nk eb nT la a2 la aF ln pk(a2 ni ae mX lc&an nD lu pa mo;kW aK=aF<mW lc ar=aK?lB aP(aF):lB bb(aF-mW);aL af mD nl oy cF(0,lm as oy e0(kT aC(aF)ll aK)af.e0(ar oy cF(lm as+1,lm bv oJ aK)af.e0(ar oy cF(lm bv+1,an kX ks)kO af.aY(lb}nk dq nT la mB second kS first>=lh||second>=lh kQ kz second<oa kq nJ nS ni a nD ap(oa,b nD ap(second ll a.ae<0||b.ae<0||a.nf b.ae kQ lc gi=eb mE nS oJ gi kQ lc gj=eb mE second,first oJ gj kQ kO kT bn(a.nM pE+kT bn(b.nM pE-*gi-*gj mt cW::c7::operator()kC be&a kL be&b lA iQ(b.ah,a.ah md cW::fo lq bV la bW ku{kp&aN=bK[bV][bW]kz pl)dB.push({*pl,bV,bW,cv[bV],cv[bW],aN.ca}md cW::d5 nT ku{kp bk nD aA(ll ai!=&mo||cv ks!=bk){ai=&mo;cv.oF bk,lz<kV>nK);bK.oF bk,mv ej>(bk));dB=h1{};}kJ a8;oG jO(bk,op mF<bk mQ ae ln bl nD lu.bl(ll cv lt!=bl){cv lt=bl;a8 kD nM;jO lt lF oY mM a8 li kw;oW ko>>aj(bk)ok ln ae nD mj kz ae>=0)aj lt kD aa lb mV ae:a8){mY bS lp bS<bk mQ bS kS bS==ae||(jO[bS]&&bS<nM kR kp bV=lO lM bS),bW=lx lM bS);lv aN=bK[bV][bW]mQ aN.ca;pl pj)mw first:aj[bV]){mV second:aj[bW]ln a3=dq mE nS oJ a3||*a3<=1e-9 kR const aZ ah{lO nS),lx nS),*a3}kz!pl||iQ(ah,*pl))pl=ah oY fo(bV,bW lb}mM dB ks>2*bk*bk+32){dB=h1{}ld a lp a<bk mQ a)mY b=a+1;b<bk mQ b)fo(a,b lb}kx<aZ>cW::dX nT mb!fX){kx<aZ>cx ld a lp a<lh mQ a kS kT ap(a).ae<0)continue ld b=a+1;b<lh mQ b ln a3=dq mE a,b oJ a3||*a3<=1e-9 kR const aZ ah{a,b,*a3}kz!cx||iQ(ah,*cx))cx=ah;}lZ cx;}d5(ac);nO!dB li ln&a9=dB.top(ll nb i6==cv[nb bV]&&nb i7==cv[nb bW]&&nb ca==bK[nb bV][nb bW].ca kw og;dB.pop(ms kq nullopt mt cW pI fW(ah->mr ah->pJ kF);a8 lF;lZ a8 oY int main(ln jC=kM;try{kq ios::sync_with_stdio(op;kq cin.tie(nullptr ni ag=a4::fS(kq cin);a4::du::iH bs;oQ kd=1ULL ps jB=oi kM-jC).count()ps gC=300 ps reserve=gC>2?1.0:gC*0.1;oQ dU=gC-jB-reserve kz oQ dU<=0){a4::cL empty;pv cS lC of nG);a4::ei(kq cout,empty,ag oq{kp aD=a4::du::jU(ag,bs);a4::ei(kq cout,aD.b7,ag);kq cerr oA<<"iterations="<<aD.dR<<" seconds="<<aD.i3<<" stop="<<(aD.d8?"temperature":"time_limit")<<'\n';lZ 0;}catch kC kq exception&iU){kq cerr<<"error: "<<iU.what()<<'\n' kO 1 oY
