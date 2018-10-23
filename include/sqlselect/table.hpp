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


#ifndef TABLE_HPP
#define TABLE_HPP

#include "sqlselect/result.hpp"
#include "sqlselect/data.hpp"



namespace sqlselect {

class Table {
    public:
        typedef std::shared_ptr<Table> Ptr;
        virtual ~Table() {}
        virtual Result::Ptr Call(Data::Ptr data, int64 zoom) const =0;
        virtual std::string Str() const=0;
};


}

#endif
