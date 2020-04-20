OPENCL = -I/usr/local/include -L/usr/local/cuda/lib64 -lOpenCL

all: build
build: opencl1

opencl1: opencl1.cpp opencl1_opencl.h 
	g++ -o $@ $< $(OPENCL)
	cp $@ ../../

clean:
	@rm -f opencl1
	@rm -f ../../opencl1