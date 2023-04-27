#line 1 "C:/CProjects/mpg123/ports/cmake/src/libmpg123/../../../../src/libmpg123/dct64_x86_64_float.S"








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

#line 10 "C:/CProjects/mpg123/ports/cmake/src/libmpg123/../../../../src/libmpg123/dct64_x86_64_float.S"















#line 26 "C:/CProjects/mpg123/ports/cmake/src/libmpg123/../../../../src/libmpg123/dct64_x86_64_float.S"






	.section	.rodata


#line 36 "C:/CProjects/mpg123/ports/cmake/src/libmpg123/../../../../src/libmpg123/dct64_x86_64_float.S"
	.balign 32
costab_x86_64:
	.long 1056974725
	.long 1057056395
	.long 1057223771
	.long 1057485416
	.long 1057855544
	.long 1058356026
	.long 1059019886
	.long 1059897405
	.long 1061067246
	.long 1062657950
	.long 1064892987
	.long 1066774581
	.long 1069414683
	.long 1073984175
	.long 1079645762
	.long 1092815430
	.long 1057005197
	.long 1057342072
	.long 1058087743
	.long 1059427869
	.long 1061799040
	.long 1065862217
	.long 1071413542
	.long 1084439708
	.long 1057128951
	.long 1058664893
	.long 1063675095
	.long 1076102863
	.long 1057655764
	.long 1067924853
	.long 1060439283
	.long 0
	.text
	.balign 16
.globl INT123_dct64_real_x86_64
INT123_dct64_real_x86_64:

	movq		%rcx, %r9
	subq		$168, %rsp 
	movaps		%xmm6, (%rsp)
	movaps		%xmm7, 16(%rsp)
	movaps		%xmm8, 32(%rsp)
	movaps		%xmm9, 48(%rsp)
	movaps		%xmm10, 64(%rsp)
	movaps		%xmm11, 80(%rsp)
	movaps		%xmm12, 96(%rsp)
	movaps		%xmm13, 112(%rsp)
	movaps		%xmm14, 128(%rsp)
	movaps		%xmm15, 144(%rsp)
