CC = gcc

CFLAGS_RAW = -Wall -Wextra -std=c99 -O0 -g -maes -msse4.1
CFLAGS_IMP = -Wall -Wextra -std=c99 -O3 -march=native -flto -g -maes -msse4.1

TARGET = main
SOURCES = main.c aes128.c ttables.c aesni.c

RUNS ?= 1
BUILD ?= improved

# Select flags based on BUILD variable
ifeq ($(BUILD),raw)
    CFLAGS = $(CFLAGS_RAW)
else
    CFLAGS = $(CFLAGS_IMP)
endif

$(TARGET): $(SOURCES)
	$(CC) $(CFLAGS) $(SOURCES) -I. -o $(TARGET)

run: $(TARGET)
	./$(TARGET) $(RUNS)
	rm -f $(TARGET)

clean:
	rm -f $(TARGET)

help:
	@echo "Available targets:"
	@echo "  _           - Build the AES-128 program"
	@echo "  run         - Build and run the AES-128 program"
	@echo "                Optionally, set number of runs with RUNS, e.g.: 'make run RUNS=1000000'"
	@echo "                Optionally, select build type with BUILD, e.g.: 'make run BUILD=raw'"
	@echo "  clean       - Remove build artifacts"
	@echo "  help        - Show this help message"
