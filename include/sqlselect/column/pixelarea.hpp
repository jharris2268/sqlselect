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

#ifndef COLUMN_PIXELAREA_HPP
#define COLUMN_PIXELAREA_HPP

#include "sqlselect/column.hpp"

namespace sqlselect {
    
class PixelArea : public Column {
    public:
        typedef std::shared_ptr<PixelArea> Ptr;
        PixelArea() {}
        
        virtual ~PixelArea() {}
        static Ptr Create() { return std::make_shared<PixelArea>(); }
        
        virtual Value Call(Feature::Ptr feat, int64 zoom) const {
            double earth_circum = 40075016.6856;
            double ps = earth_circum / 256 / (1<<zoom);
            return ps*ps;
        }
            
        virtual std::string Str() const {
            return "PixelArea()";
        }
        
        
};



}
#endif