#line 88 "C:/CProjects/mpg123/ports/cmake/src/libmpg123/../../../../src/libmpg123/dct64_x86_64_float.S"

	leaq		costab_x86_64(%rip), %rcx

	movups 	(%r8), %xmm15
	movups 	16(%r8), %xmm14
	movups 	112(%r8), %xmm0
	movups 	96(%r8), %xmm1
	shufps 		$0x1b, %xmm0, %xmm0
	shufps 		$0x1b, %xmm1, %xmm1
	movaps 		%xmm15, %xmm8
	movaps		%xmm14, %xmm9
	addps 		%xmm0, %xmm8
	addps 		%xmm1, %xmm9
	subps 		%xmm0, %xmm15
	subps 		%xmm1, %xmm14
	
	movups 	32(%r8), %xmm13
	movups 	48(%r8), %xmm12
	movups 	80(%r8), %xmm0
	movups 	64(%r8), %xmm1
	shufps		$0x1b, %xmm0, %xmm0
	shufps		$0x1b, %xmm1, %xmm1
	movaps		%xmm13, %xmm10
	movaps		%xmm12, %xmm11
	addps		%xmm0, %xmm10
	addps		%xmm1, %xmm11
	subps		%xmm0, %xmm13
	subps		%xmm1, %xmm12
	
	movaps		(%rcx), %xmm0
	movaps		16(%rcx), %xmm1
	movaps		32(%rcx), %xmm2
	movaps		48(%rcx), %xmm3
	mulps		%xmm0, %xmm15
	mulps		%xmm1, %xmm14
	mulps		%xmm2, %xmm13
	mulps		%xmm3, %xmm12
	
	movaps		64(%rcx), %xmm0
	movaps		80(%rcx), %xmm1
	
	pshufd		$0x1b, %xmm11, %xmm2
	pshufd		$0x1b, %xmm10, %xmm3
	shufps		$0x1b, %xmm13, %xmm13
	shufps		$0x1b, %xmm12, %xmm12
	movaps		%xmm8, %xmm11
	movaps		%xmm9, %xmm10
	movaps		%xmm14, %xmm4
	movaps		%xmm15, %xmm5
	subps		%xmm2, %xmm11
	subps		%xmm3, %xmm10
	subps		%xmm13, %xmm14
	subps		%xmm12, %xmm15
	addps		%xmm2, %xmm8
	addps		%xmm3, %xmm9
	addps		%xmm5, %xmm12
	addps		%xmm4, %xmm13
	mulps		%xmm0, %xmm11
	mulps		%xmm1, %xmm10
	mulps		%xmm1, %xmm14
	mulps		%xmm0, %xmm15
	
	movaps		96(%rcx), %xmm0
	
	pshufd		$0x1b, %xmm9, %xmm1
	pshufd		$0x1b, %xmm13, %xmm2
	shufps		$0x1b, %xmm10, %xmm10
	shufps		$0x1b, %xmm14, %xmm14
	movaps		%xmm8, %xmm9
	movaps		%xmm12, %xmm13
	movaps		%xmm11, %xmm3
	movaps		%xmm15, %xmm4
	subps		%xmm1, %xmm9
	subps		%xmm2, %xmm13
	subps		%xmm10, %xmm11
	subps		%xmm14, %xmm15
	addps		%xmm1, %xmm8
	addps		%xmm2, %xmm12
	addps		%xmm3, %xmm10
	addps		%xmm4, %xmm14
	mulps		%xmm0, %xmm9
	mulps		%xmm0, %xmm13
	mulps		%xmm0, %xmm11
	mulps		%xmm0, %xmm15
	
	movaps		112(%rcx), %xmm0
	movaps		%xmm0, %xmm1
	movlhps		%xmm1, %xmm1
	
	movaps		%xmm8, %xmm2
	movaps		%xmm9, %xmm3
	shufps		$0x44, %xmm10, %xmm2
	shufps		$0xbb, %xmm11, %xmm9
	shufps		$0xbb, %xmm10, %xmm8
	shufps		$0x44, %xmm11, %xmm3
	movaps		%xmm2, %xmm4
	movaps		%xmm3, %xmm5
	subps		%xmm8, %xmm2
	subps		%xmm9, %xmm3
	addps		%xmm4, %xmm8
	addps		%xmm5, %xmm9
	mulps		%xmm1, %xmm2
	mulps		%xmm1, %xmm3
	movaps		%xmm8, %xmm10
	movaps		%xmm9, %xmm11
	shufps		$0x14, %xmm2, %xmm8
	shufps		$0xbe, %xmm2, %xmm10
	shufps		$0x14, %xmm3, %xmm9
	shufps		$0xbe, %xmm3, %xmm11
	
	movaps		%xmm12, %xmm2
	movaps		%xmm13, %xmm3
	shufps		$0x44, %xmm14, %xmm2
	shufps		$0xbb, %xmm15, %xmm13
	shufps		$0xbb, %xmm14, %xmm12
	shufps		$0x44, %xmm15, %xmm3
	movaps		%xmm2, %xmm4
	movaps		%xmm3, %xmm5
	subps		%xmm12, %xmm2
	subps		%xmm13, %xmm3
	addps		%xmm4, %xmm12
	addps		%xmm5, %xmm13
	mulps		%xmm1, %xmm2
	mulps		%xmm1, %xmm3
	movaps		%xmm12, %xmm14
	movaps		%xmm13, %xmm15
	shufps		$0x14, %xmm2, %xmm12
	shufps		$0xbe, %xmm2, %xmm14
	shufps		$0x14, %xmm3, %xmm13
	shufps		$0xbe, %xmm3, %xmm15
	
	shufps		$0xaa, %xmm0, %xmm0
	pcmpeqd		%xmm1, %xmm1
	pslld		$31, %xmm1
	psllq		$32, %xmm1
	xorps		%xmm1, %xmm0
	
	movaps		%xmm8, %xmm1
	movaps		%xmm10, %xmm2
	unpcklps	%xmm9, %xmm8
	unpckhps	%xmm9, %xmm1
	unpcklps	%xmm11, %xmm10
	unpckhps	%xmm11, %xmm2
	movaps		%xmm8, %xmm3
	movaps		%xmm10, %xmm4
	unpcklps	%xmm1, %xmm8
	unpckhps	%xmm1, %xmm3
	unpcklps	%xmm2, %xmm10
	unpckhps	%xmm2, %xmm4
	movaps		%xmm8, %xmm1
	movaps		%xmm10, %xmm2
	subps		%xmm3, %xmm1
	subps		%xmm4, %xmm2
	addps		%xmm3, %xmm8
	addps		%xmm4, %xmm10
	mulps		%xmm0, %xmm1
	mulps		%xmm0, %xmm2
	movaps		%xmm8, %xmm9
	movaps		%xmm10, %xmm11
	unpcklps	%xmm1, %xmm8
	unpckhps	%xmm1, %xmm9
	unpcklps	%xmm2, %xmm10
	unpckhps	%xmm2, %xmm11
	
	movaps		%xmm12, %xmm1
	movaps		%xmm14, %xmm2
	unpcklps	%xmm13, %xmm12
	unpckhps	%xmm13, %xmm1
	unpcklps	%xmm15, %xmm14
	unpckhps	%xmm15, %xmm2
	movaps		%xmm12, %xmm3
	movaps		%xmm14, %xmm4
	unpcklps	%xmm1, %xmm12
	unpckhps	%xmm1, %xmm3
	unpcklps	%xmm2, %xmm14
	unpckhps	%xmm2, %xmm4
	movaps		%xmm12, %xmm1
	movaps		%xmm14, %xmm2
	subps		%xmm3, %xmm1
	subps		%xmm4, %xmm2
	addps		%xmm3, %xmm12
	addps		%xmm4, %xmm14
	mulps		%xmm0, %xmm1
	mulps		%xmm0, %xmm2
	movaps		%xmm12, %xmm13
	movaps		%xmm14, %xmm15
	unpcklps	%xmm1, %xmm12
	unpckhps	%xmm1, %xmm13
	unpcklps	%xmm2, %xmm14
	unpckhps	%xmm2, %xmm15
	
	
	xorps		%xmm0, %xmm0
	xorps		%xmm1, %xmm1
	shufpd		$0x2, %xmm8, %xmm0
	shufpd		$0x2, %xmm9, %xmm1
	psrlq		$32, %xmm0
	psrlq		$32, %xmm1
	addps		%xmm0, %xmm8
	addps		%xmm1, %xmm9
	
	xorps		%xmm0, %xmm0
	xorps		%xmm1, %xmm1
	shufpd		$0x2, %xmm10, %xmm0
	shufpd		$0x2, %xmm11, %xmm1
	psrlq		$32, %xmm0
	psrlq		$32, %xmm1
	addps		%xmm0, %xmm10
	addps		%xmm1, %xmm11
	
	xorps		%xmm0, %xmm0
	xorps		%xmm1, %xmm1
	shufpd		$0x2, %xmm12, %xmm0
	shufpd		$0x2, %xmm13, %xmm1
	psrlq		$32, %xmm0
	psrlq		$32, %xmm1
	addps		%xmm0, %xmm12
	addps		%xmm1, %xmm13
	
	xorps		%xmm0, %xmm0
	xorps		%xmm1, %xmm1
	shufpd		$0x2, %xmm14, %xmm0
	shufpd		$0x2, %xmm15, %xmm1
	psrlq		$32, %xmm0
	psrlq		$32, %xmm1
	addps		%xmm0, %xmm14
	addps		%xmm1, %xmm15
	
	pshufd		$0x78, %xmm9, %xmm0
	pshufd		$0x78, %xmm11, %xmm1
	pshufd		$0x78, %xmm13, %xmm2
	pshufd		$0x78, %xmm15, %xmm3
	psrldq		$4, %xmm0
	psrldq		$4, %xmm1
	psrldq		$4, %xmm2
	psrldq		$4, %xmm3
	addps		%xmm0, %xmm9
	addps		%xmm1, %xmm11
	addps		%xmm2, %xmm13
	addps		%xmm3, %xmm15
	
	pshufd		$0x78, %xmm10, %xmm0
	pshufd		$0x78, %xmm14, %xmm1
	psrldq		$4, %xmm0
	psrldq		$4, %xmm1
	addps		%xmm11, %xmm10
	addps		%xmm15, %xmm14
	addps		%xmm0, %xmm11
	addps		%xmm1, %xmm15
	
	
	movss		%xmm8, 1024(%r9)
	movss		%xmm10, 896(%r9)
	movss		%xmm9, 768(%r9)
	movss		%xmm11, 640(%r9)
	movhlps		%xmm8, %xmm0
	movhlps		%xmm10, %xmm1
	movhlps		%xmm9, %xmm2
	movhlps		%xmm11, %xmm3
	movss		%xmm0, 512(%r9)
	movss		%xmm1, 384(%r9)
	movss		%xmm2, 256(%r9)
	movss		%xmm3, 128(%r9)
	
	pshuflw		$0xee, %xmm8, %xmm4
	pshuflw		$0xee, %xmm10, %xmm5
	pshuflw		$0xee, %xmm9, %xmm6
	pshuflw		$0xee, %xmm11, %xmm7
	movss		%xmm4, (%r9)
	movss		%xmm4, (%rdx)
	movss		%xmm5, 128(%rdx)
	movss		%xmm6, 256(%rdx)
	movss		%xmm7, 384(%rdx)
	
	pshuflw		$0xee, %xmm0, %xmm0
	pshuflw		$0xee, %xmm1, %xmm1
	pshuflw		$0xee, %xmm2, %xmm2
	pshuflw		$0xee, %xmm3, %xmm3
	movss		%xmm0, 512(%rdx)
	movss		%xmm1, 640(%rdx)
	movss		%xmm2, 768(%rdx)
	movss		%xmm3, 896(%rdx)
	
	pshufd		$0x78, %xmm12, %xmm0
	movaps		%xmm13, %xmm1
	psrldq		$4, %xmm0
	
	addps		%xmm14, %xmm12
	addps		%xmm15, %xmm13
	addps		%xmm1, %xmm14
	addps		%xmm0, %xmm15
	
	movss		%xmm12, 960(%r9)
	movss		%xmm14, 832(%r9)
	movss		%xmm13, 704(%r9)
	movss		%xmm15, 576(%r9)
	movhlps		%xmm12, %xmm0
	movhlps		%xmm14, %xmm1
	movhlps		%xmm13, %xmm2
	movhlps		%xmm15, %xmm3
	movss		%xmm0, 448(%r9)
	movss		%xmm1, 320(%r9)
	movss		%xmm2, 192(%r9)
	movss		%xmm3, 64(%r9)
	
	pshuflw		$0xee, %xmm12, %xmm4
	pshuflw		$0xee, %xmm14, %xmm5
	pshuflw		$0xee, %xmm13, %xmm6
	pshuflw		$0xee, %xmm15, %xmm7
	movss		%xmm4, 64(%rdx)
	movss		%xmm5, 192(%rdx)
	movss		%xmm6, 320(%rdx)
	movss		%xmm7, 448(%rdx)
	
	pshuflw		$0xee, %xmm0, %xmm0
	pshuflw		$0xee, %xmm1, %xmm1
	pshuflw		$0xee, %xmm2, %xmm2
	pshuflw		$0xee, %xmm3, %xmm3
	movss		%xmm0, 576(%rdx)
	movss		%xmm1, 704(%rdx)
	movss		%xmm2, 832(%rdx)
	movss		%xmm3, 960(%rdx)
	

	movaps		(%rsp), %xmm6
	movaps		16(%rsp), %xmm7
	movaps		32(%rsp), %xmm8
	movaps		48(%rsp), %xmm9
	movaps		64(%rsp), %xmm10
	movaps		80(%rsp), %xmm11
	movaps		96(%rsp), %xmm12
	movaps		112(%rsp), %xmm13
	movaps		128(%rsp), %xmm14
	movaps		144(%rsp), %xmm15
	addq		$168, %rsp
#line 424 "C:/CProjects/mpg123/ports/cmake/src/libmpg123/../../../../src/libmpg123/dct64_x86_64_float.S"
	ret


