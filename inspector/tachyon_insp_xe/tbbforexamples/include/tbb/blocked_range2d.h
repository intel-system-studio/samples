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

#ifndef __TBB_blocked_range2d_H
#define __TBB_blocked_range2d_H

#include "tbb_stddef.h"
#include "blocked_range.h"

namespace tbb {

//! A 2-dimensional range that models the Range concept.
/** @ingroup algorithms */
template<typename RowValue, typename ColValue=RowValue>
class blocked_range2d {
public:
    //! Type for size of an iteration range
    typedef blocked_range<RowValue> row_range_type;
    typedef blocked_range<ColValue> col_range_type;
 
private:
    row_range_type my_rows;
    col_range_type my_cols;

public:

    blocked_range2d( RowValue row_begin, RowValue row_end, typename row_range_type::size_type row_grainsize,
                     ColValue col_begin, ColValue col_end, typename col_range_type::size_type col_grainsize ) : 
        my_rows(row_begin,row_end,row_grainsize),
        my_cols(col_begin,col_end,col_grainsize)
    {
    }

    blocked_range2d( RowValue row_begin, RowValue row_end,
                     ColValue col_begin, ColValue col_end ) : 
        my_rows(row_begin,row_end),
        my_cols(col_begin,col_end)
    {
    }

    //! True if range is empty
    bool empty() const {
        // Yes, it is a logical OR here, not AND.
        return my_rows.empty() || my_cols.empty();
    }

    //! True if range is divisible into two pieces.
    bool is_divisible() const {
        return my_rows.is_divisible() || my_cols.is_divisible();
    }

    blocked_range2d( blocked_range2d& r, split ) : 
        my_rows(r.my_rows),
        my_cols(r.my_cols)
    {
        if( my_rows.size()*double(my_cols.grainsize()) < my_cols.size()*double(my_rows.grainsize()) ) {
            my_cols.my_begin = col_range_type::do_split(r.my_cols);
        } else {
            my_rows.my_begin = row_range_type::do_split(r.my_rows);
        }
    }

    //! The rows of the iteration space 
    const row_range_type& rows() const {return my_rows;}

    //! The columns of the iteration space 
    const col_range_type& cols() const {return my_cols;}
};

} // namespace tbb 

#endif /* __TBB_blocked_range2d_H */
