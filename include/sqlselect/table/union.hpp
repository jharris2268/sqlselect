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
 
#ifndef TABLE_UNION_HPP
#define TABLE_UNION_HPP

#include "sqlselect/table.hpp"


namespace sqlselect {
    
class Union : public Table {
    
    
    public:
        Union(
            Table::Ptr left_,
            Table::Ptr right_,
            const std::string& name_) :
            
            left(left_), right(right_), name(name_) {}
        
        
        virtual ~Union() {}
        typedef std::shared_ptr<Union> Ptr;
        static Ptr Create(
            Table::Ptr left, Table::Ptr right,
            const std::string& name) {
                return std::make_shared<Union>(left,right,name); }
        
        
        Result::Ptr Call(Data::Ptr data, int64 zoom) const {
            auto l = left->Call(data,zoom);
            auto r = right->Call(data,zoom);
            if (!l) { return r; }
            if (!r) { return l; }
            return UnionResult::Create(l,r);
        }
        
        std::string Str() const {
            return "Union(" + left->Str()+", "+right->Str()+")";
        }
            
        Table::Ptr GetLeft() const { return left; }
        Table::Ptr GetRight() const { return right; }
        const std::string& GetName() const { return name; }
        void SetName(const std::string& n) { name = n; }
            
    private:
        Table::Ptr left;
        Table::Ptr right;
        std::string name;
};
    
}
#endif
