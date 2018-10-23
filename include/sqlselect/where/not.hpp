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

#ifndef WHERE_NOT_HPP
#define WHERE_NOT_HPP

#include "sqlselect/where.hpp"

namespace sqlselect {


class Not : public Where {
    public:
        Not(Where::Ptr where_) : where(where_) {}
        
        virtual ~Not() {}
        typedef std::shared_ptr<Not> Ptr;
        static Ptr Create(Where::Ptr val) { return std::make_shared<Not>(val); }
        
        bool Call(Feature::Ptr feat, int64 zoom) const;        
        std::string Str() const;
        Where::Ptr GetWhere() const { return where; }
        
    private:
        Where::Ptr where;
};
    
}
#endif
