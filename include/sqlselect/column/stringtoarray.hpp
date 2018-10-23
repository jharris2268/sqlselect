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

#ifndef COLUMN_STRINGTOARRAY_HPP
#define COLUMN_STRINGTOARRAY_HPP

#include "sqlselect/column.hpp"

namespace sqlselect {


class StringToArray : public Column {
    public:
        typedef std::shared_ptr<StringToArray> Ptr;
        StringToArray(Column::Ptr col_, const std::string& split_) : col(col_), split(split_) {}
        
        virtual ~StringToArray() {}
        static Ptr Create(Column::Ptr col, const std::string& split) {
            return std::make_shared<StringToArray>(col,split);
        }
        
        
        virtual Value Call(Feature::Ptr feat, int64 zoom) const {
            auto r = col->Call(feat,zoom);
            
            return r.to_unicode();
                    
        }
        virtual std::string Str() const {
            return "StringToArray("+col->Str()+", "+quote_str(split)+")";
        }
        
        Column::Ptr GetColumn() const { return col; }
        std::string GetSplit() const { return split; }
    private:
        
        Column::Ptr col;
        std::string split;
};

}

#endif
