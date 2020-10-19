# Compile settings
CC      := gcc
CCFLAGS := -Wall --std=gnu17 -O3
LDFLAGS :=
TARGET  := memtest

# Make targets
$(TARGET): main.o
	$(CC) $(LDFLAGS) -o $(TARGET) main.o 

main.o: main.c
	$(CC) -c $(CCFLAGS) main.c


# Phony targets
.PHONY: all clean

all: $(TARGET)

clean: 
	rm -rf $(TARGET) ./*.o
