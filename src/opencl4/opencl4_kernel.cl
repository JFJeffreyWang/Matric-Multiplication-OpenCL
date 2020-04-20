 __kernel void matMlp(
      __global const float4 * matA,
      __global const float4 * matB,
      __global float4 * matC,
     const int W,
     __local float4 * localA
     )
{
    const int c=get_global_id(0);
    const int rr=get_global_id(1);
    const int r=rr*4;

    localA[rr] = matA[c*W/4+rr];
    barrier(CLK_LOCAL_MEM_FENCE);
    
    float4 t;
    t=0.0f;
     for(int i=0; i<W/4; i++){
       t += localA[i].x * matB[i*W/4 + rr];
       t += localA[i].y * matB[(i+1)*W/4 + rr];
       t += localA[i].z * matB[(i+2)*W/4 + rr];
       t += localA[i].w * matB[(i+3)*W/4 + rr];
    }

	matC[c*W/4+rr] = t;


}