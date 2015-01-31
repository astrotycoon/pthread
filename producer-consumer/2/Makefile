CC=gcc
INCPATH=/home/astrol/work/test/pthread/lib_pj/include
LIBPATH=/home/astrol/work/test/pthread/lib_pj/lib
CFLAGS+=-I$(INCPATH) -L$(LIBPATH)
CFLAGS+=-lpj
RM=rm -rf 
OBJS=pc.o
TARGET=pc

all: $(OBJS) 
	$(CC) $< -o $(TARGET) $(CFLAGS)

$(OBJS): %.o:%.c
	$(CC) $(CFLAGS) -c -o $@ $^

clean:
	$(RM) $(OBJS) $(TARGET) 


