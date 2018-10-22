#ifndef TABLE_JOIN_HPP
#define TABLE_JOIN_HPP

#include "table.hpp"


namespace sqlselect {
    
class Join : public Table {
    
    
    public:
        Join(
            Table::Ptr left_,
            Table::Ptr right_,
            const std::vector<Column::Ptr>& cols_,
            const std::string& name_) :
            
            left(left_), right(right_), cols(cols_), name(name_) {}
        
        
        virtual ~Join() {}
        typedef std::shared_ptr<Join> Ptr;
        static Ptr Create(
            Table::Ptr left, Table::Ptr right,
            const std::vector<Column::Ptr>& cols,
            const std::string& name) {
                return std::make_shared<Join>(left,right,cols,name); }
        
        
        Result::Ptr Call(Data::Ptr data, int64 zoom) const {
            Feature_vec res;
            
            auto l = left->Call(data,zoom);
            auto r = right->Call(data,zoom);
            
            if (!l) { return nullptr; }
            if (!r) { return nullptr; }
            
            
            auto lf = l->next();
            while (lf) {
                Feature::Ptr jf;
                r->reset();
                auto rf = r->next();
                while (rf && !jf) {
                    if (matches(lf, rf, zoom)) {
                        
                        jf = make_feature_modified(lf, other_props(lf,rf), true);
                    } else {
                        rf = r->next();
                    }
                }
                if (jf) {
                    res.push_back(jf);
                }
                
                lf = l->next();
            }
            
            return MoveResult::Create(std::move(res));
        }
        
        std::string Str() const {
            return "Join(" + left->Str()+", "+right->Str()+", ["+all_str(cols)+"])";
        }
            
        Table::Ptr GetLeft() const { return left; }
        Table::Ptr GetRight() const { return right; }
        const std::vector<Column::Ptr>& GetColumns() { return cols; }
        const std::string& GetName() const { return name; }
        void SetName(const std::string& n) { name = n; }
            
    private:
        Table::Ptr left;
        Table::Ptr right;
        std::vector<Column::Ptr> cols;
        std::string name;
        
        bool matches(Feature::Ptr l, Feature::Ptr r, int64 zoom) const {
            for (auto c: cols) {
                if (c->Call(l, zoom) != c->Call(r, zoom)) {
                    return false;
                }
            }
            return true;
        }
        KeyValueMap other_props(Feature::Ptr l, Feature::Ptr r) const {
            KeyValueMap res;
            for (auto kk : r->Keys()) {
                auto ov = l->Get(kk,false);
                if (ov.is_null()) {
                    res[kk]=r->Get(kk,false);
                }
            }
            return res;
        }
        
};
    
}
#endif
