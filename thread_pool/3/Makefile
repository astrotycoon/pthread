CC=gcc
CFLAGS+=-I. -pthread  
RM=rm -rf 
OBJS=$(patsubst %.c, %.o, $(wildcard *.c))  
TARGETS=test

all: $(TARGETS)

$(TARGETS): $(OBJS) 
#	$(foreach TARGET, $(TARGETS), $(CC) $(CFLAGS) $(TARGET).o -o $(TARGET);)
	$(CC) $(CFLAGS) -o $@ libtpool.a $^

$(OBJS): %.o:%.c
	$(foreach SRC, $^, $(CC) $(CFLAGS) -c -o $@ $(SRC))

clean:
	$(RM) $(OBJS) $(TARGETS) 



