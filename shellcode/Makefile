target:
	gcc -static -nostdlib shellcode_mkdir.s -o sc_mkdir
	objcopy --dump-section .text=sc_mkdir_raw sc_mkdir
	rm sc_mkdir
