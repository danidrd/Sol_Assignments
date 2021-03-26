CC		=  gcc
CFLAGS		=  -Wall -pedantic
TARGETS		=  passwd	\
		  binmat	\
		  readbinmat	\
		  wcsimple


.PHONY:  clean all
.SUFFIXES: .c .h

%: %.c
	$(CC) $(CFLAGS) -o $@ $<

$.o: $.c
	$(CC) $(CFLAGS) -c -o $@ $<

all		: $(TARGETS)

clean		:
	-rm -f $(TARGETS)
