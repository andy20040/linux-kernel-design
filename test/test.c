#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#define PRECISTION 48
static const double EXP2_LUT[48] = {
    1.4142135623730951,  // [0]
    1.1892071150027210,  // [1]
    1.0905077326652577,  // [2]
    1.0442737824274138,  // [3]
    1.0218971486541166,  // [4]
    1.0108892860517005,  // [5]
    1.0054299011128027,  // [6]
    1.0027112750502025,  // [7]
    1.0013547198921082,  // [8]
    1.0006771306930664,  // [9]
    1.0003385080526823,  // [10]
    1.0001692397053021,  // [11]
    1.0000846162726944,  // [12]
    1.0000423072413958,  // [13]
    1.0000211533969647,  // [14]
    1.0000105766425498,  // [15]
    1.0000052883072919,  // [16]
    1.0000026441501502,  // [17]
    1.0000013220742012,  // [18]
    1.0000006610368821,  // [19]
    1.0000003305183864,  // [20]
    1.0000001652591795,  // [21]
    1.0000000826295863,  // [22]
    1.0000000413147923,  // [23]
    1.0000000206573960,  // [24]
    1.0000000103286979,  // [25]
    1.0000000051643489,  // [26]
    1.0000000025821745,  // [27]
    1.0000000012910872,  // [28]
    1.0000000006455436,  // [29]
    1.0000000003227718,  // [30]
    1.0000000001613858,  // [31]
    1.0000000000806930,  // [32]
    1.0000000000403464,  // [33]
    1.0000000000201732,  // [34]
    1.0000000000100866,  // [35]
    1.0000000000050433,  // [36]
    1.0000000000025218,  // [37]
    1.0000000000012608,  // [38]
    1.0000000000006304,  // [39]
    1.0000000000003152,  // [40]
    1.0000000000001576,  // [41]
    1.0000000000000788,  // [42]
    1.0000000000000394,  // [43]
    1.0000000000000197,  // [44]
    1.0000000000000099,  // [45]
    1.0000000000000049,  // [46]
    1.0000000000000025,  // [47]
};
int log2floor(uint32_t N){
    int BITS = 31;
    while (BITS) {
    if (N & 0x80000000U) break;
    N <<= 1;
    BITS--;
    }
    return BITS;
}
double mylog2(int N){
    int e=log2floor((uint32_t)N);
    double m=(double)N / (double)(1ULL << e);
    double sum=0.0, sum_iter=0.5;
    int iter=PRECISTION;
    while(iter--){
        m=m*m;
        if(m>=2.0){
            m/=2.0;
            sum+=sum_iter;
        }
        sum_iter*=0.5;
    }
    return (double)e + sum;
}
double mypow2(double x) {
    int I = (int)x;
    double F = x - (double)I;
    double result = 1.0;
    uint64_t fraction_bits = (uint64_t)(F * (double)(1ULL << PRECISTION));
    for (int i = 0; i < PRECISTION; i++) {
        if (fraction_bits & (1ULL << (PRECISTION - 1 - i))) {
            result *= EXP2_LUT[i];
        }
    }
    result *= (double)(1ULL << I);
    return result;
}
double geomean(int *inputs, int num){
    int all_same = 1;
    for (int i = 1; i < num; i++) {
        if (inputs[i] != inputs[0]) {
            all_same = 0;
            break;
        }
    }
    if (all_same)
        return (double)inputs[0];
    double logsum=0.0;
    for(int i=0;i<num;i++){
        logsum+=mylog2(inputs[i]);
    }
    logsum/=num;
    return mypow2(logsum);
}


int main(){
    int arraynum;
    printf("input array size:\n");
    scanf("%d",&arraynum);
    int arr[arraynum];
    printf("input array element:\n");
    for (int i = 0; i < arraynum; i++)
    {
        scanf("%d",&arr[i]);
        if(arr[i]==0){
            printf("geomean : 0.000000\n");
            exit(1);
        }
    }
    printf("geomean : %.6f\n", geomean(arr, arraynum));
    return 0;
}
