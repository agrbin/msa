CC=gcc -O3 -Wall -I.
.PHONY : clean test

TEST=test_lsh test_lsh1 test_lsh2 test_lsh3 test_lsh4

test : $(TEST) bin/lsh_distance.o
	@for test in $(TEST); do \
		/bin/echo -e "\e[1mrunning $$test... \033[0m " && ./bin/$$test; \
	done

valgrind : $(TEST) lsh_distance.o
	@for test in $(TEST); do \
		/bin/echo -e "\e[1mrunning $$test under valgrind... \033[0m " && valgrind ./bin/$$test; \
	done
	
$(TEST) : % : test/%.c lsh_distance.o
	$(CC) $< lsh_distance.o -o bin/$@

bin/lsh_distance.o : lsh_distance.c lsh_distance.h
	$(CC) -O3 -o bin/lsh_distance.o -c lsh_distance.c

clean :
	@rm -f bin/*
