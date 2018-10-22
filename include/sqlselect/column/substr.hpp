#ifndef COLUMN_SUBSTR_HPP
#define COLUMN_SUBSTR_HPP

#include "column.hpp"

namespace sqlselect {
    
class SubStr : public Column {
    public:
        typedef std::shared_ptr<SubStr> Ptr;
        SubStr(Column::Ptr col_, Column::Ptr from_, Column::Ptr to_) : col(col_),from(from_),to(to_) {}
        
        virtual ~SubStr() {}
        static Ptr Create(Column::Ptr col, Column::Ptr from, Column::Ptr to) { return std::make_shared<SubStr>(col,from,to); }
        
        virtual Value Call(Feature::Ptr feat, int64 zoom) const {
            auto s = col->Call(feat,zoom).to_unicode();
            auto f = from->Call(feat,zoom).to_int();
            auto t = to->Call(feat,zoom).to_int();
            
            return mapnik::value_unicode_string(s, (f==0 ? 0 : f-1),(f==0 ? t-1 : t));
            
        }
            
        virtual std::string Str() const {
            return "SubStr("+col->Str()+", "+from->Str()+", "+to->Str()+")";
        }
        
        Column::Ptr GetColumn() const { return col; }
        Column::Ptr GetFrom() const { return from; }
        Column::Ptr GetTo() const { return to; }
        
    private:
        Column::Ptr col;
        Column::Ptr from;
        Column::Ptr to;
        
};



}
#endif
