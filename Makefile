
PROJECTS := opencl1 opencl2 opencl3 opencl4
BUILDS :=  $(addsuffix .build, $(PROJECTS))
CLEANS := $(addsuffix .clean, $(PROJECTS))
all: $(BUILDS)
clean: $(CLEANS) output.clean

$(BUILDS):%.build : 
	@$(MAKE) -C src/$*

$(CLEANS):%.clean : 
	@$(MAKE) -C src/$* clean

 output.clean:
	@rm -f result.out