#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#define PRECISTION 4
const float EXP2_LUT_FLOAT[23] = {
    1.41421356f,  // [0]: 2^(1/2)
    1.18920711f,  // [1]: 2^(1/4)
    1.09050773f,  // [2]: 2^(1/8)
    1.04427378f,  // [3]: 2^(1/16)
    1.02189714f,  // [4]: 2^(1/32)
    1.01088928f,  // [5]: 2^(1/64)
    1.00542990f,  // [6]: 2^(1/128)
    1.00271127f,  // [7]: 2^(1/256)
    1.00135471f,  // [8]: 2^(1/512)
    1.00067713f,  // [9]: 2^(1/1024)
    1.00033850f,  // [10]: 2^(1/2048)
    1.00016923f,  // [11]: 2^(1/4096)
    1.00008461f,  // [12]: 2^(1/8192)
    1.00004230f,  // [13]: 2^(1/16384)
    1.00002115f,  // [14]: 2^(1/32768)
    1.00001057f,  // [15]: 2^(1/65536)
    1.00000528f,  // [16]: 2^(1/131072)
    1.00000264f,  // [17]: 2^(1/262144)
    1.00000132f,  // [18]: 2^(1/524288)
    1.00000066f,  // [19]: 2^(1/1048576)
    1.00000033f,  // [20]: 2^(1/2097152)
    1.00000016f,  // [21]: 2^(1/4194304)
    1.00000008f   // [22]: 2^(1/8388608)
};
int log2floor(int N){
    int BITS = 31;
    while (BITS) {
    if (N & 0x80000000) break;
    N <<= 1;
    BITS--;
    }
    return BITS;
}
float mylog2(int N){
    int e=log2floor(N);
    float m=(float)N/(1<<e),sum=0.f,sum_iter=0.5;
    int iter=PRECISTION,b1;
    while(iter--){
        m=m*m;
        if(m>=2){
            //b1 is 1 move b1.b2b3b4 to 0.b2b3b4
            b1=1;
            m/=2;// 1.b2b3b4 -1
            sum+=sum_iter;
        }
        else{
            b1=0;
        }
        sum_iter*=0.5;
    }
    return (float)e + sum;
}
float mypow2(float x) {
    int I = (int)x;
    float F = x - (float)I;
    float result = 1.0f;
    uint32_t fraction_bits = (uint32_t)(F * 8388608.0f); //mul 2^23
    for (int i = 0; i < 23; i++) {
        if (fraction_bits & (1 << (22 - i))) {
            result *= EXP2_LUT_FLOAT[i]; 
        }
    }
    result*=(1<<I);
    return result;
}
float geomean(int *inputs, int num){
    float logsum=0.f;
    for(int i=0;i<num;i++){
        logsum+=mylog2(inputs[i]);
    }
    logsum/=num;
    return mypow2(logsum);
}


int main(){
    int arr[5]={1,2,3,4,5};
    printf("geomean : %.4f\n",geomean(arr,5));
    return 0;
}
