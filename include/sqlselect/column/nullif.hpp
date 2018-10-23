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

#ifndef COLUMN_NULLIF_HPP
#define COLUMN_NULLIF_HPP

#include "sqlselect/column.hpp"
#include "sqlselect/where.hpp"
namespace sqlselect {



class NullIf : public Column {
    public:
        NullIf(Column::Ptr left_,Column::Ptr right_) : left(left_), right(right_) {}
        
        typedef std::shared_ptr<NullIf> Ptr;
        static Ptr Create(Column::Ptr left,Column::Ptr right) {
            return std::make_shared<NullIf>(left,right);
        }
        
        virtual Value Call(Feature::Ptr feat, int64 zoom) const {
            auto l = left->Call(feat,zoom);
            auto r = right->Call(feat,zoom);
            if (l==r) { return null_value(); }
            return l;
        }
        
        virtual std::string Str() const {
            return "NullIf("+left->Str()+", "+right->Str()+")";
        }
        
        Column::Ptr GetLeft() const { return left; }
        Column::Ptr GetRight() const { return right; }
    private:
        Column::Ptr left;
        Column::Ptr right;
};



}
#endif
