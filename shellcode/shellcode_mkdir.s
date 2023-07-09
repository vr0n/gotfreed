.global _start
_start:
  .intel_syntax noprefix
  mov r10, 0x74736574
  push r10

  mov rdi, rsp
  mov rsi, 511
  mov rax, 83

  syscall

  ret
