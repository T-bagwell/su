CC=gcc
TARGET=dash_parser
CFLAGS	=-O2 -Wall -g -I/usr/include/libxml2
LDFLAGS	=-lxml2

OBJS	+= \
	   dash.o


all:$(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS)

clean:
	rm -rf $(OBJS) $(TARGET)
