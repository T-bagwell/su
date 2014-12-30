CFLAGS += -g -I.

OBJS	+= \
	   time_to_val.o

all:$(OBJS)
	gcc -g main.c -o time_to_val $(OBJS)

clean:
	rm -rf time_to_val $(OBJS)
