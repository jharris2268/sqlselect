#include "sqlselect_py.hpp"
using namespace std::string_literals;


using pjv = picojson::value;
using pjo = picojson::object;
using pja = picojson::array;

typedef std::function<std::pair<double,double>(double,double)> transform_func;

const double earth_width = 40075016.68557849;


int64 read_geojson_integer(const pjv& val) {
    if (val.is<int64_t>()) { return val.get<int64_t>(); }
    if (val.is<std::string>()) { return std::stol(val.get<std::string>()); }
    if (val.is<double>()) { return (int64) val.get<double>(); }
    
    throw std::domain_error("can't read as integer "+val.serialize());
}

sqlselect::Value read_geojson_value(const pjv& val) {
    if (val.is<int64_t>()) {
        return sqlselect::Value(val.get<int64_t>());
    }
    
    if (val.is<double>()) {
        return sqlselect::Value(val.get<double>());
    }
    
    if (val.is<bool>()) {
        return sqlselect::Value(val.get<bool>());
    }
    if (val.is<std::string>()) {
        auto vs = val.get<std::string>();
        return sqlselect::s(vs);
    }
    return sqlselect::null_value();
}

int64 read_geojson_properties(const pjv& val, sqlselect::KeyValueMap& kvm) {
    int64 minzoom=-1;
    
    if (!val.is<pjo>()) { throw std::domain_error("properties isn't an object"); }
    
    const pjo& obj = val.get<pjo>();
    
    for (const auto& pp: obj) {
        if (pp.first == "min_zoom") {
            minzoom = read_geojson_integer(pp.second);
        } else {
            kvm[pp.first] = read_geojson_value(pp.second);
        }
    }
    return minzoom;
}

std::string get_type(const pjo& val) {
    if (val.count("type") == 0) { throw std::domain_error("no type"); }
    const auto& tyv = val.at("type");
    if (!tyv.is<std::string>()) { throw std::domain_error("type isn't a string"); }
    return tyv.get<std::string>();
}
        


size_t write_uint32(std::string& data, size_t pos, uint32_t i) {
    union {
        uint32_t si;
        char sc[4];
    };
    si=i;
    data[pos] = sc[0];
    data[pos+1] = sc[1];
    data[pos+2] = sc[2];
    data[pos+3] = sc[3];
    return pos+4;
}



size_t write_double(std::string& data, size_t pos, double d) {
    union {
        double sd;
        char sc[8];
    };
    sd=d;
    data[pos] = sc[0];
    data[pos+1] = sc[1];
    data[pos+2] = sc[2];
    data[pos+3] = sc[3];
    data[pos+4] = sc[4];
    data[pos+5] = sc[5];
    data[pos+6] = sc[6];
    data[pos+7] = sc[7];
    return pos+8;
}

size_t write_point(std::string& s, size_t p, const pja& coords, const transform_func& forward) {
    if (coords.size()!=2) { throw std::domain_error("expected two coords"); }
    
    if (!(coords[0].is<double>() && coords[1].is<double>())) { throw std::domain_error("expected two coords"); }
    
    auto pt = forward(coords[0].get<double>(), coords[1].get<double>());
    p=write_double(s, p, pt.first);
    return write_double(s, p, pt.second);
    
}

std::string read_point(const pja& coords, const transform_func& forward) {
    
    std::string s(16,'\0');
    write_point(s, 0, coords, forward);
    return s;
}

std::string read_ring(const pja& coords, const transform_func& forward) {
    size_t nr = coords.size();
    
    std::string s(4 + 16*nr, '\0');
    size_t p=0;
    
    p=write_uint32(s, p, nr);
    for (const auto& cc: coords) {
        if (!cc.is<pja>()) { throw std::domain_error("can't read ring"); }
        p = write_point(s, p, cc.get<pja>(), forward);
    }
    return s;
}
    
    
std::string read_geojson_point(const pja& coords, const transform_func& forward) {
    return "\1\1\0\0\0"s+read_point(coords, forward);
}

std::string read_geojson_multipoint(const pja& coords, const transform_func& forward) {
    size_t nl = coords.size();
    std::string rs(9,'\0');
    rs[0]='\1'; rs[1]='\4'; write_uint32(rs, 5, nl);
    
    for (auto rr: coords) {
        if (!rr.is<pja>()) { throw std::domain_error("can't read multipoint"); }
        rs += read_geojson_point(rr.get<pja>(), forward);
    }
    return rs;
    
}

std::string read_geojson_linestring(const pja& coords, const transform_func& forward) {
    return "\1\2\0\0\0"s+read_ring(coords, forward);
}


