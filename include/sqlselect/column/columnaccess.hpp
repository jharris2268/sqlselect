#ifndef COLUMN_COLUMNACCESS_HPP
#define COLUMN_COLUMNACCESS_HPP

#include "column.hpp"
#include "packed_column.hpp"

namespace sqlselect {
    
class ColumnAccess : public Column {
    public:
        typedef std::shared_ptr<ColumnAccess> Ptr;
        ColumnAccess(Column::Ptr src_, std::string key_) : src(src_), key(key_) {}
        
        virtual ~ColumnAccess() {}
        static Ptr Create(Column::Ptr src, std::string key) {
            return std::make_shared<ColumnAccess>(src,key);
        }
        
        
        virtual Value Call(Feature::Ptr feat, int64 zoom) const {
            auto l = src->Call(feat,zoom);
            if (l.is_null()) { return null_value(); }
            if (!l.to_bool()) {return feat->Get(key,false); }
            auto r = get_key(feat->OtherTags(), key);
            if (!r.empty()) {
                return mapnik::value_unicode_string::fromUTF8(r);
            }
        
            return null_value();
        }
            
        virtual std::string Str() const {
            return "ColumnAccess("+src->Str()+", '"+key+"')";
        }
        virtual const std::string& Key() const { return key; };
        Column::Ptr GetSrc() const { return src; }
        std::string GetKey() const { return key; }
        
    private:
        Column::Ptr src;
        std::string key;
};



}
#endif
