#ifndef TABLE_VALUESTABLE_HPP
#define TABLE_VALUESTABLE_HPP

#include "table.hpp"


namespace sqlselect {
    
class ValuesTable : public Table {
    
    
    public:
        ValuesTable(
            const Feature_vec& feats_,
            const std::string& name_) :
            
            feats(feats_), name(name_) {}
        
        
        virtual ~ValuesTable() {}
        typedef std::shared_ptr<ValuesTable> Ptr;
        static Ptr Create(
            const Feature_vec& feats,
            const std::string& name) {
                return std::make_shared<ValuesTable>(feats,name); }
        
        
        Result::Ptr Call(Data::Ptr data, int64 zoom) const {
            return RefResult::Create(feats, -1);
        }
        
        std::string Str() const {
            return "ValuesTable()";
        }
            
        const Feature_vec& GetFeatures() const { return feats; }
        const std::string& GetName() const { return name; }
        void SetName(const std::string& n) { name = n; }
            
    private:
        Feature_vec feats;
        std::string name;
};
    
}
#endif
