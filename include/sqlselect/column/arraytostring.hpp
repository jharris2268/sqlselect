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

#ifndef COLUMN_ARRAYTOSTRING_HPP
#define COLUMN_ARRAYTOSTRING_HPP

#include "sqlselect/column.hpp"

namespace sqlselect {


class ArrayToString : public Column {
    public:
        typedef std::shared_ptr<ArrayToString> Ptr;
        ArrayToString(Column::Ptr col_, const std::string& join_) : col(col_), join(join_) {}
        
        virtual ~ArrayToString() {}
        static Ptr Create(Column::Ptr col, const std::string& join) {
            return std::make_shared<ArrayToString>(col,join);
        }
        
        
        virtual Value Call(Feature::Ptr feat, int64 zoom) const {
            auto r = col->Call(feat,zoom);
            
            try {
                auto sl = split_string(r.to_string(), 0);
            
                std::stringstream res;
                bool f=true;
                for (const auto& s: sl) {
                    if (!f) { res << join; }
                    res << s;
                }
                return mapnik::value_unicode_string::fromUTF8(res.str());
            } catch(...) {}
            
            return null_value();
                    
        }
        virtual std::string Str() const {
            return "ArrayToString("+col->Str()+", "+quote_str(join)+")";
        }
        
        Column::Ptr GetColumn() const { return col; }
        std::string GetJoin() const { return join; }
    private:
        
        Column::Ptr col;
        std::string join;
};

}

#endif
