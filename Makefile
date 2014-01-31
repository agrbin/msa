INCLUDE=-I./src
CFLAGS=-O3 -flto --std=c++11 -Wall -Wextra -Werror -pedantic

CC=g++ $(CFLAGS) $(INCLUDE)
VPATH=src:bin

# these headers are different. first, they don't exists
# but *.hpp files exists. it means that they contain template
# implementation and everything must be recompiled when
# they are changed.
HPP=GappedLine.h ConnectedString.h BaseSet.h

COBJ=lsh_distance.o

OBJ=GappedLine.o ConnectedString.o GappedLineBrute.o \
		PackedIntervals.o BaseSet.o Base.o Random.o MultipleAligner.o \
		FastaFile.o LayoutFile.o

ALLOBJ=$(OBJ) $(COBJ)

TEST=testGappedLine testConnectedString \
		 testPackedIntervals testMultipleAligner testBaseSet

EXE=msa count-n

# if there will be a file called test or clean, don't confuse
# makefile. it will rebuild these targets anyway.
.PHONY: test clean

exe: $(EXE)

test: $(TEST)
	@for test in $(TEST); do \
		/bin/echo -e "\e[1mrunning $$test... \033[0m " && ./bin/$$test; \
	done

clean:
	rm bin/*

install: $(EXE)
	cp bin/msa /usr/local/bin/grbin-msa

$(COBJ): %.o: %.c %.h
	@/bin/echo -e "\e[34m     C $@ \033[0m"
	@$(CC) -c $< -o bin/$@

# if someone looks for a .h file that is header implementation
# just show them .hpp file. this is ugly hack. we create a file
# in bin/ with name sth.h to make this target work as expected.
$(HPP): %.h: %.hpp
	@touch bin/$@

# build rule for object files. note that we require recompilation
# when header-implementation file is changed.
$(OBJ): %.o: %.cpp %.h $(patsubst %,%pp,$(HPP))
	@/bin/echo -e "\e[34m    CC $@ \033[0m"
	@$(CC) -c $< -o bin/$@

# build rule for test executables.
$(TEST): %: src/test/%.cpp $(OBJ) $(COBJ)
	@/bin/echo -e "\e[34m  LINK $@ \033[0m"
	@$(CC) $< $(patsubst %,bin/%,$(ALLOBJ)) -o bin/$@

# build rule for main executables.
$(EXE): %: src/%.cpp $(OBJ) $(COBJ)
	@/bin/echo -e "\e[34m  LINK $@ \033[0m"
	@$(CC) $< $(patsubst %,bin/%,$(ALLOBJ)) -o bin/$@

