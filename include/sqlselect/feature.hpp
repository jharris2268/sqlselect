#ifndef FEATURE_HPP
#define FEATURE_HPP

#include <boost/variant.hpp>
#include <map>
#include <vector>
#include <string>
#include <memory>
#include <sstream>
#include <mapnik/value.hpp>


typedef int64_t int64;


namespace sqlselect {

/*    
struct null {};
inline bool operator<(null, null) { return false; }
inline bool operator==(null, null) { return false; }
inline std::ostream& operator<<(std::ostream& os, null) { os<<"null"; return os;}
struct boolean {
    bool b;
};    
inline bool operator<(boolean l, boolean r) { return l.b<r.b; }
inline bool operator==(boolean l, boolean r) { return l.b==r.b; }
inline std::ostream& operator<<(std::ostream& os, boolean b) {
    if (b.b) { os << "true"; }
    else { os<<"false"; }
    return os;
}

std::string quote_str(const std::string& v);


struct string_vec : public std::vector<std::string> {};
inline bool operator<(string_vec l, string_vec r) { return l<r; }
inline bool operator==(string_vec l, string_vec r) { return l==r; }

inline std::ostream& operator<<(std::ostream& os, const string_vec& vv) {
    bool f=true;
    os << "[";
    for (const auto& v: vv) {
        if (!f) { os << ", "; }
        os << sqlselect::quote_str(v);
        f=false;
    }
    os << "]";
    return os;
}
    
typedef boost::variant<null, boolean, int64, double, std::string,string_vec> Value;
std::string to_string(const Value& val);

inline bool test_null(const Value& v) { return v.which()==0; }
inline Value null_value() { return null{}; }
inline std::string get_string(const Value& v) { return boost::get<std::string>(v); }

bool is_integer(const Value& v);
bool is_number(const Value& v);
int64 get_integer(const Value& v);
double get_double_conv(const Value& v);

inline bool is_string(const Value& v) { return v.which()==4; }
*/
std::string quote_str(const std::string& v);
std::vector<std::string> split_string(const std::string& str, char delim);

typedef mapnik::value Value;

inline Value null_value() {
    mapnik::value_null n;
    return n;
}
inline std::string to_string(Value v) {
    return v.to_string();
}
inline Value s(const std::string& v) { return mapnik::value_unicode_string::fromUTF8(v); }
enum arithmitic_type {
    ADD=0,
    SUBTRACT,
    MULTIPLY,
    DIVIDE,
    MODULO
};
Value arithmitic(Value left, arithmitic_type op, Value right);
std::string arithmitic_type_str(arithmitic_type o);

typedef std::map<std::string,Value> KeyValueMap;

class Feature {
    public:
        typedef std::shared_ptr<Feature> Ptr;
        virtual int64 Id() const=0;
        virtual std::vector<std::string> Keys() const=0;
        
        virtual Value Get(const std::string& key, bool checkParent) const =0;
        
        virtual int64 MinZoom() const=0;
        virtual std::string Geometry() const =0;
        
        virtual size_t NumGeometries() const =0;
        virtual std::string GeometryAt(size_t i) const =0;
        
        
        virtual const std::string& OtherTags() const=0;
};



Feature::Ptr make_feature(int64 id, const KeyValueMap& props, int64 minzoom, const std::vector<std::string>& geometry);
Feature::Ptr make_feature(int64 id, const KeyValueMap& props, int64 minzoom, const std::vector<std::string>& geometry, const std::string& other_tags);
Feature::Ptr make_feature_modified(Feature::Ptr parent, const KeyValueMap& props, bool parentvals);


template <class T>
std::string all_str(const std::vector<T>& vals) {
    std::stringstream ss;
    bool f=true;
    for (auto v: vals) {
        if (!f) { ss << ", "; }
        f=false;
        ss << v->Str();
    }
    return ss.str();
}
template <class T>
std::string all_str_obj(const std::vector<T>& vals) {
    std::stringstream ss;
    bool f=true;
    for (auto v: vals) {
        if (!f) { ss << ", "; }
        f=false;
        ss << v.Str();
    }
    return ss.str();
}
typedef std::vector<Feature::Ptr> Feature_vec;
typedef std::map<std::string,sqlselect::Feature_vec> Data_map;

}


#endif
