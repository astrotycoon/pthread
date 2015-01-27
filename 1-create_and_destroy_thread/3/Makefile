CC=gcc
CFLAGS+=-D_REENTRANT -D_POSIX_PTHREAD_SEMANTICS -D_USE_FAST_MACRO
CFLAGS+=-Wstrict-prototypes -DLINUX2 -D_REENTRANT
CFLAGS+=-I/home/astrol/work/test/pthread/lib_acl/include
CFLAGS+=-L/home/astrol/work/test/pthread/lib_acl/lib
CFLAGS+=-l_acl -lpthread
RM=rm -rf 
OBJS=pthread_create.o
TARGET=pthread_create

all: $(OBJS) 
	$(CC) $< -o $(TARGET) $(CFLAGS)

$(OBJS): %.o:%.c
	$(CC) $(CFLAGS) -c -o $@ $^

clean:
	$(RM) $(OBJS) $(TARGET) 


