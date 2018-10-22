#ifndef COLUMN_ARRAYLENGTH_HPP
#define COLUMN_ARRAYLENGTH_HPP

#include "column.hpp"

namespace sqlselect {


class ArrayLength : public Column {
    public:
        typedef std::shared_ptr<ArrayLength> Ptr;
        ArrayLength(Column::Ptr col_) : col(col_) {}
        
        virtual ~ArrayLength() {}
        static Ptr Create(Column::Ptr col) {
            return std::make_shared<ArrayLength>(col);
        }
        
        
        virtual Value Call(Feature::Ptr feat, int64 zoom) const {
            auto r = col->Call(feat,zoom);
            
            try {
                auto sl = split_string(r.to_string(), 0);
                return sl.size();
            } catch (...) {}
            return null_value();
                    
        }
        virtual std::string Str() const {
            return "ArrayLength("+col->Str()+")";
        }
        
        Column::Ptr GetColumn() const { return col; }
    private:
        
        Column::Ptr col;

};

}

#endif
