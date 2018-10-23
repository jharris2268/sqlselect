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
 
#ifndef PACKED_COLUMN_HPP
#define PACKED_COLUMN_HPP

#include <iostream>

inline size_t read_int(const std::string& src, size_t pos) {
    size_t res=0;
    
    res += ((unsigned char) src[pos])*256;
    res += ((unsigned char) src[pos+1]);
    
    return res;
}
    

inline std::string get_key(const std::string& src, const std::string& key) {
    size_t pos=1;
    
    while (pos < src.size()) {
        size_t kl = read_int(src, pos);
        if (pos+2+kl > src.size() ) {
            std::cout << "??? " << src << " @ " << pos << " : " << kl << "/" << src.size() << std::endl;
            return std::string();
        }
        std::string k = src.substr(pos+2, kl);
        pos += (2+kl);
        size_t vl = read_int(src, pos);
        
        if (key == k) {
            if (pos+2+vl > src.size() ) {
                std::cout << "??? " << src << " (key=" << k << ") @ " << pos << " : " << vl << "/" << src.size() << std::endl;
                return std::string();
            }
            
            
            //std::cout << k << "==" << k << " => " << src.substr(pos+2, vl) << std::endl;
            return src.substr(pos+2, vl);
        }
        pos += (2+vl);
    }
    
    return std::string();
}
    

#endif
