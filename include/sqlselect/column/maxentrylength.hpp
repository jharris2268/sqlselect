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

#ifndef COLUMN_MAXENTRYLENGTH_HPP
#define COLUMN_MAXENTRYLENGTH_HPP

#include "sqlselect/column.hpp"

namespace sqlselect {


class MaxEntryLength : public Column {
    public:
        typedef std::shared_ptr<MaxEntryLength> Ptr;
        MaxEntryLength(Column::Ptr col_) : col(col_) {}
        
        virtual ~MaxEntryLength() {}
        static Ptr Create(Column::Ptr col) {
            return std::make_shared<MaxEntryLength>(col);
        }
        
        
        virtual Value Call(Feature::Ptr feat, int64 zoom) const {
            auto r = col->Call(feat,zoom);
            
            auto sl = split_string(r.to_string(), 0);
            size_t res=0;
            for (const auto& s: sl) {
                if (s.size()>res) {
                    res=s.size();
                }
            }
            return (mapnik::value_integer) res;
                    
        }
        virtual std::string Str() const {
            return "MaxEntryLength("+col->Str()+")";
        }
        
        Column::Ptr GetColumn() const { return col; }
        
    private:
        
        Column::Ptr col;
        
};

}

#endif
