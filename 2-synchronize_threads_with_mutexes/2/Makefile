CC=gcc
INCPATH=/home/astrol/work/test/pthread/lib_pj/include
LIBPATH=/home/astrol/work/test/pthread/lib_pj/lib
CFLAGS+=-I$(INCPATH) -L$(LIBPATH)
CFLAGS+=-lpj
RM=rm -rf 
OBJS=employee_with_mutex.o
TARGET=employee_with_mutex

all: $(OBJS) 
	$(CC) $< -o $(TARGET) $(CFLAGS)

$(OBJS): %.o:%.c
	$(CC) $(CFLAGS) -c -o $@ $^

clean:
	$(RM) $(OBJS) $(TARGET) 


