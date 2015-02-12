CC=gcc
CFLAGS+=-pthread
RM=rm -rf 
OBJS=employee_with_mutex.o
OBJS=$(wildcard %.c, %.o, )
TARGET=employee_with_mutex

all: $(OBJS) 
	$(CC) $(CFLAGS) $< -o $(TARGET)

$(OBJS): %.o:%.c
	$(CC) $(CFLAGS) -c -o $@ $^

clean:
	$(RM) $(OBJS) $(TARGET) 


