#line 1 "C:/CProjects/mpg123/ports/cmake/src/libmpg123/../../../../src/libmpg123/dct64_sse_float.S"








#line 1 "c:\\cprojects\\mpg123\\src\\libmpg123\\mangle.h"












#line 1 "C:/CProjects/mpg123/build2/src\\config.h"



































































































































































#line 165 "C:/CProjects/mpg123/build2/src\\config.h"











#line 14 "c:\\cprojects\\mpg123\\src\\libmpg123\\mangle.h"
#line 1 "C:/CProjects/mpg123/ports/cmake/../../src\\intsym.h"




#line 1 "C:/CProjects/mpg123/build2/src\\config.h"



































































































































































#line 165 "C:/CProjects/mpg123/build2/src\\config.h"











#line 6 "C:/CProjects/mpg123/ports/cmake/../../src\\intsym.h"









































































































































































































































































































































































#line 368 "C:/CProjects/mpg123/ports/cmake/../../src\\intsym.h"
#line 15 "c:\\cprojects\\mpg123\\src\\libmpg123\\mangle.h"



#line 22 "c:\\cprojects\\mpg123\\src\\libmpg123\\mangle.h"





#line 28 "c:\\cprojects\\mpg123\\src\\libmpg123\\mangle.h"











































#line 72 "c:\\cprojects\\mpg123\\src\\libmpg123\\mangle.h"







#line 80 "c:\\cprojects\\mpg123\\src\\libmpg123\\mangle.h"





#line 87 "c:\\cprojects\\mpg123\\src\\libmpg123\\mangle.h"


























#line 114 "c:\\cprojects\\mpg123\\src\\libmpg123\\mangle.h"














#line 129 "c:\\cprojects\\mpg123\\src\\libmpg123\\mangle.h"











#line 141 "c:\\cprojects\\mpg123\\src\\libmpg123\\mangle.h"




#line 146 "c:\\cprojects\\mpg123\\src\\libmpg123\\mangle.h"

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

#line 10 "C:/CProjects/mpg123/ports/cmake/src/libmpg123/../../../../src/libmpg123/dct64_sse_float.S"










	.section	.rodata


#line 24 "C:/CProjects/mpg123/ports/cmake/src/libmpg123/../../../../src/libmpg123/dct64_sse_float.S"
	.balign 16
pnpn:
	.long	0
	.long	-2147483648
	.long	0
	.long	-2147483648
	.balign 16
mask:
	.long	-1
	.long	-1
	.long	-1
	.long	0
	
	.text
	.balign 16
.globl _INT123_dct64_real_sse
_INT123_dct64_real_sse:
	pushl		%ebp
	movl		%esp, %ebp
	
	andl		$-16, %esp 
	subl		$128, %esp 
	pushl		%ebx

	
	
	movl		(8+2*4)(%ebp), %eax
	
	movups 	(%eax), %xmm7
	movups 	16(%eax), %xmm6
	movups 	112(%eax), %xmm0
	movups 	96(%eax), %xmm1
	shufps 		$0x1b, %xmm0, %xmm0
	shufps 		$0x1b, %xmm1, %xmm1
	movaps 		%xmm7, %xmm4
	movaps		%xmm6, %xmm5
	addps 		%xmm0, %xmm4
	addps 		%xmm1, %xmm5
	subps 		%xmm0, %xmm7
	subps 		%xmm1, %xmm6
	movaps		%xmm4, (4+0*16)(%esp)
	movaps		%xmm5, (4+1*16)(%esp)
	
	movups 	32(%eax), %xmm2
	movups 	48(%eax), %xmm3
	movups 	80(%eax), %xmm0
	movups 	64(%eax), %xmm1
	shufps 		$0x1b, %xmm0, %xmm0
	shufps 		$0x1b, %xmm1, %xmm1
	movaps 		%xmm2, %xmm5
	movaps		%xmm3, %xmm4
	addps 		%xmm0, %xmm2
	addps 		%xmm1, %xmm3
	subps 		%xmm0, %xmm5
	subps 		%xmm1, %xmm4
	



	lea			_INT123_costab_mmxsse, %eax
