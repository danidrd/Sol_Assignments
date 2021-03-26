CFLAGS		=  -Wall -pedantic
TARGETS		=  passwd	\
		  binmat	\
		  readbinmat	\
		  wcsimple
DIR		= /home/dan/Scrivania/SOL_ASSIGNMENTS/Es4/objs

OBJS		= $(DIR)/passwd		\
		  $(DIR)/binmat		\
		  $(DIR)/readbinmat	\
		  $(DIR)/wcsimple

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
	-mv $(TARGETS) $(DIR)

clean		:
	-rm -f $(OBJS)

