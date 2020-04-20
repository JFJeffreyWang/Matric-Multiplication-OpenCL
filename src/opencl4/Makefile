OPENCL = -I/usr/local/include -L/usr/local/cuda/lib64 -lOpenCL

all: build
build: opencl4

opencl4: opencl4.cpp opencl4_opencl.h 
	g++ -o $@ $< $(OPENCL)
	cp $@ ../../

clean:
	@rm -f opencl4
	@rm -f ../../opencl4