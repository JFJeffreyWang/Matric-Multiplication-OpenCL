OPENCL = -I/usr/local/include -L/usr/local/cuda/lib64 -lOpenCL

all: build
build: opencl2

opencl2: opencl2.cpp opencl2_opencl.h 
	g++ -o $@ $< $(OPENCL)
	cp $@ ../../

clean:
	@rm -f opencl2
	@rm -f ../../opencl2