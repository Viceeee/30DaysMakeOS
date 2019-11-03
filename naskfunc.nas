; naskfunc
; TAB=4

[FORMAT "WCOFF"]				;制作目标文件的模式
[INSTRSET "i486p"]				;要使用的CPU的486命令
[BITS 32]						;制作32位模式用的机械语言
;制作目标文件的信息
[FILE "naskfunc.nas"]			;源文件名信息

		GLOBAL	_io_hlt, _io_cli, _io_sti, _io_stihlt
		GLOBAL	_io_in8,  _io_in16,  _io_in32
		GLOBAL	_io_out8, _io_out16, _io_out32
		GLOBAL	_io_load_eflags, _io_store_eflags
		GLOBAL	_load_gdtr, _load_idtr
		;以下是实际的函数

[SECTION .text]		;目标文件中写了这些之后在写程序

_io_hlt:	; void io_hlt(void);
		HLT
		RET

_io_cli:	; void io_cli(void);
		CLI
		RET

_io_sti:	; void io_sti(void);
		STI
		RET

_io_stihlt:	; void io_stihlt(void);
		STI
		HLT
		RET

_io_in8:	; int io_in8(int port);
		MOV		EDX,[ESP+4]		; port
		MOV		EAX,0
		IN		AL,DX
		RET

_io_in16:	; int io_in16(int port);
		MOV		EDX,[ESP+4]		; port
		MOV		EAX,0
		IN		AX,DX
		RET

_io_in32:	; int io_in32(int port);
		MOV		EDX,[ESP+4]		; port
		IN		EAX,DX
		RET

_io_out8:	; void io_out8(int port, int data);
		MOV		EDX,[ESP+4]		; port
		MOV		AL,[ESP+8]		; data
		OUT		DX,AL
		RET

_io_out16:	; void io_out16(int port, int data);
		MOV		EDX,[ESP+4]		; port
		MOV		EAX,[ESP+8]		; data
		OUT		DX,AX
		RET

_io_out32:	; void io_out32(int port, int data);
		MOV		EDX,[ESP+4]		; port
		MOV		EAX,[ESP+8]		; data
		OUT		DX,EAX
		RET

_io_load_eflags:	; int io_load_eflags(void);
		PUSHFD		; means PUSH EFLAGS 
		POP		EAX
		RET

_io_store_eflags:	; void io_store_eflags(int eflags);
		MOV		EAX,[ESP+4]
		PUSH	EAX
		POPFD		; means POP EFLAGS 
		RET
		
_load_gdtr:		;void load_gdtr(int limit, int addr);
		MOV		AX,[ESP+4]		;limit
		MOV		[ESP+6],AX
		LIDT	[ESP+6]
		RET
		
; 这个函数用来将指定的段上限（limit）和地址值赋值给名为GDTR的48位寄存器。这是一个很特别的48位寄存器
; 并不能用我们常用的MOV指令来赋值。给他赋值的时候，唯一的方法就是指定一个内存地址，从指定的地址中读取
; 6个字节（48位），然后赋值给GDTR寄存器。完成这一任务的指令就是lgdt，该寄存器的低16位即是内存的最初的
; 2个字节是段上限，它等于“gdt的有效字节数-1”，
; 在最初执行这个函数时，DWORD[ESP+4]里存放的是段上限，DWORD[ESP+8]里存放的是地址。为了执行LGDT，
		
_load_idtr:		;void load_idtr(int limit, int addr);
		MOV		AX,[ESP+4]		;limit
		MOV		[ESP+6],AX
		LIDT	[ESP+6]
		RET
		
		
		
		
		
		
