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
 
#ifndef ORDERBY_HPP
#define ORDERBY_HPP

#include "sqlselect/feature.hpp"

namespace sqlselect {
    
    
struct OrderItem {
    OrderItem(Column::Ptr column_, bool asc_) : column(column_), asc(asc_) {}
    
    std::string Str() const;
    
    Column::Ptr column;
    bool asc;
};
typedef std::vector<OrderItem> OrderItemVec;
    
class OrderBy {
    public:
        typedef std::shared_ptr<OrderBy> Ptr;
        typedef std::function<bool(Feature::Ptr,Feature::Ptr)> cmp_func;
        
        OrderBy(const OrderItemVec& spec_) : spec(spec_) {}
        
        static Ptr Create(const OrderItemVec& spec) {
            return std::make_shared<OrderBy>(spec);
        }
        
        virtual cmp_func Call(int64 zoom) const;
        virtual std::string Str() const;
        
        const OrderItemVec& GetSpec() const { return spec; }
        
    private:
        OrderItemVec spec;
        
};



}

#endif
