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

#ifndef COLUMN_CHARLENGTH_HPP
#define COLUMN_CHARLENGTH_HPP

#include "sqlselect/column.hpp"

namespace sqlselect {
    
class CharLength : public Column {
    public:
        typedef std::shared_ptr<CharLength> Ptr;
        CharLength(Column::Ptr col_) : col(col_) {}
        
        virtual ~CharLength() {}
        static Ptr Create(Column::Ptr col) { return std::make_shared<CharLength>(col); }
        
        virtual Value Call(Feature::Ptr feat, int64 zoom) const {
            auto v = col->Call(feat,zoom);
            auto s = v.to_unicode();
            return s.length();
        }
            
        virtual std::string Str() const {
            return "CharLength("+col->Str()+")";
        }
        
        Column::Ptr GetColumn() const { return col; }
        
    private:
        Column::Ptr col;
        
};



}
#endif
