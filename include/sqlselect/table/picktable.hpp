#ifndef TABLE_PICKTABLE_HPP
#define TABLE_PICKTABLE_HPP

#include "table.hpp"

namespace sqlselect {
    
class PickTable : public Table {
    public:
        PickTable(const std::string& key_) : key(key_) {}
        virtual ~PickTable() {}
        
        typedef std::shared_ptr<PickTable> Ptr;
        static Ptr Create(const std::string& key) { return std::make_shared<PickTable>(key); }
        
        Result::Ptr Call(Data::Ptr data, int64 minzoom) const {
            return data->Call(key,minzoom);
        }
        
        std::string Str() const {
            return "PickTable(\"" + key + "\")";
        }
        
        std::string GetTableKey() { return key; }
        
    private:
        std::string key;
};
    
}
#endif
