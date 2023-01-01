all: cachesim lrustacktest
	

cachesim: cachesim.o lrustack.o
	g++ -o $@ $^

lrustacktest: lrustacktest.o lrustack.o
	g++ -o  $@ $^


%.o: %.c
	g++ -c  $< -o $@

clean: 
	rm *.o cachesim lrustacktest