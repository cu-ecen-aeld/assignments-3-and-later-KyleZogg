#added this ifdef to make the cross compile a define option
ifdef CROSS_COMPILE
CC = $(CROSS_COMPILE)
endif

#made this look a little more proffesional 
SRC := writer.c
TARGET = writer
OBJS := $(SRC:.c=.o)

#this is the "all" target that the buildroot will try and use
all: $(TARGET)

$(TARGET) : $(OBJS)
	$(CC) $(CFLAGS) $(INCLUDES) $(OBJS) -o $(TARGET) $(LDFLAGS)

#updated the clean target to be more proffesional
clean:
	-rm -f *.o $(TARGET) *.elf *.map