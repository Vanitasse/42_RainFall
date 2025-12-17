# Level 0 - Binary Exploitation Walkthrough

## Initial Analysis

We begin by examining the binary using Ghidra to understand its behavior.

## Code Review

Upon analyzing the pseudo-code in Ghidra, we identify that we need to satisfy a specific conditional statement to progress. The key condition involves the hexadecimal value `0x1a7`, which converts to decimal `423`.

## Exploitation

To enter the required conditional branch, we execute the binary with the following parameter:
```bash
./level0 423
```

This successfully satisfies the condition and spawns a shell.

## Privilege Verification

After gaining shell access, we verify our current privileges:
```bash
id
```

The output confirms that we are already running as the `level1` user.

## Retrieving the Password

Since we have `level1` privileges, we can directly access the password file:
```bash
cat /home/user/level1/.pass
```

## Credentials

**Username:** level1  
**Password:** `1fe8a524fa4bec01ca4ea2a869af2a02260d4a7d5fe7e7c24d8617e6dca12d3a`

## Summary

This level demonstrates a simple authentication bypass through parameter manipulation. By providing the correct numerical value derived from reverse engineering, we gain immediate elevated access without requiring additional exploitation techniques.