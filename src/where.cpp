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


#include "sqlselect/column.hpp"

#include "sqlselect/where/and.hpp"
#include "sqlselect/where/or.hpp"
#include "sqlselect/where/not.hpp"
#include "sqlselect/where/isnull.hpp"
#include "sqlselect/where/in.hpp"
#include "sqlselect/where/op.hpp"


namespace sqlselect {

bool And::Call(Feature::Ptr feat, int64 zoom) const {
    bool l = left->Call(feat,zoom);
    if (!l) { return false; }
    return right->Call(feat,zoom);
}

std::string And::Str() const {
    return "And("+left->Str()+", "+right->Str()+")";
}



bool Or::Call(Feature::Ptr feat, int64 zoom) const {
            bool l = left->Call(feat,zoom);
            if (l) { return true; }
            return right->Call(feat,zoom);
        }
        
std::string Or::Str() const {
    return "Or("+left->Str()+", "+right->Str()+")";
}


bool Not::Call(Feature::Ptr feat, int64 zoom) const {
    return !where->Call(feat,zoom);
}
        
std::string Not::Str() const {
    return "Not("+where->Str()+")";
}



bool IsNull::Call(Feature::Ptr feat, int64 zoom) const {
    auto v = val->Call(feat,zoom);
    return v.is_null();
    
}
std::string IsNull::Str() const {
    return "IsNull("+val->Str()+")";
}

std::string comparison_type_str(comparison_type c) {
    switch (c) {
        case EQ: return "==";
        case NE: return "!=";
        case LT: return "<";
        case GT: return ">";
        case LE: return "<=";
        case GE: return ">=";
        
    }
    throw std::domain_error("unexpected comparison_type");
    return "";
};
    



bool Op::Call(Feature::Ptr feat, int64 zoom) const {
    auto l = left->Call(feat,zoom);
    if (l.is_null()) { return false; }
    
    auto r = right->Call(feat,zoom);
    if (r.is_null()) { return false; }
    
    if (op==EQ) {
        return l==r;
    } else if (op == LT) {
        return l<r;
    } else if (op == GT) {
        return l>r;
    } else if (op == LE) {
        return l<=r;
    } else if (op == GE) {
        return l>=r;
    } else if (op == NE) {
        return l!=r;
    }
    throw std::domain_error("unexpected comparision");
    return false;
    
}
std::string Op::Str() const {
    return "Op("+left->Str()+", \""+comparison_type_str(op)+"\", "+right->Str()+")";
}






bool In::Call(Feature::Ptr feat, int64 zoom) const {
    auto l = left->Call(feat,zoom);
    
    for (auto cc: right) {
        auto r = cc->Call(feat,zoom);
        if (l==r) { return true; }
    }
               
    return false;
}
std::string In::Str() const {
    return "In("+left->Str()+", ["+all_str(right)+"])";
}



}
