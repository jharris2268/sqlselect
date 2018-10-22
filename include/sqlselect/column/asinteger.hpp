#ifndef COLUMN_ASINTEGER_HPP
#define COLUMN_ASINTEGER_HPP

#include "column.hpp"
#include "feature.hpp"

namespace sqlselect {
    
class AsInteger : public Column {
    public:
        typedef std::shared_ptr<AsInteger> Ptr;
        AsInteger(Column::Ptr col_) : col(col_) {}
        
        virtual ~AsInteger() {}
        static Ptr Create(Column::Ptr col) { return std::make_shared<AsInteger>(col); }
        
        virtual Value Call(Feature::Ptr feat, int64 zoom) const {
            auto v = col->Call(feat,zoom);
            try {
                return v.to_int();
            } catch ( ... ) {}
            return null_value();
        }
            
        virtual std::string Str() const {
            return "AsInteger("+col->Str()+")";
        }
        
        Column::Ptr GetColumn() const { return col; }
        
    private:
        Column::Ptr col;
        
};



}
#endif
