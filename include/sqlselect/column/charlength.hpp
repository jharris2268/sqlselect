#ifndef COLUMN_CHARLENGTH_HPP
#define COLUMN_CHARLENGTH_HPP

#include "column.hpp"

namespace sqlselect {
    
class CharLength : public Column {
    public:
        typedef std::shared_ptr<CharLength> Ptr;
        CharLength(Column::Ptr col_) : col(col_) {}
        
        virtual ~CharLength() {}
        static Ptr Create(Column::Ptr col) { return std::make_shared<CharLength>(col); }
        
        virtual Value Call(Feature::Ptr feat, int64 zoom) const {
            auto v = col->Call(feat,zoom);
            auto s = v.to_unicode();
            return s.length();
        }
            
        virtual std::string Str() const {
            return "CharLength("+col->Str()+")";
        }
        
        Column::Ptr GetColumn() const { return col; }
        
    private:
        Column::Ptr col;
        
};



}
#endif
