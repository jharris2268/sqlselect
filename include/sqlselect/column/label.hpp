#ifndef COLUMN_LABEL_HPP
#define COLUMN_LABEL_HPP

#include "column.hpp"

namespace sqlselect {
    
class Label : public Column {
    public:
        typedef std::shared_ptr<Label> Ptr;
        Label(const std::string& key_) : key(key_) {}
        
        virtual ~Label() {}
        static Ptr Create(const std::string& key) {
            return std::make_shared<Label>(key);
        }
        
        virtual const std::string& Key() const { return key; }
        virtual Value Call(Feature::Ptr feat, int64 zoom) const { return feat->Get(key,false); }
        virtual std::string Str() const;
    private:
        std::string key;
};



}
#endif
