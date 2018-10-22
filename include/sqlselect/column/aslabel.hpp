#ifndef COLUMN_ASLABEL_HPP
#define COLUMN_ASLABEL_HPP

#include "column.hpp"

namespace sqlselect {
    

class AsLabel : public Column {
    public:
        typedef std::shared_ptr<AsLabel> Ptr;
        AsLabel(const std::string& key_, Column::Ptr parent_) : key(key_), parent(parent_) {}
        
        virtual ~AsLabel() {}
        static Ptr Create(const std::string& key, Column::Ptr parent) {
            return std::make_shared<AsLabel>(key,parent);
        }
        
        virtual const std::string& Key() const { return key; }
        virtual Value Call(Feature::Ptr feat, int64 zoom) const { return parent->Call(feat,zoom); }
        virtual std::string Str() const;
        
        Column::Ptr Parent() const { return parent; }
    private:
        std::string key;
        Column::Ptr parent;
};    


}
#endif
