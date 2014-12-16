// int kbd_read_output_asm(unsigned long* output);

#include "_kbd.h"

.global _kbd_scan_int_handler_asm

.data

makecode_text:	.string "makecode: 0x%X\n"
breakcode_text:	.string "breakcode: 0x%X\n"

.zero	1
.lcomm	two_byte_scancode, 1

.text
_kbd_scan_int_handler_asm:
		xor		%eax, %eax											// set EAX register to 0
		in		$I8042_OUT_BUF, %al									// read the output buffer (must be to the al register)
		cmp		$TWO_BYTE_SCANCODE, %al								// check if it's a 2-byte scancode
		jne		onebyte												// if it's a 1-byte scancode, jump to onebyte
		movb	$1, two_byte_scancode								// set two_byte_scancode to true
		mov		$0, %eax											// set return value to 0
		ret
onebyte:cmp		$0, two_byte_scancode								// check if the last byte was TWO_BYTE_SCANCODE
		je		ln2byte												// if the last byte read was not TWO_BYTE_SCANCODE, jump to ln2byte
		mov		$TWO_BYTE_SCANCODE, %ah								// same as C's (scancode |= (TWO_BYTE_SCANCODE << 8))
		movb	$0, two_byte_scancode								// set two_byte_scancode to false
ln2byte:push	%eax												// second argument of printf()
		test	$BIT(I8042_BREAK_CODE_BIT), %al
		jnz		bc													// if it's a break code, jump to bc
		push	$makecode_text										// first argument of printf()
		call	_printf
		mov		$0, %eax											// set return value to 0
		jmp		end
bc:		push	$breakcode_text										// first argument of printf()
		call	_printf
		cmpw	$(KEY_ESC | BIT(I8042_BREAK_CODE_BIT)), 4(%esp)		// efficient way of getting the old value of EAX and comparing it with ESC breakcode without having to waste time saving EAX in the stack or copying it to another register
		mov		$0, %eax											// set return value to 0
		jne		end
		mov		$-1, %eax											// set return value to -1
end:	add 	$8, %esp											// "clean" the stack of the arguments passed to printf()
		ret
