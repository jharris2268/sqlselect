#include "table/select.hpp"
#include <sstream>
#include <iostream>
namespace sqlselect {



Result::Ptr Select::Call(Data::Ptr data, int64 zoom) const {
    auto src = table->Call(data, zoom);
    if (!src) {
        //std::cout << "Select::Call no data" << std::endl;
        return EmptyResult::Create();
    }
    
    bool isSelectStar = (columns->HasStar() && columns->Cols().empty());
    
    if ((!where) && (!orderby) && isSelectStar) {
        //Eg is "select * from <table>"                
        return src;
    }
                
    std::vector<Feature::Ptr> result;
    
                
    auto feat = src->next();
    while (feat) {
        try {
            if ((!where) || where->Call(feat, zoom)) {
                result.push_back(columns->Call(feat,zoom));
            }
            
        } catch (...) {}
        feat = src->next();
    }
    if (orderby) {
        try {
            std::sort(result.begin(), result.end(), orderby->Call(zoom));
        } catch (...) {}
    }
    
    auto res = MoveResult::Create(std::move(result));
    return res;
}
        
std::string Select::Str() const {
    std::stringstream ss;
    
    ss << "Select(" << columns->Str() << ", ";
    if (where) {
        ss << where->Str();
    } else {
        ss << "None";
    }
    ss << ", "  << table->Str() << ", ";
    if (orderby) {
        ss << orderby->Str();
    } else {
        ss << "None";
    }
    ss << ", " << quote_str(name) << ")";
    return ss.str();
}

std::string OrderItem::Str() const {
    return "OrderItem("+column->Str()+", "+(asc ? "True" : "False")+")";
}


class CPFeat : public Feature {
    public:
        CPFeat(Feature::Ptr pp_) : pp(pp_) {}
        
        
        virtual int64 Id() const { return pp->Id(); }
        virtual std::vector<std::string> Keys() const { return pp->Keys(); }
        
        virtual Value Get(const std::string& key, bool checkParent) const { return pp->Get(key,true); }
        
        virtual int64 MinZoom() const { return pp->MinZoom(); }
        virtual std::string Geometry() const { return pp->Geometry(); }
        virtual std::string GeometryAt(size_t i) const { return pp->GeometryAt(i); }
        virtual size_t NumGeometries() const { return pp->NumGeometries(); }
        virtual const std::string& OtherTags() const { return pp->OtherTags(); }
    private:
        Feature::Ptr pp;
};

Feature::Ptr make_cpfeat(Feature::Ptr pp) { return std::make_shared<CPFeat>(pp); }

OrderBy::cmp_func OrderBy::Call(int64 zoom) const {
    return [this, zoom](Feature::Ptr left_in, Feature::Ptr right_in) {
        auto left = make_cpfeat(left_in);
        auto right = make_cpfeat(right_in);
        for (auto ss: spec) {
            auto l = ss.column->Call(left, zoom);
            auto r = ss.column->Call(right, zoom);
            if (l<r) { return ss.asc; }
            if (l>r) { return !ss.asc; }
            
        }
        return false;
    };
}

std::string OrderBy::Str() const {
    return "OrderBy({"+all_str_obj(spec)+"})";
}          
        


}
