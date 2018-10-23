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
 
#ifndef TABLE_VALUESTABLE_HPP
#define TABLE_VALUESTABLE_HPP

#include "sqlselect/table.hpp"


namespace sqlselect {
    
class ValuesTable : public Table {
    
    
    public:
        ValuesTable(
            const Feature_vec& feats_,
            const std::string& name_) :
            
            feats(feats_), name(name_) {}
        
        
        virtual ~ValuesTable() {}
        typedef std::shared_ptr<ValuesTable> Ptr;
        static Ptr Create(
            const Feature_vec& feats,
            const std::string& name) {
                return std::make_shared<ValuesTable>(feats,name); }
        
        
        Result::Ptr Call(Data::Ptr data, int64 zoom) const {
            return RefResult::Create(feats, -1);
        }
        
        std::string Str() const {
            return "ValuesTable()";
        }
            
        const Feature_vec& GetFeatures() const { return feats; }
        const std::string& GetName() const { return name; }
        void SetName(const std::string& n) { name = n; }
            
    private:
        Feature_vec feats;
        std::string name;
};
    
}
#endif
