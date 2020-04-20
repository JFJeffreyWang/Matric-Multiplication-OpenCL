#include <iostream>
#include <iostream>
#include <cstring>
#include <string.h>
#include <sys/time.h>
#include <cstdio>
#include <libgen.h>
#include "opencl2_opencl.h"
using namespace std;

char kernel_name[200] = "src/opencl2/opencl2_kernel.cl";
struct timeval tsBegin,tsEnd;
int W, times;
float * matA;
float * matB;
float * matC;

int run(){
    double t=0;
    double whole_time=0;
    for(int i=0;i<times;i++)
	{
		struct timeval tsBegin,tsEnd;
		long using_time;
		gettimeofday(&tsBegin,NULL);
		
		t+=matMlp(kernel_name, matA, matB, matC, W);
		
		gettimeofday(&tsEnd,NULL);
		using_time=1000000L*(tsEnd.tv_sec-tsBegin.tv_sec)+(tsEnd.tv_usec-tsBegin.tv_usec);
		whole_time+=(using_time/1000.0);	
	}	
	printf("Avg Calc time : %lf ms\n", t/times);
	printf("Avg Total time: %lf ms\n\n", whole_time/times);

  
}
int main(int argc, char* argv[])
{
 

    if(argc<3) cout<<"参数不足"<<endl;
    sscanf(argv[1], "%d", &W);
	sscanf(argv[2], "%d", &times);
	matA = (float*)malloc(sizeof(float)*W*W);
	matB = (float*)malloc(sizeof(float)*W*W);
	matC = (float*)malloc(sizeof(float)*W*W);
	for(int i=0; i<W*W; i++){
		matA[i] = (float)(1.0);
		matB[i] = (float)(1.0);
	}
    cout<<"opencl2 : Width*Height= "<<W<<"*"<<W<<endl;
    run();
	for(int i=0; i<W*W; i++){
		if(matC[i]-W > 1e-5){
			cout<<"output error!!!!"<<endl;
		}
	}
    return 0;

}

