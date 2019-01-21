libapdu.a: build
	$(MAKE) -C build all

clean: build
	$(MAKE) -C build clean

build: CMakeLists.txt
	mkdir -p build
	cd build && cmake ..
