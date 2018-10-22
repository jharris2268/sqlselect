#ifndef COLUMN_STRINGTOARRAY_HPP
#define COLUMN_STRINGTOARRAY_HPP

#include "column.hpp"

namespace sqlselect {


class StringToArray : public Column {
    public:
        typedef std::shared_ptr<StringToArray> Ptr;
        StringToArray(Column::Ptr col_, const std::string& split_) : col(col_), split(split_) {}
        
        virtual ~StringToArray() {}
        static Ptr Create(Column::Ptr col, const std::string& split) {
            return std::make_shared<StringToArray>(col,split);
        }
        
        
        virtual Value Call(Feature::Ptr feat, int64 zoom) const {
            auto r = col->Call(feat,zoom);
            
            return r.to_unicode();
                    
        }
        virtual std::string Str() const {
            return "StringToArray("+col->Str()+", "+quote_str(split)+")";
        }
        
        Column::Ptr GetColumn() const { return col; }
        std::string GetSplit() const { return split; }
    private:
        
        Column::Ptr col;
        std::string split;
};

}

#endif
