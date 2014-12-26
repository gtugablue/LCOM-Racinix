.text

.globl _memset16
_memset16: // uint16_t *ptr, uint16_t value, size_t num
	pushl	%ebp
	movl	%esp, %ebp
	push %edi
	mov 8(%ebp), %edi // ptr
	xor %eax, %eax
	mov 12(%ebp), %ax // value
	xor %ecx, %ecx
	mov 16(%ebp), %ecx // num

	rep stosw

	pop %edi
	mov 8(%ebp), %eax // return ptr

	leave
	ret

.globl _memset32
_memset32: // uint32_t *ptr, uint32_t value, size_t num
	pushl	%ebp
	movl	%esp, %ebp
	push %edi
	mov 8(%ebp), %edi // *dest
	xor %eax, %eax
	mov 12(%ebp), %ax // value
	xor %ecx, %ecx
	mov 16(%ebp), %ecx // num

	rep stosl

	pop %edi
	mov 8(%ebp), %eax // return ptr

	leave
	ret
