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

#ifndef COLUMN_CASE_HPP
#define COLUMN_CASE_HPP

#include "sqlselect/column.hpp"
#include "sqlselect/where.hpp"
namespace sqlselect {


struct Clause {
    Where::Ptr where;
    Column::Ptr column;
    Clause(Where::Ptr where_, Column::Ptr column_) : where(where_), column(column_) {}
    
    std::string Str() const {
        std::string res = "Clause(";
        if (where) { res += where->Str(); }
        else { res += "None"; }
        res += ", "+column->Str()+")";
        return res;
    }
};

typedef std::vector<Clause> Clause_Vec;



class Case : public Column {
    public:
        Case(const Clause_Vec& clauses_) : clauses(clauses_) {}
        
        typedef std::shared_ptr<Case> Ptr;
        static Ptr Create(const Clause_Vec& clauses) {
            return std::make_shared<Case>(clauses);
        }
        
        virtual Value Call(Feature::Ptr feat, int64 zoom) const {
            for (const auto& clause : clauses) {
                if (!clause.where || (clause.where->Call(feat,zoom))) {
                    return clause.column->Call(feat,zoom);
                }
            }
            return null_value();
        }
        
        virtual std::string Str() const {
            return "Case("+all_str_obj(clauses)+")";
        }
        
        const Clause_Vec& GetClauses() const { return clauses; }
    private:
        Clause_Vec clauses;
};



}
#endif
