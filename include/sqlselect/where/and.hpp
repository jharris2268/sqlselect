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

#ifndef WHERE_AND_HPP
#define WHERE_AND_HPP

#include "sqlselect/where.hpp"

namespace sqlselect {
    
class And : public Where {
    public:
        And(Where::Ptr left_, Where::Ptr right_) : left(left_), right(right_) {}
        
        virtual ~And() {}
        typedef std::shared_ptr<And> Ptr;
        static Ptr Create(Where::Ptr left, Where::Ptr right) { return std::make_shared<And>(left,right); }
        
        bool Call(Feature::Ptr feat, int64 zoom) const;
        
        std::string Str() const;
        
        Where::Ptr GetLeft() const { return left; }
        Where::Ptr GetRight() const { return right; }
        
    private:
        Where::Ptr left, right;
};

}
#endif
