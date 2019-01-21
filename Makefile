libapdu.a: build/Makefile
	$(MAKE) -C build

clean: build
	$(MAKE) -C build clean

build/Makefile: CMakeLists.txt
	mkdir -p build
	cd build && cmake ..
