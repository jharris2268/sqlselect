#include "columnlist.hpp"
#include <sstream>

namespace sqlselect {

        
Feature::Ptr ColumnList::Call(Feature::Ptr feat, int64 zoom) const {
    KeyValueMap props;
    for (const auto& col : cols) {
        props[col->Key()] = col->Call(feat, zoom);
    }
    return make_feature_modified(feat, props, star);
}
std::string ColumnList::Str() const {
    
    return "ColumnList({" + all_str(cols) +"}, "+(star ? "True" : "False")+")";
}

}
