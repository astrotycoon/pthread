CC=gcc
CFLAGS+=-g `pkg-config --cflags libpjproject`
LDFLAGS+=-g `pkg-config --libs libpjproject` 
RM=rm -rf 
OBJS=pthread_create.o
TARGET=pthread_create

all: $(OBJS) 
	$(CC) $< -o $(TARGET) $(LDFLAGS)

$(OBJS): %.o:%.c
	$(CC) $(CFLAGS) -c -o $@ $^

clean:
	$(RM) $(OBJS) $(TARGET) 


