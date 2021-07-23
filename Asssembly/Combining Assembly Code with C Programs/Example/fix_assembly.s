# file assembly.s
.globl	tmult_ok_asm
.globl	umult_ok_asm
#.globl	tmulk_ok_asm_v1

tmult_ok_asm:
	imulq	%rsi, %rdi
	pushq   %rbx
	setae   %bl
	movzbl	%bl, %eax
	popq   %rbx
	movq	%rdi, (%rdx)
	ret

umult_ok_asm:
# With the `mulq`instruction,
# the other argument to the multiplication must be in register %rax,
# and the product is sotred in registers %rdx (high-order 8 bytes) and %rax (low-order 8 bytes).
# So, we must save copy of %rdx before MUL.
    movq    %rdx, %rcx
    movq    %rsi, %rax
    mulq    %rdi
    movq	%rax, (%rcx)
    setae	%al
	movzbl	%al, %eax
	ret


tmulk_ok_asm_v1:
.LFB26:
	.cfi_startproc
	endbr64
	pushq	%rbx
	.cfi_def_cfa_offset 16
	.cfi_offset 3, -16
	imulq	%rsi, %rdi
	movq	%rdi, (%rdx)
#APP
# 63 "c_with_assembly_code.c" 1
	setae %bl
	movzbl %bl, %eax
# 0 "" 2
#NO_APP
	popq	%rbx
	.cfi_def_cfa_offset 8
	ret
	.cfi_endproc

