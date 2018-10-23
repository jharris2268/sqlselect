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

#ifndef COLUMN_ASLABEL_HPP
#define COLUMN_ASLABEL_HPP

#include "sqlselect/column.hpp"

namespace sqlselect {
    

class AsLabel : public Column {
    public:
        typedef std::shared_ptr<AsLabel> Ptr;
        AsLabel(const std::string& key_, Column::Ptr parent_) : key(key_), parent(parent_) {}
        
        virtual ~AsLabel() {}
        static Ptr Create(const std::string& key, Column::Ptr parent) {
            return std::make_shared<AsLabel>(key,parent);
        }
        
        virtual const std::string& Key() const { return key; }
        virtual Value Call(Feature::Ptr feat, int64 zoom) const { return parent->Call(feat,zoom); }
        virtual std::string Str() const { return std::string("AsLabel(\"")+key+"\", " +parent->Str()+")"; };
        
        Column::Ptr Parent() const { return parent; }
    private:
        std::string key;
        Column::Ptr parent;
};    


}
#endif
