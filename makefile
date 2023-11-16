.PHONY: clean all client server

all: clean all client server

clean:
	rm -rf *.o

client server: %: makefile.%
	$(MAKE) -f $<