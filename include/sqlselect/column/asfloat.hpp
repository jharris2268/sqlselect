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

#ifndef COLUMN_ASFLOAT_HPP
#define COLUMN_ASFLOAT_HPP

#include "sqlselect/column.hpp"
#include "sqlselect/feature.hpp"

namespace sqlselect {
    
class AsFloat : public Column {
    public:
        typedef std::shared_ptr<AsFloat> Ptr;
        AsFloat(Column::Ptr col_) : col(col_) {}
        
        virtual ~AsFloat() {}
        static Ptr Create(Column::Ptr col) { return std::make_shared<AsFloat>(col); }
        
        virtual Value Call(Feature::Ptr feat, int64 zoom) const {
            auto v = col->Call(feat,zoom);
            try {
                return v.to_double();
            } catch ( ... ) {}
            return null_value();
        }
            
        virtual std::string Str() const {
            return "AsFloat("+col->Str()+")";
        }
        
        Column::Ptr GetColumn() const { return col; }
        
    private:
        Column::Ptr col;
        
};



}
#endif
