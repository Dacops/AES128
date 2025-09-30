CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -O2 -g

TARGET = main
SOURCES = main.c aes128.c ttables.c

RUNS ?= 1

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
	@echo "                Optionally, set number of runs with RUNS, e.g.: 'make run RUNS=1000000' (default is 1)"
	@echo "  clean       - Remove build artifacts"
	@echo "  help        - Show this help message"
