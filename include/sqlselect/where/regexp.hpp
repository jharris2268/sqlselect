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

#ifndef WHERE_REGEXP_HPP
#define WHERE_REGEXP_HPP

#include "sqlselect/where.hpp"
#include "sqlselect/column.hpp"
#include <regex>
namespace sqlselect {

    
class RegEx : public Where {
    public:
        typedef std::shared_ptr<RegEx> Ptr;
        RegEx(Column::Ptr col_, const std::string& regex_str_) : col(col_), regex_str(regex_str_) { regex = std::regex(regex_str); }
        
        virtual ~RegEx() {}
        static Ptr Create(Column::Ptr col, const std::string& regex_str) { return std::make_shared<RegEx>(col,regex_str); }
        
        virtual bool Call(Feature::Ptr feat, int64 zoom) const {
            auto s = col->Call(feat,zoom).to_string();
            return std::regex_match(s, regex);
            
        }
            
        virtual std::string Str() const {
            return "RegEx("+col->Str()+", "+quote_str(regex_str)+")";
        }
        
        Column::Ptr GetColumn() const { return col; }
        std::string GetMatch() const { return regex_str; }
        
        
    private:
        Column::Ptr col;
        std::string regex_str;
        std::regex regex;
        
};



}
#endif

