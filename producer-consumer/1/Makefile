CC=gcc
CFLAGS+=-pthread
RM=rm -rf 
OBJS=pc.o
TARGET=pc

all: $(OBJS) 
	$(CC) $(CFLAGS) $< -o $(TARGET)

$(OBJS): %.o:%.c
	$(CC) $(CFLAGS) -c -o $@ $^

clean:
	$(RM) $(OBJS) $(TARGET) 


