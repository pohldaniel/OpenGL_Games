#line 1 "C:/CProjects/mpg123/ports/cmake/src/libmpg123/../../../../src/libmpg123/getcpuflags_x86_64.S"








#line 1 "c:\\cprojects\\mpg123\\src\\libmpg123\\mangle.h"












#line 1 "C:/CProjects/mpg123/build264/src\\config.h"



































































































































































#line 165 "C:/CProjects/mpg123/build264/src\\config.h"











#line 14 "c:\\cprojects\\mpg123\\src\\libmpg123\\mangle.h"
#line 1 "C:/CProjects/mpg123/ports/cmake/../../src\\intsym.h"




#line 1 "C:/CProjects/mpg123/build264/src\\config.h"



































































































































































#line 165 "C:/CProjects/mpg123/build264/src\\config.h"











#line 6 "C:/CProjects/mpg123/ports/cmake/../../src\\intsym.h"









































































































































































































































































































































































#line 368 "C:/CProjects/mpg123/ports/cmake/../../src\\intsym.h"
#line 15 "c:\\cprojects\\mpg123\\src\\libmpg123\\mangle.h"



#line 22 "c:\\cprojects\\mpg123\\src\\libmpg123\\mangle.h"





#line 28 "c:\\cprojects\\mpg123\\src\\libmpg123\\mangle.h"











































#line 72 "c:\\cprojects\\mpg123\\src\\libmpg123\\mangle.h"







#line 80 "c:\\cprojects\\mpg123\\src\\libmpg123\\mangle.h"


#line 84 "c:\\cprojects\\mpg123\\src\\libmpg123\\mangle.h"


#line 87 "c:\\cprojects\\mpg123\\src\\libmpg123\\mangle.h"




























































#line 148 "c:\\cprojects\\mpg123\\src\\libmpg123\\mangle.h"



#line 152 "c:\\cprojects\\mpg123\\src\\libmpg123\\mangle.h"

#line 154 "c:\\cprojects\\mpg123\\src\\libmpg123\\mangle.h"





#line 160 "c:\\cprojects\\mpg123\\src\\libmpg123\\mangle.h"






#line 167 "c:\\cprojects\\mpg123\\src\\libmpg123\\mangle.h"









#line 177 "c:\\cprojects\\mpg123\\src\\libmpg123\\mangle.h"

#line 179 "c:\\cprojects\\mpg123\\src\\libmpg123\\mangle.h"



#line 183 "c:\\cprojects\\mpg123\\src\\libmpg123\\mangle.h"








#line 192 "c:\\cprojects\\mpg123\\src\\libmpg123\\mangle.h"









#line 202 "c:\\cprojects\\mpg123\\src\\libmpg123\\mangle.h"

#line 204 "c:\\cprojects\\mpg123\\src\\libmpg123\\mangle.h"

#line 10 "C:/CProjects/mpg123/ports/cmake/src/libmpg123/../../../../src/libmpg123/getcpuflags_x86_64.S"

	.text
	.balign 4
	.globl INT123_getcpuflags
INT123_getcpuflags:
	push	%rbp
	mov		%rsp, %rbp
	push	%rbx
	

	push	%rdi
	mov		%rcx, %rdi
#line 23 "C:/CProjects/mpg123/ports/cmake/src/libmpg123/../../../../src/libmpg123/getcpuflags_x86_64.S"

	movl	$0, 12(%rdi)
	movl	$0, 16(%rdi)

	mov		$0x80000000, %eax
	cpuid
	cmp		$0x80000001, %eax
	jb		1f
	mov		$0x80000001, %eax
	cpuid
	movl	%edx, 12(%rdi)
1:
	mov		$0x00000001, %eax
	cpuid
	movl	%eax, (%rdi)
	movl	%ecx, 4(%rdi)
	movl	%edx, 8(%rdi)
	test	$0x04000000, %ecx
	jz		2f
	test	$0x08000000, %ecx
	jz		2f
	xor		%ecx, %ecx
	.byte	0x0f, 0x01, 0xd0 
	movl	%eax, 16(%rdi)
	movl	(%rdi), %eax
2:

	pop		%rdi
#line 52 "C:/CProjects/mpg123/ports/cmake/src/libmpg123/../../../../src/libmpg123/getcpuflags_x86_64.S"
	pop		%rbx
	mov		%rbp, %rsp
	pop		%rbp
	ret
	
