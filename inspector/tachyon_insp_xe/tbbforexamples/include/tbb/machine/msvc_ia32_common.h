/*

!==============================================================
!
! SAMPLE SOURCE CODE - SUBJECT TO THE TERMS OF SAMPLE CODE LICENSE AGREEMENT,
! http://software.intel.com/en-us/articles/intel-sample-source-code-license-agreement/
!
! Copyright 2016 Intel Corporation
!
! THIS FILE IS PROVIDED "AS IS" WITH NO WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT
! NOT LIMITED TO ANY IMPLIED WARRANTY OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
! PURPOSE, NON-INFRINGEMENT OF INTELLECTUAL PROPERTY RIGHTS.
!
! =============================================================
*/

#ifndef __TBB_machine_msvc_ia32_common_H
#define __TBB_machine_msvc_ia32_common_H

#include <intrin.h>

//TODO: consider moving this macro to tbb_config.h and used there MSVC asm is used
#if  !_M_X64 || __INTEL_COMPILER
    #define __TBB_X86_MSVC_INLINE_ASM_AVAILABLE 1

    #if _M_X64
        #define __TBB_r(reg_name) r##reg_name
    #else
        #define __TBB_r(reg_name) e##reg_name
    #endif
#else
    //MSVC in x64 mode does not accept inline assembler
    #define __TBB_X86_MSVC_INLINE_ASM_AVAILABLE 0
#endif


#define __TBB_NO_X86_MSVC_INLINE_ASM_MSG "The compiler being used is not supported (outdated?)"

#if (_MSC_VER >= 1300) || (__INTEL_COMPILER) //Use compiler intrinsic when available
    #define __TBB_PAUSE_USE_INTRINSIC 1
    #pragma intrinsic(_mm_pause)
    namespace tbb { namespace internal { namespace intrinsics { namespace msvc {
        static inline void __TBB_machine_pause (uintptr_t delay ) {
            for (;delay>0; --delay )
                _mm_pause();
        }
    }}}}
#else
    #if !__TBB_X86_MSVC_INLINE_ASM_AVAILABLE
        #error __TBB_NO_X86_MSVC_INLINE_ASM_MSG
    #endif

    namespace tbb { namespace internal { namespace inline_asm { namespace msvc {
        static inline void __TBB_machine_pause (uintptr_t delay ) {
            _asm
            {
                mov __TBB_r(ax), delay
              __TBB_L1:
                pause
                add __TBB_r(ax), -1
                jne __TBB_L1
            }
            return;
        }
    }}}}
#endif

static inline void __TBB_machine_pause (uintptr_t delay ){
    #if __TBB_PAUSE_USE_INTRINSIC
        tbb::internal::intrinsics::msvc::__TBB_machine_pause(delay);
    #else
        tbb::internal::inline_asm::msvc::__TBB_machine_pause(delay);
    #endif
}

//TODO: move this function to windows_api.h or to place where it is used
#if (_MSC_VER<1400) && (!_WIN64) && (__TBB_X86_MSVC_INLINE_ASM_AVAILABLE)
    static inline void* __TBB_machine_get_current_teb () {
        void* pteb;
        __asm mov eax, fs:[0x18]
        __asm mov pteb, eax
        return pteb;
    }
#endif

#if ( _MSC_VER>=1400 && !defined(__INTEL_COMPILER) ) ||  (__INTEL_COMPILER>=1200)
// MSVC did not have this intrinsic prior to VC8.
// ICL 11.1 fails to compile a TBB example if __TBB_Log2 uses the intrinsic.
    #define __TBB_LOG2_USE_BSR_INTRINSIC 1
    #if _M_X64
        #define __TBB_BSR_INTRINSIC _BitScanReverse64
    #else
        #define __TBB_BSR_INTRINSIC _BitScanReverse
    #endif
    #pragma intrinsic(__TBB_BSR_INTRINSIC)

    namespace tbb { namespace internal { namespace intrinsics { namespace msvc {
        inline uintptr_t __TBB_machine_lg( uintptr_t i ){
            unsigned long j;
            __TBB_BSR_INTRINSIC( &j, i );
            return j;
        }
    }}}}
#else
    #if !__TBB_X86_MSVC_INLINE_ASM_AVAILABLE
        #error __TBB_NO_X86_MSVC_INLINE_ASM_MSG
    #endif

    namespace tbb { namespace internal { namespace inline_asm { namespace msvc {
        inline uintptr_t __TBB_machine_lg( uintptr_t i ){
            uintptr_t j;
            __asm
            {
                bsr __TBB_r(ax), i
                mov j, __TBB_r(ax)
            }
            return j;
        }
    }}}}
#endif

static inline intptr_t __TBB_machine_lg( uintptr_t i ) {
#if __TBB_LOG2_USE_BSR_INTRINSIC
    return tbb::internal::intrinsics::msvc::__TBB_machine_lg(i);
#else
    return tbb::internal::inline_asm::msvc::__TBB_machine_lg(i);
#endif
}

// API to retrieve/update FPU control setting
#define __TBB_CPU_CTL_ENV_PRESENT 1
struct __TBB_cpu_ctl_env_t {
    int     mxcsr;
    short   x87cw;
};
#if __TBB_X86_MSVC_INLINE_ASM_AVAILABLE
    inline void __TBB_get_cpu_ctl_env ( __TBB_cpu_ctl_env_t* ctl ) {
        __asm {
            __asm mov     __TBB_r(ax), ctl
            __asm stmxcsr [__TBB_r(ax)]
            __asm fstcw   [__TBB_r(ax)+4]
        }
    }
    inline void __TBB_set_cpu_ctl_env ( const __TBB_cpu_ctl_env_t* ctl ) {
        __asm {
            __asm mov     __TBB_r(ax), ctl
            __asm ldmxcsr [__TBB_r(ax)]
            __asm fldcw   [__TBB_r(ax)+4]
        }
    }
#else
    extern "C" {
        void __TBB_EXPORTED_FUNC __TBB_get_cpu_ctl_env ( __TBB_cpu_ctl_env_t* );
        void __TBB_EXPORTED_FUNC __TBB_set_cpu_ctl_env ( const __TBB_cpu_ctl_env_t* );
    }
#endif

extern "C" __declspec(dllimport) int __stdcall SwitchToThread( void );
#define __TBB_Yield()  SwitchToThread()

#define __TBB_Pause(V) __TBB_machine_pause(V)
#define __TBB_Log2(V)  __TBB_machine_lg(V)

#undef __TBB_r

#endif /* __TBB_machine_msvc_ia32_common_H */
