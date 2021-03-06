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

#ifndef COLUMN_ARRAYLENGTH_HPP
#define COLUMN_ARRAYLENGTH_HPP

#include "sqlselect/column.hpp"

namespace sqlselect {


class ArrayLength : public Column {
    public:
        typedef std::shared_ptr<ArrayLength> Ptr;
        ArrayLength(Column::Ptr col_) : col(col_) {}
        
        virtual ~ArrayLength() {}
        static Ptr Create(Column::Ptr col) {
            return std::make_shared<ArrayLength>(col);
        }
        
        
        virtual Value Call(Feature::Ptr feat, int64 zoom) const {
            auto r = col->Call(feat,zoom);
            
            try {
                auto sl = split_string(r.to_string(), 0);
                return sl.size();
            } catch (...) {}
            return null_value();
                    
        }
        virtual std::string Str() const {
            return "ArrayLength("+col->Str()+")";
        }
        
        Column::Ptr GetColumn() const { return col; }
    private:
        
        Column::Ptr col;

};

}

#endif
