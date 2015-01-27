CC=gcc
CFLAGS+=-lpj
RM=rm -rf 
OBJS=pthread_create.o
TARGET=pthread_create

all: $(OBJS) 
	$(CC) $(CFLAGS) $< -o $(TARGET)

$(OBJS): %.o:%.c
	$(CC) $(CFLAGS) -c -o $@ $^

clean:
	$(RM) $(OBJS) $(TARGET) 


