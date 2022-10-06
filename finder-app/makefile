ifdef CROSS_COMPILE
CC= $(CROSS_COMPILE)
endif

SRC := writer.c
TARGET = writer
OBJS := $(SRC:.c=.o)

all: $(TARGET)

$(TARGET) : $(OBJS)
	$(CC) $(CFLAGS) $(INCLUDES) $(OBJS) -o $(TARGET) $(LDFLAGS)

#writer: writer.c
#	gcc -o writer writer.c

clean: 
	rm -f ./writer

#CROSS_COMPILE:
#	aarch64-none-linux-gnu-gcc -o writer writer.c