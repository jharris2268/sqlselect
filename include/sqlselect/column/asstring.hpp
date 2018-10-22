#ifndef COLUMN_ASSTRING_HPP
#define COLUMN_ASSTRING_HPP

#include "column.hpp"
#include "feature.hpp"

namespace sqlselect {
    
class AsString : public Column {
    public:
        typedef std::shared_ptr<AsString> Ptr;
        AsString(Column::Ptr col_) : col(col_) {}
        
        virtual ~AsString() {}
        static Ptr Create(Column::Ptr col) { return std::make_shared<AsString>(col); }
        
        virtual Value Call(Feature::Ptr feat, int64 zoom) const {
            auto v = col->Call(feat,zoom);
            
            if (v.is_null()) { return v; }
            return v.to_unicode();
            
            /*if (is_string(v)) { return get_string(v); }
            if (test_null(v)) { return null_value(); }
            
            return to_string(v);*/
            
        }
            
        virtual std::string Str() const {
            return "AsString("+col->Str()+")";
        }
        
        Column::Ptr GetColumn() const { return col; }
        
    private:
        Column::Ptr col;
        
};



}
#endif
