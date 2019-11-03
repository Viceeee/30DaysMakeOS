; naskfunc
; TAB=4

[FORMAT "WCOFF"]				;����Ŀ���ļ���ģʽ
[INSTRSET "i486p"]				;Ҫʹ�õ�CPU��486����
[BITS 32]						;����32λģʽ�õĻ�е����
;����Ŀ���ļ�����Ϣ
[FILE "naskfunc.nas"]			;Դ�ļ�����Ϣ

		GLOBAL	_io_hlt, _io_cli, _io_sti, _io_stihlt
		GLOBAL	_io_in8,  _io_in16,  _io_in32
		GLOBAL	_io_out8, _io_out16, _io_out32
		GLOBAL	_io_load_eflags, _io_store_eflags
		GLOBAL	_load_gdtr, _load_idtr
		;������ʵ�ʵĺ���

[SECTION .text]		;Ŀ���ļ���д����Щ֮����д����

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
		
; �������������ָ���Ķ����ޣ�limit���͵�ֵַ��ֵ����ΪGDTR��48λ�Ĵ���������һ�����ر��48λ�Ĵ���
; �����������ǳ��õ�MOVָ������ֵ��������ֵ��ʱ��Ψһ�ķ�������ָ��һ���ڴ��ַ����ָ���ĵ�ַ�ж�ȡ
; 6���ֽڣ�48λ����Ȼ��ֵ��GDTR�Ĵ����������һ�����ָ�����lgdt���üĴ����ĵ�16λ�����ڴ�������
; 2���ֽ��Ƕ����ޣ������ڡ�gdt����Ч�ֽ���-1����
; �����ִ���������ʱ��DWORD[ESP+4]���ŵ��Ƕ����ޣ�DWORD[ESP+8]���ŵ��ǵ�ַ��Ϊ��ִ��LGDT��
		
_load_idtr:		;void load_idtr(int limit, int addr);
		MOV		AX,[ESP+4]		;limit
		MOV		[ESP+6],AX
		LIDT	[ESP+6]
		RET
		
		
		
		
		
		