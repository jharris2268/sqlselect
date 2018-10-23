/*****************************************************************************
 *
 * This file is part of sqlselect
 *
 * Copyright (C) 2018 James Harris
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 *****************************************************************************/

#ifndef WHERE_OP_HPP
#define WHERE_OP_HPP

#include "sqlselect/where.hpp"
#include "sqlselect/column.hpp"
namespace sqlselect {
    
enum comparison_type {
    EQ = 0,
    NE,
    LT,
    GT,
    LE,
    GE
};    
std::string comparison_type_str(comparison_type c);

class Op : public Where {
    public:
        Op(Column::Ptr left_, comparison_type op_, Column::Ptr right_) : left(left_), op(op_), right(right_) {}
        virtual ~Op() {}
        
        typedef std::shared_ptr<Op> Ptr;
        static Ptr Create(Column::Ptr left, comparison_type op, Column::Ptr right) { return std::make_shared<Op>(left,op,right); }
        virtual bool Call(Feature::Ptr feat, int64 zoom) const;
        virtual std::string Str() const;
        
        Column::Ptr GetLeft() const { return left; }
        comparison_type GetOp() const { return op; }
        Column::Ptr GetRight() const { return right; }
        
        
    private:
        Column::Ptr left;
        comparison_type op;
        Column::Ptr right;
};

}
#endif
