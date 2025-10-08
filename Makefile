CC = gcc

# -Wno-deprecated-declarations since OpenSSL AES functions are deprecated
CFLAGS_0 = -Wall -Wextra -std=c99 -O0 -g -maes -msse4.1 -Wno-deprecated-declarations
CFLAGS_1 = -Wall -Wextra -std=c99 -O1 -g -maes -msse4.1 -Wno-deprecated-declarations
CFLAGS_2 = -Wall -Wextra -std=c99 -O2 -g -maes -msse4.1 -Wno-deprecated-declarations
CFLAGS_3 = -Wall -Wextra -std=c99 -O3 -g -maes -msse4.1 -Wno-deprecated-declarations
CFLAGS_4 = -Wall -Wextra -std=c99 -O3 -march=native -flto -g -maes -msse4.1 -Wno-deprecated-declarations

TARGET = main
SOURCES = main.c aes128.c ttables.c aesni.c openssl.c

RUNS ?= 1
OPT ?= 4   # default to highest optimization

# Select flags based on OPT variable
CFLAGS = $(CFLAGS_$(OPT))

LDFLAGS = -lcrypto

$(TARGET): $(SOURCES)
	$(CC) $(CFLAGS) $(SOURCES) -I. -o $(TARGET) $(LDFLAGS)

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
	@echo "                Optionally, select optimization level with OPT, e.g.: 'make run OPT=0'"
	@echo "                Available OPT levels: 0 (-O0), 1 (-O1), 2 (-O2), 3 (-O3), 4 (O3 + native + LTO)"
	@echo "  clean       - Remove build artifacts"
	@echo "  help        - Show this help message"
