// Standard input: official instance. Standard output: profit and one route per taxi.
// HSOL replaces CPLEX as the LP backend for our MIP branch-and-bound solver.

// HSOL by Qi Huangfu and Julian Hall. Upstream: https://github.com/jajhall/hsol
/*
MIT License

Copyright (c) 2018 Julian Hall

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#include <bits/stdc++.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>

namespace hsol_embed {

struct DeadlineReached {};
inline std::chrono::steady_clock::time_point caller_deadline =
    std::chrono::steady_clock::time_point::max();
inline void check_deadline() {
    if (std::chrono::steady_clock::now() >= caller_deadline)
        throw DeadlineReached{};
}

// Upstream file: HDual.cpp

// Upstream file: HDual.h
#ifndef HDUAL_H_
#define HDUAL_H_

// Upstream file: HModel.h
#ifndef HMODEL_H_
#define HMODEL_H_

// Upstream file: HMatrix.h
#ifndef HMATRIX_H_
#define HMATRIX_H_

// Upstream file: HVector.h
#ifndef HVECTOR_H_
#define HVECTOR_H_

using namespace std;

class HVector {
  public:
    // Main part
    void setup(int size_);
    void clear();
    int size;
    int count;            // count of non zeros
    vector<int> index;    // index of non zeros
    vector<double> array; // array
    int pseudoTick;
    double fakeTick;

    // For update
    vector<char> cwork; // char working buffer
    vector<int> iwork;  // integer working buffer
    HVector *next;

    // Package
    void tight();
    void pack();
    bool packFlag;            // pack flag: do pack or not
    int packCount;            // pack count
    vector<int> packIndex;    // pack index
    vector<double> packValue; // pack value

    // Advanced
    void copy(const HVector *from);
    double norm2();
    void saxpy(const double pivotX, const HVector *pivot);
};

typedef HVector *HVector_ptr;

#endif /* HVECTOR_H_ */

using namespace std;

class HMatrix {
  public:
    const int *getAstart() const {
        return &Astart[0];
    }
    const int *getAindex() const {
        return &Aindex[0];
    }
    const double *getAvalue() const {
        return &Avalue[0];
    }
    void setup(int numCol, int numRow, const int *Astart, const int *Aindex, const double *Avalue);
    void price_by_col(HVector &row_ap, HVector &row_ep) const;
    void price_by_row(HVector &row_ap, HVector &row_ep) const;
    void update(int columnIn, int columnOut);
    double compute_dot(HVector &vector, int iCol) const;
    void collect_aj(HVector &vector, int iCol, double multi) const;

    void compute_vecT_matB(const double *vec, const int *base, HVector *res);
    void compute_matB_vec(const double *vec, const int *base, HVector *res);

  private:
    int numCol;
    int numRow;
    vector<int> Astart;
    vector<int> Aindex;
    vector<double> Avalue;

    vector<int> ARstart;
    vector<int> AR_Nend;
    vector<int> ARindex;
    vector<double> ARvalue;
};

#endif /* HMATRIX_H_ */

// Upstream file: HFactor.h
#ifndef HFACTOR_H_
#define HFACTOR_H_

using namespace std;

enum UPDATE_METHOD {
    UPDATE_METHOD_FT = 1,
    UPDATE_METHOD_PF = 2,
    UPDATE_METHOD_MPF = 3,
    UPDATE_METHOD_APF = 4
};

class HFactor {
  public:
    void copyFrom(const HFactor *from);
    void setup(int numCol, int numRow, int *Astart, int *Aindex, double *Avalue, int *baseIndex,
               int updateMethod = UPDATE_METHOD_FT);
    void change(int updateMethod);
    void build();
    void ftran(HVector &vector, double hist_dsty) const;
    void btran(HVector &vector, double hist_dsty) const;
    void update(HVector *aq, HVector *ep, int *iRow, int *hint);
    int pseudoTick;
    int BtotalX;
    int FtotalX;

    double realTick;
    double fakeTick;

  private:
    /**
     * Data of the factor
     */

    // Problem size, coefficient matrix and update method
    int numRow;
    int numCol;
    const int *Astart;
    const int *Aindex;
    const double *Avalue;
    int *baseIndex;
    int updateMethod;

    // Count of elements

    // Working buffer
    int nwork;
    vector<int> iwork;
    vector<double> dwork;

    // Basis matrix
    vector<int> Bstart;
    vector<int> Bindex;
    vector<double> Bvalue;

    // Permutation
    vector<int> permute;

    // Kernel matrix
    vector<int> MCstart;
    vector<int> MCcountA;
    vector<int> MCcountN;
    vector<int> MCspace;
    vector<int> MCindex;
    vector<double> MCvalue;
    vector<double> MCminpivot;

    // Row wise kernel matrix
    vector<int> MRstart;
    vector<int> MRcount;
    vector<int> MRspace;
    vector<int> MRcountb4;
    vector<int> MRindex;

    // Kernel column buffer
    vector<int> McolumnIndex;
    vector<char> McolumnMark;
    vector<double> McolumnArray;

    // Count link list
    vector<int> clinkFirst;
    vector<int> clinkNext;
    vector<int> clinkLast;

    vector<int> rlinkFirst;
    vector<int> rlinkNext;
    vector<int> rlinkLast;

    // Factor L
    vector<int> LpivotLookup;
    vector<int> LpivotIndex;

    vector<int> Lstart;
    vector<int> Lindex;
    vector<double> Lvalue;
    vector<int> LRstart;
    vector<int> LRindex;
    vector<double> LRvalue;

    // Factor U
    vector<int> UpivotLookup;
    vector<int> UpivotIndex;
    vector<double> UpivotValue;

    int UmeritX;
    int UtotalX;
    vector<int> Ustart;
    vector<int> Ulastp;
    vector<int> Uindex;
    vector<double> Uvalue;
    vector<int> URstart;
    vector<int> URlastp;
    vector<int> URspace;
    vector<int> URindex;
    vector<double> URvalue;

    // Update buffer
    vector<double> PFpivotValue;
    vector<int> PFpivotIndex;
    vector<int> PFstart;
    vector<int> PFindex;
    vector<double> PFvalue;

    // Implementation
    void buildSimple();
    void buildKernel();
    void buildFinish();

    void ftranL(HVector &vector, double hist_dsty) const;
    void btranL(HVector &vector, double hist_dsty) const;
    void ftranU(HVector &vector, double hist_dsty) const;
    void btranU(HVector &vector, double hist_dsty) const;

    void ftranFT(HVector &vector) const;
    void btranFT(HVector &vector) const;
    void ftranPF(HVector &vector) const;
    void btranPF(HVector &vector) const;
    void ftranMPF(HVector &vector) const;
    void btranMPF(HVector &vector) const;
    void ftranAPF(HVector &vector) const;
    void btranAPF(HVector &vector) const;

    void updateCFT(HVector *aq, HVector *ep, int *iRow, int *hint);
    void updateFT(HVector *aq, HVector *ep, int iRow, int *hint);
    void updatePF(HVector *aq, HVector *ep, int iRow, int *hint);
    void updateMPF(HVector *aq, HVector *ep, int iRow, int *hint);
    void updateAPF(HVector *aq, HVector *ep, int iRow, int *hint);

    /**
     * Local in-line functions
     */
    void colInsert(const int iCol, const int iRow, const double value) {
        const int iput = MCstart[iCol] + MCcountA[iCol]++;
        MCindex[iput] = iRow;
        MCvalue[iput] = value;
    }
    void colStoreN(const int iCol, const int iRow, const double value) {
        const int iput = MCstart[iCol] + MCspace[iCol] - (++MCcountN[iCol]);
        MCindex[iput] = iRow;
        MCvalue[iput] = value;
    }
    void colFixMax(const int iCol) {
        double maxValue = 0;
        for (int k = MCstart[iCol]; k < MCstart[iCol] + MCcountA[iCol]; k++)
            maxValue = max(maxValue, fabs(MCvalue[k]));
        MCminpivot[iCol] = maxValue * 0.1;
    }

    double colDelete(const int iCol, const int iRow) {
        int idel = MCstart[iCol];
        int imov = idel + (--MCcountA[iCol]);
        while (MCindex[idel] != iRow)
            idel++;
        double pivotX = MCvalue[idel];
        MCindex[idel] = MCindex[imov];
        MCvalue[idel] = MCvalue[imov];
        return pivotX;
    }

    void rowInsert(const int iCol, const int iRow) {
        int iput = MRstart[iRow] + MRcount[iRow]++;
        MRindex[iput] = iCol;
    }

    void rowDelete(const int iCol, const int iRow) {
        int idel = MRstart[iRow];
        int imov = idel + (--MRcount[iRow]);
        while (MRindex[idel] != iCol)
            idel++;
        MRindex[idel] = MRindex[imov];
    }

    void clinkAdd(const int index, const int count) {
        const int mover = clinkFirst[count];
        clinkLast[index] = -2 - count;
        clinkNext[index] = mover;
        clinkFirst[count] = index;
        if (mover >= 0)
            clinkLast[mover] = index;
    }

    void clinkDel(const int index) {
        const int xlast = clinkLast[index];
        const int xnext = clinkNext[index];
        if (xlast >= 0)
            clinkNext[xlast] = xnext;
        else
            clinkFirst[-xlast - 2] = xnext;
        if (xnext >= 0)
            clinkLast[xnext] = xlast;
    }

    void rlinkAdd(const int index, const int count) {
        const int mover = rlinkFirst[count];
        rlinkLast[index] = -2 - count;
        rlinkNext[index] = mover;
        rlinkFirst[count] = index;
        if (mover >= 0)
            rlinkLast[mover] = index;
    }

    void rlinkDel(const int index) {
        const int xlast = rlinkLast[index];
        const int xnext = rlinkNext[index];
        if (xlast >= 0)
            rlinkNext[xlast] = xnext;
        else
            rlinkFirst[-xlast - 2] = xnext;
        if (xnext >= 0)
            rlinkLast[xnext] = xlast;
    }
};

#endif /* HFACTOR_H_ */

// Upstream file: HRandom.h
#ifndef HRANDOM_H_
#define HRANDOM_H_

class HRandom {
  public:
    HRandom() {
        random_mw = 1985;
        random_mz = 2012;
    }
    int intRandom() {
        random_mz = 36969 * (random_mz & 65535) + (random_mz >> 16);
        random_mw = 18000 * (random_mw & 65535) + (random_mw >> 16);
        unsigned result = (random_mz << 16) + random_mw;
        return result >> 1;
    }
    double dblRandom() {
        random_mz = 36969 * (random_mz & 65535) + (random_mz >> 16);
        random_mw = 18000 * (random_mw & 65535) + (random_mw >> 16);
        unsigned result = (random_mz << 16) + random_mw;
        return (result + 1.0) * 2.328306435454494e-10;
    }

  private:
    unsigned random_mw;
    unsigned random_mz;
};

#endif /* HRANDOM_H_ */

// Upstream file: HTimer.h
#ifndef HTIMER_H_
#define HTIMER_H_

using namespace std;

enum HTickItem {
    // Predefined groups
    HTICK_GROUP1 = 0,
    HTICK_GROUP2,
    HTICK_GROUP3,
    HTICK_GROUP4,
    HTICK_GROUP5,
    HTICK_GROUP6,
    HTICK_GROUP7,
    HTICK_GROUP8,
    HTICK_GROUP9,

    // Simplex method related items
    HTICK_INVERT,
    HTICK_CHUZR1,
    HTICK_CHUZC0,
    HTICK_CHUZC1,
    HTICK_CHUZC2,
    HTICK_CHUZC3,
    HTICK_CHUZC4,
    HTICK_FTRAN,
    HTICK_BTRAN,
    HTICK_PRICE,
    HTICK_FTRAN_DSE,
    HTICK_FTRAN_MIX,
    HTICK_UPDATE_DUAL,
    HTICK_UPDATE_PRIMAL,
    HTICK_UPDATE_WEIGHT,
    HTICK_UPDATE_FACTOR,
    HTICK_UPDATE_ROW_EP,

    // The total count
    HTICK_ITEMS_COUNT
};

class HTimer {
  public:
    HTimer() {
        itemNames[HTICK_GROUP1] = "GP1";
        itemNames[HTICK_GROUP2] = "GP2";
        itemNames[HTICK_GROUP3] = "GP3";
        itemNames[HTICK_GROUP4] = "GP4";
        itemNames[HTICK_GROUP5] = "GP5";
        itemNames[HTICK_GROUP6] = "GP6";
        itemNames[HTICK_GROUP7] = "GP7";
        itemNames[HTICK_GROUP8] = "GP8";
        itemNames[HTICK_GROUP9] = "GP9";

        itemNames[HTICK_INVERT] = "INV";
        itemNames[HTICK_CHUZR1] = "CR1";
        itemNames[HTICK_CHUZC0] = "CC0";
        itemNames[HTICK_CHUZC1] = "CC1";
        itemNames[HTICK_CHUZC2] = "CC2";
        itemNames[HTICK_CHUZC3] = "CC3";
        itemNames[HTICK_CHUZC4] = "CC4";
        itemNames[HTICK_FTRAN] = "COL";
        itemNames[HTICK_BTRAN] = "REP";
        itemNames[HTICK_PRICE] = "RAP";
        itemNames[HTICK_FTRAN_DSE] = "DSE";
        itemNames[HTICK_FTRAN_MIX] = "MIX";
        itemNames[HTICK_UPDATE_DUAL] = "UPD";
        itemNames[HTICK_UPDATE_PRIMAL] = "UPP";
        itemNames[HTICK_UPDATE_WEIGHT] = "UPW";
        itemNames[HTICK_UPDATE_FACTOR] = "UPF";
        itemNames[HTICK_UPDATE_ROW_EP] = "UPR";

        reset();
    }

    // Reset the timer
    void reset() {
        for (int i = 0; i < HTICK_ITEMS_COUNT; i++)
            itemTicks[i] = 0;
        startTime = getWallTime();
        startTick = getWallTick();
    }

    // Record tick of one item
    void recordStart(int itemKey) {
        itemStart[itemKey] = getWallTick();
    }
    void recordFinish(int itemKey) {
        itemTicks[itemKey] += (getWallTick() - itemStart[itemKey]);
    }

    // Report specific items in the list
    void report(int itemCount, int *itemList) {
        double totalTick = getTick();
        fprintf(stderr, "profile-name ");
        for (int i = 0; i < itemCount; i++)
            fprintf(stderr, " %s", itemNames[itemList[i]].c_str());
        fprintf(stderr, "\n");
        fprintf(stderr, "profile-item ");
        for (int i = 0; i < itemCount; i++) {
            int percent = 1000.0 * itemTicks[itemList[i]] / totalTick;
            fprintf(stderr, " %3d", percent);
        }
        fprintf(stderr, "\n");
    }

    // Current eclipsed time
    double getTime() {
        return getWallTime() - startTime;
    }

    // Current eclipsed CPU tick
    double getTick() {
        return getWallTick() - startTick;
    }

  private:
    // The start tick and time
    double startTime;
    double startTick;

    // The items tick and name
    double itemStart[HTICK_ITEMS_COUNT];
    double itemTicks[HTICK_ITEMS_COUNT];
    string itemNames[HTICK_ITEMS_COUNT];

    // Current wall time
    double getWallTime() {
        double walltime;
        struct timeval tv;
        gettimeofday(&tv, NULL);
        walltime = tv.tv_sec;
        walltime += (double)tv.tv_usec / 1000000.0;
        return walltime;
    }

    // Current wall tick
    double getWallTick() {
        unsigned a, d;
        asm volatile("rdtsc" : "=a"(a), "=d"(d));
        return ((unsigned long long)a) | (((unsigned long long)d) << 32);
    }
};

#endif /* HTIMER_H_ */

using namespace std;

const int LP_Status_Unset = -1;
const int LP_Status_Optimal = 0;
const int LP_Status_Infeasible = 1;
const int LP_Status_Unbounded = 2;
const int LP_Status_Singular = 3;
const int LP_Status_Failed = 4;

enum HSOL_INT_OPTIONS {
    INTOPT_PRINT_FLAG = 0, // 0/1 = none/do-print
    INTOPT_TRANSPOSE_FLAG, // 0/1 = none/do-transpose if possible
    INTOPT_SCALE_FLAG,     // 0/1 = none/do-scale
    INTOPT_TIGHT_FLAG,     // 0/1 = none/do-tight
    INTOPT_PERMUTE_FLAG,   // 0/1 = none/do-permute
    INTOPT_PERTURB_FLAG,   // 0/1 = none/do-perturb
    INTOPT_COUNT
};

enum HSOL_DBL_OPTIONS {
    DBLOPT_TIME_LIMIT = 0,
    DBLOPT_PRIMAL_TOL,
    DBLOPT_DUAL_TOL,
    DBLOPT_PERTURB_BASE,
    DBLOPT_PAMI_CUTOFF,
    DBLOPT_COUNT
};

enum HSOL_STR_OPTIONS {
    STROPT_PARTITION_FILE = 0, // name of row partition file
    STROPT_COUNT
};

class HModel {
  public:
    HModel();
    void setup(const char *filename);
    void setup_loadMPS(const char *filename);
    void setup_transposeLP();
    void setup_scaleMatrix();
    void setup_tightenBound();
    void setup_shuffleColumn();
    void setup_allocWorking();

    int getNumRow() {
        return numRow;
    }
    int getNumCol() {
        return numCol;
    }
    int getNumTot() {
        return numTot;
    }
    const HMatrix *getMatrix() {
        return &matrix;
    }
    const HFactor *getFactor() {
        return &factor;
    }
    int *getBaseIndex() {
        return &basicIndex[0];
    }
    int *getNonbasicFlag() {
        return &nonbasicFlag[0];
    }
    int *getNonbasicMove() {
        return &nonbasicMove[0];
    }
    int *getWorkIntBreak() {
        return &intBreak[0];
    }

    double *getWorkDual() {
        return &workDual[0];
    }
    double *getWorkShift() {
        return &workShift[0];
    }
    double *getWorkLower() {
        return &workLower[0];
    }
    double *getWorkUpper() {
        return &workUpper[0];
    }
    double *getWorkRange() {
        return &workRange[0];
    }
    double *getWorkValue() {
        return &workValue[0];
    }
    double *getBaseLower() {
        return &baseLower[0];
    }
    double *getBaseUpper() {
        return &baseUpper[0];
    }
    double *getBaseValue() {
        return &baseValue[0];
    }

    void initCost(int perturb = 0);
    void initBound(int phase = 2);
    void initValue();

    void computeFactor();
    void computeDual();
    void computeDualInfeasInDual(int *dualInfeasCount);
    void computeDualInfeasInPrimal(int *dualInfeasCount);
    void correctDual(int *freeInfeasCount);
    void computePrimal();
    void computeObject(int phase = 2);

    void shiftCost(int iCol, double amount);
    void shiftBack(int iCol);
    void flipBound(int iCol);

    void updateFactor(HVector *column, HVector *row_ep, int *iRow, int *hint);
    void updateMatrix(int columnIn, int columnOut);
    void updatePivots(int columnIn, int rowOut, int sourceOut);

    void changeUpdate(int updateMethod);

    void reportPivots(int columnIn, int columnOut, double alpha);
    void reportStatus(int status);

    void printMessage(const char *message);
    void printObject();
    void printResult();
    void printProgress();
    void writePivots(const char *suffix);
    void writeMPS(const char *filename);

    // Solving options
    int intOption[INTOPT_COUNT];
    double dblOption[DBLOPT_COUNT];
    string strOption[STROPT_COUNT];

    // Random generator
    HRandom random;

    // The time and timer
    HTimer timer;
    double totalTime;

    // Perturbation flag
    int problemPerturbed;

    // The original models
    string modelName;

    // Solving result
    int limitUpdate;
    int countUpdate;
    int numberIteration;
    double objective;

    //private:
    // The original model
    int numCol;
    int numRow;
    int numTot;
    vector<int> Astart;
    vector<int> Aindex;
    vector<double> Avalue;
    vector<double> colCost;
    vector<double> colLower;
    vector<double> colUpper;
    vector<double> rowLower;
    vector<double> rowUpper;
    double objOffset;

    // Associated data of original model
    vector<int> workRowPart; // Row partition
    vector<int> intBreak;
    vector<double> dblXpert;

    // Working model
    int problemStatus;
    vector<int> basicIndex;
    vector<int> nonbasicFlag;
    vector<int> nonbasicMove;
    HMatrix matrix;
    HFactor factor;
    HVector buffer;
    HVector bufferLong;

    vector<double> workCost;
    vector<double> workDual;
    vector<double> workShift;

    vector<double> workLower;
    vector<double> workUpper;
    vector<double> workRange;
    vector<double> workValue;

    vector<double> baseLower;
    vector<double> baseUpper;
    vector<double> baseValue;

    vector<int> historyColumnIn;
    vector<int> historyColumnOut;
    vector<double> historyAlpha;
};

#endif /* HMODEL_H_ */

// Upstream file: HDualRow.h
#ifndef HDUALROW_H_
#define HDUALROW_H_

using namespace std;

/**
 * This class deal with ratio test
 * and some update dual/flip tasks.
 */

class HDualRow {
  public:
    void setup(HModel *model);
    void setupSlice(HModel *model, int size);
    void clear();
    void choose_makepack(const HVector *row, const int offset);
    void choose_possible();
    void choose_joinpack(const HDualRow *otherRow);
    void choose_final();

    void update_flip(HVector *bfrtColumn);
    void update_dual(double theta);

    void create_Freelist();
    void create_Freemove(HVector *row_ep);
    void delete_Freemove();
    void delete_Freelist(int iColumn);

    HModel *workModel;
    int workSize;
    const int *workRand;
    const int *workMove;
    const double *workDual;
    const double *workRange;

    set<int> freeList;

    int packCount;
    vector<int> packIndex;
    vector<double> packValue;

    double workDelta;
    double workAlpha;
    double workTheta;
    int workPivot;
    int workCount;

    vector<pair<int, double>> workData;
    vector<int> workGroup;
};

#endif /* HDUALROW_H_ */

// Upstream file: HDualRHS.h
#ifndef HDUALRHS_H_
#define HDUALRHS_H_

using namespace std;

/**
 * This class deal with optimality test
 * and some update primal/weight tasks
 */

class HDualRHS {
  public:
    void setup(HModel *model);
    void setup_partition(const char *filename);

    void choose_normal(int *chIndex);
    void choose_multi_global(int *chIndex, int *chCount, int chLimit);
    void choose_multi_HGauto(int *chIndex, int *chCount, int chLimit);
    void choose_multi_HGpart(int *chIndex, int *chCount, int chLimit);

    void update_primal(HVector *column, double theta);
    void update_weight(HVector *column, double devex, double Kai, double *dse);
    void update_pivots(int iRow, double value);

    void update_infeasList(HVector *column);
    void create_infeasList(double columnDensity);
    void create_infeasArray();

    HModel *workModel;
    double workCutoff;
    int workCount;
    vector<char> workMark;
    vector<int> workIndex;
    vector<double> workArray;
    vector<double> workDevex;
    vector<double> workDevexFull;

    int partNum;
    int partNumRow;
    int partNumCol;
    int partNumCut;
    int partSwitch;
    vector<int> workPartition;
};

#endif /* HDUALRHS_H_ */

using namespace std;

const int HSOL_THREAD_LIMIT = 32;
const int HSOL_SLICED_LIMIT = 100;

enum HDUAL_VARIANT {
    HDUAL_VARIANT_PLAIN = 0,
    HDUAL_VARIANT_TASKS,
    HDUAL_VARIANT_MULTI,
};

class HDual {
  public:
    void solve(HModel *model, int variant = 0, int num_threads = 1);

  public:
    void init(int num_threads);
    void init_slice(int init_sliced_num);

    void solve_phase1();
    void solve_phase2();

    void rebuild();
    void cleanup();

    void iterate();
    void iterate_tasks();
    void iterate_multi();

    void chooseRow();

    void chooseColumn(HVector *row_ep);
    void chooseColumn_slice(HVector *row_ep);

    void updateFtranBFRT();
    void updateFtran();
    void updateFtranDSE(HVector *dseVector);
    void updateVerify();
    void updateDual();
    void updatePrimal(HVector *dseVector);
    void updatePivots();

    void major_chooseRow();
    void major_chooseRowBtran();
    void minor_chooseRow();

    void minor_update();
    void minor_updateDual();
    void minor_updatePrimal();
    void minor_updatePivots();
    void minor_updateRows();

    void major_update();
    void major_updateFtranPrepare();
    void major_updateFtranParallel();
    void major_updateFtranFinal();
    void major_updatePrimal();
    void major_updateFactor();

    void major_rollback();

    // Variant choice
    int dual_variant;

    // Model
    HModel *model;
    double Tp; // Tolerance for primal
    double Td; // Tolerance for dual

    int numCol;
    int numRow;
    int numTot;
    const HMatrix *matrix;
    const HFactor *factor;

    const int *jMove;
    const double *workRange;
    const double *baseLower;
    const double *baseUpper;
    double *baseValue;
    double *workDual;

    int solvePhase;
    int invertHint;

    HVector row_ep;
    HVector row_ap;
    HVector column;
    HVector columnBFRT;
    HVector columnDSE;
    double row_epDensity;
    double rowdseDensity;
    double columnDensity;

    HDualRow dualRow;

    // Solving related buffers
    int dualInfeasCount;

    HDualRHS dualRHS;

    // Simplex pivotal information
    int rowOut;
    int columnOut;
    int sourceOut; // -1 from small to lower, +1 to upper
    int columnIn;
    double deltaPrimal;
    double thetaDual;
    double thetaPrimal;
    double alpha;
    double alphaRow;

    // Partitioned coefficient matrix
    int slice_num;
    int slice_PRICE;
    int slice_start[HSOL_SLICED_LIMIT + 1];
    HMatrix slice_matrix[HSOL_SLICED_LIMIT];
    HVector slice_row_ap[HSOL_SLICED_LIMIT];
    HDualRow slice_dualRow[HSOL_SLICED_LIMIT];

    // Multiple price data
    struct MChoice {
        int rowOut;
        double baseValue;
        double baseLower;
        double baseUpper;
        double infeasValue;
        double infeasDevex;
        double infeasLimit;
        HVector row_ep;
        HVector column;
        HVector columnBFRT;
    };

    struct MFinish {
        int moveIn;
        double shiftOut;
        vector<int> flipList;

        int rowOut;
        int columnOut;
        int columnIn;
        double alphaRow;
        double thetaPrimal;
        double basicBound;
        double basicValue;
        double devex;
        HVector_ptr row_ep;
        HVector_ptr column;
        HVector_ptr columnBFRT;
    };

    int multi_num;
    int multi_iChoice;
    int multi_nFinish;
    int multi_iteration;
    int multi_chooseAgain;
    MChoice multi_choice[HSOL_THREAD_LIMIT];
    MFinish multi_finish[HSOL_THREAD_LIMIT];

    double total_fake;
    double total_INVERT_TICK;
    double total_FT_inc_TICK;
};

#endif /* HDUAL_H_ */

// Upstream file: HConst.h
#ifndef HCONST_H_
#define HCONST_H_

const double HSOL_CONST_INF = 1e200;
const double HSOL_CONST_TINY = 1e-14;
const double HSOL_CONST_ZERO = 1e-50;

#endif /* HCONST_H_ */

// Upstream file: HPrimal.h
#ifndef HPRIMAL_H_
#define HPRIMAL_H_

class HPrimal {
  public:
    void solvePhase2(HModel *ptr_model);

  private:
    void primalRebuild();
    void primalChooseColumn();
    void primalChooseRow();
    void primalUpdate();

    // Model pointer
    HModel *model;
    int numCol;
    int numRow;
    int numTot;

    // Pivot related
    int limitUpdate;
    int countUpdate;
    int invertHint;
    int columnIn;
    int rowOut;

    // Solve buffer
    HVector row_ep;
    HVector row_ap;
    HVector column;
    double row_epDensity;
    double columnDensity;
};

#endif /* HPRIMAL_H_ */

using namespace std;

void HDual::solve(HModel *ptr_model, int variant, int num_threads) {
    dual_variant = variant;
    model = ptr_model;
    if (model->getNumRow() == 0)
        return;
    model->timer.reset();

    // Initialise working environment
    init(num_threads);

    model->initCost(1);
    model->computeFactor();
    model->computeDual();
    model->computeDualInfeasInDual(&dualInfeasCount);
    solvePhase = dualInfeasCount > 0 ? 1 : 2;

    // Find largest dual and adjust the dual tolerance accordingly
    double largeDual = 0;
    for (int i = 0; i < numTot; i++) {
        if (model->getNonbasicFlag()[i]) {
            double myDual = fabs(workDual[i] * jMove[i]);
            if (largeDual < myDual)
                largeDual = myDual;
        }
    }

    // The major solving loop
    while (solvePhase) {
        switch (solvePhase) {
        case 1:
            solve_phase1();
            break;
        case 2:
            solve_phase2();
            break;
        case 4:
            break;
        default:
            solvePhase = 0;
            break;
        }
        // Jump for primal
        if (solvePhase == 4)
            break;
    }

    // Report the ticks before primal
    if (dual_variant == HDUAL_VARIANT_PLAIN) {
        int reportList[] = {HTICK_INVERT,        HTICK_CHUZR1,       HTICK_BTRAN,
                            HTICK_PRICE,         HTICK_CHUZC1,       HTICK_CHUZC2,
                            HTICK_CHUZC3,        HTICK_FTRAN,        HTICK_FTRAN_MIX,
                            HTICK_FTRAN_DSE,     HTICK_UPDATE_DUAL,  HTICK_UPDATE_PRIMAL,
                            HTICK_UPDATE_WEIGHT, HTICK_UPDATE_FACTOR};
        int reportCount = sizeof(reportList) / sizeof(int);
        if (model->intOption[INTOPT_PRINT_FLAG])
            model->timer.report(reportCount, reportList);
    }

    if (dual_variant == HDUAL_VARIANT_TASKS) {
        int reportList[] = {
            HTICK_INVERT,        HTICK_CHUZR1,        HTICK_BTRAN,       HTICK_PRICE,
            HTICK_CHUZC1,        HTICK_CHUZC2,        HTICK_CHUZC3,      HTICK_FTRAN,
            HTICK_FTRAN_MIX,     HTICK_FTRAN_DSE,     HTICK_UPDATE_DUAL, HTICK_UPDATE_PRIMAL,
            HTICK_UPDATE_WEIGHT, HTICK_UPDATE_FACTOR, HTICK_GROUP1,      HTICK_GROUP2};
        int reportCount = sizeof(reportList) / sizeof(int);
        if (model->intOption[INTOPT_PRINT_FLAG])
            model->timer.report(reportCount, reportList);
    }

    if (dual_variant == HDUAL_VARIANT_MULTI) {
        int reportList[] = {HTICK_INVERT,        HTICK_CHUZR1,        HTICK_BTRAN,
                            HTICK_PRICE,         HTICK_CHUZC1,        HTICK_CHUZC2,
                            HTICK_CHUZC3,        HTICK_FTRAN,         HTICK_FTRAN_MIX,
                            HTICK_FTRAN_DSE,     HTICK_UPDATE_DUAL,   HTICK_UPDATE_PRIMAL,
                            HTICK_UPDATE_WEIGHT, HTICK_UPDATE_FACTOR, HTICK_UPDATE_ROW_EP};
        int reportCount = sizeof(reportList) / sizeof(int);
        if (model->intOption[INTOPT_PRINT_FLAG])
            model->timer.report(reportCount, reportList);
        fprintf(stderr, "PAMI   %-20s    CUTOFF  %6g    PERSISTENSE  %6g\n",
                model->modelName.c_str(), model->dblOption[DBLOPT_PAMI_CUTOFF],
                model->numberIteration / (1.0 + multi_iteration));
    }

    // Use primal to clean up
    if (solvePhase == 4) {
        HPrimal hPrimal;
        hPrimal.solvePhase2(model);
    }

    // Save the solved results
    model->totalTime += model->timer.getTime();
}

void HDual::init(int num_threads) {
    // Copy size, matrix and factor
    numCol = model->getNumCol();
    numRow = model->getNumRow();
    numTot = model->getNumTot();
    matrix = model->getMatrix();
    factor = model->getFactor();

    // Copy pointers
    jMove = model->getNonbasicMove();
    workDual = model->getWorkDual();
    workRange = model->getWorkRange();
    baseLower = model->getBaseLower();
    baseUpper = model->getBaseUpper();
    baseValue = model->getBaseValue();

    // Copy tolerances
    Tp = model->dblOption[DBLOPT_PRIMAL_TOL];
    Td = model->dblOption[DBLOPT_DUAL_TOL];

    // Setup local vectors
    columnDSE.setup(numRow);
    columnBFRT.setup(numRow);
    column.setup(numRow);
    row_ep.setup(numRow);
    row_ap.setup(numCol);
    columnDensity = 0;
    row_epDensity = 0;
    rowdseDensity = 0;

    // Setup other buffers
    dualRow.setup(model);
    dualRHS.setup(model);

    // Initialize for tasks
    if (dual_variant == HDUAL_VARIANT_TASKS) {
        init_slice(num_threads - 2);
    }

    // Initialize for multi
    if (dual_variant == HDUAL_VARIANT_MULTI) {
        multi_num = num_threads;
        if (multi_num < 1)
            multi_num = 1;
        if (multi_num > HSOL_THREAD_LIMIT)
            multi_num = HSOL_THREAD_LIMIT;
        for (int i = 0; i < multi_num; i++) {
            multi_choice[i].row_ep.setup(numRow);
            multi_choice[i].column.setup(numRow);
            multi_choice[i].columnBFRT.setup(numRow);
        }
        init_slice(multi_num - 1);
    }
    multi_iteration = 0;
    string partitionFile = model->strOption[STROPT_PARTITION_FILE];
    if (partitionFile.size()) {
        dualRHS.setup_partition(partitionFile.c_str());
    }
}

void HDual::init_slice(int init_sliced_num) {
    // Number of slices
    slice_num = init_sliced_num;
    if (slice_num < 1)
        slice_num = 1;
    if (slice_num > HSOL_SLICED_LIMIT)
        slice_num = HSOL_SLICED_LIMIT;

    // Alias to the matrix
    const int *Astart = matrix->getAstart();
    const int *Aindex = matrix->getAindex();
    const double *Avalue = matrix->getAvalue();
    const int AcountX = Astart[numCol];

    // Figure out partition weight
    double sliced_countX = AcountX / slice_num;
    slice_start[0] = 0;
    for (int i = 0; i < slice_num - 1; i++) {
        int endColumn = slice_start[i] + 1; // At least one column
        int endX = Astart[endColumn];
        int stopX = (i + 1) * sliced_countX;
        while (endX < stopX) {
            endX = Astart[++endColumn];
        }
        slice_start[i + 1] = endColumn;
        if (endColumn >= numCol) {
            slice_num = i; // SHRINK
            break;
        }
    }
    slice_start[slice_num] = numCol;

    // Partition the matrix, row_ap and related packet
    vector<int> sliced_Astart;
    for (int i = 0; i < slice_num; i++) {
        // The matrix
        int mystart = slice_start[i];
        int mycount = slice_start[i + 1] - mystart;
        int mystartX = Astart[mystart];
        sliced_Astart.resize(mycount + 1);
        for (int k = 0; k <= mycount; k++)
            sliced_Astart[k] = Astart[k + mystart] - mystartX;
        slice_matrix[i].setup(mycount, numRow, &sliced_Astart[0], Aindex + mystartX,
                              Avalue + mystartX);

        // The row_ap and its packages
        slice_row_ap[i].setup(mycount);
        slice_dualRow[i].setupSlice(model, mycount);
    }
}

void HDual::solve_phase1() {
    model->printMessage("dual-phase-1-start");
    // Switch to dual phase 1 bounds
    model->initBound(1);
    model->initValue();

    // Main solving structure
    for (;;) {
        rebuild();
        for (;;) {
            switch (dual_variant) {
            default:
            case HDUAL_VARIANT_PLAIN:
                iterate();
                break;
            case HDUAL_VARIANT_TASKS:
                iterate_tasks();
                break;
            case HDUAL_VARIANT_MULTI:
                iterate_multi();
                break;
            }
            if (invertHint)
                break;
        }
        if (model->countUpdate == 0)
            break;
    }

    if (rowOut == -1) {
        model->printMessage("dual-phase-1-optimal");
        // Go to phase 2
        if (model->objective == 0) {
            solvePhase = 2;
        } else {
            // We still have dual infeasible
            if (model->problemPerturbed) {
                // Clean up perturbation and go on
                cleanup();
                if (dualInfeasCount == 0)
                    solvePhase = 2;
            } else {
                // Report dual infeasible
                solvePhase = -1;
                model->printMessage("dual-infeasible");
                model->reportStatus(LP_Status_Unbounded);
            }
        }
    } else if (columnIn == -1) {
        // We got dual phase 1 unbounded - strange
        model->printMessage("dual-phase-1-unbounded");
        if (model->problemPerturbed) {
            // Clean up perturbation and go on
            cleanup();
            if (dualInfeasCount == 0)
                solvePhase = 2;
        } else {
            // Report strange issues
            solvePhase = -1;
            model->printMessage("dual-phase-1-not-solved");
            model->reportStatus(LP_Status_Failed);
        }
    }

    if (solvePhase == 2) {
        model->initBound();
        model->initValue();
    }
}

void HDual::solve_phase2() {
    model->printMessage("dual-phase-2-start");

    // Collect free variables
    dualRow.create_Freelist();

    // Main solving structure
    for (;;) {
        rebuild();
        if (dualInfeasCount > 0)
            break;
        for (;;) {
            model->printProgress();
            switch (dual_variant) {
            default:
            case HDUAL_VARIANT_PLAIN:
                iterate();
                break;
            case HDUAL_VARIANT_TASKS:
                iterate_tasks();
                break;
            case HDUAL_VARIANT_MULTI:
                iterate_multi();
                break;
            }

            if (invertHint)
                break;
        }
        if (model->countUpdate == 0)
            break;
    }

    if (dualInfeasCount > 0) {
        // We got free variables - need dual phase 1
        model->printMessage("dual-phase-2-found-free");
        solvePhase = 1;
    } else if (rowOut == -1) {
        // Tentative optimal
        model->printMessage("dual-phase-2-optimal");
        cleanup();
        if (dualInfeasCount > 0) {
            solvePhase = 4; // Do primal
        } else {
            solvePhase = 0;
            model->printMessage("problem-optimal");
            model->reportStatus(LP_Status_Optimal);
        }
    } else if (columnIn == -1) {
        model->printMessage("dual-phase-2-unbounded");
        if (model->problemPerturbed) {
            cleanup();
        } else {
            solvePhase = -1;
            model->printMessage("problem-infeasible");
            model->reportStatus(LP_Status_Infeasible);
        }
    }
}

void HDual::rebuild() {
    check_deadline();
    // Save history information
    model->reportPivots(-1, -1, 0); // Indicate REINVERT
    model->timer.recordStart(HTICK_INVERT);

    invertHint = 0;
    // Rebuild model->factor
    if (model->countUpdate > 0) {
        const int *baseIndex = model->getBaseIndex();
        for (int i = 0; i < numRow; i++)
            dualRHS.workDevexFull[baseIndex[i]] = dualRHS.workDevex[i];
        model->computeFactor();
        for (int i = 0; i < numRow; i++)
            dualRHS.workDevex[i] = dualRHS.workDevexFull[baseIndex[i]];
    }

    // Recompute dual solution
    model->computeDual();
    model->correctDual(&dualInfeasCount);
    model->computePrimal();

    // Collect primal infeasible as a list
    dualRHS.create_infeasArray();
    dualRHS.create_infeasList(columnDensity);

    // Compute the objective value
    model->computeObject(solvePhase);
    model->printObject();

    total_INVERT_TICK = factor->pseudoTick;
    total_FT_inc_TICK = 0;
    total_fake = 0;

    model->timer.recordFinish(HTICK_INVERT);
}

void HDual::cleanup() {
    // Remove perturbation and recompute the dual solution
    model->printMessage("dual-cleanup-shift");
    model->initCost();
    model->initBound();
    model->computeDual();
    model->computeObject(solvePhase);
    model->printObject();

    model->computeDualInfeasInPrimal(&dualInfeasCount);
}

void HDual::iterate() {
    check_deadline();
    chooseRow();
    chooseColumn(&row_ep);

    updateFtranBFRT();
    updateFtran();
    updateFtranDSE(&row_ep);

    updateVerify();
    updateDual();
    updatePrimal(&row_ep);
    updatePivots();
}

void HDual::iterate_tasks() {
    slice_PRICE = 1;

    // Group 1
    chooseRow();

    // Disable slice when too sparse
    if (1.0 * row_ep.count / numRow < 0.01)
        slice_PRICE = 0;

    model->timer.recordStart(HTICK_GROUP1);

    {

        {
            columnDSE.copy(&row_ep);
            updateFtranDSE(&columnDSE);
        }

        {
            if (slice_PRICE)
                chooseColumn_slice(&row_ep);
            else
                chooseColumn(&row_ep);

            updateFtranBFRT();

            updateFtran();
        }
    }
    model->timer.recordFinish(HTICK_GROUP1);

    updateVerify();
    updateDual();
    updatePrimal(&columnDSE);
    updatePivots();
}

void HDual::chooseRow() {
    if (invertHint)
        return;
    for (;;) {
        // Choose row
        dualRHS.choose_normal(&rowOut);
        if (rowOut == -1) {
            invertHint = 1;
            return;
        }

        // Verify weight
        model->timer.recordStart(HTICK_BTRAN);
        row_ep.clear();
        row_ep.count = 1;
        row_ep.index[0] = rowOut;
        row_ep.array[rowOut] = 1;
        row_ep.packFlag = true;
        factor->btran(row_ep, row_epDensity);
        model->timer.recordFinish(HTICK_BTRAN);
        double u_weight = dualRHS.workDevex[rowOut];
        double c_weight = dualRHS.workDevex[rowOut] = row_ep.norm2();
        if (u_weight >= 0.25 * c_weight)
            break;
    }

    // Assign basic info
    columnOut = model->getBaseIndex()[rowOut];
    if (baseValue[rowOut] < baseLower[rowOut])
        deltaPrimal = baseValue[rowOut] - baseLower[rowOut];
    else
        deltaPrimal = baseValue[rowOut] - baseUpper[rowOut];
    sourceOut = deltaPrimal < 0 ? -1 : 1;
    row_epDensity *= 0.95;
    row_epDensity += 0.05 * row_ep.count / numRow;
}

void HDual::chooseColumn(HVector *row_ep) {
    if (invertHint)
        return;

    // Compute pivot row
    model->timer.recordStart(HTICK_PRICE);
    row_ap.clear();
    matrix->price_by_row(row_ap, *row_ep);
    model->timer.recordFinish(HTICK_PRICE);

    // Choose column - possible
    model->timer.recordStart(HTICK_CHUZC1);
    dualRow.clear();
    dualRow.workDelta = deltaPrimal;
    dualRow.create_Freemove(row_ep);
    dualRow.choose_makepack(&row_ap, 0);
    dualRow.choose_makepack(row_ep, numCol);
    dualRow.choose_possible();
    model->timer.recordFinish(HTICK_CHUZC1);

    // Choose column - check problem
    columnIn = -1;
    if (dualRow.workTheta <= 0 || dualRow.workCount == 0) {
        invertHint = 1;
        return;
    }

    // Choose column - final
    dualRow.choose_final();
    dualRow.delete_Freemove();

    columnIn = dualRow.workPivot;
    alphaRow = dualRow.workAlpha;
    thetaDual = dualRow.workTheta;
}

void HDual::chooseColumn_slice(HVector *row_ep) {
    if (invertHint)
        return;

    model->timer.recordStart(HTICK_CHUZC1);
    dualRow.clear();
    dualRow.workDelta = deltaPrimal;
    dualRow.create_Freemove(row_ep);

    // Row_ep:         PACK + CC1

    {

        dualRow.choose_makepack(row_ep, numCol);
        dualRow.choose_possible();
    }

    // Row_ap: PRICE + PACK + CC1
    for (int i = 0; i < slice_num; i++) {

        {
            slice_row_ap[i].clear();
            slice_matrix[i].price_by_row(slice_row_ap[i], *row_ep);

            slice_dualRow[i].clear();
            slice_dualRow[i].workDelta = deltaPrimal;
            slice_dualRow[i].choose_makepack(&slice_row_ap[i], slice_start[i]);
            slice_dualRow[i].choose_possible();
        }
    }

    // Join CC1 results here
    for (int i = 0; i < slice_num; i++)
        dualRow.choose_joinpack(&slice_dualRow[i]);

    // Infeasible we created before
    columnIn = -1;
    if (dualRow.workTheta <= 0 || dualRow.workCount == 0) {
        invertHint = 1;
        return;
    }
    model->timer.recordFinish(HTICK_CHUZC1);

    // Choose column 2, This only happens if didn't go out
    dualRow.choose_final();
    dualRow.delete_Freemove();
    columnIn = dualRow.workPivot;
    alphaRow = dualRow.workAlpha;
    thetaDual = dualRow.workTheta;
}

void HDual::updateFtranBFRT() {
    if (invertHint)
        return;
    model->timer.recordStart(HTICK_FTRAN_MIX);
    dualRow.update_flip(&columnBFRT);
    if (columnBFRT.count)
        factor->ftran(columnBFRT, columnDensity);
    model->timer.recordFinish(HTICK_FTRAN_MIX);
}

void HDual::updateFtran() {
    if (invertHint)
        return;
    model->timer.recordStart(HTICK_FTRAN);
    column.clear();
    column.packFlag = true;
    matrix->collect_aj(column, columnIn, 1);
    factor->ftran(column, columnDensity);
    alpha = column.array[rowOut];
    model->timer.recordFinish(HTICK_FTRAN);
}

void HDual::updateFtranDSE(HVector *dseVector) {
    if (invertHint)
        return;
    model->timer.recordStart(HTICK_FTRAN_DSE);
    factor->ftran(*dseVector, rowdseDensity);
    model->timer.recordFinish(HTICK_FTRAN_DSE);
}

void HDual::updateVerify() {
    if (invertHint)
        return;

    // The alpha
    double aCol = fabs(alpha);
    double aRow = fabs(alphaRow);
    double aDiff = fabs(aCol - aRow);
    if (aDiff / min(aCol, aRow) > 1e-7 && model->countUpdate > 0) {
        invertHint = 1;
    }

    // We get this thing, but it is not actived by default.
    //    // The dual reduced cost
    //    double dualin_u = workDual[columnIn];
    //    double dualin_c = model->getWorkCost()[columnIn];
    //    for (int i = 0; i < column.count; i++) {
    //        int iRow = column.index[i];
    //        int iCol = model->getBaseIndex()[iRow];
    //        double value = column.array[iRow];
    //        double cost = model->getWorkCost()[iCol] + model->getWorkShift()[iCol];
    //        dualin_c -= cost * value;
    //    }
    //    double dualin_diff = fabs(dualin_c - dualin_u);
    ////    if (dualin_diff > Td) {
    ////        cerr << dualin_c << "\t" << dualin_u << "\t" << dualin_diff << endl;
    ////        invertHint = 1;
    ////    }
}

void HDual::updateDual() {
    if (invertHint)
        return;

    // Update - dual (shift and back)
    if (thetaDual == 0)
        model->shiftCost(columnIn, -workDual[columnIn]);
    else {
        dualRow.update_dual(thetaDual);
        if (dual_variant != HDUAL_VARIANT_PLAIN && slice_PRICE)
            for (int i = 0; i < slice_num; i++)
                slice_dualRow[i].update_dual(thetaDual);
    }
    workDual[columnIn] = 0;
    workDual[columnOut] = -thetaDual;
    model->shiftBack(columnOut);
}

void HDual::updatePrimal(HVector *dseVector) {
    if (invertHint)
        return;

    // Update - primal and weight
    dualRHS.update_primal(&columnBFRT, 1);
    dualRHS.update_infeasList(&columnBFRT);
    double x_out = baseValue[rowOut];
    double l_out = baseLower[rowOut];
    double u_out = baseUpper[rowOut];
    thetaPrimal = (x_out - (deltaPrimal < 0 ? l_out : u_out)) / alpha;
    dualRHS.update_primal(&column, thetaPrimal);
    double thisDevex = dualRHS.workDevex[rowOut] / (alpha * alpha);
    dualRHS.update_weight(&column, thisDevex, -2 / alpha, &dseVector->array[0]);
    dualRHS.workDevex[rowOut] = thisDevex;
    dualRHS.update_infeasList(&column);

    rowdseDensity = 0.95 * rowdseDensity + 0.05 * dseVector->count / numRow;
    columnDensity = 0.95 * columnDensity + 0.05 * column.count / numRow;

    total_fake += column.fakeTick;
    total_fake += dseVector->fakeTick;

    total_FT_inc_TICK += column.pseudoTick;
    total_FT_inc_TICK += dseVector->pseudoTick;
}

void HDual::updatePivots() {
    if (invertHint)
        return;

    // Update - pivots
    model->updatePivots(columnIn, rowOut, sourceOut);
    model->reportPivots(columnIn, columnOut, alpha);
    model->updateFactor(&column, &row_ep, &rowOut, &invertHint);
    model->updateMatrix(columnIn, columnOut);
    dualRow.delete_Freelist(columnIn);
    dualRHS.update_pivots(rowOut, model->getWorkValue()[columnIn] + thetaPrimal);

    if (total_fake >= factor->fakeTick && model->countUpdate >= 50) {
        //        cerr << total_fake << "\t" << factor->fakeTick << endl;
        //        fprintf(stderr,
        //                "%10d   %10.2e  %10.2e  %10.4f          %10.2e  %10.2e  %10.4f\n",
        //                model->countUpdate, total_INVERT_TICK, total_FT_inc_TICK,
        //                total_FT_inc_TICK / total_INVERT_TICK, factor->fakeTick,
        //                total_fake, total_fake / factor->fakeTick);
        invertHint = 1;
    }
}

// Upstream file: HDualMulti.cpp

using namespace std;

void HDual::iterate_multi() {
    slice_PRICE = 1;

    // Report candidate
    major_chooseRow();
    minor_chooseRow();
    if (rowOut == -1) {
        invertHint = 1;
        return;
    }

    // Assign the slice_row_ep, skip if possible
    if (1.0 * multi_finish[multi_nFinish].row_ep->count / numRow < 0.01)
        slice_PRICE = 0;

    if (slice_PRICE) {

        chooseColumn_slice(multi_finish[multi_nFinish].row_ep);
    } else {
        chooseColumn(multi_finish[multi_nFinish].row_ep);
    }
    // If we failed.
    if (invertHint) {
        major_update();
        return;
    }

    minor_update();
    major_update();
}

void HDual::major_chooseRow() {

    /**
     * 0. Initial check to see if we need to do it again
     */
    if (model->countUpdate == 0)
        multi_chooseAgain = 1;
    if (!multi_chooseAgain)
        return;
    multi_chooseAgain = 0;
    multi_iteration++;

    /**
     * Major loop:
     *     repeat 1-5, until we found a good sets of choices
     */
    for (;;) {
        // 1. Multiple CHUZR
        int initialCount = 0;
        vector<int> choiceIndex(multi_num);
        dualRHS.choose_multi_HGauto(&choiceIndex[0], &initialCount, multi_num);
        //        dualRHS.choose_multi_global(&choiceIndex[0], &initialCount, multi_num);
        if (initialCount == 0 && dualRHS.workCutoff == 0) {
            // OPTIMAL
            return;
        }

        // 2. Shrink the size by cutoff
        int choiceCount = 0;
        for (int i = 0; i < initialCount; i++) {
            int iRow = choiceIndex[i];
            if (dualRHS.workArray[iRow] / dualRHS.workDevex[iRow] >= dualRHS.workCutoff) {
                choiceIndex[choiceCount++] = iRow;
            }
        }
        if (initialCount == 0 || choiceCount <= initialCount / 3) {
            // Need to do the list again
            dualRHS.create_infeasList(columnDensity);
            continue;
        }

        // 3. Store the choiceIndex to buffer
        for (int ich = 0; ich < multi_num; ich++)
            multi_choice[ich].rowOut = -1;
        for (int ich = 0; ich < choiceCount; ich++)
            multi_choice[ich].rowOut = choiceIndex[ich];

        // 4. Parallel BTRAN and compute weight
        major_chooseRowBtran();

        // 5. Update row weights
        for (int ich = 0; ich < multi_num; ich++) {
            if (multi_choice[ich].rowOut >= 0) {
                row_epDensity *= 0.95;
                row_epDensity += 0.05 * multi_choice[ich].row_ep.count / numRow;
            }
        }

        // 6. Check updated and computed weight
        int countWrongDevex = 0;
        for (int i = 0; i < multi_num; i++) {
            const int iRow = multi_choice[i].rowOut;
            if (iRow < 0)
                continue;
            double u_weight = dualRHS.workDevex[iRow];
            double c_weight = dualRHS.workDevex[iRow] = multi_choice[i].infeasDevex;
            if (u_weight < 0.25 * c_weight) {
                multi_choice[i].rowOut = -1;
                countWrongDevex++;
            }
        }
        if (countWrongDevex <= choiceCount / 3)
            break;
    }

    // 6. Take other info associated with choices
    double pamiCutoff = model->dblOption[DBLOPT_PAMI_CUTOFF];
    for (int i = 0; i < multi_num; i++) {
        const int iRow = multi_choice[i].rowOut;
        if (iRow < 0)
            continue;
        // Other info
        multi_choice[i].baseValue = baseValue[iRow];
        multi_choice[i].baseLower = baseLower[iRow];
        multi_choice[i].baseUpper = baseUpper[iRow];
        multi_choice[i].infeasValue = dualRHS.workArray[iRow];
        multi_choice[i].infeasDevex = dualRHS.workDevex[iRow];
        multi_choice[i].infeasLimit = dualRHS.workArray[iRow] / dualRHS.workDevex[iRow];
        multi_choice[i].infeasLimit *= pamiCutoff;
    }

    // 6. Finish count
    multi_nFinish = 0;
}

void HDual::major_chooseRowBtran() {
    model->timer.recordStart(HTICK_BTRAN);

    // 4.1. Prepare BTRAN buffer
    int multi_ntasks = 0;
    int multi_iRow[HSOL_THREAD_LIMIT];
    int multi_iwhich[HSOL_THREAD_LIMIT];
    double multi_devex[HSOL_THREAD_LIMIT];
    HVector_ptr multi_vector[HSOL_THREAD_LIMIT];
    for (int ich = 0; ich < multi_num; ich++) {
        if (multi_choice[ich].rowOut >= 0) {
            multi_iRow[multi_ntasks] = multi_choice[ich].rowOut;
            multi_vector[multi_ntasks] = &multi_choice[ich].row_ep;
            multi_iwhich[multi_ntasks] = ich;
            multi_ntasks++;
        }
    }

    // 4.2 Perform BTRAN

    for (int i = 0; i < multi_ntasks; i++) {
        const int iRow = multi_iRow[i];
        HVector_ptr work_ep = multi_vector[i];
        work_ep->clear();
        work_ep->count = 1;
        work_ep->index[0] = iRow;
        work_ep->array[iRow] = 1;
        work_ep->packFlag = true;
        factor->btran(*work_ep, row_epDensity);
        multi_devex[i] = work_ep->norm2();
    }

    // 4.3 Put back devex
    for (int i = 0; i < multi_ntasks; i++)
        multi_choice[multi_iwhich[i]].infeasDevex = multi_devex[i];
    model->timer.recordFinish(HTICK_BTRAN);
}

void HDual::minor_chooseRow() {
    /**
     * 1. Find which to go out
     *        Because we had other checking code
     *        We know current best is OK to be used
     */
    multi_iChoice = -1;
    double bestMerit = 0;
    for (int ich = 0; ich < multi_num; ich++) {
        const int iRow = multi_choice[ich].rowOut;
        if (iRow < 0)
            continue;
        double infeasValue = multi_choice[ich].infeasValue;
        double infeasDevex = multi_choice[ich].infeasDevex;
        double infeasMerit = infeasValue / infeasDevex;
        if (bestMerit < infeasMerit) {
            bestMerit = infeasMerit;
            multi_iChoice = ich;
        }
    }

    /**
     * 2. Obtain other info for
     *        current sub-optimisation choice
     */
    rowOut = -1;
    if (multi_iChoice != -1) {
        MChoice *workChoice = &multi_choice[multi_iChoice];

        // Assign useful variables
        rowOut = workChoice->rowOut;
        columnOut = model->getBaseIndex()[rowOut];
        double valueOut = workChoice->baseValue;
        double lowerOut = workChoice->baseLower;
        double upperOut = workChoice->baseUpper;
        deltaPrimal = valueOut - (valueOut < lowerOut ? lowerOut : upperOut);
        sourceOut = deltaPrimal < 0 ? -1 : 1;

        // Assign buffers
        MFinish *workFinish = &multi_finish[multi_nFinish];
        workFinish->rowOut = rowOut;
        workFinish->columnOut = columnOut;
        workFinish->row_ep = &workChoice->row_ep;
        workFinish->column = &workChoice->column;
        workFinish->columnBFRT = &workChoice->columnBFRT;
        workFinish->devex = workChoice->infeasDevex;

        // Disable current row
        workChoice->rowOut = -1;
    }
}

void HDual::minor_update() {
    // Minor update - store roll back data
    MFinish *Fin = &multi_finish[multi_nFinish];
    Fin->moveIn = model->getNonbasicMove()[columnIn];
    Fin->shiftOut = model->getWorkShift()[columnOut];
    Fin->flipList.clear();
    for (int i = 0; i < dualRow.workCount; i++)
        Fin->flipList.push_back(dualRow.workData[i].first);

    // Minor update - key parts
    minor_updateDual();
    minor_updatePrimal();
    minor_updatePivots();
    minor_updateRows();
    multi_nFinish++;

    // Minor update - check for the next iteration
    int countRemain = 0;
    for (int i = 0; i < multi_num; i++) {
        int iRow = multi_choice[i].rowOut;
        if (iRow < 0)
            continue;
        double myInfeas = multi_choice[i].infeasValue;
        double myWeight = multi_choice[i].infeasDevex;
        countRemain += (myInfeas / myWeight > multi_choice[i].infeasLimit);
    }
    if (countRemain == 0)
        multi_chooseAgain = 1;
    //    if (multi_nFinish + 1 == multi_num)
    //        multi_chooseAgain = 1;
}

void HDual::minor_updateDual() {
    /**
     * 1. Update the dual solution
     *    XXX Data parallel (depends on the ap partition before)
     */
    if (thetaDual == 0) {
        model->shiftCost(columnIn, -workDual[columnIn]);
    } else {
        dualRow.update_dual(thetaDual);
        if (slice_PRICE) {
            for (int i = 0; i < slice_num; i++)
                slice_dualRow[i].update_dual(thetaDual);
        }
    }
    workDual[columnIn] = 0;
    workDual[columnOut] = -thetaDual;
    model->shiftBack(columnOut);

    /**
     * 2. Apply global bound flip
     */
    dualRow.update_flip(multi_finish[multi_nFinish].columnBFRT);

    /**
     * 3. Apply local bound flips
     */
    for (int ich = 0; ich < multi_num; ich++) {
        if (ich == multi_iChoice || multi_choice[ich].rowOut >= 0) {
            HVector *this_ep = &multi_choice[ich].row_ep;
            for (int i = 0; i < dualRow.workCount; i++) {
                double dot = matrix->compute_dot(*this_ep, dualRow.workData[i].first);
                multi_choice[ich].baseValue -= dualRow.workData[i].second * dot;
            }
        }
    }
}

void HDual::minor_updatePrimal() {
    MChoice *Cho = &multi_choice[multi_iChoice];
    MFinish *Fin = &multi_finish[multi_nFinish];
    double valueOut = Cho->baseValue;
    double lowerOut = Cho->baseLower;
    double upperOut = Cho->baseUpper;
    if (deltaPrimal < 0) {
        thetaPrimal = (valueOut - lowerOut) / alphaRow;
        Fin->basicBound = lowerOut;
    }
    if (deltaPrimal > 0) {
        thetaPrimal = (valueOut - upperOut) / alphaRow;
        Fin->basicBound = upperOut;
    }
    Fin->thetaPrimal = thetaPrimal;

    /**
     * 5. Update the other primal value
     *    By the pivot (thetaPrimal)
     */
    for (int ich = 0; ich < multi_num; ich++) {
        if (multi_choice[ich].rowOut >= 0) {
            HVector *this_ep = &multi_choice[ich].row_ep;
            double dot = matrix->compute_dot(*this_ep, columnIn);
            multi_choice[ich].baseValue -= thetaPrimal * dot;
            double value = multi_choice[ich].baseValue;
            double lower = multi_choice[ich].baseLower;
            double upper = multi_choice[ich].baseUpper;
            double infeas = 0;
            if (value < lower - Tp)
                infeas = value - lower;
            if (value > upper + Tp)
                infeas = value - upper;
            infeas *= infeas;
            multi_choice[ich].infeasValue = infeas;
        }
    }
}
void HDual::minor_updatePivots() {
    MFinish *Fin = &multi_finish[multi_nFinish];
    model->updatePivots(columnIn, rowOut, sourceOut);
    Fin->devex /= (alphaRow * alphaRow);
    Fin->basicValue = model->getWorkValue()[columnIn] + thetaPrimal;
    model->updateMatrix(columnIn, columnOut);
    Fin->columnIn = columnIn;
    Fin->alphaRow = alphaRow;
    model->reportPivots(columnIn, columnOut, alphaRow);
}

void HDual::minor_updateRows() {
    model->timer.recordStart(HTICK_UPDATE_ROW_EP);
    const HVector *Row = multi_finish[multi_nFinish].row_ep;
    int updateRows_inDense = (Row->count < 0) || (Row->count > 0.1 * numRow);
    if (updateRows_inDense) {
        int multi_nTasks = 0;
        int multi_iwhich[HSOL_THREAD_LIMIT];
        double multi_xpivot[HSOL_THREAD_LIMIT];
        HVector_ptr multi_vector[HSOL_THREAD_LIMIT];

        /*
         * Dense mode
         *  1. Find which ones to do and the pivotX
         *  2. Do all of them in task parallel
         */

        // Collect tasks
        for (int ich = 0; ich < multi_num; ich++) {
            if (multi_choice[ich].rowOut >= 0) {
                HVector *next_ep = &multi_choice[ich].row_ep;
                double pivotX = matrix->compute_dot(*next_ep, columnIn);
                if (fabs(pivotX) < HSOL_CONST_TINY)
                    continue;
                multi_vector[multi_nTasks] = next_ep;
                multi_xpivot[multi_nTasks] = -pivotX / alphaRow;
                multi_iwhich[multi_nTasks] = ich;
                multi_nTasks++;
            }
        }

        // Perform tasks

        for (int i = 0; i < multi_nTasks; i++) {
            HVector_ptr nextEp = multi_vector[i];
            const double xpivot = multi_xpivot[i];
            nextEp->saxpy(xpivot, Row);
            nextEp->tight();
            multi_xpivot[i] = nextEp->norm2();
        }

        // Put weight back
        for (int i = 0; i < multi_nTasks; i++)
            multi_choice[multi_iwhich[i]].infeasDevex = multi_xpivot[i];
    } else {
        // Sparse mode: just do it sequentially
        for (int ich = 0; ich < multi_num; ich++) {
            if (multi_choice[ich].rowOut >= 0) {
                HVector *next_ep = &multi_choice[ich].row_ep;
                double pivotX = matrix->compute_dot(*next_ep, columnIn);
                if (fabs(pivotX) < HSOL_CONST_TINY)
                    continue;
                next_ep->saxpy(-pivotX / alphaRow, Row);
                next_ep->tight();
                multi_choice[ich].infeasDevex = next_ep->norm2();
            }
        }
    }
    model->timer.recordFinish(HTICK_UPDATE_ROW_EP);
}

void HDual::major_update() {
    /**
     * 0. See if it's ready to perform a major update
     */
    if (invertHint)
        multi_chooseAgain = 1;
    if (!multi_chooseAgain)
        return;

    // Major update - FTRANs
    major_updateFtranPrepare();
    major_updateFtranParallel();
    major_updateFtranFinal();

    // Major update - check for roll back
    for (int iFn = 0; iFn < multi_nFinish; iFn++) {
        MFinish *iFinish = &multi_finish[iFn];
        HVector *iColumn = iFinish->column;
        int iRowOut = iFinish->rowOut;
        double alphaC = fabs(iColumn->array[iRowOut]);
        double alphaR = fabs(iFinish->alphaRow);
        double compare = min(alphaC, alphaR);
        double alphaDiff = fabs(alphaC - alphaR);
        //        int startUpdate = model->countUpdate - multi_nFinish;
        if (alphaDiff / compare > 1e-8 && model->countUpdate > 0) {
            cerr << "REPORT " << model->modelName << " NEED-ROLL-BACK   ";
            cerr << model->numberIteration << " alpha = " << alphaC << " alphaR = " << alphaR
                 << " diff = " << alphaDiff / compare << "  multi_nFinish = " << multi_nFinish
                 << endl;
            invertHint = 1;
            //            if (startUpdate > 0) {
            major_rollback();
            return;
            //            }
        }
    }

    // Major update - primal and factor
    major_updatePrimal();
    major_updateFactor();
}

void HDual::major_updateFtranPrepare() {
    // Prepare FTRAN BFRT buffer
    columnBFRT.clear();
    for (int iFn = 0; iFn < multi_nFinish; iFn++) {
        MFinish *Fin = &multi_finish[iFn];
        HVector *Vec = Fin->columnBFRT;
        matrix->collect_aj(*Vec, Fin->columnIn, Fin->thetaPrimal);

        // Update this buffer by previous Row_ep
        for (int jFn = iFn - 1; jFn >= 0; jFn--) {
            MFinish *jFinish = &multi_finish[jFn];
            double *jRow_epArray = &jFinish->row_ep->array[0];
            double pivotX = 0;
            for (int k = 0; k < Vec->count; k++) {
                int iRow = Vec->index[k];
                pivotX += Vec->array[iRow] * jRow_epArray[iRow];
            }
            if (fabs(pivotX) > HSOL_CONST_TINY) {
                pivotX /= jFinish->alphaRow;
                matrix->collect_aj(*Vec, jFinish->columnIn, -pivotX);
                matrix->collect_aj(*Vec, jFinish->columnOut, pivotX);
            }
        }
        columnBFRT.saxpy(1, Vec);
    }

    // Prepare regular FTRAN buffer
    for (int iFn = 0; iFn < multi_nFinish; iFn++) {
        MFinish *iFinish = &multi_finish[iFn];
        HVector *iColumn = iFinish->column;
        iColumn->clear();
        iColumn->packFlag = true;
        matrix->collect_aj(*iColumn, iFinish->columnIn, 1);
    }
}

void HDual::major_updateFtranParallel() {
    model->timer.recordStart(HTICK_FTRAN_MIX);

    // Prepare buffers
    int multi_ntasks = 0;
    double multi_density[HSOL_THREAD_LIMIT * 2 + 1];
    HVector_ptr multi_vector[HSOL_THREAD_LIMIT * 2 + 1];
    // BFRT first
    multi_density[multi_ntasks] = columnDensity;
    multi_vector[multi_ntasks] = &columnBFRT;
    multi_ntasks++;
    // Then DSE
    for (int iFn = 0; iFn < multi_nFinish; iFn++) {
        multi_density[multi_ntasks] = rowdseDensity;
        multi_vector[multi_ntasks] = multi_finish[iFn].row_ep;
        multi_ntasks++;
    }
    // Then Column
    for (int iFn = 0; iFn < multi_nFinish; iFn++) {
        multi_density[multi_ntasks] = columnDensity;
        multi_vector[multi_ntasks] = multi_finish[iFn].column;
        multi_ntasks++;
    }

    // Perform FTRAN

    for (int i = 0; i < multi_ntasks; i++) {
        HVector_ptr rhs = multi_vector[i];
        double density = multi_density[i];
        factor->ftran(*rhs, density);
    }

    // Update ticks
    for (int iFn = 0; iFn < multi_nFinish; iFn++) {
        MFinish *Fin = &multi_finish[iFn];
        HVector *Col = Fin->column;
        HVector *Row = Fin->row_ep;
        total_FT_inc_TICK += Col->pseudoTick;
        total_FT_inc_TICK += Row->pseudoTick;
    }

    // Update rates
    for (int iFn = 0; iFn < multi_nFinish; iFn++) {
        MFinish *Fin = &multi_finish[iFn];
        HVector *Col = Fin->column;
        HVector *Row = Fin->row_ep;
        columnDensity = 0.95 * columnDensity + 0.05 * Col->count / numRow;
        rowdseDensity = 0.95 * rowdseDensity + 0.05 * Row->count / numRow;
    }
    model->timer.recordFinish(HTICK_FTRAN_MIX);
}

void HDual::major_updateFtranFinal() {
    model->timer.recordStart(HTICK_FTRAN_MIX);
    int updateFTRAN_inDense = dualRHS.workCount < 0;
    if (updateFTRAN_inDense) {
        for (int iFn = 0; iFn < multi_nFinish; iFn++) {
            multi_finish[iFn].column->count = -1;
            multi_finish[iFn].row_ep->count = -1;
            double *myCol = &multi_finish[iFn].column->array[0];
            double *myRow = &multi_finish[iFn].row_ep->array[0];
            for (int jFn = 0; jFn < iFn; jFn++) {
                int pivotRow = multi_finish[jFn].rowOut;
                const double pivotAlpha = multi_finish[jFn].alphaRow;
                const double *pivotArray = &multi_finish[jFn].column->array[0];
                double pivotX1 = myCol[pivotRow];
                double pivotX2 = myRow[pivotRow];

                // The FTRAN regular buffer
                if (fabs(pivotX1) > HSOL_CONST_TINY) {
                    const double pivot = pivotX1 / pivotAlpha;

                    for (int i = 0; i < numRow; i++)
                        myCol[i] -= pivot * pivotArray[i];
                    myCol[pivotRow] = pivot;
                }
                // The FTRAN-DSE buffer
                if (fabs(pivotX2) > HSOL_CONST_TINY) {
                    const double pivot = pivotX2 / pivotAlpha;

                    for (int i = 0; i < numRow; i++)
                        myRow[i] -= pivot * pivotArray[i];
                    myRow[pivotRow] = pivot;
                }
            }
        }
    } else {
        for (int iFn = 0; iFn < multi_nFinish; iFn++) {
            MFinish *Fin = &multi_finish[iFn];
            HVector *Col = Fin->column;
            HVector *Row = Fin->row_ep;
            for (int jFn = 0; jFn < iFn; jFn++) {
                MFinish *jFinish = &multi_finish[jFn];
                int pivotRow = jFinish->rowOut;
                double pivotX1 = Col->array[pivotRow];
                // The FTRAN regular buffer
                if (fabs(pivotX1) > HSOL_CONST_TINY) {
                    pivotX1 /= jFinish->alphaRow;
                    Col->saxpy(-pivotX1, jFinish->column);
                    Col->array[pivotRow] = pivotX1;
                }
                // The FTRAN-DSE buffer
                double pivotX2 = Row->array[pivotRow];
                if (fabs(pivotX2) > HSOL_CONST_TINY) {
                    pivotX2 /= jFinish->alphaRow;
                    Row->saxpy(-pivotX2, jFinish->column);
                    Row->array[pivotRow] = pivotX2;
                }
            }
        }
    }
    model->timer.recordFinish(HTICK_FTRAN_MIX);
}

void HDual::major_updatePrimal() {
    int updatePrimal_inDense = dualRHS.workCount < 0;
    if (updatePrimal_inDense) {
        // Update the RHS in dense
        const double *mixArray = &columnBFRT.array[0];
        double *rhs = &dualRHS.workArray[0];

        for (int iRow = 0; iRow < numRow; iRow++) {
            baseValue[iRow] -= mixArray[iRow];
            const double value = baseValue[iRow];
            const double less = baseLower[iRow] - value;
            const double more = value - baseUpper[iRow];
            double infeas = less > Tp ? less : (more > Tp ? more : 0);
            rhs[iRow] = infeas * infeas;
        }

        // Update the weight in dense
        for (int iFn = 0; iFn < multi_nFinish; iFn++) {
            const double pivotDevex = multi_finish[iFn].devex;
            const double *colArray = &multi_finish[iFn].column->array[0];
            const double *dseArray = &multi_finish[iFn].row_ep->array[0];
            double Kai = -2 / multi_finish[iFn].alphaRow;
            double *devex = &dualRHS.workDevex[0];

            for (int iRow = 0; iRow < numRow; iRow++) {
                const double val = colArray[iRow];
                devex[iRow] += val * (pivotDevex * val + Kai * dseArray[iRow]);
                if (devex[iRow] < 1e-4)
                    devex[iRow] = 1e-4;
            }
        }
    } else {
        // Update primal and pivots
        dualRHS.update_primal(&columnBFRT, 1);
        dualRHS.update_infeasList(&columnBFRT);

        // Update weights
        for (int iFn = 0; iFn < multi_nFinish; iFn++) {
            MFinish *Fin = &multi_finish[iFn];
            HVector *Col = Fin->column;
            HVector *Row = Fin->row_ep;
            double Kai = -2 / Fin->alphaRow;
            dualRHS.update_weight(Col, Fin->devex, Kai, &Row->array[0]);
            dualRHS.update_infeasList(Col);
        }
    }

    // Update primal value for the pivots
    for (int iFn = 0; iFn < multi_nFinish; iFn++) {
        MFinish *Fin = &multi_finish[iFn];
        int iRow = Fin->rowOut;
        double value = baseValue[iRow] - Fin->basicBound + Fin->basicValue;
        dualRHS.update_pivots(iRow, value);
    }

    // Update weight value for the pivots
    for (int iFn = 0; iFn < multi_nFinish; iFn++) {
        const int iRow = multi_finish[iFn].rowOut;
        const double pivotDevex = multi_finish[iFn].devex;
        const double *colArray = &multi_finish[iFn].column->array[0];
        const double *dseArray = &multi_finish[iFn].row_ep->array[0];
        double Kai = -2 / multi_finish[iFn].alphaRow;
        for (int jFn = 0; jFn < iFn; jFn++) {
            int jRow = multi_finish[jFn].rowOut;
            double value = colArray[jRow];
            double devex = dualRHS.workDevex[jRow];
            devex += value * (pivotDevex * value + Kai * dseArray[jRow]);
            if (devex < 1e-4)
                devex = 1e-4;
            dualRHS.workDevex[jRow] = devex;
        }
        dualRHS.workDevex[iRow] = pivotDevex;
    }
}

void HDual::major_updateFactor() {
    /**
     * 9. Update the factor by CFT
     */
    vector<int> iRows(multi_nFinish);
    for (int iCh = 0; iCh < multi_nFinish - 1; iCh++) {
        multi_finish[iCh].row_ep->next = multi_finish[iCh + 1].row_ep;
        multi_finish[iCh].column->next = multi_finish[iCh + 1].column;
        iRows[iCh] = multi_finish[iCh].rowOut;
    }
    iRows[multi_nFinish - 1] = multi_finish[multi_nFinish - 1].rowOut;
    if (multi_nFinish > 0)
        model->updateFactor(multi_finish[0].column, multi_finish[0].row_ep, iRows.data(),
                            &invertHint);

    if (total_FT_inc_TICK > total_INVERT_TICK * 1.5 && model->countUpdate > 200)
        invertHint = 1;
}

void HDual::major_rollback() {
    for (int iFn = multi_nFinish - 1; iFn >= 0; iFn--) {
        MFinish *Fin = &multi_finish[iFn];

        // 1. Roll back pivot
        model->getNonbasicMove()[Fin->columnIn] = Fin->moveIn;
        model->getNonbasicFlag()[Fin->columnIn] = 1;
        model->getNonbasicMove()[Fin->columnOut] = 0;
        model->getNonbasicFlag()[Fin->columnOut] = 0;
        model->getBaseIndex()[Fin->rowOut] = Fin->columnOut;

        // 2. Roll back matrix
        model->updateMatrix(Fin->columnOut, Fin->columnIn);

        // 3. Roll back flips
        for (unsigned i = 0; i < Fin->flipList.size(); i++)
            model->flipBound(Fin->flipList[i]);

        // 4. Roll back cost
        model->getWorkShift()[Fin->columnIn] = 0;
        model->getWorkShift()[Fin->columnOut] = Fin->shiftOut;

        // 5. The iteration count
        model->numberIteration--;
    }
}

// Upstream file: HDualRHS.cpp

using namespace std;

void HDualRHS::setup(HModel *model) {
    workModel = model;
    workMark.resize(workModel->getNumRow());
    workIndex.resize(workModel->getNumRow());
    workArray.resize(workModel->getNumRow());
    workDevex.assign(workModel->getNumRow(), 1);
    workDevexFull.resize(workModel->getNumTot());
    partNum = 0;
    partSwitch = 0;
}

void HDualRHS::setup_partition(const char *filename) {
    ifstream reader(filename);
    reader >> partNum >> partNumRow >> partNumCol >> partNumCut;
    if (partNumRow != workModel->getNumRow()) {
        partNum = 0;
        workModel->printMessage("wrong-partition-file");
        reader.close();
        return;
    }
    workPartition.assign(workModel->getNumRow(), 0);
    for (int i = 0; i < partNumRow; i++)
        reader >> workPartition[i];
    reader.close();
    partSwitch = 1;
}

void HDualRHS::choose_normal(int *chIndex) {
    workModel->timer.recordStart(HTICK_CHUZR1);
    int random = workModel->random.intRandom();
    if (workCount < 0) {
        // DENSE mode
        const int numRow = -workCount;
        int randomStart = random % numRow;
        double bestMerit = 0;
        int bestIndex = -1;
        for (int section = 0; section < 2; section++) {
            const int start = (section == 0) ? randomStart : 0;
            const int end = (section == 0) ? numRow : randomStart;
            for (int iRow = start; iRow < end; iRow++) {
                if (workArray[iRow] > HSOL_CONST_ZERO) {
                    const double myInfeas = workArray[iRow];
                    const double myWeight = workDevex[iRow];
                    if (bestMerit * myWeight < myInfeas) {
                        bestMerit = myInfeas / myWeight;
                        bestIndex = iRow;
                    }
                }
            }
        }
        *chIndex = bestIndex;

    } else {
        // SPARSE mode
        if (workCount == 0) {
            *chIndex = -1;
            return;
        }

        int randomStart = random % workCount;
        double bestMerit = 0;
        int bestIndex = -1;
        for (int section = 0; section < 2; section++) {
            const int start = (section == 0) ? randomStart : 0;
            const int end = (section == 0) ? workCount : randomStart;
            for (int i = start; i < end; i++) {
                int iRow = workIndex[i];
                if (workArray[iRow] > HSOL_CONST_ZERO) {
                    const double myInfeas = workArray[iRow];
                    const double myWeight = workDevex[iRow];
                    if (bestMerit * myWeight < myInfeas) {
                        bestMerit = myInfeas / myWeight;
                        bestIndex = iRow;
                    }
                }
            }
        }

        int createListAgain = 0;
        if (bestIndex == -1) {
            createListAgain = workCutoff > 0;
        } else if (bestMerit <= workCutoff * 0.99) {
            createListAgain = 1;
        }
        if (createListAgain) {
            create_infeasList(0);
            choose_normal(&bestIndex);
        }
        *chIndex = bestIndex;
    }
    workModel->timer.recordFinish(HTICK_CHUZR1);
}

void HDualRHS::choose_multi_global(int *chIndex, int *chCount, int chLimit) {
    workModel->timer.recordStart(HTICK_CHUZR1);

    for (int i = 0; i < chLimit; i++)
        chIndex[i] = -1;

    const unsigned int chooseCHECK = chLimit * 2;
    vector<pair<double, int>> setP;
    setP.reserve(chooseCHECK);

    if (workCount < 0) {
        // DENSE mode
        const int numRow = -workCount;
        double cutoffMerit = 0;
        for (int iRow = 0; iRow < numRow; iRow++) {
            if (workArray[iRow] > HSOL_CONST_ZERO) {
                const double myInfeas = workArray[iRow];
                const double myWeight = workDevex[iRow];
                if (cutoffMerit * myWeight < myInfeas) {
                    // Save
                    setP.push_back(make_pair(-myInfeas / myWeight, iRow));
                    // Shrink
                    if (setP.size() >= chooseCHECK) {
                        sort(setP.begin(), setP.end());
                        setP.resize(chLimit);
                        cutoffMerit = -setP.back().first;
                    }
                }
            }
        }

    } else {
        // SPARSE Mode
        double cutoffMerit = 0;
        for (int i = 0; i < workCount; i++) {
            int iRow = workIndex[i];
            if (workArray[iRow] > HSOL_CONST_ZERO) {
                const double myInfeas = workArray[iRow];
                const double myWeight = workDevex[iRow];
                if (cutoffMerit * myWeight < myInfeas) {
                    // Save
                    setP.push_back(make_pair(-myInfeas / myWeight, iRow));
                    // Shrink
                    if (setP.size() >= chooseCHECK) {
                        sort(setP.begin(), setP.end());
                        setP.resize(chLimit);
                        cutoffMerit = -setP.back().first;
                    }
                }
            }
        }
    }

    // Store the setP
    sort(setP.begin(), setP.end());
    if ((int)(setP.size()) > chLimit)
        setP.resize(chLimit);
    *chCount = setP.size();
    for (unsigned i = 0; i < setP.size(); i++)
        chIndex[i] = setP[i].second;
    workModel->timer.recordFinish(HTICK_CHUZR1);
}

void HDualRHS::choose_multi_HGauto(int *chIndex, int *chCount, int chLimit) {
    // Automatically decide to use partition or not
    if (partSwitch)
        choose_multi_HGpart(chIndex, chCount, chLimit);
    else
        choose_multi_global(chIndex, chCount, chLimit);
}

void HDualRHS::choose_multi_HGpart(int *chIndex, int *chCount, int chLimit) {
    workModel->timer.recordStart(HTICK_CHUZR1);

    // Force to use partition method, unless doesn't exist
    if (partNum != chLimit) {
        choose_multi_global(chIndex, chCount, chLimit);
        partSwitch = 0;
        return;
    }

    // Initialise
    for (int i = 0; i < chLimit; i++)
        chIndex[i] = -1;
    *chCount = 0;

    int random = workModel->random.intRandom();
    if (workCount < 0) {
        // DENSE mode
        const int numRow = -workCount;
        int randomStart = random % numRow;
        vector<double> bestMerit(chLimit, 0);
        vector<int> bestIndex(chLimit, -1);
        for (int section = 0; section < 2; section++) {
            const int start = (section == 0) ? randomStart : 0;
            const int end = (section == 0) ? numRow : randomStart;
            for (int iRow = start; iRow < end; iRow++) {
                if (workArray[iRow] > HSOL_CONST_ZERO) {
                    int iPart = workPartition[iRow];
                    const double myInfeas = workArray[iRow];
                    const double myWeight = workDevex[iRow];
                    if (bestMerit[iPart] * myWeight < myInfeas) {
                        bestMerit[iPart] = myInfeas / myWeight;
                        bestIndex[iPart] = iRow;
                    }
                }
            }
        }
        int count = 0;
        for (int i = 0; i < chLimit; i++) {
            if (bestIndex[i] != -1) {
                chIndex[count++] = bestIndex[i];
            }
        }
        *chCount = count;

    } else {
        // SPARSE mode
        if (workCount == 0) {
            return;
        }

        int randomStart = random % workCount;
        vector<double> bestMerit(chLimit, 0);
        vector<int> bestIndex(chLimit, -1);
        for (int section = 0; section < 2; section++) {
            const int start = (section == 0) ? randomStart : 0;
            const int end = (section == 0) ? workCount : randomStart;
            for (int i = start; i < end; i++) {
                int iRow = workIndex[i];
                if (workArray[iRow] > HSOL_CONST_ZERO) {
                    int iPart = workPartition[iRow];
                    const double myInfeas = workArray[iRow];
                    const double myWeight = workDevex[iRow];
                    if (bestMerit[iPart] * myWeight < myInfeas) {
                        bestMerit[iPart] = myInfeas / myWeight;
                        bestIndex[iPart] = iRow;
                    }
                }
            }
        }
        int count = 0;
        for (int i = 0; i < chLimit; i++) {
            if (bestIndex[i] != -1) {
                chIndex[count++] = bestIndex[i];
            }
        }
        *chCount = count;
    }

    workModel->timer.recordFinish(HTICK_CHUZR1);
}

void HDualRHS::update_primal(HVector *column, double theta) {
    workModel->timer.recordStart(HTICK_UPDATE_PRIMAL);

    const int numRow = workModel->getNumRow();
    const int columnCount = column->count;
    const int *columnIndex = &column->index[0];
    const double *columnArray = &column->array[0];

    const double *baseLower = workModel->getBaseLower();
    const double *baseUpper = workModel->getBaseUpper();
    const double Tp = workModel->dblOption[DBLOPT_PRIMAL_TOL];

    double *baseValue = workModel->getBaseValue();

    bool updatePrimal_inDense = columnCount < 0 || columnCount > 0.4 * numRow;

    if (updatePrimal_inDense) {
        for (int iRow = 0; iRow < numRow; iRow++) {
            baseValue[iRow] -= theta * columnArray[iRow];
            const double value = baseValue[iRow];
            const double less = baseLower[iRow] - value;
            const double more = value - baseUpper[iRow];
            double infeas = less > Tp ? less : (more > Tp ? more : 0);
            workArray[iRow] = infeas * infeas;
        }
    } else {
        for (int i = 0; i < columnCount; i++) {
            int iRow = columnIndex[i];
            baseValue[iRow] -= theta * columnArray[iRow];
            const double value = baseValue[iRow];
            const double less = baseLower[iRow] - value;
            const double more = value - baseUpper[iRow];
            double infeas = less > Tp ? less : (more > Tp ? more : 0);
            workArray[iRow] = infeas * infeas;
        }
    }

    workModel->timer.recordFinish(HTICK_UPDATE_PRIMAL);
}

void HDualRHS::update_weight(HVector *column, double devex, double Kai, double *dseArray) {
    workModel->timer.recordStart(HTICK_UPDATE_WEIGHT);

    const int numRow = workModel->getNumRow();
    const int columnCount = column->count;
    const int *columnIndex = &column->index[0];
    const double *columnArray = &column->array[0];

    bool updateWeight_inDense = columnCount < 0 || columnCount > 0.4 * numRow;
    if (updateWeight_inDense) {
        for (int iRow = 0; iRow < numRow; iRow++) {
            const double val = columnArray[iRow];
            workDevex[iRow] += val * (devex * val + Kai * dseArray[iRow]);
            if (workDevex[iRow] < 1e-4)
                workDevex[iRow] = 1e-4;
        }
    } else {
        for (int i = 0; i < columnCount; i++) {
            const int iRow = columnIndex[i];
            const double val = columnArray[iRow];
            workDevex[iRow] += val * (devex * val + Kai * dseArray[iRow]);
            if (workDevex[iRow] < 1e-4)
                workDevex[iRow] = 1e-4;
        }
    }
    workModel->timer.recordFinish(HTICK_UPDATE_WEIGHT);
}

void HDualRHS::update_pivots(int iRow, double value) {
    const double *baseLower = workModel->getBaseLower();
    const double *baseUpper = workModel->getBaseUpper();
    const double Tp = workModel->dblOption[DBLOPT_PRIMAL_TOL];
    double *baseValue = workModel->getBaseValue();
    baseValue[iRow] = value;
    double pivotInfeas = 0;
    if (baseValue[iRow] < baseLower[iRow] - Tp)
        pivotInfeas = baseValue[iRow] - baseLower[iRow];
    if (baseValue[iRow] > baseUpper[iRow] + Tp)
        pivotInfeas = baseValue[iRow] - baseUpper[iRow];
    workArray[iRow] = pivotInfeas * pivotInfeas;
}

void HDualRHS::update_infeasList(HVector *column) {
    workModel->timer.recordStart(HTICK_UPDATE_PRIMAL);

    const int columnCount = column->count;
    const int *columnIndex = &column->index[0];

    // DENSE mode: disabled
    if (workCount < 0)
        return;

    if (workCutoff <= 0) {
        // The regular sparse way
        for (int i = 0; i < columnCount; i++) {
            int iRow = columnIndex[i];
            if (workMark[iRow] == 0) {
                if (workArray[iRow]) {
                    workIndex[workCount++] = iRow;
                    workMark[iRow] = 1;
                }
            }
        }
    } else {
        // The hyper sparse way
        for (int i = 0; i < columnCount; i++) {
            int iRow = columnIndex[i];
            if (workMark[iRow] == 0) {
                if (workArray[iRow] > workDevex[iRow] * workCutoff) {
                    workIndex[workCount++] = iRow;
                    workMark[iRow] = 1;
                }
            }
        }
    }

    workModel->timer.recordFinish(HTICK_UPDATE_PRIMAL);
}

void HDualRHS::create_infeasArray() {
    int numRow = workModel->getNumRow();
    const double *baseValue = workModel->getBaseValue();
    const double *baseLower = workModel->getBaseLower();
    const double *baseUpper = workModel->getBaseUpper();
    const double Tp = workModel->dblOption[DBLOPT_PRIMAL_TOL];
    for (int i = 0; i < numRow; i++) {
        const double value = baseValue[i];
        const double less = baseLower[i] - value;
        const double more = value - baseUpper[i];
        double infeas = less > Tp ? less : (more > Tp ? more : 0);
        workArray[i] = infeas * infeas;
    }
}

void HDualRHS::create_infeasList(double columnDensity) {
    int numRow = workModel->getNumRow();
    double *dwork = &workDevexFull[0];

    // 1. Build the full list
    fill_n(&workMark[0], numRow, 0);
    workCount = 0;
    workCutoff = 0;
    for (int iRow = 0; iRow < numRow; iRow++) {
        if (workArray[iRow]) {
            workMark[iRow] = 1;
            workIndex[workCount++] = iRow;
        }
    }

    // 2. See if it worth to try to go sparse
    //    (Many candidates, really sparse RHS)
    if (workCount > max(numRow * 0.01, 500.0) && columnDensity < 0.05) {
        int icutoff = max(workCount * 0.001, 500.0);
        double maxMerit = 0;
        for (int iRow = 0, iPut = 0; iRow < numRow; iRow++)
            if (workMark[iRow]) {
                double myMerit = workArray[iRow] / workDevex[iRow];
                if (maxMerit < myMerit)
                    maxMerit = myMerit;
                dwork[iPut++] = -myMerit;
            }
        nth_element(dwork, dwork + icutoff, dwork + workCount);
        double cutMerit = -dwork[icutoff];
        workCutoff = min(maxMerit * 0.99999, cutMerit * 1.00001);

        // Create again
        fill_n(&workMark[0], numRow, 0);
        workCount = 0;
        for (int iRow = 0; iRow < numRow; iRow++) {
            if (workArray[iRow] >= workDevex[iRow] * workCutoff) {
                workIndex[workCount++] = iRow;
                workMark[iRow] = 1;
            }
        }

        // Reduce by drop smaller
        if (workCount > icutoff * 1.5) {
            // Firstly take up "icutoff" number of elements
            int fullCount = workCount;
            workCount = icutoff;
            for (int i = icutoff; i < fullCount; i++) {
                int iRow = workIndex[i];
                if (workArray[iRow] > workDevex[iRow] * cutMerit) {
                    workIndex[workCount++] = iRow;
                } else {
                    workMark[iRow] = 0;
                }
            }
        }

        //        cerr
        //                << "======================================================> WORK COUNT = "
        //                << workCount << "\t icutoff = " << icutoff << "\t maxMerit = "
        //                << maxMerit << "\t cutMerit = " << cutMerit << endl;
    }

    // 3. If there is still too much candidates: disable them
    if (workCount > 0.2 * numRow) {
        workCount = -numRow;
        workCutoff = 0;
    }
}

// Upstream file: HDualRow.cpp

using namespace std;

void HDualRow::setupSlice(HModel *model, int size) {
    // Copy pointer
    workModel = model;
    workSize = size;
    workMove = model->getNonbasicMove();
    workDual = model->getWorkDual();
    workRange = model->getWorkRange();

    // Allocate spaces
    packCount = 0;
    packIndex.resize(workSize);
    packValue.resize(workSize);

    workCount = 0;
    workData.resize(workSize);
}

void HDualRow::setup(HModel *model) {
    // Setup common vectors
    setupSlice(model, model->getNumTot());
    workRand = model->getWorkIntBreak();
}

void HDualRow::clear() {
    packCount = 0;
    workCount = 0;
}

void HDualRow::choose_makepack(const HVector *row, const int offset) {
    /**
     * Pack the index array to the row
     * Can be parallel
     */
    const int rowCount = row->count;
    const int *rowIndex = &row->index[0];
    const double *rowArray = &row->array[0];

    for (int i = 0; i < rowCount; i++) {
        const int index = rowIndex[i];
        const double value = rowArray[index];
        packIndex[packCount] = index + offset;
        packValue[packCount++] = value;
    }
}

void HDualRow::choose_possible() {
    /**
     * Will determine the possible variables
     * Can be parallel.
     */
    const double Ta = workModel->countUpdate < 10   ? 1e-9
                      : workModel->countUpdate < 20 ? 3e-8
                                                    : 1e-6;
    const double Td = workModel->dblOption[DBLOPT_DUAL_TOL];
    const int sourceOut = workDelta < 0 ? -1 : 1;
    workTheta = HSOL_CONST_INF;
    workCount = 0;
    for (int i = 0; i < packCount; i++) {
        const int iCol = packIndex[i];
        const int move = workMove[iCol];
        const double alpha = packValue[i] * sourceOut * move;
        if (alpha > Ta) {
            workData[workCount++] = make_pair(iCol, alpha);
            const double relax = workDual[iCol] * move + Td;
            if (workTheta * alpha > relax)
                workTheta = relax / alpha;
        }
    }
}

void HDualRow::choose_joinpack(const HDualRow *otherRow) {
    /**
     * Will join the possible pack
     * Must be sequentail
     */
    const int otherCount = otherRow->workCount;
    const pair<int, double> *otherData = &otherRow->workData[0];
    copy(otherData, otherData + otherCount, &workData[workCount]);
    workCount = workCount + otherCount;
    workTheta = min(workTheta, otherRow->workTheta);
}

void HDualRow::choose_final() {
    /**
     * This routine choose the dual entering variable by
     * BFRT and EXPAND. (In sequential)
     *
     * It will
     * (1) reduce the candidates as a small collection
     * (2) choose by BFRT by going over break points
     * (3) choose final by alpha
     * (4) determine final flip variables
     */

    // 1. Reduce by large step BFRT
    workModel->timer.recordStart(HTICK_CHUZC2);
    int fullCount = workCount;
    workCount = 0;
    double totalChange = 0;
    double totalDelta = fabs(workDelta);
    double selectTheta = 10 * workTheta + 1e-7;
    for (;;) {
        for (int i = workCount; i < fullCount; i++) {
            int iCol = workData[i].first;
            double alpha = workData[i].second;
            double tight = workMove[iCol] * workDual[iCol];
            if (alpha * selectTheta >= tight) {
                swap(workData[workCount++], workData[i]);
                totalChange += workRange[iCol] * alpha;
            }
        }
        selectTheta *= 10;
        if (totalChange >= totalDelta || workCount == fullCount)
            break;
    }
    workModel->timer.recordFinish(HTICK_CHUZC2);

    // 2. Choose by small step BFRT
    workModel->timer.recordStart(HTICK_CHUZC3);
    const double Td = workModel->dblOption[DBLOPT_DUAL_TOL];
    fullCount = workCount;
    workCount = 0;
    totalChange = 1e-12;
    selectTheta = workTheta;
    workGroup.clear();
    workGroup.push_back(0);
    while (selectTheta < 1e18) {
        double remainTheta = 1e100;
        for (int i = workCount; i < fullCount; i++) {
            int iCol = workData[i].first;
            double value = workData[i].second;
            double dual = workMove[iCol] * workDual[iCol];
            // Tight satisfy
            if (dual <= selectTheta * value) {
                swap(workData[workCount++], workData[i]);
                totalChange += value * (workRange[iCol]);
            } else if (dual + Td < remainTheta * value) {
                remainTheta = (dual + Td) / value;
            }
        }
        workGroup.push_back(workCount);
        selectTheta = remainTheta;
        if (totalChange >= totalDelta || workCount == fullCount)
            break;
    }

    // 3. Choose large alpha
    double finalCompare = 0;
    for (int i = 0; i < workCount; i++)
        finalCompare = max(finalCompare, workData[i].second);
    finalCompare = min(0.1 * finalCompare, 1.0);
    int countGroup = workGroup.size() - 1;
    int breakGroup = -1;
    int breakIndex = -1;
    for (int iGroup = countGroup - 1; iGroup >= 0; iGroup--) {
        double dMaxFinal = 0;
        int iMaxFinal = -1;
        for (int i = workGroup[iGroup]; i < workGroup[iGroup + 1]; i++) {
            if (dMaxFinal < workData[i].second) {
                dMaxFinal = workData[i].second;
                iMaxFinal = i;
            } else if (dMaxFinal == workData[i].second) {
                int jCol = workData[iMaxFinal].first;
                int iCol = workData[i].first;
                if (workRand[iCol] < workRand[jCol]) {
                    iMaxFinal = i;
                }
            }
        }

        if (workData[iMaxFinal].second > finalCompare) {
            breakIndex = iMaxFinal;
            breakGroup = iGroup;
            break;
        }
    }

    int sourceOut = workDelta < 0 ? -1 : 1;
    workPivot = workData[breakIndex].first;
    workAlpha = workData[breakIndex].second * sourceOut * workMove[workPivot];
    if (workDual[workPivot] * workMove[workPivot] > 0)
        workTheta = workDual[workPivot] / workAlpha;
    else
        workTheta = 0;

    // 4. Determine BFRT flip index: flip all
    fullCount = breakIndex;
    workCount = 0;
    for (int i = 0; i < workGroup[breakGroup]; i++) {
        const int iCol = workData[i].first;
        const int move = workMove[iCol];
        workData[workCount++] = make_pair(iCol, move * workRange[iCol]);
    }
    if (workTheta == 0)
        workCount = 0;
    sort(workData.begin(), workData.begin() + workCount);
    workModel->timer.recordFinish(HTICK_CHUZC3);
}

void HDualRow::update_flip(HVector *bfrtColumn) {
    bfrtColumn->clear();
    for (int i = 0; i < workCount; i++) {
        const int iCol = workData[i].first;
        const double change = workData[i].second;
        workModel->flipBound(iCol);
        workModel->getMatrix()->collect_aj(*bfrtColumn, iCol, change);
    }
}

void HDualRow::update_dual(double theta) {
    workModel->timer.recordStart(HTICK_UPDATE_DUAL);
    double *workDual = workModel->getWorkDual();
    for (int i = 0; i < packCount; i++)
        workDual[packIndex[i]] -= theta * packValue[i];
    workModel->timer.recordFinish(HTICK_UPDATE_DUAL);
}

void HDualRow::create_Freelist() {
    freeList.clear();
    const int *nonbasicFlag = workModel->getNonbasicFlag();
    for (int i = 0; i < workModel->getNumTot(); i++) {
        if (nonbasicFlag[i] && workRange[i] > 1.5 * HSOL_CONST_INF)
            freeList.insert(i);
    }
}

void HDualRow::create_Freemove(HVector *row_ep) {
    if (!freeList.empty()) {
        double Ta = workModel->countUpdate < 10 ? 1e-9 : workModel->countUpdate < 20 ? 3e-8 : 1e-6;
        int sourceOut = workDelta < 0 ? -1 : 1;
        set<int>::iterator sit;
        for (sit = freeList.begin(); sit != freeList.end(); sit++) {
            int iCol = *sit;
            assert(iCol < workModel->getNumCol());
            double alpha = workModel->getMatrix()->compute_dot(*row_ep, iCol);
            if (fabs(alpha) > Ta) {
                if (alpha * sourceOut > 0)
                    workModel->getNonbasicMove()[iCol] = 1;
                else
                    workModel->getNonbasicMove()[iCol] = -1;
            }
        }
    }
}
void HDualRow::delete_Freemove() {
    if (!freeList.empty()) {
        set<int>::iterator sit;
        for (sit = freeList.begin(); sit != freeList.end(); sit++) {
            int iCol = *sit;
            assert(iCol < workModel->getNumCol());
            workModel->getNonbasicMove()[iCol] = 0;
        }
    }
}

void HDualRow::delete_Freelist(int iColumn) {
    if (!freeList.empty()) {
        if (freeList.count(iColumn))
            freeList.erase(iColumn);
    }
}

// Upstream file: HFactor.cpp

using namespace std;

void solveMatrixT(const int Xstart, const int Xend, const int Ystart, const int Yend,
                  const int *Tindex, const double *Tvalue, const double Tpivot, int *RHScount,
                  int *RHSindex, double *RHSarray) {

    // Collect by X
    double pivotX = 0;
    for (int k = Xstart; k < Xend; k++)
        pivotX += Tvalue[k] * RHSarray[Tindex[k]];

    // Scatter by Y
    if (fabs(pivotX) > HSOL_CONST_TINY) {
        int workCount = *RHScount;

        pivotX /= Tpivot;
        for (int k = Ystart; k < Yend; k++) {
            const int index = Tindex[k];
            const double value0 = RHSarray[index];
            const double value1 = value0 - pivotX * Tvalue[k];
            if (value0 == 0)
                RHSindex[workCount++] = index;
            RHSarray[index] = (fabs(value1) < HSOL_CONST_TINY) ? HSOL_CONST_ZERO : value1;
        }

        *RHScount = workCount;
    }
}

void solveHyper(const int Hsize, const int *Hlookup, const int *HpivotIndex,
                const double *HpivotValue, const int *Hstart, const int *Hend, const int *Hindex,
                const double *Hvalue, HVector *rhs) {

    int RHS_Tick = rhs->pseudoTick;
    int RHScount = rhs->count;
    int *RHSindex = &rhs->index[0];
    double *RHSarray = &rhs->array[0];

    // Take count

    // Build list
    char *listMark = &rhs->cwork[0];
    int *listIndex = &rhs->iwork[0];
    int *listStack = &rhs->iwork[Hsize];
    int listCount = 0;

    int countPivot = 0;
    int countEntry = 0;

    for (int i = 0; i < RHScount; i++) {
        // Skip touched index
        int iTrans = Hlookup[RHSindex[i]]; // XXX: this contains a bug iTran
        if (listMark[iTrans])              // XXX bug here
            continue;

        int Hi = iTrans;     // H matrix pivot index
        int Hk = Hstart[Hi]; // H matrix non zero position
        int nStack = -1;     // Usage of the stack (-1 not used)

        listMark[Hi] = 1; // Mark this as touched

        for (;;) {
            if (Hk < Hend[Hi]) {
                int Hi_sub = Hlookup[Hindex[Hk++]];
                if (listMark[Hi_sub] == 0) {  // Go to a child
                    listMark[Hi_sub] = 1;     // Mark as touched
                    listStack[++nStack] = Hi; // Store current into stack
                    listStack[++nStack] = Hk;
                    Hi = Hi_sub; // Replace current with child
                    Hk = Hstart[Hi];
                    if (Hi >= Hsize) {
                        countPivot++;
                        countEntry += Hend[Hi] - Hstart[Hi];
                        RHS_Tick += 10;
                    }
                }
            } else {
                listIndex[listCount++] = Hi;
                if (nStack == -1) // Quit on empty stack
                    break;
                Hk = listStack[nStack--]; // Back to last in stack
                Hi = listStack[nStack--];
            }
        }
    }

    rhs->fakeTick += countPivot * 20 + countEntry * 10;

    // Solve with list
    if (HpivotValue == 0) {
        RHScount = 0;
        for (int iList = listCount - 1; iList >= 0; iList--) {
            int i = listIndex[iList];
            listMark[i] = 0;
            int pivotRow = HpivotIndex[i];
            double pivotX = RHSarray[pivotRow];
            if (fabs(pivotX) > HSOL_CONST_TINY) {
                RHSindex[RHScount++] = pivotRow;
                const int start = Hstart[i];
                const int end = Hend[i];
                RHS_Tick += end - start;
                for (int k = start; k < end; k++)
                    RHSarray[Hindex[k]] -= pivotX * Hvalue[k];
            } else
                RHSarray[pivotRow] = 0;
        }
        rhs->pseudoTick = RHS_Tick;
        rhs->count = RHScount;
    } else {
        RHScount = 0;
        for (int iList = listCount - 1; iList >= 0; iList--) {
            int i = listIndex[iList];
            listMark[i] = 0;
            int pivotRow = HpivotIndex[i];
            double pivotX = RHSarray[pivotRow];
            if (fabs(pivotX) > HSOL_CONST_TINY) {
                pivotX /= HpivotValue[i];
                RHSarray[pivotRow] = pivotX;
                RHSindex[RHScount++] = pivotRow;
                const int start = Hstart[i];
                const int end = Hend[i];
                RHS_Tick += end - start;
                for (int k = start; k < end; k++)
                    RHSarray[Hindex[k]] -= pivotX * Hvalue[k];
            } else
                RHSarray[pivotRow] = 0;
        }
        rhs->pseudoTick = RHS_Tick;
        rhs->count = RHScount;
    }
}

void HFactor::setup(int numCol_, int numRow_, int *Astart_, int *Aindex_, double *Avalue_,
                    int *baseIndex_, int updateMethod_) {

    // Copy Problem size and (pointer to) coefficient matrix
    numRow = numRow_;
    numCol = numCol_;
    Astart = Astart_;
    Aindex = Aindex_;
    Avalue = Avalue_;
    baseIndex = baseIndex_;
    updateMethod = updateMethod_;

    // Allocate for working buffer
    iwork.reserve(numRow * 2);
    dwork.assign(numRow, 0);

    // Find Basis matrix limit size
    int BlimitX = 0;
    iwork.assign(numRow + 1, 0);
    for (int i = 0; i < numCol; i++)
        iwork[Astart[i + 1] - Astart[i]]++;
    // A basis contains at most numRow structural columns. Count only the
    // remaining slots in the last degree bucket, not the entire bucket.
    for (int i = numRow, counted = 0; i >= 0 && counted < numRow; i--) {
        const int take = std::min(numRow - counted, iwork[i]);
        BlimitX += i * take;
        counted += take;
    }
    BlimitX += numRow;

    // Allocate spaces for basis matrix, L, U factor and Update buffer
    Bstart.resize(numRow + 1, 0);
    Bindex.resize(BlimitX);
    Bvalue.resize(BlimitX);

    // Allocate spaces for pivot records
    permute.resize(numRow);

    // Allocate spaces for Markowitz matrices
    MCstart.resize(numRow);
    MCcountA.resize(numRow);
    MCcountN.resize(numRow);
    MCspace.resize(numRow);
    MCminpivot.resize(numRow);
    MCindex.resize(BlimitX * 2);
    MCvalue.resize(BlimitX * 2);

    MRstart.resize(numRow);
    MRcount.resize(numRow);
    MRspace.resize(numRow);
    MRcountb4.resize(numRow);
    MRindex.resize(BlimitX * 2);

    McolumnMark.assign(numRow, 0);
    McolumnIndex.resize(numRow);
    McolumnArray.assign(numRow, 0);

    // Allocate spaces for count-link-list
    clinkFirst.assign(numRow + 1, -1);
    clinkNext.resize(numRow);
    clinkLast.resize(numRow);

    rlinkFirst.assign(numRow + 1, -1);
    rlinkNext.resize(numRow);
    rlinkLast.resize(numRow);

    // Allocate spaces for L factor
    LpivotLookup.resize(numRow);
    LpivotIndex.reserve(numRow);
    Lstart.reserve(numRow + 1);
    Lindex.reserve(BlimitX * 3);
    Lvalue.reserve(BlimitX * 3);

    LRstart.reserve(numRow + 1);
    LRindex.reserve(BlimitX * 3);
    LRvalue.reserve(BlimitX * 3);

    // Allocate spaces for U factor
    UpivotLookup.resize(numRow);
    UpivotIndex.reserve(numRow + 1000);
    UpivotValue.reserve(numRow + 1000);

    Ustart.reserve(numRow + 1000 + 1);
    Ulastp.reserve(numRow + 1000);
    Uindex.reserve(BlimitX * 3);
    Uvalue.reserve(BlimitX * 3);

    URstart.reserve(numRow + 1000 + 1);
    URlastp.reserve(numRow + 1000);
    URspace.reserve(numRow + 1000);
    URindex.reserve(BlimitX * 3);
    URvalue.reserve(BlimitX * 3);

    // Allocate spaces for Update buffer
    PFpivotValue.reserve(1000);
    PFpivotIndex.reserve(1000);
    PFstart.reserve(2000 + 1);
    PFindex.reserve(BlimitX * 4);
    PFvalue.reserve(BlimitX * 4);
}

void HFactor::build() {
    check_deadline();
    pseudoTick = 0;

    HTimer timer;
    timer.reset();

    // Build the L, U factor
    fakeTick = 0;
    realTick = timer.getTick();
    buildSimple();
    buildKernel();
    buildFinish();

    realTick = timer.getTick() - realTick;

    // I can't believe I made this bug!!!
    pseudoTick *= 8;

    //    fprintf(stderr,"realTick = %10.0f, COMPARE = %10.4f  COMPAREX = %10.4f\n", realTick,
    //            (realTick) / (fakeTick), pseudoTick / fakeTick);
}

void HFactor::ftran(HVector &vector, double hist_dsty) const {
    ftranL(vector, hist_dsty);
    ftranU(vector, hist_dsty);
}

void HFactor::btran(HVector &vector, double hist_dsty) const {
    btranU(vector, hist_dsty);
    btranL(vector, hist_dsty);
}

void HFactor::update(HVector *aq, HVector *ep, int *iRow, int *hint) {
    // Special case
    if (aq->next) {
        updateCFT(aq, ep, iRow, hint);
        return;
    }

    if (updateMethod == UPDATE_METHOD_FT)
        updateFT(aq, ep, *iRow, hint);
    if (updateMethod == UPDATE_METHOD_PF)
        updatePF(aq, ep, *iRow, hint);
    if (updateMethod == UPDATE_METHOD_MPF)
        updateMPF(aq, ep, *iRow, hint);
    if (updateMethod == UPDATE_METHOD_APF)
        updateAPF(aq, ep, *iRow, hint);
}

void HFactor::buildSimple() {
    /**
     * 0. Clear L and U factor
     */
    Lstart.clear();
    Lstart.push_back(0);
    Lindex.clear();
    Lvalue.clear();

    UpivotIndex.clear();
    UpivotValue.clear();
    Ustart.clear();
    Ustart.push_back(0);
    Uindex.clear();
    Uvalue.clear();

    /**
     * 1. Prepare basis matrix and deal with unit columns
     */

    int BcountX = 0;
    fill_n(&MRcountb4[0], numRow, 0);
    nwork = 0;
    for (int iCol = 0; iCol < numRow; iCol++) {
        int iMat = baseIndex[iCol];
        int iRow = -1;
        if (iMat >= numCol) {
            // 1.1 Logical column
            // Check for double pivot
            int lc_iRow = iMat - numCol;
            if (MRcountb4[lc_iRow] >= 0) {
                iRow = lc_iRow;
            } else {
                fprintf(stderr, "STRANGE: Found a logical column with pivot already in row %d\n",
                        lc_iRow);
                MRcountb4[lc_iRow]++;
                Bindex[BcountX] = lc_iRow;
                Bvalue[BcountX++] = 1.0;
                iwork[nwork++] = iCol;
            }
        } else {
            // 1.2 Structural column
            int start = Astart[iMat];
            int count = Astart[iMat + 1] - start;
            int lc_iRow = Aindex[start];
            // Check for unit column with double pivot
            bool unit_column = count == 1 && Avalue[start] == 1;
            if (unit_column && MRcountb4[lc_iRow] >= 0) {
                iRow = lc_iRow;
            } else {
                if (unit_column)
                    fprintf(stderr, "STRANGE: Found a second unit column with pivot in row %d\n",
                            lc_iRow);
                for (int k = start; k < start + count; k++) {
                    MRcountb4[Aindex[k]]++;
                    Bindex[BcountX] = Aindex[k];
                    Bvalue[BcountX++] = Avalue[k];
                }
                iwork[nwork++] = iCol;
            }
        }

        if (iRow >= 0) {
            // 1.3 Record unit column
            //fprintf(stderr,"Found unit column (Row, Col) = (%d, %d)\n", Lindex.size(), Uindex.size());
            permute[iCol] = iRow;
            Lstart.push_back(Lindex.size());
            UpivotIndex.push_back(iRow);
            UpivotValue.push_back(1);
            Ustart.push_back(Uindex.size());
            MRcountb4[iRow] = -numRow;
            // fprintf(stderr,"Found unit column (Row, Col) = (%d, %d)\n", iRow, iCol);
        }
        Bstart[iCol + 1] = BcountX;
    }
    BtotalX = numRow - nwork + BcountX;
    pseudoTick += (numRow - nwork) * 2;
    pseudoTick += BcountX * 1.5;

    //    count1 = 0;
    // Comments: for pds-20, dfl001: 60 / 80
    // Comments: when system is large: enlarge
    // Comments: when system is small: decrease
    fakeTick += BcountX * 60 + (numRow - nwork) * 80;

    /**
     * 2. Search for and deal with singletons
     */
    double t2_search = 0;
    double t2_storeL = Lindex.size();
    double t2_storeU = Uindex.size();
    double t2_storep = nwork;
    while (nwork > 0) {
        int nworkLast = nwork;
        nwork = 0;
        for (int i = 0; i < nworkLast; i++) {
            const int iCol = iwork[i];
            const int start = Bstart[iCol];
            const int end = Bstart[iCol + 1];
            int pivot_k = -1;
            int found_row_singleton = 0;
            int count = 0;

            // 2.1 Search for singleton
            t2_search += end - start;
            pseudoTick += (end - start) * 0.5;
            for (int k = start; k < end; k++) {
                const int iRow = Bindex[k];
                if (MRcountb4[iRow] == 1) {
                    pivot_k = k;
                    found_row_singleton = 1;
                    break;
                }
                if (MRcountb4[iRow] > 1) {
                    pivot_k = k;
                    count++;
                }
            }

            if (found_row_singleton) {
                // 2.2 Deal with row singleton
                const double pivotX = 1 / Bvalue[pivot_k];
                for (int section = 0; section < 2; section++) {
                    int p0 = section == 0 ? start : pivot_k + 1;
                    int p1 = section == 0 ? pivot_k : end;
                    for (int k = p0; k < p1; k++) {
                        int iRow = Bindex[k];
                        if (MRcountb4[iRow] > 0) {
                            Lindex.push_back(iRow);
                            Lvalue.push_back(Bvalue[k] * pivotX);
                        } else {
                            Uindex.push_back(iRow);
                            Uvalue.push_back(Bvalue[k]);
                        }
                        MRcountb4[iRow]--;
                    }
                }
                int iRow = Bindex[pivot_k];
                MRcountb4[iRow] = 0;
                permute[iCol] = iRow;
                Lstart.push_back(Lindex.size());

                UpivotIndex.push_back(iRow);
                UpivotValue.push_back(Bvalue[pivot_k]);
                Ustart.push_back(Uindex.size());
            } else if (count == 1) {
                // 2.3 Deal with column singleton
                for (int k = start; k < pivot_k; k++) {
                    Uindex.push_back(Bindex[k]);
                    Uvalue.push_back(Bvalue[k]);
                }
                for (int k = pivot_k + 1; k < end; k++) {
                    Uindex.push_back(Bindex[k]);
                    Uvalue.push_back(Bvalue[k]);
                }

                int iRow = Bindex[pivot_k];
                MRcountb4[iRow] = 0;
                permute[iCol] = iRow;
                Lstart.push_back(Lindex.size());

                UpivotIndex.push_back(iRow);
                UpivotValue.push_back(Bvalue[pivot_k]);
                Ustart.push_back(Uindex.size());
            } else {
                iwork[nwork++] = iCol;
            }
        }

        // No singleton found in the last pass
        pseudoTick += (nworkLast - nwork) * 2; // For pivot
        if (nworkLast == nwork)
            break;
    }
    t2_storeL = Lindex.size() - t2_storeL;
    t2_storeU = Uindex.size() - t2_storeU;
    t2_storep = t2_storep - nwork;

    fakeTick += t2_search * 20 + (t2_storep + t2_storeL + t2_storeU) * 80;
    pseudoTick += (Lindex.size() + Uindex.size()) * 1.5; // For factor

    /**
     * 3. Prepare the kernel parts
     */
    // 3.1 Prepare row links, row matrix spaces
    rlinkFirst.assign(numRow + 1, -1);
    MRcount.assign(numRow, 0);
    int MRcountX = 0;
    for (int iRow = 0; iRow < numRow; iRow++) {
        int count = MRcountb4[iRow];
        if (count > 0) {
            MRstart[iRow] = MRcountX;
            MRspace[iRow] = count * 2;
            MRcountX += count * 2;
            rlinkAdd(iRow, count);
        }
    }
    MRindex.resize(MRcountX);

    // 3.2 Prepare column links, kernel matrix
    clinkFirst.assign(numRow + 1, -1);
    MCindex.clear();
    MCvalue.clear();
    MCcountA.assign(numRow, 0);
    MCcountN.assign(numRow, 0);
    int MCcountX = 0;
    for (int i = 0; i < nwork; i++) {
        int iCol = iwork[i];
        MCstart[iCol] = MCcountX;
        MCspace[iCol] = (Bstart[iCol + 1] - Bstart[iCol]) * 2;
        MCcountX += MCspace[iCol];
        MCindex.resize(MCcountX);
        MCvalue.resize(MCcountX);

        for (int k = Bstart[iCol]; k < Bstart[iCol + 1]; k++) {
            const int iRow = Bindex[k];
            const double value = Bvalue[k];
            if (MRcountb4[iRow] > 0) {
                colInsert(iCol, iRow, value);
                rowInsert(iCol, iRow);
            } else {
                colStoreN(iCol, iRow, value);
            }
        }
        colFixMax(iCol);
        clinkAdd(iCol, MCcountA[iCol]);
    }

    pseudoTick += nwork * 2; // DLL
    pseudoTick += MCcountX;  // MATRIX

    fakeTick += (numRow + nwork + MCcountX) * 40 + MRcountX * 20;
}

void HFactor::buildKernel() {
    // Deal with the kernel part by 'n-work' pivoting

    double fake_search = 0;
    double fake_fill = 0;
    double fake_eliminate = 0;

    while (nwork-- > 0) {
        /**
         * 1. Search for the pivot
         */
        int jColPivot = -1;
        int iRowPivot = -1;

        // 1.1. Setup search merits
        int searchLimit = min(nwork, 8);
        int searchCount = 0;
        double meritLimit = 1.0 * numRow * numRow;
        double meritPivot = meritLimit;

        // 1.2. Search for local singletons
        bool foundPivot = false;
        if (!foundPivot && clinkFirst[1] != -1) {
            jColPivot = clinkFirst[1];
            iRowPivot = MCindex[MCstart[jColPivot]];
            foundPivot = true;
        }
        if (!foundPivot && rlinkFirst[1] != -1) {
            iRowPivot = rlinkFirst[1];
            jColPivot = MRindex[MRstart[iRowPivot]];
            foundPivot = true;
        }

        // 1.3. Major search loop
        for (int count = 2; !foundPivot && count <= numRow; count++) {
            // 1.3.1 Search for columns
            for (int j = clinkFirst[count]; j != -1; j = clinkNext[j]) {
                double minpivot = MCminpivot[j];
                int start = MCstart[j];
                int end = start + MCcountA[j];
                pseudoTick += MCcountA[j] * 0.3;
                for (int k = start; k < end; k++) {
                    if (fabs(MCvalue[k]) >= minpivot) {
                        int i = MCindex[k];
                        int rowCount = MRcount[i];
                        double meritLocal = 1.0 * (count - 1) * (rowCount - 1);
                        if (meritPivot > meritLocal) {
                            meritPivot = meritLocal;
                            jColPivot = j;
                            iRowPivot = i;
                            foundPivot = foundPivot || (rowCount < count);
                        }
                    }
                }

                if (searchCount++ >= searchLimit && meritPivot < meritLimit)
                    foundPivot = true;
                if (foundPivot)
                    break;

                fake_search += count;
            }

            // 1.3.2 Search for rows
            for (int i = rlinkFirst[count]; i != -1; i = rlinkNext[i]) {
                int start = MRstart[i];
                int end = start + MRcount[i];
                pseudoTick += MRcount[i] * 0.3;
                for (int k = start; k < end; k++) {
                    int j = MRindex[k];
                    int columnCount = MCcountA[j];
                    double meritLocal = 1.0 * (count - 1) * (columnCount - 1);
                    if (meritLocal < meritPivot) {
                        int ifind = MCstart[j];
                        while (MCindex[ifind] != i)
                            ifind++;
                        pseudoTick += columnCount * 0.05;
                        if (fabs(MCvalue[ifind]) >= MCminpivot[j]) {
                            meritPivot = meritLocal;
                            jColPivot = j;
                            iRowPivot = i;
                            foundPivot = foundPivot || (columnCount <= count);
                        }
                    }
                }
                if (searchCount++ >= searchLimit && meritPivot < meritLimit)
                    foundPivot = true;
                if (foundPivot)
                    break;
            }

            fake_search += count;
        }

        // 1.4. If we found nothing: tell singular
        if (!foundPivot)
            throw runtime_error("singular-basis-matrix");

        /**
         * 2. Elimination other elements by the pivot
         */

        // XXX NEED TO UPDATE THIS
        pseudoTick += MCcountA[jColPivot] * 2 + MCcountN[jColPivot];

        // 2.1. Delete the pivot
        double pivotX = colDelete(jColPivot, iRowPivot);
        rowDelete(jColPivot, iRowPivot);
        clinkDel(jColPivot);
        rlinkDel(iRowPivot);
        permute[jColPivot] = iRowPivot;

        // 2.2. Store active pivot column to L
        int start_A = MCstart[jColPivot];
        int end_A = start_A + MCcountA[jColPivot];
        int McolumnCount = 0;
        for (int k = start_A; k < end_A; k++) {
            const int iRow = MCindex[k];
            const double value = MCvalue[k] / pivotX;
            McolumnIndex[McolumnCount++] = iRow;
            McolumnArray[iRow] = value;
            McolumnMark[iRow] = 1;
            Lindex.push_back(iRow);
            Lvalue.push_back(value);
            MRcountb4[iRow] = MRcount[iRow];
            rowDelete(jColPivot, iRow);
        }
        Lstart.push_back(Lindex.size());
        fake_fill += 2 * MCcountA[jColPivot];

        // 2.3. Store non active pivot column to U
        int end_N = start_A + MCspace[jColPivot];
        int start_N = end_N - MCcountN[jColPivot];
        for (int i = start_N; i < end_N; i++) {
            Uindex.push_back(MCindex[i]);
            Uvalue.push_back(MCvalue[i]);
        }
        UpivotIndex.push_back(iRowPivot);
        UpivotValue.push_back(pivotX);
        Ustart.push_back(Uindex.size());
        fake_fill += end_N - start_N;

        // 2.4. Loop over pivot row to eliminate other column
        const int row_start = MRstart[iRowPivot];
        const int row_end = row_start + MRcount[iRowPivot];
        for (int row_k = row_start; row_k < row_end; row_k++) {

            // 2.4.1. My pointer
            int iCol = MRindex[row_k];
            const int my_count = MCcountA[iCol];
            const int my_start = MCstart[iCol];
            const int my_end = my_start + my_count - 1;
            double my_pivot = colDelete(iCol, iRowPivot);
            colStoreN(iCol, iRowPivot, my_pivot);

            // 2.4.2. Elimination on the overlapping part
            int nFillin = McolumnCount;
            int nCancel = 0;
            pseudoTick += McolumnCount;
            for (int my_k = my_start; my_k < my_end; my_k++) {
                int iRow = MCindex[my_k];
                double value = MCvalue[my_k];
                if (McolumnMark[iRow]) {
                    McolumnMark[iRow] = 0;
                    nFillin--;
                    value -= my_pivot * McolumnArray[iRow];
                    if (fabs(value) < HSOL_CONST_TINY) {
                        value = 0;
                        nCancel++;
                    }
                    MCvalue[my_k] = value;
                }
            }
            fake_eliminate += McolumnCount;
            fake_eliminate += nFillin * 2;

            // 2.4.3. Remove cancellation gaps
            if (nCancel > 0) {
                int new_end = my_start;
                for (int my_k = my_start; my_k < my_end; my_k++) {
                    if (MCvalue[my_k] != 0) {
                        MCindex[new_end] = MCindex[my_k];
                        MCvalue[new_end++] = MCvalue[my_k];
                    } else {
                        rowDelete(iCol, MCindex[my_k]);
                    }
                }
                MCcountA[iCol] = new_end - my_start;
            }

            // 2.4.4. Insert fill-in
            if (nFillin > 0) {
                pseudoTick += nFillin * 5;
                // 2.4.4.1 Check column size
                if (MCcountA[iCol] + MCcountN[iCol] + nFillin > MCspace[iCol]) {
                    // p1&2=active, p3&4=non active, p5=new p1, p7=new p3
                    int p1 = MCstart[iCol];
                    int p2 = p1 + MCcountA[iCol];
                    int p3 = p1 + MCspace[iCol] - MCcountN[iCol];
                    int p4 = p1 + MCspace[iCol];
                    MCspace[iCol] += max(MCspace[iCol], nFillin);
                    int p5 = MCstart[iCol] = MCindex.size();
                    int p7 = p5 + MCspace[iCol] - MCcountN[iCol];
                    MCindex.resize(p5 + MCspace[iCol]);
                    MCvalue.resize(p5 + MCspace[iCol]);
                    copy(&MCindex[p1], &MCindex[p2], &MCindex[p5]);
                    copy(&MCvalue[p1], &MCvalue[p2], &MCvalue[p5]);
                    copy(&MCindex[p3], &MCindex[p4], &MCindex[p7]);
                    copy(&MCvalue[p3], &MCvalue[p4], &MCvalue[p7]);
                }

                // 2.4.4.2 Fill into column copy
                for (int i = 0; i < McolumnCount; i++) {
                    int iRow = McolumnIndex[i];
                    if (McolumnMark[iRow])
                        colInsert(iCol, iRow, -my_pivot * McolumnArray[iRow]);
                }

                // 2.4.4.3 Fill into the row copy
                for (int i = 0; i < McolumnCount; i++) {
                    int iRow = McolumnIndex[i];
                    if (McolumnMark[iRow]) {
                        // Expand row space
                        if (MRcount[iRow] == MRspace[iRow]) {
                            int p1 = MRstart[iRow];
                            int p2 = p1 + MRcount[iRow];
                            int p3 = MRstart[iRow] = MRindex.size();
                            MRspace[iRow] *= 2;
                            MRindex.resize(p3 + MRspace[iRow]);
                            copy(&MRindex[p1], &MRindex[p2], &MRindex[p3]);
                        }
                        rowInsert(iCol, iRow);
                    }
                }
            }

            // 2.4.5. Reset pivot column mark
            for (int i = 0; i < McolumnCount; i++)
                McolumnMark[McolumnIndex[i]] = 1;

            // 2.4.6. Fix max value and link list
            colFixMax(iCol);
            if (my_count != MCcountA[iCol]) {
                clinkDel(iCol);
                clinkAdd(iCol, MCcountA[iCol]);
            }
        }

        // 2.5. Clear pivot column buffer
        for (int i = 0; i < McolumnCount; i++)
            McolumnMark[McolumnIndex[i]] = 0;

        // 2.6. Correct row links for the remain active part
        for (int i = start_A; i < end_A; i++) {
            int iRow = MCindex[i];
            if (MRcountb4[iRow] != MRcount[iRow]) {
                rlinkDel(iRow);
                rlinkAdd(iRow, MRcount[iRow]);
            }
        }
    }
    fakeTick += fake_search * 20 + fake_fill * 160 + fake_eliminate * 80;
}

void HFactor::buildFinish() {
    // The look up table
    for (int i = 0; i < numRow; i++)
        UpivotLookup[UpivotIndex[i]] = i;
    LpivotIndex = UpivotIndex;
    LpivotLookup = UpivotLookup;

    // LR space
    int LcountX = Lindex.size();
    LRindex.resize(LcountX);
    LRvalue.resize(LcountX);

    // LR pointer
    iwork.assign(numRow, 0);
    for (int k = 0; k < LcountX; k++)
        iwork[LpivotLookup[Lindex[k]]]++;

    LRstart.assign(numRow + 1, 0);
    for (int i = 1; i <= numRow; i++)
        LRstart[i] = LRstart[i - 1] + iwork[i - 1];

    // LR elements
    iwork.assign(&LRstart[0], &LRstart[numRow]);
    for (int i = 0; i < numRow; i++) {
        const int index = LpivotIndex[i];
        for (int k = Lstart[i]; k < Lstart[i + 1]; k++) {
            int iRow = LpivotLookup[Lindex[k]];
            int iPut = iwork[iRow]++;
            LRindex[iPut] = index;
            LRvalue[iPut] = Lvalue[k];
        }
    }

    // U pointer
    Ulastp.assign(&Ustart[1], &Ustart[numRow + 1]);
    Ustart.resize(numRow);

    // UR space
    int UcountX = Uindex.size();
    int URstuffX = updateMethod == UPDATE_METHOD_FT ? 5 : 0;
    int URcountX = UcountX + URstuffX * numRow;
    URindex.resize(URcountX);
    URvalue.resize(URcountX);

    // UR pointer
    URstart.assign(numRow + 1, 0);
    URlastp.assign(numRow, 0);
    URspace.assign(numRow, URstuffX);
    for (int k = 0; k < UcountX; k++)
        URlastp[UpivotLookup[Uindex[k]]]++;
    for (int i = 1; i <= numRow; i++)
        URstart[i] = URstart[i - 1] + URlastp[i - 1] + URstuffX;
    URstart.resize(numRow);

    // UR element
    URlastp = URstart;
    for (int i = 0; i < numRow; i++) {
        const int index = UpivotIndex[i];
        for (int k = Ustart[i]; k < Ulastp[i]; k++) {
            int iRow = UpivotLookup[Uindex[k]];
            int iPut = URlastp[iRow]++;
            URindex[iPut] = index;
            URvalue[iPut] = Uvalue[k];
        }
    }

    // Re-factor merit
    UmeritX = numRow + (LcountX + UcountX) * 1.5;
    UtotalX = UcountX;
    if (updateMethod == UPDATE_METHOD_PF)
        UmeritX = numRow + UcountX * 4;
    if (updateMethod == UPDATE_METHOD_MPF)
        UmeritX = numRow + UcountX * 3;

    // Clear update buffer
    PFpivotValue.clear();
    PFpivotIndex.clear();
    PFstart.clear();
    PFstart.push_back(0);
    PFindex.clear();
    PFvalue.clear();

    // Finally, permute the base index
    iwork.assign(baseIndex, baseIndex + numRow);
    for (int i = 0; i < numRow; i++)
        baseIndex[permute[i]] = iwork[i];

    pseudoTick += LcountX + UcountX + numRow;
    FtotalX = LcountX + UcountX + numRow;

    fakeTick += numRow * 80 + (LcountX + UcountX) * 60;
}

void HFactor::ftranL(HVector &rhs, double hist_dsty) const {
    const double hyperFTRANL = 0.15;
    const double hyperCANCEL = 0.05;

    if (updateMethod == UPDATE_METHOD_APF) {
        rhs.tight();
        rhs.pack();
        ftranAPF(rhs);
        rhs.tight();
    }

    double curr_dsty = 1.0 * rhs.count / numRow;
    if (curr_dsty > hyperCANCEL || hist_dsty > hyperFTRANL) {
        // Alias to RHS
        int RHScount = 0;
        int *RHSindex = &rhs.index[0];
        double *RHSarray = &rhs.array[0];

        // Alias to factor L
        const int *Lstart = &this->Lstart[0];
        const int *Lindex = &this->Lindex[0];
        const double *Lvalue = &this->Lvalue[0];

        // Transform
        for (int i = 0; i < numRow; i++) {
            int pivotRow = LpivotIndex[i];
            const double pivotX = RHSarray[pivotRow];
            if (fabs(pivotX) > HSOL_CONST_TINY) {
                RHSindex[RHScount++] = pivotRow;
                const int start = Lstart[i];
                const int end = Lstart[i + 1];
                for (int k = start; k < end; k++)
                    RHSarray[Lindex[k]] -= pivotX * Lvalue[k];
            } else
                RHSarray[pivotRow] = 0;
        }

        // Save the count
        rhs.count = RHScount;
    } else {
        solveHyper(numRow, &LpivotLookup[0], &LpivotIndex[0], 0, &Lstart[0], &Lstart[1], &Lindex[0],
                   &Lvalue[0], &rhs);
    }
}

void HFactor::btranL(HVector &rhs, double hist_dsty) const {
    const double hyperBTRANL = 0.10;
    const double hyperCANCEL = 0.05;

    double curr_dsty = 1.0 * rhs.count / numRow;
    if (curr_dsty > hyperCANCEL || hist_dsty > hyperBTRANL) {
        // Alias to RHS
        int RHScount = 0;
        int *RHSindex = &rhs.index[0];
        double *RHSarray = &rhs.array[0];

        // Alias to factor L
        const int *LRstart = &this->LRstart[0];
        const int *LRindex = &this->LRindex[0];
        const double *LRvalue = &this->LRvalue[0];

        // Transform
        for (int i = numRow - 1; i >= 0; i--) {
            int pivotRow = LpivotIndex[i];
            const double pivotX = RHSarray[pivotRow];
            if (fabs(pivotX) > HSOL_CONST_TINY) {
                RHSindex[RHScount++] = pivotRow;
                RHSarray[pivotRow] = pivotX;
                const int start = LRstart[i];
                const int end = LRstart[i + 1];
                for (int k = start; k < end; k++)
                    RHSarray[LRindex[k]] -= pivotX * LRvalue[k];
            } else
                RHSarray[pivotRow] = 0;
        }

        // Save the count
        rhs.count = RHScount;
    } else {
        solveHyper(numRow, &LpivotLookup[0], &LpivotIndex[0], 0, &LRstart[0], &LRstart[1],
                   &LRindex[0], &LRvalue[0], &rhs);
    }

    if (updateMethod == UPDATE_METHOD_APF) {
        btranAPF(rhs);
        rhs.tight();
        rhs.pack();
    }
}

void HFactor::ftranU(HVector &rhs, double hist_dsty) const {
    // The update part
    if (updateMethod == UPDATE_METHOD_FT) {
        ftranFT(rhs);
        rhs.tight();
        rhs.pack();
    }
    if (updateMethod == UPDATE_METHOD_MPF) {
        ftranMPF(rhs);
        rhs.tight();
        rhs.pack();
    }

    // The regular part
    const double hyperFTRANU = 0.10;
    const double hyperCANCEL = 0.05;

    double curr_dsty = 1.0 * rhs.count / numRow;
    if (curr_dsty > hyperCANCEL || hist_dsty > hyperFTRANU) {
        // Alias to non constant
        int RHS_Tick = rhs.pseudoTick;
        int RHScount = 0;
        int *RHSindex = &rhs.index[0];
        double *RHSarray = &rhs.array[0];

        // Alias to the factor
        const int *Ustart = &this->Ustart[0];
        const int *Uend = &this->Ulastp[0];
        const int *Uindex = &this->Uindex[0];
        const double *Uvalue = &this->Uvalue[0];

        // Transform
        double RHS_start = RHS_Tick;
        int UpivotCount = UpivotIndex.size();
        for (int iLogic = UpivotCount - 1; iLogic >= 0; iLogic--) {
            // Skip void
            if (UpivotIndex[iLogic] == -1)
                continue;

            // Normal part
            const int pivotRow = UpivotIndex[iLogic];
            double pivotX = RHSarray[pivotRow];
            if (fabs(pivotX) > HSOL_CONST_TINY) {
                pivotX /= UpivotValue[iLogic];
                RHSindex[RHScount++] = pivotRow;
                RHSarray[pivotRow] = pivotX;
                const int start = Ustart[iLogic];
                const int end = Uend[iLogic];
                if (iLogic >= numRow)
                    RHS_Tick += (end - start);
                for (int k = start; k < end; k++)
                    RHSarray[Uindex[k]] -= pivotX * Uvalue[k];
            } else
                RHSarray[pivotRow] = 0;
        }

        // Save the count
        rhs.count = RHScount;
        rhs.pseudoTick = RHS_Tick + (UpivotCount - numRow);

        rhs.fakeTick += (RHS_Tick - RHS_start) * 15 + (UpivotCount - numRow) * 10;

    } else {
        solveHyper(numRow, &UpivotLookup[0], &UpivotIndex[0], &UpivotValue[0], &Ustart[0],
                   &Ulastp[0], &Uindex[0], &Uvalue[0], &rhs);
    }

    if (updateMethod == UPDATE_METHOD_PF) {
        ftranPF(rhs);
        rhs.tight();
        rhs.pack();
    }
}

void HFactor::btranU(HVector &rhs, double hist_dsty) const {
    if (updateMethod == UPDATE_METHOD_PF) {
        btranPF(rhs);
    }

    // The regular part
    const double hyperBTRANU = 0.15;
    const double hyperCANCEL = 0.05;

    double curr_dsty = 1.0 * rhs.count / numRow;
    if (curr_dsty > hyperCANCEL || hist_dsty > hyperBTRANU) {
        // Alias to non constant
        int RHS_Tick = rhs.pseudoTick;
        int RHScount = 0;
        int *RHSindex = &rhs.index[0];
        double *RHSarray = &rhs.array[0];

        // Alias to the factor
        const int *URstart = &this->URstart[0];
        const int *URend = &this->URlastp[0];
        const int *URindex = &this->URindex[0];
        const double *URvalue = &this->URvalue[0];

        // Transform
        double RHS_start = RHS_Tick;

        int UpivotCount = UpivotIndex.size();
        for (int iLogic = 0; iLogic < UpivotCount; iLogic++) {
            // Skip void
            if (UpivotIndex[iLogic] == -1)
                continue;

            // Normal part
            const int pivotRow = UpivotIndex[iLogic];
            double pivotX = RHSarray[pivotRow];
            if (fabs(pivotX) > HSOL_CONST_TINY) {
                pivotX /= UpivotValue[iLogic];
                RHSindex[RHScount++] = pivotRow;
                RHSarray[pivotRow] = pivotX;
                const int start = URstart[iLogic];
                const int end = URend[iLogic];
                if (iLogic >= numRow)
                    RHS_Tick += end - start;
                for (int k = start; k < end; k++)
                    RHSarray[URindex[k]] -= pivotX * URvalue[k];
            } else
                RHSarray[pivotRow] = 0;
        }

        // Save the count
        rhs.count = RHScount;
        rhs.pseudoTick = RHS_Tick + (UpivotCount - numRow);

        rhs.fakeTick += (RHS_Tick - RHS_start) * 15 + (UpivotCount - numRow) * 10;

    } else {
        solveHyper(numRow, &UpivotLookup[0], &UpivotIndex[0], &UpivotValue[0], &URstart[0],
                   &URlastp[0], &URindex[0], &URvalue[0], &rhs);
    }

    // The update part
    if (updateMethod == UPDATE_METHOD_FT) {
        rhs.tight();
        rhs.pack();
        btranFT(rhs);
        rhs.tight();
    }
    if (updateMethod == UPDATE_METHOD_MPF) {
        rhs.tight();
        rhs.pack();
        btranMPF(rhs);
        rhs.tight();
    }
}

void HFactor::ftranFT(HVector &vector) const {
    // Alias to PF buffer
    const int PFpivotCount = PFpivotIndex.size();
    const int *PFpivotIndex = &this->PFpivotIndex[0];
    const int *PFstart = &this->PFstart[0];
    const int *PFindex = &this->PFindex[0];
    const double *PFvalue = &this->PFvalue[0];

    // Alias to non constant
    int RHS_tick = vector.pseudoTick;
    int RHScount = vector.count;
    int *RHSindex = &vector.index[0];
    double *RHSarray = &vector.array[0];

    // Forwardly apply row ETA
    for (int i = 0; i < PFpivotCount; i++) {
        int iRow = PFpivotIndex[i];
        double value0 = RHSarray[iRow];
        double value1 = value0;
        const int start = PFstart[i];
        const int end = PFstart[i + 1];
        RHS_tick += end - start;
        for (int k = start; k < end; k++)
            value1 -= RHSarray[PFindex[k]] * PFvalue[k];
        // This would skip the situation where they are both zeros
        if (value0 || value1) {
            if (value0 == 0)
                RHSindex[RHScount++] = iRow;
            RHSarray[iRow] = (fabs(value1) < HSOL_CONST_TINY) ? HSOL_CONST_ZERO : value1;
        }
    }

    // Save count back
    vector.count = RHScount;
    vector.pseudoTick = RHS_tick + PFpivotCount * 0.5;

    vector.fakeTick += PFpivotCount * 20 + PFstart[PFpivotCount] * 5;
    if (PFstart[PFpivotCount] / (PFpivotCount + 1) < 5)
        vector.fakeTick += PFstart[PFpivotCount] * 5;
}

void HFactor::btranFT(HVector &vector) const {
    // Alias to PF buffer
    const int PFpivotCount = PFpivotIndex.size();
    const int *PFpivotIndex = &this->PFpivotIndex[0];
    const int *PFstart = &this->PFstart[0];
    const int *PFindex = &this->PFindex[0];
    const double *PFvalue = &this->PFvalue[0];

    // Alias to non constant
    int RHS_tick = vector.pseudoTick;
    int RHScount = vector.count;
    int *RHSindex = &vector.index[0];
    double *RHSarray = &vector.array[0];

    // Backwardly apply row ETA
    double RHS_start = RHS_tick;
    for (int i = PFpivotCount - 1; i >= 0; i--) {
        int pivotRow = PFpivotIndex[i];
        double pivotX = RHSarray[pivotRow];
        if (pivotX) {
            const int start = PFstart[i];
            const int end = PFstart[i + 1];
            RHS_tick += end - start;
            for (int k = start; k < end; k++) {
                int iRow = PFindex[k];
                double value0 = RHSarray[iRow];
                double value1 = value0 - pivotX * PFvalue[k];
                if (value0 == 0)
                    RHSindex[RHScount++] = iRow;
                RHSarray[iRow] = (fabs(value1) < HSOL_CONST_TINY) ? HSOL_CONST_ZERO : value1;
            }
        }
    }

    vector.fakeTick += (RHS_tick - RHS_start) * 15 + PFpivotCount * 10;

    // Save count back
    vector.pseudoTick = RHS_tick + PFpivotCount * 0.3;
    vector.count = RHScount;
}

void HFactor::ftranPF(HVector &vector) const {
    // Alias to PF buffer
    const int PFpivotCount = PFpivotIndex.size();
    const int *PFpivotIndex = &this->PFpivotIndex[0];
    const double *PFpivotValue = &this->PFpivotValue[0];
    const int *PFstart = &this->PFstart[0];
    const int *PFindex = &this->PFindex[0];
    const double *PFvalue = &this->PFvalue[0];

    // Alias to non constant
    int RHScount = vector.count;
    int *RHSindex = &vector.index[0];
    double *RHSarray = &vector.array[0];

    // Forwardly
    for (int i = 0; i < PFpivotCount; i++) {
        int pivotRow = PFpivotIndex[i];
        double pivotX = RHSarray[pivotRow];
        if (fabs(pivotX) > HSOL_CONST_TINY) {
            pivotX /= PFpivotValue[i];
            RHSarray[pivotRow] = pivotX;
            for (int k = PFstart[i]; k < PFstart[i + 1]; k++) {
                const int index = PFindex[k];
                const double value0 = RHSarray[index];
                const double value1 = value0 - pivotX * PFvalue[k];
                if (value0 == 0)
                    RHSindex[RHScount++] = index;
                RHSarray[index] = (fabs(value1) < HSOL_CONST_TINY) ? HSOL_CONST_ZERO : value1;
            }
        }
    }

    // Save count
    vector.count = RHScount;
}

void HFactor::btranPF(HVector &vector) const {
    // Alias to PF buffer
    const int PFpivotCount = PFpivotIndex.size();
    const int *PFpivotIndex = &this->PFpivotIndex[0];
    const double *PFpivotValue = &this->PFpivotValue[0];
    const int *PFstart = &this->PFstart[0];
    const int *PFindex = &this->PFindex[0];
    const double *PFvalue = &this->PFvalue[0];

    // Alias to non constant
    int RHScount = vector.count;
    int *RHSindex = &vector.index[0];
    double *RHSarray = &vector.array[0];

    // Backwardly
    for (int i = PFpivotCount - 1; i >= 0; i--) {
        int pivotRow = PFpivotIndex[i];
        double pivotX = RHSarray[pivotRow];
        for (int k = PFstart[i]; k < PFstart[i + 1]; k++)
            pivotX -= PFvalue[k] * RHSarray[PFindex[k]];
        pivotX /= PFpivotValue[i];

        if (RHSarray[pivotRow] == 0)
            RHSindex[RHScount++] = pivotRow;
        RHSarray[pivotRow] = (fabs(pivotX) < HSOL_CONST_TINY) ? 1e-100 : pivotX;
    }

    // Save count
    vector.count = RHScount;
}

void HFactor::ftranMPF(HVector &vector) const {
    // Alias to non constant
    int RHScount = vector.count;
    int *RHSindex = &vector.index[0];
    double *RHSarray = &vector.array[0];

    // Forwardly
    int PFpivotCount = PFpivotValue.size();
    for (int i = 0; i < PFpivotCount; i++) {
        solveMatrixT(PFstart[i * 2 + 1], PFstart[i * 2 + 2], PFstart[i * 2], PFstart[i * 2 + 1],
                     &PFindex[0], &PFvalue[0], PFpivotValue[i], &RHScount, RHSindex, RHSarray);
    }

    // Remove cancellation
    vector.count = RHScount;
}

void HFactor::btranMPF(HVector &vector) const {
    // Alias to non constant
    int RHScount = vector.count;
    int *RHSindex = &vector.index[0];
    double *RHSarray = &vector.array[0];

    // Backwardly
    for (int i = PFpivotValue.size() - 1; i >= 0; i--) {
        solveMatrixT(PFstart[i * 2], PFstart[i * 2 + 1], PFstart[i * 2 + 1], PFstart[i * 2 + 2],
                     &PFindex[0], &PFvalue[0], PFpivotValue[i], &RHScount, RHSindex, RHSarray);
    }

    // Remove cancellation
    vector.count = RHScount;
}

void HFactor::ftranAPF(HVector &vector) const {
    // Alias to non constant
    int RHScount = vector.count;
    int *RHSindex = &vector.index[0];
    double *RHSarray = &vector.array[0];

    // Backwardly
    int PFpivotCount = PFpivotValue.size();
    for (int i = PFpivotCount - 1; i >= 0; i--) {
        solveMatrixT(PFstart[i * 2 + 1], PFstart[i * 2 + 2], PFstart[i * 2], PFstart[i * 2 + 1],
                     &PFindex[0], &PFvalue[0], PFpivotValue[i], &RHScount, RHSindex, RHSarray);
    }

    // Remove cancellation
    vector.count = RHScount;
}

void HFactor::btranAPF(HVector &vector) const {
    // Alias to non constant
    int RHScount = vector.count;
    int *RHSindex = &vector.index[0];
    double *RHSarray = &vector.array[0];

    // Forwardly
    int PFpivotCount = PFpivotValue.size();
    for (int i = 0; i < PFpivotCount; i++) {
        solveMatrixT(PFstart[i * 2], PFstart[i * 2 + 1], PFstart[i * 2 + 1], PFstart[i * 2 + 2],
                     &PFindex[0], &PFvalue[0], PFpivotValue[i], &RHScount, RHSindex, RHSarray);
    }
    vector.count = RHScount;
}

void HFactor::updateCFT(HVector *aq, HVector *ep, int *iRow, int *hint) {

    /*
     * In the major update loop, the prefix
     *
     * c(p) = current working pivot
     * p(p) = previous pivot  (0 =< pp < cp)
     */

    int numUpdate = 0;
    for (HVector *vec = aq; vec != 0; vec = vec->next)
        numUpdate++;

    vector<HVector *> aqWork(numUpdate);
    vector<HVector *> epWork(numUpdate);

    for (int i = 0; i < numUpdate; i++) {
        aqWork[i] = aq;
        epWork[i] = ep;
        aq = aq->next;
        ep = ep->next;
    }

    // Pivot related buffers
    int PFnp0 = PFpivotIndex.size();
    vector<int> pLogic(numUpdate);
    vector<double> pValue(numUpdate);
    vector<double> pAlpha(numUpdate);
    for (int cp = 0; cp < numUpdate; cp++) {
        int cRow = iRow[cp];
        int iLogic = UpivotLookup[cRow];
        pLogic[cp] = iLogic;
        pValue[cp] = UpivotValue[iLogic];
        pAlpha[cp] = aqWork[cp]->array[cRow];
    }

    // Temporary U pointers
    vector<int> Tstart(numUpdate + 1);
    vector<double> Tpivot(numUpdate);
    Tstart[0] = Uindex.size();

    // Logically sorted previous row_ep
    vector<pair<int, int>> sorted_pp;

    // Major update loop
    for (int cp = 0; cp < numUpdate; cp++) {
        // 1. Expand partial FTRAN result to buffer
        iwork.clear();
        for (int i = 0; i < aqWork[cp]->packCount; i++) {
            int index = aqWork[cp]->packIndex[i];
            double value = aqWork[cp]->packValue[i];
            iwork.push_back(index);
            dwork[index] = value;
        }

        // 2. Update partial FTRAN result by recent FT matrix
        for (int pp = 0; pp < cp; pp++) {
            int pRow = iRow[pp];
            double value = dwork[pRow];
            int PFpp = pp + PFnp0;
            for (int i = PFstart[PFpp]; i < PFstart[PFpp + 1]; i++)
                value -= dwork[PFindex[i]] * PFvalue[i];
            iwork.push_back(pRow); // OK to duplicate
            dwork[pRow] = value;
        }

        // 3. Store the partial FTRAN result to matirx U
        double ppaq = dwork[iRow[cp]]; // pivot of the partial aq
        dwork[iRow[cp]] = 0;
        int UcountX = Tstart[cp];
        int UstartX = UcountX;
        for (unsigned i = 0; i < iwork.size(); i++) {
            int index = iwork[i];
            double value = dwork[index];
            dwork[index] = 0; // This effectively removes all duplication
            if (fabs(value) > HSOL_CONST_TINY) {
                Uindex.push_back(index);
                Uvalue.push_back(value);
            }
        }
        UcountX = Uindex.size();
        Tstart[cp + 1] = UcountX;
        Tpivot[cp] = pValue[cp] * pAlpha[cp];

        // 4. Expand partial BTRAN result to buffer
        iwork.clear();
        for (int i = 0; i < epWork[cp]->packCount; i++) {
            int index = epWork[cp]->packIndex[i];
            double value = epWork[cp]->packValue[i];
            iwork.push_back(index);
            dwork[index] = value;
        }

        // 5. Delete logical later rows (in logical order)
        for (int isort = 0; isort < cp; isort++) {
            int pp = sorted_pp[isort].second;
            int pRow = iRow[pp];
            double multiplier = -pValue[pp] * dwork[pRow];
            if (fabs(dwork[pRow]) > HSOL_CONST_TINY) {
                for (int i = 0; i < epWork[pp]->packCount; i++) {
                    int index = epWork[pp]->packIndex[i];
                    double value = epWork[pp]->packValue[i];
                    iwork.push_back(index);
                    dwork[index] += value * multiplier;
                }
            }
            dwork[pRow] = 0; // Force to be 0
        }

        // 6. Update partial BTRAN result by recent U columns
        for (int pp = 0; pp < cp; pp++) {
            int kpivot = iRow[pp];
            double value = dwork[kpivot];
            for (int k = Tstart[pp]; k < Tstart[pp + 1]; k++)
                value -= dwork[Uindex[k]] * Uvalue[k];
            value /= Tpivot[pp];
            iwork.push_back(kpivot);
            dwork[kpivot] = value; // Again OK to duplicate
        }

        // 6.x compute current alpha
        double thex = 0;
        for (int k = UstartX; k < UcountX; k++) {
            int index = Uindex[k];
            double value = Uvalue[k];
            thex += dwork[index] * value;
        }
        Tpivot[cp] = ppaq + thex * pValue[cp];

        // 7. Store BTRAN result to FT elimination, update logic helper
        dwork[iRow[cp]] = 0;
        double pivotX = -pValue[cp];
        for (unsigned i = 0; i < iwork.size(); i++) {
            int index = iwork[i];
            double value = dwork[index];
            dwork[index] = 0;
            if (fabs(value) > HSOL_CONST_TINY) {
                PFindex.push_back(index);
                PFvalue.push_back(value * pivotX);
            }
        }
        PFpivotIndex.push_back(iRow[cp]);
        FtotalX += PFindex.size() - PFstart.back() + 1;
        UtotalX += PFindex.size() - PFstart.back();
        PFstart.push_back(PFindex.size());

        // 8. Update the sorted ep
        sorted_pp.push_back(make_pair(pLogic[cp], cp));
        sort(sorted_pp.begin(), sorted_pp.end());
    }

    // Now modify the U matrix
    for (int cp = 0; cp < numUpdate; cp++) {
        // 1. Delete pivotal row from U
        int cIndex = iRow[cp];
        int cLogic = pLogic[cp];
        FtotalX -= URlastp[cLogic] - URstart[cLogic];
        UtotalX -= URlastp[cLogic] - URstart[cLogic];
        for (int k = URstart[cLogic]; k < URlastp[cLogic]; k++) {
            // Find the pivotal position
            int iLogic = UpivotLookup[URindex[k]];
            int iFind = Ustart[iLogic];
            int iLast = --Ulastp[iLogic];
            for (; iFind <= iLast; iFind++)
                if (Uindex[iFind] == cIndex)
                    break;
            // Put last to find, and delete last
            Uindex[iFind] = Uindex[iLast];
            Uvalue[iFind] = Uvalue[iLast];
        }

        // 2. Delete pivotal column from UR
        FtotalX -= Ulastp[cLogic] - Ustart[cLogic];
        UtotalX -= Ulastp[cLogic] - Ustart[cLogic];
        for (int k = Ustart[cLogic]; k < Ulastp[cLogic]; k++) {
            // Find the pivotal position
            int iLogic = UpivotLookup[Uindex[k]];
            int iFind = URstart[iLogic];
            int iLast = --URlastp[iLogic];
            for (; iFind <= iLast; iFind++)
                if (URindex[iFind] == cIndex)
                    break;
            // Put last to find, and delete last
            URspace[iLogic]++;
            URindex[iFind] = URindex[iLast];
            URvalue[iFind] = URvalue[iLast];
        }

        // 3. Insert the (stored) partial FTRAN to the row matrix
        int UstartX = Tstart[cp];
        int UendX = Tstart[cp + 1];
        FtotalX += UendX - UstartX + 1;
        UtotalX += UendX - UstartX;
        // Store column as UR elements
        for (int k = UstartX; k < UendX; k++) {
            // Which ETA file
            int iLogic = UpivotLookup[Uindex[k]];

            // Move row to the end if necessary
            if (URspace[iLogic] == 0) {
                // Make pointers
                int row_start = URstart[iLogic];
                int row_count = URlastp[iLogic] - row_start;
                int new_start = URindex.size();
                int new_space = row_count * 1.1 + 5;

                // Check matrix UR
                URindex.resize(new_start + new_space);
                URvalue.resize(new_start + new_space);

                // Move elements
                int iFrom = row_start;
                int iEnd = row_start + row_count;
                int iTo = new_start;
                copy(&URindex[iFrom], &URindex[iEnd], &URindex[iTo]);
                copy(&URvalue[iFrom], &URvalue[iEnd], &URvalue[iTo]);

                // Save new pointers
                URstart[iLogic] = new_start;
                URlastp[iLogic] = new_start + row_count;
                URspace[iLogic] = new_space - row_count;
            }

            // Put into the next available space
            URspace[iLogic]--;
            int iPut = URlastp[iLogic]++;
            URindex[iPut] = cIndex;
            URvalue[iPut] = Uvalue[k];
        }

        // 4. Save pointers
        Ustart.push_back(UstartX);
        Ulastp.push_back(UendX);

        URstart.push_back(URstart[cLogic]);
        URlastp.push_back(URstart[cLogic]);
        URspace.push_back(URspace[cLogic] + URlastp[cLogic] - URstart[cLogic]);

        UpivotLookup[cIndex] = UpivotIndex.size();
        UpivotIndex[cLogic] = -1;
        UpivotIndex.push_back(cIndex);
        UpivotValue.push_back(Tpivot[cp]);
    }

    //    // See if we want refactor
    //    if (UtotalX > UmeritX && PFpivotIndex.size() > 100)
    //        *hint = 1;
}

void HFactor::updateFT(HVector *aq, HVector *ep, int iRow, int *hint) {
    // Store pivot
    int pLogic = UpivotLookup[iRow];
    double pivot = UpivotValue[pLogic];
    double alpha = aq->array[iRow];
    UpivotIndex[pLogic] = -1;

    // Delete pivotal row from U
    FtotalX -= URlastp[pLogic] - URstart[pLogic];
    for (int k = URstart[pLogic]; k < URlastp[pLogic]; k++) {
        // Find the pivotal position
        int iLogic = UpivotLookup[URindex[k]];
        int iFind = Ustart[iLogic];
        int iLast = --Ulastp[iLogic];
        for (; iFind <= iLast; iFind++)
            if (Uindex[iFind] == iRow)
                break;
        // Put last to find, and delete last
        Uindex[iFind] = Uindex[iLast];
        Uvalue[iFind] = Uvalue[iLast];
    }

    // Delete pivotal column from UR
    FtotalX -= Ulastp[pLogic] - Ustart[pLogic];
    for (int k = Ustart[pLogic]; k < Ulastp[pLogic]; k++) {
        // Find the pivotal position
        int iLogic = UpivotLookup[Uindex[k]];
        int iFind = URstart[iLogic];
        int iLast = --URlastp[iLogic];
        for (; iFind <= iLast; iFind++)
            if (URindex[iFind] == iRow)
                break;
        // Put last to find, and delete last
        URspace[iLogic]++;
        URindex[iFind] = URindex[iLast];
        URvalue[iFind] = URvalue[iLast];
    }

    // Store column to U
    Ustart.push_back(Uindex.size());
    for (int i = 0; i < aq->packCount; i++)
        if (aq->packIndex[i] != iRow) {
            Uindex.push_back(aq->packIndex[i]);
            Uvalue.push_back(aq->packValue[i]);
        }
    Ulastp.push_back(Uindex.size());
    int UstartX = Ustart.back();
    int UendX = Ulastp.back();
    UtotalX += UendX - UstartX + 1;
    FtotalX += UendX - UstartX + 1;

    // Store column as UR elements
    for (int k = UstartX; k < UendX; k++) {
        // Which ETA file
        int iLogic = UpivotLookup[Uindex[k]];

        // Move row to the end if necessary
        if (URspace[iLogic] == 0) {
            // Make pointers
            int row_start = URstart[iLogic];
            int row_count = URlastp[iLogic] - row_start;
            int new_start = URindex.size();
            int new_space = row_count * 1.1 + 5;

            // Check matrix UR
            URindex.resize(new_start + new_space);
            URvalue.resize(new_start + new_space);

            // Move elements
            int iFrom = row_start;
            int iEnd = row_start + row_count;
            int iTo = new_start;
            copy(&URindex[iFrom], &URindex[iEnd], &URindex[iTo]);
            copy(&URvalue[iFrom], &URvalue[iEnd], &URvalue[iTo]);

            // Save new pointers
            URstart[iLogic] = new_start;
            URlastp[iLogic] = new_start + row_count;
            URspace[iLogic] = new_space - row_count;
        }

        // Put into the next available space
        URspace[iLogic]--;
        int iPut = URlastp[iLogic]++;
        URindex[iPut] = iRow;
        URvalue[iPut] = Uvalue[k];
    }

    // Store UR pointers
    URstart.push_back(URstart[pLogic]);
    URlastp.push_back(URstart[pLogic]);
    URspace.push_back(URspace[pLogic] + URlastp[pLogic] - URstart[pLogic]);

    // Update pivot count
    UpivotLookup[iRow] = UpivotIndex.size();
    UpivotIndex.push_back(iRow);
    UpivotValue.push_back(pivot * alpha);

    // Store row_ep as R matrix
    for (int i = 0; i < ep->packCount; i++) {
        if (ep->packIndex[i] != iRow) {
            PFindex.push_back(ep->packIndex[i]);
            PFvalue.push_back(-ep->packValue[i] * pivot);
        }
    }
    UtotalX += PFindex.size() - PFstart.back();
    FtotalX += PFindex.size() - PFstart.back() + 1;

    // Store R matrix pivot
    PFpivotIndex.push_back(iRow);
    PFstart.push_back(PFindex.size());

    // Update total countX
    UtotalX -= Ulastp[pLogic] - Ustart[pLogic];
    UtotalX -= URlastp[pLogic] - URstart[pLogic];

    //    // See if we want refactor
    //    if (UtotalX > UmeritX && PFpivotIndex.size() > 100)
    //        *hint = 1;
}

void HFactor::updatePF(HVector *aq, HVector *ep, int iRow, int *hint) {
    // Check space
    const int columnCount = aq->packCount;
    const int *columnIndex = &aq->packIndex[0];
    const double *columnArray = &aq->packValue[0];

    // Copy the pivotal column
    for (int i = 0; i < columnCount; i++) {
        int index = columnIndex[i];
        double value = columnArray[i];
        if (index != iRow) {
            PFindex.push_back(index);
            PFvalue.push_back(value);
        }
    }

    // Save pivot
    PFpivotIndex.push_back(iRow);
    PFpivotValue.push_back(aq->array[iRow]);
    PFstart.push_back(PFindex.size());

    // Check refactor
    UtotalX += aq->packCount;
    if (UtotalX > UmeritX)
        *hint = 1;
    FtotalX += aq->packCount;
}

void HFactor::updateMPF(HVector *aq, HVector *ep, int iRow, int *hint) {
    int PFcountX0 = PFindex.size();

    // Store elements
    for (int i = 0; i < aq->packCount; i++) {
        PFindex.push_back(aq->packIndex[i]);
        PFvalue.push_back(aq->packValue[i]);
    }
    int pLogic = UpivotLookup[iRow];
    int UstartX = Ustart[pLogic];
    int UendX = Ustart[pLogic + 1];
    for (int k = UstartX; k < UendX; k++) {
        PFindex.push_back(Uindex[k]);
        PFvalue.push_back(-Uvalue[k]);
    }
    PFindex.push_back(iRow);
    PFvalue.push_back(-UpivotValue[pLogic]);
    PFstart.push_back(PFindex.size());

    for (int i = 0; i < ep->packCount; i++) {
        PFindex.push_back(ep->packIndex[i]);
        PFvalue.push_back(ep->packValue[i]);
    }
    PFstart.push_back(PFindex.size());

    // Store pivot
    PFpivotValue.push_back(aq->array[iRow]);

    // Refactor or not
    UtotalX += aq->packCount + ep->packCount;
    FtotalX += PFindex.size() - PFcountX0;
    if (UtotalX > UmeritX)
        *hint = 1;
}

void HFactor::updateAPF(HVector *aq, HVector *ep, int iRow, int *hint) {
    int PFcountX0 = PFindex.size();

    // Store elements
    for (int i = 0; i < aq->packCount; i++) {
        PFindex.push_back(aq->packIndex[i]);
        PFvalue.push_back(aq->packValue[i]);
    }

    int columnOut = baseIndex[iRow];
    if (columnOut >= numCol) {
        PFindex.push_back(columnOut - numCol);
        PFvalue.push_back(-1);
    } else {
        for (int k = Astart[columnOut]; k < Astart[columnOut + 1]; k++) {
            PFindex.push_back(Aindex[k]);
            PFvalue.push_back(-Avalue[k]);
        }
    }
    PFstart.push_back(PFindex.size());

    for (int i = 0; i < ep->packCount; i++) {
        PFindex.push_back(ep->packIndex[i]);
        PFvalue.push_back(ep->packValue[i]);
    }
    PFstart.push_back(PFindex.size());
    FtotalX += PFindex.size() - PFcountX0;

    // Store pivot
    PFpivotValue.push_back(aq->array[iRow]);
}

// Upstream file: HMatrix.cpp

void HMatrix::setup(int numCol_, int numRow_, const int *Astart_, const int *Aindex_,
                    const double *Avalue_) {
    // Copy A
    numCol = numCol_;
    numRow = numRow_;
    Astart.assign(Astart_, Astart_ + numCol_ + 1);

    int AcountX = Astart_[numCol_];
    Aindex.assign(Aindex_, Aindex_ + AcountX);
    Avalue.assign(Avalue_, Avalue_ + AcountX);

    // Build row copy - pointers
    ARstart.resize(numRow + 1);
    AR_Nend.assign(numRow, 0);
    for (int k = 0; k < AcountX; k++)
        AR_Nend[Aindex[k]]++;
    ARstart[0] = 0;
    for (int i = 1; i <= numRow; i++)
        ARstart[i] = ARstart[i - 1] + AR_Nend[i - 1];
    for (int i = 0; i < numRow; i++)
        AR_Nend[i] = ARstart[i];

    // Build row copy - elements
    ARindex.resize(AcountX);
    ARvalue.resize(AcountX);
    for (int iCol = 0; iCol < numCol; iCol++) {
        for (int k = Astart[iCol]; k < Astart[iCol + 1]; k++) {
            int iRow = Aindex[k];
            int iPut = AR_Nend[iRow]++;
            ARindex[iPut] = iCol;
            ARvalue[iPut] = Avalue[k];
        }
    }
}

void HMatrix::update(int columnIn, int columnOut) {
    if (columnIn < numCol) {
        for (int k = Astart[columnIn]; k < Astart[columnIn + 1]; k++) {
            int iRow = Aindex[k];
            int iFind = ARstart[iRow];
            int iSwap = --AR_Nend[iRow];
            while (ARindex[iFind] != columnIn)
                iFind++;
            swap(ARindex[iFind], ARindex[iSwap]);
            swap(ARvalue[iFind], ARvalue[iSwap]);
        }
    }

    if (columnOut < numCol) {
        for (int k = Astart[columnOut]; k < Astart[columnOut + 1]; k++) {
            int iRow = Aindex[k];
            int iFind = AR_Nend[iRow];
            int iSwap = AR_Nend[iRow]++;
            while (ARindex[iFind] != columnOut)
                iFind++;
            swap(ARindex[iFind], ARindex[iSwap]);
            swap(ARvalue[iFind], ARvalue[iSwap]);
        }
    }
}

double HMatrix::compute_dot(HVector &vector, int iCol) const {
    double result = 0;
    if (iCol < numCol) {
        for (int k = Astart[iCol]; k < Astart[iCol + 1]; k++)
            result += vector.array[Aindex[k]] * Avalue[k];
    } else {
        result = vector.array[iCol - numCol];
    }
    return result;
}

void HMatrix::collect_aj(HVector &vector, int iCol, double multi) const {
    if (iCol < numCol) {
        for (int k = Astart[iCol]; k < Astart[iCol + 1]; k++) {
            int index = Aindex[k];
            double value0 = vector.array[index];
            double value1 = value0 + multi * Avalue[k];
            if (value0 == 0)
                vector.index[vector.count++] = index;
            vector.array[index] = (fabs(value1) < HSOL_CONST_TINY) ? HSOL_CONST_ZERO : value1;
        }
    } else {
        int index = iCol - numCol;
        double value0 = vector.array[index];
        double value1 = value0 + multi;
        if (value0 == 0)
            vector.index[vector.count++] = index;
        vector.array[index] = (fabs(value1) < HSOL_CONST_TINY) ? HSOL_CONST_ZERO : value1;
    }
}

void HMatrix::price_by_col(HVector &row_ap, HVector &row_ep) const {
    // Alias
    int ap_count = 0;
    int *ap_index = &row_ap.index[0];
    double *ap_array = &row_ap.array[0];
    const double *ep_array = &row_ep.array[0];

    // Computation
    for (int iCol = 0; iCol < numCol; iCol++) {
        double value = 0;
        for (int k = Astart[iCol]; k < Astart[iCol + 1]; k++) {
            value += ep_array[Aindex[k]] * Avalue[k];
        }
        if (fabs(value) > HSOL_CONST_TINY) {
            ap_array[iCol] = value;
            ap_index[ap_count++] = iCol;
        }
    }
    row_ap.count = ap_count;
}

void HMatrix::price_by_row(HVector &row_ap, HVector &row_ep) const {
    // Alias
    int ap_count = 0;
    int *ap_index = &row_ap.index[0];
    double *ap_array = &row_ap.array[0];
    const int ep_count = row_ep.count;
    const int *ep_index = &row_ep.index[0];
    const double *ep_array = &row_ep.array[0];

    // Computation
    for (int i = 0; i < ep_count; i++) {
        int iRow = ep_index[i];
        double multi = ep_array[iRow];
        for (int k = ARstart[iRow]; k < AR_Nend[iRow]; k++) {
            int index = ARindex[k];
            double value0 = ap_array[index];
            double value1 = value0 + multi * ARvalue[k];
            if (value0 == 0)
                ap_index[ap_count++] = index;
            ap_array[index] = (fabs(value1) < HSOL_CONST_TINY) ? HSOL_CONST_ZERO : value1;
        }
    }

    // Try to remove cancellation
    const int apcount1 = ap_count;
    ap_count = 0;
    for (int i = 0; i < apcount1; i++) {
        const int index = ap_index[i];
        const double value = ap_array[index];
        if (fabs(value) > HSOL_CONST_TINY) {
            ap_index[ap_count++] = index;
        } else {
            ap_array[index] = 0;
        }
    }
    row_ap.count = ap_count;
}

// Upstream file: HModel.cpp

using namespace std;

HModel::HModel() {
    intOption[INTOPT_PRINT_FLAG] = 0;     // no print
    intOption[INTOPT_TRANSPOSE_FLAG] = 0; // no transpose
    intOption[INTOPT_SCALE_FLAG] = 1;     // do scale
    intOption[INTOPT_TIGHT_FLAG] = 1;     // do tight
    intOption[INTOPT_PERMUTE_FLAG] = 0;   // no permute
    intOption[INTOPT_PERTURB_FLAG] = 1;   // do perturb

    dblOption[DBLOPT_PRIMAL_TOL] = 1e-7;
    dblOption[DBLOPT_DUAL_TOL] = 1e-7;
    dblOption[DBLOPT_PAMI_CUTOFF] = 0.95;

    strOption[STROPT_PARTITION_FILE] = "";
}

void HModel::setup_allocWorking() {
    // Setup starting base
    basicIndex.resize(numRow);
    for (int iRow = 0; iRow < numRow; iRow++)
        basicIndex[iRow] = iRow + numCol;
    nonbasicFlag.assign(numTot, 0);
    nonbasicMove.resize(numTot);
    for (int i = 0; i < numCol; i++)
        nonbasicFlag[i] = 1;

    // Matrix, factor
    matrix.setup(numCol, numRow, &Astart[0], &Aindex[0], &Avalue[0]);
    factor.setup(numCol, numRow, &Astart[0], &Aindex[0], &Avalue[0], &basicIndex[0]);
    limitUpdate = 5000;

    // Setup other buffer
    buffer.setup(numRow);
    bufferLong.setup(numCol);

    // Setup bounds and solution spaces
    workCost.resize(numTot);
    workDual.resize(numTot);
    workShift.assign(numTot, 0);

    workLower.resize(numTot);
    workUpper.resize(numTot);
    workRange.resize(numTot);
    workValue.resize(numTot);

    baseLower.resize(numRow);
    baseUpper.resize(numRow);
    baseValue.resize(numRow);
}

void HModel::initCost(int perturb) {
    // Copy the cost
    for (int i = 0; i < numCol; i++)
        workCost[i] = colCost[i];
    for (int i = numCol; i < numTot; i++)
        workCost[i] = 0;
    workShift.assign(numTot, 0);

    // See if we want to skip perturbation
    problemPerturbed = 0;
    if (perturb == 0 || intOption[INTOPT_PERTURB_FLAG] == 0)
        return;
    problemPerturbed = 1;

    // Perturb the original costs, scale down if is too big
    double bigc = 0;
    for (int i = 0; i < numCol; i++)
        bigc = max(bigc, fabs(workCost[i]));
    if (bigc > 100)
        bigc = sqrt(sqrt(bigc));

    // If there's few boxed variables, we will just use Simple perturbation
    double boxedRate = 0;
    for (int i = 0; i < numTot; i++)
        boxedRate += (workRange[i] < 1e30);
    boxedRate /= numTot;
    if (boxedRate < 0.01)
        bigc = min(bigc, 1.0);
    if (bigc < 1) {
        //        bigc = sqrt(bigc);
    }

    // Determine the perturbation base
    double base = 5e-7 * bigc;

    // Now do the perturbation
    for (int i = 0; i < numCol; i++) {
        double lower = colLower[i];
        double upper = colUpper[i];
        double xpert = (fabs(workCost[i]) + 1) * base * (1 + dblXpert[i]);
        if (lower == -HSOL_CONST_INF && upper == HSOL_CONST_INF) {
            // Free - no perturb
        } else if (upper == HSOL_CONST_INF) { // Lower
            workCost[i] += xpert;
        } else if (lower == -HSOL_CONST_INF) { // Upper
            workCost[i] += -xpert;
        } else if (lower != upper) { // Boxed
            workCost[i] += (workCost[i] >= 0) ? xpert : -xpert;
        } else {
            // Fixed - no perturb
        }
    }

    for (int i = numCol; i < numTot; i++) {
        workCost[i] += (0.5 - dblXpert[i]) * 1e-12;
    }
}

void HModel::initBound(int phase) {
    // Copy bounds
    for (int i = 0; i < numCol; i++) {
        workLower[i] = colLower[i];
        workUpper[i] = colUpper[i];
    }
    for (int i = 0, j = numCol; i < numRow; i++, j++) {
        workLower[j] = -rowUpper[i];
        workUpper[j] = -rowLower[i];
    }

    // Change to dual phase 1 bound
    if (phase == 1) {
        const double inf = HSOL_CONST_INF;
        for (int i = 0; i < numTot; i++) {
            if (workLower[i] == -inf && workUpper[i] == inf) {
                // Won't change for row variables: they should never
                // Become non basic
                if (i >= numCol)
                    continue;
                workLower[i] = -1000, workUpper[i] = 1000; // FREE
            } else if (workLower[i] == -inf) {
                workLower[i] = -1, workUpper[i] = 0; // UPPER
            } else if (workUpper[i] == inf) {
                workLower[i] = 0, workUpper[i] = 1; // LOWER
            } else {
                workLower[i] = 0, workUpper[i] = 0; // BOXED or FIXED
            }
        }
    }

    // Work out ranges
    for (int i = 0; i < numTot; i++)
        workRange[i] = workUpper[i] - workLower[i];
}

void HModel::initValue() {
    for (int i = 0; i < numTot; i++) {
        if (nonbasicFlag[i]) {
            if (workLower[i] == workUpper[i]) {
                // Fixed
                workValue[i] = workLower[i];
                nonbasicMove[i] = 0;
            } else if (workLower[i] != -HSOL_CONST_INF) {
                // Lower
                workValue[i] = workLower[i];
                nonbasicMove[i] = 1;
            } else if (workUpper[i] != +HSOL_CONST_INF) {
                // UPPER
                workValue[i] = workUpper[i];
                nonbasicMove[i] = -1;
            } else {
                // FREE
                workValue[i] = 0;
                nonbasicMove[i] = 0;
            }
        } else {
            nonbasicMove[i] = 0;
        }
    }
}

void HModel::computeFactor() {
    try {
        factor.build();
    } catch (runtime_error &error) {
        cerr << error.what() << endl;
        problemStatus = 3;
        throw runtime_error("HSOL basis factorization failed");
    }
    countUpdate = 0;
}

void HModel::computeDual() {
    buffer.clear();
    for (int iRow = 0; iRow < numRow; iRow++) {
        buffer.index[iRow] = iRow;
        buffer.array[iRow] = workCost[basicIndex[iRow]] + workShift[basicIndex[iRow]];
    }
    buffer.count = numRow;
    factor.btran(buffer, 1);

    bufferLong.clear();
    matrix.price_by_col(bufferLong, buffer);
    for (int i = 0; i < numCol; i++)
        workDual[i] = workCost[i] - bufferLong.array[i];
    for (int i = numCol; i < numTot; i++)
        workDual[i] = workCost[i] - buffer.array[i - numCol];
}

void HModel::computeDualInfeasInDual(int *dualInfeasCount) {
    int workCount = 0;
    const double inf = HSOL_CONST_INF;
    const double tau_d = dblOption[DBLOPT_DUAL_TOL];
    for (int i = 0; i < numTot; i++) {
        // Only for non basic variables
        if (!nonbasicFlag[i])
            continue;
        // Free
        if (workLower[i] == -inf && workUpper[i] == inf)
            workCount += (fabs(workDual[i]) >= tau_d);
        // In dual, assuming that boxed variables will be flipped
        if (workLower[i] == -inf || workUpper[i] == inf)
            workCount += (nonbasicMove[i] * workDual[i] <= -tau_d);
    }
    *dualInfeasCount = workCount;
}

void HModel::computeDualInfeasInPrimal(int *dualInfeasCount) {
    int workCount = 0;
    const double inf = HSOL_CONST_INF;
    const double tau_d = dblOption[DBLOPT_DUAL_TOL];
    for (int i = 0; i < numTot; i++) {
        // Only for non basic variables
        if (!nonbasicFlag[i])
            continue;
        // Free
        if (workLower[i] == -inf && workUpper[i] == inf)
            workCount += (fabs(workDual[i]) >= tau_d);
        // In primal don't assume flip
        workCount += (nonbasicMove[i] * workDual[i] <= -tau_d);
    }
    *dualInfeasCount = workCount;
}

void HModel::correctDual(int *freeInfeasCount) {
    const double tau_d = dblOption[DBLOPT_DUAL_TOL];
    const double inf = HSOL_CONST_INF;
    int workCount = 0;
    for (int i = 0; i < numTot; i++) {
        if (nonbasicFlag[i]) {
            if (workLower[i] == -inf && workUpper[i] == inf) {
                // FREE variable
                workCount += (fabs(workDual[i]) >= tau_d);
            } else if (nonbasicMove[i] * workDual[i] <= -tau_d) {
                if (workLower[i] != -inf && workUpper[i] != inf) {
                    // Boxed variable = flip
                    flipBound(i);
                } else {
                    // Other variable = shift
                    problemPerturbed = 1;
                    if (nonbasicMove[i] == 1) {
                        double dual = (1 + random.dblRandom()) * tau_d;
                        double shift = dual - workDual[i];
                        workDual[i] = dual;
                        workCost[i] = workCost[i] + shift;
                    } else {
                        double dual = -(1 + random.dblRandom()) * tau_d;
                        double shift = dual - workDual[i];
                        workDual[i] = dual;
                        workCost[i] = workCost[i] + shift;
                    }
                }
            }
        }
    }

    *freeInfeasCount = workCount;
}

void HModel::computePrimal() {
    buffer.clear();
    for (int i = 0; i < numTot; i++)
        if (nonbasicFlag[i] && workValue[i] != 0)
            matrix.collect_aj(buffer, i, workValue[i]);
    factor.ftran(buffer, 1);
    for (int i = 0; i < numRow; i++) {
        int iCol = basicIndex[i];
        baseValue[i] = -buffer.array[i];
        baseLower[i] = workLower[iCol];
        baseUpper[i] = workUpper[iCol];
    }
}

void HModel::computeObject(int phase) {
    objective = 0;
    for (int i = 0; i < numTot; i++)
        if (nonbasicFlag[i])
            objective += workValue[i] * workDual[i];
    if (phase != 1)
        objective -= objOffset;
}

void HModel::shiftCost(int iCol, double amount) {
    problemPerturbed = 1;
    assert(workShift[iCol] == 0);
    workShift[iCol] = amount;
}

void HModel::shiftBack(int iCol) {
    workDual[iCol] -= workShift[iCol];
    workShift[iCol] = 0;
}

void HModel::flipBound(int iCol) {
    const int move = nonbasicMove[iCol] = -nonbasicMove[iCol];
    workValue[iCol] = move == 1 ? workLower[iCol] : workUpper[iCol];
}

void HModel::updateFactor(HVector *column, HVector *row_ep, int *iRow, int *hint) {
    timer.recordStart(HTICK_UPDATE_FACTOR);
    factor.update(column, row_ep, iRow, hint);
    if (countUpdate >= limitUpdate)
        *hint = 1;
    timer.recordFinish(HTICK_UPDATE_FACTOR);
}

void HModel::updateMatrix(int columnIn, int columnOut) {
    timer.recordStart(HTICK_UPDATE_FACTOR);
    matrix.update(columnIn, columnOut);
    timer.recordFinish(HTICK_UPDATE_FACTOR);
}

void HModel::updatePivots(int columnIn, int rowOut, int sourceOut) {
    int columnOut = basicIndex[rowOut];

    // Incoming variable
    basicIndex[rowOut] = columnIn;
    nonbasicFlag[columnIn] = 0;
    nonbasicMove[columnIn] = 0;
    baseLower[rowOut] = workLower[columnIn];
    baseUpper[rowOut] = workUpper[columnIn];

    // Outgoing variable
    nonbasicFlag[columnOut] = 1;
    if (workLower[columnOut] == workUpper[columnOut]) {
        workValue[columnOut] = workLower[columnOut];
        nonbasicMove[columnOut] = 0;
    } else if (sourceOut == -1) {
        workValue[columnOut] = workLower[columnOut];
        nonbasicMove[columnOut] = 1;
    } else {
        workValue[columnOut] = workUpper[columnOut];
        nonbasicMove[columnOut] = -1;
    }

    countUpdate++;
}

void HModel::reportPivots(int columnIn, int columnOut, double alpha) {
    if (columnIn >= 0)
        numberIteration++;
    historyColumnIn.push_back(columnIn);
    historyColumnOut.push_back(columnOut);
    historyAlpha.push_back(alpha);
}

void HModel::reportStatus(int status) {
    problemStatus = status;
}

void HModel::printMessage(const char *message) {
    if (intOption[INTOPT_PRINT_FLAG])
        fprintf(stderr, "%s\n", message);
}

void HModel::printObject() {
    if (intOption[INTOPT_PRINT_FLAG])
        fprintf(stderr, "%10d  %20.10e\n", numberIteration, objective);
}

void HModel::printProgress() {
    //    return;
    //    static double nextReport = 0;
    //    double currentTime = timer.getTime();
    //    if (currentTime >= nextReport) {
    //        computeObject();
    //        fprintf(stderr,"PROGRESS %16s %20.10e %10d %10.3f\n", modelName.c_str(),
    //                objective, numberIteration, timer.getTime());
    //        if (currentTime < 50) {
    //            nextReport = ((int) (5 * currentTime + 1)) / 5.0 - 0.00001;
    //        } else if (currentTime < 500) {
    //            nextReport = ((int) (currentTime + 1)) - 0.00001;
    //        } else {
    //            nextReport = ((int) (0.2 * currentTime + 1)) / 0.2 - 0.00001;
    //        }
    //    }
}

// Upstream file: HPrimal.cpp

using namespace std;

void HPrimal::solvePhase2(HModel *ptr_model) {
    // Copy size
    model = ptr_model;
    numCol = model->getNumCol();
    numRow = model->getNumRow();
    numTot = model->getNumTot();

    // Setup update limits
    limitUpdate = min(100 + numRow / 100, 1000);
    countUpdate = 0;

    // Setup local vectors
    column.setup(numRow);
    row_ep.setup(numRow);
    row_ap.setup(numCol);
    columnDensity = 0;
    row_epDensity = 0;

    // Setup other buffers

    model->printMessage("primal-start");

    for (;;) {
        primalRebuild();

        for (;;) {
            primalChooseColumn();
            if (columnIn == -1)
                break;

            primalChooseRow();
            if (rowOut == -1) {
                break;
            }
            primalUpdate();
            if (invertHint)
                break;
        }

        // Fresh factorization required
        if (countUpdate == 0)
            break;
    }
    if (columnIn == -1) {
        model->printMessage("primal-optimal");
        model->printMessage("problem-optimal");
        model->reportStatus(LP_Status_Optimal);
    } else {
        model->printMessage("primal-unbounded");
        model->reportStatus(LP_Status_Unbounded);
    }
}

void HPrimal::primalRebuild() {
    check_deadline();
    model->reportPivots(-1, -1, 0); // Indicate REINVERT

    // Rebuild model->factor - only if we got updates
    invertHint = 0;
    if (countUpdate > 0) {
        model->computeFactor();
        countUpdate = 0;
    }

    model->computeDual();
    model->computePrimal();
    model->computeObject();
    model->printObject();
}

void HPrimal::primalChooseColumn() {
    check_deadline();
    columnIn = -1;
    double bestInfeas = 0;
    const int *jFlag = model->getNonbasicFlag();
    const int *jMove = model->getNonbasicMove();
    double *workDual = model->getWorkDual();
    const double *workLower = model->getWorkLower();
    const double *workUpper = model->getWorkUpper();
    const double dualTolerance = model->dblOption[DBLOPT_DUAL_TOL];

    for (int iCol = 0; iCol < numTot; iCol++) {
        if (jFlag[iCol] && fabs(workDual[iCol]) > dualTolerance) {
            // Always take free
            // Then deal with it in dual phase 1
            if (workLower[iCol] == -HSOL_CONST_INF && workUpper[iCol] == HSOL_CONST_INF) {
                columnIn = iCol;
                break;
            }
            // Then look at dual infeasible
            if (jMove[iCol] * workDual[iCol] < -dualTolerance) {
                if (bestInfeas < fabs(workDual[iCol])) {
                    bestInfeas = fabs(workDual[iCol]);
                    columnIn = iCol;
                }
            }
        }
    }
}

void HPrimal::primalChooseRow() {
    const double *baseLower = model->getBaseLower();
    const double *baseUpper = model->getBaseUpper();
    double *baseValue = model->getBaseValue();
    const double primalTolerance = model->dblOption[DBLOPT_PRIMAL_TOL];

    // Compute pivot column
    column.clear();
    column.packFlag = true;
    model->getMatrix()->collect_aj(column, columnIn, 1);
    model->getFactor()->ftran(column, columnDensity);
    columnDensity = 0.95 * columnDensity + 0.05 * column.count / numRow;

    // Initialize
    rowOut = -1;

    // Choose column pass 1
    double alphaTol = countUpdate < 10 ? 1e-9 : countUpdate < 20 ? 1e-8 : 1e-7;
    const int *jMove = model->getNonbasicMove();
    int moveIn = jMove[columnIn];
    if (moveIn == 0) {
        // If there's still free in the N
        // We would report not-solved
        // Need to handle free
    }
    double relaxTheta = 1e100;
    for (int i = 0; i < column.count; i++) {
        int index = column.index[i];
        double alpha = column.array[index] * moveIn;
        if (alpha > alphaTol) {
            double relaxSpace = baseValue[index] - baseLower[index] + primalTolerance;
            if (relaxSpace < relaxTheta * alpha)
                relaxTheta = relaxSpace / alpha;
        } else if (alpha < -alphaTol) {
            double relaxSpace = baseValue[index] - baseUpper[index] - primalTolerance;
            if (relaxSpace > relaxTheta * alpha)
                relaxTheta = relaxSpace / alpha;
        }
    }

    // Choose column pass 2
    double bestAlpha = 0;
    for (int i = 0; i < column.count; i++) {
        int index = column.index[i];
        double alpha = column.array[index] * moveIn;
        if (alpha > alphaTol) {
            double tightSpace = baseValue[index] - baseLower[index];
            if (tightSpace < relaxTheta * alpha) {
                if (bestAlpha < alpha) {
                    bestAlpha = alpha;
                    rowOut = index;
                }
            }
        } else if (alpha < -alphaTol) {
            double tightSpace = baseValue[index] - baseUpper[index];
            if (tightSpace > relaxTheta * alpha) {
                if (bestAlpha < -alpha) {
                    bestAlpha = -alpha;
                    rowOut = index;
                }
            }
        }
    }
}

void HPrimal::primalUpdate() {
    int *jMove = model->getNonbasicMove();
    double *workDual = model->getWorkDual();
    const double *workLower = model->getWorkLower();
    const double *workUpper = model->getWorkUpper();
    const double *baseLower = model->getBaseLower();
    const double *baseUpper = model->getBaseUpper();
    double *workValue = model->getWorkValue();
    double *baseValue = model->getBaseValue();
    const double primalTolerance = model->dblOption[DBLOPT_PRIMAL_TOL];

    // Compute thetaPrimal
    int moveIn = jMove[columnIn];
    int columnOut = model->getBaseIndex()[rowOut];
    double alpha = column.array[rowOut];
    double thetaPrimal = 0;
    if (alpha * moveIn > 0) {
        // Lower bound
        thetaPrimal = (baseValue[rowOut] - baseLower[rowOut]) / alpha;
    } else {
        // Upper bound
        thetaPrimal = (baseValue[rowOut] - baseUpper[rowOut]) / alpha;
    }

    // 1. Make sure it is inside bounds or just flip bound
    double lowerIn = workLower[columnIn];
    double upperIn = workUpper[columnIn];
    double valueIn = workValue[columnIn] + thetaPrimal;
    bool flipped = false;
    if (jMove[columnIn] == 1) {
        if (valueIn > upperIn + primalTolerance) {
            // Flip to upper
            workValue[columnIn] = upperIn;
            thetaPrimal = upperIn - lowerIn;
            flipped = true;
            jMove[columnIn] = -1;
        }
    } else if (jMove[columnIn] == -1) {
        if (valueIn < lowerIn - primalTolerance) {
            // Flip to lower
            workValue[columnIn] = lowerIn;
            thetaPrimal = lowerIn - upperIn;
            flipped = true;
            jMove[columnIn] = 1;
        }
    }

    for (int i = 0; i < column.count; i++) {
        int index = column.index[i];
        baseValue[index] -= thetaPrimal * column.array[index];
    }

    // If flipped, then no need touch the pivots
    if (flipped) {
        return;
    }

    // Pivot in
    int sourceOut = alpha * moveIn > 0 ? -1 : 1;
    model->updatePivots(columnIn, rowOut, sourceOut);

    baseValue[rowOut] = valueIn;

    // Check for any possible infeasible
    for (int iRow = 0; iRow < numRow; iRow++) {
        if (baseValue[iRow] < baseLower[iRow] - primalTolerance) {
            invertHint = 1;
        } else if (baseValue[iRow] > baseUpper[iRow] + primalTolerance) {
            invertHint = 1;
        }
    }

    // 2. Now we can update the dual
    row_ep.clear();
    row_ap.clear();
    row_ep.count = 1;
    row_ep.index[0] = rowOut;
    row_ep.array[rowOut] = 1;
    row_ep.packFlag = true;
    model->getFactor()->btran(row_ep, row_epDensity);
    model->getMatrix()->price_by_row(row_ap, row_ep);
    row_epDensity = 0.95 * row_epDensity + 0.05 * row_ep.count / numRow;

    double thetaDual = workDual[columnIn] / alpha;
    for (int i = 0; i < row_ap.count; i++) {
        int iCol = row_ap.index[i];
        workDual[iCol] -= thetaDual * row_ap.array[iCol];
    }
    for (int i = 0; i < row_ep.count; i++) {
        int iGet = row_ep.index[i];
        int iCol = iGet + numCol;
        workDual[iCol] -= thetaDual * row_ep.array[iGet];
    }

    // Dual for the pivot
    workDual[columnIn] = 0;
    workDual[columnOut] = -thetaDual;

    // Update model->factor basis
    model->updateFactor(&column, &row_ep, &rowOut, &invertHint);
    model->updateMatrix(columnIn, columnOut);
    if (++countUpdate >= limitUpdate)
        invertHint = true;

    model->reportPivots(columnIn, columnOut, alpha);
}

// Upstream file: HVector.cpp

void HVector::setup(int size_) {
    size = size_;
    count = 0;
    index.resize(size);
    array.assign(size, 0);
    cwork.assign(size + 6400, 0); // MAX invert
    iwork.assign(size * 4, 0);

    packCount = 0;
    packIndex.resize(size);
    packValue.resize(size);
}

void HVector::clear() {
    int clearVector_inDense = count < 0 || count > size * 0.3;
    if (clearVector_inDense) {
        array.assign(size, 0);
    } else {
        for (int i = 0; i < count; i++)
            array[index[i]] = 0;
    }
    packFlag = false;
    count = 0;
    pseudoTick = 0;
    fakeTick = 0;
    next = 0;
}

void HVector::tight() {
    int totalCount = 0;
    for (int i = 0; i < count; i++) {
        const int my_index = index[i];
        const double value = array[my_index];
        if (fabs(value) > HSOL_CONST_TINY) {
            index[totalCount++] = my_index;
        } else {
            array[my_index] = 0;
        }
    }
    count = totalCount;
}

void HVector::pack() {
    if (packFlag) {
        packFlag = false;
        packCount = 0;

        for (int i = 0; i < count; i++) {
            const int ipack = index[i];
            packIndex[packCount] = ipack;
            packValue[packCount] = array[ipack];
            packCount++;
        }
    }
}

void HVector::copy(const HVector *from) {
    clear();
    fakeTick = from->fakeTick;
    pseudoTick = from->pseudoTick;
    const int fromCount = count = from->count;
    const int *fromIndex = &from->index[0];
    const double *fromArray = &from->array[0];
    for (int i = 0; i < fromCount; i++) {
        const int iFrom = fromIndex[i];
        const double xFrom = fromArray[iFrom];
        index[i] = iFrom;
        array[iFrom] = xFrom;
    }
}

double HVector::norm2() {
    const int workCount = count;
    const int *workIndex = &index[0];
    const double *workArray = &array[0];

    double result = 0;
    for (int i = 0; i < workCount; i++) {
        double value = workArray[workIndex[i]];
        result += value * value;
    }
    return result;
}

void HVector::saxpy(const double pivotX, const HVector *pivot) {
    int workCount = count;
    int *workIndex = &index[0];
    double *workArray = &array[0];

    const int pivotCount = pivot->count;
    const int *pivotIndex = &pivot->index[0];
    const double *pivotArray = &pivot->array[0];

    for (int k = 0; k < pivotCount; k++) {
        const int iRow = pivotIndex[k];
        const double x0 = workArray[iRow];
        const double x1 = x0 + pivotX * pivotArray[iRow];
        if (x0 == 0)
            workIndex[workCount++] = iRow;
        workArray[iRow] = (fabs(x1) < HSOL_CONST_TINY) ? HSOL_CONST_ZERO : x1;
    }
    count = workCount;
}

} // namespace hsol_embed
// Adapter for this project's max-profit packing LPs: A*x <= 1, lower <= x <= upper.
// Each column is stored as the row indices whose coefficients equal one.
// One active solve per process; intended for the single-core CMS solver.
namespace embedded_hsol {
struct WarmBasis {
    int rows = 0, cols = 0;
    std::vector<int> basic, move;
};

struct Result {
    std::shared_ptr<const WarmBasis> basis;
    // Upstream status: 0 optimal, 1 infeasible, 2 unbounded, 3 singular, 4 failure.
    // Adapter status 5 means the caller's deadline was reached.
    int status = 4;
    int iterations = 0;
    double objective = 0;
    double solve_seconds = 0;
    std::vector<double> primal, dual;
};
inline Result solve(int rows, const std::vector<std::vector<int>> &column_rows,
                    const std::vector<double> &profit, const std::vector<double> &lower,
                    const std::vector<double> &upper,
                    std::chrono::steady_clock::time_point deadline,
                    const WarmBasis *warm = nullptr) {
    Result result;
    const size_t cols = profit.size();
    if (rows < 0 || column_rows.size() != cols || lower.size() != cols || upper.size() != cols)
        return result;
    for (size_t j = 0; j < cols; ++j) {
        if (!std::isfinite(profit[j]) || !std::isfinite(lower[j]) || lower[j] < 0 ||
            std::isnan(upper[j]))
            return result;
        if (lower[j] > upper[j]) {
            result.status = 1;
            return result;
        }
        for (int row : column_rows[j])
            if (row < 0 || row >= rows)
                return result;
    }
    if (cols == 0) {
        result.status = 0;
        result.dual.assign(rows, 0);
        return result;
    }
    struct DeadlineScope {
        std::chrono::steady_clock::time_point old;
        explicit DeadlineScope(std::chrono::steady_clock::time_point next)
            : old(hsol_embed::caller_deadline) {
            hsol_embed::caller_deadline = next;
        }
        ~DeadlineScope() {
            hsol_embed::caller_deadline = old;
        }
    } deadline_scope(deadline);
    try {
        hsol_embed::check_deadline();
        hsol_embed::HModel model;
        model.numRow = rows;
        model.numCol = static_cast<int>(cols);
        model.numTot = rows + static_cast<int>(cols);
        model.objOffset = 0;
        model.timer.reset();
        model.totalTime = 0;
        model.problemStatus = -1;
        model.numberIteration = 0;
        model.modelName = "route-bank";
        model.intOption[hsol_embed::INTOPT_SCALE_FLAG] = 0;
        model.rowLower.assign(rows, -hsol_embed::HSOL_CONST_INF);
        model.rowUpper.assign(rows, 1.0);
        model.colLower = lower;
        model.colUpper = upper;
        size_t nonzeros = 0;
        for (const auto &indices : column_rows)
            nonzeros += indices.size();
        model.colCost.reserve(cols);
        model.Astart.reserve(cols + 1);
        model.Aindex.reserve(nonzeros);
        model.Avalue.reserve(nonzeros);
        for (size_t j = 0; j < cols; ++j) {
            model.colCost.push_back(-profit[j]);
            model.Astart.push_back(static_cast<int>(model.Aindex.size()));
            for (int row : column_rows[j]) {
                model.Aindex.push_back(row);
                model.Avalue.push_back(1.0);
            }
            if (!std::isfinite(model.colUpper[j]))
                model.colUpper[j] = hsol_embed::HSOL_CONST_INF;
        }
        model.Astart.push_back(static_cast<int>(model.Aindex.size()));
        if (model.Aindex.empty()) { // No row coefficient restricts any variable.
            result.primal = lower;
            result.dual.assign(rows, 0);
            for (size_t j = 0; j < cols; ++j) {
                if (profit[j] > 0 && !std::isfinite(upper[j])) {
                    result.status = 2;
                    return result;
                }
                result.primal[j] = profit[j] > 0 ? upper[j] : lower[j];
                result.objective += profit[j] * result.primal[j];
            }
            result.status = 0;
            return result;
        }
        model.intBreak.resize(model.numTot);
        std::iota(model.intBreak.begin(), model.intBreak.end(), 0);
        for (int i = model.numTot - 1; i >= 1; --i) {
            int j = model.random.intRandom() % (i + 1);
            std::swap(model.intBreak[i], model.intBreak[j]);
        }
        model.dblXpert.resize(model.numTot);
        for (double &value : model.dblXpert)
            value = model.random.dblRandom();
        model.setup_allocWorking();
        model.initCost();
        model.initBound();
        model.initValue();
        bool imported = false;
        if (warm && warm->rows == rows && warm->cols == int(cols) &&
            warm->basic.size() == size_t(rows) && warm->move.size() == size_t(model.numTot)) {
            std::vector<unsigned char> seen(model.numTot, 0);
            bool valid = true;
            for (int j : warm->basic) {
                if (j < 0 || j >= model.numTot || seen[j]) {
                    valid = false;
                    break;
                }
                seen[j] = 1;
            }
            if (valid) {
                // Copy into existing storage: factor.setup holds basicIndex.data().
                std::copy(warm->basic.begin(), warm->basic.end(), model.basicIndex.begin());
                for (int j = 0; j < model.numTot; ++j)
                    model.nonbasicFlag[j] = !seen[j];
                for (int j : warm->basic)
                    if (j < int(cols))
                        model.matrix.update(j, int(cols));
                model.initValue();
                for (int j = 0; j < model.numTot; ++j)
                    if (model.nonbasicFlag[j] && warm->move[j] < 0 &&
                        model.workUpper[j] < hsol_embed::HSOL_CONST_INF &&
                        model.workLower[j] != model.workUpper[j]) {
                        model.nonbasicMove[j] = -1;
                        model.workValue[j] = model.workUpper[j];
                    }
                imported = true;
            }
        }

        hsol_embed::check_deadline();
        hsol_embed::HDual solver;
        auto start = std::chrono::steady_clock::now();
        solver.solve(&model, hsol_embed::HDUAL_VARIANT_PLAIN, 1);
        result.solve_seconds =
            std::chrono::duration<double>(std::chrono::steady_clock::now() - start).count();
        result.status = model.problemStatus;
        result.iterations = model.numberIteration;
        if (result.status == hsol_embed::LP_Status_Optimal) {
            auto basis = std::make_shared<WarmBasis>();
            basis->rows = rows;
            basis->cols = cols;
            basis->basic = model.basicIndex;
            basis->move = model.nonbasicMove;
            result.basis = basis;
            auto values = model.workValue;
            for (int i = 0; i < rows; ++i)
                values[model.basicIndex[i]] = model.baseValue[i];
            result.primal.assign(values.begin(), values.begin() + cols);
            result.dual.resize(rows);
            for (int i = 0; i < rows; ++i)
                result.dual[i] = model.workDual[cols + i];
            for (size_t j = 0; j < cols; ++j)
                result.objective += result.primal[j] * profit[j];
        }
    } catch (const hsol_embed::DeadlineReached &) {
        result.status = 5;
    } catch (const std::exception &) {
        result.status = 4;
    }
    if (warm && result.status != 0 && result.status != 1 && result.status != 5 &&
        std::chrono::steady_clock::now() < deadline) {
        return solve(rows, column_rows, profit, lower, upper, deadline, nullptr);
    }
    return result;
}
} // namespace embedded_hsol

// Passenger-parcel routing: generate routes, select a compatible fleet, then improve it.
// Layout: settings/data -> declarations -> pipeline/search -> helper implementations.
// Node IDs are one-based; taxi and request indices are zero-based. Tabs use width 4.
#pragma GCC optimize("O3", "unroll-loops")
#include <bits/stdc++.h>
using namespace std;

using Clock = chrono::steady_clock;
using Seconds = chrono::seconds;
using Milliseconds = chrono::milliseconds;
using IntVector = vector<int>;
using UInt64 = uint64_t;
using Real = double;
using RealVector = vector<Real>;
using IndexVector = vector<size_t>;
using Deadline = Clock::time_point;
using Route = IntVector;
using Fleet = vector<Route>;

// --- Settings: numerical tolerances and random streams ---
Real INF = numeric_limits<Real>::infinity();
const Real FEASIBILITY_EPSILON = 1e-8, IMPROVEMENT_EPSILON = 1e-9, PIVOT_EPSILON = 1e-9,
           LP_FEASIBILITY_EPSILON = 1e-7, BOUND_RELATIVE_EPSILON = 1e-10;
// Fixed increment gives repeatable phase-specific random streams.
const UInt64 SEED_STRIDE = 0x9e3779b97f4a7c15ULL;

// --- Settings: search effort ---
// Fleet-size thresholds count taxis, not requests or nodes.
constexpr int SMALL_FLEET_TAXI_LIMIT = 48; // Enable repeated selection and pricing above this size.
constexpr int GROUP_MASTER_TAXI_LIMIT = 64;
constexpr int LONG_GENERATION_TAXI_LIMIT = 64;
constexpr int LARGE_FLEET_TAXI_LIMIT = 96;

// Phase budgets in seconds, with the output margin included in the search deadline.
constexpr int SEARCH_SECONDS = 298;
constexpr int INITIAL_LARGE_DFS_SECONDS = 2;
constexpr int INITIAL_SMALL_DFS_SECONDS = 40;
constexpr int INITIAL_SELECTION_SECONDS = 20;
constexpr int LARGE_GENERATION_RESERVE_SECONDS = 8;
constexpr int SMALL_GENERATION_RESERVE_SECONDS = 4;
constexpr int SMALL_GENERATION_SECONDS = 100;
constexpr int SELECTION_ROUND_SECONDS = 6;
constexpr int REBUILD_ROUND_SECONDS = 6;
constexpr int PRICING_GUARD_SECONDS = 2;
constexpr int PRICED_GENERATION_SECONDS = 3;
constexpr int FINAL_SELECTION_RESERVE_SECONDS = 2;
constexpr int FINAL_SELECTION_SECONDS = 80;
constexpr int FINAL_POLISH_CYCLES = 2;

// Route generation and randomized rebuilding.
constexpr int DEFAULT_BANK_ROUTE_LIMIT = 1000;
constexpr int LARGE_FLEET_BANK_LIMIT = 400;
constexpr int DFS_PREFIX_LIMIT = 1000000;
constexpr int DFS_CLOCK_INTERVAL = 64;
constexpr int LANDMARK_LIMIT = 8;
constexpr int LANDMARK_CLOCK_INTERVAL = 64;
constexpr Real LANDMARK_MAX_SECONDS = .6;
constexpr Real LANDMARK_BUDGET_FRACTION = .25;
constexpr int REBUILD_RESTART_INTERVAL = 8;
constexpr Real REBUILD_PROFIT_RETENTION = .995;
constexpr int REBUILD_POLISH_INTERVAL = 2;
constexpr int MIN_REMOVED_REQUESTS = 4;
constexpr int REQUEST_REMOVAL_DIVISOR = 12;
constexpr int TAXI_REMOVAL_DIVISOR = 12;
constexpr int PRICED_BANK_STARTS = 3;
constexpr int PRICED_MOVE_LIMIT = 48;
constexpr int PRICED_RESTART_LIMIT = 8;

// LP, group selection, and request pricing.
constexpr int SIMPLEX_ITERATION_LIMIT = 10000;
constexpr int SIMPLEX_CLOCK_MASK =
    7; // Check time every eight iterations, including iteration zero.
constexpr int GROUP_MIN_TAXIS = 8;
constexpr int GROUP_TAXI_INCREMENT = 4;
constexpr int GROUP_WIDTH_LEVELS = 4;
constexpr int GROUP_RANDOM_SEED_CANDIDATES = 24;
constexpr int GROUP_OVERLAP_CANDIDATES = 32;
constexpr int GROUP_ROUTE_CANDIDATES = 64;
constexpr int GROUP_SOLVE_MILLISECONDS = 150;
constexpr int GROUP_STAGNATION_ROUNDS_PER_TAXI = 12;
constexpr int RANKED_LP_SECONDS = 6;
constexpr int RANKED_FIRST_SOLVE_SECONDS = 4;
constexpr int RANKED_WIDE_SOLVE_SECONDS = 6;
constexpr int RANKED_GROUP_GUARD_MILLISECONDS = 100;
constexpr int PRICE_ITERATION_LIMIT = 192;
constexpr Real PRICE_STEP_FACTOR = .7;

// --- Data structures ---
struct Request {
    int pickup_node, delivery_node;
    bool is_passenger;
    Real weight, pickup_earliest, pickup_latest, delivery_earliest, delivery_latest, service_time,
        revenue;
};

// Each taxi keeps its best route for each sorted set of served requests.
struct StoredRoute {
    string route;
    Real profit;
};

struct Instance {
    int taxi_count, passenger_count, parcel_count, node_count, request_count;
    IntVector taxi_origin;
    RealVector taxi_capacity, travel_time, travel_cost;
    vector<Request> requests;
    IntVector node_request, is_delivery;
};

// A master column is one complete taxi route and the requests it covers.
// Views refer to immutable map keys during selection; reduced/group masters
// share those same lists instead of allocating a copy for every column.
struct RequestList {
    const IntVector *values = nullptr;
    RequestList() = default;
    RequestList(const IntVector &source) : values(&source) {
    }
    RequestList(IntVector &&) = delete;
    auto begin() const {
        return values->begin();
    }
    auto end() const {
        return values->end();
    }
    size_t size() const {
        return values->size();
    }
};
struct MasterColumn {
    uint32_t taxi = 0;
    RequestList requests;
    Real profit = 0;
    MasterColumn(size_t taxi_id, RequestList ids, Real value)
        : taxi(static_cast<uint32_t>(taxi_id)), requests(ids), profit(value) {
    }
};

struct MasterResult {
    IntVector selected_columns;
    Real profit = 0;
    bool timed_out = false;
};

struct PackingMatrix {
    const vector<MasterColumn> *columns;
    vector<IntVector> column_rows;
    IndexVector row_resources;
    size_t dimension = 0, taxi_count;
};

// Revised simplex state: basis inverse, basic values, and resource dual prices.
struct BackendLPData {
    std::shared_ptr<const embedded_hsol::WarmBasis> basis;
    RealVector primal;
    bool infeasible = false;
};
struct PackingLP {
    const PackingMatrix *matrix;
    size_t row_count;
    RealVector basis_inverse, basic_values, dual_prices, right_hand_sides, bound_signs;
    IndexVector basis_columns;
    vector<unsigned char> in_basis;
    IntVector column_bound_rows;
    bool timed_out = false;
    shared_ptr<const BackendLPData> backend;
    RealVector backend_lower, backend_upper;
};

// Positive, globally unique route versions invalidate move caches after every update.
struct SearchState {
    Fleet routes;
    IntVector request_owner;
    RealVector route_profit;
    vector<UInt64> route_version;
    Real profit = 0;
};

// Both positions refer to the original route; insert delivery before pickup.
struct InsertionChoice {
    int request = -1, taxi = -1, pickup_position = 0, delivery_position = 0;
    Real profit_delta = -INF;
};

// A segment maps first-node arrival a to max(a + duration, earliest_departure).
// Arrival must be <= latest_arrival; load extrema include the empty prefix.
struct Segment {
    int first_node = 0, last_node = 0;
    Real duration = 0, earliest_departure = -INF, latest_arrival = INF, load_change = 0,
         minimum_load = 0, maximum_load = 0;
    bool feasible = true;
};

// Disjoint sparse table: join two summaries to query any contiguous route range.
struct RouteSegmentCache {
    Route nodes;
    vector<Segment> node_segments;
    vector<vector<Segment>> range_table;
};

// Cache request/taxi insertions until that taxi's route version changes.
struct InsertionCache {
    vector<vector<UInt64>> route_version;
    vector<vector<InsertionChoice>> choices;
};

struct ReplacementChoice {
    int outgoing_request = -1;
    InsertionChoice insertion;
    Real profit_delta = -INF;
};

struct ReplacementCache {
    vector<UInt64> route_version;
    vector<char> is_unserved;
    vector<vector<ReplacementChoice>> choices;
};

using RouteBank = map<IntVector, StoredRoute>;

// --- Shared working data ---
// One instance per process. Preserve cache lifetimes and globally unique route versions.
// DFS request states: 0 unseen, 1 picked up, 2 delivered.
Instance instance;
UInt64 next_route_version = 1, random_seed = 1;
mt19937_64 random_engine;
vector<RouteBank> route_bank;
RealVector bank_profit_floor;
int bank_route_limit = DEFAULT_BANK_ROUTE_LIMIT;
Deadline search_deadline;
RealVector delivery_time_lower_bound;
vector<IntVector> successor_requests;
IntVector dfs_request_state, dfs_route, dfs_requests;
size_t dfs_visited_prefixes, dfs_prefix_limit = DFS_PREFIX_LIMIT;
Real priced_score_floor = -INF;
Deadline dfs_deadline;
bool dfs_stopped;
RealVector dfs_request_prices;
map<IntVector, pair<Real, Route>> priced_route_bank;
size_t dfs_generation_calls = 0;
vector<RouteSegmentCache> route_segment_cache;
ReplacementCache replacement_cache;
size_t fleet_generation_calls = 0;
size_t priced_replacement_calls = 0;

// --- Search declarations ---
bool enumerate_routes(Deadline deadline, const RealVector &prices = {});
void enumerate_route_prefix(int taxi, int last_node, Real departure_time, Real total_cost,
                            Real revenue, Real load, int open_parcels);
void rebuild_fleets(SearchState &incumbent, Deadline deadline);
void destroy_solution(SearchState &state, int round_index);
void generate_priced_replacements(SearchState &state, Deadline deadline);
void select_fleet_routes(SearchState &state, Deadline deadline, bool full_fleet = false);
MasterResult solve_route_master(const vector<MasterColumn> &columns, size_t taxi_count,
                                size_t request_count, const IntVector &initial_columns,
                                Deadline deadline, bool use_root_fixing = false);
MasterResult solve_taxi_groups(const vector<MasterColumn> &columns, size_t taxi_count,
                               size_t request_count, const IntVector &initial_columns,
                               Deadline deadline, UInt64 group_seed,
                               const RealVector &request_prices, Real bank_upper = INF);
MasterResult solve_ranked_master(const vector<MasterColumn> &columns, int taxi_count,
                                 int request_count, const IntVector &initial_columns,
                                 Deadline deadline);
RealVector estimate_request_prices(Real incumbent_profit = 0, Deadline deadline = Deadline::max());
static void refine_parcel_deliveries(SearchState &state, Deadline deadline);
void improve_solution(SearchState &state, Deadline deadline, bool full_neighborhoods,
                      int cycles = 3);
InsertionChoice find_route_insertion(int request, int taxi, const Route &route,
                                     Real minimum_profit_delta = FEASIBILITY_EPSILON);
InsertionChoice find_fleet_insertion(SearchState &state, int request, InsertionCache &cache);
bool improve_by_removal(SearchState &state);
bool improve_by_insertion(SearchState &state, InsertionCache &cache);
bool improve_by_replacement(SearchState &state, Deadline deadline);
bool improve_by_tail_exchange(SearchState &state, Deadline deadline);
bool improve_by_relocation(SearchState &state, Deadline deadline);
bool improve_by_swap(SearchState &state, Deadline deadline);

// --- Helper declarations ---
Instance read_instance();
optional<Real> evaluate_route(const Instance &input, int taxi, const IntVector &route);
Deadline deadline_after(Real seconds);
Real travel_time(int from_node, int to_node);
Real travel_cost(int from_node, int to_node);
IntVector served_requests(const Route &route);
Real route_cost(int taxi, const Route &route);
Real route_profit(int taxi, const Route &route);
Real fleet_profit(const Fleet &fleet);
void store_route(int taxi, const Route &route, bool enforce_limit = false, Real profit = -INF);
void store_fleet(const Fleet &fleet);
SearchState make_search_state(const Fleet &fleet);
void update_route(SearchState &state, int taxi, const Route &route);
void update_route_pair(SearchState &state, int first_taxi, const Route &first_route,
                       int second_taxi, const Route &second_route);
void apply_insertion(SearchState &state, InsertionChoice choice);
bool is_insertion_boundary(const Route &route, int position);
Route remove_request(const Route &route, int request);
Route insert_request(Route route, int request, int pickup_position, int delivery_position);
IntVector empty_load_cuts(const Route &route);
void build_delivery_lower_bounds(Deadline deadline);
Segment make_node_segment(int node);
Segment join_segments(Segment left, const Segment &right);
bool is_segment_feasible(int taxi, const Segment &segment);
RouteSegmentCache &cached_route_segments(int taxi, const Route &route);
Segment segment_range(const RouteSegmentCache &segments, int begin, int end);
PackingMatrix build_packing_matrix(const vector<MasterColumn> &columns, size_t taxi_count,
                                   size_t request_count);
PackingLP initialize_lp(const PackingMatrix &matrix);
Real column_coefficient(const PackingLP &lp, size_t row, size_t column);
Real reduced_cost(const PackingLP &lp, size_t column);
void pivot_basis(PackingLP &lp, size_t leaving, size_t entering);
void primal_simplex(PackingLP &lp, Deadline deadline);
void add_column_bound(PackingLP &lp, size_t column, bool include);
void dual_simplex(PackingLP &lp, Deadline deadline);
RealVector primal_values(const PackingLP &lp);
Real dual_upper_bound(const PackingLP &lp);
RealVector request_prices_from_lp(const PackingLP &lp, size_t request_count);

// Compact route bytes preserve every node. Large vertex IDs use a wide fallback.
static string pack_stored_route(const Route &route) {
    const bool narrow = instance.node_count <= numeric_limits<uint16_t>::max();
    const size_t width = narrow ? sizeof(uint16_t) : sizeof(uint32_t);
    string packed(route.size() * width, '\0');
    for (size_t i = 0; i < route.size(); ++i) {
        if (narrow) {
            const uint16_t node = static_cast<uint16_t>(route[i]);
            memcpy(&packed[i * width], &node, width);
        } else {
            const uint32_t node = static_cast<uint32_t>(route[i]);
            memcpy(&packed[i * width], &node, width);
        }
    }
    return packed;
}
static Route unpack_stored_route(const string &packed) {
    const bool narrow = instance.node_count <= numeric_limits<uint16_t>::max();
    const size_t width = narrow ? sizeof(uint16_t) : sizeof(uint32_t);
    Route route(packed.size() / width);
    for (size_t i = 0; i < route.size(); ++i) {
        if (narrow) {
            uint16_t node;
            memcpy(&node, &packed[i * width], width);
            route[i] = node;
        } else {
            uint32_t node;
            memcpy(&node, &packed[i * width], width);
            route[i] = static_cast<int>(node);
        }
    }
    return route;
}

// Check before constructing large LP data. The budget reserves headroom below
// the observed500MB judge failure; smaller models keep the original selector.
constexpr uint64_t MASTER_MEMORY_BUDGET = 450000000;
static bool master_memory_limited = false;
static uint64_t process_memory_bytes() {
    ifstream status("/proc/self/statm");
    uint64_t pages = 0, resident = 0;
    if (status >> pages >> resident) {
        const long page_size = sysconf(_SC_PAGESIZE);
        return max(pages, resident) * uint64_t(page_size > 0 ? page_size : 4096);
    }
    rusage usage{};
    if (getrusage(RUSAGE_SELF, &usage) == 0)
        return uint64_t(usage.ru_maxrss) * 1024 + 32000000;
    return MASTER_MEMORY_BUDGET; // Unknown accounting: decline large allocation.
}
static bool master_memory_allows(uint64_t additional) {
    const uint64_t current = process_memory_bytes();
    return current < MASTER_MEMORY_BUDGET && additional <= MASTER_MEMORY_BUDGET - current;
}
static uint64_t master_lp_workspace(size_t columns, size_t rows, uint64_t nonzeros) {
    // Include CSC and row copies, factor workspaces, solver arrays and transient
    // result buffers. This is conservative and checked again after adding cuts.
    return uint64_t(columns) * 256 + nonzeros * 48 + uint64_t(rows) * rows * 256 + 16000000;
}
static bool full_master_fits(const vector<MasterColumn> &columns) {
    uint64_t nonzeros = 0;
    for (const auto &column : columns)
        nonzeros += 1 + column.requests.size();
    return master_memory_allows(master_lp_workspace(
        columns.size(), instance.taxi_count + instance.request_count, nonzeros));
}
static bool lp_copy_fits(const PackingLP &lp) {
    const uint64_t copy_bytes =
        uint64_t(lp.backend_lower.size() + lp.backend_upper.size()) * sizeof(Real) +
        uint64_t(lp.column_bound_rows.size()) * sizeof(int) +
        uint64_t(lp.basis_inverse.size() + lp.dual_prices.size() + lp.right_hand_sides.size() +
                 lp.bound_signs.size() + lp.basic_values.size()) *
            sizeof(Real) +
        1000000;
    return master_memory_allows(copy_bytes);
}

size_t measured_rounds = 0;
struct PerfEntry {
    size_t calls = 0;
    double inclusive = 0, exclusive = 0;
};
static map<string, PerfEntry> perf_entries;
struct PerfScope {
    const char *name;
    Clock::time_point start = Clock::now();
    double children = 0;
    PerfScope *parent;
    static PerfScope *active;
    explicit PerfScope(const char *n) : name(n), parent(active) {
        active = this;
    }
    ~PerfScope() {
        double elapsed = chrono::duration<double>(Clock::now() - start).count();
        auto &entry = perf_entries[name];
        ++entry.calls;
        entry.inclusive += elapsed;
        entry.exclusive += elapsed - children;
        if (parent)
            parent->children += elapsed;
        active = parent;
    }
};
PerfScope *PerfScope::active = nullptr;

// --- Search pipeline ---

// Embedded LP adapter with independently recomputed feasibility and dual checks.
static void solve_embedded_lp(PackingLP &lp, Deadline deadline) {

    auto &columns = *lp.matrix->columns;
    RealVector profit;
    profit.reserve(columns.size());
    for (auto &column : columns)
        profit.push_back(column.profit);
    uint64_t nonzeros = 0;
    for (const auto &rows : lp.matrix->column_rows)
        nonzeros += rows.size();
    if (!master_memory_allows(
            master_lp_workspace(columns.size(), lp.matrix->dimension, nonzeros))) {
        master_memory_limited = true;
        lp.timed_out = true;
        return;
    }
    auto result = embedded_hsol::solve(lp.matrix->dimension, lp.matrix->column_rows, profit,
                                       lp.backend_lower, lp.backend_upper, deadline,
                                       lp.backend ? lp.backend->basis.get() : nullptr);

    auto data = make_shared<BackendLPData>();
    data->basis = result.basis;
    lp.timed_out = false;
    if (result.status == 1) {
        data->infeasible = true;
        lp.backend = data;
        return;
    }
    if (result.status != 0) {
        lp.timed_out = true;
        lp.backend = data;
        return;
    }
    if (result.primal.size() != columns.size() || result.dual.size() != lp.matrix->dimension) {
        lp.timed_out = true;
        lp.backend = data;
        return;
    }
    // Recompute feasibility and reduced costs from the original packing matrix and bounds.
    RealVector usage(lp.matrix->dimension, 0);
    Real primal_error = 0, dual_error = 0;
    for (size_t j = 0; j < columns.size(); j++) {
        Real x = result.primal[j];
        if (!isfinite(x)) {
            primal_error = INF;
            break;
        }
        primal_error = max(primal_error, max(lp.backend_lower[j] - x, x - lp.backend_upper[j]));
        for (auto r : lp.matrix->column_rows[j])
            usage[r] += x;
    }
    for (size_t r = 0; r < usage.size(); r++) {
        primal_error = max(primal_error, usage[r] - 1);
        if (!isfinite(result.dual[r]))
            dual_error = INF;
        else
            dual_error = max(dual_error, -result.dual[r]);
    }
    lp.dual_prices.assign(lp.row_count, 0);
    copy(result.dual.begin(), result.dual.end(), lp.dual_prices.begin());
    for (size_t j = 0; j < columns.size(); j++) {
        Real rc = profit[j];
        for (auto r : lp.matrix->column_rows[j])
            rc -= result.dual[r];
        if (!isfinite(rc)) {
            dual_error = INF;
            break;
        }
        if (!isfinite(lp.backend_upper[j]))
            dual_error = max(dual_error, rc);
        // Map a column bound's multiplier back to the original signed B&B constraint row.
        int row = lp.column_bound_rows[j];
        if (row >= 0)
            lp.dual_prices[row] = lp.bound_signs[row] < 0 ? max(0.0, -rc) : max(0.0, rc);
    }
    if (primal_error > 1e-7 || dual_error > 1e-7) {
        lp.timed_out = true;
        lp.backend = data;
        return;
    }
    // Immutable snapshots permit safe parent/child LP copies; no shared state is mutated.
    data->primal = move(result.primal);
    lp.backend = data;
}

// An uncapped probe can establish complete candidate coverage on sparse inputs.
bool full_probe_mode = false, full_probe_truncated = false, route_bank_complete = false;
size_t full_probe_columns = 0;
constexpr size_t FULL_PROBE_COLUMN_GUARD = 350000;

// Relax request identities while retaining per-taxi route choices and the total
// passenger/parcel counts. This can certify a fleet without allocating an LP.
// Work guards only decline this optional certificate; they never discard routes.
static bool bank_cardinality_certified(const SearchState &state, Deadline deadline,
                                       Real *reported_upper = nullptr) {
    struct Profile {
        int passengers, parcels;
        Real profit;
    };
    const int n = instance.passenger_count, m = instance.parcel_count;
    const size_t states = size_t(n + 1) * (m + 1);
    constexpr size_t MAX_PROFILES = 4096, MAX_UPDATES = 40000000;
    if (states > 1000000 || Clock::now() >= deadline)
        return false;
    RealVector best(states, -INF);
    vector<vector<Profile>> profiles(instance.taxi_count);
    size_t profile_count = 0, scanned = 0;
    int max_passengers = 0, max_parcels = 0;
    bool integral = true;
    for (int taxi = 0; taxi < instance.taxi_count; ++taxi) {
        fill(best.begin(), best.end(), -INF);
        int taxi_max_p = 0, taxi_max_q = 0;
        for (const auto &entry : route_bank[taxi]) {
            if ((++scanned & 1023) == 0 && Clock::now() >= deadline)
                return false;
            int p = 0, q = 0;
            for (int request : entry.first) {
                if (request < n)
                    ++p;
                else
                    ++q;
            }
            const Real profit = entry.second.profit;
            if (!isfinite(profit) || p > n || q > m)
                return false;
            integral &= profit == round(profit);
            best[size_t(p) * (m + 1) + q] = max(best[size_t(p) * (m + 1) + q], profit);
        }
        for (int p = 0; p <= n; ++p)
            for (int q = 0; q <= m; ++q) {
                const Real profit = best[size_t(p) * (m + 1) + q];
                if (!isfinite(profit) || profit <= 0)
                    continue;
                if (++profile_count > MAX_PROFILES)
                    return false;
                profiles[taxi].push_back({p, q, profit});
                taxi_max_p = max(taxi_max_p, p);
                taxi_max_q = max(taxi_max_q, q);
            }
        max_passengers = min(n, max_passengers + taxi_max_p);
        max_parcels = min(m, max_parcels + taxi_max_q);
    }
    const size_t width = max_parcels + 1;
    const size_t dimension = size_t(max_passengers + 1) * width;
    if (profile_count && dimension > MAX_UPDATES / profile_count)
        return false;
    RealVector values(dimension, -INF), next;
    values[0] = 0;
    const Real infinity = numeric_limits<Real>::infinity();
    size_t updates = 0;
    for (const auto &taxi_profiles : profiles) {
        next = values; // Leaving a taxi unused is always allowed.
        for (int p = 0; p <= max_passengers; ++p) {
            for (int q = 0; q <= max_parcels; ++q) {
                const Real previous = values[size_t(p) * width + q];
                if (!isfinite(previous))
                    continue;
                for (const auto &option : taxi_profiles) {
                    if ((++updates & 4095) == 0 && Clock::now() >= deadline)
                        return false;
                    const int new_p = p + option.passengers, new_q = q + option.parcels;
                    if (new_p > max_passengers || new_q > max_parcels)
                        continue;
                    Real &target = next[size_t(new_p) * width + new_q];
                    target = max(target, nextafter(previous + option.profit, infinity));
                }
            }
        }
        values.swap(next);
    }
    const Real upper = *max_element(values.begin(), values.end());
    if (reported_upper)
        *reported_upper = upper;
    const Real lower = fleet_profit(state.routes);
    const bool certified = isfinite(upper) && isfinite(lower) &&
                           (integral ? upper < floor(lower) + 1 : upper <= lower);

    return certified;
}

static void improve_parcel_alns(SearchState &state, Deadline deadline);

// --- Route generation: bounded DFS ---
bool enumerate_routes(Deadline deadline, const RealVector &prices) {
    // Vary request-price strength between calls while keeping each seed reproducible.
    dfs_request_prices = prices;
    auto call = dfs_generation_calls++;
    if (!prices.empty()) {
        random_engine.seed(random_seed + SEED_STRIDE * call);
        Real scale[] = {1, .8, 1.2};
        for (auto &price : dfs_request_prices) {
            price *= scale[call % 3];
        }
    }
    // Build reusable pickup candidates, ordered by latest possible departure.
    if (successor_requests.empty()) {
        build_delivery_lower_bounds(deadline);
        successor_requests.resize(instance.node_count + 1);
        for (int node = 1; node <= instance.node_count; node++) {
            auto &candidates = successor_requests[node];
            for (int request = 0; request < instance.request_count; request++) {
                if (travel_time(node, instance.requests[request].pickup_node) <=
                    instance.requests[request].pickup_latest + FEASIBILITY_EPSILON) {
                    candidates.push_back(request);
                }
            }
            stable_sort(candidates.begin(), candidates.end(), [&](int left, int right) {
                return instance.requests[left].pickup_latest -
                           travel_time(node, instance.requests[left].pickup_node) >
                       instance.requests[right].pickup_latest -
                           travel_time(node, instance.requests[right].pickup_node);
            });
        }
    }
    bool complete = true;
    dfs_request_state.assign(instance.request_count, 0);
    for (int taxi = 0; taxi < instance.taxi_count; taxi++) {
        if (full_probe_mode && full_probe_truncated)
            return false;
        // Divide remaining time among remaining taxis and reset this taxi's DFS state.
        auto current_time = Clock::now();
        if (current_time >= deadline) {
            return false;
        }
        auto taxi_deadline =
            current_time + (deadline - current_time) / (instance.taxi_count - taxi);
        dfs_deadline = taxi_deadline;
        dfs_stopped = false;
        dfs_visited_prefixes = 0;
        dfs_prefix_limit = full_probe_mode ? (size_t(1) << 50) : DFS_PREFIX_LIMIT;
        dfs_route.clear();
        dfs_requests.clear();
        priced_route_bank.clear();
        priced_score_floor = -INF;

        // Ordinary DFS starts at the depot; priced DFS diversifies the first pickup.
        if (prices.empty()) {
            enumerate_route_prefix(taxi, instance.taxi_origin[taxi], 0, 0, 0, 0, 0);
        } else {
            IntVector first_requests;
            for (int request : successor_requests[instance.taxi_origin[taxi]]) {
                if (instance.requests[request].weight <=
                        instance.taxi_capacity[taxi] + FEASIBILITY_EPSILON &&
                    travel_time(instance.taxi_origin[taxi],
                                instance.requests[request].pickup_node) <=
                        instance.requests[request].pickup_latest + FEASIBILITY_EPSILON) {
                    first_requests.push_back(request);
                }
            }
            shuffle(first_requests.begin(), first_requests.end(), random_engine);
            // Share each taxi's time and prefix budget across its shuffled first pickups.
            int remaining_starts = first_requests.size();
            for (int request : first_requests) {
                current_time = Clock::now();
                if (current_time >= taxi_deadline || dfs_visited_prefixes >= DFS_PREFIX_LIMIT) {
                    complete = false;
                    break;
                }
                dfs_deadline = current_time + (taxi_deadline - current_time) / remaining_starts;
                dfs_prefix_limit =
                    dfs_visited_prefixes +
                    max<size_t>(1, (DFS_PREFIX_LIMIT - dfs_visited_prefixes) / remaining_starts);
                dfs_stopped = false;
                auto &request_data = instance.requests[request];
                Real arrival = travel_time(instance.taxi_origin[taxi], request_data.pickup_node),
                     departure_time =
                         max(arrival, request_data.pickup_earliest) + request_data.service_time,
                     initial_cost =
                         travel_cost(instance.taxi_origin[taxi], request_data.pickup_node);
                dfs_requests.push_back(request);
                dfs_route.push_back(request_data.pickup_node);
                dfs_request_state[request] = 1;
                if (request_data.is_passenger) {
                    dfs_request_state[request] = 2;
                    dfs_route.push_back(request_data.delivery_node);
                    enumerate_route_prefix(
                        taxi, request_data.delivery_node,
                        departure_time +
                            travel_time(request_data.pickup_node, request_data.delivery_node) +
                            request_data.service_time,
                        initial_cost +
                            travel_cost(request_data.pickup_node, request_data.delivery_node),
                        request_data.revenue, 0, 0);
                    dfs_route.pop_back();
                } else {
                    enumerate_route_prefix(taxi, request_data.pickup_node, departure_time,
                                           initial_cost, request_data.revenue, request_data.weight,
                                           1);
                }
                dfs_request_state[request] = 0;
                dfs_requests.pop_back();
                dfs_route.pop_back();
                complete &= !dfs_stopped;
                remaining_starts--;
            }
        }
        complete &= !dfs_stopped;

        // Transfer priced candidates to the persistent bank using their actual profits.
        for (auto &[ids, priced_route] : priced_route_bank) {
            store_route(taxi, priced_route.second);
        }
    }
    return complete;
}

// Stop by deadline or prefix count; save profitable prefixes with no open parcels.
void enumerate_route_prefix(int taxi, int last_node, Real departure_time, Real total_cost,
                            Real revenue, Real load, int open_parcels) {
    // Stop the entire DFS call when its prefix or time budget is exhausted.
    if (dfs_stopped) {
        return;
    }
    if (++dfs_visited_prefixes >= dfs_prefix_limit ||
        (dfs_visited_prefixes % DFS_CLOCK_INTERVAL == 0 && Clock::now() >= dfs_deadline)) {
        dfs_stopped = true;
        return;
    }

    // Prune when even a delivery lower bound misses an onboard parcel's deadline.
    for (int request : dfs_requests) {
        if (dfs_request_state[request] == 1 &&
            departure_time + delivery_time_lower_bound[(last_node - 1) * instance.parcel_count +
                                                       request - instance.passenger_count] >
                instance.requests[request].delivery_latest + FEASIBILITY_EPSILON) {
            return;
        }
    }
    // A closed profitable prefix is a complete candidate route; extensions may still improve it.
    if (!open_parcels && revenue - total_cost > FEASIBILITY_EPSILON) {
        if (dfs_request_prices.empty()) {
            store_route(taxi, dfs_route, true, revenue - total_cost);
        } else {
            Real score = revenue - total_cost;
            for (int request : dfs_requests) {
                score -= dfs_request_prices[request];
            }
            if (score > FEASIBILITY_EPSILON &&
                (priced_route_bank.size() < (size_t)bank_route_limit ||
                 score >= priced_score_floor)) {
                auto requests = served_requests(dfs_route);
                auto entry = priced_route_bank.find(requests);
                if (entry == priced_route_bank.end() || entry->second.first < score) {
                    priced_route_bank[requests] = {score, dfs_route};
                    if (priced_route_bank.size() > (size_t)bank_route_limit) {
                        priced_route_bank.erase(
                            min_element(priced_route_bank.begin(), priced_route_bank.end(),
                                        [](auto &left, auto &right) {
                                            return left.second.first < right.second.first;
                                        }));
                        priced_score_floor =
                            min_element(priced_route_bank.begin(), priced_route_bank.end(),
                                        [](auto &left, auto &right) {
                                            return left.second.first < right.second.first;
                                        })
                                ->second.first;
                    }
                }
            }
        }
    }
    // Enumerate onboard deliveries and new pickups; priced DFS rotates their event order.
    size_t pickup_count = successor_requests[last_node].size(),
           delivery_count = dfs_requests.size();
    UInt64 hash = 0;
    if (!dfs_request_prices.empty()) {
        auto &candidates = successor_requests[last_node];
        pickup_count =
            partition_point(candidates.begin(), candidates.end(),
                            [&](int request) {
                                return departure_time <=
                                       instance.requests[request].pickup_latest -
                                           travel_time(last_node,
                                                       instance.requests[request].pickup_node) +
                                           FEASIBILITY_EPSILON;
                            }) -
            candidates.begin();
        // Mix the phase, taxi, and last node to rotate priced DFS event order.
        hash = random_seed + SEED_STRIDE * (dfs_generation_calls - 1) +
               taxi * 0x94d049bb133111ebULL + last_node * 0xbf58476d1ce4e5b9ULL;
        hash = (hash ^ (hash >> 30)) * 0xbf58476d1ce4e5b9ULL;
        hash ^= hash >> 27;
    }
    size_t events = delivery_count + pickup_count,
           offset = events && (hash & 1) ? hash % events : 0;
    for (size_t i = 0; i < events; i++) {
        auto event = offset ? (i + offset) % events : i;
        if (event < delivery_count) {
            // Deliver one onboard parcel, recurse, then restore the prefix.
            int request = dfs_requests[event];
            if (dfs_request_state[request] != 1 || instance.requests[request].is_passenger) {
                continue;
            }
            auto &request_data = instance.requests[request];
            Real arrival = departure_time + travel_time(last_node, request_data.delivery_node);
            if (arrival > request_data.delivery_latest + FEASIBILITY_EPSILON) {
                continue;
            }
            dfs_request_state[request] = 2;
            dfs_route.push_back(request_data.delivery_node);
            enumerate_route_prefix(taxi, request_data.delivery_node,
                                   max(arrival, request_data.delivery_earliest) +
                                       request_data.service_time,
                                   total_cost + travel_cost(last_node, request_data.delivery_node),
                                   revenue, load - request_data.weight, open_parcels - 1);
            dfs_route.pop_back();
            dfs_request_state[request] = 1;
            if (dfs_stopped) {
                return;
            }
            continue;
        }

        // Reject pickups that violate time, capacity, or the request's current state.
        int request = successor_requests[last_node][event - delivery_count];
        auto &request_data = instance.requests[request];
        Real arc_time = travel_time(last_node, request_data.pickup_node);
        Real rounding_margin =
            FEASIBILITY_EPSILON + 8 * numeric_limits<Real>::epsilon() *
                                      max({1.0, abs(request_data.pickup_latest), abs(arc_time)});
        if (departure_time > request_data.pickup_latest - arc_time + rounding_margin) {
            if (dfs_request_prices.empty()) {
                break;
            }
            continue;
        }
        Real arrival = departure_time + arc_time;
        if (arrival > request_data.pickup_latest + FEASIBILITY_EPSILON) {
            continue;
        }
        if (dfs_request_state[request] ||
            load + request_data.weight > instance.taxi_capacity[taxi] + FEASIBILITY_EPSILON) {
            continue;
        }
        Real next_departure =
                 max(arrival, request_data.pickup_earliest) + request_data.service_time,
             next_cost = total_cost + travel_cost(last_node, request_data.pickup_node);
        dfs_request_state[request] = 1;
        dfs_requests.push_back(request);
        dfs_route.push_back(request_data.pickup_node);

        // Serve passengers as adjacent pickup/delivery blocks; leave parcels onboard.
        if (request_data.is_passenger) {
            dfs_request_state[request] = 2;
            dfs_route.push_back(request_data.delivery_node);
            enumerate_route_prefix(
                taxi, request_data.delivery_node,
                next_departure + travel_time(request_data.pickup_node, request_data.delivery_node) +
                    request_data.service_time,
                next_cost + travel_cost(request_data.pickup_node, request_data.delivery_node),
                revenue + request_data.revenue, load, open_parcels);
            dfs_route.pop_back();
            dfs_request_state[request] = 1;
        } else {
            enumerate_route_prefix(taxi, request_data.pickup_node, next_departure, next_cost,
                                   revenue + request_data.revenue, load + request_data.weight,
                                   open_parcels + 1);
        }

        // Backtrack so the next event sees exactly the same incoming prefix.
        dfs_route.pop_back();
        dfs_requests.pop_back();
        dfs_request_state[request] = 0;
        if (dfs_stopped) {
            return;
        }
    }
}

// --- Route generation: fleet rebuilding ---
// Randomized destroy/rebuild rounds add diverse routes to the bank.
// Periodically restart from the best fleet, including after a sufficient profit loss.
void rebuild_fleets(SearchState &incumbent, Deadline deadline) {
    random_engine.seed(random_seed + SEED_STRIDE * fleet_generation_calls++);
    SearchState current = incumbent, best_state = incumbent;
    InsertionCache cache;
    IntVector order(instance.request_count);
    iota(order.begin(), order.end(), 0);
    int round_index = 0;
    while (Clock::now() < deadline) {
        // Perturb the current fleet; periodically return to the best fleet found in this call.
        if (round_index) {
            if (round_index % REBUILD_RESTART_INTERVAL == 0 ||
                current.profit < best_state.profit * REBUILD_PROFIT_RETENTION) {
                current = best_state;
            }
            destroy_solution(current, round_index);
        }

        // Reinsert unserved requests in random order using cached best taxi positions.
        shuffle(order.begin(), order.end(), random_engine);
        for (int request : order) {
            if (Clock::now() >= deadline) {
                break;
            }
            if (current.request_owner[request] >= 0) {
                continue;
            }
            auto choice = find_fleet_insertion(current, request, cache);
            if (choice.taxi >= 0) {
                apply_insertion(current, choice);
            }
        }
        // Complete profitable insertions, then occasionally run the cheaper descent neighborhoods.
        while (Clock::now() < deadline && improve_by_insertion(current, cache)) {
        }
        if (round_index % REBUILD_POLISH_INTERVAL == 0 && Clock::now() < deadline) {
            improve_solution(current, deadline, false);
            if (Clock::now() < deadline)
                improve_parcel_alns(current, deadline);
        }

        // Keep the best complete fleet even when later rounds deliberately lose profit.
        if (current.profit > best_state.profit) {
            best_state = current;
        }
        ++measured_rounds;

        round_index++;
    }
    // Return the best fleet; preserve its routes as candidates for later master solves.
    incumbent = best_state;
    store_fleet(incumbent.routes);
}

// Cycle through whole-taxi, random, related, and profit-based removals.
void destroy_solution(SearchState &state, int round_index) {
    int count =
        max(MIN_REMOVED_REQUESTS,
            (int)count_if(state.request_owner.begin(), state.request_owner.end(), [](int taxi) {
                return taxi >= 0;
            }) / REQUEST_REMOVAL_DIVISOR);
    // Mode 0: empty a few taxis to free whole route combinations.
    if (round_index % 4 == 0) {
        for (int i = 0, taxi_removals = 2 + random_engine() %
                                                max(2, instance.taxi_count / TAXI_REMOVAL_DIVISOR);
             i < taxi_removals; i++) {
            int taxi = random_engine() % instance.taxi_count;
            if (!state.routes[taxi].empty()) {
                update_route(state, taxi, {});
            }
        }
        return;
    }
    IntVector served_ids;
    for (int request = 0; request < instance.request_count; request++) {
        if (state.request_owner[request] >= 0) {
            served_ids.push_back(request);
        }
    }
    if (served_ids.empty()) {
        return;
    }
    // Mode 1: remove random served requests, retaining feasibility after each removal.
    if (round_index % 4 == 1) {
        served_ids.resize(instance.request_count);
        iota(served_ids.begin(), served_ids.end(), 0);
        shuffle(served_ids.begin(), served_ids.end(), random_engine);
        int removed = 0;
        for (int request : served_ids) {
            if (state.request_owner[request] >= 0) {
                int taxi = state.request_owner[request];
                auto route = remove_request(state.routes[taxi], request);
                if (evaluate_route(instance, taxi, route)) {
                    update_route(state, taxi, route);
                    if (++removed == count) {
                        break;
                    }
                }
            }
        }
        return;
    }
    // Mode 2 removes requests related to an anchor; mode 3 favors the best removal gain.
    int anchor = round_index % 4 == 2 ? served_ids[random_engine() % served_ids.size()] : 0;
    for (int removed = 0; removed < count; removed++) {
        int chosen = -1;
        Real best = -INF;
        Route reduced_route;
        for (int request = 0; request < instance.request_count; request++) {
            if (state.request_owner[request] >= 0) {
                int taxi = state.request_owner[request];
                auto route = remove_request(state.routes[taxi], request);
                auto evaluated_profit = evaluate_route(instance, taxi, route);
                if (!evaluated_profit) {
                    continue;
                }
                Real score = *evaluated_profit - state.route_profit[taxi];
                if (round_index % 4 == 2) {
                    score = -travel_time(instance.requests[anchor].pickup_node,
                                         instance.requests[request].pickup_node) -
                            travel_time(instance.requests[request].pickup_node,
                                        instance.requests[anchor].pickup_node) -
                            abs(instance.requests[anchor].pickup_earliest -
                                instance.requests[request].pickup_earliest);
                }
                if (score > best) {
                    best = score;
                    chosen = request;
                    reduced_route = move(route);
                }
            }
        }
        if (chosen < 0) {
            break;
        }
        update_route(state, state.request_owner[chosen], reduced_route);
    }
}

// --- Route generation: priced replacements ---
// Generate routes by insertion/removal/replacement under request opportunity costs.
// Start from the incumbent route and several bank routes, with bounded restarts.
void generate_priced_replacements(SearchState &state, Deadline deadline) {
    // Penalize expensive-to-share requests and vary that penalty across generation calls.
    auto prices = estimate_request_prices(state.profit, deadline);
    auto call = priced_replacement_calls++;
    random_engine.seed(random_seed + SEED_STRIDE * call);
    Real scale[] = {1, .8, 1.2};
    for (auto &price : prices) {
        price *= scale[call % 3];
    }
    for (int taxi = 0; taxi < instance.taxi_count && Clock::now() < deadline; taxi++) {
        // Share time fairly; seed this taxi's search from its incumbent and top bank routes.
        auto current_time = Clock::now(),
             taxi_deadline =
                 current_time + (deadline - current_time) / (instance.taxi_count - taxi);
        vector<pair<Real, const StoredRoute *>> ranked;
        for (auto &[ids, stored_route] : route_bank[taxi]) {
            Real reduced_profit = stored_route.profit;
            for (int request : ids) {
                reduced_profit -= prices[request];
            }
            ranked.push_back({reduced_profit, &stored_route});
        }
        int count = min<int>(PRICED_BANK_STARTS, ranked.size());
        partial_sort(ranked.begin(), ranked.begin() + count, ranked.end(),
                     [](auto &left, auto &right) { return left.first > right.first; });
        vector<Route> seeds{state.routes[taxi]};
        for (int i = 0; i < count; i++) {
            seeds.push_back(unpack_stored_route(ranked[i].second->route));
        }
        SearchState candidate_state = make_search_state(Fleet(instance.taxi_count));
        update_route(candidate_state, taxi, seeds[0]);
        int restarts = 0;
        for (int iteration = 0; iteration < PRICED_MOVE_LIMIT && Clock::now() < taxi_deadline;
             iteration++) {
            Real best = FEASIBILITY_EPSILON;
            Route chosen;
            bool changed = false;
            auto score = [&](const Route &route) {
                Real reduced_profit = route_profit(taxi, route);
                for (int request : served_requests(route)) {
                    reduced_profit -= prices[request];
                }
                return reduced_profit;
            };
            Real previous_score = score(candidate_state.routes[taxi]);

            // Try adding an unserved request, accounting for its request-price penalty.
            for (int request = 0; request < instance.request_count && Clock::now() < taxi_deadline;
                 request++) {
                if (candidate_state.request_owner[request] < 0) {
                    auto choice = find_route_insertion(request, taxi, candidate_state.routes[taxi],
                                                       best + prices[request]);
                    if (choice.taxi >= 0 && choice.profit_delta - prices[request] > best) {
                        best = choice.profit_delta - prices[request];
                        chosen = insert_request(candidate_state.routes[taxi], request,
                                                choice.pickup_position, choice.delivery_position);
                        changed = true;
                    }
                }
            }
            // Compare removals and one-for-one replacements by their change in priced profit.
            for (int outgoing_request : served_requests(candidate_state.routes[taxi])) {
                auto route = remove_request(candidate_state.routes[taxi], outgoing_request);
                Real removal_gain = route_profit(taxi, route) - candidate_state.route_profit[taxi];
                if (evaluate_route(instance, taxi, route) && score(route) - previous_score > best) {
                    best = score(route) - previous_score;
                    chosen = route;
                    changed = true;
                }
                for (int incoming_request = 0;
                     incoming_request < instance.request_count && Clock::now() < taxi_deadline;
                     incoming_request++) {
                    if (candidate_state.request_owner[incoming_request] < 0) {
                        auto choice =
                            find_route_insertion(incoming_request, taxi, route,
                                                 best - removal_gain - prices[outgoing_request] +
                                                     prices[incoming_request]);
                        if (choice.taxi >= 0 && removal_gain + choice.profit_delta +
                                                        prices[outgoing_request] -
                                                        prices[incoming_request] >
                                                    best) {
                            best = removal_gain + choice.profit_delta + prices[outgoing_request] -
                                   prices[incoming_request];
                            chosen = insert_request(route, incoming_request, choice.pickup_position,
                                                    choice.delivery_position);
                            changed = true;
                        }
                    }
                }
            }
            // At a local optimum, restart from another seed and remove a few requests for diversity.
            if (!changed) {
                if (++restarts > PRICED_RESTART_LIMIT) {
                    break;
                }
                update_route(candidate_state, taxi, seeds[restarts % seeds.size()]);
                auto ids = served_requests(candidate_state.routes[taxi]);
                if (!ids.empty()) {
                    shuffle(ids.begin(), ids.end(), random_engine);
                    ids.resize(min<size_t>(ids.size(), 1 + random_engine() % 3));
                    Route reduced;
                    for (int node : candidate_state.routes[taxi]) {
                        if (find(ids.begin(), ids.end(), instance.node_request[node]) ==
                            ids.end()) {
                            reduced.push_back(node);
                        }
                    }
                    if (evaluate_route(instance, taxi, reduced)) {
                        update_route(candidate_state, taxi, reduced);
                    }
                }
                continue;
            }
            // Apply the best priced move and store the resulting route with its actual profit.
            update_route(candidate_state, taxi, chosen);
            store_route(taxi, chosen);
        }
    }
}

// Equal per-request profit shares give feasible nonnegative packing prices:
// each route's request prices sum to at least its profit. Round every operation
// upward, and only skip selection when no integer improvement is possible.
static bool bank_incumbent_certified(const SearchState &state) {
    RealVector price(instance.request_count, 0);
    bool integral = true;
    const Real infinity = numeric_limits<Real>::infinity();
    for (const auto &bank : route_bank)
        for (const auto &entry : bank) {
            const auto &ids = entry.first;
            Real profit = entry.second.profit;
            if (ids.empty() || !isfinite(profit))
                return false;
            integral &= profit == round(profit);
            Real share = nextafter(profit / Real(ids.size()), infinity);
            for (int request : ids)
                price[request] = max(price[request], share);
        }
    Real upper = 0;
    for (Real value : price)
        upper = nextafter(upper + value, infinity);
    Real lower = fleet_profit(state.routes);
    if (!isfinite(upper) || !isfinite(lower))
        return false;
    return integral ? upper < floor(lower) + 1 : upper <= lower;
}

// --- Route selection: master dispatch ---
// Build the master from the bank and retain the incumbent unless profit improves.
extern RealVector fast_request_prices;
void select_fleet_routes(SearchState &state, Deadline deadline, bool full_fleet) {
    // The preceding generation/pricing call may have used its whole slice.
    if (Clock::now() >= deadline)
        return;

    // Preserve incumbent routes and map every stored route to a master column.
    store_fleet(state.routes);
    if (bank_incumbent_certified(state))
        return;
    Real bank_upper = INF;
    if (full_fleet && !route_bank_complete &&
        bank_cardinality_certified(state, deadline, &bank_upper)) {
        return;
    }
    size_t column_count = 0;
    for (const auto &bank : route_bank)
        column_count += bank.size();
    vector<MasterColumn> columns;
    vector<const StoredRoute *> routes;
    columns.reserve(column_count);
    routes.reserve(column_count);
    IntVector initial(instance.taxi_count, -1);
    for (int taxi = 0; taxi < instance.taxi_count; taxi++) {
        for (auto &[ids, stored_route] : route_bank[taxi]) {
            if (ids == served_requests(state.routes[taxi])) {
                initial[taxi] = columns.size();
            }
            columns.push_back({(size_t)taxi, ids, stored_route.profit});
            routes.push_back(&stored_route);
        }
    }
    // Short calls solve a diverse active view,not the complete ever-growing
    // bank. Keep protected incumbent,raw-profit and price-ranked alternatives.
    // The final full-fleet call retains access to every bank column.
    if (!full_fleet) {
        vector<IntVector> by_taxi(instance.taxi_count);
        for (size_t j = 0; j < columns.size(); ++j)
            by_taxi[columns[j].taxi].push_back(j);
        vector<unsigned char> keep(columns.size(), 0);
        for (int j : initial)
            if (j >= 0)
                keep[j] = 1;
        auto reduced_score = [&](int j) {
            Real value = columns[j].profit;
            if (fast_request_prices.size() == size_t(instance.request_count))
                for (int r : columns[j].requests)
                    value -= fast_request_prices[r];
            return value;
        };
        for (auto &ids : by_taxi) {
            auto raw = [&](int a, int b) {
                return columns[a].profit != columns[b].profit
                           ? columns[a].profit > columns[b].profit
                           : a < b;
            };
            sort(ids.begin(), ids.end(), raw);
            for (int i = 0; i < min<int>(16, ids.size()); ++i)
                keep[ids[i]] = 1;
            stable_sort(ids.begin(), ids.end(), [&](int a, int b) {
                Real x = reduced_score(a), y = reduced_score(b);
                return x != y ? x > y : a < b;
            });
            for (int i = 0; i < min<int>(32, ids.size()); ++i)
                keep[ids[i]] = 1;
            // Coverage alternatives add columns serving requests absent from
            // the primary active routes,without deleting them from the bank.
            IntVector coverage(instance.request_count, 0);
            for (int j : ids)
                if (keep[j])
                    for (int r : columns[j].requests)
                        ++coverage[r];
            vector<pair<Real, int>> alternatives;
            for (int j : ids)
                if (!keep[j]) {
                    Real score = 0;
                    for (int r : columns[j].requests)
                        score += 1.0 / (1 + coverage[r]);
                    if (columns[j].requests.size())
                        score /= columns[j].requests.size();
                    alternatives.push_back({score, j});
                }
            stable_sort(alternatives.begin(), alternatives.end(), [](auto &a, auto &b) {
                return a.first != b.first ? a.first > b.first : a.second < b.second;
            });
            for (int i = 0; i < min<int>(16, alternatives.size()); ++i)
                keep[alternatives[i].second] = 1;
        }
        vector<MasterColumn> active;
        vector<const StoredRoute *> active_routes;
        IntVector remap(columns.size(), -1);
        for (size_t j = 0; j < columns.size(); ++j)
            if (keep[j]) {
                remap[j] = active.size();
                active.push_back(columns[j]);
                active_routes.push_back(routes[j]);
            }
        for (int &j : initial)
            if (j >= 0)
                j = remap[j];
        columns.swap(active);
        routes.swap(active_routes);
    }
    // Choose a master strategy by fleet size; larger fleets use request opportunity costs.
    auto prices = instance.taxi_count > GROUP_MASTER_TAXI_LIMIT
                      ? estimate_request_prices(state.profit, deadline)
                      : RealVector{};
    if (Clock::now() >= deadline)
        return;
    MasterResult solved;
    master_memory_limited = false;
    if (full_fleet && !full_master_fits(columns)) {
        solved = solve_taxi_groups(columns, instance.taxi_count, instance.request_count, initial,
                                   deadline, random_seed, prices, bank_upper);
    } else if (full_fleet) {
        solved = solve_route_master(columns, instance.taxi_count, instance.request_count, initial,
                                    deadline, true);
    } else if (instance.taxi_count > GROUP_MASTER_TAXI_LIMIT) {
        solved = solve_taxi_groups(columns, instance.taxi_count, instance.request_count, initial,
                                   deadline, random_seed, prices);
    } else if (instance.taxi_count > SMALL_FLEET_TAXI_LIMIT) {
        solved = solve_ranked_master(columns, instance.taxi_count, instance.request_count, initial,
                                     deadline);
    } else {
        solved = solve_route_master(columns, instance.taxi_count, instance.request_count, initial,
                                    deadline);
    }
    // A later cut or branch can also exceed the workspace budget. Continue
    // with overlapping small masters, carrying forward the best feasible fleet.
    if (master_memory_limited && Clock::now() < deadline) {
        master_memory_limited = false;
        solved =
            solve_taxi_groups(columns, instance.taxi_count, instance.request_count,
                              solved.selected_columns, deadline, random_seed, prices, bank_upper);
    }
    // Reconstruct the selected fleet and accept it only when actual profit improves.
    Fleet selected_fleet(instance.taxi_count);
    for (int taxi = 0; taxi < instance.taxi_count; taxi++) {
        if (solved.selected_columns[taxi] >= 0) {
            selected_fleet[taxi] =
                unpack_stored_route(routes[solved.selected_columns[taxi]]->route);
        }
    }
    if (fleet_profit(selected_fleet) > state.profit + FEASIBILITY_EPSILON) {
        state = make_search_state(selected_fleet);
    }
}

// Summing three unit packing rows and rounding down yields a valid unit row
// on all columns touching at least two of these resources.
static void strengthen_packing(PackingMatrix &matrix, PackingLP &root, Deadline deadline) {
    const size_t resources = matrix.dimension;
    set<array<size_t, 3>> installed;
    for (int round = 0; round < 6 && Clock::now() < deadline; round++) {
        auto x = primal_values(root);
        vector<vector<Real>> pair_weight(resources, RealVector(resources, 0));
        map<array<size_t, 3>, Real> triple_weight;
        for (size_t j = 0; j < x.size(); j++)
            if (x[j] > 1e-8) {
                IndexVector rows;
                for (size_t r : matrix.column_rows[j])
                    if (r < resources)
                        rows.push_back(r);
                sort(rows.begin(), rows.end());
                for (size_t a = 0; a < rows.size(); a++)
                    for (size_t b = a + 1; b < rows.size(); b++) {
                        pair_weight[rows[a]][rows[b]] += x[j];
                        for (size_t c = b + 1; c < rows.size(); c++)
                            triple_weight[{rows[a], rows[b], rows[c]}] += x[j];
                    }
            }
        vector<pair<Real, array<size_t, 3>>> candidates;
        for (size_t a = 0; a < resources; a++)
            for (size_t b = a + 1; b < resources; b++)
                if (pair_weight[a][b] > 1e-8)
                    for (size_t c = b + 1; c < resources; c++)
                        if (pair_weight[a][c] > 1e-8 && pair_weight[b][c] > 1e-8) {
                            array<size_t, 3> key{a, b, c};
                            if (installed.count(key))
                                continue;
                            auto it = triple_weight.find(key);
                            Real triple = it == triple_weight.end() ? 0 : it->second;
                            Real weight = pair_weight[a][b] + pair_weight[a][c] +
                                          pair_weight[b][c] - 2 * triple;
                            if (weight > 1 + 1e-6)
                                candidates.emplace_back(weight, key);
                        }
        if (candidates.empty())
            break;
        sort(candidates.begin(), candidates.end(), [](auto &a, auto &b) {
            return a.first != b.first ? a.first > b.first : a.second < b.second;
        });
        if (candidates.size() > 24)
            candidates.resize(24);
        for (auto &entry : candidates) {
            auto key = entry.second;
            installed.insert(key);
            size_t row = matrix.dimension++;
            matrix.row_resources.push_back(matrix.taxi_count + instance.request_count +
                                           matrix.dimension);
            for (auto &rows : matrix.column_rows) {
                int count = 0;
                for (size_t r : rows)
                    count += (r == key[0] || r == key[1] || r == key[2]);
                if (count >= 2)
                    rows.push_back(row);
            }
        }
        root = initialize_lp(matrix);
        primal_simplex(root, deadline);
        if (root.timed_out)
            break;
    }
}

// --- Route selection: branch and bound ---
// Branch on a fractional route; greedily round the LP to update the incumbent.
MasterResult solve_route_master(const vector<MasterColumn> &columns, size_t taxi_count,
                                size_t request_count, const IntVector &initial_columns,
                                Deadline deadline, bool use_root_fixing) {
    // Start with the supplied feasible fleet so a timeout still returns an incumbent.
    MasterResult result;
    result.selected_columns = initial_columns;
    for (auto column_id : initial_columns) {
        if (column_id >= 0) {
            result.profit += columns[column_id].profit;
        }
    }
    if (columns.empty()) {
        return result;
    }
    // Solve the root LP relaxation before exploring include/exclude branches.
    if (!full_master_fits(columns)) {
        master_memory_limited = true;
        result.timed_out = true;
        return result;
    }
    auto matrix = build_packing_matrix(columns, taxi_count, request_count);
    auto root = initialize_lp(matrix);
    primal_simplex(root, deadline);
    if (root.timed_out) {
        result.timed_out = true;
        return result;
    }
    if (use_root_fixing) {
        strengthen_packing(matrix, root, deadline);
        if (root.timed_out) {
            result.timed_out = true;
            return result;
        }
    }
    // Integer route profits permit rounding the upper bound down before pruning.
    bool integral = true;
    for (auto &column : columns) {
        integral &= column.profit == round(column.profit);
    }

    if (use_root_fixing && Clock::now() < deadline) {
        // Establish a feasible incumbent from the root solution before exact bound fixing.
        auto values = primal_values(root);
        IndexVector order(columns.size());
        iota(order.begin(), order.end(), 0);
        stable_sort(order.begin(), order.end(), [&](size_t a, size_t b) {
            return values[a] != values[b] ? values[a] > values[b]
                                          : columns[a].profit > columns[b].profit;
        });
        vector<char> used(request_count), assigned(taxi_count);
        IntVector chosen(taxi_count, -1);
        Real rounded = 0;
        for (size_t j : order) {
            auto &c = columns[j];
            if (assigned[c.taxi])
                continue;
            bool fits = true;
            for (size_t r : c.requests)
                if (used[r]) {
                    fits = false;
                    break;
                }
            if (!fits)
                continue;
            assigned[c.taxi] = 1;
            chosen[c.taxi] = j;
            rounded += c.profit;
            for (size_t r : c.requests)
                used[r] = 1;
        }
        if (rounded > result.profit + IMPROVEMENT_EPSILON) {
            result.profit = rounded;
            result.selected_columns = move(chosen);
        }
        // Any nonnegative row prices can be repaired into a feasible dual by taxi.
        // Outward rounding keeps every conditional upper bound conservative.
        const Real infinity = numeric_limits<Real>::infinity();
        RealVector prices(matrix.dimension), residual(taxi_count, 0);
        IntVector taxi_row(taxi_count, -1);
        bool usable = true;
        for (size_t r = 0; r < matrix.dimension; r++) {
            if (!isfinite(root.dual_prices[r])) {
                usable = false;
                break;
            }
            prices[r] = max(0.0, root.dual_prices[r]);
            if (matrix.row_resources[r] < taxi_count)
                taxi_row[matrix.row_resources[r]] = r;
        }
        if (usable) {
            for (size_t j = 0; j < columns.size(); j++) {
                Real upper = columns[j].profit;
                for (size_t r : matrix.column_rows[j])
                    upper = nextafter(upper - prices[r], infinity);
                residual[columns[j].taxi] = max(residual[columns[j].taxi], upper);
            }
            for (size_t t = 0; t < taxi_count; t++)
                if (taxi_row[t] >= 0) {
                    size_t r = taxi_row[t];
                    prices[r] = nextafter(prices[r] + max(0.0, residual[t]), infinity);
                }
            Real upper = 0;
            for (Real price : prices)
                upper = nextafter(upper + price, infinity);
            if (isfinite(upper)) {
                vector<char> protected_column(columns.size());
                for (int j : result.selected_columns)
                    if (j >= 0)
                        protected_column[j] = 1;
                IntVector retained;
                retained.reserve(columns.size());
                for (size_t j = 0; j < columns.size(); j++) {
                    Real conditional = nextafter(upper + columns[j].profit, infinity);
                    for (size_t r : matrix.column_rows[j])
                        conditional = nextafter(conditional - prices[r], infinity);
                    bool may_improve =
                        integral ? conditional >= result.profit + 1 : conditional > result.profit;
                    if (protected_column[j] || may_improve)
                        retained.push_back(j);
                }
                // A reduction is optional; ignoring a small reduction preserves the full problem.
                if (retained.size() * 5 < columns.size() * 4 && Clock::now() < deadline) {
                    vector<MasterColumn> reduced;
                    reduced.reserve(retained.size());
                    IntVector remap(columns.size(), -1);
                    for (int j : retained) {
                        remap[j] = reduced.size();
                        reduced.push_back(columns[j]);
                    }
                    IntVector reduced_initial(taxi_count, -1);
                    for (size_t t = 0; t < taxi_count; t++)
                        if (result.selected_columns[t] >= 0)
                            reduced_initial[t] = remap[result.selected_columns[t]];
                    root = PackingLP{};
                    matrix = PackingMatrix{};
                    RealVector{}.swap(values);
                    IndexVector{}.swap(order);
                    IntVector{}.swap(remap);
                    auto solved = solve_route_master(reduced, taxi_count, request_count,
                                                     reduced_initial, deadline, true);
                    for (int &j : solved.selected_columns)
                        if (j >= 0)
                            j = retained[j];
                    return solved;
                }
            }
        }
    }

    vector<unsigned char> fixed(taxi_count, 0), occupied(request_count, 0),
        forbidden(columns.size(), 0);
    IntVector selected_columns(taxi_count, -1);
    // Track conflicts with decisions fixed along the current branch-and-bound path.
    auto compatible = [&](size_t column_id) {
        auto &column = columns[column_id];
        if (fixed[column.taxi] || forbidden[column_id]) {
            return false;
        }
        for (auto request : column.requests) {
            if (occupied[request]) {
                return false;
            }
        }
        return true;
    };
    function<void(const PackingLP &, Real)> visit = [&](const PackingLP &lp, Real profit) {
        if (lp.backend && lp.backend->infeasible)
            return;
        if (result.timed_out) {
            return;
        }
        if (Clock::now() >= deadline) {
            result.timed_out = true;
            return;
        }
        // Prune this subtree if its LP upper bound cannot beat the incumbent.
        Real upper_bound = ::dual_upper_bound(lp);
        if (integral) {
            upper_bound = floor(upper_bound);
        }
        if (upper_bound <= result.profit + IMPROVEMENT_EPSILON) {
            return;
        }
        // Rank compatible routes by LP value, breaking ties by route profit.
        auto lp_values = primal_values(lp);
        IndexVector ranking;
        for (size_t j = 0; j < columns.size(); ++j) {
            if (compatible(j)) {
                ranking.push_back(j);
            }
        }
        if (ranking.empty()) {
            if (profit > result.profit + IMPROVEMENT_EPSILON) {
                result.profit = profit;
                result.selected_columns = selected_columns;
            }
            return;
        }
        stable_sort(ranking.begin(), ranking.end(), [&](auto left, auto right) {
            return lp_values[left] != lp_values[right]
                       ? lp_values[left] > lp_values[right]
                       : columns[left].profit > columns[right].profit;
        });
        // Greedily round the ranked columns to obtain a feasible complete fleet.
        auto used_requests = occupied, assigned_taxis = fixed;
        auto candidate_columns = selected_columns;
        Real candidate = profit;
        for (auto column_id : ranking) {
            auto &column = columns[column_id];
            if (assigned_taxis[column.taxi]) {
                continue;
            }
            bool available = true;
            for (auto request : column.requests) {
                available &= !used_requests[request];
            }
            if (!available) {
                continue;
            }
            assigned_taxis[column.taxi] = 1;
            for (auto request : column.requests) {
                used_requests[request] = 1;
            }
            candidate_columns[column.taxi] = int(column_id);
            candidate += column.profit;
        }
        if (candidate > result.profit + IMPROVEMENT_EPSILON) {
            result.profit = candidate;
            result.selected_columns = move(candidate_columns);
        }
        if (upper_bound <= result.profit + IMPROVEMENT_EPSILON) {
            return;
        }
        // Prefer the most fractional route; fall back to the first compatible column.
        size_t branch_column = ranking.front();
        Real fractionality = -1;
        for (auto j : ranking) {
            if (lp_values[j] <= LP_FEASIBILITY_EPSILON ||
                lp_values[j] >= 1 - LP_FEASIBILITY_EPSILON) {
                continue;
            }
            auto score = min(lp_values[j], 1 - lp_values[j]);
            if (score > fractionality) {
                fractionality = score;
                branch_column = j;
            }
        }
        auto &column = columns[branch_column];

        // Include this route, repair the LP by dual simplex, recurse, then undo conflicts.
        {
            if (!lp_copy_fits(lp)) {
                master_memory_limited = true;
                result.timed_out = true;
                return;
            }
            auto child = lp;
            add_column_bound(child, branch_column, true);
            dual_simplex(child, deadline);
            if (child.timed_out) {
                result.timed_out = true;
                return;
            }
            fixed[column.taxi] = 1;
            for (auto request : column.requests) {
                occupied[request] = 1;
            }
            selected_columns[column.taxi] = int(branch_column);
            visit(child, profit + column.profit);
            selected_columns[column.taxi] = -1;
            fixed[column.taxi] = 0;
            for (auto request : column.requests) {
                occupied[request] = 0;
            }
        }
        // Explore the sibling branch with this route excluded, unless time has expired.
        if (!result.timed_out) {
            if (!lp_copy_fits(lp)) {
                master_memory_limited = true;
                result.timed_out = true;
                return;
            }
            auto child = lp;
            add_column_bound(child, branch_column, false);
            dual_simplex(child, deadline);
            if (child.timed_out) {
                result.timed_out = true;
                return;
            }
            forbidden[branch_column] = 1;
            visit(child, profit);
            forbidden[branch_column] = 0;
        }
    };
    visit(root, 0);
    return result;
}

// --- Route selection: taxi groups ---
// Optimize overlapping taxi groups while keeping all outside routes fixed.
MasterResult solve_taxi_groups(const vector<MasterColumn> &columns, size_t taxi_count,
                               size_t request_count, const IntVector &initial_columns,
                               Deadline deadline, UInt64 group_seed,
                               const RealVector &request_prices, Real bank_upper) {
    // Index routes by taxi and precompute each route's request-price penalty.
    MasterResult result;
    result.selected_columns = initial_columns;
    for (auto column_id : initial_columns) {
        if (column_id >= 0) {
            result.profit += columns[column_id].profit;
        }
    }
    bool integral = true;
    for (const auto &column : columns)
        integral &= column.profit == round(column.profit);
    auto reached_upper = [&] {
        return isfinite(bank_upper) &&
               (integral ? bank_upper < floor(result.profit) + 1 : bank_upper <= result.profit);
    };
    vector<IndexVector> taxi_columns(taxi_count);
    for (size_t j = 0; j < columns.size(); ++j) {
        taxi_columns[columns[j].taxi].push_back(j);
    }
    RealVector penalties(columns.size(), 0);
    if (!request_prices.empty()) {
        for (size_t j = 0; j < columns.size(); ++j) {
            for (auto request : columns[j].requests) {
                penalties[j] += request_prices[request];
            }
        }
    }
    // Try several greedy fleet constructions before coordinated group improvement.
    for (Real scale : {0.0, 0.5, 1.0, 1.5}) {
        IndexVector ranking(columns.size());
        iota(ranking.begin(), ranking.end(), 0);
        sort(ranking.begin(), ranking.end(), [&](auto left, auto right) {
            auto left_score = columns[left].profit - scale * penalties[left],
                 right_score = columns[right].profit - scale * penalties[right];
            return left_score != right_score ? left_score > right_score : left < right;
        });
        vector<unsigned char> occupied(request_count, 0);
        IntVector fleet(taxi_count, -1);
        Real profit = 0;
        for (auto j : ranking) {
            auto &column = columns[j];
            if (fleet[column.taxi] >= 0) {
                continue;
            }
            bool available = true;
            for (auto request : column.requests) {
                available &= !occupied[request];
            }
            if (!available) {
                continue;
            }
            fleet[column.taxi] = int(j);
            for (auto request : column.requests) {
                occupied[request] = 1;
            }
            profit += column.profit;
        }
        if (profit > result.profit + IMPROVEMENT_EPSILON) {
            result.profit = profit;
            result.selected_columns = move(fleet);
        }
    }
    // Order each taxi's bank by priced profit for candidate selection below.
    for (auto &taxi_bank : taxi_columns) {
        sort(taxi_bank.begin(), taxi_bank.end(), [&](auto left, auto right) {
            auto left_score = columns[left].profit - penalties[left],
                 right_score = columns[right].profit - penalties[right];
            return left_score != right_score ? left_score > right_score : left < right;
        });
    }
    mt19937_64 group_random_engine(group_seed);
    size_t rounds = 0, last_gain = 0;
    while (Clock::now() < deadline && !reached_upper()) {
        // Rebuild request ownership from the latest incumbent fleet.
        IntVector request_owner(request_count, -1);
        for (size_t taxi = 0; taxi < taxi_count; ++taxi) {
            if (result.selected_columns[taxi] >= 0) {
                for (auto request : columns[result.selected_columns[taxi]].requests) {
                    request_owner[request] = int(taxi);
                }
            }
        }
        // Seed a rotating taxi group with owners blocking one of its attractive routes.
        size_t width =
            min(taxi_count, size_t(GROUP_MIN_TAXIS + GROUP_TAXI_INCREMENT * ((rounds / taxi_count) %
                                                                             GROUP_WIDTH_LEVELS)));
        IndexVector group;
        vector<unsigned char> inside(taxi_count, 0);
        auto add = [&](size_t taxi) {
            if (!inside[taxi]) {
                group.push_back(taxi);
                inside[taxi] = 1;
            }
        };
        auto first = rounds % taxi_count;
        add(first);
        if (!taxi_columns[first].empty()) {
            auto rank = rounds % 3 == 0
                            ? group_random_engine() % min<size_t>(GROUP_RANDOM_SEED_CANDIDATES,
                                                                  taxi_columns[first].size())
                            : 0;
            for (auto request : columns[taxi_columns[first][rank]].requests) {
                if (request_owner[request] >= 0 && group.size() < width) {
                    add(request_owner[request]);
                }
            }
        }
        // Grow the group toward conflicting owners; occasional random choices diversify it.
        while (group.size() < width) {
            RealVector score(taxi_count, 0);
            for (auto taxi : group) {
                auto previous_profit = result.selected_columns[taxi] >= 0
                                           ? columns[result.selected_columns[taxi]].profit
                                           : 0;
                auto inspected_columns =
                    min<size_t>(GROUP_OVERLAP_CANDIDATES, taxi_columns[taxi].size());
                for (size_t j = 0; j < inspected_columns; ++j) {
                    auto &column = columns[taxi_columns[taxi][j]];
                    auto overlap_value = max(1.0, column.profit - previous_profit);
                    for (auto request : column.requests) {
                        if (request_owner[request] >= 0 && !inside[request_owner[request]]) {
                            score[request_owner[request]] += overlap_value;
                        }
                    }
                }
            }
            size_t chosen = taxi_count;
            Real maximum = 0;
            for (size_t taxi = 0; taxi < taxi_count; ++taxi) {
                if (!inside[taxi] && score[taxi] > maximum) {
                    maximum = score[taxi];
                    chosen = taxi;
                }
            }
            if (chosen == taxi_count || rounds % 5 == 4) {
                do {
                    chosen = group_random_engine() % taxi_count;
                } while (inside[chosen]);
            }
            add(chosen);
        }
        // Keep bounded candidate banks that avoid requests owned outside this group.
        vector<MasterColumn> local_columns;
        IndexVector global_columns;
        IntVector initial(group.size(), -1);
        for (size_t taxi = 0; taxi < group.size(); ++taxi) {
            size_t candidate_count = 0;
            for (auto column_id : taxi_columns[group[taxi]]) {
                auto &column = columns[column_id];
                bool available = true;
                for (auto request : column.requests) {
                    if (request_owner[request] >= 0 && !inside[request_owner[request]]) {
                        available = false;
                        break;
                    }
                }
                if (!available) {
                    continue;
                }
                // Keep the best 64 available candidates and always retain the incumbent.
                if (candidate_count++ >= GROUP_ROUTE_CANDIDATES &&
                    result.selected_columns[group[taxi]] != int(column_id)) {
                    continue;
                }
                if (result.selected_columns[group[taxi]] == int(column_id)) {
                    initial[taxi] = int(local_columns.size());
                }
                local_columns.push_back({taxi, column.requests, column.profit});
                global_columns.push_back(column_id);
            }
        }
        // Solve the small group master within a short slice of the remaining budget.
        auto current_time = Clock::now();
        auto local_deadline = min(deadline, current_time + Milliseconds(GROUP_SOLVE_MILLISECONDS));
        auto solved =
            solve_route_master(local_columns, group.size(), request_count, initial, local_deadline);
        Real previous_profit = 0;
        for (auto column_id : initial) {
            if (column_id >= 0) {
                previous_profit += local_columns[column_id].profit;
            }
        }
        // Map improved local choices back to global columns; all outside routes stay fixed.
        if (solved.profit > previous_profit + IMPROVEMENT_EPSILON) {
            for (size_t taxi = 0; taxi < group.size(); ++taxi) {
                result.selected_columns[group[taxi]] =
                    solved.selected_columns[taxi] >= 0
                        ? int(global_columns[solved.selected_columns[taxi]])
                        : -1;
            }
            result.profit += solved.profit - previous_profit;
            last_gain = rounds;
        }
        ++rounds;
        if (rounds - last_gain > GROUP_STAGNATION_ROUNDS_PER_TAXI * taxi_count) {
            break;
        }
    }
    result.timed_out = Clock::now() >= deadline;
    return result;
}

// --- Route selection: ranked subsets ---
// Solve progressively wider price-ranked route subsets, then refine taxi groups.
MasterResult solve_ranked_master(const vector<MasterColumn> &columns, int taxi_count,
                                 int request_count, const IntVector &initial_columns,
                                 Deadline deadline) {
    // Estimate opportunity costs from the full LP and rank each taxi's routes.
    MasterResult best;
    best.selected_columns = initial_columns;
    for (int j : initial_columns) {
        if (j >= 0) {
            best.profit += columns[j].profit;
        }
    }
    RealVector prices;
    auto matrix = build_packing_matrix(columns, taxi_count, request_count);
    auto lp = initialize_lp(matrix);
    primal_simplex(lp, min(deadline, deadline_after(RANKED_LP_SECONDS)));
    prices = request_prices_from_lp(lp, request_count);
    vector<IntVector> taxi_columns(taxi_count);
    RealVector score(columns.size());
    for (int j = 0; j < (int)columns.size(); j++) {
        auto &column = columns[j];
        taxi_columns[column.taxi].push_back(j);
        score[j] = column.profit;
        for (int request : column.requests) {
            score[j] -= prices[request];
        }
    }
    for (auto &taxi_bank : taxi_columns) {
        sort(taxi_bank.begin(), taxi_bank.end(), [&](int left, int right) {
            return score[left] != score[right] ? score[left] > score[right] : left < right;
        });
    }
    // Widen the candidate subsets gradually, always retaining the current incumbent.
    for (int width : {32, 64, 128}) {
        if (Clock::now() >= deadline) {
            break;
        }
        vector<MasterColumn> subset_columns;
        IntVector global_columns, initial(taxi_count, -1);
        for (int taxi = 0; taxi < taxi_count; taxi++) {
            auto add = [&](int j) {
                if (best.selected_columns[taxi] == j) {
                    initial[taxi] = subset_columns.size();
                }
                subset_columns.push_back(columns[j]);
                global_columns.push_back(j);
            };
            for (int i = 0; i < min<int>(width, taxi_columns[taxi].size()); i++) {
                add(taxi_columns[taxi][i]);
            }
            if (best.selected_columns[taxi] >= 0 && initial[taxi] < 0) {
                add(best.selected_columns[taxi]);
            }
        }
        auto solved = solve_route_master(
            subset_columns, taxi_count, request_count, initial,
            min(deadline, deadline_after(width == 32 ? RANKED_FIRST_SOLVE_SECONDS
                                                     : RANKED_WIDE_SOLVE_SECONDS)));
        if (solved.profit > best.profit + FEASIBILITY_EPSILON) {
            best.profit = solved.profit;
            for (int taxi = 0; taxi < taxi_count; taxi++) {
                best.selected_columns[taxi] = solved.selected_columns[taxi] >= 0
                                                  ? global_columns[solved.selected_columns[taxi]]
                                                  : -1;
            }
        }
    }
    // Spend any remaining time on coordinated changes among taxi groups.
    if (Clock::now() + Milliseconds(RANKED_GROUP_GUARD_MILLISECONDS) < deadline) {
        auto grouped = solve_taxi_groups(columns, taxi_count, request_count, best.selected_columns,
                                         deadline, random_engine(), prices);
        if (grouped.profit > best.profit + FEASIBILITY_EPSILON) {
            best.profit = grouped.profit;
            best.selected_columns = move(grouped.selected_columns);
        }
    }
    return best;
}

// --- Route selection: request prices ---
// Subgradient request prices penalize collisions between independently chosen taxi routes.
// A feasible fleet profit supplies the lower bound used by the step size.
RealVector estimate_request_prices(Real incumbent_profit, Deadline deadline) {
    // Collect bank columns and build a feasible greedy profit bound for step-size control.
    RealVector prices(instance.request_count), best_prices(instance.request_count);
    // A partial subgradient sweep is not a valid evaluated dual vector.
    // On timeout retain the best fully completed iteration (or zero prices).
    if (Clock::now() >= deadline)
        return best_prices;
    Real lower = 0, best = INF;
    size_t scanned = 0;
    vector<MasterColumn> columns;
    for (int taxi = 0; taxi < instance.taxi_count; taxi++) {
        for (auto &[ids, column] : route_bank[taxi]) {
            if ((++scanned & 1023) == 0 && Clock::now() >= deadline)
                return best_prices;
            columns.push_back({(size_t)taxi, ids, column.profit});
        }
    }
    if (Clock::now() >= deadline)
        return best_prices;
    IntVector ranking(columns.size());
    iota(ranking.begin(), ranking.end(), 0);
    sort(ranking.begin(), ranking.end(),
         [&](int left, int right) { return columns[left].profit > columns[right].profit; });
    if (Clock::now() >= deadline)
        return best_prices;
    vector<char> used_requests(instance.request_count), assigned_taxis(instance.taxi_count);
    for (int j : ranking) {
        if ((++scanned & 1023) == 0 && Clock::now() >= deadline)
            return best_prices;
        auto &column = columns[j];
        if (assigned_taxis[column.taxi]) {
            continue;
        }
        bool available = true;
        for (int request : column.requests) {
            available &= !used_requests[request];
        }
        if (available) {
            lower += column.profit;
            assigned_taxis[column.taxi] = 1;
            for (int request : column.requests) {
                used_requests[request] = 1;
            }
        }
    }
    lower = max(lower, incumbent_profit);
    for (int iteration = 0; iteration < PRICE_ITERATION_LIMIT; iteration++) {
        if (Clock::now() >= deadline)
            return best_prices;
        // Relax shared-request conflicts: let each taxi choose its best priced route independently.
        RealVector best_taxi_score(instance.taxi_count);
        IntVector chosen(instance.taxi_count, -1), request_usage(instance.request_count);
        for (int j = 0; j < (int)columns.size(); j++) {
            if ((j & 1023) == 0 && Clock::now() >= deadline)
                return best_prices;
            auto &column = columns[j];
            Real reduced_profit = column.profit;
            for (int request : column.requests) {
                reduced_profit -= prices[request];
            }
            if (reduced_profit > best_taxi_score[column.taxi]) {
                best_taxi_score[column.taxi] = reduced_profit, chosen[column.taxi] = j;
            }
        }
        // Retain the price vector with the smallest dual bound seen so far.
        Real dual_value = accumulate(prices.begin(), prices.end(), 0.0) +
                          accumulate(best_taxi_score.begin(), best_taxi_score.end(), 0.0);
        if (dual_value < best) {
            best = dual_value, best_prices = prices;
        }
        // Count request collisions and compute the squared projected subgradient norm.
        for (int j : chosen) {
            if (j >= 0) {
                for (int request : columns[j].requests) {
                    request_usage[request]++;
                }
            }
        }
        Real squared_norm = 0;
        for (int request = 0; request < instance.request_count; request++) {
            if (prices[request] > 0 || request_usage[request] > 1) {
                squared_norm += (request_usage[request] - 1) * (request_usage[request] - 1);
            }
        }
        if (squared_norm == 0 || dual_value <= lower + LP_FEASIBILITY_EPSILON) {
            break;
        }
        // Adjust prices by excess request usage, keeping every price nonnegative.
        Real step = PRICE_STEP_FACTOR * (dual_value - lower) / squared_norm;
        for (int request = 0; request < instance.request_count; request++) {
            prices[request] = max(0.0, prices[request] + step * (request_usage[request] - 1));
        }
    }
    return best_prices;
}

// --- Local search: parcel delivery refinement ---
// Evaluate edits with constant-many cached segments. Build a route only when
// applying a move; each taxi caches one edit rather than a pool of routes.
struct ParcelRouteEdit {
    enum Kind { NONE, DELIVERY, INSERT, JOINT } kind = NONE;
    Real gain = FEASIBILITY_EPSILON;
    int first_added = -1, second_added = -1;
    int pickup_position = 0, delivery_position = 0, old_delivery = 0;
    array<int, 4> nodes{};
};

struct ParcelRouteSummary {
    Segment constraints;
    Real cost = 0;
};

static ParcelRouteSummary join_parcel_summaries(ParcelRouteSummary left,
                                                const ParcelRouteSummary &right) {
    if (!left.constraints.first_node)
        return right;
    if (!right.constraints.first_node)
        return left;
    left.cost += travel_cost(left.constraints.last_node, right.constraints.first_node) + right.cost;
    left.constraints = join_segments(left.constraints, right.constraints);
    return left;
}

static ParcelRouteEdit find_parcel_route_edit(const SearchState &state, int taxi,
                                              Deadline deadline) {
    const Route &route = state.routes[taxi];
    const int length = route.size();
    ParcelRouteEdit best;
    if (Clock::now() >= deadline)
        return best;

    // Preprocess the unchanged route. Prefix costs and disjoint-table summaries
    // then provide O(1) cost, time-window and load information for any piece.
    const auto &segments = cached_route_segments(taxi, route);
    static RealVector prefix_cost;
    prefix_cost.assign(length + 1, 0);
    for (int i = 1; i < length; ++i)
        prefix_cost[i + 1] = prefix_cost[i] + travel_cost(route[i - 1], route[i]);
    static IntVector pickup_positions;
    pickup_positions.assign(instance.request_count, -1);
    for (int i = 0; i < length; ++i)
        if (!instance.is_delivery[route[i]])
            pickup_positions[instance.node_request[route[i]]] = i;
    const Real old_cost =
        length ? prefix_cost[length] + travel_cost(instance.taxi_origin[taxi], route.front()) : 0;
    auto piece = [&](int begin, int end) {
        return ParcelRouteSummary{segment_range(segments, begin, end),
                                  begin < end ? prefix_cost[end] - prefix_cost[begin + 1] : 0};
    };
    auto consider = [&](const ParcelRouteSummary &summary, Real added_revenue,
                        ParcelRouteEdit edit) {
        const Real cost =
            summary.cost + travel_cost(instance.taxi_origin[taxi], summary.constraints.first_node);
        const Real gain = added_revenue + old_cost - cost;
        if (gain > best.gain && is_segment_feasible(taxi, summary.constraints)) {
            edit.gain = gain;
            best = edit;
        }
    };

    // Reposition an existing parcel delivery, retaining all other stop order.
    // Three unchanged pieces and one delivery stop describe the entire edit.
    for (int delivery = 0; delivery < length; ++delivery) {
        const int node = route[delivery], request = instance.node_request[node];
        if (!instance.is_delivery[node] || instance.requests[request].is_passenger)
            continue;
        const int pickup = pickup_positions[request];
        const ParcelRouteSummary stop{segments.node_segments[delivery], 0};
        for (int position = int(pickup) + 1; position < length; ++position) {
            const int previous = position - 1 < delivery ? position - 1 : position;
            const int previous_node = route[previous];
            if (!instance.is_delivery[previous_node] &&
                instance.requests[instance.node_request[previous_node]].is_passenger)
                continue;
            ParcelRouteSummary summary;
            if (position <= delivery) {
                summary = join_parcel_summaries(
                    join_parcel_summaries(join_parcel_summaries(piece(0, position), stop),
                                          piece(position, delivery)),
                    piece(delivery + 1, length));
            } else {
                summary = join_parcel_summaries(
                    join_parcel_summaries(join_parcel_summaries(piece(0, delivery),
                                                                piece(delivery + 1, position + 1)),
                                          stop),
                    piece(position + 1, length));
            }
            ParcelRouteEdit edit;
            edit.kind = ParcelRouteEdit::DELIVERY;
            edit.old_delivery = delivery;
            edit.delivery_position = position;
            consider(summary, 0, edit);
        }
    }

    static IntVector unserved;
    unserved.clear();
    unserved.reserve(instance.parcel_count);
    for (int request = instance.passenger_count; request < instance.request_count; ++request) {
        if (state.request_owner[request] < 0 &&
            instance.requests[request].weight <= instance.taxi_capacity[taxi] + FEASIBILITY_EPSILON)
            unserved.push_back(request);
    }

    // Existing single-insertion checks also use O(1) segment composition per
    // placement. Keep its positions without constructing a trial route.
    for (int request : unserved) {
        const auto choice = find_route_insertion(request, taxi, route, best.gain);
        if (choice.taxi >= 0 && choice.profit_delta > best.gain) {
            best.kind = ParcelRouteEdit::INSERT;
            best.gain = choice.profit_delta;
            best.first_added = request;
            best.second_added = -1;
            best.pickup_position = choice.pickup_position;
            best.delivery_position = choice.delivery_position;
        }
    }

    // Joint insertion replaces one arc with four stops. Neither singleton has
    // to be profitable or feasible; only the complete move is tested.
    static constexpr int orders[6][4] = {{0, 1, 2, 3}, {0, 2, 1, 3}, {0, 2, 3, 1},
                                         {2, 0, 1, 3}, {2, 0, 3, 1}, {2, 3, 0, 1}};
    static vector<array<ParcelRouteSummary, 2>> stops;
    stops.clear();
    stops.reserve(unserved.size());
    for (int request : unserved)
        stops.push_back(
            {ParcelRouteSummary{make_node_segment(instance.requests[request].pickup_node), 0},
             ParcelRouteSummary{make_node_segment(instance.requests[request].delivery_node), 0}});
    for (size_t a = 0; a < unserved.size(); ++a) {
        for (size_t b = a + 1; b < unserved.size(); ++b) {
            const int first = unserved[a], second = unserved[b];
            const auto &ra = instance.requests[first], &rb = instance.requests[second];
            const ParcelRouteSummary nodes[4] = {stops[a][0], stops[a][1], stops[b][0],
                                                 stops[b][1]};
            for (const auto &order : orders) {
                ParcelRouteSummary chain;
                for (int i : order)
                    chain = join_parcel_summaries(chain, nodes[i]);
                if (!chain.constraints.feasible ||
                    chain.constraints.maximum_load >
                        instance.taxi_capacity[taxi] + FEASIBILITY_EPSILON)
                    continue;
                for (int gap = 0; gap <= length; ++gap) {
                    if (!is_insertion_boundary(route, gap))
                        continue;
                    const int previous = gap ? route[gap - 1] : instance.taxi_origin[taxi];
                    Real added_cost =
                        travel_cost(previous, chain.constraints.first_node) + chain.cost;
                    if (gap < length)
                        added_cost += travel_cost(chain.constraints.last_node, route[gap]) -
                                      travel_cost(previous, route[gap]);
                    if (ra.revenue + rb.revenue - added_cost <= best.gain - FEASIBILITY_EPSILON)
                        continue;
                    const auto summary = join_parcel_summaries(
                        join_parcel_summaries(piece(0, gap), chain), piece(gap, length));
                    ParcelRouteEdit edit;
                    edit.kind = ParcelRouteEdit::JOINT;
                    edit.first_added = first;
                    edit.second_added = second;
                    edit.pickup_position = gap;
                    for (int i = 0; i < 4; ++i)
                        edit.nodes[i] = nodes[order[i]].constraints.first_node;
                    consider(summary, ra.revenue + rb.revenue, edit);
                }
            }
        }
    }
    return best;
}

static Route apply_parcel_route_edit(const Route &route, const ParcelRouteEdit &edit) {
    if (edit.kind == ParcelRouteEdit::INSERT)
        return insert_request(route, edit.first_added, edit.pickup_position,
                              edit.delivery_position);
    Route result = route;
    if (edit.kind == ParcelRouteEdit::DELIVERY) {
        const int node = result[edit.old_delivery];
        result.erase(result.begin() + edit.old_delivery);
        result.insert(result.begin() + edit.delivery_position, node);
    } else if (edit.kind == ParcelRouteEdit::JOINT) {
        result.insert(result.begin() + edit.pickup_position, edit.nodes.begin(), edit.nodes.end());
    }
    return result;
}

static void refine_parcel_deliveries(SearchState &state, Deadline deadline) {
    if (!instance.parcel_count)
        return;
    vector<ParcelRouteEdit> edits(instance.taxi_count);
    vector<UInt64> versions(instance.taxi_count, 0);
    while (Clock::now() < deadline) {
        int chosen = -1;
        Real best_gain = FEASIBILITY_EPSILON;
        for (int taxi = 0; taxi < instance.taxi_count && Clock::now() < deadline; ++taxi) {
            auto &edit = edits[taxi];
            const bool unavailable =
                (edit.first_added >= 0 && state.request_owner[edit.first_added] >= 0) ||
                (edit.second_added >= 0 && state.request_owner[edit.second_added] >= 0);
            if (versions[taxi] != state.route_version[taxi] || unavailable) {
                edit = find_parcel_route_edit(state, taxi, deadline);
                versions[taxi] = state.route_version[taxi];
            }
            // Other taxis only shrink the unserved set. A cached best edit
            // remains best if its route and added requests are still available.
            if (edit.gain > best_gain) {
                best_gain = edit.gain;
                chosen = taxi;
            }
        }
        if (chosen < 0)
            break;
        Route candidate = apply_parcel_route_edit(state.routes[chosen], edits[chosen]);
        // Build and independently simulate only the chosen move: O(route length)
        // per application, outside the O(1) neighborhood evaluation loop.
        const auto value = evaluate_route(instance, chosen, candidate);
        if (value && *value > state.route_profit[chosen] + FEASIBILITY_EPSILON)
            update_route(state, chosen, candidate);
        else
            edits[chosen] = ParcelRouteEdit{};
        // This phase follows the last master solve; no trial routes enter an LP.
    }
}

// --- Local search: descent ---
// Descend through removal, insertion, relocation, tail exchange, swap, and replacement.
void improve_solution(SearchState &state, Deadline deadline, bool full_neighborhoods, int cycles) {
    static InsertionCache cache;
    for (int cycle = 0; cycle < cycles && Clock::now() < deadline; cycle++) {
        // Exhaust profitable removals and insertions before trying multi-route changes.
        bool changed = false;
        while (Clock::now() < deadline && improve_by_removal(state)) {
            changed = true;
        }
        while (Clock::now() < deadline && improve_by_insertion(state, cache)) {
            changed = true;
        }
        // Full descent also relocates requests; both modes exchange closed route tails.
        if (full_neighborhoods) {
            while (Clock::now() < deadline && improve_by_relocation(state, deadline)) {
                changed = true;
            }
        }
        while (Clock::now() < deadline && improve_by_tail_exchange(state, deadline)) {
            changed = true;
        }

        // Finish with optional swaps and cached one-for-one replacements.
        if (full_neighborhoods) {
            while (Clock::now() < deadline && improve_by_swap(state, deadline)) {
                changed = true;
            }
        }
        while (Clock::now() < deadline && improve_by_replacement(state, deadline)) {
            changed = true;
        }
        // Repeat the neighborhood sequence only if this cycle changed the fleet.
        if (!changed) {
            break;
        }
    }
    store_fleet(state.routes);
}

// --- Local search: insertion candidates ---
// Filter positions by cost bounds, then check time windows and capacity by segments.
InsertionChoice find_route_insertion(int request, int taxi, const Route &route,
                                     Real minimum_profit_delta) {
    // Reject an oversized request and prepare reusable cost-delta arrays.
    InsertionChoice best;
    auto &request_data = instance.requests[request];
    if (request_data.weight > instance.taxi_capacity[taxi] + FEASIBILITY_EPSILON) {
        return best;
    }
    int route_length = route.size();
    static RealVector pickup_cost_delta, delivery_cost_delta, adjacent_cost_delta,
        minimum_delivery_suffix;
    pickup_cost_delta.resize(route_length + 1);
    delivery_cost_delta.resize(route_length + 1);
    adjacent_cost_delta.resize(route_length + 1);
    minimum_delivery_suffix.assign(route_length + 2, INF);
    // Bound insertion cost before checking time windows: adjacent stops or split parcel stops.
    Real minimum_cost_delta = INF, minimum_pickup_delta = INF;
    for (int pickup_position = 0; pickup_position <= route_length; pickup_position++) {
        int previous_node =
            pickup_position ? route[pickup_position - 1] : instance.taxi_origin[taxi];
        Real replaced_arc_cost =
            pickup_position < route_length ? travel_cost(previous_node, route[pickup_position]) : 0;
        pickup_cost_delta[pickup_position] =
            travel_cost(previous_node, request_data.pickup_node) - replaced_arc_cost +
            (pickup_position < route_length
                 ? travel_cost(request_data.pickup_node, route[pickup_position])
                 : 0);
        delivery_cost_delta[pickup_position] =
            travel_cost(previous_node, request_data.delivery_node) - replaced_arc_cost +
            (pickup_position < route_length
                 ? travel_cost(request_data.delivery_node, route[pickup_position])
                 : 0);
        adjacent_cost_delta[pickup_position] =
            travel_cost(previous_node, request_data.pickup_node) +
            travel_cost(request_data.pickup_node, request_data.delivery_node) - replaced_arc_cost +
            (pickup_position < route_length
                 ? travel_cost(request_data.delivery_node, route[pickup_position])
                 : 0);
        minimum_cost_delta = min(minimum_cost_delta, adjacent_cost_delta[pickup_position]);
        if (!request_data.is_passenger) {
            minimum_cost_delta = min(minimum_cost_delta,
                                     minimum_pickup_delta + delivery_cost_delta[pickup_position]);
            minimum_pickup_delta = min(minimum_pickup_delta, pickup_cost_delta[pickup_position]);
        }
    }
    if (request_data.revenue - minimum_cost_delta + FEASIBILITY_EPSILON < minimum_profit_delta) {
        return best;
    }

    // Suffix minima bound the best delivery position after each possible pickup.
    for (int pickup_position = route_length; pickup_position >= 0; pickup_position--) {
        minimum_delivery_suffix[pickup_position] =
            min(delivery_cost_delta[pickup_position], minimum_delivery_suffix[pickup_position + 1]);
    }
    // Reuse route summaries to check insertions without rebuilding every candidate route.
    auto &segments = cached_route_segments(taxi, route);
    auto pickup = make_node_segment(request_data.pickup_node),
         delivery = make_node_segment(request_data.delivery_node);
    for (int pickup_position = 0; pickup_position <= route_length; pickup_position++) {
        if (is_insertion_boundary(route, pickup_position)) {
            Real required_profit_delta = max(minimum_profit_delta, best.profit_delta),
                 cost_lower_bound = request_data.is_passenger
                                        ? adjacent_cost_delta[pickup_position]
                                        : min(adjacent_cost_delta[pickup_position],
                                              pickup_cost_delta[pickup_position] +
                                                  minimum_delivery_suffix[pickup_position + 1]);
            if (request_data.revenue - cost_lower_bound + FEASIBILITY_EPSILON <
                required_profit_delta) {
                continue;
            }
            // A feasible pickup prefix is required before exploring delivery positions.
            auto prefix = join_segments(segment_range(segments, 0, pickup_position), pickup);
            if (!is_segment_feasible(taxi, prefix)) {
                continue;
            }
            for (int delivery_position = pickup_position;
                 delivery_position <= (request_data.is_passenger ? pickup_position : route_length);
                 delivery_position++) {
                if (is_insertion_boundary(route, delivery_position)) {
                    Real profit_delta =
                        request_data.revenue - (pickup_position == delivery_position
                                                    ? adjacent_cost_delta[pickup_position]
                                                    : pickup_cost_delta[pickup_position] +
                                                          delivery_cost_delta[delivery_position]);
                    if (profit_delta <
                        max(minimum_profit_delta, best.profit_delta) - FEASIBILITY_EPSILON) {
                        continue;
                    }
                    // Assemble prefix, middle, delivery, and suffix summaries for the complete route.
                    auto completed_route = join_segments(
                        join_segments(join_segments(prefix, segment_range(segments, pickup_position,
                                                                          delivery_position)),
                                      delivery),
                        segment_range(segments, delivery_position, route_length));
                    if (is_segment_feasible(taxi, completed_route) &&
                        (best.taxi < 0 || profit_delta > best.profit_delta)) {
                        best = {request, taxi, pickup_position, delivery_position, profit_delta};
                    }
                }
            }
        }
    }
    return best;
}

InsertionChoice find_fleet_insertion(SearchState &state, int request, InsertionCache &cache) {
    if (cache.route_version.empty()) {
        cache.route_version.assign(instance.request_count, vector<UInt64>(instance.taxi_count));
        cache.choices.assign(instance.request_count, vector<InsertionChoice>(instance.taxi_count));
    }
    InsertionChoice best;
    for (int taxi = 0; taxi < instance.taxi_count; taxi++) {
        // Recompute only taxi routes that changed since this request was last examined.
        if (cache.route_version[request][taxi] != state.route_version[taxi]) {
            cache.choices[request][taxi] = find_route_insertion(request, taxi, state.routes[taxi]);
            cache.route_version[request][taxi] = state.route_version[taxi];
        }
        auto choice = cache.choices[request][taxi];
        if (choice.taxi >= 0 && choice.profit_delta > FEASIBILITY_EPSILON &&
            choice.profit_delta > best.profit_delta) {
            best = choice;
        }
    }
    return best;
}

// Per-version preprocessing of the ORIGINAL route only. Own the segment tables:
// the older insertion helper's per-taxi cache can be overwritten by other scans.
struct DeltaRouteIndex {
    UInt64 version = 0;
    RouteSegmentCache segments;
    IntVector pickup, delivery;
};
static vector<DeltaRouteIndex> delta_route_indexes;
static const DeltaRouteIndex &delta_index(const SearchState &state, int taxi) {
    if (delta_route_indexes.size() != size_t(instance.taxi_count))
        delta_route_indexes.resize(instance.taxi_count);
    auto &idx = delta_route_indexes[taxi];
    if (idx.version != state.route_version[taxi]) {
        idx.segments = cached_route_segments(taxi, state.routes[taxi]);
        idx.pickup.assign(instance.request_count, -1);
        idx.delivery.assign(instance.request_count, -1);
        for (int i = 0; i < int(state.routes[taxi].size()); ++i) {
            int node = state.routes[taxi][i], r = instance.node_request[node];
            (instance.is_delivery[node] ? idx.delivery : idx.pickup)[r] = i;
        }
        idx.version = state.route_version[taxi];
    }
    return idx;
}
// A descriptor over at most three unchanged intervals; it never copies a trial
// route. Every node/range lookup takes constant time after route preprocessing.
struct DeltaRouteView {
    const DeltaRouteIndex &idx;
    int taxi, p = -1, d = -1;
    int size() const {
        return int(idx.segments.nodes.size()) - (p >= 0 ? 2 : 0);
    }
    int node(int i) const {
        int k = i;
        if (p >= 0) {
            if (k >= p)
                ++k;
            if (k >= d)
                ++k;
        }
        return idx.segments.nodes[k];
    }
    bool boundary(int i) const {
        return !i || instance.is_delivery[node(i - 1)] ||
               !instance.requests[instance.node_request[node(i - 1)]].is_passenger;
    }
    Segment range(int begin, int end) const {
        if (begin >= end)
            return {};
        if (p < 0)
            return segment_range(idx.segments, begin, end);
        // Most ranges lie wholly inside one retained interval.
        if (end <= p)
            return segment_range(idx.segments, begin, end);
        if (begin >= d - 1)
            return segment_range(idx.segments, begin + 2, end + 2);
        if (begin >= p && end <= d - 1)
            return segment_range(idx.segments, begin + 1, end + 1);
        Segment result;
        if (begin < p) {
            result = segment_range(idx.segments, begin, p);
            begin = p;
        }
        if (begin < d - 1) {
            int stop = min(end, d - 1);
            result = join_segments(result, segment_range(idx.segments, begin + 1, stop + 1));
            begin = stop;
        }
        if (begin < end)
            result = join_segments(result, segment_range(idx.segments, begin + 2, end + 2));
        return result;
    }
};
// Optional route-end arcs have zero cost: there is no return-to-depot edge.
static Real end_arc(int a, int b) {
    return b ? travel_cost(a, b) : 0;
}
static Real removal_arc_delta(const DeltaRouteIndex &idx, int taxi, int r) {
    const auto &v = idx.segments.nodes;
    int p = idx.pickup[r], d = idx.delivery[r];
    int a = p ? v[p - 1] : instance.taxi_origin[taxi], b = d + 1 < int(v.size()) ? v[d + 1] : 0;
    if (d == p + 1)
        return end_arc(a, b) - travel_cost(a, v[p]) - travel_cost(v[p], v[d]) - end_arc(v[d], b);
    return travel_cost(a, v[p + 1]) - travel_cost(a, v[p]) - travel_cost(v[p], v[p + 1]) +
           end_arc(v[d - 1], b) - travel_cost(v[d - 1], v[d]) - end_arc(v[d], b);
}
// Replace a request at its old positions. An incoming passenger's delivery is
// adjacent to its pickup, matching the original operator's precise semantics.
static Real replacement_arc_delta(const DeltaRouteIndex &idx, int taxi, int outgoing,
                                  int incoming) {
    const auto &v = idx.segments.nodes;
    const auto &q = instance.requests[incoming];
    int p = idx.pickup[outgoing], d = idx.delivery[outgoing];
    int a = p ? v[p - 1] : instance.taxi_origin[taxi], b = d + 1 < int(v.size()) ? v[d + 1] : 0;
    if (d == p + 1)
        return travel_cost(a, q.pickup_node) + travel_cost(q.pickup_node, q.delivery_node) +
               end_arc(q.delivery_node, b) - travel_cost(a, v[p]) - travel_cost(v[p], v[d]) -
               end_arc(v[d], b);
    if (q.is_passenger)
        return travel_cost(a, q.pickup_node) + travel_cost(q.pickup_node, q.delivery_node) +
               travel_cost(q.delivery_node, v[p + 1]) - travel_cost(a, v[p]) -
               travel_cost(v[p], v[p + 1]) + end_arc(v[d - 1], b) - travel_cost(v[d - 1], v[d]) -
               end_arc(v[d], b);
    return travel_cost(a, q.pickup_node) + travel_cost(q.pickup_node, v[p + 1]) -
           travel_cost(a, v[p]) - travel_cost(v[p], v[p + 1]) +
           travel_cost(v[d - 1], q.delivery_node) + end_arc(q.delivery_node, b) -
           travel_cost(v[d - 1], v[d]) - end_arc(v[d], b);
}
static Segment replacement_constraints(const DeltaRouteIndex &idx, int outgoing, int incoming) {
    int p = idx.pickup[outgoing], d = idx.delivery[outgoing], length = idx.segments.nodes.size();
    auto first = make_node_segment(instance.requests[incoming].pickup_node);
    auto second = make_node_segment(instance.requests[incoming].delivery_node);
    auto result = join_segments(segment_range(idx.segments, 0, p), first);
    if (instance.requests[incoming].is_passenger) {
        result = join_segments(result, second);
        result = join_segments(result, segment_range(idx.segments, p + 1, d));
    } else {
        result = join_segments(result, segment_range(idx.segments, p + 1, d));
        result = join_segments(result, second);
    }
    return join_segments(result, segment_range(idx.segments, d + 1, length));
}

static InsertionChoice find_view_insertion(int request, int taxi, const DeltaRouteView &view,
                                           Real minimum_profit_delta) {
    // Reject an oversized request and prepare reusable cost-delta arrays.
    InsertionChoice best;
    auto &request_data = instance.requests[request];
    if (request_data.weight > instance.taxi_capacity[taxi] + FEASIBILITY_EPSILON) {
        return best;
    }
    int route_length = view.size();
    static RealVector pickup_cost_delta, delivery_cost_delta, adjacent_cost_delta,
        minimum_delivery_suffix;
    pickup_cost_delta.resize(route_length + 1);
    delivery_cost_delta.resize(route_length + 1);
    adjacent_cost_delta.resize(route_length + 1);
    minimum_delivery_suffix.assign(route_length + 2, INF);
    // Bound insertion cost before checking time windows: adjacent stops or split parcel stops.
    Real minimum_cost_delta = INF, minimum_pickup_delta = INF;
    for (int pickup_position = 0; pickup_position <= route_length; pickup_position++) {
        int previous_node =
            pickup_position ? view.node(pickup_position - 1) : instance.taxi_origin[taxi];
        Real replaced_arc_cost = pickup_position < route_length
                                     ? travel_cost(previous_node, view.node(pickup_position))
                                     : 0;
        pickup_cost_delta[pickup_position] =
            travel_cost(previous_node, request_data.pickup_node) - replaced_arc_cost +
            (pickup_position < route_length
                 ? travel_cost(request_data.pickup_node, view.node(pickup_position))
                 : 0);
        delivery_cost_delta[pickup_position] =
            travel_cost(previous_node, request_data.delivery_node) - replaced_arc_cost +
            (pickup_position < route_length
                 ? travel_cost(request_data.delivery_node, view.node(pickup_position))
                 : 0);
        adjacent_cost_delta[pickup_position] =
            travel_cost(previous_node, request_data.pickup_node) +
            travel_cost(request_data.pickup_node, request_data.delivery_node) - replaced_arc_cost +
            (pickup_position < route_length
                 ? travel_cost(request_data.delivery_node, view.node(pickup_position))
                 : 0);
        minimum_cost_delta = min(minimum_cost_delta, adjacent_cost_delta[pickup_position]);
        if (!request_data.is_passenger) {
            minimum_cost_delta = min(minimum_cost_delta,
                                     minimum_pickup_delta + delivery_cost_delta[pickup_position]);
            minimum_pickup_delta = min(minimum_pickup_delta, pickup_cost_delta[pickup_position]);
        }
    }
    if (request_data.revenue - minimum_cost_delta + FEASIBILITY_EPSILON < minimum_profit_delta) {
        return best;
    }

    // Suffix minima bound the best delivery position after each possible pickup.
    for (int pickup_position = route_length; pickup_position >= 0; pickup_position--) {
        minimum_delivery_suffix[pickup_position] =
            min(delivery_cost_delta[pickup_position], minimum_delivery_suffix[pickup_position + 1]);
    }
    // Reuse route summaries to check insertions without rebuilding every candidate route.
    auto pickup = make_node_segment(request_data.pickup_node),
         delivery = make_node_segment(request_data.delivery_node);
    for (int pickup_position = 0; pickup_position <= route_length; pickup_position++) {
        if (view.boundary(pickup_position)) {
            Real required_profit_delta = max(minimum_profit_delta, best.profit_delta),
                 cost_lower_bound = request_data.is_passenger
                                        ? adjacent_cost_delta[pickup_position]
                                        : min(adjacent_cost_delta[pickup_position],
                                              pickup_cost_delta[pickup_position] +
                                                  minimum_delivery_suffix[pickup_position + 1]);
            if (request_data.revenue - cost_lower_bound + FEASIBILITY_EPSILON <
                required_profit_delta) {
                continue;
            }
            // A feasible pickup prefix is required before exploring delivery positions.
            auto prefix = join_segments(view.range(0, pickup_position), pickup);
            if (!is_segment_feasible(taxi, prefix)) {
                continue;
            }
            for (int delivery_position = pickup_position;
                 delivery_position <= (request_data.is_passenger ? pickup_position : route_length);
                 delivery_position++) {
                if (view.boundary(delivery_position)) {
                    Real profit_delta =
                        request_data.revenue - (pickup_position == delivery_position
                                                    ? adjacent_cost_delta[pickup_position]
                                                    : pickup_cost_delta[pickup_position] +
                                                          delivery_cost_delta[delivery_position]);
                    if (profit_delta <
                        max(minimum_profit_delta, best.profit_delta) - FEASIBILITY_EPSILON) {
                        continue;
                    }
                    // Assemble prefix, middle, delivery, and suffix summaries for the complete route.
                    auto completed_route = join_segments(
                        join_segments(
                            join_segments(prefix, view.range(pickup_position, delivery_position)),
                            delivery),
                        view.range(delivery_position, route_length));
                    if (is_segment_feasible(taxi, completed_route) &&
                        (best.taxi < 0 || profit_delta > best.profit_delta)) {
                        best = {request, taxi, pickup_position, delivery_position, profit_delta};
                    }
                }
            }
        }
    }
    return best;
}
// --- Local search: neighborhoods ---
// Remove one request if the remaining complete route is feasible and more profitable.
bool improve_by_removal(SearchState &state) {
    Real best = FEASIBILITY_EPSILON;
    int selected = -1;
    for (int request = 0; request < instance.request_count; ++request) {
        int taxi = state.request_owner[request];
        if (taxi < 0)
            continue;
        const auto &idx = delta_index(state, taxi);
        Real gain = -instance.requests[request].revenue - removal_arc_delta(idx, taxi, request);
        if (gain > best) {
            DeltaRouteView view{idx, taxi, idx.pickup[request], idx.delivery[request]};
            if (is_segment_feasible(taxi, view.range(0, view.size()))) {
                best = gain;
                selected = request;
            }
        }
    }
    if (selected < 0)
        return false;
    int taxi = state.request_owner[selected];
    update_route(state, taxi, remove_request(state.routes[taxi], selected));
    return true;
}

bool improve_by_insertion(SearchState &state, InsertionCache &cache) {
    InsertionChoice best;
    for (int request = 0; request < instance.request_count; request++) {
        if (state.request_owner[request] < 0) {
            auto choice = find_fleet_insertion(state, request, cache);
            if (choice.taxi >= 0 && choice.profit_delta > best.profit_delta) {
                best = choice;
            }
        }
    }
    if (best.taxi < 0) {
        return false;
    }
    apply_insertion(state, best);
    return true;
}

// Replace a served request with an unserved one; cache the best pair per taxi/incoming request.
// A removal-only route may be infeasible on nonmetric input: test the completed move.
bool improve_by_replacement(SearchState &state, Deadline deadline) {
    if (Clock::now() >= deadline)
        return false;
    // Track route versions and unserved requests from the last completed cache refresh.
    auto &cache = replacement_cache;
    if (cache.route_version.empty()) {
        cache.route_version.assign(instance.taxi_count, 0);
        cache.is_unserved.assign(instance.request_count, 0);
        cache.choices.assign(instance.taxi_count,
                             vector<ReplacementChoice>(instance.request_count));
    }
    // Unchanged routes need entries only for requests that have just become unserved.
    IntVector incoming, newly_unserved;
    for (int request = 0; request < instance.request_count; request++) {
        if (state.request_owner[request] < 0) {
            incoming.push_back(request);
            if (!cache.is_unserved[request]) {
                newly_unserved.push_back(request);
            }
        }
    }
    // Refresh affected taxi/request entries by trying each outgoing served request.
    for (int taxi = 0; taxi < instance.taxi_count; taxi++) {
        bool changed = cache.route_version[taxi] != state.route_version[taxi];
        auto &ids = changed ? incoming : newly_unserved;
        if (ids.empty()) {
            continue;
        }
        cache.route_version[taxi] = 0; // Invalidate before a timed refresh.
        for (int request : ids) {
            cache.choices[taxi][request] = ReplacementChoice{};
        }
        for (int outgoing_request : served_requests(state.routes[taxi])) {
            const auto &idx = delta_index(state, taxi);
            DeltaRouteView view{idx, taxi, idx.pickup[outgoing_request],
                                idx.delivery[outgoing_request]};
            Real removal_gain = -instance.requests[outgoing_request].revenue -
                                removal_arc_delta(idx, taxi, outgoing_request);
            for (int incoming_request : ids) {
                auto &entry = cache.choices[taxi][incoming_request];
                Real required_profit_delta = max(FEASIBILITY_EPSILON, entry.profit_delta);
                auto choice = find_view_insertion(incoming_request, taxi, view,
                                                  required_profit_delta - removal_gain);
                if (choice.taxi >= 0 &&
                    removal_gain + choice.profit_delta > required_profit_delta) {
                    entry = {outgoing_request, choice, removal_gain + choice.profit_delta};
                }
            }
        }
        cache.route_version[taxi] = state.route_version[taxi];
    }

    // Commit the unserved bitmap only after every requested cache entry is refreshed.
    for (int request = 0; request < instance.request_count; request++) {
        cache.is_unserved[request] = state.request_owner[request] < 0;
    }

    // Select the best cached move, using request IDs and insertion positions to break ties.
    ReplacementChoice best;
    best.profit_delta = FEASIBILITY_EPSILON;
    for (int taxi = 0; taxi < instance.taxi_count; taxi++) {
        for (int incoming_request : incoming) {
            auto &replacement = cache.choices[taxi][incoming_request];
            if (replacement.outgoing_request >= 0 &&
                (replacement.profit_delta > best.profit_delta ||
                 (replacement.profit_delta == best.profit_delta &&
                  tie(replacement.outgoing_request, replacement.insertion.request,
                      replacement.insertion.pickup_position,
                      replacement.insertion.delivery_position) <
                      tie(best.outgoing_request, best.insertion.request,
                          best.insertion.pickup_position, best.insertion.delivery_position)))) {
                best = replacement;
            }
        }
    }
    if (best.outgoing_request < 0) {
        return false;
    }
    // Apply removal and insertion together so feasibility belongs to the completed move.
    int taxi = best.insertion.taxi;
    update_route(state, taxi,
                 insert_request(remove_request(state.routes[taxi], best.outgoing_request),
                                best.insertion.request, best.insertion.pickup_position,
                                best.insertion.delivery_position));
    return true;
}

bool improve_by_tail_exchange(SearchState &state, Deadline deadline) {
    if (Clock::now() >= deadline)
        return false;
    // Only cut where no request is onboard, so exchanged tails keep pickup/delivery pairs intact.
    vector<IntVector> cuts(instance.taxi_count);
    for (int taxi = 0; taxi < instance.taxi_count; taxi++) {
        cuts[taxi] = empty_load_cuts(state.routes[taxi]);
    }
    for (int first_taxi = 0; first_taxi < instance.taxi_count; first_taxi++) {
        for (int second_taxi = first_taxi + 1; second_taxi < instance.taxi_count; second_taxi++) {
            auto &first_route = state.routes[first_taxi];
            auto &second_route = state.routes[second_taxi];
            int first_size = first_route.size(), second_size = second_route.size();
            if (first_route.empty() && second_route.empty()) {
                continue;
            }
            Real best = FEASIBILITY_EPSILON;
            int first_cut = -1, second_cut = -1;
            auto &first_segments = cached_route_segments(first_taxi, first_route);
            auto &second_segments = cached_route_segments(second_taxi, second_route);

            // Internal tail costs stay unchanged; only the two connecting arcs affect profit.
            for (int i : cuts[first_taxi]) {
                for (int j : cuts[second_taxi]) {
                    int first_previous = i ? first_route[i - 1] : instance.taxi_origin[first_taxi],
                        second_previous =
                            j ? second_route[j - 1] : instance.taxi_origin[second_taxi];
                    Real profit_delta = 0;
                    if (i < first_size) {
                        profit_delta += travel_cost(first_previous, first_route[i]) -
                                        travel_cost(second_previous, first_route[i]);
                    }
                    if (j < second_size) {
                        profit_delta += travel_cost(second_previous, second_route[j]) -
                                        travel_cost(first_previous, second_route[j]);
                    }
                    if (profit_delta <= best + FEASIBILITY_EPSILON) {
                        continue;
                    }
                    // Check both recombined routes using cached prefix/suffix summaries.
                    if (!is_segment_feasible(
                            first_taxi,
                            join_segments(segment_range(first_segments, 0, i),
                                          segment_range(second_segments, j, second_size))) ||
                        !is_segment_feasible(
                            second_taxi,
                            join_segments(segment_range(second_segments, 0, j),
                                          segment_range(first_segments, i, first_size)))) {
                        continue;
                    }
                    best = profit_delta;
                    first_cut = i;
                    second_cut = j;
                }
            }
            // Apply the best exchange for the first taxi pair that admits an improvement.
            if (first_cut >= 0) {
                Route new_first_route(first_route.begin(), first_route.begin() + first_cut),
                    new_second_route(second_route.begin(), second_route.begin() + second_cut);
                new_first_route.insert(new_first_route.end(), second_route.begin() + second_cut,
                                       second_route.end());
                new_second_route.insert(new_second_route.end(), first_route.begin() + first_cut,
                                        first_route.end());
                update_route_pair(state, first_taxi, new_first_route, second_taxi,
                                  new_second_route);
                return true;
            }
        }
    }
    return false;
}

// Move a request within its route or to another taxi, choosing new insertion positions.

// Swap requests between taxis at their existing pickup/delivery positions.

// ============================================================================
// Helper implementations: input, route bookkeeping, bounds, caches, and LP kernels.
// ============================================================================

// --- Helpers: Input and route evaluation ---
Instance read_instance() {
    // Read fleet dimensions and map one-based service nodes back to zero-based requests.
    Instance input;
    cin >> input.taxi_count >> input.passenger_count >> input.parcel_count;
    input.request_count = input.passenger_count + input.parcel_count;
    input.node_count = input.taxi_count + 2 * input.request_count;
    input.taxi_origin.resize(input.taxi_count);
    input.taxi_capacity.resize(input.taxi_count);
    for (int taxi = 0; taxi < input.taxi_count; taxi++) {
        cin >> input.taxi_origin[taxi] >> input.taxi_capacity[taxi];
    }
    input.requests.resize(input.request_count);
    input.node_request.assign(input.node_count + 1, -1);
    input.is_delivery.assign(input.node_count + 1, 0);
    // Passenger records have pickup windows; parcel records also include weight and delivery windows.
    for (int passenger = 0; passenger < input.passenger_count; passenger++) {
        auto &request_data = input.requests[passenger];
        request_data.is_passenger = 1;
        cin >> request_data.pickup_node >> request_data.delivery_node >>
            request_data.pickup_earliest >> request_data.pickup_latest >>
            request_data.service_time >> request_data.revenue;
        input.node_request[request_data.pickup_node] =
            input.node_request[request_data.delivery_node] = passenger;
        input.is_delivery[request_data.delivery_node] = 1;
    }
    for (int parcel = 0; parcel < input.parcel_count; parcel++) {
        auto &request_data = input.requests[input.passenger_count + parcel];
        request_data.is_passenger = 0;
        cin >> request_data.pickup_node >> request_data.delivery_node >> request_data.weight >>
            request_data.pickup_earliest >> request_data.pickup_latest >>
            request_data.delivery_earliest >> request_data.delivery_latest >>
            request_data.service_time >> request_data.revenue;
        input.node_request[request_data.pickup_node] =
            input.node_request[request_data.delivery_node] = input.passenger_count + parcel;
        input.is_delivery[request_data.delivery_node] = 1;
    }
    // Store the directed travel matrices in row-major order.
    input.travel_time.resize(input.node_count * input.node_count);
    input.travel_cost.resize(input.node_count * input.node_count);
    for (auto &value : input.travel_time) {
        cin >> value;
    }
    for (auto &value : input.travel_cost) {
        cin >> value;
    }
    return input;
}

// Return profit for a feasible route, or no value when a route constraint fails.
// Passengers require adjacent pickup/delivery; parcels may stay onboard between stops.
optional<Real> evaluate_route(const Instance &input, int taxi, const IntVector &route) {
    // Visit stamps avoid clearing per-request pickup/delivery markers on every evaluation.
    static vector<UInt64> picked_at, delivered_at;
    static UInt64 evaluation_stamp = 0;
    if (picked_at.size() != (size_t)input.request_count) {
        picked_at.assign(input.request_count, 0);
        delivered_at.assign(input.request_count, 0);
    }
    ++evaluation_stamp;
    Real departure_time = 0, load = 0, total_cost = 0, revenue = 0;
    int previous_node = input.taxi_origin[taxi], open_requests = 0;
    // Simulate stops in order, accumulating arc cost and checking time, load, and precedence.
    for (int position = 0; position < (int)route.size(); ++position) {
        int node = route[position], request = input.node_request[node];
        auto &request_data = input.requests[request];
        Real arrival =
            departure_time + input.travel_time[(previous_node - 1) * input.node_count + node - 1];
        if (!isfinite(arrival)) {
            return {};
        }
        total_cost += input.travel_cost[(previous_node - 1) * input.node_count + node - 1];
        if (!input.is_delivery[node]) {
            if (picked_at[request] == evaluation_stamp ||
                arrival > request_data.pickup_latest + FEASIBILITY_EPSILON) {
                return {};
            }
            if (request_data.is_passenger && (position + 1 == (int)route.size() ||
                                              route[position + 1] != request_data.delivery_node)) {
                return {};
            }
            load += request_data.weight;
            if (load > input.taxi_capacity[taxi] + FEASIBILITY_EPSILON) {
                return {};
            }
            picked_at[request] = evaluation_stamp;
            ++open_requests;
            revenue += request_data.revenue;
            departure_time = max(arrival, request_data.pickup_earliest) + request_data.service_time;
        } else {
            if (picked_at[request] != evaluation_stamp ||
                delivered_at[request] == evaluation_stamp ||
                (!request_data.is_passenger &&
                 arrival > request_data.delivery_latest + FEASIBILITY_EPSILON)) {
                return {};
            }
            load -= request_data.weight;
            delivered_at[request] = evaluation_stamp;
            --open_requests;
            departure_time =
                max(arrival, request_data.delivery_earliest) + request_data.service_time;
        }
        previous_node = node;
    }
    // Revenue counts at pickup; all picked-up requests must be delivered before the route ends.
    if (open_requests) {
        return {};
    }
    return revenue - total_cost;
}

// --- Helpers: Time, travel, and route accounting ---
Deadline deadline_after(Real seconds) {
    return Clock::now() + chrono::duration_cast<Clock::duration>(chrono::duration<Real>(seconds));
}

Real travel_time(int from_node, int to_node) {
    return instance.travel_time[(from_node - 1) * instance.node_count + to_node - 1];
}

Real travel_cost(int from_node, int to_node) {
    return instance.travel_cost[(from_node - 1) * instance.node_count + to_node - 1];
}

IntVector served_requests(const Route &route) {
    IntVector requests;
    for (int node : route) {
        if (!instance.is_delivery[node]) {
            requests.push_back(instance.node_request[node]);
        }
    }
    sort(requests.begin(), requests.end());
    return requests;
}

// Arc cost only; the route need not be feasible.
Real route_cost(int taxi, const Route &route) {
    Real total_cost = 0;
    int previous_node = instance.taxi_origin[taxi];
    for (int node : route) {
        total_cost += travel_cost(previous_node, node);
        previous_node = node;
    }
    return total_cost;
}

// Revenue minus arc cost; feasibility is checked by the caller when required.
Real route_profit(int taxi, const Route &route) {
    Real total_profit = -route_cost(taxi, route);
    for (int node : route) {
        if (!instance.is_delivery[node]) {
            total_profit += instance.requests[instance.node_request[node]].revenue;
        }
    }
    return total_profit;
}

Real fleet_profit(const Fleet &fleet) {
    Real total_profit = 0;
    for (int taxi = 0; taxi < instance.taxi_count; taxi++) {
        total_profit += route_profit(taxi, fleet[taxi]);
    }
    return total_profit;
}

// --- Helpers: Route-bank maintenance ---
// Ordinary DFS routes obey the bank cap; fleet and priced routes bypass it.
void store_route(int taxi, const Route &route, bool enforce_limit, Real profit) {
    if (full_probe_mode)
        enforce_limit = false;
    // Discard unprofitable routes and candidates below a full capped bank's profit floor.
    if (profit == -INF) {
        profit = route_profit(taxi, route);
    }
    if (profit <= FEASIBILITY_EPSILON) {
        return;
    }
    auto &taxi_bank = route_bank[taxi];
    if (enforce_limit && taxi_bank.size() >= (size_t)bank_route_limit &&
        profit < bank_profit_floor[taxi]) {
        return;
    }
    // Routes covering the same request set compete for one bank entry, keeping the best profit.
    auto requests = served_requests(route);
    auto entry = taxi_bank.lower_bound(requests);
    if (entry != taxi_bank.end() && entry->first == requests) {
        if (entry->second.profit >= profit) {
            return;
        }
        entry->second = {pack_stored_route(route), profit};
    } else {
        if (full_probe_mode) {
            if (full_probe_columns >= FULL_PROBE_COLUMN_GUARD) {
                full_probe_truncated = true;
                dfs_stopped = true;
                return;
            }
            full_probe_columns++;
        }
        taxi_bank.emplace_hint(entry, requests, StoredRoute{pack_stored_route(route), profit});
    }
    // Protected insertions bypass the cap; capped insertions evict the least profitable route.
    if (!enforce_limit) {
        bank_profit_floor[taxi] = -INF;
    }
    if (enforce_limit && taxi_bank.size() > (size_t)bank_route_limit) {
        auto worst = min_element(taxi_bank.begin(), taxi_bank.end(), [](auto &left, auto &right) {
            return left.second.profit < right.second.profit;
        });
        taxi_bank.erase(worst);
        bank_profit_floor[taxi] =
            min_element(taxi_bank.begin(), taxi_bank.end(), [](auto &left, auto &right) {
                return left.second.profit < right.second.profit;
            })->second.profit;
    }
}

void store_fleet(const Fleet &fleet) {
    for (int taxi = 0; taxi < instance.taxi_count; taxi++) {
        store_route(taxi, fleet[taxi]);
    }
}

// --- Helpers: Fleet-state updates ---
SearchState make_search_state(const Fleet &fleet) {
    SearchState state;
    state.routes = fleet;
    state.request_owner.assign(instance.request_count, -1);
    state.route_profit.resize(instance.taxi_count);
    state.route_version.resize(instance.taxi_count);
    for (int taxi = 0; taxi < instance.taxi_count; taxi++) {
        for (int request : served_requests(fleet[taxi])) {
            state.request_owner[request] = taxi;
        }
        state.route_profit[taxi] = route_profit(taxi, fleet[taxi]);
        state.profit += state.route_profit[taxi];
        state.route_version[taxi] = next_route_version++;
    }
    return state;
}

void update_route(SearchState &state, int taxi, const Route &route) {
    for (int request : served_requests(state.routes[taxi])) {
        state.request_owner[request] = -1;
    }
    state.profit -= state.route_profit[taxi];
    state.routes[taxi] = route;
    state.route_profit[taxi] = route_profit(taxi, route);
    state.profit += state.route_profit[taxi];
    for (int request : served_requests(route)) {
        state.request_owner[request] = taxi;
    }
    state.route_version[taxi] = next_route_version++;
}

void update_route_pair(SearchState &state, int first_taxi, const Route &first_route,
                       int second_taxi, const Route &second_route) {
    if (first_taxi == second_taxi) {
        update_route(state, first_taxi, second_route);
        return;
    }
    // Release both old request sets before assigning either new route, avoiding ownership conflicts.
    for (int taxi : {first_taxi, second_taxi}) {
        for (int request : served_requests(state.routes[taxi])) {
            state.request_owner[request] = -1;
        }
        state.profit -= state.route_profit[taxi];
    }
    state.routes[first_taxi] = first_route;
    state.routes[second_taxi] = second_route;
    // Rebuild ownership, profit, and cache versions for both new routes.
    for (int taxi : {first_taxi, second_taxi}) {
        state.route_profit[taxi] = route_profit(taxi, state.routes[taxi]);
        state.profit += state.route_profit[taxi];
        for (int request : served_requests(state.routes[taxi])) {
            state.request_owner[request] = taxi;
        }
        state.route_version[taxi] = next_route_version++;
    }
}

void apply_insertion(SearchState &state, InsertionChoice choice) {
    update_route(state, choice.taxi,
                 insert_request(state.routes[choice.taxi], choice.request, choice.pickup_position,
                                choice.delivery_position));
    store_route(choice.taxi, state.routes[choice.taxi]);
}

// --- Helpers: Route edits and cut positions ---
// Never split a passenger's adjacent pickup/delivery block.
bool is_insertion_boundary(const Route &route, int position) {
    return !position || instance.is_delivery[route[position - 1]] ||
           !instance.requests[instance.node_request[route[position - 1]]].is_passenger;
}

Route remove_request(const Route &route, int request) {
    Route reduced_route;
    for (int node : route) {
        if (instance.node_request[node] != request) {
            reduced_route.push_back(node);
        }
    }
    return reduced_route;
}

Route insert_request(Route route, int request, int pickup_position, int delivery_position) {
    route.insert(route.begin() + delivery_position, instance.requests[request].delivery_node);
    route.insert(route.begin() + pickup_position, instance.requests[request].pickup_node);
    return route;
}

// Tail exchanges cut only where every picked-up request has been delivered.
IntVector empty_load_cuts(const Route &route) {
    IntVector cuts{0};
    int open_requests = 0;
    for (int i = 0; i < (int)route.size(); i++) {
        open_requests += instance.is_delivery[route[i]] ? -1 : 1;
        if (!open_requests) {
            cuts.push_back(i + 1);
        }
    }
    return cuts;
}

// --- Helpers: Directed delivery bounds ---
// Landmark shortest paths bound parcel delivery time on directed, nonmetric input.
// A direct arc is not a valid lower bound when intermediate stops can be faster.
void build_delivery_lower_bounds(Deadline deadline) {
    int node_count = instance.node_count;
    vector<RealVector> landmarks;
    RealVector spread(node_count, INF);
    vector<char> used(node_count);
    int landmark = 0;
    auto bound_deadline = min(
        deadline, deadline_after(min(LANDMARK_MAX_SECONDS,
                                     LANDMARK_BUDGET_FRACTION *
                                         chrono::duration<Real>(deadline - Clock::now()).count())));
    // Run a bounded number of dense Dijkstra searches from spread-out landmarks.
    for (int pass = 0; pass < min(LANDMARK_LIMIT, node_count) && Clock::now() < bound_deadline;
         pass++) {
        RealVector distances(instance.travel_time.begin() + landmark * node_count,
                             instance.travel_time.begin() + (landmark + 1) * node_count);
        vector<char> visited(node_count);
        distances[landmark] = 0;
        bool complete = true;
        for (int step = 0; step < node_count; step++) {
            if (step % LANDMARK_CLOCK_INTERVAL == 0 && Clock::now() >= bound_deadline) {
                complete = false;
                break;
            }
            int closest_node = -1;
            Real closest_distance = INF;
            for (int node = 0; node < node_count; node++) {
                if (!visited[node] && distances[node] < closest_distance) {
                    closest_distance = distances[node];
                    closest_node = node;
                }
            }
            if (closest_node < 0) {
                break;
            }
            visited[closest_node] = 1;
            for (int node = 0; node < node_count; node++) {
                if (!visited[node]) {
                    distances[node] = min(
                        distances[node],
                        closest_distance + instance.travel_time[closest_node * node_count + node]);
                }
            }
        }
        // Discard interrupted searches; only completed shortest-path distances define valid bounds.
        if (!complete) {
            break;
        }
        used[landmark] = 1;
        for (int node = 0; node < node_count; node++) {
            spread[node] = min(spread[node], distances[node]);
        }
        landmarks.push_back(move(distances));
        // Pick the next landmark farthest from its nearest existing landmark.
        Real farthest = -1;
        for (int node = 0; node < node_count; node++) {
            if (!used[node] && spread[node] > farthest) {
                farthest = spread[node];
                landmark = node;
            }
        }
    }
    // Directed triangle inequality for shortest paths gives d(L, delivery) - d(L, node).
    // Reduce by a rounding margin and retain the strongest nonnegative landmark bound.
    delivery_time_lower_bound.assign(node_count * instance.parcel_count, 0);
    for (int node = 0; node < node_count; node++) {
        for (int parcel = 0; parcel < instance.parcel_count; parcel++) {
            for (auto &distances : landmarks) {
                int delivery_node =
                    instance.requests[instance.passenger_count + parcel].delivery_node - 1;
                Real rounding_margin =
                    FEASIBILITY_EPSILON + 64 * numeric_limits<Real>::epsilon() * node_count *
                                              max({1.0, distances[delivery_node], distances[node]});
                auto &lower_bound =
                    delivery_time_lower_bound[node * instance.parcel_count + parcel];
                lower_bound =
                    max(lower_bound, distances[delivery_node] - distances[node] - rounding_margin);
            }
        }
    }
}

// --- Helpers: Segment composition and cache queries ---
Segment make_node_segment(int node) {
    auto &request_data = instance.requests[instance.node_request[node]];
    bool is_delivery = instance.is_delivery[node];
    Segment segment;
    segment.first_node = segment.last_node = node;
    segment.duration = request_data.service_time;
    segment.earliest_departure =
        (!is_delivery ? request_data.pickup_earliest : request_data.delivery_earliest) +
        request_data.service_time;
    segment.latest_arrival =
        (!is_delivery ? request_data.pickup_latest : request_data.delivery_latest) +
        FEASIBILITY_EPSILON;
    if (is_delivery && request_data.is_passenger) {
        segment.earliest_departure = -INF;
        segment.latest_arrival = numeric_limits<Real>::max();
    }
    segment.load_change = is_delivery ? -request_data.weight : request_data.weight;
    segment.minimum_load = min(0.0, segment.load_change);
    segment.maximum_load = max(0.0, segment.load_change);
    return segment;
}

Segment join_segments(Segment left, const Segment &right) {
    if (!left.first_node) {
        return right;
    }
    if (!right.first_node) {
        return left;
    }
    Real arc_time = travel_time(left.last_node, right.first_node), duration = left.duration,
         earliest_departure = left.earliest_departure, load_change = left.load_change;
    // Compose waiting and latest-arrival limits across the connecting arc.
    left.feasible =
        left.feasible && right.feasible && earliest_departure + arc_time <= right.latest_arrival;
    left.latest_arrival = min(left.latest_arrival, right.latest_arrival - arc_time - duration);
    left.duration = duration + arc_time + right.duration;
    left.earliest_departure =
        max(earliest_departure + arc_time + right.duration, right.earliest_departure);
    // Shift right-segment load extrema by the net load change in the left segment.
    left.minimum_load = min(left.minimum_load, load_change + right.minimum_load);
    left.maximum_load = max(left.maximum_load, load_change + right.maximum_load);
    left.load_change = load_change + right.load_change;
    left.last_node = right.last_node;
    return left;
}

bool is_segment_feasible(int taxi, const Segment &segment) {
    return !segment.first_node ||
           (segment.feasible &&
            travel_time(instance.taxi_origin[taxi], segment.first_node) <= segment.latest_arrival &&
            segment.minimum_load >= -FEASIBILITY_EPSILON &&
            segment.maximum_load <= instance.taxi_capacity[taxi] + FEASIBILITY_EPSILON);
}

RouteSegmentCache &cached_route_segments(int taxi, const Route &route) {
    if (route_segment_cache.empty()) {
        route_segment_cache.resize(instance.taxi_count);
    }
    auto &segments = route_segment_cache[taxi];
    if (segments.nodes != route) {
        // Rebuild only when route contents change; first summarize each service node.
        segments.nodes = route;
        segments.node_segments.clear();
        segments.range_table.clear();
        int node_count = route.size();
        for (int node : route) {
            segments.node_segments.push_back(make_node_segment(node));
        }
        // Each level stores suffixes left of a block midpoint and prefixes to its right.
        for (int half = 1; half < node_count; half *= 2) {
            segments.range_table.emplace_back(node_count);
            auto &level = segments.range_table.back();
            for (int start = 0; start < node_count; start += 2 * half) {
                int middle = min(start + half, node_count), end = min(start + 2 * half, node_count);
                if (middle > start) {
                    level[middle - 1] = segments.node_segments[middle - 1];
                    for (int i = middle - 1; i > start; i--) {
                        level[i - 1] = join_segments(segments.node_segments[i - 1], level[i]);
                    }
                }
                if (middle < end) {
                    level[middle] = segments.node_segments[middle];
                    for (int i = middle + 1; i < end; i++) {
                        level[i] = join_segments(level[i - 1], segments.node_segments[i]);
                    }
                }
            }
        }
    }
    return segments;
}

// Query the half-open range [begin, end).
Segment segment_range(const RouteSegmentCache &segments, int begin, int end) {
    if (begin >= end) {
        return {};
    }
    if (begin + 1 == end) {
        return segments.node_segments[begin];
    }
    // The highest differing endpoint bit selects two disjoint summaries covering the range.
    int level = 31 - __builtin_clz((unsigned)(begin ^ (end - 1)));
    return join_segments(segments.range_table[level][begin], segments.range_table[level][end - 1]);
}

// --- Helpers: Packing matrix and simplex kernels ---
// Rows enforce one route per taxi and at most one visit per shared request.
// Requests appearing only in one taxi's columns need no separate resource row.
PackingMatrix build_packing_matrix(const vector<MasterColumn> &columns, size_t taxi_count,
                                   size_t request_count) {
    PackingMatrix matrix;
    matrix.columns = &columns;
    matrix.column_rows.resize(columns.size());
    matrix.taxi_count = taxi_count;
    // Mark requests shared across taxis; private requests are already limited by their taxi row.
    IntVector request_taxi(request_count, -1);
    for (auto &column : columns) {
        for (auto resource : column.requests) {
            if (request_taxi[resource] == -1) {
                request_taxi[resource] = column.taxi;
            } else if (request_taxi[resource] != (int)column.taxi) {
                request_taxi[resource] = -2;
            }
        }
    }
    // Compress taxi and shared-request resources into the rows touched by each column.
    IntVector resource_row(taxi_count + request_count, -1);
    for (size_t j = 0; j < columns.size(); ++j) {
        auto add_resource = [&](size_t resource) {
            if (resource >= taxi_count && request_taxi[resource - taxi_count] != -2) {
                return;
            }
            if (resource_row[resource] < 0) {
                resource_row[resource] = int(matrix.dimension++);
                matrix.row_resources.push_back(resource);
            }
            matrix.column_rows[j].push_back(resource_row[resource]);
        };
        add_resource(columns[j].taxi);
        for (auto resource : columns[j].requests) {
            add_resource(taxi_count + resource);
        }
    }
    return matrix;
}

PackingLP initialize_lp(const PackingMatrix &matrix) {

    PackingLP lp;
    lp.matrix = &matrix;
    lp.row_count = matrix.dimension;
    lp.right_hand_sides.assign(lp.row_count, 1);
    lp.bound_signs.assign(lp.row_count, 0);
    lp.dual_prices.assign(lp.row_count, 0);
    lp.column_bound_rows.assign(matrix.columns->size(), -1);
    lp.backend_lower.assign(matrix.columns->size(), 0);
    lp.backend_upper.assign(matrix.columns->size(), INF);
    return lp;
}

void primal_simplex(PackingLP &lp, Deadline deadline) {
    solve_embedded_lp(lp, deadline);
}

// Include/exclude a column with -x <= -1 or x <= 0, retaining the current basis.
void add_column_bound(PackingLP &lp, size_t column, bool include) {

    lp.column_bound_rows[column] = static_cast<int>(lp.row_count++);
    lp.bound_signs.push_back(include ? -1 : 1);
    lp.right_hand_sides.push_back(include ? -1 : 0);
    lp.dual_prices.push_back(0);
    if (include)
        lp.backend_lower[column] = 1;
    else
        lp.backend_upper[column] = 0;
}

// Restore primal feasibility after a branch adds a bound.
void dual_simplex(PackingLP &lp, Deadline deadline) {
    solve_embedded_lp(lp, deadline);
}

RealVector primal_values(const PackingLP &lp) {
    return lp.backend ? lp.backend->primal : RealVector(lp.matrix->columns->size(), 0);
}

// Correct dual residuals per taxi and add a rounding margin for safe pruning.
Real dual_upper_bound(const PackingLP &lp) {
    // Nonnegative resource prices contribute the main dual bound, including branch rows.
    auto prices = lp.dual_prices;
    Real bound = 0, magnitude = 0;
    for (size_t resource_row = 0; resource_row < lp.row_count; ++resource_row) {
        if (!isfinite(prices[resource_row])) {
            return INF;
        }
        prices[resource_row] = max(0.0, prices[resource_row]);
        bound += prices[resource_row] * lp.right_hand_sides[resource_row];
        magnitude += abs(prices[resource_row] * lp.right_hand_sides[resource_row]);
    }
    // Add each taxi's largest positive reduced-profit residual to cover dual violations.
    RealVector residual(lp.matrix->taxi_count, 0);
    for (size_t j = 0; j < lp.matrix->columns->size(); ++j) {
        auto &column = (*lp.matrix->columns)[j];
        Real value = column.profit;
        for (auto resource_row : lp.matrix->column_rows[j]) {
            value -= prices[resource_row];
        }
        if (lp.column_bound_rows[j] >= 0) {
            auto resource_row = size_t(lp.column_bound_rows[j]);
            value -= lp.bound_signs[resource_row] * prices[resource_row];
        }
        residual[column.taxi] = max(residual[column.taxi], value);
    }
    // Include residuals and numerical margins before using the bound for branch pruning.
    auto remainder = accumulate(residual.begin(), residual.end(), 0.0);
    bound += remainder;
    magnitude += remainder;
    if (!isfinite(bound) || !isfinite(magnitude)) {
        return INF;
    }
    return bound + LP_FEASIBILITY_EPSILON + BOUND_RELATIVE_EPSILON * magnitude;
}

RealVector request_prices_from_lp(const PackingLP &lp, size_t request_count) {
    RealVector result(request_count, 0);
    for (size_t row = 0; row < lp.matrix->row_resources.size(); ++row) {
        auto resource = lp.matrix->row_resources[row];
        if (resource >= lp.matrix->taxi_count && resource < lp.matrix->taxi_count + request_count &&
            isfinite(lp.dual_prices[row])) {
            result[resource - lp.matrix->taxi_count] = max(0.0, lp.dual_prices[row]);
        }
    }
    return result;
}

// Parcel-only descent during fleet reconstruction. Probe cost/feasibility with
// constant-many segment summaries; materialize only the chosen complete move.
// Preserve the periodic clock check across swap scans.
static size_t parcel_swap_clock_count = 0;

static bool improve_alns_parcel_relocation(SearchState &, Deadline);

struct ParcelSwapScan {
    const Route *route = nullptr;
    RouteSegmentCache *segments = nullptr;
    RealVector prefix;
    Real cost = 0;
    ParcelRouteSummary piece(int begin, int end) const {
        return {segment_range(*segments, begin, end),
                begin < end ? prefix[end] - prefix[begin + 1] : 0};
    }
};
static bool improve_alns_parcel_swap(SearchState &state, Deadline deadline);

static void improve_parcel_alns(SearchState &state, Deadline deadline) {
    bool changed = true;
    while (changed && Clock::now() < deadline) {
        changed = false;
        while (Clock::now() < deadline && improve_alns_parcel_relocation(state, deadline))
            changed = true;
        while (Clock::now() < deadline && improve_alns_parcel_swap(state, deadline))
            changed = true;
    }
    store_fleet(state.routes);
}

// Reusable scratch belongs to this single-threaded solver. Outputs are
// distinct from the live state routes and are never moved out of the buffers.
static void remove_request_into(const Route &route, int request, Route &out) {
    out.clear();
    out.reserve(route.size());
    for (int node : route)
        if (instance.node_request[node] != request)
            out.push_back(node);
}
static void insert_request_into(const Route &route, int request, int pickup, int delivery,
                                Route &out) {
    out.reserve(route.size() + 2);
    out.assign(route.begin(), route.end());
    out.insert(out.begin() + delivery, instance.requests[request].delivery_node);
    out.insert(out.begin() + pickup, instance.requests[request].pickup_node);
}
static void swapped_request_route_into(const SearchState &state, int taxi, int outgoing,
                                       int incoming, Route &out) {
    const auto &route = state.routes[taxi];
    out.reserve(route.size() + 2);
    out.assign(route.begin(), route.end());
    int pickup =
        find(out.begin(), out.end(), instance.requests[outgoing].pickup_node) - out.begin();
    int delivery =
        find(out.begin(), out.end(), instance.requests[outgoing].delivery_node) - out.begin();
    out.erase(out.begin() + delivery);
    out.erase(out.begin() + pickup);
    out.insert(out.begin() + (instance.requests[incoming].is_passenger ? pickup : delivery - 1),
               instance.requests[incoming].delivery_node);
    out.insert(out.begin() + pickup, instance.requests[incoming].pickup_node);
}
// Exact relocation memoization. A request/destination depends only on its
// owner's route and the destination route. Versions are globally unique.
struct RelocationMemo {
    UInt64 source_version = 0, destination_version = 0;
    int source = -1;
    InsertionChoice insertion;
    Real gain = -INF;
};
struct RemovalMemo {
    UInt64 version = 0;
    int taxi = -1;
    Route reduced;
    Real gain = 0;
    bool feasible = false;
};
static vector<RelocationMemo> relocation_memos;
static vector<RemovalMemo> removal_memos;
static size_t relocation_cache_hits = 0, relocation_cache_misses = 0, removal_cache_builds = 0;

static const RemovalMemo &cached_request_removal(const SearchState &state, int request) {
    if (removal_memos.size() != size_t(instance.request_count))
        removal_memos.resize(instance.request_count);
    auto &entry = removal_memos[request];
    int taxi = state.request_owner[request];
    if (entry.taxi != taxi || entry.version != state.route_version[taxi]) {
        entry.taxi = taxi;
        entry.version = state.route_version[taxi];
        const auto &idx = delta_index(state, taxi);
        DeltaRouteView view{idx, taxi, idx.pickup[request], idx.delivery[request]};
        entry.feasible = is_segment_feasible(taxi, view.range(0, view.size()));
        entry.gain = -instance.requests[request].revenue - removal_arc_delta(idx, taxi, request);
        ++removal_cache_builds;
    }
    return entry;
}
static const RelocationMemo &cached_relocation(const SearchState &state, int request,
                                               int destination) {
    const size_t count = size_t(instance.request_count) * instance.taxi_count;
    if (relocation_memos.size() != count)
        relocation_memos.resize(count);
    int source = state.request_owner[request];
    auto &entry = relocation_memos[size_t(request) * instance.taxi_count + destination];
    if (entry.source == source && entry.source_version == state.route_version[source] &&
        entry.destination_version == state.route_version[destination]) {
        ++relocation_cache_hits;
        return entry;
    }
    ++relocation_cache_misses;
    const auto &removal = cached_request_removal(state, request);
    entry.insertion = InsertionChoice{};
    entry.gain = -INF;
    // Cross-route removal is final on its source and must be feasible. A
    // same-route removal skeleton may be infeasible: the reinsertion can fix it.
    if (source == destination || removal.feasible) {
        const auto &idx = delta_index(state, destination);
        DeltaRouteView view{idx, destination};
        if (source == destination) {
            view.p = idx.pickup[request];
            view.d = idx.delivery[request];
        }
        entry.insertion =
            find_view_insertion(request, destination, view, FEASIBILITY_EPSILON - removal.gain);
        if (entry.insertion.taxi >= 0)
            entry.gain = removal.gain + entry.insertion.profit_delta;
    }
    entry.source = source;
    entry.source_version = state.route_version[source];
    entry.destination_version = state.route_version[destination];
    return entry;
}
static bool cached_relocation_descent_step(SearchState &state, Deadline deadline,
                                           bool parcels_only) {
    if (Clock::now() >= deadline)
        return false;
    Real best = FEASIBILITY_EPSILON;
    int source = -1, target = -1, selected = -1;
    InsertionChoice chosen;
    for (int request = parcels_only ? instance.passenger_count : 0;
         request < instance.request_count; ++request) {
        if (state.request_owner[request] < 0)
            continue;
        for (int destination = 0; destination < instance.taxi_count; ++destination) {
            const auto &entry = cached_relocation(state, request, destination);
            if (entry.insertion.taxi >= 0 && entry.gain > best) {
                best = entry.gain;
                source = entry.source;
                target = destination;
                selected = request;
                chosen = entry.insertion;
            }
        }
    }
    if (selected < 0)
        return false;
    static Route reduced, inserted;
    remove_request_into(state.routes[source], selected, reduced);
    insert_request_into(source == target ? reduced : state.routes[target], selected,
                        chosen.pickup_position, chosen.delivery_position, inserted);
    if (parcels_only) {
        // Preserve the parcel operator's independent complete-route validation.
        auto value = evaluate_route(instance, target, inserted);
        if (!value)
            return false;
        Real delta = source == target ? *value - state.route_profit[source]
                                      : route_profit(source, reduced) + *value -
                                            state.route_profit[source] - state.route_profit[target];
        if (delta <= FEASIBILITY_EPSILON || abs(delta - best) > 1e-7)
            return false;
    }
    update_route_pair(state, source, reduced, target, inserted);
    return true;
}
bool improve_by_relocation(SearchState &state, Deadline deadline) {
    return cached_relocation_descent_step(state, deadline, false);
}
static bool improve_alns_parcel_relocation(SearchState &state, Deadline deadline) {
    return cached_relocation_descent_step(state, deadline, true);
}

// Cache both positive and negative results for the fixed-position request swap.
// Source and target route versions certify feasibility, profit and stop positions.
struct SwapMemo {
    UInt64 first_version = 0, second_version = 0;
    Real gain = -INF;
    bool feasible = false;
};
static vector<SwapMemo> swap_memos;
static size_t swap_cache_hits = 0, swap_cache_misses = 0;

bool improve_by_swap(SearchState &state, Deadline deadline) {
    if (Clock::now() >= deadline)
        return false;
    size_t count = size_t(instance.request_count) * instance.request_count;
    if (swap_memos.size() != count)
        swap_memos.resize(count);
    static Route left, right;
    Real best = FEASIBILITY_EPSILON;
    int selected_a = -1, selected_b = -1;
    for (int a = 0; a < instance.request_count; ++a) {
        int first = state.request_owner[a];
        if (first < 0)
            continue;
        for (int b = a + 1; b < instance.request_count; ++b) {
            int second = state.request_owner[b];
            if (second < 0 || first == second)
                continue;
            auto &entry = swap_memos[size_t(a) * instance.request_count + b];
            if (entry.first_version == state.route_version[first] &&
                entry.second_version == state.route_version[second])
                ++swap_cache_hits;
            else {
                ++swap_cache_misses;
                const auto &left_idx = delta_index(state, first),
                           &right_idx = delta_index(state, second);
                entry.gain = -replacement_arc_delta(left_idx, first, a, b) -
                             replacement_arc_delta(right_idx, second, b, a);
                entry.feasible =
                    entry.gain > FEASIBILITY_EPSILON &&
                    is_segment_feasible(first, replacement_constraints(left_idx, a, b)) &&
                    is_segment_feasible(second, replacement_constraints(right_idx, b, a));
                entry.first_version = state.route_version[first];
                entry.second_version = state.route_version[second];
            }
            if (entry.feasible && entry.gain > best) {
                best = entry.gain;
                selected_a = a;
                selected_b = b;
            }
        }
    }
    if (selected_a < 0)
        return false;
    int first = state.request_owner[selected_a], second = state.request_owner[selected_b];
    swapped_request_route_into(state, first, selected_a, selected_b, left);
    swapped_request_route_into(state, second, selected_b, selected_a, right);
    update_route_pair(state, first, left, second, right);
    return true;
}

// Separate memo table preserves this operator's segment-based arithmetic and
// includes same-route delivery exchanges. No reuse of generic-swap gain values.
static vector<SwapMemo> parcel_swap_memos;
static size_t parcel_swap_cache_hits = 0, parcel_swap_cache_misses = 0;
static bool improve_alns_parcel_swap(SearchState &state, Deadline deadline) {
    if (Clock::now() >= deadline)
        return false;
    size_t memo_count = size_t(instance.request_count) * instance.request_count;
    if (parcel_swap_memos.size() != memo_count)
        parcel_swap_memos.resize(memo_count);

    struct Parcel {
        int request, taxi, pickup, delivery;
    };
    static vector<Parcel> parcels;
    parcels.clear();
    parcels.reserve(instance.parcel_count);
    static vector<ParcelSwapScan> scans;
    scans.resize(instance.taxi_count);
    static IntVector pickup;
    pickup.resize(instance.request_count);
    // Build one cache per unchanged route, before probing any swap.
    for (int taxi = 0; taxi < instance.taxi_count; ++taxi) {
        const auto &route = state.routes[taxi];
        auto &scan = scans[taxi];
        scan.route = &route;
        scan.segments = &cached_route_segments(taxi, route);
        scan.prefix.assign(route.size() + 1, 0);
        fill(pickup.begin(), pickup.end(), -1);
        for (int pos = 0; pos < int(route.size()); ++pos) {
            if (pos)
                scan.prefix[pos + 1] = scan.prefix[pos] + travel_cost(route[pos - 1], route[pos]);
            int request = instance.node_request[route[pos]];
            if (!instance.is_delivery[route[pos]])
                pickup[request] = pos;
            else if (!instance.requests[request].is_passenger)
                parcels.push_back({request, taxi, pickup[request], pos});
        }
        scan.cost = route.empty() ? 0
                                  : scan.prefix.back() +
                                        travel_cost(instance.taxi_origin[taxi], route.front());
    }
    auto substitution = [&](const Parcel &outgoing, int incoming) {
        auto &scan = scans[outgoing.taxi];
        const auto &q = instance.requests[incoming];
        ParcelRouteSummary pickup{make_node_segment(q.pickup_node), 0},
            delivery{make_node_segment(q.delivery_node), 0};
        return join_parcel_summaries(
            join_parcel_summaries(
                join_parcel_summaries(join_parcel_summaries(scan.piece(0, outgoing.pickup), pickup),
                                      scan.piece(outgoing.pickup + 1, outgoing.delivery)),
                delivery),
            scan.piece(outgoing.delivery + 1, scan.route->size()));
    };
    auto total_cost = [&](int taxi, const ParcelRouteSummary &summary) {
        return summary.cost +
               travel_cost(instance.taxi_origin[taxi], summary.constraints.first_node);
    };
    Real best = FEASIBILITY_EPSILON;
    int selected_a = -1, selected_b = -1;
    bool same_route = false;
    for (size_t a = 0; a < parcels.size(); ++a)
        for (size_t b = a + 1; b < parcels.size(); ++b) {
            const auto &x = parcels[a], &y = parcels[b];
            auto &memo = parcel_swap_memos[size_t(x.request) * instance.request_count + y.request];
            if (memo.first_version == state.route_version[x.taxi] &&
                memo.second_version == state.route_version[y.taxi]) {
                ++parcel_swap_cache_hits;
                if (memo.feasible && memo.gain > best) {
                    best = memo.gain;
                    selected_a = a;
                    selected_b = b;
                    same_route = x.taxi == y.taxi;
                }
                continue;
            }
            ++parcel_swap_cache_misses;
            memo.first_version = state.route_version[x.taxi];
            memo.second_version = state.route_version[y.taxi];
            memo.gain = -INF;
            memo.feasible = false;

            if (x.taxi == y.taxi) {
                // Exchange delivery endpoints while retaining every other stop.
                const Parcel &left = x.delivery < y.delivery ? x : y,
                             &right = x.delivery < y.delivery ? y : x;
                if (right.pickup >= left.delivery)
                    continue;
                const auto &scan = scans[x.taxi];
                ParcelRouteSummary first{
                    make_node_segment(instance.requests[right.request].delivery_node), 0};
                ParcelRouteSummary second{
                    make_node_segment(instance.requests[left.request].delivery_node), 0};
                auto summary = join_parcel_summaries(
                    join_parcel_summaries(
                        join_parcel_summaries(
                            join_parcel_summaries(scan.piece(0, left.delivery), first),
                            scan.piece(left.delivery + 1, right.delivery)),
                        second),
                    scan.piece(right.delivery + 1, scan.route->size()));
                Real gain = scan.cost - total_cost(x.taxi, summary);
                memo.gain = gain;
                memo.feasible =
                    gain > FEASIBILITY_EPSILON && is_segment_feasible(x.taxi, summary.constraints);
                if (memo.feasible && gain > best) {
                    best = gain;
                    selected_a = a;
                    selected_b = b;
                    same_route = true;
                }
            } else {
                auto first = substitution(x, y.request), second = substitution(y, x.request);
                Real gain = scans[x.taxi].cost + scans[y.taxi].cost - total_cost(x.taxi, first) -
                            total_cost(y.taxi, second);
                memo.gain = gain;
                memo.feasible = gain > FEASIBILITY_EPSILON &&
                                is_segment_feasible(x.taxi, first.constraints) &&
                                is_segment_feasible(y.taxi, second.constraints);
                if (memo.feasible && gain > best) {
                    best = gain;
                    selected_a = a;
                    selected_b = b;
                    same_route = false;
                }
            }
        }
    if (selected_a < 0)
        return false;
    const auto &x = parcels[selected_a], &y = parcels[selected_b];
    static Route first, second;
    first.assign(state.routes[x.taxi].begin(), state.routes[x.taxi].end());
    second.assign(state.routes[y.taxi].begin(), state.routes[y.taxi].end());
    if (same_route)
        swap(first[x.delivery], first[y.delivery]);
    else {
        first[x.pickup] = instance.requests[y.request].pickup_node;
        first[x.delivery] = instance.requests[y.request].delivery_node;
        second[y.pickup] = instance.requests[x.request].pickup_node;
        second[y.delivery] = instance.requests[x.request].delivery_node;
    }
    const auto a = evaluate_route(instance, x.taxi, first),
               b = same_route ? optional<Real>{0} : evaluate_route(instance, y.taxi, second);
    Real old = state.route_profit[x.taxi] + (same_route ? 0 : state.route_profit[y.taxi]);
    if (!a || !b || *a + *b <= old + FEASIBILITY_EPSILON || abs(*a + *b - old - best) > 1e-7)
        return false;
    if (same_route) {
        update_route(state, x.taxi, first);
    } else {
        update_route_pair(state, x.taxi, first, y.taxi, second);
    }
    return true;
}

// High-throughput, bounded ruin/repair. This intentionally changes search effort:
// repair visits removed requests plus a rotating external shortlist; local search
// probes affected routes instead of exhaustive fleet-wide neighborhoods.
struct RebuildProgress {
    size_t rounds = 0;
};
static RebuildProgress rebuild_progress;
// A completed descent exhausts the existing best-improvement neighborhoods.
// An interrupted final sweep preserves feasible improvements but is not convergence.
// Exchange closed blocks of one or two complete requests between taxis.
// At least one exchanged block contains two requests, and at least one parcel
// participates. Existing request order inside each block is preserved, including
// interleaved parcel pickups/deliveries. No removal-only feasibility gate is used.
struct ClosedBlock {
    int begin = 0, end = 0, requests = 0, parcels = 0, previous = 0, next = 0;
    Segment prefix, body, suffix;
    Real removed_boundary_cost = 0;
};
struct ClosedBlockContext {
    UInt64 version = 0;
    vector<ClosedBlock> blocks;
};
struct ClosedBlockMemo {
    UInt64 first_version = 0, second_version = 0;
    Real gain = 0;
    int first_block = -1, second_block = -1;
};
static vector<ClosedBlockContext> closed_block_contexts;
static vector<ClosedBlockMemo> closed_block_memos;
static const vector<ClosedBlock> &closed_blocks(const SearchState &state, int taxi) {
    if (closed_block_contexts.size() != size_t(instance.taxi_count))
        closed_block_contexts.resize(instance.taxi_count);
    auto &context = closed_block_contexts[taxi];
    if (context.version == state.route_version[taxi])
        return context.blocks;
    const auto &idx = delta_index(state, taxi);
    const auto &route = state.routes[taxi];
    context.blocks.clear();
    for (int begin = 0; begin < int(route.size()); ++begin) {
        for (int end = begin + 2; end <= min<int>(route.size(), begin + 4); end += 2) {
            bool closed = true;
            int parcels = 0;
            for (int p = begin; p < end; ++p) {
                int r = instance.node_request[route[p]];
                if (idx.pickup[r] < begin || idx.delivery[r] >= end) {
                    closed = false;
                    break;
                }
                if (!instance.is_delivery[route[p]] && !instance.requests[r].is_passenger)
                    ++parcels;
            }
            if (!closed)
                continue;
            ClosedBlock block;
            block.begin = begin;
            block.end = end;
            block.requests = (end - begin) / 2;
            block.parcels = parcels;
            block.prefix = segment_range(idx.segments, 0, begin);
            block.body = segment_range(idx.segments, begin, end);
            block.suffix = segment_range(idx.segments, end, route.size());
            block.previous = begin ? route[begin - 1] : instance.taxi_origin[taxi];
            block.next = end < int(route.size()) ? route[end] : 0;
            block.removed_boundary_cost =
                travel_cost(block.previous, block.body.first_node) +
                (block.next ? travel_cost(block.body.last_node, block.next) : 0);
            context.blocks.push_back(block);
        }
    }
    context.version = state.route_version[taxi];
    return context.blocks;
}
static Real closed_block_delta(const ClosedBlock &a, const ClosedBlock &b) {
    // All served revenues and internal block arcs cancel in the fleet sum.
    // Only four boundary arcs change. No modified route is constructed.
    return a.removed_boundary_cost + b.removed_boundary_cost -
           travel_cost(a.previous, b.body.first_node) -
           (a.next ? travel_cost(b.body.last_node, a.next) : 0) -
           travel_cost(b.previous, a.body.first_node) -
           (b.next ? travel_cost(a.body.last_node, b.next) : 0);
}
static bool improve_closed_block_exchange(SearchState &state, Deadline deadline) {
    if (Clock::now() >= deadline)
        return false;

    size_t count = size_t(instance.taxi_count) * instance.taxi_count;
    if (closed_block_memos.size() != count)
        closed_block_memos.resize(count);
    for (int t = 0; t < instance.taxi_count; ++t)
        closed_blocks(state, t);
    Real best = FEASIBILITY_EPSILON;
    int first = -1, second = -1, ai = -1, bi = -1;
    for (int a = 0; a < instance.taxi_count; ++a)
        for (int b = a + 1; b < instance.taxi_count; ++b) {
            auto &memo = closed_block_memos[size_t(a) * instance.taxi_count + b];
            const auto &as = closed_block_contexts[a].blocks;
            const auto &bs = closed_block_contexts[b].blocks;
            if (memo.first_version != state.route_version[a] ||
                memo.second_version != state.route_version[b]) {
                memo = {};
                memo.first_version = state.route_version[a];
                memo.second_version = state.route_version[b];
                memo.gain = FEASIBILITY_EPSILON;
                for (int i = 0; i < int(as.size()); ++i)
                    for (int j = 0; j < int(bs.size()); ++j) {
                        const auto &x = as[i];
                        const auto &y = bs[j];
                        if ((x.requests == 1 && y.requests == 1) || !(x.parcels + y.parcels))
                            continue;

                        Real gain = closed_block_delta(x, y);
                        if (gain <= memo.gain)
                            continue;
                        auto left = join_segments(join_segments(x.prefix, y.body), x.suffix);
                        if (!is_segment_feasible(a, left))
                            continue;
                        auto right = join_segments(join_segments(y.prefix, x.body), y.suffix);
                        if (!is_segment_feasible(b, right))
                            continue;
                        memo.gain = gain;
                        memo.first_block = i;
                        memo.second_block = j;
                    }
            }
            if (memo.first_block >= 0 && memo.gain > best) {
                best = memo.gain;
                first = a;
                second = b;
                ai = memo.first_block;
                bi = memo.second_block;
            }
        }
    if (first < 0)
        return false;
    const auto &a = closed_block_contexts[first].blocks[ai],
               &b = closed_block_contexts[second].blocks[bi];
    const auto &old_a = state.routes[first], &old_b = state.routes[second];
    Route left, right;
    left.reserve(old_a.size() + old_b.size());
    right.reserve(old_a.size() + old_b.size());
    left.insert(left.end(), old_a.begin(), old_a.begin() + a.begin);
    left.insert(left.end(), old_b.begin() + b.begin, old_b.begin() + b.end);
    left.insert(left.end(), old_a.begin() + a.end, old_a.end());
    right.insert(right.end(), old_b.begin(), old_b.begin() + b.begin);
    right.insert(right.end(), old_a.begin() + a.begin, old_a.begin() + a.end);
    right.insert(right.end(), old_b.begin() + b.end, old_b.end());
    auto lv = evaluate_route(instance, first, left), rv = evaluate_route(instance, second, right);
    if (!lv || !rv)
        throw runtime_error("Closed-block feasibility disagreement");
    Real gain = *lv + *rv - state.route_profit[first] - state.route_profit[second];
    if (abs(gain - best) > 1e-6 * max(1.0, abs(best)))
        throw runtime_error("Closed-block delta disagreement");
    if (gain <= FEASIBILITY_EPSILON)
        return false;
    update_route_pair(state, first, left, second, right);
    store_route(first, state.routes[first]);
    store_route(second, state.routes[second]);
    return true;
}

static bool best_neighborhood_descent(SearchState &state, Deadline deadline) {

    while (Clock::now() < deadline) {
        Real before = state.profit;
        improve_solution(state, deadline, true, 1);
        if (Clock::now() < deadline)
            improve_parcel_alns(state, deadline);
        if (Clock::now() >= deadline) {
            return false;
        }
        improve_closed_block_exchange(state, deadline);
        if (state.profit <= before + FEASIBILITY_EPSILON) {
            return true;
        }
    }
    return false;
}
// Greedy acceptance after a complete destroy,repair and local descent.
static void fast_rebuild_fleets(SearchState &incumbent, Deadline deadline) {
    const auto call_id = fleet_generation_calls++;
    random_engine.seed(random_seed + SEED_STRIDE * call_id);
    SearchState current = incumbent, best = incumbent;
    InsertionCache cache;
    IntVector served, repair;
    vector<char> scheduled(instance.request_count, 0);
    size_t round = 0, cursor = random_engine() % max(1, instance.request_count);
    while (true) {
        const auto now = Clock::now();
        if (now >= deadline)
            break;
        SearchState previous = current;

        served.clear();
        for (int r = 0; r < instance.request_count; ++r)
            if (current.request_owner[r] >= 0)
                served.push_back(r);
        repair.clear();
        fill(scheduled.begin(), scheduled.end(), 0);
        auto add = [&](int r) {
            if (!scheduled[r]) {
                scheduled[r] = 1;
                repair.push_back(r);
            }
        };
        int source = -1;
        // Two to five complete requests, with occasional whole-route ruin.
        if (!served.empty()) {
            int anchor = served[random_engine() % served.size()];
            source = current.request_owner[anchor];
            if (round % 16 == 0) {
                auto ids = served_requests(current.routes[source]);
                for (int r : ids)
                    add(r);
                update_route(current, source, {});
            } else {
                int amount = 2 + random_engine() % 4;
                if (round % 4 == 1) {
                    // Related ruin frees requests occupying the same geographic
                    // corridor. Directed endpoint links are evaluated both ways.
                    const auto &anchor_data = instance.requests[anchor];
                    vector<pair<Real, int>> related;
                    related.reserve(served.size());
                    for (int r : served) {
                        const auto &q = instance.requests[r];
                        Real link = min({travel_cost(anchor_data.pickup_node, q.pickup_node),
                                         travel_cost(q.pickup_node, anchor_data.pickup_node),
                                         travel_cost(anchor_data.delivery_node, q.delivery_node),
                                         travel_cost(q.delivery_node, anchor_data.delivery_node)});
                        related.push_back({link, r});
                    }
                    sort(related.begin(), related.end());
                    amount = min<int>(6 + random_engine() % 7, related.size());
                    served.clear();
                    // Retain a small random component inside the related set.
                    for (int i = 0; i < min<int>(amount * 2, related.size()); ++i)
                        served.push_back(related[i].second);
                }
                for (int j = 0; j < amount && !served.empty(); ++j) {
                    size_t pos = random_engine() % served.size();
                    int r = served[pos];
                    served[pos] = served.back();
                    served.pop_back();
                    int taxi = current.request_owner[r];
                    if (taxi < 0)
                        continue;
                    auto route = remove_request(current.routes[taxi], r);
                    if (evaluate_route(instance, taxi, route)) {
                        update_route(current, taxi, route);
                        add(r);
                    }
                }
            }
        }
        // Fixed-work coverage is explicit: every request is visited eventually,
        // but this round evaluates at most eight additional external requests.
        for (int scanned = 0, added = 0; scanned < instance.request_count && added < 8; ++scanned) {
            int r = cursor++ % instance.request_count;
            if (current.request_owner[r] < 0 && !scheduled[r]) {
                add(r);
                ++added;
            }
        }
        shuffle(repair.begin(), repair.end(), random_engine);
        for (int r : repair) {
            if (Clock::now() >= deadline)
                break;
            if (current.request_owner[r] >= 0)
                continue;

            auto choice = find_fleet_insertion(current, r, cache);
            if (choice.taxi >= 0)
                apply_insertion(current, choice);
        }
        bool complete = best_neighborhood_descent(current, deadline);
        if (current.profit > best.profit) {
            best = current;
            store_fleet(best.routes);
        }
        // Preserve a better interrupted proposal,but do not start another round.
        if (!complete)
            break;
        const Real change = current.profit - previous.profit;
        if (!(change >= -FEASIBILITY_EPSILON))
            current = move(previous);
        ++round;
        ++rebuild_progress.rounds;
    }
    incumbent = best;
    store_fleet(incumbent.routes);
}

// Incremental priced repair: one taxi at a time, cached request prices and a
// bounded rotating request scan. Candidate routes may reuse requests assigned
// to other taxis; only the master reconciles cross-taxi conflicts.
struct PricingProgress {
    size_t calls = 0, probes = 0, cursor = 0, taxi_cursor = 0;
};
static PricingProgress pricing_progress;
RealVector fast_request_prices;
static void fast_price_routes(SearchState &state, Deadline deadline) {
    if (Clock::now() >= deadline || !instance.request_count)
        return;
    ++pricing_progress.calls;
    if (fast_request_prices.size() != size_t(instance.request_count))
        fast_request_prices.assign(instance.request_count, 0);
    int taxi = pricing_progress.taxi_cursor++ % instance.taxi_count;
    auto score = [&](const Route &route) {
        Real value = route_profit(taxi, route);
        for (int r : served_requests(route))
            value -= fast_request_prices[r];
        return value;
    };
    Route route = state.routes[taxi];
    Real current_score = score(route);
    // Bank seeds supply alternatives that are unavailable to a feasible-fleet
    // repair. Read map entries before writing a new candidate.
    if (pricing_progress.calls % 3 == 0) {
        const StoredRoute *best = nullptr;
        Real value = -INF;
        for (const auto &entry : route_bank[taxi]) {
            Real reduced = entry.second.profit;
            for (int r : entry.first)
                reduced -= fast_request_prices[r];
            if (reduced > value) {
                value = reduced;
                best = &entry.second;
            }
        }
        if (best) {
            route = unpack_stored_route(best->route);
            current_score = score(route);
        }
    }
    for (int pass = 0; pass < 2 && Clock::now() < deadline; ++pass) {
        auto ids = served_requests(route);
        IntVector candidates;
        vector<char> used(instance.request_count, 0);
        for (int r : ids)
            used[r] = 1;
        for (int scan = 0; scan < instance.request_count && candidates.size() < 24; ++scan) {
            int r = pricing_progress.cursor++ % instance.request_count;
            if (!used[r])
                candidates.push_back(r);
        }
        int removed = ids.empty() ? -1 : ids[random_engine() % ids.size()];
        Route reduced = removed < 0 ? route : remove_request(route, removed);
        const Real removal_score = score(reduced);
        Real best_score = current_score + FEASIBILITY_EPSILON;
        Route chosen;
        for (int r : candidates) {
            if (Clock::now() >= deadline)
                break;
            ++pricing_progress.probes;
            auto insert = find_route_insertion(r, taxi, route,
                                               best_score - current_score + fast_request_prices[r]);
            if (insert.taxi >= 0) {
                Real value = current_score + insert.profit_delta - fast_request_prices[r];
                if (value > best_score) {
                    best_score = value;
                    chosen =
                        insert_request(route, r, insert.pickup_position, insert.delivery_position);
                }
            }
            if (removed >= 0) {
                auto replace = find_route_insertion(
                    r, taxi, reduced, best_score - removal_score + fast_request_prices[r]);
                if (replace.taxi >= 0) {
                    Real value = removal_score + replace.profit_delta - fast_request_prices[r];
                    if (value > best_score) {
                        best_score = value;
                        chosen = insert_request(reduced, r, replace.pickup_position,
                                                replace.delivery_position);
                    }
                }
            }
        }
        if (chosen.empty())
            break;
        const auto value = evaluate_route(instance, taxi, chosen);
        if (!value)
            break;
        route = move(chosen);
        current_score = best_score;
        store_route(taxi, route, false, *value);
    }
}

// Retention is heuristic, never a dominance certificate. Keep the incumbent,
// reserve20% for request coverage, and rank the rest by raw or priced profit.
struct BankRetentionStats {
    size_t before = 0, after = 0, protected_routes = 0, coverage_routes = 0;
};
static BankRetentionStats retain_bank(const SearchState &state, const RealVector &prices,
                                      size_t limit, bool priced, bool diversity) {
    BankRetentionStats stats;
    for (int taxi = 0; taxi < instance.taxi_count; ++taxi) {
        auto &bank = route_bank[taxi];
        stats.before += bank.size();
        if (bank.size() <= limit) {
            stats.after += bank.size();
            continue;
        }
        struct Row {
            RouteBank::iterator entry;
            Real score;
            bool keep = false;
        };
        vector<Row> rows;
        rows.reserve(bank.size());
        IntVector incumbent = served_requests(state.routes[taxi]);
        size_t kept = 0;
        IntVector coverage(instance.request_count, 0), occurrence(instance.request_count, 0);
        for (auto it = bank.begin(); it != bank.end(); ++it) {
            Real score = it->second.profit;
            for (int r : it->first) {
                ++occurrence[r];
                if (priced)
                    score -= prices[r];
            }
            rows.push_back({it, score, false});
        }
        auto keep = [&](Row &row) {
            if (row.keep)
                return;
            row.keep = true;
            ++kept;
            for (int r : row.entry->first)
                ++coverage[r];
        };
        for (auto &row : rows)
            if (row.entry->first == incumbent) {
                keep(row);
                ++stats.protected_routes;
                break;
            }
        auto better = [&](const Row &a, const Row &b) {
            if (a.score != b.score)
                return a.score > b.score;
            if (a.entry->second.profit != b.entry->second.profit)
                return a.entry->second.profit > b.entry->second.profit;
            return a.entry->first < b.entry->first;
        };
        stable_sort(rows.begin(), rows.end(), better);
        const size_t reserve = diversity ? limit / 5 : 0, primary = limit - reserve;
        for (auto &row : rows)
            if (kept < primary)
                keep(row);
        // A bounded coverage reserve favors request sets underrepresented in
        // primary routes. Rarity breaks ties; primary ranks remain deterministic.
        struct Alternative {
            size_t row;
            Real coverage, rarity;
        };
        vector<Alternative> alternatives;
        for (size_t j = 0; j < rows.size(); ++j)
            if (!rows[j].keep) {
                Real value = 0, rarity = 0;
                for (int r : rows[j].entry->first) {
                    value += 1.0 / (1 + coverage[r]);
                    rarity += 1.0 / occurrence[r];
                }
                if (!rows[j].entry->first.empty())
                    value /= rows[j].entry->first.size();
                alternatives.push_back({j, value, rarity});
            }
        stable_sort(alternatives.begin(), alternatives.end(), [&](const auto &a, const auto &b) {
            if (a.coverage != b.coverage)
                return a.coverage > b.coverage;
            if (a.rarity != b.rarity)
                return a.rarity > b.rarity;
            return better(rows[a.row], rows[b.row]);
        });
        for (auto &candidate : alternatives)
            if (kept < limit) {
                keep(rows[candidate.row]);
                ++stats.coverage_routes;
            }
        for (auto &row : rows)
            if (!row.keep)
                bank.erase(row.entry);
        stats.after += bank.size();
        bank_profit_floor[taxi] = -INF;
    }
    // A pruned bank is never complete, even if generation once was exhaustive.
    if (stats.after < stats.before)
        route_bank_complete = false;
    return stats;
}

// --- Driver: generation,greedy rebuilding,bank selection,pricing and polish ---
int main(int argc, char **argv) {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    const auto start = Clock::now();
    if (argc > 1)
        random_seed = strtoull(argv[1], nullptr, 10);
    random_engine.seed(random_seed);
    instance = read_instance();
    // Include input time;reserve two seconds for an in-flight scan and output.
    search_deadline = start + Seconds(299) - Seconds(2);
    route_bank.resize(instance.taxi_count);
    bank_profit_floor.assign(instance.taxi_count, -INF);
    if (instance.taxi_count > LARGE_FLEET_TAXI_LIMIT)
        bank_route_limit = LARGE_FLEET_BANK_LIMIT;
    SearchState state = make_search_state(Fleet(instance.taxi_count));

    // Probe completeness,then build the ordinary bounded bank when necessary.
    full_probe_mode = true;
    bool complete = enumerate_routes(min(search_deadline, deadline_after(1)));
    full_probe_mode = false;
    route_bank_complete = complete && !full_probe_truncated;
    if (!route_bank_complete) {
        route_bank = vector<RouteBank>(instance.taxi_count);
        bank_profit_floor.assign(instance.taxi_count, -INF);
        dfs_generation_calls = 0;
        enumerate_routes(min(search_deadline, deadline_after(20)));
    }

    // Preserve eight seconds for the final full-bank solve and route refinement.
    const auto loop_deadline = search_deadline - Seconds(8);
    rebuild_fleets(state, min(loop_deadline, deadline_after(10)));
    size_t cycles = 0;
    while (Clock::now() < loop_deadline) {
        const size_t old_rounds = rebuild_progress.rounds;
        fast_rebuild_fleets(state, min(loop_deadline, deadline_after(.25)));
        if (cycles && cycles % 16 == 0 && Clock::now() < loop_deadline) {
            rebuild_fleets(state, min(loop_deadline, deadline_after(.5)));
            improve_solution(state, min(loop_deadline, deadline_after(.2)), true, 1);
        }
        if (Clock::now() >= loop_deadline)
            break;

        // Bound bank storage while retaining incumbent and coverage alternatives.
        size_t columns = 0;
        for (const auto &bank : route_bank)
            columns += bank.size();
        if (columns > 2 * size_t(bank_route_limit) * instance.taxi_count) {
            if (fast_request_prices.empty())
                fast_request_prices.assign(instance.request_count, 0);
            retain_bank(state, fast_request_prices, bank_route_limit, true, true);
        }
        select_fleet_routes(state, min(loop_deadline, deadline_after(.30)));
        if (Clock::now() >= loop_deadline)
            break;

        // Refresh opportunity costs periodically,and grow independent alternatives.
        if (cycles % 16 == 0) {
            auto prices =
                estimate_request_prices(state.profit, min(loop_deadline, deadline_after(.04)));
            if (any_of(prices.begin(), prices.end(), [](Real p) { return p > 0; }))
                fast_request_prices = move(prices);
        }
        const size_t old_probes = pricing_progress.probes;
        fast_price_routes(state, min(loop_deadline, deadline_after(.10)));
        if (cycles % 8 == 0 && Clock::now() < loop_deadline)
            generate_priced_replacements(state, min(loop_deadline, deadline_after(.30)));
        if (rebuild_progress.rounds > old_rounds && pricing_progress.probes > old_probes)
            ++cycles;
    }

    // Reconcile the full bank,then refine parcel deliveries and the complete fleet.
    select_fleet_routes(state, min(search_deadline, deadline_after(3)), true);
    refine_parcel_deliveries(state, min(search_deadline, deadline_after(2)));
    improve_solution(state, search_deadline, true, FINAL_POLISH_CYCLES);
    cout << setprecision(17) << state.profit << '\n';
    for (const auto &route : state.routes) {
        cout << route.size();
        for (int node : route)
            cout << ' ' << node;
        cout << '\n';
    }
    return 0;
}
