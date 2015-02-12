CC=gcc
#INCPATH=/home/astrol/work/test/pthread/lib_pj/include
#LIBPATH=/home/astrol/work/test/pthread/lib_pj/lib
#CFLAGS+=-I$(INCPATH) -L$(LIBPATH)
CFLAGS+=`pkg-config --cflags libpjproject`
LDFLAGS+=`pkg-config --libs libpjproject`
RM=rm -rf 
OBJS=employee_with_mutex.o
TARGET=employee_with_mutex

all: $(OBJS) 
	$(CC) $< -o $(TARGET) $(LDFLAGS)

$(OBJS): %.o:%.c
	$(CC) $(CFLAGS) -c -o $@ $^

clean:
	$(RM) $(OBJS) $(TARGET) 


