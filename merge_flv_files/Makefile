include ./config

OBJS	+= \
	$(TOPDIR)/lib/flv_parser.o


CGI_OBJS	+= \
	   $(TOPDIR)/cgi/flvss.o

RECORDER_OBJS	+= \
	   $(TOPDIR)/recorder/recorder.o

TARGET	= merge_flv

FLVSS	= flvss

all:$(OBJS) $(CGI_OBJS) $(RECORDER_OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(RECORDER_OBJS) -o $(TARGET)
	$(CC) $(CFLAGS) $(OBJS) $(CGI_OBJS) -o $(FLVSS)

clean:
	rm -rf $(TARGET) $(OBJS) $(CGI_OBJS) $(FLVSS) $(RECORDER_OBJS)
