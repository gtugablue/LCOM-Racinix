.text
.globl _vg_set_pixel_asm
_vg_set_pixel_asm: // unsigned long x, unsigned long y, uint16_t color
	pushl	%ebp
	movl	%esp, %ebp

	// Verify input
	mov 8(%ebp), %edx // EDX = x
	//cmp %edx, _h_res
	cmp $1024, %edx // Hardcoded width
	//jbe error
	ja error
	mov 12(%ebp), %ecx // ECX = y
	//cmp %ecx, _v_res
	cmp $768, %ecx // Hardcoded height
	//jbe error
	ja error

	// Set pixel
	//imul _h_res, %ecx // ECX = y * h_res
	shl $10, %ecx // Hardcoded width (1024)
	add %edx, %ecx // ECX = x + y * h_res
	mov _double_buffer, %eax // ESI = double_buffer
	mov 16(%ebp), %edx // EDX = color
	mov %dx, (%eax, %ecx, 2) // Change pixel color

	xor %eax, %eax // true
	leave
	ret

error: mov $1, %eax // false
	leave
	ret
