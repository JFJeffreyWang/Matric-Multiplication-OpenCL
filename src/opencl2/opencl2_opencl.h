#define CL_USE_DEPRECATED_OPENCL_1_2_APIS
#define CL_TARGET_OPENCL_VERSION 220
#define LW 2
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <fstream>
#include <stdlib.h>
#include <CL/cl.h> 
using namespace std;
void checkError(cl_int error, int line);

long file_length(char filename[])
{
	FILE *p=fopen(filename,"r");
	if(p==NULL)
	{
		cout <<filename<<" is not exist!"<<endl;
		return -1;
	}
	fseek(p,0,SEEK_END);
	const long length=ftell(p);
	fclose(p);
	return length;
}

void read_kernel(char filename[],char *source,long length)
{
	FILE *p=fopen(filename,"rb");
	int t=fread(source,1,length,p);
	source[t]='\0';
	fclose(p);
}


double matMlp(char* kernel_name,float *matA,float *matB, float * matC, int W)
{
	cl_uint status;
    cl_platform_id platform;

    //创建平台对象
    status = clGetPlatformIDs(1, &platform, NULL);
    cl_device_id device;
    //创建 GPU 设备
    clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU,1,&device,NULL);
	
	//创建context
	cl_context context = clCreateContext(NULL,1,&device,NULL, NULL, NULL);
	
	//创建命令队列
	cl_command_queue commandQueue = clCreateCommandQueue(context, device,CL_QUEUE_PROFILING_ENABLE, NULL);
	if (commandQueue == NULL) 
		perror("Failed to create commandQueue for device 0.");

	//创建内存对象
	cl_mem memObj[3] = { 0, 0, 0};
	memObj[0]=clCreateBuffer(context,CL_MEM_READ_ONLY|CL_MEM_COPY_HOST_PTR,sizeof(float)*(W*W),matA,NULL);
		
	memObj[1]=clCreateBuffer(context,CL_MEM_READ_ONLY|CL_MEM_COPY_HOST_PTR,sizeof(float)*W*W,matB,NULL);

	memObj[2]=clCreateBuffer(context,CL_MEM_READ_WRITE,sizeof(float)*(W*W),matC,NULL);

		
	if (memObj[0]== NULL||memObj[1]==NULL||memObj[2]==NULL) 
		perror("创建内存对象错误\n");

	
	//创建program
	long length=file_length(kernel_name);
	if(length==-1)
		return 0;
	char *source=(char*)malloc(length);
	read_kernel(kernel_name,source,length);

	size_t sourceSize[] = { (size_t)length };

	cl_program program = clCreateProgramWithSource(context,1,(const char **)&source,sourceSize,NULL);
	
	//编译kernel函数
	status = clBuildProgram(program, 1, &device, NULL, NULL, NULL);
	if (status != 0)
	{
		printf("编译kernel函数失败 :%d\n", status);
		char tbuf[0x10000];
		clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, 0x10000, tbuf,NULL);
		printf("\n%s\n", tbuf);
	}
	
	//创建kernel对象
	cl_kernel kernel = clCreateKernel(program, "matMlp", NULL);
	
	//设置内核函数参数
	status = clSetKernelArg(kernel,0,sizeof(cl_mem),&memObj[0]);
	status|= clSetKernelArg(kernel,1,sizeof(cl_mem),&memObj[1]);
	status|= clSetKernelArg(kernel,2,sizeof(cl_mem),&memObj[2]);
	status|= clSetKernelArg(kernel,3,sizeof(int),&W);
	status|= clSetKernelArg(kernel,4,sizeof(float)*W,NULL);
	if (status)
	{
		cout << "设置内核函数参数错误 ！" << endl;
		checkError(status,__LINE__);
	}	
	
	size_t global[2];
	cl_event run_event;
	cl_ulong ev_start_time = (cl_ulong)0;
	cl_ulong ev_end_time = (cl_ulong)0;
	double run_time;
	
	global[0] = (size_t)((int)W);
	global[1] = (size_t)((int)W);
	size_t local[2]={(size_t)1,(size_t)W};
	
	//执行内核
	status = clEnqueueNDRangeKernel(commandQueue, kernel, 2, NULL, global, local, 0, NULL, &run_event);
	if (status)
	{
		cout << "执行内核错误！" << endl;
		checkError(status,__LINE__);
	}
	
	//等待执行完成
	clWaitForEvents(1, &run_event);

	//读取执行时间
	status = clGetEventProfilingInfo(run_event,CL_PROFILING_COMMAND_QUEUED,
				sizeof(cl_ulong),&ev_start_time,NULL);
	status |= clGetEventProfilingInfo(run_event,CL_PROFILING_COMMAND_END,
				sizeof(cl_ulong),&ev_end_time,NULL);
	if (status) 
	{
		perror("读取执行时间错误！\n");
		checkError(status,__LINE__);
	}	
	run_time = (double)(ev_end_time - ev_start_time);

	//读取结果数据
	status = clEnqueueReadBuffer(commandQueue, memObj[2],CL_TRUE, 0,
			sizeof(int)*W*W,matC,0, NULL, NULL);
	if (status) 
	{
		perror("读取结果数据错误！\n");
		checkError(status,__LINE__);
	}	

	//释放申请的空间
	clReleaseMemObject(memObj[1]);
	clReleaseMemObject(memObj[0]);
	clReleaseMemObject(memObj[2]);
	clReleaseProgram(program);
	clReleaseCommandQueue(commandQueue);
	clReleaseContext(context);
	
	
	//printf("GPU Cost time: %lf ms\n", run_time/1000000.0);
	return run_time/1000000.0;
}

