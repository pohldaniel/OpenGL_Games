#line 1 "C:/CProjects/mpg123/ports/cmake/src/libmpg123/../../../../src/libmpg123/synth_stereo_avx_float.S"








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

#line 10 "C:/CProjects/mpg123/ports/cmake/src/libmpg123/../../../../src/libmpg123/synth_stereo_avx_float.S"



















#line 30 "C:/CProjects/mpg123/ports/cmake/src/libmpg123/../../../../src/libmpg123/synth_stereo_avx_float.S"







	.section	.rodata


#line 41 "C:/CProjects/mpg123/ports/cmake/src/libmpg123/../../../../src/libmpg123/synth_stereo_avx_float.S"
	.balign 16
scale_avx:
	.long   939524096
	.text
	.balign 16
	.globl INT123_synth_1to1_real_s_avx_asm
INT123_synth_1to1_real_s_avx_asm:

	push		%rbp
	mov			%rsp, %rbp
	sub			$144, %rsp
	movaps		%xmm6, (%rsp)
	movaps		%xmm7, 16(%rsp)
	movaps		%xmm8, 32(%rsp)
	movaps		%xmm9, 48(%rsp)
	movaps		%xmm10, 64(%rsp)
	movaps		%xmm11, 80(%rsp)
	movaps		%xmm12, 96(%rsp)
	movaps		%xmm13, 112(%rsp)
	movaps		%xmm14, 128(%rsp)
	movl		48(%rbp), %eax 
#line 63 "C:/CProjects/mpg123/ports/cmake/src/libmpg123/../../../../src/libmpg123/synth_stereo_avx_float.S"

	vbroadcastss	scale_avx(%rip), %ymm14
	

	shl			$2, %eax
	mov			%rcx, %r10




#line 74 "C:/CProjects/mpg123/ports/cmake/src/libmpg123/../../../../src/libmpg123/synth_stereo_avx_float.S"
	add			$64, %r10
	sub			%rax, %r10

	mov			$128, %rax
	mov			$4, %ecx
	
	.balign 16
1:
	vmovups		(%r10), %ymm8
	vmovups		32(%r10), %ymm9
	vmovups		(%r10,%rax), %ymm10
	vmovups		32(%r10,%rax), %ymm11
	vmulps		(%rdx), %ymm8, %ymm0
	vmulps		32(%rdx), %ymm9, %ymm1
	vmulps		(%r8), %ymm8, %ymm2
	vmulps		32(%r8), %ymm9, %ymm3
	vmulps		64(%rdx), %ymm10, %ymm4
	vmulps		96(%rdx), %ymm11, %ymm5
	vmulps		64(%r8), %ymm10, %ymm6
	vmulps		96(%r8), %ymm11, %ymm7
	vaddps		%ymm1, %ymm0, %ymm8
	vaddps		%ymm3, %ymm2, %ymm0
	vaddps		%ymm5, %ymm4, %ymm9
	vaddps		%ymm7, %ymm6, %ymm1
	lea			(%r10,%rax,2), %r10
	add			%rax, %rdx
	add			%rax, %r8
	
	vmovups		(%r10), %ymm10
	vmovups		32(%r10), %ymm11
	vmovups		(%r10,%rax), %ymm12
	vmovups		32(%r10,%rax), %ymm13
	vmulps		(%rdx), %ymm10, %ymm2
	vmulps		32(%rdx), %ymm11, %ymm3
	vmulps		(%r8), %ymm10, %ymm4
	vmulps		32(%r8), %ymm11, %ymm5
	vmulps		64(%rdx), %ymm12, %ymm6
	vmulps		96(%rdx), %ymm13, %ymm10
	vmulps		64(%r8), %ymm12, %ymm7
	vmulps		96(%r8), %ymm13, %ymm11
	vaddps		%ymm3, %ymm2, %ymm2
	vaddps		%ymm5, %ymm4, %ymm3
	vaddps		%ymm6, %ymm10, %ymm4
	vaddps		%ymm7, %ymm11, %ymm5
	lea			(%r10,%rax,2), %r10
	add			%rax, %rdx
	add			%rax, %r8
	
	vunpcklps	%ymm0, %ymm8, %ymm6
	vunpckhps	%ymm0, %ymm8, %ymm0
	vunpcklps	%ymm1, %ymm9, %ymm7
	vunpckhps	%ymm1, %ymm9, %ymm1
	vaddps		%ymm6, %ymm0, %ymm0
	vaddps		%ymm7, %ymm1, %ymm1
	vunpcklps	%ymm3, %ymm2, %ymm6
	vunpckhps	%ymm3, %ymm2, %ymm2
	vunpcklps	%ymm5, %ymm4, %ymm7
	vunpckhps	%ymm5, %ymm4, %ymm3
	vaddps		%ymm6, %ymm2, %ymm2
	vaddps		%ymm7, %ymm3, %ymm3
	
	vunpcklpd	%ymm1, %ymm0, %ymm4
	vunpckhpd	%ymm1, %ymm0, %ymm0
	vunpcklpd	%ymm3, %ymm2, %ymm5
	vunpckhpd	%ymm3, %ymm2, %ymm1
	vsubps		%ymm0, %ymm4, %ymm0
	vsubps		%ymm1, %ymm5, %ymm1
	vperm2f128	$0x20, %ymm1, %ymm0, %ymm2
	vperm2f128	$0x31, %ymm1, %ymm0, %ymm3
	vaddps		%ymm3, %ymm2, %ymm0
	vmulps		%ymm14, %ymm0, %ymm0
	
	vmovups		%ymm0, (%r9)
	add			$32, %r9
	dec			%ecx
	jnz			1b
	
	mov			$4, %ecx
	
	.balign 16
