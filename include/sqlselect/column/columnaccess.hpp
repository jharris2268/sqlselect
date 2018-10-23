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

#ifndef COLUMN_COLUMNACCESS_HPP
#define COLUMN_COLUMNACCESS_HPP

#include "sqlselect/column.hpp"
#include "sqlselect/packed_column.hpp"

namespace sqlselect {
    
class ColumnAccess : public Column {
    public:
        typedef std::shared_ptr<ColumnAccess> Ptr;
        ColumnAccess(Column::Ptr src_, std::string key_) : src(src_), key(key_) {}
        
        virtual ~ColumnAccess() {}
        static Ptr Create(Column::Ptr src, std::string key) {
            return std::make_shared<ColumnAccess>(src,key);
        }
        
        
        virtual Value Call(Feature::Ptr feat, int64 zoom) const {
            auto l = src->Call(feat,zoom);
            if (l.is_null()) { return null_value(); }
            if (!l.to_bool()) {return feat->Get(key,false); }
            auto r = get_key(feat->OtherTags(), key);
            if (!r.empty()) {
                return mapnik::value_unicode_string::fromUTF8(r);
            }
        
            return null_value();
        }
            
        virtual std::string Str() const {
            return "ColumnAccess("+src->Str()+", '"+key+"')";
        }
        virtual const std::string& Key() const { return key; };
        Column::Ptr GetSrc() const { return src; }
        std::string GetKey() const { return key; }
        
    private:
        Column::Ptr src;
        std::string key;
};



}
#endif
