CC=gcc
INCPATH=/home/astrol/work/test/pthread/lib_apr/include
LIBPATH=/home/astrol/work/test/pthread/lib_apr/lib
CFLAGS+=-D_LARGEFILE64_SOURCE
CFLAGS+=-I$(INCPATH) -L$(LIBPATH)
CFLAGS+=-lapr
RM=rm -rf 
OBJS=employee_with_mutex.o
TARGET=employee_with_mutex

all: $(OBJS) 
	$(CC) $< -o $(TARGET) $(CFLAGS)

$(OBJS): %.o:%.c
	$(CC) $(CFLAGS) -c -o $@ $^

clean:
	$(RM) $(OBJS) $(TARGET) 