1:
	vmovups		(%r10), %ymm8
	vmovups		32(%r10), %ymm9
	vmovups		(%r10,%rax), %ymm10
	vmovups		32(%r10,%rax), %ymm11
	vmulps		(%rdx), %ymm8, %ymm0
	vmulps		32(%rdx), %ymm9, %ymm1
	vmulps		(%r8), %ymm8, %ymm2
	vmulps		32(%r8), %ymm9, %ymm3
	vmulps		-64(%rdx), %ymm10, %ymm4
	vmulps		-32(%rdx), %ymm11, %ymm5
	vmulps		-64(%r8), %ymm10, %ymm6
	vmulps		-32(%r8), %ymm11, %ymm7
	vaddps		%ymm1, %ymm0, %ymm8
	vaddps		%ymm3, %ymm2, %ymm0
	vaddps		%ymm5, %ymm4, %ymm9
	vaddps		%ymm7, %ymm6, %ymm1
	lea			(%r10,%rax,2), %r10
	sub			%rax, %rdx
	sub			%rax, %r8
	
	vmovups		(%r10), %ymm10
	vmovups		32(%r10), %ymm11
	vmovups		(%r10,%rax), %ymm12
	vmovups		32(%r10,%rax), %ymm13
	vmulps		(%rdx), %ymm10, %ymm2
	vmulps		32(%rdx), %ymm11, %ymm3
	vmulps		(%r8), %ymm10, %ymm4
	vmulps		32(%r8), %ymm11, %ymm5
	vmulps		-64(%rdx), %ymm12, %ymm6
	vmulps		-32(%rdx), %ymm13, %ymm10
	vmulps		-64(%r8), %ymm12, %ymm7
	vmulps		-32(%r8), %ymm13, %ymm11
	vaddps		%ymm3, %ymm2, %ymm2
	vaddps		%ymm5, %ymm4, %ymm3
	vaddps		%ymm6, %ymm10, %ymm4
	vaddps		%ymm7, %ymm11, %ymm5
	lea			(%r10,%rax,2), %r10
	sub			%rax, %rdx
	sub			%rax, %r8
	
	vunpcklps	%ymm0, %ymm8, %ymm6
	vunpckhps	%ymm0, %ymm8, %ymm0
	vunpcklps	%ymm1, %ymm9, %ymm7
	vunpckhps	%ymm1, %ymm9, %ymm1
	vaddps		%ymm6, %ymm0, %ymm0
	vaddps		%ymm7, %ymm1, %ymm1
	vunpcklps	%ymm3, %ymm2, %ymm6
	vunpckhps	%ymm3, %ymm2, %ymm2
	vunpcklps	%ymm5, %ymm4, %ymm7
	vunpckhps	%ymm5, %ymm4, %ymm3
	vaddps		%ymm6, %ymm2, %ymm2
	vaddps		%ymm7, %ymm3, %ymm3
	
	vunpcklpd	%ymm1, %ymm0, %ymm4
	vunpckhpd	%ymm1, %ymm0, %ymm0
	vunpcklpd	%ymm3, %ymm2, %ymm5
	vunpckhpd	%ymm3, %ymm2, %ymm1
	vaddps		%ymm0, %ymm4, %ymm0
	vaddps		%ymm1, %ymm5, %ymm1
	vperm2f128	$0x20, %ymm1, %ymm0, %ymm2
	vperm2f128	$0x31, %ymm1, %ymm0, %ymm3
	vaddps		%ymm3, %ymm2, %ymm0
	vmulps		%ymm14, %ymm0, %ymm0
	
	vmovups		%ymm0, (%r9)
	add			$32, %r9
	dec			%ecx
	jnz			1b
	
	vzeroupper
	
	xor			%eax, %eax
	

	movaps		(%rsp), %xmm6
	movaps		16(%rsp), %xmm7
	movaps		32(%rsp), %xmm8
	movaps		48(%rsp), %xmm9
	movaps		64(%rsp), %xmm10
	movaps		80(%rsp), %xmm11
	movaps		96(%rsp), %xmm12
	movaps		112(%rsp), %xmm13
	movaps		128(%rsp), %xmm14
	mov			%rbp, %rsp
	pop			%rbp
#line 241 "C:/CProjects/mpg123/ports/cmake/src/libmpg123/../../../../src/libmpg123/synth_stereo_avx_float.S"
	ret


