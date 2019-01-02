LIBRARY = apdu
LIBRARY_FILE = build/lib$(LIBRARY).a
OBJECT_FILES = src/apdu.o
TEST_FILES = src/test.o
CXXFLAGS = -std=c++11 -Wall

all: $(LIBRARY_FILE)

$(LIBRARY_FILE): $(OBJECT_FILES)
	mkdir -p build
	ar rvs $(LIBRARY_FILE) $(OBJECT_FILES)

%.o: %.cpp
	g++ $(CXXFLAGS) -c -o $@ $^

test: $(TEST_FILES) $(LIBRARY_FILE)
	g++ -o build/test $^
	build/test

clean:
	rm -rf build/ src/*.o
