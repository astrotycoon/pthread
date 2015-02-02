CC=gcc
CFLAGS+=-pthread -DDEBUG
RM=rm -rf 
OBJS=cond.o
TARGET=cond

all: $(OBJS) 
	$(CC) $(CFLAGS) $< -o $(TARGET)

$(OBJS): %.o:%.c
	$(CC) $(CFLAGS) -c -o $@ $^

clean:
	$(RM) $(OBJS) $(TARGET) 


