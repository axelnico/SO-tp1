.POSIX:
.SUFFIXES:
.SUFFIXES: .o .cpp

CXX = g++
CXXFLAGS = -std=c++11
LDLIBS = -lpthread

.cpp.o:
	$(CXX) $(CXXFLAGS) -c $<

BIN = test-1 test-2 test-3 test-5 max-compare
OBJ = ConcurrentHashMap.o

all: $(BIN)

$(BIN): ListaAtomica.hpp

test-1: $(OBJ) test-1.cpp
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ test-1.cpp $(OBJ) $(LDLIBS)

test-1-run: test-1
	./test-1

test-2: $(OBJ) test-2.cpp
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ test-2.cpp $(OBJ) $(LDLIBS)
	
test-2-run: test-2
	awk -f corpus.awk corpus | sort >corpus-post
	./test-2 | sort | diff -u - corpus-post
	rm -f corpus-post

test-3: $(OBJ) test-3.cpp
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ test-3.cpp $(OBJ) $(LDLIBS)

test-3-run: test-3
	awk -f corpus.awk corpus | sort >corpus-post
	for i in 0 1 2 3 4; do sed -n "$$((i * 500 + 1)),$$(((i + 1) * 500))p" corpus >corpus-"$$i"; done
	for i in 0 1 2 3 4; do ./test-3 $$((i + 1)) | sort | diff -u - corpus-post; done
	rm -f corpus-post corpus-[0-4]

test-5: $(OBJ) test-5.cpp
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ test-5.cpp $(OBJ) $(LDLIBS)

test-5-run: test-5
	awk -f corpus.awk corpus | sort -nk 2 | tail -n 1 >corpus-max
	cat corpus-max
	for i in 0 1 2 3 4; do sed -n "$$((i * 500 + 1)),$$(((i + 1) * 500))p" corpus >corpus-"$$i"; done
	for i in 0 1 2 3 4; do for j in 0 1 2 3 4; do \
		./test-5 $$((i + 1)) $$((j + 1)) | diff -u - corpus-max; \
	done; done
	rm -f corpus-max corpus-[0-4]

max-compare: $(OBJ) max-compare.cpp
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ max-compare.cpp $(OBJ) $(LDLIBS)

max-compare-run: max-compare
	awk -f corpus.awk corpus | sort -nk 2 | tail -n 1 >corpus-max
	cat corpus-max
	for i in 0 1 2 3 4; do sed -n "$$((i * 500 + 1)),$$(((i + 1) * 500))p" corpus >corpus-"$$i"; done
	for i in 0 1 2 3 4; do for j in 0 1 2 3 4; do \
		./max-compare $$((i + 1)) $$((j + 1)) | diff -u - corpus-max; \
	done; done
	rm -f corpus-max corpus-[0-4]


clean:
	rm -f $(BIN) $(OBJ)
