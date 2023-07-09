mkdir:
	gcc -static -nostdlib shellcode_mkdir.s -o sc_mkdir
	objcopy --dump-section .text=sc_mkdir_raw sc_mkdir
	rm sc_mkdir

sh:
	gcc -static -nostdlib shellcode_sh.s -o sc_shell
	objcopy --dump-section .text=sc_shell_raw sc_shell
	rm sc_shell
