CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -O2 -g

TARGET = main
SOURCES = main.c aes128.c

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
	@echo "  _	     - Build the AES-128 program"
	@echo "  run        - Build and run the AES-128 program"
	@echo "  clean      - Remove build artifacts"
	@echo "  help       - Show this help message"