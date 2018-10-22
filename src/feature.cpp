#include "feature.hpp"


#include <iomanip>
#include <algorithm>
using namespace std::string_literals;


namespace sqlselect {

std::string make_multi(const std::vector<std::string>& geoms) {
    if (geoms.empty()) { return "\1\7\0\0\0\0\0\0\0"s; }
    if (geoms.size()==1) { return geoms.at(0); }
    
    size_t ss=9;
    for (const auto& g: geoms) { ss+=g.size(); }
    std::string result(ss, '\0');
    result[0]='\1';
    result[1]='\3' + geoms[0][1];
    
    size_t gl=geoms.size();
    result[5] = gl&255;
    result[6] = (gl>>8)&255;
    result[7] = (gl>>16)&255;
    result[8] = (gl>>24)&255;
    
    size_t p=9;
    for (const auto& g: geoms) {
        std::copy(g.begin(),g.end(), result.begin()+p);
        p+=g.size();
    }
    return result;
}

class FeatureImpl : public Feature {
    public:
        FeatureImpl(int64 id_, const KeyValueMap& props_, int64 minzoom_, const std::vector<std::string>& geometry_, const std::string& other_tags_)
            : id(id_), props(props_), minzoom(minzoom_), geometry(geometry_), other_tags(other_tags_) {}
        virtual ~FeatureImpl() {}
        
        int64 Id() const { return id; }
        std::vector<std::string> Keys() const {
            std::vector<std::string> keys;
            keys.reserve(props.size());
            for (const auto& p: props) { keys.push_back(p.first); }
            return keys;
        }
        
        Value Get(const std::string& key, bool checkParent) const {
            auto it = props.find(key);
            if (it!=props.end()) {
                return it->second;
            }
            return null_value();
        }
        
        int64 MinZoom() const { return minzoom; }
        std::string Geometry() const {
            return make_multi(geometry);
        }
        
        std::string GeometryAt(size_t i) const {
            return geometry.at(i);
        }
        size_t NumGeometries() const { return geometry.size(); }
        
        const std::string& OtherTags() const { return other_tags; }
        
    private:
        int64 id;
        KeyValueMap props;
        int64 minzoom;
        std::vector<std::string> geometry;
        
        std::string other_tags;
        
};

Feature::Ptr make_feature(int64 id, const KeyValueMap& props, int64 minzoom, const std::vector<std::string>& geometry) {
    return std::make_shared<FeatureImpl>(id,props,minzoom,geometry,"");
}

Feature::Ptr make_feature(int64 id, const KeyValueMap& props, int64 minzoom, const std::vector<std::string>& geometry, const std::string& other_tags) {
    return std::make_shared<FeatureImpl>(id,props,minzoom,geometry,other_tags);
}

class FeatureModified : public Feature {
    public:
        FeatureModified(Feature::Ptr parent_, const std::map<std::string,Value>& props_, bool parentvals_) :
            parent(parent_), props(props_),parentvals(parentvals_) {}
        
        virtual ~FeatureModified() {}
        
        int64 Id() const { return parent->Id(); }
        int64 MinZoom() const { return parent->MinZoom(); }
        std::string Geometry() const { return parent->Geometry(); }
        std::string GeometryAt(size_t i) const { return parent->GeometryAt(i); }
        size_t NumGeometries() const { return parent->NumGeometries(); }
        
        const std::string& OtherTags() const { return parent->OtherTags(); }
        std::vector<std::string> Keys() const {
            std::vector<std::string> keys;
            keys.reserve(props.size());
            for (const auto& p: props) { keys.push_back(p.first); }
            if (parentvals) {
                auto pk = parent->Keys();
                for (auto k: pk) {
                    if (std::count(keys.begin(),keys.end(),k)==0) {
                        keys.push_back(k);
                    }
                }
            }
            
            return keys;
        }
        
        Value Get(const std::string& key, bool checkParent) const {
            auto it = props.find(key);
            if (it!=props.end()) {
                return it->second;
            }
            if (parentvals || checkParent) {
                return parent->Get(key, checkParent);
            }
            return null_value();
        }
    private:
        Feature::Ptr parent;
        const std::map<std::string,Value> props;
        bool parentvals;
};

Feature::Ptr make_feature_modified(Feature::Ptr parent, const KeyValueMap& props, bool parentvals) {
    return std::make_shared<FeatureModified>(parent,props,parentvals);
}


std::vector<std::string> split_string(const std::string& str, char delim) {
    return {str};
}

std::string quote_str(const std::string& v) {
    std::stringstream ss;
    ss << std::quoted(v);
    return ss.str();
};
/*
inline std::string double_str(double d, int w, int p) { 
    std::stringstream ss;
    ss << std::fixed;
    if (w!=0) { ss << std::setw(w); }
    if (p>=0) { ss << std::setprecision(p); }
    ss << d;
    return ss.str();
}

struct valstr {
    
    std::string operator()(const std::string& v) const {
        return quote_str(v);
    }
    
    std::string operator()(null) const {
        return "None";
    }
    
    std::string operator()(int64 i) const {
        return std::to_string(i);
    }
    std::string operator()(double f) const {
        
        return double_str(f,0,1);
    }
    std::string operator()(boolean b) const {
        return b.b ? "True" : "False";
    }
    
    std::string operator()(const string_vec& vv) const {
        std::stringstream os;
        bool f=true;
        os << "[";
        for (const auto& v: vv) {
            if (!f) { os << ", "; }
            os << sqlselect::quote_str(v);
            f=false;
        }
        os << "]";
        
        return os.str();
    }
    
};
*/


std::string to_string(const Value& val) {
    return val.to_string();
    //return boost::apply_visitor(valstr(), val);
    //return "Value["+std::to_string(val.which())+"]("+boost::apply_visitor(valstr(), val)+")";
}
/*
bool is_integer(const Value& val) { return val.which()==2; }

bool is_number(const Value& val) {
    return (val.which()==2) || (val.which()==3);
};

int64 get_integer(const Value& val) {
    return boost::get<int64>(val);
}

double get_double_conv(const Value& val) {
    try {
        return boost::get<double>(val);
    } catch (...) {}
    
    int64 i = boost::get<int64>(val);
    return i;
        
}

template <class T>
T modulo(T left, T right) {
    return left % right;
}

template <>
double modulo(double left, double right) {
    return fmod(left, right);
}
*/
template <class T>
T arithmitic_internal(T left, arithmitic_type op, T right) {
    switch (op) {
        case ADD: return left+right;
        case SUBTRACT: return left-right;
        case DIVIDE: return left/right;
        case MULTIPLY: return left*right;
        //case MODULO: return modulo<T>(left,right);
        case MODULO: return left%right;
    }
    return null_value();
};

Value arithmitic(Value left, arithmitic_type op, Value right) {
    
    return arithmitic_internal(left,op,right);
    
    /*if (!(is_number(left) && is_number(right))) { return null_value(); }
    
    try {
        return arithmitic_internal<int64>(get_integer(left), op, get_integer(right));
    } catch (...) {}
    
    try {
        return arithmitic_internal<double>(get_double_conv(left), op, get_double_conv(right));
    } catch(...) {}
    throw std::domain_error("?? "+ to_string(left)+ " " + arithmitic_type_str(op) + " " + to_string(right));
    return null_value();*/
}

std::string arithmitic_type_str(arithmitic_type o) {
    switch (o) {
        case ADD: return "+";
        case SUBTRACT: return "-";
        case DIVIDE: return "/";
        case MULTIPLY: return "*";
        case MODULO: return "%";
    }
    return "??";
}

}


