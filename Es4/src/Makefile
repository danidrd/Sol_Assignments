CFLAGS		=  -Wall -pedantic
TARGETS		=  passwd	\
		  binmat	\
		  readbinmat	\
		  wcsimple


.PHONY:  all	\
	 move	\
	 clean
.SUFFIXES: .c .h

%: %.c
	$(CC) $(CFLAGS) -o $@ $<

$.o: $.c
	$(CC) $(CFLAGS) -c -o $@ $<

all		: $(TARGETS)

move		:
	-mv $(TARGETS) /home/dan/Scrivania/SOL_ASSIGNMENTS/Es4/objs

clean		:
	-rm -f $(TARGETS)

