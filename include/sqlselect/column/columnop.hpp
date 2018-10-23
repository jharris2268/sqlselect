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

#ifndef COLUMN_COLUMNOP_HPP
#define COLUMN_COLUMNOP_HPP

#include "sqlselect/column.hpp"

namespace sqlselect {
    
class ColumnOp : public Column {
    public:
        typedef std::shared_ptr<ColumnOp> Ptr;
        ColumnOp(Column::Ptr left_, arithmitic_type op_, Column::Ptr right_) : left(left_), op(op_), right(right_) {}
        
        virtual ~ColumnOp() {}
        static Ptr Create(Column::Ptr left, arithmitic_type op, Column::Ptr right) {
            return std::make_shared<ColumnOp>(left,op,right);
        }
        
        
        virtual Value Call(Feature::Ptr feat, int64 zoom) const {
            auto l = left->Call(feat,zoom);
            auto r = right->Call(feat,zoom);
            return arithmitic(l,op,r);
        }
            
        virtual std::string Str() const {
            return "ColumnOp("+left->Str()+", "+arithmitic_type_str(op)+", "+right->Str()+")";
        }
        
        Column::Ptr GetLeft() const { return left; }
        arithmitic_type GetOp() const { return op; }
        Column::Ptr GetRight() const { return right; }
        
    private:
        Column::Ptr left;
        arithmitic_type op;
        Column::Ptr right;
};



}
#endif
