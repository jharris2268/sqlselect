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

#ifndef COLUMN_LABEL_HPP
#define COLUMN_LABEL_HPP

#include "sqlselect/column.hpp"

namespace sqlselect {
    
class Label : public Column {
    public:
        typedef std::shared_ptr<Label> Ptr;
        Label(const std::string& key_) : key(key_) {}
        
        virtual ~Label() {}
        static Ptr Create(const std::string& key) {
            return std::make_shared<Label>(key);
        }
        
        virtual const std::string& Key() const { return key; }
        virtual Value Call(Feature::Ptr feat, int64 zoom) const { return feat->Get(key,false); }
        virtual std::string Str() const { return std::string("Label(\"")+key+"\")"; }
    private:
        std::string key;
};



}
#endif
