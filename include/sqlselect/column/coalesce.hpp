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

#ifndef COLUMN_COALESCE_HPP
#define COLUMN_COALESCE_HPP

#include "sqlselect/column.hpp"

namespace sqlselect {
    

class Coalesce : public Column {
    public:
        Coalesce(const std::vector<Column::Ptr>& cols_) : cols(cols_) {}
        virtual ~Coalesce() {}
        
        typedef std::shared_ptr<Coalesce> Ptr;
        static Ptr Create(const std::vector<Column::Ptr>& cols) {
            return std::make_shared<Coalesce>(cols);
        }
        
        virtual Value Call(Feature::Ptr feat, int64 zoom) const {
            for (const auto& cc : cols) {
                auto v = cc->Call(feat,zoom);
                if (!v.is_null()) {
                    return v;
                }
            }
            return null_value();
        }
        
        virtual std::string Str() const {
            return "Coalesce({"+all_str(cols)+"})";
        }
        
        const std::vector<Column::Ptr>& GetColumns() const { return cols; }
    private:
        std::vector<Column::Ptr> cols;
};


}
#endif
