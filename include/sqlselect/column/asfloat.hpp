#ifndef COLUMN_ASFLOAT_HPP
#define COLUMN_ASFLOAT_HPP

#include "column.hpp"
#include "feature.hpp"

namespace sqlselect {
    
class AsFloat : public Column {
    public:
        typedef std::shared_ptr<AsFloat> Ptr;
        AsFloat(Column::Ptr col_) : col(col_) {}
        
        virtual ~AsFloat() {}
        static Ptr Create(Column::Ptr col) { return std::make_shared<AsFloat>(col); }
        
        virtual Value Call(Feature::Ptr feat, int64 zoom) const {
            auto v = col->Call(feat,zoom);
            try {
                return v.to_double();
            } catch ( ... ) {}
            return null_value();
        }
            
        virtual std::string Str() const {
            return "AsFloat("+col->Str()+")";
        }
        
        Column::Ptr GetColumn() const { return col; }
        
    private:
        Column::Ptr col;
        
};



}
#endif
