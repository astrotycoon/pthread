CC=gcc
CFLAGS+=-pthread
RM=rm -rf 
OBJS=pc0.o pc1.o pc2.o pc3.o  
TARGETS=pc0 pc1 pc2 pc3 

all: $(OBJS) 
	$(foreach TARGET, $(TARGETS), $(CC) $(CFLAGS) $(TARGET).o -o $(TARGET);)

$(OBJS): %.o:%.c
	$(foreach SRC, $^, $(CC) $(CFLAGS) -c -o $@ $(SRC))

clean:
	$(RM) $(OBJS) $(TARGETS) 