std::string read_geojson_multilinestring(const pja& coords, const transform_func& forward) {
    size_t nl = coords.size();
    std::string rs(9,'\0');
    rs[0]='\1'; rs[1]='\5'; write_uint32(rs, 5, nl);
    
    for (auto rr: coords) {
        if (!rr.is<pja>()) { throw std::domain_error("can't read multilinestring"); }
        rs += read_geojson_linestring(rr.get<pja>(), forward);
    }
    return rs;
    
}



std::string read_geojson_polygon(const pja& coords, const transform_func& forward) {
    
    auto nr = coords.size();
    std::string rs(9,'\0');
    rs[0]='\1'; rs[1]='\3';
    write_uint32(rs,5,nr);
    for (auto rr: coords) {
        if (!rr.is<pja>()) { throw std::domain_error("can't read polygon"); }
        rs += read_ring(rr.get<pja>(), forward);
    }
    return rs;
}
    

std::string read_geojson_multipolygon(const pja& coords, const transform_func& forward) {
    auto np = coords.size();
    std::string rs(9,'\0');
    rs[0]='\1'; rs[1]='\6'; write_uint32(rs, 5, np);
    
    for (auto rr: coords) {
        if (!rr.is<pja>()) { throw std::domain_error("can't read multipolygon"); }
        rs += read_geojson_polygon(rr.get<pja>(), forward);
    }
    return rs;
    
}
    


std::string read_geojson_geometry(const pjv& val, const transform_func& forward) {
    if (!val.is<pjo>()) { throw std::domain_error("geometry isn't an object"); }
    const pjo& obj = val.get<pjo>();
    
    auto ty = get_type(obj);
    
    if (ty=="GeometryCollection") {
        return "\1\7\0\0\0\0\0\0\0";
    }
    
    if (obj.count("coordinates")==0) { throw std::domain_error("no coordinates"); }
    const auto& cc = obj.at("coordinates");
    if (!cc.is<pja>()) { throw std::domain_error("coordinates is not an array"); }
    
    const auto& coords = cc.get<pja>();
    
    if (ty == "Point") {
        return read_geojson_point(coords, forward);
    } else if (ty == "MultiPoint") {
        return read_geojson_multipoint(coords, forward);
    } else if (ty == "LineString") {
        return read_geojson_linestring(coords, forward);
    } else if (ty == "MultiLineString") {
        return read_geojson_multilinestring(coords, forward);
    } else if (ty == "Polygon") {
        return read_geojson_polygon(coords, forward);
    } else if (ty == "MultiPolygon") {
        return read_geojson_multipolygon(coords, forward);
    }
    throw std::domain_error("unexpected type "+ty);
    return "";
}

std::vector<std::string> read_geojson_geometry_split(const pjv& val, const transform_func& forward) {
    if (!val.is<pjo>()) { throw std::domain_error("geometry isn't an object"); }
    const pjo& obj = val.get<pjo>();
    
    auto ty = get_type(obj);
    std::vector<std::string> result;
    if (ty=="GeometryCollection") {
        return result;
    }
    
    if (obj.count("coordinates")==0) { throw std::domain_error("no coordinates"); }
    const auto& cc = obj.at("coordinates");
    if (!cc.is<pja>()) { throw std::domain_error("coordinates is not an array"); }
    
    const auto& coords = cc.get<pja>();
    
    if (ty == "Point") {
        result.push_back(read_geojson_point(coords, forward));
    } else if (ty == "MultiPoint") {
        for (const auto& cc: coords) {
            if (!cc.is<pja>()) { throw std::domain_error("expected array or arrays..."); }
            result.push_back(read_geojson_point(cc.get<pja>(), forward));
        }
        
    } else if (ty == "LineString") {
        result.push_back(read_geojson_linestring(coords, forward));
    } else if (ty == "MultiLineString") {
        for (const auto& cc: coords) {
            if (!cc.is<pja>()) { throw std::domain_error("expected array or arrays..."); }
            result.push_back(read_geojson_linestring(cc.get<pja>(), forward));
        }
    } else if (ty == "Polygon") {
        result.push_back(read_geojson_polygon(coords, forward));
    } else if (ty == "MultiPolygon") {
        for (const auto& cc: coords) {
            if (!cc.is<pja>()) { throw std::domain_error("expected array or arrays..."); }
            result.push_back(read_geojson_polygon(cc.get<pja>(), forward));
        }
    } else {
        throw std::domain_error("unexpected type "+ty);
    }
    return result;
}

