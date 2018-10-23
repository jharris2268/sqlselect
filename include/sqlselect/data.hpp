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

#ifndef DATA_HPP
#define DATA_HPP

#include "sqlselect/result.hpp"

namespace sqlselect {


class Data {
    public:
        Data(Data_map&& contents_) : contents(contents_) {}
        
        typedef std::shared_ptr<Data> Ptr;
        static Ptr Create(Data_map&& feats) { return std::make_shared<Data>(std::move(feats)); }
        
        virtual Result::Ptr Call(const std::string& table, int64 zoom) const {
            auto it = contents.find(table);
            if (it==contents.end()) { return EmptyResult::Create(); }
            return RefResult::Create(it->second, zoom);
        }

        void AddResult(const std::string& key, Result::Ptr result) {
            contents[key] = read_result(result);
        }

        const Data_map& Contents() const { return contents; }
    private:
        Data_map contents;
        
};



}

#endif
