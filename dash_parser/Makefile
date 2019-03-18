CC=gcc
TARGET=dash_parser
CFLAGS	=-O2 -Wall -g
LDFLAGS	=-lxml2

OBJS	+= \
	   dash.o


all:$(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS)

clean:
	rm -rf $(OBJS) $(TARGET)
