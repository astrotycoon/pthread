CC=gcc
CFLAGS+=-pthread
RM=rm -rf 
OBJS=sem.o pc.o pc1.o 
TARGETS=sem pc pc1  

all: $(OBJS) 
	$(foreach TARGET, $(TARGETS), $(CC) $(CFLAGS) $(TARGET).o -o $(TARGET);)

$(OBJS): %.o:%.c
	$(foreach SRC, $^, $(CC) $(CFLAGS) -c -o $@ $(SRC))

clean:
	$(RM) $(OBJS) $(TARGETS) 



