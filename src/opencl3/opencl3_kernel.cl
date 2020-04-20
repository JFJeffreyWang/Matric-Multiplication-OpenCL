 __kernel void matMlp(
      __global const float * matA,
      __global const float * matB,
      __global float * matC,
     const int W,
     __local float * localA
     )
{
    const int c=get_global_id(0);
    const int rr=get_global_id(1);
    const int r=rr*4;

    int idx = c*W+r;
    localA[r] = matA[idx];
    localA[r+1] = matA[idx+1];
    localA[r+2] = matA[idx+2];
    localA[r+3] = matA[idx+3];
    barrier(CLK_LOCAL_MEM_FENCE);

    float t0 = 0;
	float t1 = 0;
    float t2 = 0;
    float t3 = 0;
    
     for(int i=0; i<W; i++){
        t0 += localA[i] * matB[i*W+r];
		t1 += localA[i] * matB[i*W+r+1];
        t2 += localA[i] * matB[i*W+r+2];
        t3 += localA[i] * matB[i*W+r+3];
    }

	matC[idx] = t0;
    matC[idx+1] = t1;
    matC[idx+2] = t2;
    matC[idx+3] = t3;

}