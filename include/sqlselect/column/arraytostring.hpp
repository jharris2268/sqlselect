#ifndef COLUMN_ARRAYTOSTRING_HPP
#define COLUMN_ARRAYTOSTRING_HPP

#include "column.hpp"

namespace sqlselect {


class ArrayToString : public Column {
    public:
        typedef std::shared_ptr<ArrayToString> Ptr;
        ArrayToString(Column::Ptr col_, const std::string& join_) : col(col_), join(join_) {}
        
        virtual ~ArrayToString() {}
        static Ptr Create(Column::Ptr col, const std::string& join) {
            return std::make_shared<ArrayToString>(col,join);
        }
        
        
        virtual Value Call(Feature::Ptr feat, int64 zoom) const {
            auto r = col->Call(feat,zoom);
            
            try {
                auto sl = split_string(r.to_string(), 0);
            
                std::stringstream res;
                bool f=true;
                for (const auto& s: sl) {
                    if (!f) { res << join; }
                    res << s;
                }
                return mapnik::value_unicode_string::fromUTF8(res.str());
            } catch(...) {}
            
            return null_value();
                    
        }
        virtual std::string Str() const {
            return "ArrayToString("+col->Str()+", "+quote_str(join)+")";
        }
        
        Column::Ptr GetColumn() const { return col; }
        std::string GetJoin() const { return join; }
    private:
        
        Column::Ptr col;
        std::string join;
};

}

#endif
