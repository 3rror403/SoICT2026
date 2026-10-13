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
#include <bit>
#include <memory>
#include <span>
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
#define jW std::size_t
#define jX const auto
#define jY std::
#define jZ jY vector
#define ka .size()
#define kb [[nodiscard]
#define kc )const
#define kd }namespace a2::dh{
#define ke )return
#define kf jY optional
#define kg jY numeric_limits<double>::infinity(
#define kh ;if(
#define ki {return
#define kj (const
#define kk ;for(
#define kl static_cast<
#define km .begin()
#define kn public:jY string_view g7(kc override
#define ko .push_back(
#define kp jZ<jW>
#define kq ,const
#define kr const double
#define ks ;return
#define kt ;double
#define ku ke jY nullopt
#define kv <double>
#define kw )throw jY logic_error
#define kx ){if(
#define ky continue;
#define kz ac.
#define kA jY mt19937_64&
#define kB jY uint64_t
#define kC const bool
#define kD throw jY invalid_argument
#define kE .ax()
#define kF jY chrono::steady_clock::now()
#define kG ab&ac
#define kH ,jW
#define kI .end()
#define kJ );}
#define kK ;jX
#define kL jY make_unique
#define kM kz ad()
#define kN ke false
#define kO .empty()
#define kP (aa)
#define kQ kk jW
#define kR bk(af)
#define kS loc.
#define kT final:public
#define kU )kh
#define kV (jW
#define kW private:
#define kX kc override
#define kY ){jX
#define kZ [af]
#define la =0;
#define lb };kd class
#define lc auto&
#define ld jY max(
#define le .resize(
#define lf {public:
#define lg in.cR(
#define lh insert(
#define li )throw jY runtime_error
#define lj jY chrono::duration_cast<hj::duration>(jY chrono::duration kv(
#define lk ,af,
#define ll true;
#define lm (kc ki
#define ln .clear()
#define lo ke{false,0,
#define lp jY numeric_limits
#define lq kl int>(
#define lr af kq aj&
#define ls )if(all||af==first||af==second)hw.add
#define lt );read(in,
#define lu ap(r).af
#define lv =false
#define lw ka;++
#define lx jY shared_ptr<
#define ly ;mutable
#define lz }return
#define lA kj au&
#define lB jY min(
#define lC kc{if(
#define lD ;struct
#define lE kj kG kH aa
#define lF ap kP.af>=0
#define lG ;void
#define lH .erase(
#define lI kb]const
#define lJ kJ void
#define lK kk jX
#define lL first kH
#define lM kf<ak>
#define lN kz ag()
#define lO )=default;virtual jY string_view g7(kc la virtual
#define lP first,
#define lQ ,double
#define lR ;bool
#define lS kJ return
#define lT .duration
#define lU .reserve(
#define lV break;
#define lW ->a1
#define lX kq kp&
#define lY jY move(
#define lZ af la af<
#define ma jZ<
#define mb in.
#define mc kb]jW
#define md (ac,
#define me ae.c5(
#define mf ls(cw md bY,r lk kz kR,aG),bg kJ
#define mg namespace a2
#define mh ar ka
#define mi }void
#define mj }kp d9(ab&kH,kA kX;lb
#define mk }bool
#define ml kV lr an kH
#define mm k=mb bl ka,n=mb mh,m=mb aS ka
#define mn if(
#define mo kk auto
#define mp for(jX
#define mq d9(kG kH count,kA rng
#define mr ;++
#define ms for kV
#define mt kl jW>(
#define mu kz al()
#define mv jY lower_bound(av km,av kI
#define mw kQ r la r<kM mr r
#define mx )kK
#define my +1e-8<ld 1e-9,bs?bs lW:1e-9))ky jX
#define mz a7.
#define mA aa,ah->af,ah->as,ah->
#define mB namespace{
#define mC kb]double
#define mD kV aa kc
#define mE ;jY uniform_real_distribution kv
#define mF af:kz bm()
#define mG lq af kU bA==aC kP)kS as=p;else kS br
#define mH =kz
#define mI [pos]
#define mJ [aa]
#define mK kI,[](jX&a,jX&b)ki a.
#define mL ;class
#define mM kf kv
#define mN lA in kH
#define mO an lH an km+kS br);an lH an km+kS as
#define mP ax lH ax km+kS br);ax lH ax km+kS as
#define mQ ah.
#define mR lC!fX()||af>=bh.aA(
#define mS af==
#define mT ;while(
#define mU kj ab&
#define mV ::cs(kG kc{
#define mW kC hz=mS b3 kK cf=an kE ka-(hz?2
#define mX af)
#define mY af,
#define mZ ;kb]
#define na (in,
#define nb {double
#define nc bl ka
#define nd ;else
#define ne swap(
#define nf cU md aa lk kz kR,data
#define ng ;const
#define nh jY unique_ptr<
#define ni jY ostream&out
#define nj kU!
#define nk kd struct
#define nl ai->
#define nm ab::
#define nn lP second
#define no kj kG
#define np data.
#define nq aA()
#define nr )ky jX
#define ns );ae.
#define nt r:bN.am)
#define nu }double
#define nv first)
#define nw (*ai
#define nx ::max()
#define ny (in lk an
#define nz lx const aW>
#define nA kE,kS as,kS
#define nB ;an.lh an km
#define nC ag.
#define nD aD.co
#define nE [&]kV
#define nF .bi()
#define nG operator()kj ba&
#define nH bn(aH&ae kH lL end
#define nI kQ aa la aa<kM mr aa
#define nJ false)
#define nK kV af
#define nL lL second
#define nM kE ka
#define nN 1e-9)
#define nO .emplace_back(
#define nP kJ else
#define nQ {kn ki
#define nR <<jY setprecision(17)
#define nS d6 ka&&!aU mr
#define nT ;lc loc=a4 mJ;kS af=mG=
#define nU dv mH az();kC cC=dv&&dv
#define nV bA>=k+2*n&&bA<k+2*n+
#define nW assign(
#define nX kz lu
#define nY ma bool>
#define nZ [i]
#define oa mb aS[
#define ob lK aa:
#define oc bJ.ej(kL<
#define od kQ pos la pos<=b4 mr pos
#define oe kc{jX
#define of jY shuffle(
#define og kQ i la i
#define oh lc an=b1;an
#define oi mD{jX n=nl mh ks aa<n?
#define oj km,
#define ok lA ag
#define ol inline double
#define om kW struct
#define on (mY an
#define oo loc mH ap
#define op jY istream&in
#define oq .bf+1e-8
#define os kX;kW mutable
#define ot an.ay na
#define ou bA-k-2*
#define ov bS[aa*d1+af]
#define ow bR.
#define ox ;kr
#define oy count)
#define oz empty.
#define oA md aa
#define oB bw kH aZ
#define oC .as.bG
#define oD kK&in=
#define oE kz az()
#define oF bh.ag(
#define oG hz)bn(ae,
#define oH >an nM-(mS b3?2:0)
#define oI }mn
#define oJ =true
#define oK jY isfinite(
#define oL aM.ah
#define oM .bc()
template<class T,class P>void gK(ma T>&fL,P iy){fL lH jY remove_if(fL oj fL kI,iy),fL kI kJ inline jW gC kV value)ki value?lp<unsigned long long>::digits-__builtin_clzll(kl unsigned long long>(value)):0;}mg{struct hm nb bG=0 kt fx la}lD jO nb bf la}lD ie nb aT la hm as kt df la}lD jg nb aT=0 kt cK la hm as;hm aw kt df la}lD au{ma jO>bl;ma ie>ar;ma jg>aS;kp dg;jZ kv ew;jZ kv ev;mc ef lm nc+2*(mh+aS ka kJ mc gE kV bA kc ki dg kO?bA+1:dg.at(bA kJ mc cM kV i kc ki nc+i;}mc aR kV i kc ki nc+mh+i;}mc dF kV j kc ki nc+2*mh+j;}mc bd kV j kc ki nc+2*mh+aS ka+j;}mC ib kV u kH v kc ki ew.at(u*ef()+v kJ mC cR kV u kH v kc ki ev.at(u*ef()+v lJ hh(kc;};}mg{mB void ch(double x kq char*g7 kx!oK x)||x<0)kD(g7 lJ eO(hm w){ch(w.bG,"negative or nonfinite window start");ch(w.fx,"negative or nonfinite window end" kU w.bG>w.fx)kD("reversed time window" kJ mi au::hh(lC bl kO)kD("instance needs at least one taxi" mx v=ef(nj dg kO kx dg ka!=v)kD("invalid vertex mapping size");nY jr(v,nJ lK id:dg kx id==0||id>v||jr[id-1])kD("vertex IDs must be a permutation of 1..V");jr[id-1]=ll}oI v>lp<jW>nx/v||ew ka!=v*v||ev ka!=v*v)kD("matrix dimensions do not match node count")lK&af:bl)ch(af.bf,"invalid taxi capacity")lK&p:ar){ch(p.aT,"invalid passenger revenue");ch(p.df,"invalid passenger service duration");eO(p.as kJ mp&p:aS){ch(p.aT,"invalid parcel revenue" nj oK p.cK)||p.cK<=0)kD("invalid parcel weight");ch(p.df,"invalid parcel service duration");eO(p.as);eO(p.aw kJ for(double x:ew)ch(x,"invalid travel time")kk double x:ev)ch(x,"invalid travel cost" kJ}mg{struct gM{bool bc lv kt bM la jY string is;}lD cA{ma ma jW>>cI mZ gM eq ok kc;};}mg{gM cA::eq ok lC cI ka!=nC nc lo"route count differs from taxi count"}kK n=nC mh kK m=nC aS ka kK cD=nC nc kK jc=nC ef();ma int>ce(n+m,-1);ma int>ec(n+m,-1)kt bM la constexpr double eps=1e-8 kQ k la k<cD mr k){jW a9=k kt cH=0 kt load=0 kK&an=cI[k]kQ pos la pos<an lw pos kY bA=an mI kh bA<cD||bA>=jc lo"route contains invalid service node"}ox cY=cH+nC ib(a9,bA nj oK cY)lo"nonfinite route time"};bM-=nC cR(a9,bA kU bA<cD+n kY i=bA-cD kh ce nZ!=-1 lo"passenger picked up more than once"}kK&p=nC ar nZ kh cY>p.as.fx+eps lo"passenger pickup is late"}kh pos+1==an ka||an[pos+1]!=nC aR(i)lo"passenger drop-off must immediately follow pickup"};ce nZ=lq k);cH=ld cY,p oC)+p.df;}else mn bA<cD+2*n kY i=bA-cD-n kh ce nZ!=lq k)||ec nZ!=-1 lo"invalid passenger drop-off"};ec nZ=lq k);cH=cY+nC ar nZ.df;}else mn bA<cD+2*n+m kY j=bA-cD-2*n kK id=n+j kh ce[id]!=-1 lo"parcel picked up more than once"}kK&p=nC aS[j]kh cY>p.as.fx+eps lo"parcel pickup is late"};load+=p.cK kh load>nC bl[k].bf+eps lo"parcel capacity exceeded"};ce[id]=lq k);cH=ld cY,p oC)+p.df;}else{jX j=bA-cD-2*n-m kK id=n+j kh ce[id]!=lq k)||ec[id]!=-1 lo"invalid parcel drop-off"}kK&p=nC aS[j]kh cY>p.aw.fx+eps lo"parcel delivery is late"};load-=p.cK;ec[id]=lq k);cH=ld cY,p.aw.bG)+p.df;}a9=bA;}}ms i la i<n+m mr i kx ce nZ!=ec nZ lo"partially served request"}kh ce nZ!=-1)bM+=i<n?nC ar nZ.aT:nC aS[i-n].aT;lz{true,bM,{}};}}mg{au fE(op)lG iH(ni kq au&ag kJ mg{mB template<typename T>void read(op,T&value kq char*jC kx!(in>>value)li(jY string("cannot read ")+jC kJ}au fE(op){jW k=0,n=0,m la read na k,"taxi count" lt n,"passenger count" lt m,"parcel count" kU k==0||k>100000||n>20000||m>20000||n+m>20000 li("invalid or unsupported instance size")ng jW v=k+2*(n+m kU v>50000 li("node count too large");au ag;nC bl le k);nC ar le n);nC aS le m);nC dg le v);nY jr(v,false mx hD=nE bA){lc id=nC dg[bA];read na id,"vertex ID" kU id==0||id>v||jr[id-1]li("vertex IDs must be distinct and within 1..V");jr[id-1]=ll}kQ lZ k mr mX{hD(af lt nC bl kZ.bf,"taxi capacity" kJ ms i la i<n mr i){lc p=nC ar nZ;hD(nC cM(i));hD(nC aR(i)lt p oC,"passenger earliest" lt p.as.fx,"passenger latest" lt p.df,"passenger service" lt p.aT,"passenger revenue" kJ ms j la j<m mr j){lc p=nC aS[j];hD(nC dF(j));hD(nC bd(j)lt p.cK,"parcel weight" lt p oC,"parcel pickup earliest" lt p.as.fx,"parcel pickup latest" lt p.aw.bG,"parcel delivery earliest" lt p.aw.fx,"parcel delivery latest" lt p.df,"parcel service" lt p.aT,"parcel revenue" kJ kp g4(v)kQ bA la bA<v mr bA)g4[nC gE(bA)-1]=bA mo*iV:{&nC ew,&nC ev}){iV->resize(v*v)kQ u la u<v mr u)ms w la w<v mr w)read na(*iV)[g4[u]*v+g4[w]],"matrix entry" kJ jY string jB kh in>>jB li("unexpected data after instance matrices");nC hh()ks ag;}}mg{cA iM(op kq au&ag)lG d7(ni kq cA&b5 kq au&ag kJ mg{void d7(ni kq cA&b5 kq au&ag){nC hh(mx dc=b5.eq(ag nj dc.bc li("invalid solution: "+dc.is);out nR<<dc.bM<<'\n' lK&an:b5.cI){out<<an ka lK bA:an)out<<' '<<nC gE(bA);out<<'\n';oI!out li("failed to write solution" kJ nk bQ{jW first=0,hK=0 kt duration=0 kt ha=-kg)kt bv=kg)kt cR=0 kt bz=0,dD=0,dC la bool bc=ll};bQ b2 ok kH bA);bQ jp ok kq bQ&jV kq bQ&jS)mL aj lf aj()=default;aj ok kH af lX an)lG fj ok kH af lX an)lG clear()lG dM kj aj&bR)mZ bQ c5 ok kH lL hK kc mZ bool ay ok kH aV kc;mC cR lm h6;}kb]kB bi lm fM;}lI kp&ax lm dy;}lI jZ kv&cY lm el;}lI jZ kv&cH lm db;}lI jZ kv&load lm hL;}kW kp dy;ma bQ>ct;ma ma bQ>>dr;jZ kv el,db,hL kt h6 la kB fM la}mL aK lf aK ok kH lr bk)lG cv kV lL end)lG e3 kj bQ&dS)lG eP kV bA)mZ mM aX(kc;mC gG(kc;kW const au&ai;jW hg ng aj&eY;kf<bQ>cu;};kd mB jY atomic<kB>fB{1}kt df mN bA kY mm kh bA<k||bA>=mb ef()kw("invalid service node" kU bA<k+2*n ke mb ar[(bA-k)%n].df ks oa(ou n)%m].df;nu h7 mN bA kY mm kh bA<k+2*n ke 0 kK j=(ou n)%m ks bA<k+2*n+m?oa j].cK:-oa j].cK;}}bQ b2 mN bA){bQ s;s.first=s.hK=bA;s lT=df na bA mx mm ng hm*gY=nullptr kh bA<k+n)gY=&mb ar[bA-k].as nd mn nV m)gY=&oa ou n].as nd mn bA>=k+2*n+m)gY=&oa ou n-m].aw kh gY){s.ha=gY->bG+s lT;s.bv=gY->fx;}s.bz=h7 na bA);s.dD=lB 0.0,s.bz);s.dC=ld 0.0,s.bz)ks s;}bQ jp lA in kq bQ&a kq bQ&b){kr fJ=mb ib(a.hK,b.nv;bQ s;s.first=a.first;s.hK=b.hK;s lT=a lT+fJ+b lT;s.ha=ld a.ha+fJ+b lT,b.ha);s.bv=lB a.bv,b.bv-fJ-a lT);s.bc=a.bc&&b.bc&&a.ha+fJ<=b.bv;s.cR=a.cR+lg a.hK,b.nv+b.cR;s.bz=a.bz+b.bz;s.dD=lB a.dD,a.bz+b.dD);s.dC=ld a.dC,a.bz+b.dC)ks s;}aj::aj mN af lX an){fj ny lJ aj::clear(){dy ln;ct ln;el ln;db ln;hL ln kk lc fz:dr)fz ln;h6 la fM la mi aj::dM kj aj&bR kx this==&bR ke;dy=ow dy;ct=ow ct;el=ow el;db=ow db;hL=ow hL kh dr ka<ow dr ka)dr le ow dr ka)og<ow dr lw i)dr nZ=ow dr nZ kQ i=ow dr ka;i<dr lw i)dr nZ ln;h6=ow h6;fM=ow fM;mi aj::fj mN af lX an kx&an!=&dy)dy=an;ct ln;el ln;db ln;hL ln kk lc fz:dr)fz ln;h6 la fM=fB.fetch_add(1,jY memory_order_relaxed mx size=an ka;ct lU size);el lU size);db lU size);hL lU size);jW a9=af kt cH=0,load=0 lK bA:an){ct ko b2 na bA))ox cY=cH+mb ib(a9,bA);cH=ld cY+ct.back()lT,ct.back().ha);load+=ct.back().bz;h6+=lg a9,bA);el ko cY);db ko cH);hL ko load);a9=bA;}jW fc=0 kQ jo=1;jo<size;jo*=2,++fc kx fc==dr ka)dr nO);lc fz=dr[fc];fz le size)ng jW jk=jo*2 kQ he la he<size;he+=jk kY mid=lB he+jo,size mx end=lB he+jk,size kU mid>he){fz[mid-1]=ct[mid-1]mo i=mid-1;i>he;--i)fz[i-1]=jp na ct[i-1],fz nZ kJ mn mid<end){fz[mid]=ct[mid]mo i=mid+1;i<end mr i)fz nZ=jp na fz[i-1],ct nZ kJ oI jo>size/2)lV}}bQ aj::c5 mN lL hK lC first>hK||hK>=dy ka)throw jY out_of_range("invalid route range" kU first==hK ke ct[first]kK fz=gC(first^hK)-1 ks jp na dr.at(fz)[first],dr.at(fz)[hK]kJ bool aj::ay mN pos lC pos>dy ka kN kh pos==0 ke true kK eJ=dy[pos-1],k=mb nc ks eJ<k||eJ>=k+mb mh;}aK::aK mN lr bk):ai(in),hg(mX,eY(bk){mi aK::cv kV lL end kx first==end ke;e3(eY.c5(ai,lP end-1)lJ aK::e3 kj bQ&dS){cu=cu?jp(ai,*cu,dS):dS;mi aK::eP kV bA){e3(b2(ai,bA)kJ mM aK::aX(lC!cu ke 0.0 kK&s=*cu ox cY=ai.ib(hg,s.first nj s.bc||cY>s.bv||s.dD<-1e-8||s.dC>ai.bl[hg]oq ku ks ai.cR(hg,s.nv+s.cR;nu aK::gG(lC!cu ke 0.0 ks ai.cR(hg,cu->nv+cu->cR;}kd class aW mL a8 lD eA{int af=-1;jW as=0,br la}mL ab lf ab ok,cA b5,lx a8>aN={})lG dM mU bR)lG ne ab&bR)noexcept mZ cA b5(kc;lI aj&bk nK kc;lI kp&bm lm av;}kb]bool b9 nK kc ki af<aE ka&&!aE kZ kE.empty(kJ mc aA lm nl nc;}lI eA&ap mD ki a4.at(aa kJ mc ad lm a4 ka;}mc al lm nl mh;}mc eN(kc;mC bM lm cL;}kb]a8&aN lm*by;}lI lx a8>&ee lm by;}mC aT mD;lI au&ag lm*ai;}mc aC mD;lI nz&az lm bK;mi dA(nz dv){bK=lY dv kJ kb]mM aO mD mZ mM e0 kV aa kH af kH oB kc lR ga kV aa)lR dP nK lX am)lR insert kV aa kH af kH oB)lR cx kV aa kH oB)lR eL kV a0 kH aF kH oB)lR fI kV nL)lR cZ kV bU kH e7 kH bV kH e8)lG ho(kc;kW const au*ai;lx a8>by;ma aj>aE;kp av;ma eA>a4 lD hl{kB bu la mM a1;}ly ma hl>cp;nz bK kt cL la kp b1;aj bb lG c3 nK lX an)lG cr kV mY aj&c4 lQ a1)lG da kV bU,aj gd kH bV,aj ge lQ a1);mc cd mD;lb a8 lf struct i5 nb*as kt*aw kt*iI kt*jt;};explicit a8 kV am):aQ(am),fW(gF(am)),hf(4*fW){}mc ad lm aQ;}mc gu lm fW-2;}mc jQ lm hf ka*sizeof(double kJ kb]i5 fP kV hR kx hR>gu())throw jY out_of_range("insertion route exceeds fixed scratch capacity");auto*data=hf.data()ks{data,data+fW,data+2*fW,data+3*fW};}kW static jW gF kV am){constexpr auto iv=lp<jW>nx/sizeof(double)/4 kh am>(iv-2)/2)throw jY length_error("insertion scratch size overflow")ks 2*am+2;}jW aQ,fW;jZ kv hf;lb aH lf aH mN lr an):in_(in),hg(mX,i1(an){mi c5 kV lL end kx first<end)ij(i1.c5(in_,lP end-1)lJ bA kV id){ij(b2(in_,id)lJ dS kj bQ&gS){ij(gS kJ kb]bool bc(lC!cu ke true kK&s=*cu ks s.bc&&in_.ib(hg,s.nv<=s.bv&&s.dD>=-1e-8&&s.dC<=in_.bl[hg]oq;}kW void ij kj bQ&gS kx!cu){cu=gS ks;}lc s=*cu ox fJ=in_.ib(s.hK,gS.nv ox fS=s.bz ox f8=s lT ox ht=s.ha;s.bc=s.bc&&gS.bc&&ht+fJ<=gS.bv;s.bv=lB s.bv,gS.bv-fJ-f8);s lT=f8+fJ+gS lT;s.ha=ld ht+fJ+gS lT,gS.ha);s.dD=lB s.dD,fS+gS.dD);s.dC=ld s.dC,fS+gS.dC);s.bz=fS+gS.bz;s.hK=gS.hK;}const au&in_;jW hg ng aj&i1;kf<bQ>cu;};nk dJ{const kp&ax;jW gI=0,gc la bool aP lv;mc size lm ax ka-(aP?2:0 kJ mc bA kV aV lC!aP ke ax[aV]kh aV<gI ke ax[aV]kh aV<gc-1 ke ax[aV+1]ks ax[aV+2];}};ol bE mN af lX ax kH c2 kH b0 kY as=ax[c2],aw=ax[b0]kK dB=c2?ax[c2-1]:af kh b0==c2+1)nb hA=lg dB,as)+lg as,aw kU b0+1<ax ka kY jh=ax[b0+1];hA+=lg aw,jh)-lg dB,jh lS hA;}jX f0=ax[c2+1]kK eC=ax[b0-1]kt hA=lg dB,as)+lg as,f0)-lg dB,f0)+lg eC,aw kU b0+1<ax ka kY eW=ax[b0+1];hA+=lg aw,eW)-lg eC,eW lS hA;}ol bT mN af kq dJ&an kH at kH bA kY a9=at?an.bA(at-1):af kt eb=lg a9,bA kU at<an ka kY gS=an.bA(at);eb+=lg bA,gS)-lg a9,gS lS eb;}ol bF mN af kq dJ&an kH at kH as kH aw kY a9=at?an.bA(at-1):af kt eb=lg a9,as)+lg as,aw kU at<an ka kY gS=an.bA(at);eb+=lg aw,gS)-lg a9,gS lS eb;}ol cV mN af kq dJ&an kH c2 kH b0 kH as kH aw kx c2==b0 ke bF ny,c2,as,aw)ks bT ny,c2,as)+bT ny,b0,aw kJ kd nm ab lA in,cA b5,lx a8>aN):ai(&in),by(aN?lY aN):jY make_shared<a8>(mb mh+mb aS ka)),aE(mb nc),a4(mb mh+mb aS ka),cp(a4 ka kx by->ad()!=a4 ka kw("insertion scratch request count mismatch");av lU mb nc mx dc=b5.eq(in nj dc.bc kw("invalid search state: "+dc.is);cL=dc.bM kQ k la k<b5.cI lw k)mn!b5.cI[k]kO)c3(k,b5.cI[k]lJ nm dM mU bR kx this==&bR ke kh ai!=ow ai kw("cannot copy states of different instances")kQ lZ aE lw mX mn aE kZ nF!=ow aE kZ nF)aE kZ.dM(ow aE kZ);av lU nq);av=ow av;a4=ow a4;cp=ow cp;bK=ow bK;cL=ow cL;mi nm ne ab&bR)noexcept{using jY swap;ne ai,ow ai);by.ne ow by);aE.ne ow aE);av.ne ow av);a4.ne ow a4);cp.ne ow cp);bK.ne ow bK);ne cL,ow cL);b1.ne ow b1);ne bb,ow bb kJ cA nm b5(kc{cA aD;aD.cI le nl nc)lK af:av)aD.cI kZ=kR kE ks aD;}const aj&nm bk nK kc{static const aj empty ks af<aE ka?aE kZ:empty;}jW nm eN lm jY count_if(a4 oj a4 kI,[](jX&ap)ki ap.af>=0;}kJ double nm aT oi nl ar.at kP.aT:nl aS.at(aa-n).aT;}jW nm aC oi nl cM kP:nl dF(aa-n kJ jW nm cd oi nl aR kP:nl bd(aa-n lJ nm c3 nK lX an kY&in=*ai kK n=mb mh,m=mb aS ka,cD=mb nc lK bA:kR kE kY aa=bA<cD+2*n?(bA-cD)%n:n+(bA-cD-2*n)%m;a4 mJ={};}lc iO=aE.at(mX;kC eT=b9(af kU an kO kx eT){iO ln;av lH mv,mX kJ}else{iO.fj ny nj eT)av.lh mv,mX,af kJ ms p la p<an lw p kY bA=an[p];kC aJ=bA<cD+2*n kK aa=aJ?(bA-cD)%n:n+(bA-cD-2*n)%m nT p;}}mM nm aO mD{jX loc=ap kP kh kS af<0 ku kK af=mt kS af mx&eH=kR;lc a7=cp mJ kh mz bu==eH nF ke mz a1 kK size=eH nM;aH ae nw lk eH);me 0,kS as);me kS as+1,kS br);me kS br+1,size);mz bu=eH nF;mz a1=ae oM?mM(bE nw lk eH nA br)-aT kP):jY nullopt ks mz a1;}mM nm e0 kV aa kH af kH a kH b lC aa>=ad()||lF||af>=nq ku kK&dR=kR kK size=dR nM;kC aJ=aa<nl mh kh a>b||b>size||(aJ&&a!=b)||!dR.ay nw,a)||!dR.ay nw,b)ku kh!aJ&&nl aS[aa-nl mh].cK>nl bl kZ oq ku;aH ae nw lk dR);me 0,a ns bA(aC kP nj aJ)me a,b ns bA(cd kP);me b,size nj ae oM ku ng dJ iG{dR kE}kK cR=aJ?bF nw lk iG,a,aC kP,cd kP):cV nw lk iG,a,b,aC kP,cd kP)ks aT kP-cR;mk nm ga kV aa kY a1=aO kP kh!a1 kN kK loc=ap kP kK af=mt kS mX;oh=kR kE;mO);cL+=*a1;c3 on)ks ll mk nm dP nK lX am kx af>=nq||am kO kN kK&old=kR;nY fv(old nM,nJ kt f5=0 ob am kx aa>=ad()kN kK loc=ap kP kh kS af!=lq mX||fv[kS as]kN;fv[kS as]=fv[kS br]=ll f5+=aT(aa kJ aK ae nw lk old);oh ln kQ first la first<old nM;kx fv[first]){++first;ky}auto end=first+1 mT end<old nM&&!fv[end])++end;ae.cv(lP end);an.lh an kI,old kE km+lP old kE km+end);first=end;}jX h0=ae.aX(nj h0 kN;cL+=old.cR()-*h0-f5;c3 on)ks ll mk nm insert kV aa kH af kH a kH b kY a1=e0(aa lk a,b nj a1 kN;oh=kR kE nB+b,cd kP)nB+a,aC kP);cL+=*a1;c3 on)ks ll mk nm cx kV aa kH a kH b kx aa>=ad()||ap kP.af<0 kN kK af=mt ap kP.af mx g8=kR.cR();oh=kR kE kK loc=ap kP;mO);kC aJ=aa<al(kU a>b||b>an ka||(aJ&&a!=b)kN kK ay=nE aV kx!aV ke true kK eJ=an[aV-1],k=nl nc ks eJ<k||eJ>=k+al(kJ kh!ay(a)||!ay(b)kN kh!aJ&&nl aS[aa-al()].cK>nl bl kZ oq kN nB+b,cd kP)nB+a,aC kP);bb.fj nw lk an);aK ae nw lk bb ns cv(0,an ka mx cR=ae.aX(nj cR kN;cr(mY bb,g8-*cR)ks ll mk nm eL kV a0 kH aF kH a kH b kx a0>=ad()||aF>=ad()||ap(a0).af<0||ap(aF).af>=0 kN kK loc=ap(a0 mx af=mt kS af mx g8=kR.cR();oh=kR kE;mO);kC aJ=aF<al(kU a>b||b>an ka||(aJ&&a!=b)kN kK ay=nE aV)ki!aV||an[aV-1]>=nq+al(kJ kh!ay(a)||!ay(b)kN nB+b,cd(aF))nB+a,aC(aF));bb.fj nw lk an);aK ae nw lk bb ns cv(0,an ka mx cR=ae.aX(nj cR kN ox a1=aT(aF)-aT(a0)+g8-*cR kh a1<=1e-9 kN;cr(mY bb,a1)ks ll mi nm cr kV mY aj&c4 lQ a1 kY k=nq,n=al(),m=nl aS ka kK gU=nE bA)ki bA<k+2*n?(bA-k)%n:n+(ou n)%m;};lc iO=aE.at(mX lK bA:iO kE)a4[gU(bA)]={};kC eT=!iO kE kO;kC gN=!c4 kE kO kh eT!=gN kY hV=mv,af kU gN)av.lh hV,mX nd av lH hV kJ jY ne iO,c4 nj gN)iO ln kQ p la p<iO kE lw p kY bA=iO kE[p],aa=gU(bA)nT p;}cL+=a1;mk nm fI kV nL kx first>=ad()||second>=ad()kN kh second<nv jY ne nn mx a=ap(nv,b=ap(second kU a.af<0||b.af<0||a.mS b.af kN kK gQ=nE a0 kH aF kY loc=ap(a0);auto ax=bk(kS mX kE;mP mx aZ=aF<al()?kS as:kS br-1;ax.lh ax km+aZ,cd(aF));ax.lh ax km+kS as,aC(aF))ks ax;}kK fe=gQ(nn),ff=gQ(second,nv;aj gd nw,a.mY fe),ge nw,b.mY ff);aK eD nw,a.mY gd),eE nw,b.mY ge);eD.cv(0,fe ka);eE.cv(0,ff ka mx f3=eD.aX(),f4=eE.aX(nj f3||!f4 kN ox a1=bk(a.mX.cR()+bk(b.mX.cR()-*f3-*f4 kh a1<=1e-9 kN;da(a.mY lY gd),b.mY lY ge),a1)ks ll mk nm cZ kV bU kH e7 kH bV kH e8 kx bU==bV||bU>=nq||bV>=nq kN kK&h9=bk(bU mx&ia=bk(bV kU e7>h9 nM||e8>ia nM kN lK&loc:a4 kx(kS mS lq bU)&&kS as<e7&&kS br>=e7)||(kS mS lq bV)&&kS as<e8&&kS br>=e8)kN;}kp fe(h9 kE oj h9 kE km+e7);fe.lh fe kI,ia kE km+e8,ia kE kI);kp ff(ia kE oj ia kE km+e8);ff.lh ff kI,h9 kE km+e7,h9 kE kI);aj gd nw,bU,fe),ge nw,bV,ff);aK eD nw,bU,gd),eE nw,bV,ge);eD.cv(0,fe ka);eE.cv(0,ff ka mx f3=eD.aX(),f4=eE.aX(nj f3||!f4 kN ox a1=h9.cR()+ia.cR()-*f3-*f4 kh a1<=1e-9 kN;da(bU,lY gd),bV,lY ge),a1)ks ll mi nm da kV bU,aj gd kH bV,aj ge lQ a1){for(lc loc:a4)mn kS mS lq bU)||kS mS lq bV))loc={}kK assign=nE mY aj c4){auto hV=mv,mX;kC gN=hV!=av kI&&*hV==af kh c4 kE kO kx gN)av lH hV);aE kZ.clear(nP{mn!gN)av.lh hV,mX;aE kZ=lY c4 kJ jX k=nl nc,n=al(),m=nl aS ka kK&ax=kR kE kQ p la p<ax lw p kY bA=ax[p]kK aa=bA<k+2*n?(bA-k)%n:n+(ou n)%m nT p;}};nW bU,lY gd));nW bV,lY ge));cL+=a1;mi nm ho(oe aD=b5().eq nw nj aD.bc||jY abs(aD.bM-cL)>1e-6*ld 1.0,jY abs(aD.bM))kw("search-state objective or feasibility mismatch: "+aD.is kJ nk hi{jW eF=0,fV=0,fF la}mL e2 lf e2 kV d3 kH am,bool gg);hi i3(kA rng kH eh)lG i4(hi ah lQ eM);lI jZ kv&jH lm fk;}lI kp&jA lm gP;}om Arm nb cK=1,sum=0,gW la jW count la}lR hB;ma Arm>g2;ma ma Arm>>h4;kp gw;jZ kv fk;kp gP;jW iq kj ma Arm>&iK,kA rng kc;};kd e2::e2 kV d3 kH am,bool gg):hB(gg),g2(d3),h4(d3),fk(d3,0),gP(d3,0 kx d3==0)kD("BALANCE needs an operator couple")kQ p=1;p<=am;p*=2){gw ko p kU p>am/2)lV oI gw kO)gw ko 1)kk lc iK:h4)iK le gw ka kJ jW e2::iq kj ma Arm>&iK,kA rng lC!hB)nb jv=0 lK&arm:iK)jv+=arm.cK mE jR(0,jv)kt c8=jR(rng)og<iK lw i){c8-=iK nZ.cK kh c8<=0 ke i;lz iK ka-1;}constexpr double mu0=0,hJ=0.01,ji=1,jz=100;jW ah=0 kt co=-kg)og<iK lw i kY&a=iK nZ ox count=kl double>(a.oy ox iu=hJ+count ox jE=(hJ*mu0+a.sum)/iu ox jy=ji+count/2 ox jP=jz+0.5*(a.gW+hJ*mu0*mu0-iu*jE*jE);jY gamma_distribution kv jD(jy,1.0/ld jP,1e-12))ox ix=jD(rng);jY normal_distribution kv jq(jE,1.0/jY sqrt(ld iu*ix,1e-12)))ox i2=jq(rng kU i2>co){co=i2;ah=i;}lz ah;}hi e2::i3(kA rng kH eh){hi ah;mQ eF=iq(g2,rng);mQ fV=iq(h4[mQ eF],rng);mQ fF=lB eh,gw[mQ fV])ks ah;mi e2::i4(hi ah lQ eM){eM=ld 0.0,eM);auto gX=[eM](Arm&a){++a.count;a.sum+=eM;a.gW+=eM*eM;a.cK=0.9*a.cK+0.1*(0.01+eM kJ;gX(g2[mQ eF]);gX(h4[mQ eF][mQ fV]);fk[mQ eF]+=eM mr gP[mQ eF];}nk ic{kB jL=1 kt ei=59.0 kt cz=200.0 kt i7=jY exp(jY log(1e-4)/60000.0)lR ho lv lR bJ=ll bool e6=ll bool e4=ll bool g9=ll bool hc=ll bool fh=ll bool eg=ll jW gs=1 lR gg lv;jY string iL;};nk ig{cA b5 kt bM la jW dG=0 kt iB la bool dX lv;};ig js ok kq ic&b7);kd class bL lf virtual~bL(lO kp mq kc la lb fl kT bL nQ"random";mj eU kT bL nQ"related";mj fZ kT bL nQ"worst";mj fY kT bL nQ"route";mj dW kT bL nQ"opportunity";mj iF lf virtual~iF(lO bool cs(kG kc la lb gh lf void ej(nh iF>move){fA ko lY move)kJ bool gV(kG kc;kW ma nh iF>>fA;lb cG lf virtual~cG(lO void d9(ab&lX aP,kA kc la};kd using dV=jY chrono::steady_clock::time_point lD ak{jW aa=0,af=0,as=0,aw=0 kt a1 la}lD aB{lM co kt bO=-kg kJ lD ex{jW af=0 kt a1 la}lD b6{bool aJ;bQ as,aw kt aT,cK;}mL aW lf explicit aW ok)lR go kV aa,dV bP=dV nx kc mZ bool jw mD ki fp.at(aa kJ lI ma ex>&dt mD ki eQ.at(aa kJ lI b6&bo mD ki cb.at(aa kJ mc hy lm cb ka*sizeof(b6 kJ kW const au*ai ly ma ma ex>>eQ ly kp gT ly nY fp;ma b6>cb;};nz c6 mU fa,dV bP);aB ed lE);lM di lE);lM cg lE kH lr an lQ f7=-kg))mL cW lf lM co lE);om gZ{kB bu=0 kt bq=-kg);lM co;};ma ma gZ>>bS;}mL a3 lf void hO mU lX am)lG cT mU kH af lX am);lI lM&co mD ki bS.at(aQ*d1+aa).ah;}mc iP lm bS ka*sizeof(gZ kJ om gZ{kB bi=lp<kB>nx;lM ah;};nz iT;jW d1=0,aQ la ma gZ>bS lG et mU kH aa kH mX lG eu kV aa kJ mL aI lf kb]static bool hH no);explicit aI no);aI no lX am)mZ aB ci mD lG cT kV mX;kW const ab&bh;using gj=jY pair<jW,aB>;ma ma gj>>cq;kp aQ;lb hk lf template<class jN>lM get kV aa kH af kH aA,kB aG,kB cJ lQ ck,bool de,jN&&jT kx bS ka<=aa)bS le aa+1);lc row=bS mJ kh row ka<aA)row le aA);lc a7=row kZ;kC gt=mz fX&&mz cJ==cJ&&mz de==de;kC jG=gt&&(de?mz aG==aG:(mz mQ has_value()||ck<=mz ck)nj jG){mz ah=jT();mz aG=aG;mz cJ=cJ;mz ck=ck;mz de=de;mz fX=ll oI mz ah&&!de){auto hX=*mz ah;hX.a1+=ck-mz ck ks hX;lz mz ah;}om gZ{lM ah;kB aG=0,cJ=0 kt ck la bool de lv lR fX lv;};ma ma gZ>>bS;};nk dx{ak ah;jW bg;}mL eV lf void add(lM ah kH bg kx ah&&ah lW>nN fA ko{*ah,bg}kJ kb]kf<dx>co(lC fA kO ku ks*jY max_element(fA oj fA mK mQ a1<b.mQ a1;}lJ eZ kV nL){gK(fA,[&](jX&move)ki move.bg==first||move.bg==second||move.mQ mS first||move.mQ mS second;}lJ clear(){fA.clear(kJ kW ma dx>fA;lb ao lf ao lE)mZ bool fX lm gB;}kb]bool dw lm bH.has_value(kJ mC cy lm bB;}mc b4 nK kc mZ mM a1 nK kH oB kc mZ mM a1 ml oB kc mZ mM bj ml oB kc mZ kf<aH>er ml bw kc mZ mM du ml oB kq aH&dZ kc;kW const ab&bh;jW dl,b3,dH,cS lR gB lv;mM bH kt bB la bQ iw,hp;mc ca kV aV kc mZ bool cj kj aj&an kH bt kc lG nH kc;}mL d2 kT iF nQ"parcel_relocate";mk cs(ab&os hk eY mZ lM cw mU kq ao&kH aa kH lr,kB aG kc;lb aq lf aq lE)mZ bool fX lm gB;}kb]bool dw lm bH.has_value(kJ mC cy lm bB;}mc b4 nK kc mZ mM a1 nK kH bt kc mZ mM a1 ml bt kc mZ mM bj ml bt kc;kW const ab&bh;jW dl,b3,cQ lR gB lv;mM bH kt bB la mc ca kV aV kc lG nH kc;}mL c9 kT iF nQ"passenger_relocate";mk cs(ab&os hk eY mZ lM cw mU kq aq&kH aa kH lr,kB aG kc;lb gL kT iF nQ"add-request";mk cs(kG os a3 eY ly cW dN ly kp aQ;lb fm kT iF nQ"remove-request";mk cs(kG kX;lb ds kT iF nQ"clean-cut-2opt-star";mk cs(kG kX;mc jA lm gP;}mc iJ lm eX;}mC je lm gD;}kW mutable jW gP=0,eX la mutable double gD la};nk aL{jW a0;ak bp kt a1;}mL a5 kT iF lf explicit a5(bool hT oJ):fK(hT){}jY string_view g7(kX ki"replace-request";mk cs(ab&kX mZ kf<aL>dL mU kc;kW bool fK mZ kf<aL>dm mU kc lD gZ{bool fX lv kt bq la kf<aL>ah;}lD Row{kB bu la ma gZ>hI;kB eK la aj es kt aO la}ly ma Row>iN lD d8{kB b8 la kf<aL>ah;}lD ba{aL ah;kB bu,b8;}lD cX{bool nG kq ba&kc;};using hE=jY priority_queue<ba,ma ba>,cX>ly const au*fH=nullptr ly ma kB>cc ly nY dU ly ma ma d8>>bI ly hE dp lG cP mU kH af lX a0 lX aF kc lG dT mU kc lG ft(kc lG fb nK kH aF kc lG fo(kc;};nk aY{jW nn kt a1;};mM dd mU kH nL)mL cN kT iF lf explicit cN(bool hT oJ):fK(hT){}jY string_view g7(kX ki"swap-request";mk cs(ab&kX mZ kf<aY>dL mU kc;om d8{kB b8 la kf<aY>ah;}lD ba{aY ah;jW bU,bV;kB iD,iE,b8;}lD cX{bool nG kq ba&kc;};using hE=jY priority_queue<ba,ma ba>,cX>lR fK ly const au*ai=nullptr ly ma kB>cm ly ma ma d8>>bI ly hE dp lG dT mU kc lG fb kV bU kH bV kc;lb ey kT cG nQ"greedy";mi d9(ab&lX,kA kX;lb ez kT cG nQ"profit";mi d9(ab&lX,kA os a3 dO ly cW dN;lb be lf explicit be ok kH iv=200)lG hb no)lR cs(kG,dV bP);jW bX lm em;}jW ax lm dy;mk ju lm aU;mi write(ni kc;om hW{kp am,an kt bD la}ng au&ai;jW iU,em=0,dy la bool aU lv;ma jY map<kp,hW>>gv;ma kB>cm lG add mU kH mY bool fu kJ;kd ig js ok kq ic&b7){using hj=jY chrono::steady_clock kh!b7.gs)kD("ls interval must be positive" mx he=hj::now(mx bP=he+lj b7.ei));jY mt19937_64 rng(b7.jL)ox gJ=b7.eg?1.0:0.0 kK gz=bP-lj lB gJ,b7.ei*0.05)));ma nh bL>>ep;ep ko kL<fl>());ep ko kL<fZ>());ep ko kL<eU>()kU b7.hc)ep ko kL<fY>()kU b7.fh)ep ko kL<dW>());ma nh cG>>gx;gx ko kL<ey>()kU b7.g9)gx ko kL<ez>())lD jf{jW i8,i0;};ma jf>d3 kQ d la d<ep lw d)ms r la r<gx lw r)d3 ko{d,r});e2 il(d3 ka,nC mh+nC aS ka,b7.gg);gh bJ;oc fm>());oc gL>());oc c9>());oc d2>());ds*ir=nullptr kh b7.e6){auto move=kL<ds>();ir=move.get();bJ.ej(lY move)kJ oc cN>());oc a5>());cA empty;oz cI le nC nc);ab bZ(ag,lY empty)mx gH=lB bP,hj::now()+lj lB 1.0,b7.ei*0.05))));bZ.dA(c6(bZ,gH));kp c0(bZ.ad());jY iota(c0 oj c0 kI,0);of c0 oj c0 kI,rng)ob c0 kx hj::now()>=bP)break kK ah=di(bZ,aa kU ah&&ah lW>nN bZ.lh mA aw kJ mn b7.ho)bZ.ho();ab co=bZ;ab ae=bZ;nh be>dI kh b7.eg){dI=kL<be>(ag);dI->hb(bZ kJ double cz=b7.cz ox cO=b7.cz*1e-4;jW dG la kp cB;cB lU bZ.ad())mE hC(0.0,1.0)mT cz>cO kY gr=hj::now(kU gr>=gz)break kK gA=il.i3(rng,bZ.eN()mx eF=d3[gA.eF];ae.dM(bZ mx aP=ep[eF.i8]->d9(ae,gA.fF,rng kU b7.e4){cB ln kQ r la r<ae.ad()mr r)mn ae.lu<0)cB ko r);of cB oj cB kI,rng);gx[eF.i0]->d9(ae,cB,rng nP gx[eF.i0]->d9(ae,aP,rng);kC iz=ae.bM()>bZ.bM(kU b7.bJ&&(iz||dG%b7.gs==0))bJ.gV(ae kU b7.ho)ae.ho(kU dI)dI->hb(ae)ox eb=ae.bM()-bZ.bM()ox eM=ld 0.0,eb/ld 1.0,jY abs(bZ.bM())))lR ii=eb>=0 kh!ii){ii=hC(rng)<jY exp(eb/cz kJ mn ii){bZ.ne ae kU bZ.bM()>co.bM()+nN{co.dM(bZ kJ}il.i4(gA,eM)mr dG;cz=ld cO,cz*b7.i7 kJ mn dI){dI->cs(co,bP kJ mn b7.ho)co.ho();ig aD;aD.b5=co.b5();aD.bM=co.bM();aD.dG=dG;aD.iB=jY chrono::duration kv(hj::now()-he).count();aD.dX=cz<=cO ks aD;}kd kp fl::mq kc{kp am(kM);jY iota(am oj am kI,0);of am oj am kI,rng);kp aP;aP lU oy ob am kx aP ka>=oy break kh kz ga kP)aP ko aa lS aP;}kd kp fZ::d9(kG kH count,kA kc{kp aP;aP lU oy mT aP ka<oy{jW ah=kM kt bC=-kg)mw kY a1 mH aO(r kU a1&&*a1>bC){ah=r;bC=*a1;}oI ah==kM||!kz ga(ah))lV aP ko ah lS aP;}kd kp eU::mq kc{kp eh mw)mn nX>=0)eh ko r kU eh kO ke{}kK f1=eh[rng()%eh ka]oD lN kK gl mH aC(f1 mx bG=nE r)ki r<mb mh?mb ar[r]oC:oa r-mb mh]oC;}ox hF=bG(f1);kp aP;aP lU oy mT aP ka<oy{jW ah=kM kt fn=kg)mw kx nX<0||!kz aO(r)nr bA mH aC(r)ox gf=mb ib(gl,bA)+mb ib(bA,gl)+jY abs(hF-bG(r)kU gf<fn){fn=gf;ah=r;}oI ah==kM||!kz ga(ah))lV aP ko ah lS aP;}kd kp fY::mq kc{auto bl mH bm();of bl oj bl kI,rng);kp aP lK af:bl kx aP ka>=oy lV kp am mw)mn nX==lq mX)am ko r mx hx=count-aP ka kh am ka<=hx kx kz dP(mY am))aP.lh aP kI,am oj am kI nP{for(int ik la ik<4 mr ik){of am oj am kI,rng)ng kp iC(am oj am km+hx kU kz dP(mY iC)){aP.lh aP kI,iC oj iC kI);lV}}}lz aP;}kd kp dW::mq lC!count ke{}oD lN kK&dv mH az();kp hS,eh;jZ kv hU mw kx nX>=0){eh ko r);ky nu fg mH aT(r kU dv){dv->go(r mx&dt=dv->dt(r);fg=dt kO?0:ld 0.0,dt.front().a1 kJ mn fg>nN{hS ko r);hU ko fg kJ oI eh kO ke{};jW f1 kh hS kO)f1=eh[rng()%eh ka]nd f1=hS[jY discrete_distribution<jW>(hU oj hU kI)(rng)]kK br=nE r)ki r<mu?mb aR(r):mb bd(r-mu kJ kK bG=nE r)ki r<mu?mb ar[r]oC:oa r-mu]oC;}kK h1 mH aC(f1),iS=br(f1);ma jY pair<double kH>>fD mo r:eh kY as mH aC(r),jK=br(r)ox gf=0.5*(mb ib(h1,as)+mb ib(as,h1)+mb ib(iS,jK)+mb ib(jK,iS))+jY abs(bG(f1)-bG(r));fD nO gf,r kJ jY sort(fD oj fD kI)mE jb(0,1);kp aP mT!fD kO&&aP ka<count kY aV=lB fD ka-1,mt jY pow(jb(rng),3)*fD ka)mx r=fD[aV].second;fD lH fD km+aV kU kz ga(r))aP ko r lS aP;}kd mB b6 eG mN aa kY n=mb mh;kC aJ=aa<n kK as=aJ?mb cM kP:mb dF(aa-n mx aw=aJ?mb aR kP:mb bd(aa-n)ks{aJ,b2 na as),b2 na aw),aJ?mb ar mJ.aT:oa aa-n].aT,aJ?0:oa aa-n].cK};}}aW::aW ok):ai(&ag),eQ(nC mh+nC aS ka),gT(eQ ka,0),fp(eQ ka,nJ,cb(eQ ka){ms aa la aa<cb lw aa)cb mJ=eG(ag,aa kJ bool aW::go kV aa,dV bP lC fp.at kP ke true oD*ai kK n=mb mh kK as=aa<n?mb cM kP:mb dF(aa-n mx aw=aa<n?mb aR kP:mb bd(aa-n mx aT=aa<n?mb ar mJ.aT:oa aa-n].aT ng aj empty;lc dt=eQ mJ kk lc af=gT mJ;af<mb bl lw af kx bP!=dV nx&&kF>=bP kN;aK ae(in lk empty ns eP(as ns eP(aw mx cR=ae.aX(kU cR)dt ko{mY aT-*cR}kJ jY sort(dt oj dt mK a1>b.a1||(a.a1==b.a1&&a.af<b.af kJ);fp mJ oJ ks ll}nz c6 mU fa,dV bP){auto dv=jY make_shared<aW>(fa.ag())kQ aa la aa<fa.ad()mr aa kx!dv->go(aa,bP))lV lz dv;}mB void fq(aB&aD,ak ah kx!nD||mQ a1>nD lW||(mQ a1==nD lW&&mQ af<nD->mX kx nD)aD.bO=ld aD.bO,nD lW);nD=ah;}else aD.bO=ld aD.bO,mQ a1 lJ hG(aB&aD kq aB&an kx an.co)fq(aD,*an.co);aD.bO=ld aD.bO,an.bO kJ bool dK(aB&aD kq kG kH aa kY&nU->go kP kh cC){jW g3=0 lK&empty:dv->dt kP kx kz b9(oz mX)ky fq(aD,{aa,oz mY 0,0,oz a1}kU++g3==2)lV}lz cC;}b6 bo lE kY&in=lN kK al=mu;kC aJ=aa<al ks{aJ,b2 na kz aC kP),b2 na aJ?mb aR kP:mb bd(aa-al)),kz aT kP,aJ?0:oa aa-al].cK};}aB cU lE kH lr an kq b6&data,a8&fU lQ fw=-kg),bool im lv){aB aD kK&in mH ag(kU np cK>mb bl kZ oq ke aD kK b4=an nM kK as=np as.lP aw=np aw.first kK&ax=an kE kK fP=fU.fP(b4);auto*dY=fP.as;auto*dj=fP.aw;auto*cn=fP.iI od kY a9=pos?ax[pos-1]:af kK hM=pos<b4?lg a9,ax mI):0.0;dY mI=lg a9,as)-hM+(pos<b4?lg as,ax mI):0.0);dj mI=lg a9,aw)-hM+(pos<b4?lg aw,ax mI):0.0);cn mI=lg a9,as)+lg as,aw)-hM+(pos<b4?lg aw,ax mI):0.0 kJ jX fQ=fw kt c1=kg)kt d4=kg)od){c1=lB c1,cn mI nj np aJ){c1=lB c1,d4+dj mI);d4=lB d4,dY mI kJ oI np aT-c1+1e-8<fQ ke aD;auto*bx=fP.jt kh!np aJ){bx[b4+1]=kg)kQ pos=b4+1;pos-->0;)bx mI=lB dj mI,bx[pos+1]kJ ms a la a<=b4 mr a kx!ot a)nr bq=[&]ki ld fw,im&&nD?nD lW:aD.bO kJ kK fN=np aJ?cn[a]:lB cn[a],dY[a]+bx[a+1]kU np aT-fN+1e-8<bq())ky aH dZ ny);dZ.c5(0,a);dZ.dS(np as nj dZ oM)continue kh np aJ){auto ae=dZ;ae.dS(np aw);me a,b4 kU ae oM)fq(aD,{aa lk a,a,np aT-cn[a]}nP{jX eq=nE b lQ eb){auto ae=dZ;me a,b ns dS(np aw);me b,b4 kU ae oM)fq(aD,{aa lk a,b,np aT-eb}kJ kQ b=a;b<=b4 mr b kx!ot b)nr eb=b==a?cn[a]:dY[a]+dj[b]kh np aT-eb+1e-8<bq())ky eq(b,eb kJ}lz aD;}}lM cg lE kH lr an lQ f7 kx aa>=kM||af>=kz nq||kz lF ku ks cU oA lk an,bo oA),kz aN(),f7,true).co;}mB aB c7 lE,bool fd){aB aD kh kz lF ke aD;kC cC=dK(aD,ac,aa mx data=bo oA);lc fU mH aN(mx bq=[&]ki fd?aD.bO:(nD?nD lW:-kg)kJ kh cC){mp mF)hG(aD,nf,fU,bq(),!fd)nP{ms lZ kz nq mr mX hG(aD,nf,fU,bq(),!fd)lS aD;}}aB ed lE)ki c7 oA,true kJ lM di lE)ki c7 oA,nJ.co;}lM cW::co lE kx kz lF ku;aB aD kh!dK(aD,ac,aa)ke di oA kU bS ka<=aa)bS le aa+1);lc row=bS mJ kh row ka<kz nq)row le kz nq mx&data mH az()->bo kP;lc fU mH aN()lK mF kY&an mH kR;lc a7=row kZ kK bq=nD?nD lW:-kg kU mz bu!=an nF||bq<mz bq){mz co=cU oA lk an,data,fU,bq,true).co;mz bu=an nF;mz bq=bq;oI mz co)fq(aD,*mz co lS nD;mi a3::et lE kH mX{lc a7=ov kK&an mH kR;mz mQ reset(kU kz b9(mX){mz ah=cU oA lk an,oE->bo kP,kz aN(),1e-9,true).co kh mz ah&&mz ah lW<=nN mz mQ reset(nP{mp&empty:oE->dt kP)mn oz mS af kx oz a1>nN mz ah=ak{aa lk 0,0,oz a1};lV}}mz bi=an.bi(lJ a3::eu kV aa){lc h5=bS[aQ*d1+aa].ah;h5.reset()kQ lZ d1 mr af kY&ah=ov.ah kh ah&&(!h5||ah lW>h5 lW))h5=ah;}mi a3::hO no lX am kY&dv mH az(nj dv kw("positive insertion cache requires empty taxi index" kU iT!=dv||d1!=kz nq||aQ!=kM){iT=dv;d1 mH nq;aQ=kM kh d1==lp<jW>nx||aQ>lp<jW>nx/(d1+1))throw jY length_error("positive insertion cache size overflow");bS.nW aQ*(d1+1),gZ{}kJ mp aa:am kx kz lF)ky dv->go kP kQ lZ d1 mr mX mn ov.bi!=kz kR nF)et oA,mX;eu(aa kJ mi a3::cT no kH af lX am){mp aa:am kx kz lF nr a9=co kP;et oA,af mx&c4=ov.ah kh a9&&a9->mS mX eu kP nd mn c4&&(!a9||c4 lW>a9 lW||(c4 lW==a9 lW&&af<a9->mX))bS[aQ*d1+aa].ah=c4;}mk aI::hH no)ki oE&&kM-kz eN()>=4;}aI::aI no):bh(ac),cq(kM){aQ le kM);jY iota(aQ oj aQ kI,0 nj oE ke ob aQ kx kz lF nr data=bo oA);lc ci=cq mJ;ci lU kz bm()ka)lK mF)ci nO mY nf,kz aN())kJ}aI::aI no lX am):bh(ac),cq(kM){nY hZ(kM,nJ ob am kx hZ.at kP||kz lF)ky hZ mJ=ll aQ ko aa nj oE nr data=bo oA)lK mF)cq mJ nO mY nf,kz aN())kJ}aB aI::ci mD{aB aD kh bh.lF ke aD kh!bh.az()ke ed(bh,aa);dK(aD,bh,aa)lK&[mY an]:cq.at kP)hG(aD,an)ks aD;mi aI::cT nK kx!bh.az()ke kK&an=bh.kR ob aQ kx bh.lF)ky lc ci=cq mJ kK g3=jY lower_bound(ci oj ci kI lk[]kj gj&a7 kH id)ki mz first<id;}mx data=bo(bh,aa);auto c4=cU(bh,aa lk an,data,bh.aN()kU g3==ci kI||g3->first!=mX ci.lh g3,{mY lY c4)})nd g3->second=lY c4 kJ}kd void ey::d9(kG lX aP,kA kc{mp aa:aP kY ah=di oA kU ah&&ah lW>nN kz lh mA aw kJ}kd class gi kT cG nQ"regret2";mi d9(ab&lX,kA kX;};kd void gi::d9(kG lX aP,kA kc{nY hN(kM,nJ ob aP)hN.at kP=ll aI bk md aP)mT true){lM ah kt e5=-1 ob aP kx!hN mJ nr ci=bk.ci kP kh!ci.co||ci.co lW<=nN ky kr iA=oK ci.bO)?ci.co lW-ci.bO:ci.co lW kh!ah||iA>e5||(iA==e5&&ci.co lW>ah lW)){ah=ci.co;e5=iA;}oI!ah||!kz lh ah->mA aw)ke;hN[ah->aa]lv;bk.cT(ah->af kJ}kd void ez::d9(kG lX hw,kA kc{kC cC=bool(oE kU cC)dO.hO md hw)kk;;){lM ah kt it=0 ob hw kx kz lF nr hu=cC?dO.co kP:dN.co oA kU hu&&hu lW>1e-9&&hu lW>it){ah=hu;it=hu lW;}oI!ah ke kh!kz lh ah->mA aw)kw("profit repair descriptor disagrees with insertion" kU cC)dO.cT md ah->mY hw kJ}kd be::be ok kH iv):ai(ag),iU(iv),gv(nC nc),cm(gv ka,0 kx!iv)kD("route pool limit must be positive" lJ be::add no kH mY bool fu kY&ax mH kR kE kh ax kO ke kK k mH nq,n=mu;kp am kt aT=0 lK bA:ax){jW aa kh bA<k+n)aa=bA-k nd mn nV ai.aS ka)aa=n+ou n nd ky am ko aa);aT+=kz aT(aa kJ jY sort(am oj am kI mx bD=aT-kz kR.cR(kU bD<=1e-9 ke;lc i6=gv kZ kK g3=i6.find(am kU g3!=i6 kI&&g3->second.bD>=bD ke;i6[am]=hW{am,ax,bD}kh fu&&i6 ka>iU kY jI=jY min_element(i6 oj i6 mK second.bD<b.second.bD;});i6 lH jI kJ mi be::hb no kx&lN!=&ai kw("route pool instance mismatch")lK mF kx cm kZ==kz kR nF)ky cm kZ mH kR nF;add(ac lk true kJ mi be::write(ni kc{out<<"taxi,benefit,route\n" nR kQ lZ gv lw mX mp&[key,bN]:gv kZ){out<<af+1<<','<<bN.bD<<",\"" og<bN.an lw i kx i)out<<' ';out<<ai.gE(bN.an nZ kJ out<<"\"\n";}mk be::cs(kG,dV bP kx&lN!=&ai kw("route pool instance mismatch")lK mF)add(ac lk nJ;ma ma hW>>bX(gv ka);ma int>hd(gv ka,-1),cl(gv ka,-1);ma unsigned char>dk(kM,0);kp hr(kM,0),d6;em=0 kt bW=0 kQ lZ gv lw mX{lc row=bX kZ lK&[key,bN]:gv kZ)row ko bN);jY sort(row oj row mK bD>b.bD||(a.bD==b.bD&&a.am<b.am kJ);em+=row ka;ma unsigned char>hP(kM,0)og<row lw i){for(auto r:row nZ.am)hP[r]=1 lR bZ=!row nZ.am kO mo r:row nZ.am)bZ&=nX==lq af kU bZ){jW count=0 mw)count+=nX==lq af kU count==row nZ.am ka){cl kZ=lq i);bW+=row nZ.bD;}}}ms r la r<hP lw r)hr[r]+=hP[r];}kp g1(gv ka,0)kQ lZ bX lw mX mp&bN:bX kZ)for(auto nt g1 kZ+=hr[r]-1 kQ lZ bX lw mX mn!bX kZ kO)d6 ko mX;jY stable_sort(d6 oj d6 kI,[&](auto a,auto b)ki g1[a]>g1[b];}mx ea=[&]kj hW&bN){for(auto nt mn dk[r]kN ks ll};dy la aU lv kK e1=[&]kj kp&gp){jY fill(dk oj dk kI,0)kQ lZ cl lw af kx cl kZ<0||jY find(gp oj gp kI,mX!=gp kI)continue mo r:bX kZ[cl kZ].am)dk[r]=1;nu eR la ma int>gn(gp ka,-1),fO mo af:gp){fO ko cl kZ kU cl kZ>=0)eR+=bX kZ[cl kZ].bD;nu dn=eR;jY function<void kV lQ)>jd=nE fs lQ bD kx aU ke mr dy kh(dy&63)==0&&kF>=bP){aU oJ ks;oI fs==gp ka kx bD>dn+nN{dn=bD;fO=gn;lz;nu ip=bD kQ d=fs;d<gp lw d)mp&bN:bX[gp[d]])mn ea(bN)){ip+=bN.bD;lV oI ip<=dn+1e-9 ke kK af=gp[fs]og<bX kZ lw i kY&bN=bX kZ nZ kh!ea(bN))ky gn[fs]=lq i)mo nt dk[r]=1;jd(fs+1,bD+bN.bD)mo nt dk[r]=0 kh aU)lV}gn[fs]=-1 kh!aU)jd(fs+1,bD kJ kh kF<bP)jd(0,0)nd aU oJ kh dn<=eR+1e-9 kN og<gp lw i)cl[gp nZ]=fO nZ;bW+=dn-eR ks ll}lR a6=ll while(a6&&!aU){a6 lv kQ a la a<nS a)ms b=a+1;b<nS b)a6=e1({d6[a],d6[b]})||a6;}ms a la a<nS a)ms b=a+1;b<nS b)ms c=b+1;c<nS c)e1({d6[a],d6[b],d6[c]});jZ kv dz(kM,0),gm=dz kt hn=kg);ma ma double>>es(bX ka);ma ma jW>>g0(bX ka)kQ hs la hs<192&&!aU mr hs kx kF>=bP){aU=ll lV nu jn=jY accumulate(dz oj dz kI,0.0);kp jx(dz ka,0)mo af:d6)nb fR la int gn=-1 og<bX kZ lw i kY&bN=bX kZ nZ kt value=bN.bD mo nt value-=dz[r]kh value>fR){fR=value;gn=lq i kJ}jn+=fR kh gn>=0)for(auto r:bX kZ[gn].am)++jx[r];oI jn<hn){hn=jn;gm=dz;nu jF=0 kQ r la r<dz lw r)mn dz[r]>0||jx[r]>1){kr hY=kl double>(jx[r])-1;jF+=hY*hY;oI!jF||jn<=bW+1e-7)lV kr jU=0.9*(jn-bW)/jF kQ r la r<dz lw r)dz[r]=ld 0.0,dz[r]+jU*(kl double>(jx[r])-1)kJ dz=lY gm)ox hQ=jY accumulate(dz oj dz kI,0.0)mo af:d6){lc fL=es kZ lK&bN:bX kZ)nb value=bN.bD mo nt value-=dz[r];fL ko value kJ lc gq=g0 kZ;gq le fL ka);jY iota(gq oj gq kI,0);jY stable_sort(gq oj gq kI,[&](auto a,auto b)ki fL[a]>fL[b];}kJ jY fill(dk oj dk kI,0);jY function<void kV lQ lQ)>gV=nE fs lQ bD lQ fT kx aU ke mr dy kh(dy&63)==0&&kF>=bP){aU oJ ks;oI fs==d6 ka kx bD>bW+nN{bW=bD;cl=hd;lz;nu ip=bD kQ d=fs;d<d6 lw d)mp&bN:bX[d6[d]])mn ea(bN)){ip+=bN.bD;lV oI ip<=bW+1e-9 ke kt f9=bD+hQ-fT kQ d=fs;d<d6 lw d kY af=d6[d]kt fR=0 mo i:g0 kZ kx es kZ nZ<=0)break kh ea(bX kZ nZ)){fR=es kZ nZ;lV}}f9+=fR;oI f9<bW-1e-7 ke kK af=d6[fs]mo i:g0 kZ kY&bN=bX kZ nZ kh!ea(bN))ky hd kZ=lq i)kt jM=fT mo nt{dk[r]=1;jM+=dz[r];}gV(fs+1,bD+bN.bD,jM)mo nt dk[r]=0 kh aU)lV}hd kZ=-1 kh!aU)gV(fs+1,bD,fT kJ kh kF<bP)gV(0,0,0)nd aU oJ kh bW<=kz bM()+1e-9 kN;cA ae;ae.cI le kz nq)kQ lZ cl lw mX mn cl kZ>=0)ae.cI kZ=bX kZ[cl kZ].an kK hq=ae.eq(ai nj hq.bc||jY abs(hq.bM-bW)>1e-6 kw("route pool selection disagrees with full evaluation");ab h3(ai,lY ae),kz ee());h3.dA(oE);kz dM(h3)ks ll}kd bool gh::gV(kG kc{bool a6 lv kk int jl la jl<2 mr jl){bool en lv lK&move:fA){en=move->cs(ac)||en;oI!en)lV a6=ll lz a6;}kd bool gL mV kC cC=bool(oE kU aQ ka!=kM){aQ le kM);jY iota(aQ oj aQ kI,0 kJ mn cC)eY.hO md aQ)lR a6 lv kk;;){lM co ob aQ kx kz lF nr move=cC?eY.co kP:dN.co oA kU move&&move lW>1e-9&&(!co||move lW>co lW))co=move;oI!co)break kh!kz lh co->aa,co->mY co->as,co->aw)kw("add-request descriptor disagrees with mutation");a6 oJ kh cC)eY.cT md co->mY aQ lS a6;}kd bool fm mV ma kf<double>>h8(kM)nI)h8 mJ mH aO kP lR a6 lv kk;;){kf<jW>co kt bC=1e-9 nI kx h8 mJ&&*h8 mJ>bC){co=aa;bC=*h8 mJ;}oI!co)break kK af=mt kz ap(*co).af nj kz ga(*co)kw("remove-request descriptor disagrees with mutation");a6=ll h8[*co].reset()nI)mn kz ap kP.mS lq mX)h8 mJ mH aO(aa lS a6;}kd aq::aq lE):bh(ac),dl kP,b3(0),cQ(0 kx aa>=mu||kz ap kP.af<0 ke kK&oo kP;b3=mt kS mX;cQ=kS as;gB=ll bH mH aO kP kK&an mH bk(b3);bB=bE(lN,b3,an nA br kJ jW aq::b4 nK oe size=bh.kR nM ks mS b3?size-2:size;}jW aq::ca kV aV kc ki aV<cQ?aV:aV+2;mi aq::nH oe fy=lB end,cQ kU first<fy)me lP fy mx dQ=ld lP cQ kU dQ<end)me dQ+2,end+2 kJ mM aq::a1 nK kH bt lC af>=bh.nq ku ks a1(mY bh.kR,bt kJ mM aq::bj ml bt mR)||bt oH||(af!=b3&&!bH)ku ng dJ iG{an kE,cQ,cQ+1,mS b3}kK eb=bF(oF)lk iG,bt,bh.aC(dl),oF).aR(dl))ks bB-eb;}mM aq::a1 ml bt mR)||bt oH ku oD oF);mW:0 kU hz kx bt&&!ot ca(bt-1)+1)ku;}else mn!bH||!ot bt)ku;aH ae ny kU oG 0,bt)nd me 0,bt ns bA(bh.aC(dl)ns bA(mb aR(dl)kU oG bt,cf)nd me bt,cf nj ae oM ku ks bj on,bt kJ lM c9::cw no kq aq&bY kH r kH lr an,kB aG oe bg=mt nX kU af!=bg&&!bY.dw()ku ks eY.get(r lk kz nq,aG,an nF,bY.cy(),mS bg,[&]{lM bs kK b4=an nM-(mS bg?2:0)kQ a la a<=b4 mr a kY fC=bY.bj on,a nj fC||*fC my a1=bY.a1 on,a kU a1&&(!bs||*a1>bs lW))bs=ak{r lk a,a,*a1};lz bs;}kJ bool c9 mV eV hw kK dq=nE r,bool all kH nL kx nX<0 ke;aq bY md r mx bg=mt nX nj bY.fX()ke kK aG mH bk(bg).bi(mx&nU->go(r kU cC kx all){static const aj e9 lK&ah:dv->dt(r)kx kz b9(mQ mX nr a1=bY.a1(mQ mY e9,0 kU a1)hw.add(ak{r,mQ mY 0,0,*a1},bg);lV}}mp mF mf else{ms lZ kz nq mr af mf}kK fG=[&]{hw ln kQ r la r<mu mr r)dq(r,true,0,0 kJ;fG()lR a6 lv mT jX aM=hw.co()kY ah=aM->ah kK bg=aM->bg kK c8=mQ af;kC cE mH b9(bg);kC cF mH b9(c8);kC hz=bg==c8;kC eB=hz?kz cx(mQ aa,mQ as,mQ aw):(kz ga(mQ aa)&&kz lh mQ aa,c8,mQ as,mQ aw)nj eB kw("passenger relocation descriptor disagrees with mutation");a6 oJ kh kz b9(bg)!=cE||kz b9(c8)!=cF){fG();ky}hw.eZ(bg,c8)kQ r la r<mu mr r kx nX<0 nr hv=mt nX);dq(r,hv==bg||hv==c8,bg,c8 kJ lz a6;}kd ao::ao lE):bh(ac),dl kP,b3(0),dH(0),cS(0 kx aa<mu||aa>=kM||kz ap kP.af<0 ke kK&oo kP;b3=mt kS mX;dH=kS as;cS=kS br;gB=ll bH mH aO kP;iw=b2(lN,kz aC kP);hp=b2(lN,lN.bd(aa-mu)mx&an mH bk(b3);bB=bE(lN,b3,an nA br kJ jW ao::b4 nK oe size=bh.kR nM ks mS b3?size-2:size;}jW ao::ca kV aV lC aV<dH ke aV kh aV<cS-1 ke aV+1 ks aV+2;mk ao::cj kj aj&an kH bt kc ki!bt||an.ay(oF),ca(bt-1)+1 lJ ao::nH oe fy=lB end,dH kU first<fy)me lP fy mx f6=ld lP dH mx gR=lB end,cS-1 kU f6<gR)me f6+1,gR+1 mx dQ=ld lP cS-1 kU dQ<end)me dQ+2,end+2 kJ mM ao::a1 nK kH a kH b lC af>=bh.nq ku ks a1(mY bh.kR,a,b kJ mM ao::bj ml a kH b mR)||a>b||b oH||(af!=b3&&!bH)ku ng dJ iG{an kE,dH,cS,mS b3}kK eb=cV(oF)lk iG,a,b,bh.aC(dl),oF).bd(dl-bh.al()))ks bB-eb;}mM ao::a1 ml a kH b oe dZ=er on,a)ks dZ?du on,a,b,*dZ):jY nullopt;}kf<aH>ao::er ml a mR)ku;mW:0 kU a>cf ku oD oF mx cK=oa dl-bh.al()].cK kh cK>mb bl kZ oq ku kh hz kx!cj(an,a)ku;}else mn!bH||!ot a)ku;aH ae ny kU oG 0,a)nd me 0,a ns dS(iw nj ae oM ku ks ae;}mM ao::du ml a kH b kq aH&dZ kc{mW:0 kU a>b||b>cf ku oD oF kU hz kx!cj(an,b)ku;}else mn!ot b)ku;auto ae=dZ kh oG a,b)nd me a,b ns dS(hp kU oG b,cf)nd me b,cf nj ae oM ku ks bj on,a,b kJ lM d2::cw no kq ao&bY kH r kH lr an,kB aG oe bg=mt nX kU af!=bg&&!bY.dw()ku ks eY.get(r lk kz nq,aG,an nF,bY.cy(),mS bg,[&]{lM bs kK b4=an nM-(mS bg?2:0 mx&in=lN kK oo(r)ng dJ iG{an nA br,mS bg}kK aC mH aC(r mx fr=mb bd(r-mu);jZ kv fi(b4+1),eo(b4+1),ek(b4+1),bx(b4+2,kg))od){fi mI=bT(in lk iG,pos,aC);eo mI=bT(in lk iG,pos,fr);ek mI=bF(in lk iG,pos,aC,fr kJ ms pos=b4+1;pos-->0;)bx mI=lB eo mI,bx[pos+1])kQ a la a<=b4 mr a kY fN=lB ek[a],fi[a]+bx[a+1]kU bY.cy()-fN my dZ=bY.er on,a nj dZ)continue kQ b=a;b<=b4 mr b kY eb=b==a?ek[a]:fi[a]+eo[b]kK fC=bY.cy()-eb kh fC my a1=bY.du on,a,b,*dZ kU a1&&(!bs||*a1>bs lW))bs=ak{r lk a,b,*a1};}lz bs;}kJ bool d2 mV eV hw kK dq=nE r,bool all kH nL kx nX<0 ke;ao bY md r mx bg=mt nX nj bY.fX()ke kK aG mH bk(bg).bi(mx&nU->go(r kU cC kx all){static const aj e9 lK&ah:dv->dt(r)kx kz b9(mQ mX nr a1=bY.a1(mQ mY e9,0,0 kU a1)hw.add(ak{r,mQ mY 0,0,*a1},bg);lV}}mp mF mf else{ms lZ kz nq mr af mf}kK fG=[&]{hw ln kQ r=mu;r<kM mr r)dq(r,true,0,0 kJ;fG()lR a6 lv mT jX aM=hw.co()kY ah=aM->ah kK bg=aM->bg kK c8=mQ af;kC cE mH b9(bg);kC cF mH b9(c8);kC hz=bg==c8;kC eB=hz?kz cx(mQ aa,mQ as,mQ aw):(kz ga(mQ aa)&&kz lh mQ aa,c8,mQ as,mQ aw)nj eB kw("parcel relocation descriptor disagrees with mutation");a6 oJ kh kz b9(bg)!=cE||kz b9(c8)!=cF){fG();ky}hw.eZ(bg,c8)kQ r=mu;r<kM mr r kx nX<0 nr hv=mt nX);dq(r,hv==bg||hv==c8,bg,c8 kJ lz a6;}kd mB struct ih{jW af;kp gO;};kp gb no kH af kY&in=lN kK&an mH kR kK mm;jW eI la kp jJ{0}kQ pos la pos<an kE lw pos kY bA=an kE mI kh nV m)++eI nd mn bA>=k+2*n+m)--eI kh!eI&&ot pos+1))jJ ko pos+1 lS jJ;}mk ds mV++gP oD lN lR a6 lv kk;;){ma ih>cI;cI lU kz bm()ka)lK mF)cI ko{mY gb md mX})lR eB lv og<cI ka&&!eB mr i){ms j=i+1;j<cI ka&&!eB mr j kY bU=cI nZ.mY bV=cI[j].af kK&a mH bk(bU mx&b mH bk(bV mx g5=a nM,g6=b nM kt bC=1e-9;jW iQ=0,iR=0 lK e7:cI nZ.gO){mp e8:cI[j].gO kY iY=e7?a kE[e7-1]:bU kK iZ=e8?b kE[e8-1]:bV kt a1=0 kh e7<g5 kY iW=a kE[e7];a1+=lg iY,iW)-lg iZ,iW kJ mn e8<g6 kY iX=b kE[e8];a1+=lg iZ,iX)-lg iY,iX kJ mn a1<=bC+1e-8)ky aH gd na bU,a);gd.c5(0,e7 kU e8<g6)gd.dS(b.c5 na e8,g6-1)nj gd oM)ky aH ge na bV,b);ge.c5(0,e8 kU e7<g5)ge.dS(a.c5 na e7,g5-1)nj ge oM)ky bC=a1;iQ=e7;iR=e8;}oI bC>nN{kr jj mH bM(nj kz cZ(bU,iQ,bV,iR)kw("validated clean-cut exchange failed")mr eX;gD+=kz bM()-jj;a6=eB=ll}}oI!eB ke a6;}}kd kf<aL>a5::dL no lC!fK ke dm(ac);dT(ac);ft()ks dp kO?jY nullopt:kf(dp.top().ah lJ a5::ft(kc{while(!dp kO kY&a7=dp.top(mx af=mz mQ bp.af kK aF=mz mQ bp.aa kh dU[aF]&&mz bu==cc kZ&&mz b8==bI kZ[aF].b8)lV dp.pop(kJ mk a5::cX::nG a kq ba&b lC a.mQ a1!=b.mQ a1 ke a.mQ a1<b.mQ a1 kK key=[]kj aL&c)ki jY tuple(c.a0,c.bp.aa,c.bp.as,c.bp.aw kJ ks key(a.ah)>key(b.ah lJ a5::cP no kH af lX a0 lX aF lC aF kO ke;lc row=bI kZ ob aF){++row mJ.b8;row mJ.mQ reset(kJ jX&dE mH kR;kp ax ob a0 kY oo kP;lc f2=iN mJ kh f2.eK!=dE nF){ax=dE kE;mP);f2.es.fj(lN lk ax);f2.aO=bE(lN lk dE nA br)-kz aT kP;f2.eK=dE.bi(kJ mp gy:aF){lc aM=row[gy]ox bq=oL?oL lW:1e-9 kK bp=cg md gy lk f2.es,bq-f2.aO kU bp&&bp lW+f2.aO>bq)oL=aL{aa,*bp,bp lW+f2.aO};}}mp aa:aF)fb(mY aa lJ a5::fb nK kH aF oe&aM=bI kZ[aF]kh oL){dp.push({*oL,cc kZ,aM.b8}kJ mi a5::dT no oe bl mH nq,am=kM kh fH!=&lN||cc ka!=bl||dU ka!=am){fH=&lN;cc.nW bl,lp<kB>nx);dU.nW am,nJ;bI.nW bl,ma d8>(am));dp=hE{};iN le am kJ ma ma jW>>a0(bl);kp aF,d5 kQ aa la aa<am mr aa kY oo kP;kC eS=kS af<0 kh eS){aF ko aa nj dU mJ)d5 ko aa nP a0[kS af]ko aa);dU mJ=eS;}ms lZ bl mr af kY bi mH kR.bi(kU cc kZ!=bi){cc kZ=bi;cP(ac lk a0 kZ,aF nP mn!d5 kO){cP(ac lk a0 kZ,d5 kJ oI dp ka>2*bl*am+32)fo(lJ a5::fo(kc{dp=hE{}kQ lZ bI lw mX ms aF la aF<dU lw aF kx dU[aF])fb(mY aF kJ}kf<aL>a5::dm no kc{kp eS mw)mn nX<0)eS ko r kU eS kO ku;iN le kM);kf<aL>co kt bC=1e-9;kp ax;aj es kQ a0 la a0<kM mr a0 kY oo(a0 kU kS af<0 nr af=mt kS af mx&dE mH kR;lc row=iN[a0]kh row.bu!=dE nF){row.bu=dE nF;row.hI ln;row.hI le kM kJ kr aO=bE(lN lk dE nA br)-kz aT(a0)lR h2 lv lK aF:eS){lc a7=row.hI[aF]kh!mz fX||(!mz ah&&bC<mz bq)kx!h2){ax=dE kE;mP);es.fj(lN lk ax);h2=ll}mz fX=ll mz bq=bC;mz mQ reset(mx bp=cg md aF lk es,bC-aO kU bp&&bp lW+aO>bC)mz ah=aL{a0,*bp,bp lW+aO};oI mz ah&&mz ah lW>bC){co=mz ah;bC=co lW;}}lz co;mk a5 mV bool a6 lv mT jX ah=dL(ac)kx!kz eL(ah->a0,ah->bp.aa,ah->bp.as,ah->bp.aw)kw("replacement descriptor disagrees with mutation");a6=ll lz a6;}kd mB bool io kj aY&a kq aY&b)ki a.a1>b.a1||(a.a1==b.a1&&jY tie(a.lP a.second)<jY tie(b.lP b.second)kJ mM d0 no kH a0 kH aF kY oo(a0 mx af=mt kS af mx&an mH kR oD lN;kC aJ=aF<mu kK aw=aJ?mb aR(aF):mb bd(aF-mu);aK ae ny ns cv(0,kS as ns eP(kz aC(aF)kU aJ)ae.eP(aw ns cv(kS as+1,kS br nj aJ)ae.eP(aw ns cv(kS br+1,an nM)ks ae.aX(kJ}mM dd no kH nL kx first>=kM||second>=kM ku kh second<nv jY ne nn mx a mH ap(nv,b mH ap(second kU a.af<0||b.af<0||a.mS b.af ku kK f3=d0 md nn nj f3 ku kK f4=d0 md second,first nj f4 ku ks kz bk(a.mX.cR()+kz bk(b.mX.cR()-*f3-*f4;mk cN::cX::nG a kq ba&b kc ki io(b.ah,a.ah lJ cN::fb kV bU kH bV oe&aM=bI[bU][bV]kh oL)dp.push({*oL,bU,bV,cm[bU],cm[bV],aM.b8}lJ cN::dT no oe bl mH aA(kU ai!=&lN||cm ka!=bl){ai=&lN;cm.nW bl,lp<kB>nx);bI.nW bl,ma d8>(bl));dp=hE{};}kp a6;nY jm(bl,nJ kQ lZ bl mr af kY bi mH kR.bi(kU cm kZ!=bi){cm kZ=bi;a6 ko mX;jm kZ=ll}oI a6 kO ke;ma ma jW>>am(bl)nI kY af mH ap kP.af kh af>=0)am kZ ko aa kJ mp af:a6){ms bR la bR<bl mr bR kx bR==af||(jm[bR]&&bR<mX nr bU=lB mY bR),bV=ld mY bR);lc aM=bI[bU][bV]mr aM.b8;aM.mQ reset()lK first:am[bU]){mp second:am[bV]kY a1=dd md nn nj a1||*a1<=nN ky const aY ah{lB nn),ld nn),*a1}kh!oL||io(ah,*oL))oL=ah;}}fb(bU,bV kJ oI dp ka>2*bl*bl+32){dp=hE{}kQ a la a<bl mr a)ms b=a+1;b<bl mr b)fb(a,b kJ}kf<aY>cN::dL no lC!fK){kf<aY>co kQ a la a<kM mr a kx kz ap(a).af<0)continue kQ b=a+1;b<kM mr b kY a1=dd md a,b nj a1||*a1<=nN ky const aY ah{a,b,*a1}kh!co||io(ah,*co))co=ah;}lz co;}dT(ac)mT!dp kO kY&a7=dp.top(kU mz iD==cm[mz bU]&&mz iE==cm[mz bV]&&mz b8==bI[mz bU][mz bV].b8 ke mz ah;dp.pop(lS jY nullopt;mk cN mV bool a6 lv mT jX ah=dL(ac)kx!kz fI(ah->lP ah->second)kw("request swap descriptor disagrees with mutation");a6=ll lz a6;}}int main(kY ja=kF;try{jY ios::sync_with_stdio(nJ;jY cin.tie(nullptr mx ag=a2::fE(jY cin);a2::dh::ic b7;b7.jL=1ULL ox i9=jY chrono::duration kv(kF-ja).count()ox gk=300 ox reserve=gk>2?1.0:gk*0.1;b7.ei=gk-i9-reserve kh b7.ei<=0){a2::cA empty;oz cI le nC nc);a2::d7(jY cout,empty,ag nP{jX aD=a2::dh::js(ag,b7);a2::d7(jY cout,aD.b5,ag);jY cerr nR<<"iterations="<<aD.dG<<" seconds="<<aD.iB<<" stop="<<(aD.dX?"temperature":"time_limit")<<'\n';lz 0;}catch kj jY exception&is){jY cerr<<"error: "<<is.what()<<'\n' ks 1;}}
