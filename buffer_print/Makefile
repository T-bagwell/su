CFLAGS	+= -I./ -Wall

OBJS	+= buffer_print.o \
	   testcase.o

TARGET	= buffer_print_test

all: $(OBJS)
	gcc $(OBJS) -o $(TARGET)

clean:
	rm -rf $(OBJS) $(TARGET)
