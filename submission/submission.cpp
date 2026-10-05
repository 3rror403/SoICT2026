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
#define jT std::size_t
#define jU const auto
#define jV std::
#define jW jV vector
#define jX .size()
#define jY [[nodiscard]
#define jZ )const
#define ka }namespace a2::dg{
#define kb )return
#define kc jV optional
#define kd jV numeric_limits<double>::infinity(
#define ke ;if(
#define kf {return
#define kg (const
#define kh ;for(
#define ki static_cast<
#define kj .begin()
#define kk public:jV string_view g7(jZ override
#define kl .push_back(
#define km jW<jT>
#define kn ,const
#define ko const double
#define kp ;return
#define kq ;double
#define kr kb jV nullopt
#define ks <double>
#define kt )throw jV logic_error
#define ku ){if(
#define kv continue;
#define kw ac.
#define kx jV mt19937_64&
#define ky jV uint64_t
#define kz const bool
#define kA throw jV invalid_argument
#define kB .ax()
#define kC jV chrono::steady_clock::now()
#define kD ab&ac
#define kE ,jT
#define kF .end()
#define kG );}
#define kH ;jU
#define kI jV make_unique
#define kJ kw ad()
#define kK kb false
#define kL .empty()
#define kM (aa)
#define kN kh jT
#define kO bk(af)
#define kP loc.
#define kQ final:public
#define kR )ke
#define kS (jT
#define kT private:
#define kU jZ override
#define kV ){jU
#define kW [af]
#define kX =0;
#define kY };ka class
#define kZ auto&
#define la jV max(
#define lb .resize(
#define lc {public:
#define ld in.cQ(
#define le insert(
#define lf )throw jV runtime_error
#define lg jV chrono::duration_cast<hj::duration>(jV chrono::duration ks(
#define lh ,af,
#define li true;
#define lj (jZ kf
#define lk kb{false,0,
#define ll jV numeric_limits
#define lm ki int>(
#define ln af kn aj&
#define lo )if(all||af==first||af==second)gE.add
#define lp );read(in,
#define lq .clear()
#define lr ap(r).af
#define ls =false
#define lt jX;++
#define lu jV shared_ptr<
#define lv ;mutable
#define lw }return
#define lx kg au&
#define ly jV min(
#define lz jZ{if(
#define lA ;struct
#define lB kg kD kE aa
#define lC ap kM.af>=0
#define lD ;void
#define lE .erase(
#define lF jY]const
#define lG kG void
#define lH kh jU
#define lI first kE
#define lJ kc<ak>
#define lK kw ag()
#define lL )=default;virtual jV string_view g7(jZ kX virtual
#define lM first,
#define lN ,double
#define lO ;bool
#define lP kG return
#define lQ .duration
#define lR break;
#define lS ->a1
#define lT kn km&
#define lU jV move(
#define lV af kX af<
#define lW jW<
#define lX in.
#define lY jY]jT
#define lZ (ac,
#define ma ae.c4(
#define mb lo(cw lZ bY,r lh kw kO,aG),bg kG
#define mc namespace a2
#define md ar jX
#define me .reserve(
#define mf }void
#define mg }km d8(ab&kE,kx kU;kY
#define mh }bool
#define mi kS ln an kE
#define mj k=lX bl jX,n=lX md,m=lX aS jX
#define mk if(
#define ml kh auto
#define mm for(jU
#define mn d8(kD kE count,kx rng
#define mo ;++
#define mp for kS
#define mq ki jT>(
#define mr kw al()
#define ms jV lower_bound(av kj,av kF
#define mt kN r kX r<kJ mo r
#define mu )kH
#define mv +1e-8<la 1e-9,bs?bs lS:1e-9))kv jU
#define mw a7.
#define mx aa,ah->af,ah->as,ah->
#define my namespace{
#define mz jY]double
#define mA kS aa jZ
#define mB af:kw bm()
#define mC lm af kR bA==aC kM)kP as=p;else kP br
#define mD =kw
#define mE [pos]
#define mF [aa]
#define mG jV uniform_real_distribution ks
#define mH kF,[](jU&a,jU&b)kf a.
#define mI ;class
#define mJ kc ks
#define mK lx in kE
#define mL an lE an kj+kP br);an lE an kj+kP as
#define mM ax lE ax kj+kP br);ax lE ax kj+kP as
#define mN ah.
#define mO lz!fW()||af>=bh.aA(
#define mP af==
#define mQ while(
#define mR kg ab&
#define mS ::cs(kD jZ{
#define mT kz hy=mP b2 kH ce=an kB jX-(hy?2
#define mU af)
#define mV af,
#define mW ;jY]
#define mX (in,
#define mY {double
#define mZ bl jX
#define na ;else
#define nb swap(
#define nc cT lZ aa lh kw kO,data
#define nd ;const
#define ne jV unique_ptr<
#define nf jV ostream&out
#define ng kR!
#define nh ka struct
#define ni ai->
#define nj ab::
#define nk lM second
#define nl kg kD
#define nm data.
#define nn aA()
#define no )kv jU
#define np );ae.
#define nq r:bM.am)
#define nr }double
#define ns first)
#define nt (*ai
#define nu ::max()
#define nv (in lh an
#define nw lu const aW>
#define nx kB,kP as,kP
#define ny ;an.le an kj
#define nz ag.
#define nA aD.co
#define nB [&]kS
#define nC .bi()
#define nD operator()kg ba&
#define nE bn(aH&ae kE lI end
#define nF kN aa kX aa<kJ mo aa
#define nG false)
#define nH kS af
#define nI lI second
#define nJ kB jX
#define nK 1e-9)
#define nL .emplace_back(
#define nM kG else
#define nN {kk kf
#define nO <<jV setprecision(17)
#define nP d5 jX&&!aU mo
#define nQ ;kZ loc=a4 mF;kP af=mC=
#define nR du mD az();kz cB=du&&du
#define nS bA>=k+2*n&&bA<k+2*n+
#define nT assign(
#define nU kw lr
#define nV lW bool>
#define nW [i]
#define nX lX aS[
#define nY lH aa:
#define nZ bJ.ei(kI<
#define oa kN pos kX pos<=b3 mo pos
#define ob jZ{jU
#define oc jV shuffle(
#define od kN i kX i
#define oe kZ an=b0;an
#define of mA{jU n=ni md kp aa<n?
#define og kj,
#define oh lx ag
#define oi inline double
#define oj kT struct
#define ok (mV an
#define ol loc mD ap
#define om jV istream&in
#define on .bf+1e-8
#define oo kU;kT mutable
#define op an.ay mX
#define oq bA-k-2*
#define os bR[aa*d0+af]
#define ot bQ.
#define ou ;ko
#define ov count)
#define ow empty.
#define ox lZ aa
#define oy bw kE aZ
#define oz .as.bG
template<class T,class P>void gK(lW T>&fK,P ix){fK lE jV remove_if(fK og fK kF,ix),fK kF kG inline jT gB kS value)kf value?ll<unsigned long long>::digits-__builtin_clzll(ki unsigned long long>(value)):0;}mc{struct hm mY bG=0 kq fw kX}lA jL mY bf kX}lA ic mY aT kX hm as kq de kX}lA jd mY aT=0 kq cJ kX hm as;hm aw kq de kX}lA au{lW jL>bl;lW ic>ar;lW jd>aS;km df;jW ks ev;jW ks eu;lY ee lj mZ+2*(md+aS jX kG lY gD kS bA jZ kf df kL?bA+1:df.at(bA kG lY cL kS i jZ kf mZ+i;}lY aR kS i jZ kf mZ+md+i;}lY dE kS j jZ kf mZ+2*md+j;}lY bd kS j jZ kf mZ+2*md+aS jX+j;}mz ia kS u kE v jZ kf ev.at(u*ee()+v kG mz cQ kS u kE v jZ kf eu.at(u*ee()+v lG hh(jZ;};}mc{my void ch(double x kn char*g7 ku!jV isfinite(x)||x<0)kA(g7 lG eN(hm w){ch(w.bG,"negative or nonfinite window start");ch(w.fw,"negative or nonfinite window end" kR w.bG>w.fw)kA("reversed time window" kG mf au::hh(lz bl kL)kA("instance needs at least one taxi" mu v=ee(ng df kL ku df jX!=v)kA("invalid vertex mapping size");nV jo(v,nG lH id:df ku id==0||id>v||jo[id-1])kA("vertex IDs must be a permutation of 1..V");jo[id-1]=li}}mk v>ll<jT>nu/v||ev jX!=v*v||eu jX!=v*v)kA("matrix dimensions do not match node count")lH&af:bl)ch(af.bf,"invalid taxi capacity")lH&p:ar){ch(p.aT,"invalid passenger revenue");ch(p.de,"invalid passenger service duration");eN(p.as kG mm&p:aS){ch(p.aT,"invalid parcel revenue" ng jV isfinite(p.cJ)||p.cJ<=0)kA("invalid parcel weight");ch(p.de,"invalid parcel service duration");eN(p.as);eN(p.aw kG for(double x:ev)ch(x,"invalid travel time")kh double x:eu)ch(x,"invalid travel cost" kG}mc{struct gM{bool bc ls kq bV kX jV string ir;}lA cA{lW lW jT>>cH mW gM ep oh jZ;};}mc{gM cA::ep oh lz cH jX!=nz mZ lk"route count differs from taxi count"}kH n=nz md kH m=nz aS jX kH cC=nz mZ kH i9=nz ee();lW int>cd(n+m,-1);lW int>eb(n+m,-1)kq bV kX constexpr double eps=1e-8 kN k kX k<cC mo k){jT a9=k kq cG=0 kq load=0 kH&an=cH[k]kN pos kX pos<an lt pos kV bA=an mE ke bA<cC||bA>=i9 lk"route contains invalid service node"}ou cX=cG+nz ia(a9,bA ng jV isfinite(cX)lk"nonfinite route time"};bV-=nz cQ(a9,bA kR bA<cC+n kV i=bA-cC ke cd nW!=-1 lk"passenger picked up more than once"}kH&p=nz ar nW ke cX>p.as.fw+eps lk"passenger pickup is late"}ke pos+1==an jX||an[pos+1]!=nz aR(i)lk"passenger drop-off must immediately follow pickup"};cd nW=lm k);cG=la cX,p oz)+p.de;}else mk bA<cC+2*n kV i=bA-cC-n ke cd nW!=lm k)||eb nW!=-1 lk"invalid passenger drop-off"};eb nW=lm k);cG=cX+nz ar nW.de;}else mk bA<cC+2*n+m kV j=bA-cC-2*n kH id=n+j ke cd[id]!=-1 lk"parcel picked up more than once"}kH&p=nz aS[j]ke cX>p.as.fw+eps lk"parcel pickup is late"};load+=p.cJ ke load>nz bl[k].bf+eps lk"parcel capacity exceeded"};cd[id]=lm k);cG=la cX,p oz)+p.de;}else{jU j=bA-cC-2*n-m kH id=n+j ke cd[id]!=lm k)||eb[id]!=-1 lk"invalid parcel drop-off"}kH&p=nz aS[j]ke cX>p.aw.fw+eps lk"parcel delivery is late"};load-=p.cJ;eb[id]=lm k);cG=la cX,p.aw.bG)+p.de;}a9=bA;}}mp i kX i<n+m mo i ku cd nW!=eb nW lk"partially served request"}ke cd nW!=-1)bV+=i<n?nz ar nW.aT:nz aS[i-n].aT;lw{true,bV,{}};}}mc{au fD(om)lD iF(nf kn au&ag kG mc{my template<typename T>void read(om,T&value kn char*jz ku!(in>>value)lf(jV string("cannot read ")+jz kG}au fD(om){jT k=0,n=0,m kX read mX k,"taxi count" lp n,"passenger count" lp m,"parcel count" kR k==0||k>100000||n>20000||m>20000||n+m>20000 lf("invalid or unsupported instance size")nd jT v=k+2*(n+m kR v>50000 lf("node count too large");au ag;nz bl lb k);nz ar lb n);nz aS lb m);nz df lb v);nV jo(v,false mu hB=nB bA){kZ id=nz df[bA];read mX id,"vertex ID" kR id==0||id>v||jo[id-1]lf("vertex IDs must be distinct and within 1..V");jo[id-1]=li}kN lV k mo mU{hB(af lp nz bl kW.bf,"taxi capacity" kG mp i kX i<n mo i){kZ p=nz ar nW;hB(nz cL(i));hB(nz aR(i)lp p oz,"passenger earliest" lp p.as.fw,"passenger latest" lp p.de,"passenger service" lp p.aT,"passenger revenue" kG mp j kX j<m mo j){kZ p=nz aS[j];hB(nz dE(j));hB(nz bd(j)lp p.cJ,"parcel weight" lp p oz,"parcel pickup earliest" lp p.as.fw,"parcel pickup latest" lp p.aw.bG,"parcel delivery earliest" lp p.aw.fw,"parcel delivery latest" lp p.de,"parcel service" lp p.aT,"parcel revenue" kG km g4(v)kN bA kX bA<v mo bA)g4[nz gD(bA)-1]=bA ml*iT:{&nz ev,&nz eu}){iT->resize(v*v)kN u kX u<v mo u)mp w kX w<v mo w)read mX(*iT)[g4[u]*v+g4[w]],"matrix entry" kG jV string jy ke in>>jy lf("unexpected data after instance matrices");nz hh()kp ag;}}mc{cA iK(om kn au&ag)lD d6(nf kn cA&b4 kn au&ag kG mc{void d6(nf kn cA&b4 kn au&ag){nz hh(mu db=b4.ep(ag ng db.bc lf("invalid solution: "+db.ir);out nO<<db.bV<<'\n' lH&an:b4.cH){out<<an jX lH bA:an)out<<' '<<nz gD(bA);out<<'\n';}mk!out lf("failed to write solution" kG nh bP{jT first=0,hI=0 kq duration=0 kq ha=-kd)kq bv=kd)kq cQ=0 kq bz=0,dC=0,dB kX bool bc=li};bP b1 oh kE bA);bP jm oh kn bP&jS kn bP&jP)mI aj lc aj()=default;aj oh kE af lT an)lD fi oh kE af lT an)lD clear()lD dL kg aj&bQ)mW bP c4 oh kE lI hI jZ mW bool ay oh kE aV jZ;mz cQ lj h5;}jY]ky bi lj fL;}lF km&ax lj dx;}lF jW ks&cX lj ek;}lF jW ks&cG lj da;}lF jW ks&load lj hJ;}kT km dx;lW bP>ct;lW lW bP>>dq;jW ks ek,da,hJ kq h5 kX ky fL kX}mI aK lc aK oh kE ln bk)lD cv kS lI end)lD e2 kg bP&dR)lD eO kS bA)mW mJ aX(jZ;mz gG(jZ;kT const au&ai;jT hg nd aj&eX;kc<bP>cu;};ka my jV atomic<ky>fA{1}kq de mK bA kV mj ke bA<k||bA>=lX ee()kt("invalid service node" kR bA<k+2*n kb lX ar[(bA-k)%n].de kp nX(oq n)%m].de;nr h6 mK bA kV mj ke bA<k+2*n kb 0 kH j=(oq n)%m kp bA<k+2*n+m?nX j].cJ:-nX j].cJ;}}bP b1 mK bA){bP s;s.first=s.hI=bA;s lQ=de mX bA mu mj nd hm*gY=nullptr ke bA<k+n)gY=&lX ar[bA-k].as na mk nS m)gY=&nX oq n].as na mk bA>=k+2*n+m)gY=&nX oq n-m].aw ke gY){s.ha=gY->bG+s lQ;s.bv=gY->fw;}s.bz=h6 mX bA);s.dC=ly 0.0,s.bz);s.dB=la 0.0,s.bz)kp s;}bP jm lx in kn bP&a kn bP&b){ko fI=lX ia(a.hI,b.ns;bP s;s.first=a.first;s.hI=b.hI;s lQ=a lQ+fI+b lQ;s.ha=la a.ha+fI+b lQ,b.ha);s.bv=ly a.bv,b.bv-fI-a lQ);s.bc=a.bc&&b.bc&&a.ha+fI<=b.bv;s.cQ=a.cQ+ld a.hI,b.ns+b.cQ;s.bz=a.bz+b.bz;s.dC=ly a.dC,a.bz+b.dC);s.dB=la a.dB,a.bz+b.dB)kp s;}aj::aj mK af lT an){fi nv lG aj::clear(){dx lq;ct lq;ek lq;da lq;hJ lq kh kZ fy:dq)fy lq;h5 kX fL kX mf aj::dL kg aj&bQ ku this==&bQ kb;dx=ot dx;ct=ot ct;ek=ot ek;da=ot da;hJ=ot hJ ke dq jX<ot dq jX)dq lb ot dq jX)od<ot dq lt i)dq nW=ot dq nW kN i=ot dq jX;i<dq lt i)dq nW lq;h5=ot h5;fL=ot fL;mf aj::fi mK af lT an ku&an!=&dx)dx=an;ct lq;ek lq;da lq;hJ lq kh kZ fy:dq)fy lq;h5 kX fL=fA.fetch_add(1,jV memory_order_relaxed mu size=an jX;ct me size);ek me size);da me size);hJ me size);jT a9=af kq cG=0,load=0 lH bA:an){ct kl b1 mX bA))ou cX=cG+lX ia(a9,bA);cG=la cX+ct.back()lQ,ct.back().ha);load+=ct.back().bz;h5+=ld a9,bA);ek kl cX);da kl cG);hJ kl load);a9=bA;}jT fb=0 kN jl=1;jl<size;jl*=2,++fb ku fb==dq jX)dq nL);kZ fy=dq[fb];fy lb size)nd jT jh=jl*2 kN he kX he<size;he+=jh kV mid=ly he+jl,size mu end=ly he+jh,size kR mid>he){fy[mid-1]=ct[mid-1]ml i=mid-1;i>he;--i)fy[i-1]=jm mX ct[i-1],fy nW kG mk mid<end){fy[mid]=ct[mid]ml i=mid+1;i<end mo i)fy nW=jm mX fy[i-1],ct nW kG}mk jl>size/2)lR}}bP aj::c4 mK lI hI lz first>hI||hI>=dx jX)throw jV out_of_range("invalid route range" kR first==hI kb ct[first]kH fy=gB(first^hI)-1 kp jm mX dq.at(fy)[first],dq.at(fy)[hI]kG bool aj::ay mK pos lz pos>dx jX kK ke pos==0 kb true kH eI=dx[pos-1],k=lX mZ kp eI<k||eI>=k+lX md;}aK::aK mK ln bk):ai(in),hg(mU,eX(bk){mf aK::cv kS lI end ku first==end kb;e2(eX.c4(ai,lM end-1)lG aK::e2 kg bP&dR){cu=cu?jm(ai,*cu,dR):dR;mf aK::eO kS bA){e2(b1(ai,bA)kG mJ aK::aX(lz!cu kb 0.0 kH&s=*cu ou cX=ai.ia(hg,s.first ng s.bc||cX>s.bv||s.dC<-1e-8||s.dB>ai.bl[hg]on kr kp ai.cQ(hg,s.ns+s.cQ;nr aK::gG(lz!cu kb 0.0 kp ai.cQ(hg,cu->ns+cu->cQ;}ka class aW mI a8 lA ez{int af=-1;jT as=0,br kX}mI ab lc ab oh,cA b4,lu a8>aN={})lD dL mR bQ)lD nb ab&bQ)noexcept mW cA b4(jZ;lF aj&bk nH jZ;lF km&bm lj av;}jY]bool b8 nH jZ kf af<aE jX&&!aE kW kB.empty(kG lY aA lj ni mZ;}lF ez&ap mA kf a4.at(aa kG lY ad lj a4 jX;}lY al lj ni md;}lY eM(jZ;mz bV lj cK;}jY]a8&aN lj*by;}lF lu a8>&ed lj by;}mz aT mA;lF au&ag lj*ai;}lY aC mA;lF nw&az lj bK;mf dz(nw du){bK=lU du kG jY]mJ aO mA mW mJ eZ kS aa kE af kE oy jZ lO f9 kS aa)lO dO nH lT am)lO insert kS aa kE af kE oy)lO cx kS aa kE oy)lO eK kS a0 kE aF kE oy)lO fH kS nI)lO cY kS bT kE e6 kE bU kE e7)lD ho(jZ;kT const au*ai;lu a8>by;lW aj>aE;km av;lW ez>a4 lA hl{ky bu kX mJ a1;}lv lW hl>cp;nw bK kq cK kX km b0;aj bb lD c2 nH lT an)lD cr kS mV aj&c3 lN a1)lD c9 kS bT,aj gc kE bU,aj gd lN a1);lY cc mA;kY a8 lc struct i3 mY*as kq*aw kq*iG kq*jq;};explicit a8 kS am):aQ(am),fV(gF(am)),hf(4*fV){}lY ad lj aQ;}lY gt lj fV-2;}lY jN lj hf jX*sizeof(double kG jY]i3 fO kS hP ku hP>gt())throw jV out_of_range("insertion route exceeds fixed scratch capacity");auto*data=hf.data()kp{data,data+fV,data+2*fV,data+3*fV};}kT static jT gF kS am){constexpr auto iu=ll<jT>nu/sizeof(double)/4 ke am>(iu-2)/2)throw jV length_error("insertion scratch size overflow")kp 2*am+2;}jT aQ,fV;jW ks hf;kY aH lc aH mK ln an):in_(in),hg(mU,iZ(an){mf c4 kS lI end ku first<end)ii(iZ.c4(in_,lM end-1)lG bA kS id){ii(b1(in_,id)lG dR kg bP&gS){ii(gS kG jY]bool bc(lz!cu kb true kH&s=*cu kp s.bc&&in_.ia(hg,s.ns<=s.bv&&s.dC>=-1e-8&&s.dB<=in_.bl[hg]on;}kT void ii kg bP&gS ku!cu){cu=gS kp;}kZ s=*cu ou fI=in_.ia(s.hI,gS.ns ou fR=s.bz ou f7=s lQ ou ht=s.ha;s.bc=s.bc&&gS.bc&&ht+fI<=gS.bv;s.bv=ly s.bv,gS.bv-fI-f7);s lQ=f7+fI+gS lQ;s.ha=la ht+fI+gS lQ,gS.ha);s.dC=ly s.dC,fR+gS.dC);s.dB=la s.dB,fR+gS.dB);s.bz=fR+gS.bz;s.hI=gS.hI;}const au&in_;jT hg nd aj&iZ;kc<bP>cu;};nh dI{const km&ax;jT gI=0,gb kX bool aP ls;lY size lj ax jX-(aP?2:0 kG lY bA kS aV lz!aP kb ax[aV]ke aV<gI kb ax[aV]ke aV<gb-1 kb ax[aV+1]kp ax[aV+2];}};oi bE mK af lT ax kE c1 kE bZ kV as=ax[c1],aw=ax[bZ]kH dA=c1?ax[c1-1]:af ke bZ==c1+1)mY hz=ld dA,as)+ld as,aw kR bZ+1<ax jX kV je=ax[bZ+1];hz+=ld aw,je)-ld dA,je lP hz;}jU fZ=ax[c1+1]kH eB=ax[bZ-1]kq hz=ld dA,as)+ld as,fZ)-ld dA,fZ)+ld eB,aw kR bZ+1<ax jX kV eV=ax[bZ+1];hz+=ld aw,eV)-ld eB,eV lP hz;}oi bS mK af kn dI&an kE at kE bA kV a9=at?an.bA(at-1):af kq ea=ld a9,bA kR at<an jX kV gS=an.bA(at);ea+=ld bA,gS)-ld a9,gS lP ea;}oi bF mK af kn dI&an kE at kE as kE aw kV a9=at?an.bA(at-1):af kq ea=ld a9,as)+ld as,aw kR at<an jX kV gS=an.bA(at);ea+=ld aw,gS)-ld a9,gS lP ea;}oi cU mK af kn dI&an kE c1 kE bZ kE as kE aw ku c1==bZ kb bF nv,c1,as,aw)kp bS nv,c1,as)+bS nv,bZ,aw kG ka nj ab lx in,cA b4,lu a8>aN):ai(&in),by(aN?lU aN):jV make_shared<a8>(lX md+lX aS jX)),aE(lX mZ),a4(lX md+lX aS jX),cp(a4 jX ku by->ad()!=a4 jX kt("insertion scratch request count mismatch");av me lX mZ mu db=b4.ep(in ng db.bc kt("invalid search state: "+db.ir);cK=db.bV kN k kX k<b4.cH lt k)mk!b4.cH[k]kL)c2(k,b4.cH[k]lG nj dL mR bQ ku this==&bQ kb ke ai!=ot ai kt("cannot copy states of different instances")kN lV aE lt mU mk aE kW nC!=ot aE kW nC)aE kW.dL(ot aE kW);av me nn);av=ot av;a4=ot a4;cp=ot cp;bK=ot bK;cK=ot cK;mf nj nb ab&bQ)noexcept{using jV swap;nb ai,ot ai);by.nb ot by);aE.nb ot aE);av.nb ot av);a4.nb ot a4);cp.nb ot cp);bK.nb ot bK);nb cK,ot cK);b0.nb ot b0);nb bb,ot bb kG cA nj b4(jZ{cA aD;aD.cH lb ni mZ)lH af:av)aD.cH kW=kO kB kp aD;}const aj&nj bk nH jZ{static const aj empty kp af<aE jX?aE kW:empty;}jT nj eM lj jV count_if(a4 og a4 kF,[](jU&ap)kf ap.af>=0;}kG double nj aT of ni ar.at kM.aT:ni aS.at(aa-n).aT;}jT nj aC of ni cL kM:ni dE(aa-n kG jT nj cc of ni aR kM:ni bd(aa-n lG nj c2 nH lT an kV&in=*ai kH n=lX md,m=lX aS jX,cC=lX mZ lH bA:kO kB kV aa=bA<cC+2*n?(bA-cC)%n:n+(bA-cC-2*n)%m;a4 mF={};}kZ iM=aE.at(mU;kz eS=b8(af kR an kL ku eS){iM lq;av lE ms,mU kG}else{iM.fi nv ng eS)av.le ms,mU,af kG mp p kX p<an lt p kV bA=an[p];kz aJ=bA<cC+2*n kH aa=aJ?(bA-cC)%n:n+(bA-cC-2*n)%m nQ p;}}mJ nj aO mA{jU loc=ap kM ke kP af<0 kr kH af=mq kP af mu&eG=kO;kZ a7=cp mF ke mw bu==eG nC kb mw a1 kH size=eG nJ;aH ae nt lh eG);ma 0,kP as);ma kP as+1,kP br);ma kP br+1,size);mw bu=eG nC;mw a1=ae.bc()?mJ(bE nt lh eG nx br)-aT kM):jV nullopt kp mw a1;}mJ nj eZ kS aa kE af kE a kE b lz aa>=ad()||lC||af>=nn kr kH&dQ=kO kH size=dQ nJ;kz aJ=aa<ni md ke a>b||b>size||(aJ&&a!=b)||!dQ.ay nt,a)||!dQ.ay nt,b)kr ke!aJ&&ni aS[aa-ni md].cJ>ni bl kW on kr;aH ae nt lh dQ);ma 0,a np bA(aC kM ng aJ)ma a,b np bA(cc kM);ma b,size ng ae.bc()kr nd dI iE{dQ kB}kH cQ=aJ?bF nt lh iE,a,aC kM,cc kM):cU nt lh iE,a,b,aC kM,cc kM)kp aT kM-cQ;mh nj f9 kS aa kV a1=aO kM ke!a1 kK kH loc=ap kM kH af=mq kP mU;oe=kO kB;mL);cK+=*a1;c2 ok)kp li mh nj dO nH lT am ku af>=nn||am kL kK kH&old=kO;nV fu(old nJ,nG kq f4=0 nY am ku aa>=ad()kK kH loc=ap kM ke kP af!=lm mU||fu[kP as]kK;fu[kP as]=fu[kP br]=li f4+=aT(aa kG aK ae nt lh old);oe lq kN first kX first<old nJ;ku fu[first]){++first;kv}auto end=first+1;mQ end<old nJ&&!fu[end])++end;ae.cv(lM end);an.le an kF,old kB kj+lM old kB kj+end);first=end;}jU hZ=ae.aX(ng hZ kK;cK+=old.cQ()-*hZ-f4;c2 ok)kp li mh nj insert kS aa kE af kE a kE b kV a1=eZ(aa lh a,b ng a1 kK;oe=kO kB ny+b,cc kM)ny+a,aC kM);cK+=*a1;c2 ok)kp li mh nj cx kS aa kE a kE b ku aa>=ad()||ap kM.af<0 kK kH af=mq ap kM.af mu g8=kO.cQ();oe=kO kB kH loc=ap kM;mL);kz aJ=aa<al(kR a>b||b>an jX||(aJ&&a!=b)kK kH ay=nB aV ku!aV kb true kH eI=an[aV-1],k=ni mZ kp eI<k||eI>=k+al(kG ke!ay(a)||!ay(b)kK ke!aJ&&ni aS[aa-al()].cJ>ni bl kW on kK ny+b,cc kM)ny+a,aC kM);bb.fi nt lh an);aK ae nt lh bb np cv(0,an jX mu cQ=ae.aX(ng cQ kK;cr(mV bb,g8-*cQ)kp li mh nj eK kS a0 kE aF kE a kE b ku a0>=ad()||aF>=ad()||ap(a0).af<0||ap(aF).af>=0 kK kH loc=ap(a0 mu af=mq kP af mu g8=kO.cQ();oe=kO kB;mL);kz aJ=aF<al(kR a>b||b>an jX||(aJ&&a!=b)kK kH ay=nB aV)kf!aV||an[aV-1]>=nn+al(kG ke!ay(a)||!ay(b)kK ny+b,cc(aF))ny+a,aC(aF));bb.fi nt lh an);aK ae nt lh bb np cv(0,an jX mu cQ=ae.aX(ng cQ kK ou a1=aT(aF)-aT(a0)+g8-*cQ ke a1<=1e-9 kK;cr(mV bb,a1)kp li mf nj cr kS mV aj&c3 lN a1 kV k=nn,n=al(),m=ni aS jX kH gU=nB bA)kf bA<k+2*n?(bA-k)%n:n+(oq n)%m;};kZ iM=aE.at(mU lH bA:iM kB)a4[gU(bA)]={};kz eS=!iM kB kL;kz gN=!c3 kB kL ke eS!=gN kV hU=ms,af kR gN)av.le hU,mU na av lE hU kG jV nb iM,c3 ng gN)iM lq kN p kX p<iM kB lt p kV bA=iM kB[p],aa=gU(bA)nQ p;}cK+=a1;mh nj fH kS nI ku first>=ad()||second>=ad()kK ke second<ns jV nb nk mu a=ap(ns,b=ap(second kR a.af<0||b.af<0||a.mP b.af kK kH gQ=nB a0 kE aF kV loc=ap(a0);auto ax=bk(kP mU kB;mM mu aZ=aF<al()?kP as:kP br-1;ax.le ax kj+aZ,cc(aF));ax.le ax kj+kP as,aC(aF))kp ax;}kH fd=gQ(nk),fe=gQ(second,ns;aj gc nt,a.mV fd),gd nt,b.mV fe);aK eC nt,a.mV gc),eD nt,b.mV gd);eC.cv(0,fd jX);eD.cv(0,fe jX mu f2=eC.aX(),f3=eD.aX(ng f2||!f3 kK ou a1=bk(a.mU.cQ()+bk(b.mU.cQ()-*f2-*f3 ke a1<=1e-9 kK;c9(a.mV lU gc),b.mV lU gd),a1)kp li mh nj cY kS bT kE e6 kE bU kE e7 ku bT==bU||bT>=nn||bU>=nn kK kH&h8=bk(bT mu&h9=bk(bU kR e6>h8 nJ||e7>h9 nJ kK lH&loc:a4 ku(kP mP lm bT)&&kP as<e6&&kP br>=e6)||(kP mP lm bU)&&kP as<e7&&kP br>=e7)kK;}km fd(h8 kB og h8 kB kj+e6);fd.le fd kF,h9 kB kj+e7,h9 kB kF);km fe(h9 kB og h9 kB kj+e7);fe.le fe kF,h8 kB kj+e6,h8 kB kF);aj gc nt,bT,fd),gd nt,bU,fe);aK eC nt,bT,gc),eD nt,bU,gd);eC.cv(0,fd jX);eD.cv(0,fe jX mu f2=eC.aX(),f3=eD.aX(ng f2||!f3 kK ou a1=h8.cQ()+h9.cQ()-*f2-*f3 ke a1<=1e-9 kK;c9(bT,lU gc),bU,lU gd),a1)kp li mf nj c9 kS bT,aj gc kE bU,aj gd lN a1){for(kZ loc:a4)mk kP mP lm bT)||kP mP lm bU))loc={}kH assign=nB mV aj c3){auto hU=ms,mU;kz gN=hU!=av kF&&*hU==af ke c3 kB kL ku gN)av lE hU);aE kW.clear(nM{mk!gN)av.le hU,mU;aE kW=lU c3 kG jU k=ni mZ,n=al(),m=ni aS jX kH&ax=kO kB kN p kX p<ax lt p kV bA=ax[p]kH aa=bA<k+2*n?(bA-k)%n:n+(oq n)%m nQ p;}};nT bT,lU gc));nT bU,lU gd));cK+=a1;mf nj ho(ob aD=b4().ep nt ng aD.bc||jV abs(aD.bV-cK)>1e-6*la 1.0,jV abs(aD.bV))kt("search-state objective or feasibility mismatch: "+aD.ir kG nh hi{jT eE=0,fU=0,fE kX}mI e1 lc e1 kS d2 kE am,bool gf);hi i1(kx rng kE eg)lD i2(hi ah lN eL);lF jW ks&jE lj fj;}lF km&jx lj gP;}oj Arm mY cJ=1,sum=0,gW kX jT count kX}lO hA;lW Arm>g2;lW lW Arm>>h3;km gv;jW ks fj;km gP;jT ip kg lW Arm>&iI,kx rng jZ;};ka e1::e1 kS d2 kE am,bool gf):hA(gf),g2(d2),h3(d2),fj(d2,0),gP(d2,0 ku d2==0)kA("BALANCE needs an operator couple")kN p=1;p<=am;p*=2){gv kl p kR p>am/2)lR}mk gv kL)gv kl 1)kh kZ iI:h3)iI lb gv jX kG jT e1::ip kg lW Arm>&iI,kx rng lz!hA)mY js=0 lH&arm:iI)js+=arm.cJ;mG jO(0,js)kq c7=jO(rng)od<iI lt i){c7-=iI nW.cJ ke c7<=0 kb i;lw iI jX-1;}constexpr double mu0=0,hH=0.01,jf=1,jw=100;jT ah=0 kq co=-kd)od<iI lt i kV&a=iI nW ou count=ki double>(a.ov ou it=hH+count ou jB=(hH*mu0+a.sum)/it ou jv=jf+count/2 ou jM=jw+0.5*(a.gW+hH*mu0*mu0-it*jB*jB);jV gamma_distribution ks jA(jv,1.0/la jM,1e-12))ou iw=jA(rng);jV normal_distribution ks jn(jB,1.0/jV sqrt(la it*iw,1e-12)))ou i0=jn(rng kR i0>co){co=i0;ah=i;}lw ah;}hi e1::i1(kx rng kE eg){hi ah;mN eE=ip(g2,rng);mN fU=ip(h3[mN eE],rng);mN fE=ly eg,gv[mN fU])kp ah;mf e1::i2(hi ah lN eL){eL=la 0.0,eL);auto gX=[eL](Arm&a){++a.count;a.sum+=eL;a.gW+=eL*eL;a.cJ=0.9*a.cJ+0.1*(0.01+eL kG;gX(g2[mN eE]);gX(h3[mN eE][mN fU]);fj[mN eE]+=eL mo gP[mN eE];}nh ib{ky jI=1 kq eh=59.0 kq cz=200.0 kq i5=jV exp(jV log(1e-4)/60000.0)lO ho ls lO bJ=li bool e5=li bool e3=li bool g9=li bool hc=li bool fg=li bool ef=li jT gr=1 lO gf ls;jV string iJ;};nh ie{cA b4 kq bV kX jT dF=0 kq iz kX bool dW ls;};ie jp oh kn ib&b6);ka class bL lc virtual~bL(lL km mn jZ kX kY fk kQ bL nN"random";mg eT kQ bL nN"related";mg fY kQ bL nN"worst";mg fX kQ bL nN"route";mg dV kQ bL nN"opportunity";mg iD lc virtual~iD(lL bool cs(kD jZ kX kY gg lc void ei(ne iD>move){fz kl lU move)kG bool gV(kD jZ;kT lW ne iD>>fz;kY cF lc virtual~cF(lL void d8(ab&lT aP,kx jZ kX};ka using dU=jV chrono::steady_clock::time_point lA ak{jT aa=0,af=0,as=0,aw=0 kq a1 kX}lA aB{lJ co kq bN=-kd kG lA ew{jT af=0 kq a1 kX}lA b5{bool aJ;bP as,aw kq aT,cJ;}mI aW lc explicit aW oh)lO gn kS aa,dU bO=dU nu jZ mW bool jt mA kf fo.at(aa kG lF lW ew>&ds mA kf eP.at(aa kG lF b5&bo mA kf ca.at(aa kG lY hx lj ca jX*sizeof(b5 kG kT const au*ai lv lW lW ew>>eP lv km gT lv nV fo;lW b5>ca;};nw c5 mR e9,dU bO);aB ec lB);lJ dh lB);lJ cf lB kE ln an lN f6=-kd))mI cV lc lJ co lB);oj gZ{ky bu=0 kq bq=-kd);lJ co;};lW lW gZ>>bR;}mI a3 lc void hM mR lT am)lD cS mR kE af lT am);lF lJ&co mA kf bR.at(aQ*d0+aa).ah;}lY iN lj bR jX*sizeof(gZ kG oj gZ{ky bi=ll<ky>nu;lJ ah;};nw iR;jT d0=0,aQ kX lW gZ>bR lD es mR kE aa kE mU lD et kS aa kG mI aI lc jY]static bool hF nl);explicit aI nl);aI nl lT am)mW aB ci mA lD cS kS mU;kT const ab&bh;using gi=jV pair<jT,aB>;lW lW gi>>cq;km aQ;kY hk lc template<class jK>lJ get kS aa kE af kE aA,ky aG,ky cI lN ck,bool dd,jK&&jQ ku bR jX<=aa)bR lb aa+1);kZ row=bR mF ke row jX<aA)row lb aA);kZ a7=row kW;kz gs=mw fW&&mw cI==cI&&mw dd==dd;kz jD=gs&&(dd?mw aG==aG:(mw mN has_value()||ck<=mw ck)ng jD){mw ah=jQ();mw aG=aG;mw cI=cI;mw ck=ck;mw dd=dd;mw fW=li}mk mw ah&&!dd){auto hW=*mw ah;hW.a1+=ck-mw ck kp hW;lw mw ah;}oj gZ{lJ ah;ky aG=0,cI=0 kq ck kX bool dd ls lO fW ls;};lW lW gZ>>bR;};nh dw{ak ah;jT bg;}mI eU lc void add(lJ ah kE bg ku ah&&ah lS>nK fz kl{*ah,bg}kG jY]kc<dw>co(lz fz kL kr kp*jV max_element(fz og fz mH mN a1<b.mN a1;}lG eY kS nI){gK(fz,[&](jU&move)kf move.bg==first||move.bg==second||move.mN mP first||move.mN mP second;}lG clear(){fz.clear(kG kT lW dw>fz;kY ao lc ao lB)mW bool fW lj gA;}jY]bool dv lj bH.has_value(kG mz cy lj bB;}lY b3 nH jZ mW mJ a1 nH kE oy jZ mW mJ a1 mi oy jZ mW mJ bj mi oy jZ mW kc<aH>eq mi bw jZ mW mJ dt mi oy kn aH&dY jZ;kT const ab&bh;jT dk,b2,dG,cR lO gA ls;mJ bH kq bB kX bP iv,hp;lY b9 kS aV jZ mW bool cj kg aj&an kE bt jZ lD nE jZ;}mI d1 kQ iD nN"parcel_relocate";mh cs(ab&oo hk eX mW lJ cw mR kn ao&kE aa kE ln,ky aG jZ;kY aq lc aq lB)mW bool fW lj gA;}jY]bool dv lj bH.has_value(kG mz cy lj bB;}lY b3 nH jZ mW mJ a1 nH kE bt jZ mW mJ a1 mi bt jZ mW mJ bj mi bt jZ;kT const ab&bh;jT dk,b2,cP lO gA ls;mJ bH kq bB kX lY b9 kS aV jZ lD nE jZ;}mI c8 kQ iD nN"passenger_relocate";mh cs(ab&oo hk eX mW lJ cw mR kn aq&kE aa kE ln,ky aG jZ;kY gL kQ iD nN"add-request";mh cs(kD oo a3 eX lv cV dM lv km aQ;kY fl kQ iD nN"remove-request";mh cs(kD kU;kY dr kQ iD nN"clean-cut-2opt-star";mh cs(kD kU;lY jx lj gP;}lY iH lj eW;}mz jb lj gC;}kT mutable jT gP=0,eW kX mutable double gC kX};nh aL{jT a0;ak bp kq a1;}mI a5 kQ iD lc explicit a5(bool hS=true):fJ(hS){}jV string_view g7(kU kf"replace-request";mh cs(ab&kU mW kc<aL>dK mR jZ;kT bool fJ mW kc<aL>dl mR jZ lA gZ{bool fW ls kq bq kX kc<aL>ah;}lA Row{ky bu kX lW gZ>hG;ky eJ kX aj er kq aO kX}lv lW Row>iL lA d7{ky b7 kX kc<aL>ah;}lA ba{aL ah;ky bu,b7;}lA cW{bool nD kn ba&jZ;};using hC=jV priority_queue<ba,lW ba>,cW>lv const au*fG=nullptr lv lW ky>cb lv nV dT lv lW lW d7>>bI lv hC dn lD cO mR kE af lT a0 lT aF jZ lD dS mR jZ lD fs(jZ lD fa nH kE aF jZ lD fn(jZ;};nh aY{jT nk kq a1;};mJ dc mR kE nI)mI cM kQ iD lc explicit cM(bool hS=true):fJ(hS){}jV string_view g7(kU kf"swap-request";mh cs(ab&kU mW kc<aY>dK mR jZ;oj d7{ky b7 kX kc<aY>ah;}lA ba{aY ah;jT bT,bU;ky iB,iC,b7;}lA cW{bool nD kn ba&jZ;};using hC=jV priority_queue<ba,lW ba>,cW>lO fJ lv const au*ai=nullptr lv lW ky>cm lv lW lW d7>>bI lv hC dn lD dS mR jZ lD fa kS bT kE bU jZ;kY ex kQ cF nN"greedy";mf d8(ab&lT,kx kU;kY ey kQ cF nN"profit";mf d8(ab&lT,kx oo a3 dN lv cV dM;kY be lc explicit be oh kE iu=200)lD hb nl)lO cs(kD,dU bO);jT bX lj el;}jT ax lj dx;mh jr lj aU;mf write(nf jZ;oj hV{km am,an kq bD kX}nd au&ai;jT iS,el=0,dx kX bool aU ls;lW jV map<km,hV>>gu;lW ky>cm lD add mR kE mV bool ft kG;ka ie jp oh kn ib&b6){using hj=jV chrono::steady_clock ke!b6.gr)kA("ls interval must be positive" mu he=hj::now(mu bO=he+lg b6.eh));jV mt19937_64 rng(b6.jI)ou gJ=b6.ef?1.0:0.0 kH gy=bO-lg ly gJ,b6.eh*0.05)));lW ne bL>>eo;eo kl kI<fk>());eo kl kI<fY>());eo kl kI<eT>()kR b6.hc)eo kl kI<fX>()kR b6.fg)eo kl kI<dV>());lW ne cF>>gw;gw kl kI<ex>()kR b6.g9)gw kl kI<ey>())lA jc{jT i6,iY;};lW jc>d2 kN d kX d<eo lt d)mp r kX r<gw lt r)d2 kl{d,r});e1 ik(d2 jX,nz md+nz aS jX,b6.gf);gg bJ;nZ fl>());nZ gL>());nZ c8>());nZ d1>());dr*iq=nullptr ke b6.e5){auto move=kI<dr>();iq=move.get();bJ.ei(lU move)kG nZ cM>());nZ a5>());cA empty;ow cH lb nz mZ);ab cg(ag,lU empty)mu gH=ly bO,hj::now()+lg ly 1.0,b6.eh*0.05))));cg.dz(c5(cg,gH));km cZ(cg.ad());jV iota(cZ og cZ kF,0);oc cZ og cZ kF,rng)nY cZ ku hj::now()>=bO)break kH ah=dh(cg,aa kR ah&&ah lS>nK cg.le mx aw kG mk b6.ho)cg.ho();ab co=cg;ab ae=cg;ne be>dH ke b6.ef){dH=kI<be>(ag);dH->hb(cg kG double cz=b6.cz ou cN=b6.cz*1e-4;jT dF kX mQ cz>cN kV gq=hj::now(kR gq>=gy)break kH gz=ik.i1(rng,cg.eM()mu eE=d2[gz.eE];ae.dL(cg mu aP=eo[eE.i6]->d8(ae,gz.fE,rng kR b6.e3){km gE kN r kX r<ae.ad()mo r)mk ae.lr<0)gE kl r);oc gE og gE kF,rng);gw[eE.iY]->d8(ae,gE,rng nM gw[eE.iY]->d8(ae,aP,rng kR b6.bJ&&dF%b6.gr==0)bJ.gV(ae kR b6.ho)ae.ho(kR dH)dH->hb(ae)ou ea=ae.bV()-cg.bV()ou eL=la 0.0,ea/la 1.0,jV abs(cg.bV())))lO ih=ea>=0 ke!ih){mG hR(0,1);ih=hR(rng)<jV exp(ea/cz kG mk ih){cg.nb ae kR cg.bV()>co.bV()+nK{co.dL(cg kG}ik.i2(gz,eL)mo dF;cz=la cN,cz*b6.i5 kG mk dH){dH->cs(co,bO kG mk b6.ho)co.ho();ie aD;aD.b4=co.b4();aD.bV=co.bV();aD.dF=dF;aD.iz=jV chrono::duration ks(hj::now()-he).count();aD.dW=cz<=cN kp aD;}ka km fk::mn jZ{km am(kJ);jV iota(am og am kF,0);oc am og am kF,rng);km aP;aP me ov nY am ku aP jX>=ov break ke kw f9 kM)aP kl aa lP aP;}ka km fY::d8(kD kE count,kx jZ{km aP;aP me ov;mQ aP jX<ov{jT ah=kJ kq bC=-kd)mt kV a1 mD aO(r kR a1&&*a1>bC){ah=r;bC=*a1;}}mk ah==kJ||!kw f9(ah))lR aP kl ah lP aP;}ka km eT::mn jZ{km eg mt)mk nU>=0)eg kl r kR eg kL kb{}kH f0=eg[rng()%eg jX]kH&in=lK kH gk mD aC(f0 mu bG=nB r)kf r<lX md?lX ar[r]oz:nX r-lX md]oz;}ou hD=bG(f0);km aP;aP me ov;mQ aP jX<ov{jT ah=kJ kq fm=kd)mt ku nU<0||!kw aO(r)no bA mD aC(r)ou ge=lX ia(gk,bA)+lX ia(bA,gk)+jV abs(hD-bG(r)kR ge<fm){fm=ge;ah=r;}}mk ah==kJ||!kw f9(ah))lR aP kl ah lP aP;}ka km fX::mn jZ{auto bl mD bm();oc bl og bl kF,rng);km aP lH af:bl ku aP jX>=ov lR km am mt)mk nU==lm mU)am kl r mu hw=count-aP jX ke am jX<=hw ku kw dO(mV am))aP.le aP kF,am og am kF nM{for(int ij kX ij<4 mo ij){oc am og am kF,rng)nd km iA(am og am kj+hw kR kw dO(mV iA)){aP.le aP kF,iA og iA kF);lR}}}lw aP;}ka km dV::mn lz!count kb{}kH&in=lK kH&du mD az();km hQ,eg;jW ks hT mt ku nU>=0){eg kl r);kv nr ff mD aT(r kR du){du->gn(r mu&ds=du->ds(r);ff=ds kL?0:la 0.0,ds.front().a1 kG mk ff>nK{hQ kl r);hT kl ff kG}mk eg kL kb{};jT f0 ke hQ kL)f0=eg[rng()%eg jX]na f0=hQ[jV discrete_distribution<jT>(hT og hT kF)(rng)]kH br=nB r)kf r<mr?lX aR(r):lX bd(r-mr kG kH bG=nB r)kf r<mr?lX ar[r]oz:nX r-mr]oz;}kH h0 mD aC(f0),iQ=br(f0);lW jV pair<double kE>>fC ml r:eg kV as mD aC(r),jH=br(r)ou ge=0.5*(lX ia(h0,as)+lX ia(as,h0)+lX ia(iQ,jH)+lX ia(jH,iQ))+jV abs(bG(f0)-bG(r));fC nL ge,r kG jV sort(fC og fC kF);mG hR(0,1);km aP;mQ!fC kL&&aP jX<count kV aV=ly fC jX-1,mq jV pow(hR(rng),3)*fC jX)mu r=fC[aV].second;fC lE fC kj+aV kR kw f9(r))aP kl r lP aP;}ka my b5 eF mK aa kV n=lX md;kz aJ=aa<n kH as=aJ?lX cL kM:lX dE(aa-n mu aw=aJ?lX aR kM:lX bd(aa-n)kp{aJ,b1 mX as),b1 mX aw),aJ?lX ar mF.aT:nX aa-n].aT,aJ?0:nX aa-n].cJ};}}aW::aW oh):ai(&ag),eP(nz md+nz aS jX),gT(eP jX,0),fo(eP jX,nG,ca(eP jX){mp aa kX aa<ca lt aa)ca mF=eF(ag,aa kG bool aW::gn kS aa,dU bO lz fo.at kM kb true kH&in=*ai kH n=lX md kH as=aa<n?lX cL kM:lX dE(aa-n mu aw=aa<n?lX aR kM:lX bd(aa-n mu aT=aa<n?lX ar mF.aT:nX aa-n].aT nd aj empty;kZ ds=eP mF kh kZ af=gT mF;af<lX bl lt af ku bO!=dU nu&&kC>=bO kK;aK ae(in lh empty np eO(as np eO(aw mu cQ=ae.aX(kR cQ)ds kl{mV aT-*cQ}kG jV sort(ds og ds mH a1>b.a1||(a.a1==b.a1&&a.af<b.af kG);fo mF=true kp li}nw c5 mR e9,dU bO){auto du=jV make_shared<aW>(e9.ag())kN aa kX aa<e9.ad()mo aa ku!du->gn(aa,bO))lR lw du;}my void fp(aB&aD,ak ah ku!nA||mN a1>nA lS||(mN a1==nA lS&&mN af<nA->mU ku nA)aD.bN=la aD.bN,nA lS);nA=ah;}else aD.bN=la aD.bN,mN a1 lG hE(aB&aD kn aB&an ku an.co)fp(aD,*an.co);aD.bN=la aD.bN,an.bN kG bool dJ(aB&aD kn kD kE aa kV&nR->gn kM ke cB){jT g3=0 lH&empty:du->ds kM ku kw b8(ow mU)kv fp(aD,{aa,ow mV 0,0,ow a1}kR++g3==2)lR}lw cB;}b5 bo lB kV&in=lK kH al=mr;kz aJ=aa<al kp{aJ,b1 mX kw aC kM),b1 mX aJ?lX aR kM:lX bd(aa-al)),kw aT kM,aJ?0:nX aa-al].cJ};}aB cT lB kE ln an kn b5&data,a8&fT lN fv=-kd),bool il ls){aB aD kH&in mD ag(kR nm cJ>lX bl kW on kb aD kH b3=an nJ kH as=nm as.lM aw=nm aw.first kH&ax=an kB kH fO=fT.fO(b3);auto*dX=fO.as;auto*di=fO.aw;auto*cn=fO.iG oa kV a9=pos?ax[pos-1]:af kH hK=pos<b3?ld a9,ax mE):0.0;dX mE=ld a9,as)-hK+(pos<b3?ld as,ax mE):0.0);di mE=ld a9,aw)-hK+(pos<b3?ld aw,ax mE):0.0);cn mE=ld a9,as)+ld as,aw)-hK+(pos<b3?ld aw,ax mE):0.0 kG jU fP=fv kq c0=kd)kq d3=kd)oa){c0=ly c0,cn mE ng nm aJ){c0=ly c0,d3+di mE);d3=ly d3,dX mE kG}mk nm aT-c0+1e-8<fP kb aD;auto*bx=fO.jq ke!nm aJ){bx[b3+1]=kd)kN pos=b3+1;pos-->0;)bx mE=ly di mE,bx[pos+1]kG mp a kX a<=b3 mo a ku!op a)no bq=[&]kf la fv,il&&nA?nA lS:aD.bN kG kH fM=nm aJ?cn[a]:ly cn[a],dX[a]+bx[a+1]kR nm aT-fM+1e-8<bq())kv aH dY nv);dY.c4(0,a);dY.dR(nm as ng dY.bc())continue ke nm aJ){auto ae=dY;ae.dR(nm aw);ma a,b3 kR ae.bc())fp(aD,{aa lh a,a,nm aT-cn[a]}nM{jU ep=nB b lN ea){auto ae=dY;ma a,b np dR(nm aw);ma b,b3 kR ae.bc())fp(aD,{aa lh a,b,nm aT-ea}kG kN b=a;b<=b3 mo b ku!op b)no ea=b==a?cn[a]:dX[a]+di[b]ke nm aT-ea+1e-8<bq())kv ep(b,ea kG}lw aD;}}lJ cf lB kE ln an lN f6 ku aa>=kJ||af>=kw nn||kw lC kr kp cT ox lh an,bo ox),kw aN(),f6,true).co;}my aB c6 lB,bool fc){aB aD ke kw lC kb aD;kz cB=dJ(aD,ac,aa mu data=bo ox);kZ fT mD aN(mu bq=[&]kf fc?aD.bN:(nA?nA lS:-kd)kG ke cB){mm mB)hE(aD,nc,fT,bq(),!fc)nM{mp lV kw nn mo mU hE(aD,nc,fT,bq(),!fc)lP aD;}}aB ec lB)kf c6 ox,true kG lJ dh lB)kf c6 ox,nG.co;}lJ cV::co lB ku kw lC kr;aB aD ke!dJ(aD,ac,aa)kb dh ox kR bR jX<=aa)bR lb aa+1);kZ row=bR mF ke row jX<kw nn)row lb kw nn mu&data mD az()->bo kM;kZ fT mD aN()lH mB kV&an mD kO;kZ a7=row kW kH bq=nA?nA lS:-kd kR mw bu!=an nC||bq<mw bq){mw co=cT ox lh an,data,fT,bq,true).co;mw bu=an nC;mw bq=bq;}mk mw co)fp(aD,*mw co lP nA;mf a3::es lB kE mU{kZ a7=os kH&an mD kO;mw mN reset(kR kw b8(mU){mw ah=cT ox lh an,kw az()->bo kM,kw aN(),1e-9,true).co ke mw ah&&mw ah lS<=nK mw mN reset(nM{mm&empty:kw az()->ds kM)mk ow mP af ku ow a1>nK mw ah=ak{aa lh 0,0,ow a1};lR}}mw bi=an.bi(lG a3::et kS aa){kZ h4=bR[aQ*d0+aa].ah;h4.reset()kN lV d0 mo af kV&ah=os.ah ke ah&&(!h4||ah lS>h4 lS))h4=ah;}mf a3::hM nl lT am kV&du mD az(ng du kt("positive insertion cache requires empty taxi index" kR iR!=du||d0!=kw nn||aQ!=kJ){iR=du;d0 mD nn;aQ=kJ ke d0==ll<jT>nu||aQ>ll<jT>nu/(d0+1))throw jV length_error("positive insertion cache size overflow");bR.nT aQ*(d0+1),gZ{}kG mm aa:am ku kw lC)kv du->gn kM kN lV d0 mo mU mk os.bi!=kw kO nC)es ox,mU;et(aa kG mf a3::cS nl kE af lT am){mm aa:am ku kw lC no a9=co kM;es ox,af mu&c3=os.ah ke a9&&a9->mP mU et kM na mk c3&&(!a9||c3 lS>a9 lS||(c3 lS==a9 lS&&af<a9->mU))bR[aQ*d0+aa].ah=c3;}mh aI::hF nl)kf kw az()&&kJ-kw eM()>=4;}aI::aI nl):bh(ac),cq(kJ){aQ lb kJ);jV iota(aQ og aQ kF,0 ng kw az()kb nY aQ ku kw lC no data=bo ox);kZ ci=cq mF;ci me kw bm()jX)lH mB)ci nL mV nc,kw aN())kG}aI::aI nl lT am):bh(ac),cq(kJ){nV hY(kJ,nG nY am ku hY.at kM||kw lC)kv hY mF=li aQ kl aa ng kw az()no data=bo ox)lH mB)cq mF nL mV nc,kw aN())kG}aB aI::ci mA{aB aD ke bh.lC kb aD ke!bh.az()kb ec(bh,aa);dJ(aD,bh,aa)lH&[mV an]:cq.at kM)hE(aD,an)kp aD;mf aI::cS nH ku!bh.az()kb kH&an=bh.kO nY aQ ku bh.lC)kv kZ ci=cq mF kH g3=jV lower_bound(ci og ci kF lh[]kg gi&a7 kE id)kf mw first<id;}mu data=bo(bh,aa);auto c3=cT(bh,aa lh an,data,bh.aN()kR g3==ci kF||g3->first!=mU ci.le g3,{mV lU c3)})na g3->second=lU c3 kG}ka void ex::d8(kD lT aP,kx jZ{mm aa:aP kV ah=dh ox kR ah&&ah lS>nK kw le mx aw kG}ka class gh kQ cF nN"regret2";mf d8(ab&lT,kx kU;};ka void gh::d8(kD lT aP,kx jZ{nV hL(kJ,nG nY aP)hL.at kM=li aI bk lZ aP);mQ true){lJ ah kq e4=-1 nY aP ku!hL mF no ci=bk.ci kM ke!ci.co||ci.co lS<=nK kv ko iy=jV isfinite(ci.bN)?ci.co lS-ci.bN:ci.co lS ke!ah||iy>e4||(iy==e4&&ci.co lS>ah lS)){ah=ci.co;e4=iy;}}mk!ah||!kw le ah->mx aw)kb;hL[ah->aa]ls;bk.cS(ah->af kG}ka void ey::d8(kD lT gE,kx jZ{kz cB=bool(kw az()kR cB)dN.hM lZ gE)kh;;){lJ ah kq is=0 nY gE ku kw lC no hu=cB?dN.co kM:dM.co ox kR hu&&hu lS>1e-9&&hu lS>is){ah=hu;is=hu lS;}}mk!ah kb ke!kw le ah->mx aw)kt("profit repair descriptor disagrees with insertion" kR cB)dN.cS lZ ah->mV gE kG}ka be::be oh kE iu):ai(ag),iS(iu),gu(nz mZ),cm(gu jX,0 ku!iu)kA("route pool limit must be positive" lG be::add nl kE mV bool ft kV&ax mD kO kB ke ax kL kb kH k mD nn,n=mr;km am kq aT=0 lH bA:ax){jT aa ke bA<k+n)aa=bA-k na mk nS ai.aS jX)aa=n+oq n na kv am kl aa);aT+=kw aT(aa kG jV sort(am og am kF mu bD=aT-kw kO.cQ(kR bD<=1e-9 kb;kZ i4=gu kW kH g3=i4.find(am kR g3!=i4 kF&&g3->second.bD>=bD kb;i4[am]=hV{am,ax,bD}ke ft&&i4 jX>iS kV jF=jV min_element(i4 og i4 mH second.bD<b.second.bD;});i4 lE jF kG mf be::hb nl ku&lK!=&ai kt("route pool instance mismatch")lH mB ku cm kW==kw kO nC)kv cm kW mD kO nC;add(ac lh true kG mf be::write(nf jZ{out<<"taxi,benefit,route\n" nO kN lV gu lt mU mm&[key,bM]:gu kW){out<<af+1<<','<<bM.bD<<",\"" od<bM.an lt i ku i)out<<' ';out<<ai.gD(bM.an nW kG out<<"\"\n";}mh be::cs(kD,dU bO ku&lK!=&ai kt("route pool instance mismatch")lH mB)add(ac lh nG;lW lW hV>>bX(gu jX);lW int>hd(gu jX,-1),cl(gu jX,-1);lW unsigned char>dj(kJ,0);km hr(kJ,0),d5;el=0 kq bW=0 kN lV gu lt mU{kZ row=bX kW lH&[key,bM]:gu kW)row kl bM);jV sort(row og row mH bD>b.bD||(a.bD==b.bD&&a.am<b.am kG);el+=row jX;lW unsigned char>hN(kJ,0)od<row lt i){for(auto r:row nW.am)hN[r]=1 lO cg=!row nW.am kL ml r:row nW.am)cg&=nU==lm af kR cg){jT count=0 mt)count+=nU==lm af kR count==row nW.am jX){cl kW=lm i);bW+=row nW.bD;}}}mp r kX r<hN lt r)hr[r]+=hN[r];}km g1(gu jX,0)kN lV bX lt mU mm&bM:bX kW)for(auto nq g1 kW+=hr[r]-1 kN lV bX lt mU mk!bX kW kL)d5 kl mU;jV stable_sort(d5 og d5 kF,[&](auto a,auto b)kf g1[a]>g1[b];}mu d9=[&]kg hV&bM){for(auto nq mk dj[r]kK kp li};dx kX aU ls kH e0=[&]kg km&go){jV fill(dj og dj kF,0)kN lV cl lt af ku cl kW<0||jV find(go og go kF,mU!=go kF)continue ml r:bX kW[cl kW].am)dj[r]=1;nr eQ kX lW int>gm(go jX,-1),fN ml af:go){fN kl cl kW kR cl kW>=0)eQ+=bX kW[cl kW].bD;nr dm=eQ;jV function<void kS lN)>ja=nB fr lN bD ku aU kb mo dx ke(dx&63)==0&&kC>=bO){aU=true kp;}mk fr==go jX ku bD>dm+nK{dm=bD;fN=gm;lw;nr io=bD kN d=fr;d<go lt d)mm&bM:bX[go[d]])mk d9(bM)){io+=bM.bD;lR}mk io<=dm+1e-9 kb kH af=go[fr]od<bX kW lt i kV&bM=bX kW nW ke!d9(bM))kv gm[fr]=lm i)ml nq dj[r]=1;ja(fr+1,bD+bM.bD)ml nq dj[r]=0 ke aU)lR}gm[fr]=-1 ke!aU)ja(fr+1,bD kG ke kC<bO)ja(0,0)na aU=true ke dm<=eQ+1e-9 kK od<go lt i)cl[go nW]=fN nW;bW+=dm-eQ kp li}lO a6=li mQ a6&&!aU){a6 ls kN a kX a<nP a)mp b=a+1;b<nP b)a6=e0({d5[a],d5[b]})||a6;}mp a kX a<nP a)mp b=a+1;b<nP b)mp c=b+1;c<nP c)e0({d5[a],d5[b],d5[c]});jW ks dy(kJ,0),gl=dy kq hn=kd);lW lW double>>er(bX jX);lW lW jT>>g0(bX jX)kN hs kX hs<192&&!aU mo hs ku kC>=bO){aU=li lR nr jk=jV accumulate(dy og dy kF,0.0);km ju(dy jX,0)ml af:d5)mY fQ kX int gm=-1 od<bX kW lt i kV&bM=bX kW nW kq value=bM.bD ml nq value-=dy[r]ke value>fQ){fQ=value;gm=lm i kG}jk+=fQ ke gm>=0)for(auto r:bX kW[gm].am)++ju[r];}mk jk<hn){hn=jk;gl=dy;nr jC=0 kN r kX r<dy lt r)mk dy[r]>0||ju[r]>1){ko hX=ki double>(ju[r])-1;jC+=hX*hX;}mk!jC||jk<=bW+1e-7)lR ko jR=0.9*(jk-bW)/jC kN r kX r<dy lt r)dy[r]=la 0.0,dy[r]+jR*(ki double>(ju[r])-1)kG dy=lU gl)ou hO=jV accumulate(dy og dy kF,0.0)ml af:d5){kZ fK=er kW lH&bM:bX kW)mY value=bM.bD ml nq value-=dy[r];fK kl value kG kZ gp=g0 kW;gp lb fK jX);jV iota(gp og gp kF,0);jV stable_sort(gp og gp kF,[&](auto a,auto b)kf fK[a]>fK[b];}kG jV fill(dj og dj kF,0);jV function<void kS lN lN)>gV=nB fr lN bD lN fS ku aU kb mo dx ke(dx&63)==0&&kC>=bO){aU=true kp;}mk fr==d5 jX ku bD>bW+nK{bW=bD;cl=hd;lw;nr io=bD kN d=fr;d<d5 lt d)mm&bM:bX[d5[d]])mk d9(bM)){io+=bM.bD;lR}mk io<=bW+1e-9 kb kq f8=bD+hO-fS kN d=fr;d<d5 lt d kV af=d5[d]kq fQ=0 ml i:g0 kW ku er kW nW<=0)break ke d9(bX kW nW)){fQ=er kW nW;lR}}f8+=fQ;}mk f8<bW-1e-7 kb kH af=d5[fr]ml i:g0 kW kV&bM=bX kW nW ke!d9(bM))kv hd kW=lm i)kq jJ=fS ml nq{dj[r]=1;jJ+=dy[r];}gV(fr+1,bD+bM.bD,jJ)ml nq dj[r]=0 ke aU)lR}hd kW=-1 ke!aU)gV(fr+1,bD,fS kG ke kC<bO)gV(0,0,0)na aU=true ke bW<=kw bV()+1e-9 kK;cA ae;ae.cH lb kw nn)kN lV cl lt mU mk cl kW>=0)ae.cH kW=bX kW[cl kW].an kH hq=ae.ep(ai ng hq.bc||jV abs(hq.bV-bW)>1e-6 kt("route pool selection disagrees with full evaluation");ab h2(ai,lU ae),kw ed());h2.dz(kw az());kw dL(h2)kp li}ka bool gg::gV(kD jZ{bool a6 ls kh int ji kX ji<2 mo ji){bool em ls lH&move:fz){em=move->cs(ac)||em;}mk!em)lR a6=li lw a6;}ka bool gL mS kz cB=bool(kw az()kR aQ jX!=kJ){aQ lb kJ);jV iota(aQ og aQ kF,0 kG mk cB)eX.hM lZ aQ)lO a6 ls kh;;){lJ co nY aQ ku kw lC no move=cB?eX.co kM:dM.co ox kR move&&move lS>1e-9&&(!co||move lS>co lS))co=move;}mk!co)break ke!kw le co->aa,co->mV co->as,co->aw)kt("add-request descriptor disagrees with mutation");a6=true ke cB)eX.cS lZ co->mV aQ lP a6;}ka bool fl mS lW kc<double>>h7(kJ)nF)h7 mF mD aO kM lO a6 ls kh;;){kc<jT>co kq bC=1e-9 nF ku h7 mF&&*h7 mF>bC){co=aa;bC=*h7 mF;}}mk!co)break kH af=mq kw ap(*co).af ng kw f9(*co)kt("remove-request descriptor disagrees with mutation");a6=li h7[*co].reset()nF)mk kw ap kM.mP lm mU)h7 mF mD aO(aa lP a6;}ka aq::aq lB):bh(ac),dk kM,b2(0),cP(0 ku aa>=mr||kw ap kM.af<0 kb kH&ol kM;b2=mq kP mU;cP=kP as;gA=li bH mD aO kM kH&an mD bk(b2);bB=bE(lK,b2,an nx br kG jT aq::b3 nH ob size=bh.kO nJ kp mP b2?size-2:size;}jT aq::b9 kS aV jZ kf aV<cP?aV:aV+2;mf aq::nE ob fx=ly end,cP kR first<fx)ma lM fx mu dP=la lM cP kR dP<end)ma dP+2,end+2 kG mJ aq::a1 nH kE bt lz af>=bh.nn kr kp a1(mV bh.kO,bt kG mJ aq::bj mi bt mO)||bt>an nJ-(mP b2?2:0)||(af!=b2&&!bH)kr nd dI iE{an kB,cP,cP+1,mP b2}kH ea=bF(bh.ag()lh iE,bt,bh.aC(dk),bh.ag().aR(dk))kp bB-ea;}mJ aq::a1 mi bt mO)||bt>an nJ-(mP b2?2:0)kr kH&in=bh.ag();mT:0 kR hy ku bt&&!op b9(bt-1)+1)kr;}else mk!bH||!op bt)kr;aH ae nv kR hy)bn(ae,0,bt)na ma 0,bt np bA(bh.aC(dk)np bA(lX aR(dk)kR hy)bn(ae,bt,ce)na ma bt,ce ng ae.bc()kr kp bj ok,bt kG lJ c8::cw nl kn aq&bY kE r kE ln an,ky aG ob bg=mq nU kR af!=bg&&!bY.dv()kr kp eX.get(r lh kw nn,aG,an nC,bY.cy(),mP bg,[&]{lJ bs kH b3=an nJ-(mP bg?2:0)kN a kX a<=b3 mo a kV fB=bY.bj ok,a ng fB||*fB mv a1=bY.a1 ok,a kR a1&&(!bs||*a1>bs lS))bs=ak{r lh a,a,*a1};lw bs;}kG bool c8 mS eU gE kH dp=nB r,bool all kE nI ku nU<0 kb;aq bY lZ r mu bg=mq nU ng bY.fW()kb kH aG mD bk(bg).bi(mu&nR->gn(r kR cB ku all){static const aj e8 lH&ah:du->ds(r)ku kw b8(mN mU no a1=bY.a1(mN mV e8,0 kR a1)gE.add(ak{r,mN mV 0,0,*a1},bg);lR}}mm mB mb else{mp lV kw nn mo af mb}kH fF=[&]{gE lq kN r kX r<mr mo r)dp(r,true,0,0 kG;fF()lO a6 ls;mQ jU aM=gE.co()kV ah=aM->ah kH bg=aM->bg kH c7=mN af;kz cD mD b8(bg);kz cE mD b8(c7);kz hy=bg==c7;kz eA=hy?kw cx(mN aa,mN as,mN aw):(kw f9(mN aa)&&kw le mN aa,c7,mN as,mN aw)ng eA kt("passenger relocation descriptor disagrees with mutation");a6=true ke kw b8(bg)!=cD||kw b8(c7)!=cE){fF();kv}gE.eY(bg,c7)kN r kX r<mr mo r ku nU<0 no hv=mq nU);dp(r,hv==bg||hv==c7,bg,c7 kG lw a6;}ka ao::ao lB):bh(ac),dk kM,b2(0),dG(0),cR(0 ku aa<mr||aa>=kJ||kw ap kM.af<0 kb kH&ol kM;b2=mq kP mU;dG=kP as;cR=kP br;gA=li bH mD aO kM;iv=b1(lK,kw aC kM);hp=b1(lK,lK.bd(aa-mr)mu&an mD bk(b2);bB=bE(lK,b2,an nx br kG jT ao::b3 nH ob size=bh.kO nJ kp mP b2?size-2:size;}jT ao::b9 kS aV lz aV<dG kb aV ke aV<cR-1 kb aV+1 kp aV+2;mh ao::cj kg aj&an kE bt jZ kf!bt||an.ay(bh.ag(),b9(bt-1)+1 lG ao::nE ob fx=ly end,dG kR first<fx)ma lM fx mu f5=la lM dG mu gR=ly end,cR-1 kR f5<gR)ma f5+1,gR+1 mu dP=la lM cR-1 kR dP<end)ma dP+2,end+2 kG mJ ao::a1 nH kE a kE b lz af>=bh.nn kr kp a1(mV bh.kO,a,b kG mJ ao::bj mi a kE b mO)||a>b||b>an nJ-(mP b2?2:0)||(af!=b2&&!bH)kr nd dI iE{an kB,dG,cR,mP b2}kH ea=cU(bh.ag()lh iE,a,b,bh.aC(dk),bh.ag().bd(dk-bh.al()))kp bB-ea;}mJ ao::a1 mi a kE b ob dY=eq ok,a)kp dY?dt ok,a,b,*dY):jV nullopt;}kc<aH>ao::eq mi a mO)kr;mT:0 kR a>ce kr kH&in=bh.ag(mu cJ=nX dk-bh.al()].cJ ke cJ>lX bl kW on kr ke hy ku!cj(an,a)kr;}else mk!bH||!op a)kr;aH ae nv kR hy)bn(ae,0,a)na ma 0,a np dR(iv ng ae.bc()kr kp ae;}mJ ao::dt mi a kE b kn aH&dY jZ{mT:0 kR a>b||b>ce kr kH&in=bh.ag(kR hy ku!cj(an,b)kr;}else mk!op b)kr;auto ae=dY ke hy)bn(ae,a,b)na ma a,b np dR(hp kR hy)bn(ae,b,ce)na ma b,ce ng ae.bc()kr kp bj ok,a,b kG lJ d1::cw nl kn ao&bY kE r kE ln an,ky aG ob bg=mq nU kR af!=bg&&!bY.dv()kr kp eX.get(r lh kw nn,aG,an nC,bY.cy(),mP bg,[&]{lJ bs kH b3=an nJ-(mP bg?2:0 mu&in=lK kH ol(r)nd dI iE{an nx br,mP bg}kH aC mD aC(r mu fq=lX bd(r-mr);jW ks fh(b3+1),en(b3+1),ej(b3+1),bx(b3+2,kd))oa){fh mE=bS(in lh iE,pos,aC);en mE=bS(in lh iE,pos,fq);ej mE=bF(in lh iE,pos,aC,fq kG mp pos=b3+1;pos-->0;)bx mE=ly en mE,bx[pos+1])kN a kX a<=b3 mo a kV fM=ly ej[a],fh[a]+bx[a+1]kR bY.cy()-fM mv dY=bY.eq ok,a ng dY)continue kN b=a;b<=b3 mo b kV ea=b==a?ej[a]:fh[a]+en[b]kH fB=bY.cy()-ea ke fB mv a1=bY.dt ok,a,b,*dY kR a1&&(!bs||*a1>bs lS))bs=ak{r lh a,b,*a1};}lw bs;}kG bool d1 mS eU gE kH dp=nB r,bool all kE nI ku nU<0 kb;ao bY lZ r mu bg=mq nU ng bY.fW()kb kH aG mD bk(bg).bi(mu&nR->gn(r kR cB ku all){static const aj e8 lH&ah:du->ds(r)ku kw b8(mN mU no a1=bY.a1(mN mV e8,0,0 kR a1)gE.add(ak{r,mN mV 0,0,*a1},bg);lR}}mm mB mb else{mp lV kw nn mo af mb}kH fF=[&]{gE lq kN r=mr;r<kJ mo r)dp(r,true,0,0 kG;fF()lO a6 ls;mQ jU aM=gE.co()kV ah=aM->ah kH bg=aM->bg kH c7=mN af;kz cD mD b8(bg);kz cE mD b8(c7);kz hy=bg==c7;kz eA=hy?kw cx(mN aa,mN as,mN aw):(kw f9(mN aa)&&kw le mN aa,c7,mN as,mN aw)ng eA kt("parcel relocation descriptor disagrees with mutation");a6=true ke kw b8(bg)!=cD||kw b8(c7)!=cE){fF();kv}gE.eY(bg,c7)kN r=mr;r<kJ mo r ku nU<0 no hv=mq nU);dp(r,hv==bg||hv==c7,bg,c7 kG lw a6;}ka my struct ig{jT af;km gO;};km ga nl kE af kV&in=lK kH&an mD kO kH mj;jT eH kX km jG{0}kN pos kX pos<an kB lt pos kV bA=an kB mE ke nS m)++eH na mk bA>=k+2*n+m)--eH ke!eH&&op pos+1))jG kl pos+1 lP jG;}mh dr mS++gP kH&in=lK lO a6 ls kh;;){lW ig>cH;cH me kw bm()jX)lH mB)cH kl{mV ga lZ mU})lO eA ls od<cH jX&&!eA mo i){mp j=i+1;j<cH jX&&!eA mo j kV bT=cH nW.mV bU=cH[j].af kH&a mD bk(bT mu&b mD bk(bU mu g5=a nJ,g6=b nJ kq bC=1e-9;jT iO=0,iP=0 lH e6:cH nW.gO){mm e7:cH[j].gO kV iW=e6?a kB[e6-1]:bT kH iX=e7?b kB[e7-1]:bU kq a1=0 ke e6<g5 kV iU=a kB[e6];a1+=ld iW,iU)-ld iX,iU kG mk e7<g6 kV iV=b kB[e7];a1+=ld iX,iV)-ld iW,iV kG mk a1<=bC+1e-8)kv aH gc mX bT,a);gc.c4(0,e6 kR e7<g6)gc.dR(b.c4 mX e7,g6-1)ng gc.bc())kv aH gd mX bU,b);gd.c4(0,e7 kR e6<g5)gd.dR(a.c4 mX e6,g5-1)ng gd.bc())kv bC=a1;iO=e6;iP=e7;}}mk bC>nK{ko jg mD bV(ng kw cY(bT,iO,bU,iP)kt("validated clean-cut exchange failed")mo eW;gC+=kw bV()-jg;a6=eA=li}}}mk!eA kb a6;}}ka kc<aL>a5::dK nl lz!fJ kb dl(ac);dS(ac);fs()kp dn kL?jV nullopt:kc(dn.top().ah lG a5::fs(jZ{mQ!dn kL kV&a7=dn.top(mu af=mw mN bp.af kH aF=mw mN bp.aa ke dT[aF]&&mw bu==cb kW&&mw b7==bI kW[aF].b7)lR dn.pop(kG mh a5::cW::nD a kn ba&b lz a.mN a1!=b.mN a1 kb a.mN a1<b.mN a1 kH key=[]kg aL&c)kf jV tuple(c.a0,c.bp.aa,c.bp.as,c.bp.aw kG kp key(a.ah)>key(b.ah lG a5::cO nl kE af lT a0 lT aF lz aF kL kb;kZ row=bI kW nY aF){++row mF.b7;row mF.mN reset(kG jU&dD mD kO;km ax nY a0 kV ol kM;kZ f1=iL mF ke f1.eJ!=dD nC){ax=dD kB;mM);f1.er.fi(lK lh ax);f1.aO=bE(lK lh dD nx br)-kw aT kM;f1.eJ=dD.bi(kG mm gx:aF){kZ aM=row[gx]ou bq=aM.ah?aM.ah lS:1e-9 kH bp=cf lZ gx lh f1.er,bq-f1.aO kR bp&&bp lS+f1.aO>bq)aM.ah=aL{aa,*bp,bp lS+f1.aO};}}mm aa:aF)fa(mV aa lG a5::fa nH kE aF ob&aM=bI kW[aF]ke aM.ah){dn.push({*aM.ah,cb kW,aM.b7}kG mf a5::dS nl ob bl mD nn,am=kJ ke fG!=&lK||cb jX!=bl||dT jX!=am){fG=&lK;cb.nT bl,ll<ky>nu);dT.nT am,nG;bI.nT bl,lW d7>(am));dn=hC{};iL lb am kG lW lW jT>>a0(bl);km aF,d4 kN aa kX aa<am mo aa kV ol kM;kz eR=kP af<0 ke eR){aF kl aa ng dT mF)d4 kl aa nM a0[kP af]kl aa);dT mF=eR;}mp lV bl mo af kV bi mD kO.bi(kR cb kW!=bi){cb kW=bi;cO(ac lh a0 kW,aF nM mk!d4 kL){cO(ac lh a0 kW,d4 kG}mk dn jX>2*bl*am+32)fn(lG a5::fn(jZ{dn=hC{}kN lV bI lt mU mp aF kX aF<dT lt aF ku dT[aF])fa(mV aF kG}kc<aL>a5::dl nl jZ{km eR mt)mk nU<0)eR kl r kR eR kL kr;iL lb kJ);kc<aL>co kq bC=1e-9;km ax;aj er kN a0 kX a0<kJ mo a0 kV ol(a0 kR kP af<0 no af=mq kP af mu&dD mD kO;kZ row=iL[a0]ke row.bu!=dD nC){row.bu=dD nC;row.hG lq;row.hG lb kJ kG ko aO=bE(lK lh dD nx br)-kw aT(a0)lO h1 ls lH aF:eR){kZ a7=row.hG[aF]ke!mw fW||(!mw ah&&bC<mw bq)ku!h1){ax=dD kB;mM);er.fi(lK lh ax);h1=li}mw fW=li mw bq=bC;mw mN reset(mu bp=cf lZ aF lh er,bC-aO kR bp&&bp lS+aO>bC)mw ah=aL{a0,*bp,bp lS+aO};}mk mw ah&&mw ah lS>bC){co=mw ah;bC=co lS;}}lw co;mh a5 mS bool a6 ls;mQ jU ah=dK(ac)ku!kw eK(ah->a0,ah->bp.aa,ah->bp.as,ah->bp.aw)kt("replacement descriptor disagrees with mutation");a6=li lw a6;}ka my bool im kg aY&a kn aY&b)kf a.a1>b.a1||(a.a1==b.a1&&jV tie(a.lM a.second)<jV tie(b.lM b.second)kG mJ dZ nl kE a0 kE aF kV ol(a0 mu af=mq kP af mu&an mD kO kH&in=lK;kz aJ=aF<mr kH aw=aJ?lX aR(aF):lX bd(aF-mr);aK ae nv np cv(0,kP as np eO(kw aC(aF)kR aJ)ae.eO(aw np cv(kP as+1,kP br ng aJ)ae.eO(aw np cv(kP br+1,an nJ)kp ae.aX(kG}mJ dc nl kE nI ku first>=kJ||second>=kJ kr ke second<ns jV nb nk mu a mD ap(ns,b mD ap(second kR a.af<0||b.af<0||a.mP b.af kr kH f2=dZ lZ nk ng f2 kr kH f3=dZ lZ second,first ng f3 kr kp kw bk(a.mU.cQ()+kw bk(b.mU.cQ()-*f2-*f3;mh cM::cW::nD a kn ba&b jZ kf im(b.ah,a.ah lG cM::fa kS bT kE bU ob&aM=bI[bT][bU]ke aM.ah)dn.push({*aM.ah,bT,bU,cm[bT],cm[bU],aM.b7}lG cM::dS nl ob bl mD aA(kR ai!=&lK||cm jX!=bl){ai=&lK;cm.nT bl,ll<ky>nu);bI.nT bl,lW d7>(bl));dn=hC{};}km a6;nV jj(bl,nG kN lV bl mo af kV bi mD kO.bi(kR cm kW!=bi){cm kW=bi;a6 kl mU;jj kW=li}}mk a6 kL kb;lW lW jT>>am(bl)nF kV af mD ap kM.af ke af>=0)am kW kl aa kG mm af:a6){mp bQ kX bQ<bl mo bQ ku bQ==af||(jj[bQ]&&bQ<mU no bT=ly mV bQ),bU=la mV bQ);kZ aM=bI[bT][bU]mo aM.b7;aM.mN reset()lH first:am[bT]){mm second:am[bU]kV a1=dc lZ nk ng a1||*a1<=nK kv const aY ah{ly nk),la nk),*a1}ke!aM.ah||im(ah,*aM.ah))aM.ah=ah;}}fa(bT,bU kG}mk dn jX>2*bl*bl+32){dn=hC{}kN a kX a<bl mo a)mp b=a+1;b<bl mo b)fa(a,b kG}kc<aY>cM::dK nl lz!fJ){kc<aY>co kN a kX a<kJ mo a ku kw ap(a).af<0)continue kN b=a+1;b<kJ mo b kV a1=dc lZ a,b ng a1||*a1<=nK kv const aY ah{a,b,*a1}ke!co||im(ah,*co))co=ah;}lw co;}dS(ac);mQ!dn kL kV&a7=dn.top(kR mw iB==cm[mw bT]&&mw iC==cm[mw bU]&&mw b7==bI[mw bT][mw bU].b7 kb mw ah;dn.pop(lP jV nullopt;mh cM mS bool a6 ls;mQ jU ah=dK(ac)ku!kw fH(ah->lM ah->second)kt("request swap descriptor disagrees with mutation");a6=li lw a6;}}int main(kV i8=kC;try{jV ios::sync_with_stdio(nG;jV cin.tie(nullptr mu ag=a2::fD(jV cin);a2::dg::ib b6;b6.jI=1ULL ou i7=jV chrono::duration ks(kC-i8).count()ou gj=300 ou reserve=gj>2?1.0:gj*0.1;b6.eh=gj-i7-reserve ke b6.eh<=0){a2::cA empty;ow cH lb nz mZ);a2::d6(jV cout,empty,ag nM{jU aD=a2::dg::jp(ag,b6);a2::d6(jV cout,aD.b4,ag);jV cerr nO<<"iterations="<<aD.dF<<" seconds="<<aD.iz<<" stop="<<(aD.dW?"temperature":"time_limit")<<'\n';lw 0;}catch kg jV exception&ir){jV cerr<<"error: "<<ir.what()<<'\n' kp 1;}}
