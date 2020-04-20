 __kernel void matMlp(
      __global const float * matA,
      __global const float * matB,
      __global float * matC,
     const int W
     )
{
    const int c=get_global_id(0);
    const int r=get_global_id(1);
	
	float total = 0;
     for(int i=0; i<W; i++){
		total += matA[c*W+i] * matB[i*W+r];
    }
	matC[c*W+r] = total;
}