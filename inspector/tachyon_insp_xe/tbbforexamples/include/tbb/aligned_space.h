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

#ifndef __TBB_aligned_space_H
#define __TBB_aligned_space_H

#include "tbb_stddef.h"
#include "tbb_machine.h"

namespace tbb {

//! Block of space aligned sufficiently to construct an array T with N elements.
/** The elements are not constructed or destroyed by this class.
    @ingroup memory_allocation */
template<typename T,size_t N>
class aligned_space {
private:
    typedef __TBB_TypeWithAlignmentAtLeastAsStrict(T) element_type;
    element_type array[(sizeof(T)*N+sizeof(element_type)-1)/sizeof(element_type)];
public:
    //! Pointer to beginning of array
    T* begin() {return internal::punned_cast<T*>(this);}

    //! Pointer to one past last element in array.
    T* end() {return begin()+N;}
};

} // namespace tbb 

#endif /* __TBB_aligned_space_H */
