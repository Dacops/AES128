# AES-128 Project

This project implements AES-128 encryption using multiple methods:

- **Naive C implementation** – [`aes128.c`](aes128.c)
- **T-Tables implementation** – [`ttables.c`](ttables.c)
- **AES-NI implementation** – [`aesni.c`](aesni.c)
- **OpenSSL implementation (legacy and EVP interfaces)** – [`openssl.c`](openssl.c)

## Prerequisites
Linux installation with GCC, Make, and OpenSSL development libraries (`libssl-dev`), which can be installed via `sudo apt install libssl-dev`.

## Compilation and Installation
Run `make run` to compile and execute the program. Use `make help` for other options like optimization levels and number of runs.

## Running the test cases
The test cases are executed automatically when running `make run`. The program prints timing and correctness results for all AES implementations. Any mismatch with expected outputs will cause the program to exit with an error, so readers can interpret the printed results as successful if no errors occur.