void read_geojson_feature_int(sqlselect::Feature_vec& result, const pjo& obj, const transform_func& forward, bool split_multis) {
    auto ty = get_type(obj);
    if (ty != "Feature") {
        throw std::domain_error("expected type == Feature not "+ty);
    }
    
    
    
    if (obj.count("id")==0) { throw std::domain_error("no id"); }
    int64 id = read_geojson_integer(obj.at("id"));
    
    if (obj.count("properties")==0) { throw std::domain_error("no properties"); }
    sqlselect::KeyValueMap kvm;
    
    int64 minzoom = read_geojson_properties(obj.at("properties"), kvm);
    
    
    if (obj.count("geometry")==0) { throw std::domain_error("no geometry"); }
    
    
    std::vector<std::string> geoms = read_geojson_geometry_split(obj.at("geometry"),forward);
    if (!split_multis) {
        
        result.push_back(sqlselect::make_feature(id, kvm, minzoom, geoms, ""));
    } else {
        for (const auto& s: geoms) {
            result.push_back(sqlselect::make_feature(id, kvm, minzoom, {s}, ""));
        }
    }
    
}
sqlselect::Feature::Ptr read_geojson_feature(const pjo& obj, const transform_func& forward) {
    sqlselect::Feature_vec rr;
    read_geojson_feature_int(rr, obj, forward, false);
    if (rr.size()!=1) { throw std::domain_error("???"); }
    return rr.at(0);
    
}
    

sqlselect::Feature_vec read_geojson_featurecollection(const pjo& obj, const transform_func& forward) {
    if (obj.count("type")==0) { throw std::domain_error("not a geojson featurecollection [no type]"); }
    const auto& ty=obj.at("type");
    if (!(ty.is<std::string>() && (ty.get<std::string>()=="FeatureCollection"))) { throw std::domain_error("not a geojson featurecollection [wrong type]"); }
    
    if (obj.count("features")==0) { throw std::domain_error("not a geojson featurecollection [no features]"); }
    if (!obj.at("features").is<pja>()) { throw std::domain_error("not a geojson featurecollection [features not an array]"); }
    const auto& arr = obj.at("features").get<pja>();
    
    sqlselect::Feature_vec result;
    for (const auto& ff: arr) {
        if (!ff.is<pjo>()) { throw std::domain_error("object at "+std::to_string(result.size())+" not a geojson feature"); }
        
        try {
            read_geojson_feature_int(result, ff.get<pjo>(), forward, false);
        } catch (std::exception& ex) {
            throw std::domain_error("object at "+std::to_string(result.size())+" failed: "+ex.what());
        }
    }
    return result;
}
    


sqlselect::Data_map read_geojson_tile(const std::string& data_in, int64 tx, int64 ty, int64 tz, int64 np) {
    pjv data;
    std::string err = picojson::parse(data, data_in);
    if (!err.empty()) {
        throw std::domain_error("can't read json data "+err);
    }
    if (!data.is<pjo>()) { 
        throw std::domain_error("data not an object");
    }
    const pjo& data_obj = data.get<pjo>();
    
    transform_func forward=[](double x, double y) { return std::make_pair(x,y); };
    if (np!=0) {
        double ts = earth_width / (1ll<<tz);
        double sf = ts / np;
        double x0 = tx*ts - earth_width/2;
        double y0 = earth_width/2 - (ty+1)*ts;
        forward = [sf,x0,y0](double x, double y) { return std::make_pair(x0 + x*sf, y0+y*sf); };
    }
       
        
    
    sqlselect::Data_map result;
    if ((data_obj.count("type")>0)) {
        if (data_obj.at("type").is<std::string>() && (data_obj.at("type").get<std::string>()=="FeatureCollection")) {
            result[""] = read_geojson_featurecollection(data_obj, forward);
        } else if (data_obj.at("type").is<std::string>() && (data_obj.at("type").get<std::string>()=="Feature")) {
            auto f = read_geojson_feature(data_obj, forward);
            result[""] = sqlselect::Feature_vec{f};
        } else {
            throw std::domain_error("unexpected object at root");
        }
            
        return result;
    }
    
    for (const auto& tab: data_obj) {
        try {
            if (!tab.second.is<pjo>()) {
                throw std::domain_error(tab.first + " isn't an object");
            }
            result[tab.first] = read_geojson_featurecollection(tab.second.get<pjo>(), forward);
        } catch (std::exception& ex) {
            throw std::domain_error(tab.first+" failed: "+ex.what());
        }
    }
    return result;
}

/*PYBIND11_PLUGIN(_geojson) {
    using namespace sqlselect;
    py::module m("_geojson", "sqlselect");
*/

void export_geojson(py::module& m) {    
    m.def("read_geojson_tile", &read_geojson_tile);
    
}
