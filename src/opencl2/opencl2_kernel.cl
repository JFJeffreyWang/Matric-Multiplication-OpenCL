 __kernel void matMlp(
      __global const float * matA,
      __global const float * matB,
      __global float * matC,
     const int W,
     __local float* localB
     )
{
    const int c=get_global_id(0);
    const int r=get_global_id(1);
    localB[r] = matB[c*W+r];
    barrier(CLK_LOCAL_MEM_FENCE);

	float total = 0;
     for(int i=0; i<W; i++){
		total += matA[c*W+i] * localB[r];
    }
	matC[c*W+r] = total;
}