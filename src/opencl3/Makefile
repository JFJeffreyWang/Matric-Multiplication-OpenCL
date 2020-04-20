OPENCL = -I/usr/local/include -L/usr/local/cuda/lib64 -lOpenCL

all: build
build: opencl3

opencl3: opencl3.cpp opencl3_opencl.h 
	g++ -o $@ $< $(OPENCL)
	cp $@ ../../

clean:
	@rm -f opencl3
	@rm -f ../../opencl3