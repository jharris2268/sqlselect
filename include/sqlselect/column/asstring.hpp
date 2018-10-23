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

#ifndef COLUMN_ASSTRING_HPP
#define COLUMN_ASSTRING_HPP

#include "sqlselect/column.hpp"
#include "sqlselect/feature.hpp"

namespace sqlselect {
    
class AsString : public Column {
    public:
        typedef std::shared_ptr<AsString> Ptr;
        AsString(Column::Ptr col_) : col(col_) {}
        
        virtual ~AsString() {}
        static Ptr Create(Column::Ptr col) { return std::make_shared<AsString>(col); }
        
        virtual Value Call(Feature::Ptr feat, int64 zoom) const {
            auto v = col->Call(feat,zoom);
            
            if (v.is_null()) { return v; }
            return v.to_unicode();
            
            /*if (is_string(v)) { return get_string(v); }
            if (test_null(v)) { return null_value(); }
            
            return to_string(v);*/
            
        }
            
        virtual std::string Str() const {
            return "AsString("+col->Str()+")";
        }
        
        Column::Ptr GetColumn() const { return col; }
        
    private:
        Column::Ptr col;
        
};



}
#endif
