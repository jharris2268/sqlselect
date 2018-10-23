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

#ifndef TABLE_SELECT_HPP
#define TABLE_SELECT_HPP

#include "sqlselect/table.hpp"
#include "sqlselect/columnlist.hpp"
#include "sqlselect/where.hpp"
#include "sqlselect/orderby.hpp"

namespace sqlselect {
    
class Select : public Table {
    
    
    public:
        Select(
            ColumnList::Ptr columns_,
            Table::Ptr table_,
            Where::Ptr where_,            
            OrderBy::Ptr orderby_,
            const std::string& name_) :
            
            columns(columns_), table(table_), where(where_), orderby(orderby_), name(name_) {}
        
        
        virtual ~Select() {}
        typedef std::shared_ptr<Select> Ptr;
        static Ptr Create(
            ColumnList::Ptr columns, Table::Ptr table,
            Where::Ptr where,     OrderBy::Ptr orderby,
            const std::string& name) {
                return std::make_shared<Select>(columns,table,where,orderby,name); }
        
        
        Result::Ptr Call(Data::Ptr data, int64 zoom) const;
        std::string Str() const;
            
        ColumnList::Ptr GetColumns() const { return columns; }
        Table::Ptr GetTable() const { return table; }
        Where::Ptr GetWhere() const { return where; }
        OrderBy::Ptr GetOrderBy() const { return orderby; }
        const std::string& GetName() const { return name; }
        void SetName(const std::string& n) { name = n; }
            
    private:
        ColumnList::Ptr columns;
        Table::Ptr table;
        Where::Ptr where;
        OrderBy::Ptr orderby;
        std::string name;
};
    
}
#endif
