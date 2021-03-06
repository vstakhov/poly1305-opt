#ifndef BASE_GCC_ARM_S
#define BASE_GCC_ARM_S

#include "asmopt_internal.h"

#if !defined(HAVE_SLASHMACRO) && !defined(HAVE_DOLLARMACRO)
	#error Unknown gnu as macro parameter convention! Run ./configure
#endif

.syntax unified
.arm

#define IS_ARM32 (defined(__arm__))
#define IS_ARM64 (defined(__aarch64__))
#define IS_ELF (defined(__ELF__))
#define IS_MACH (defined(__MACH__))

#if (IS_ELF)
	.macro FN name
		.align 2
		\name:
		_\name:
	.endm

	.macro FN_END name
		.size \name, .-\name
		.type \name STT_FUNC
		.size _\name, .-_\name
		.type _\name STT_FUNC
	.endm

	.macro HIDDEN name
	#if defined(HAVE_AS_HIDDEN)
		.hidden \name
		.hidden _\name
	#endif
	.endm

	/* set NX for stack */
	.section .note.GNU-stack,"",%progbits
#elif (IS_MACH)
	.macro FN name
		.align 2
	#if defined(HAVE_SLASHMACRO)
		\name:
		_\name:
	#elif defined(HAVE_DOLLARMACRO)
		$0:
		_$0:
	#endif
	.endm

	.macro FN_END name
	.endm

	.macro HIDDEN name
	#if defined(HAVE_AS_PRIVATE_EXTERN)
	#if defined(HAVE_SLASHMACRO)
		.private_extern \name
		.private_extern _\name
	#elif defined(HAVE_DOLLARMACRO)
		.private_extern $0
		.private_extern _$0
	#endif
	#endif
	.endm
#endif

/* put everything in the code segment to simplify things */
#if (IS_MACH)
	.macro SECTION_TEXT
		.section __TEXT,__text,regular
	.endm

	.macro SECTION_RODATA
		.section __TEXT,__text,regular
	.endm
#else
	/* put everything in the code segment to simplify things */
	.macro SECTION_TEXT
		.text
	.endm

	.macro SECTION_RODATA
		.text
	.endm
#endif

/* declare a global function */
.macro GLOBAL name
#if defined(HAVE_SLASHMACRO)
	.globl \name
	.globl _\name
#elif defined(HAVE_DOLLARMACRO)
	.globl $0
	.globl _$0
#endif
.endm

.macro FN_LOCAL_PREFIX name
#if defined(HAVE_SLASHMACRO)
	FN LOCAL_PREFIX(\name)
#elif defined(HAVE_DOLLARMACRO)
	FN LOCAL_PREFIX($0)
#endif
.endm

.macro FN_END_LOCAL_PREFIX name
#if defined(HAVE_SLASHMACRO)
	FN_END LOCAL_PREFIX(\name)
#elif defined(HAVE_DOLLARMACRO)
	FN_END LOCAL_PREFIX($0)
#endif
.endm

.macro GLOBAL_LOCAL_PREFIX name
#if defined(HAVE_SLASHMACRO)
	GLOBAL LOCAL_PREFIX(\name)
	HIDDEN LOCAL_PREFIX(\name)
#elif defined(HAVE_DOLLARMACRO)
	GLOBAL LOCAL_PREFIX($0)
	HIDDEN LOCAL_PREFIX($0)
#endif
.endm

.macro GLOBAL_HIDDEN_FN name
#if defined(HAVE_SLASHMACRO)
	GLOBAL \name
	HIDDEN \name
	FN \name
#elif defined(HAVE_DOLLARMACRO)
	GLOBAL $0
	HIDDEN $0
	FN $0
#endif
.endm

/* pic support */
.macro LOAD_VAR_PIC var, reg
#if (IS_ARM32)
	#if defined(HAVE_SLASHMACRO)
		adrl \reg, \var
	#elif defined(HAVE_DOLLARMACRO)
		adrl $1, $0
	#endif
#elif (IS_ARM64)
	#if defined(HAVE_SLASHMACRO)
		adr \reg, \var
	#elif defined(HAVE_DOLLARMACRO)
		adr $1, $0
	#endif
#endif
.endm

#if defined(HAVE_SLASHMACRO)
	#define INCLUDE_FILE_PARM "\file"
#elif defined(HAVE_DOLLARMACRO)
	#define INCLUDE_FILE_PARM $0
#endif

.macro INCLUDE file
	.include  INCLUDE_FILE_PARM
.endm

/* include the file with the variable(s) if variable 'name' is not already included */
.macro INCLUDE_VAR_FILE file, name
	#if defined(HAVE_SLASHMACRO)
		.ifndef \name
			.include INCLUDE_FILE_PARM
		.endif
	#elif defined(HAVE_DOLLARMACRO)
		.ifndef $1
			.include INCLUDE_FILE_PARM
		.endif
	#endif
.endm

#endif /* BASE_GCC_ARM_S */
