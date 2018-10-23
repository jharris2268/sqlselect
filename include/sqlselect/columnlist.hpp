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

#ifndef COLUMNLIST_HPP
#define COLUMNLIST_HPP

#include "sqlselect/feature.hpp"
#include "sqlselect/column.hpp"
#include <vector>

namespace sqlselect {

class ColumnList {
    public:
        ColumnList(const std::vector<Column::Ptr>& cols_, bool star_) : cols(cols_), star(star_) {}
        ~ColumnList() {}
        
        typedef std::shared_ptr<ColumnList> Ptr;
        static Ptr Create(const std::vector<Column::Ptr>& cols, bool star) { return std::make_shared<ColumnList>(cols, star); }
        
        
        virtual Feature::Ptr Call(Feature::Ptr feat, int64 zoom) const {
            KeyValueMap props;
            for (const auto& col : cols) {
                props[col->Key()] = col->Call(feat, zoom);
            }
            return make_feature_modified(feat, props, star);
        }
        virtual std::string Str() const {
            return "ColumnList({" + all_str(cols) +"}, "+(star ? "True" : "False")+")";
        };
        
        virtual bool HasStar() const { return star; }
        const std::vector<Column::Ptr>& Cols() const { return cols; }
    private:
        
        std::vector<Column::Ptr> cols;
        bool star;
    
};        

}

#endif