void checkError(cl_int error, int line) 
{
    if (error != CL_SUCCESS) {
        switch (error) {
		case CL_DEVICE_NOT_FOUND:                 
		printf("-- Error at %d:  Device not found.\n", line); break;
		case CL_DEVICE_NOT_AVAILABLE:            
		printf("-- Error at %d:  Device not available\n", line); break;
		case CL_COMPILER_NOT_AVAILABLE:           
		printf("-- Error at %d:  Compiler not available\n", line); break;
		case CL_MEM_OBJECT_ALLOCATION_FAILURE:    
		printf("-- Error at %d:  Memory object allocation failure\n", line); break;
		case CL_OUT_OF_RESOURCES:                 
		printf("-- Error at %d:  Out of resources\n", line); break;
		case CL_OUT_OF_HOST_MEMORY:              
		printf("-- Error at %d:  Out of host memory\n", line); break;
		case CL_PROFILING_INFO_NOT_AVAILABLE:     
		printf("-- Error at %d:  Profiling information not available\n", line); break;
		case CL_MEM_COPY_OVERLAP:                 
		printf("-- Error at %d:  Memory copy overlap\n", line); break;
		case CL_IMAGE_FORMAT_MISMATCH:            
		printf("-- Error at %d:  Image format mismatch\n", line); break;
		case CL_IMAGE_FORMAT_NOT_SUPPORTED:       
		printf("-- Error at %d:  Image format not supported\n", line); break;
		case CL_BUILD_PROGRAM_FAILURE:            
		printf("-- Error at %d:  Program build failure\n", line); break;
		case CL_MAP_FAILURE:                      
		printf("-- Error at %d:  Map failure\n", line); break;
		case CL_INVALID_VALUE:                    
		printf("-- Error at %d:  Invalid value\n", line); break;
		case CL_INVALID_DEVICE_TYPE:              
		printf("-- Error at %d:  Invalid device type\n", line); break;
		case CL_INVALID_PLATFORM:                 
		printf("-- Error at %d:  Invalid platform\n", line); break;
		case CL_INVALID_DEVICE:                   
		printf("-- Error at %d:  Invalid device\n", line); break;
		case CL_INVALID_CONTEXT:                  
		printf("-- Error at %d:  Invalid context\n", line); break;
		case CL_INVALID_QUEUE_PROPERTIES:         
		printf("-- Error at %d:  Invalid queue properties\n", line); break;
		case CL_INVALID_COMMAND_QUEUE:            
		printf("-- Error at %d:  Invalid command queue\n", line); break;
		case CL_INVALID_HOST_PTR:                 printf("-- Error at %d:  Invalid host pointer\n", line); break;
		case CL_INVALID_MEM_OBJECT:               printf("-- Error at %d:  Invalid memory object\n", line); break;
		case CL_INVALID_IMAGE_FORMAT_DESCRIPTOR:  printf("-- Error at %d:  Invalid image format descriptor\n", line); break;
		case CL_INVALID_IMAGE_SIZE:               printf("-- Error at %d:  Invalid image size\n", line); break;
		case CL_INVALID_SAMPLER:                  printf("-- Error at %d:  Invalid sampler\n", line); break;
		case CL_INVALID_BINARY:                   printf("-- Error at %d:  Invalid binary\n", line); break;
		case CL_INVALID_BUILD_OPTIONS:            printf("-- Error at %d:  Invalid build options\n", line); break;
		case CL_INVALID_PROGRAM:                  printf("-- Error at %d:  Invalid program\n", line); break;
		case CL_INVALID_PROGRAM_EXECUTABLE:       printf("-- Error at %d:  Invalid program executable\n", line); break;
		case CL_INVALID_KERNEL_NAME:              printf("-- Error at %d:  Invalid kernel name\n", line); break;
		case CL_INVALID_KERNEL_DEFINITION:        printf("-- Error at %d:  Invalid kernel definition\n", line); break;
		case CL_INVALID_KERNEL:                   printf("-- Error at %d:  Invalid kernel\n", line); break;
		case CL_INVALID_ARG_INDEX:                printf("-- Error at %d:  Invalid argument index\n", line); break;
		case CL_INVALID_ARG_VALUE:                printf("-- Error at %d:  Invalid argument value\n", line); break;
		case CL_INVALID_ARG_SIZE:                 printf("-- Error at %d:  Invalid argument size\n", line); break;
		case CL_INVALID_KERNEL_ARGS:              printf("-- Error at %d:  Invalid kernel arguments\n", line); break;
		case CL_INVALID_WORK_DIMENSION:           printf("-- Error at %d:  Invalid work dimensionsension\n", line); break;
		case CL_INVALID_WORK_GROUP_SIZE:          printf("-- Error at %d:  Invalid work group size\n", line); break;
		case CL_INVALID_WORK_ITEM_SIZE:           printf("-- Error at %d:  Invalid work item size\n", line); break;
		case CL_INVALID_GLOBAL_OFFSET:            printf("-- Error at %d:  Invalid global offset\n", line); break;
		case CL_INVALID_EVENT_WAIT_LIST:          printf("-- Error at %d:  Invalid event wait list\n", line); break;
		case CL_INVALID_EVENT:                    printf("-- Error at %d:  Invalid event\n", line); break;
		case CL_INVALID_OPERATION:                printf("-- Error at %d:  Invalid operation\n", line); break;
		case CL_INVALID_GL_OBJECT:                printf("-- Error at %d:  Invalid OpenGL object\n", line); break;
		case CL_INVALID_BUFFER_SIZE:              printf("-- Error at %d:  Invalid buffer size\n", line); break;
		case CL_INVALID_MIP_LEVEL:                printf("-- Error at %d:  Invalid mip-map level\n", line); break;
		case -1024:                               printf("-- Error at %d:  *clBLAS* Functionality is not implemented\n", line); break;
		case -1023:                               printf("-- Error at %d:  *clBLAS* Library is not initialized yet\n", line); break;
		case -1022:                               printf("-- Error at %d:  *clBLAS* Matrix A is not a valid memory object\n", line); break;
		case -1021:                               printf("-- Error at %d:  *clBLAS* Matrix B is not a valid memory object\n", line); break;
		case -1020:                               printf("-- Error at %d:  *clBLAS* Matrix C is not a valid memory object\n", line); break;
		case -1019:                               printf("-- Error at %d:  *clBLAS* Vector X is not a valid memory object\n", line); break;
		case -1018:                               printf("-- Error at %d:  *clBLAS* Vector Y is not a valid memory object\n", line); break;
		case -1017:                               printf("-- Error at %d:  *clBLAS* An input dimension (M,N,K) is invalid\n", line); break;
		case -1016:                               printf("-- Error at %d:  *clBLAS* Leading dimension A must not be less than the size of the first dimension\n", line); break;
		case -1015:                               printf("-- Error at %d:  *clBLAS* Leading dimension B must not be less than the size of the second dimension\n", line); break;
		case -1014:                               printf("-- Error at %d:  *clBLAS* Leading dimension C must not be less than the size of the third dimension\n", line); break;
		case -1013:                               printf("-- Error at %d:  *clBLAS* The increment for a vector X must not be 0\n", line); break;
		case -1012:                               printf("-- Error at %d:  *clBLAS* The increment for a vector Y must not be 0\n", line); break;
		case -1011:                               printf("-- Error at %d:  *clBLAS* The memory object for Matrix A is too small\n", line); break;
		case -1010:                               printf("-- Error at %d:  *clBLAS* The memory object for Matrix B is too small\n", line); break;
		case -1009:                               printf("-- Error at %d:  *clBLAS* The memory object for Matrix C is too small\n", line); break;
		case -1008:                               printf("-- Error at %d:  *clBLAS* The memory object for Vector X is too small\n", line); break;
		case -1007:                               printf("-- Error at %d:  *clBLAS* The memory object for Vector Y is too small\n", line); break;
		case -1001:                               printf("-- Error at %d:  Code -1001: no GPU available?\n", line); break;
		default:                                  printf("-- Error at %d:  Unknown with code %d\n", line, error);
        }
        exit(1);
    }
}
