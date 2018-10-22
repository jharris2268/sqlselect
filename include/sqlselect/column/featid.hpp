#ifndef COLUMN_FEATID_HPP
#define COLUMN_FEATID_HPP

#include "column.hpp"

namespace sqlselect {
    
class FeatId : public Column {
    public:
        typedef std::shared_ptr<FeatId> Ptr;
        FeatId(const std::string& key_) : key(key_) {}
        
        virtual ~FeatId() {}
        static Ptr Create(const std::string& key) {
            return std::make_shared<FeatId>(key);
        }
        
        virtual const std::string& Key() const { return key; }
        virtual Value Call(Feature::Ptr feat, int64 zoom) const { return feat->Id(); }
        virtual std::string Str() const { return "FeatId("+quote_str(key)+")"; }
    private:
        std::string key;
};



}
#endif