#line 85 "C:/CProjects/mpg123/ports/cmake/src/libmpg123/../../../../src/libmpg123/dct64_sse_float.S"
	mulps		(%eax), %xmm7
	mulps		16(%eax), %xmm6
	mulps		32(%eax), %xmm5
	mulps		48(%eax), %xmm4
	
	shufps		$0x1b, %xmm2, %xmm2
	shufps		$0x1b, %xmm3, %xmm3
	shufps		$0x1b, %xmm4, %xmm4
	shufps		$0x1b, %xmm5, %xmm5
	movaps		(4+0*16)(%esp), %xmm0
	movaps		(4+1*16)(%esp), %xmm1
	subps		%xmm3, %xmm0
	subps		%xmm2, %xmm1
	addps		(4+0*16)(%esp), %xmm3
	addps		(4+1*16)(%esp), %xmm2
	movaps		%xmm3, (4+0*16)(%esp)
	movaps		%xmm2, (4+1*16)(%esp)
	movaps		%xmm6, %xmm2
	movaps		%xmm7, %xmm3
	subps		%xmm5, %xmm6
	subps		%xmm4, %xmm7
	addps		%xmm3, %xmm4
	addps		%xmm2, %xmm5
	mulps		64(%eax), %xmm0
	mulps		80(%eax), %xmm1
	mulps		80(%eax), %xmm6
	mulps		64(%eax), %xmm7
	
	movaps		(4+0*16)(%esp), %xmm2
	movaps		(4+1*16)(%esp), %xmm3
	shufps		$0x1b, %xmm3, %xmm3
	shufps		$0x1b, %xmm5, %xmm5
	shufps		$0x1b, %xmm1, %xmm1
	shufps		$0x1b, %xmm6, %xmm6
	movaps		%xmm0, (4+1*16)(%esp)
	subps		%xmm3, %xmm2
	subps		%xmm1, %xmm0
	addps		(4+0*16)(%esp), %xmm3
	addps		(4+1*16)(%esp), %xmm1
	movaps		%xmm3, (4+0*16)(%esp)
	movaps		%xmm1, (4+2*16)(%esp)
	movaps		%xmm5, %xmm1
	movaps		%xmm4, %xmm5
	movaps		%xmm7, %xmm3
	subps		%xmm1, %xmm5
	subps		%xmm6, %xmm7
	addps		%xmm1, %xmm4
	addps		%xmm3, %xmm6
	mulps		96(%eax), %xmm2
	mulps		96(%eax), %xmm0
	mulps		96(%eax), %xmm5
	mulps		96(%eax), %xmm7
	movaps		%xmm2, (4+1*16)(%esp)
	movaps		%xmm0, (4+3*16)(%esp)
	
	movaps		%xmm4, %xmm2
	movaps		%xmm5, %xmm3
	shufps		$0x44, %xmm6, %xmm2
	shufps		$0xbb, %xmm7, %xmm5
	shufps		$0xbb, %xmm6, %xmm4
	shufps		$0x44, %xmm7, %xmm3
	movaps		%xmm2, %xmm6
	movaps		%xmm3, %xmm7
	subps		%xmm4, %xmm2
	subps		%xmm5, %xmm3
	addps		%xmm6, %xmm4
	addps		%xmm7, %xmm5
	movaps		112(%eax), %xmm0
	movlhps		%xmm0, %xmm0
	mulps		%xmm0, %xmm2
	mulps		%xmm0, %xmm3
	movaps		%xmm0, (4+4*16)(%esp)
	movaps		%xmm4, %xmm6
	movaps		%xmm5, %xmm7
	shufps		$0x14, %xmm2, %xmm4
	shufps		$0xbe, %xmm2, %xmm6
	shufps		$0x14, %xmm3, %xmm5
	shufps		$0xbe, %xmm3, %xmm7
	movaps		%xmm5, (4+5*16)(%esp)
	movaps		%xmm7, (4+7*16)(%esp)
	
	movaps		(4+0*16)(%esp), %xmm0
	movaps		(4+1*16)(%esp), %xmm1
	movaps		%xmm0, %xmm2
	movaps		%xmm1, %xmm3
	shufps		$0x44, (4+2*16)(%esp), %xmm2
	shufps		$0xbb, (4+3*16)(%esp), %xmm1
	shufps		$0xbb, (4+2*16)(%esp), %xmm0
	shufps		$0x44, (4+3*16)(%esp), %xmm3
	movaps		%xmm2, %xmm5
	movaps		%xmm3, %xmm7
	subps		%xmm0, %xmm2
	subps		%xmm1, %xmm3
	addps		%xmm5, %xmm0
	addps		%xmm7, %xmm1
	mulps		(4+4*16)(%esp), %xmm2
	mulps		(4+4*16)(%esp), %xmm3
	movaps		%xmm0, %xmm5
	movaps		%xmm1, %xmm7
	shufps		$0x14, %xmm2, %xmm0
	shufps		$0xbe, %xmm2, %xmm5
	shufps		$0x14, %xmm3, %xmm1
	shufps		$0xbe, %xmm3, %xmm7
	
	movaps		%xmm0, (4+0*16)(%esp)
	movaps		%xmm1, (4+1*16)(%esp)
	movaps		%xmm5, (4+2*16)(%esp)
	movaps		%xmm7, (4+3*16)(%esp)
	
	movss		120(%eax), %xmm5
	shufps		$0x00, %xmm5, %xmm5
	xorps		pnpn, %xmm5
	
	movaps		%xmm4, %xmm0
	movaps		%xmm6, %xmm1
	unpcklps	(4+5*16)(%esp), %xmm4
	unpckhps	(4+5*16)(%esp), %xmm0
	unpcklps	(4+7*16)(%esp), %xmm6
	unpckhps	(4+7*16)(%esp), %xmm1
	movaps		%xmm4, %xmm2
	movaps		%xmm6, %xmm3
	unpcklps	%xmm0, %xmm4
	unpckhps	%xmm0, %xmm2
	unpcklps	%xmm1, %xmm6
	unpckhps	%xmm1, %xmm3
	movaps		%xmm4, %xmm0
	movaps		%xmm6, %xmm1
	subps		%xmm2, %xmm0
	subps		%xmm3, %xmm1
	addps		%xmm2, %xmm4
	addps		%xmm3, %xmm6
	mulps		%xmm5, %xmm0
	mulps		%xmm5, %xmm1
	movaps		%xmm5, (4+5*16)(%esp)
	movaps		%xmm4, %xmm5
	movaps		%xmm6, %xmm7
	unpcklps	%xmm0, %xmm4
	unpckhps	%xmm0, %xmm5
	unpcklps	%xmm1, %xmm6
	unpckhps	%xmm1, %xmm7
	
	movaps		(4+0*16)(%esp), %xmm0
	movaps		(4+2*16)(%esp), %xmm2
	movaps		%xmm4, (4+4*16)(%esp)
	movaps		%xmm6, (4+6*16)(%esp)
	
	movaps		%xmm0, %xmm4
	movaps		%xmm2, %xmm6
	unpcklps	(4+1*16)(%esp), %xmm0
	unpckhps	(4+1*16)(%esp), %xmm4
	unpcklps	(4+3*16)(%esp), %xmm2
	unpckhps	(4+3*16)(%esp), %xmm6
	movaps		%xmm0, %xmm1
	movaps		%xmm2, %xmm3
	unpcklps	%xmm4, %xmm0
	unpckhps	%xmm4, %xmm1
	unpcklps	%xmm6, %xmm2
	unpckhps	%xmm6, %xmm3
	movaps		%xmm0, %xmm4
	movaps		%xmm2, %xmm6
	subps		%xmm1, %xmm4
	subps		%xmm3, %xmm6
	addps		%xmm1, %xmm0
	addps		%xmm3, %xmm2
	mulps		(4+5*16)(%esp), %xmm4
	mulps		(4+5*16)(%esp), %xmm6
	movaps		%xmm0, %xmm1
	movaps		%xmm2, %xmm3
	unpcklps	%xmm4, %xmm0
	unpckhps	%xmm4, %xmm1
	unpcklps	%xmm6, %xmm2
	unpckhps	%xmm6, %xmm3
	
	movaps		%xmm0, (4+0*16)(%esp)
	movaps		%xmm1, (4+1*16)(%esp)
	movaps		%xmm2, (4+2*16)(%esp)
	movaps		%xmm3, (4+3*16)(%esp)
	movaps		%xmm5, (4+5*16)(%esp)
	movaps		%xmm7, (4+7*16)(%esp)
	
	movss		(4+12)(%esp), %xmm0
	movss		(4+28)(%esp), %xmm1
	movss		(4+44)(%esp), %xmm2
	movss		(4+60)(%esp), %xmm3
	addss		(4+8)(%esp), %xmm0
	addss		(4+24)(%esp), %xmm1
	addss		(4+40)(%esp), %xmm2
	addss		(4+56)(%esp), %xmm3
	movss		%xmm0, (4+8)(%esp)
	movss		%xmm1, (4+24)(%esp)
	movss		%xmm2, (4+40)(%esp)
	movss		%xmm3, (4+56)(%esp)
	movss		(4+76)(%esp), %xmm0
	movss		(4+92)(%esp), %xmm1
	movss		(4+108)(%esp), %xmm2
	movss		(4+124)(%esp), %xmm3
	addss		(4+72)(%esp), %xmm0
	addss		(4+88)(%esp), %xmm1
	addss		(4+104)(%esp), %xmm2
	addss		(4+120)(%esp), %xmm3
	movss		%xmm0, (4+72)(%esp)
	movss		%xmm1, (4+88)(%esp)
	movss		%xmm2, (4+104)(%esp)
	movss		%xmm3, (4+120)(%esp)
	
	movaps		(4+16)(%esp), %xmm1
	movaps		(4+48)(%esp), %xmm3
	movaps		(4+80)(%esp), %xmm5
	movaps		(4+112)(%esp), %xmm7
	movaps		%xmm1, %xmm0
	movaps		%xmm3, %xmm2
	movaps		%xmm5, %xmm4
	movaps		%xmm7, %xmm6
	shufps		$0x1e, %xmm0, %xmm0
	shufps		$0x1e, %xmm2, %xmm2
	shufps		$0x1e, %xmm4, %xmm4
	shufps		$0x1e, %xmm6, %xmm6
	lea			mask, %eax
	andps		(%eax), %xmm0
	andps		(%eax), %xmm2
	andps		(%eax), %xmm4
	andps		(%eax), %xmm6
	addps		%xmm0, %xmm1
	addps		%xmm2, %xmm3
	addps		%xmm4, %xmm5
	addps		%xmm6, %xmm7
	
	movaps		(4+32)(%esp), %xmm2
	movaps		(4+96)(%esp), %xmm6
	movaps		%xmm2, %xmm0
	movaps		%xmm6, %xmm4
	shufps		$0x1e, %xmm0, %xmm0
	shufps		$0x1e, %xmm4, %xmm4
	andps		(%eax), %xmm0
	andps		(%eax), %xmm4
	addps		%xmm3, %xmm2
	addps		%xmm0, %xmm3
	addps		%xmm7, %xmm6
	addps		%xmm4, %xmm7
	
	movaps		(4+0)(%esp), %xmm0
	movaps		(4+64)(%esp), %xmm4
	
	movl		(8+0*4)(%ebp), %ecx
	movl		(8+1*4)(%ebp), %ebx
	
	movss		%xmm0, 1024(%ecx)
	movss		%xmm2, 896(%ecx)
	movss		%xmm1, 768(%ecx)
	movss		%xmm3, 640(%ecx)
	
	shufps		$0xe1, %xmm0, %xmm0
	shufps		$0xe1, %xmm2, %xmm2
	shufps		$0xe1, %xmm1, %xmm1
	shufps		$0xe1, %xmm3, %xmm3
	movss		%xmm0, (%ecx)
	movss		%xmm0, (%ebx)
	movss		%xmm2, 128(%ebx)
	movss		%xmm1, 256(%ebx)
	movss		%xmm3, 384(%ebx)
	
	movhlps		%xmm0, %xmm0
	movhlps		%xmm2, %xmm2
	movhlps		%xmm1, %xmm1
	movhlps		%xmm3, %xmm3
	movss		%xmm0, 512(%ecx)
	movss		%xmm2, 384(%ecx)
	movss		%xmm1, 256(%ecx)
	movss		%xmm3, 128(%ecx)
	
	shufps		$0xe1, %xmm0, %xmm0
	shufps		$0xe1, %xmm2, %xmm2
	shufps		$0xe1, %xmm1, %xmm1
	shufps		$0xe1, %xmm3, %xmm3
	movss		%xmm0, 512(%ebx)
	movss		%xmm2, 640(%ebx)
	movss		%xmm1, 768(%ebx)
	movss		%xmm3, 896(%ebx)
	
	movaps		%xmm4, %xmm0
	shufps		$0x1e, %xmm0, %xmm0
	movaps		%xmm5, %xmm1
	andps		(%eax), %xmm0
	
	addps		%xmm6, %xmm4
	addps		%xmm7, %xmm5
	addps		%xmm1, %xmm6
	addps		%xmm0, %xmm7
	
	movss		%xmm4, 960(%ecx)
	movss		%xmm6, 832(%ecx)
	movss		%xmm5, 704(%ecx)
	movss		%xmm7, 576(%ecx)
	movhlps		%xmm4, %xmm0
	movhlps		%xmm6, %xmm1
	movhlps		%xmm5, %xmm2
	movhlps		%xmm7, %xmm3
	movss		%xmm0, 448(%ecx)
	movss		%xmm1, 320(%ecx)
	movss		%xmm2, 192(%ecx)
	movss		%xmm3, 64(%ecx)
	
	shufps		$0xe1, %xmm4, %xmm4
	shufps		$0xe1, %xmm6, %xmm6
	shufps		$0xe1, %xmm5, %xmm5
	shufps		$0xe1, %xmm7, %xmm7
	movss		%xmm4, 64(%ebx)
	movss		%xmm6, 192(%ebx)
	movss		%xmm5, 320(%ebx)
	movss		%xmm7, 448(%ebx)
	
	shufps		$0xe1, %xmm0, %xmm0
	shufps		$0xe1, %xmm1, %xmm1
	shufps		$0xe1, %xmm2, %xmm2
	shufps		$0xe1, %xmm3, %xmm3
	movss		%xmm0, 576(%ebx)
	movss		%xmm1, 704(%ebx)
	movss		%xmm2, 832(%ebx)
	movss		%xmm3, 960(%ebx)

	popl		%ebx
	movl		%ebp, %esp
	popl		%ebp
	ret






#line 416 "C:/CProjects/mpg123/ports/cmake/src/libmpg123/../../../../src/libmpg123/dct64_sse_float.S"


