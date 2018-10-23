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

#ifndef TABLE_PICKTABLE_HPP
#define TABLE_PICKTABLE_HPP

#include "sqlselect/table.hpp"

namespace sqlselect {
    
class PickTable : public Table {
    public:
        PickTable(const std::string& key_) : key(key_) {}
        virtual ~PickTable() {}
        
        typedef std::shared_ptr<PickTable> Ptr;
        static Ptr Create(const std::string& key) { return std::make_shared<PickTable>(key); }
        
        Result::Ptr Call(Data::Ptr data, int64 minzoom) const {
            return data->Call(key,minzoom);
        }
        
        std::string Str() const {
            return "PickTable(\"" + key + "\")";
        }
        
        std::string GetTableKey() { return key; }
        
    private:
        std::string key;
};
    
}
#endif
