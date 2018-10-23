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

#ifndef COLUMN_CONCAT_HPP
#define COLUMN_CONCAT_HPP

#include "sqlselect/column.hpp"

namespace sqlselect {
    
class Concat : public Column {
    public:
        Concat(const std::vector<Column::Ptr>& cols_, bool allownulls_, const std::string& sep_) : cols(cols_),allownulls(allownulls_), sep(sep_) {}
        virtual ~Concat() {}
        
        typedef std::shared_ptr<Concat> Ptr;
        static Ptr Create(const std::vector<Column::Ptr>& cols, bool allownulls, const std::string& sep) {
            return std::make_shared<Concat>(cols, allownulls, sep);
        }
        
        virtual Value Call(Feature::Ptr feat, int64 zoom) const {
            
            std::string r="";
            
            for (const auto& cc : cols) {
                auto v = cc->Call(feat,zoom);
                
            
                if (v.is_null()) {
                    if (!allownulls) {
                        return null_value();
                    }
                } else {
                    if (!r.empty()) {
                        r += sep;
                    }
                    r += v.to_string();
                }
            }
            
            return mapnik::value_unicode_string::fromUTF8(r);
        }
        
        virtual std::string Str() const {
            return "Concat({"+all_str(cols)+"}, "+(allownulls ? "True" : "False")+", '"+sep+"')";
        }
        
        const std::vector<Column::Ptr>& GetColumns() const { return cols; }
        bool AllowNulls() const { return allownulls; }
        const std::string& GetSeperator() const { return sep; }
    private:
        std::vector<Column::Ptr> cols;
        bool allownulls;
        std::string sep;
};
                
            



}
#endif
