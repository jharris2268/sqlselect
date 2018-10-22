
#include <oqt/utils/compress.hpp>
#include <oqt/utils/pbf/protobuf.hpp>
#include <oqt/utils/pbf/fixedint.hpp>

#include "sqlselect_py.hpp"
using namespace std::string_literals;

typedef std::function<std::pair<double,double>(double,double,double)> transform_func;


double read_float(const std::string& data, size_t pos) {
    union {
        float f;
        char sc[4];
    };
    
    sc[0] = data[pos];
    sc[1] = data[pos+1];
    sc[2] = data[pos+2];
    sc[3] = data[pos+3];
    return f;
}

double read_double(const std::string& data, size_t pos) {
    union {
        double d;
        char sc[8];
    };
    
    sc[0] = data[pos];
    sc[1] = data[pos+1];
    sc[2] = data[pos+2];
    sc[3] = data[pos+3];
    sc[4] = data[pos+4];
    sc[5] = data[pos+5];
    sc[6] = data[pos+6];
    sc[7] = data[pos+7];
    return d;
}


sqlselect::Value read_value(const std::string& data) {
    
    if ((data[0]==25) && (data.size()==9)) {
        return sqlselect::Value(read_double(data, 1));
    }
    if ((data[0]==21) && (data.size()==5)) {
        return sqlselect::Value(read_float(data, 1));
    }
    
    size_t pos=0;
    auto tg=oqt::readPbfTag(data,pos);
    if (pos != data.size()) { throw std::domain_error("value with more than one tag???"); }
    
    if (tg.tag==1) { return sqlselect::s(tg.data); }
    
    if (tg.tag==4) { return sqlselect::Value(tg.value); }
    if (tg.tag==5) { return sqlselect::Value((int64) tg.value); }
    if (tg.tag==6) { return sqlselect::Value(oqt::unZigZag(tg.value)); }
    if (tg.tag==7) { return sqlselect::Value((tg.value==1)); }
    
    return sqlselect::null_value();
}

struct dedelta {
    dedelta() : d(0) {}
    
    int64 operator()(oqt::uint64 v) {
        d += oqt::unZigZag(v);
        return d;
    }
    int64 d;
};

size_t add_point(std::string& s, size_t pos, const transform_func& forward, int64 np, int64 x, int64 y) {
    auto xy = forward(x, y, np);
    pos = oqt::write_double_le(s, pos, xy.first);
    return oqt::write_double_le(s, pos, xy.second);
}

size_t read_points_int(std::vector<std::string>& points, const transform_func& forward, int64 np, dedelta& x, dedelta& y,const std::vector<oqt::uint64>& cmds, size_t pos) {

    if ((cmds[pos]&7)!=1) { throw std::domain_error("not a point: first cmd not a moveto"); }
    size_t npts = cmds[pos] >> 3;    
        
    pos++;
    for (size_t i=0; i < npts; i++) {
        std::string s(21, '\0');
        s[0] = '\1';
        s[1] = '\1';
        add_point(s, 5, forward, np, x(cmds[pos]), y(cmds[pos+1]));
        pos+=2;
        
        points.push_back(s);
    }
    return pos;
}

std::vector<std::string> read_point(const transform_func& forward, int64 np, const std::vector<oqt::uint64>& cmds) {
    
    
    std::vector<std::string> points;
    dedelta x,y;
    size_t pos=0;
    while (pos < cmds.size()) {
        pos= read_points_int(points, forward, np, x, y, cmds, pos);
    }
    
/*    if (points.size()==0) {
        throw std::domain_error("no points??");
    }
    if (points.size()==1) { return points[0]; }
    
        
    std::string result(9 + 21*points.size(), '\0');
    result[0]='\1';
    result[1]='\4';
    
    size_t p = 5;
    for (const auto& pt: points) {
        std::copy(pt.begin(), pt.end(), result.begin()+p);
        p+=21;
    }
    return result;*/
    return points;
}

size_t read_ring(std::vector<std::string>& rings, const transform_func& forward, int64 np, dedelta& x, dedelta& y,const std::vector<oqt::uint64>& cmds, size_t pos, bool ispoly) {
    
    if ((cmds[pos])!=9) { throw std::domain_error("failed: ring doesn't start with a single moveto"); }
    
    
    if ((cmds[pos+3]&7) != 2) { throw std::domain_error("failed: ring doesn't have a lineto after the first moveto"); }
    size_t nlt = cmds[pos+3]>>3;
    size_t endpos = pos + 4 + 2*nlt;
    size_t npts = nlt + 1;
    
    if (ispoly) {
        if ((cmds[pos+4 + 2*nlt])!= 15) { throw std::domain_error("failed: polygon ring doesn't end with a single close"); }
        npts += 1;
        endpos+=1;
    }
    
    std::string result(4 + npts*16, '\0');
    
    size_t wpos = oqt::write_uint32_le(result, 0, npts);
    
       
    wpos = add_point(result, wpos, forward, np, x(cmds[pos+1]), y(cmds[pos+2]));
    for (size_t j = 0; j < nlt; j++) {
        wpos = add_point(result, wpos, forward, np, x(cmds[pos+4+2*j]), y(cmds[pos+5+2*j]));
    }
    
    if (ispoly) {
        std::copy(result.begin()+4, result.begin()+20, result.begin()+wpos);
        wpos+=16;
    }
    
    if (wpos !=result.size()) { throw std::domain_error("failed: ring length wrong?"); }
    
    rings.push_back(result);
    return endpos;
}


std::vector<std::string> read_rings(const transform_func& forward, int64 np, const std::vector<oqt::uint64>& cmds, bool is_poly) {
    size_t pos=0;
    
    std::vector<std::string> result;
    dedelta x, y;
    while (pos < cmds.size()) {
        pos = read_ring(result, forward, np, x, y, cmds, pos,is_poly);
    }
    return result;
}



std::vector<std::string> read_linestring(const transform_func& forward, int64 np, const std::vector<oqt::uint64>& cmds) {
    
    std::vector<std::string> rings = read_rings(forward,np,cmds,false);
    
    std::vector<std::string> result;
    for (const auto s: rings) {
        result.push_back("\1\2\0\0\0"s + s);
    }
    return result;
    
    /*if (rings.size()==0) { throw std::domain_error("no linestrings"); }
    if (rings.size()==1) {
        return std::string("\1\2\0\0\0",5)+rings[0];
    }
    size_t tl = 9;
    for (const auto& s: rings) { tl+=(5+s.size()); }
    std::string result(tl,'\0');
    result[0]='\1';
    result[1]='\5';
    size_t pos = oqt::write_uint32_le(result, 5, rings.size());
    for (const auto& s: rings) {
        result[pos]='\1'; pos++;
        pos=oqt::write_uint32_le(result, pos, 2);
        std::copy(s.begin(),s.end(),result.begin()+pos);
        pos+=s.size();
    }
    return result;*/
}   

std::vector<std::string> read_polygon(const transform_func& forward, int64 np, const std::vector<oqt::uint64>& cmds) {
    
    std::vector<std::string> rings = read_rings(forward,np,cmds,true);
    if (rings.size()==0) { throw std::domain_error("no polygons"); }
    
    size_t tl = 9;
    for (const auto& s: rings) { tl+=s.size(); }
    std::string result(tl,'\0');
    result[0]='\1';
    result[1]='\3';
    size_t pos = oqt::write_uint32_le(result, 5, rings.size());
    for (const auto& s: rings) {
        
        std::copy(s.begin(),s.end(),result.begin()+pos);
        pos+=s.size();
    }
    return {result};
}  


std::vector<std::string> read_mvt_geometry(const transform_func& forward, int64 np, oqt::uint64 gt, const std::vector<oqt::uint64>& cmds) {
    if (gt==1) {
        return read_point(forward, np, cmds);
    } else if (gt==2) {
        return read_linestring(forward, np, cmds);
    } else if (gt==3) {
        return read_polygon(forward, np, cmds);
    }
    throw std::domain_error("unexpected geometry type "+std::to_string(gt));
    return {};
}

sqlselect::Feature::Ptr read_mvt_feature(
    const transform_func& forward, int64 np,
    const std::vector<std::string>& keys, const std::vector<sqlselect::Value> vals, 
    const std::string& data) {
        
    sqlselect::KeyValueMap kvm;
    int64 id=0;
    int64 minzoom=-1;
    size_t gt=0;
    std::vector<oqt::uint64> cmds;
    
    
    size_t pos=0;
    oqt::PbfTag tg = oqt::readPbfTag(data, pos);
    for ( ; tg.tag>0; tg=oqt::readPbfTag(data, pos)) {
        if (tg.tag==1) {
            id = (int64) tg.value;
        } else if (tg.tag==2) {
            auto ii = oqt::readPackedInt(tg.data);
            for (size_t i=0; i < ii.size(); i+=2) {
                if (keys.at(ii.at(i))=="min_zoom") {
                    try {
                        minzoom = vals.at(ii.at(i+1)).to_int();
                    } catch ( ... ) {}
                } else {
                    kvm[keys.at(ii.at(i))] = vals.at(ii.at(i+1));
                }
            }
        } else if (tg.tag==3) {
            gt=tg.value;
        } else if (tg.tag==4) {
            cmds = oqt::readPackedInt(tg.data);
        } else if (tg.tag==8) {
            //bounds, skip
        }
    }
    if (kvm.count("tags")==0) {
        kvm["tags"]=sqlselect::Value(false);
    }
    return sqlselect::make_feature(id,kvm,minzoom,read_mvt_geometry(forward, np, gt, cmds));
}       
    

void read_mvt_layer(sqlselect::Data_map& result, const transform_func& forward, const std::string& data) {
    
    
    std::vector<std::string> feature_blobs;
    std::vector<std::string> keys;
    std::vector<sqlselect::Value> vals;
    std::string table;
    int64 np=4096;
    
    size_t pos=0;
    oqt::PbfTag tg = oqt::readPbfTag(data, pos);
    for ( ; tg.tag>0; tg=oqt::readPbfTag(data, pos)) {
        if (tg.tag==15) {
            if (tg.value!=2) { throw std::domain_error("wrong version??"); }
        } else if (tg.tag==1) {
          table=tg.data;  
        } else if (tg.tag==2) {
            feature_blobs.push_back(tg.data);
        } else if (tg.tag==3) {
            keys.push_back(tg.data);
        } else if (tg.tag==4) {
            vals.push_back(read_value(tg.data));
        } else if (tg.tag==5) {
            np = tg.value;
        } else if (tg.tag==9) {
            np = tg.value; //TEMPORARY: IDIOT CODING IN OQTTILES
        }
    }
    //std::cout << "layer " << table << " (np=" << np << ") " << keys.size() << " keys, "  << " " << vals.size() << " vals, " << " " << feature_blobs.size() << " features"  << std::endl;
    
    sqlselect::Feature_vec features;
    features.reserve(feature_blobs.size());
    for (const auto& f: feature_blobs) {
        features.push_back(read_mvt_feature(forward, np, keys, vals, f));
    }
    result[table] = features;
    
}


sqlselect::Data_map read_mvt_tile(const std::string& data_in, int64 tx, int64 ty, int64 tz, bool gzipped) {
    
    if (gzipped) {
        return read_mvt_tile(oqt::decompress_gzip(data_in), tx, ty, tz, false);
    }
    
    const double earth_width = 40075016.68557849;

    double ts = earth_width / (1ll<<tz);
    double x0 = tx*ts - earth_width/2;
    double y0 = earth_width/2 - (ty)*ts;
    transform_func forward = [ts,x0,y0](double x, double y, double np) { return std::make_pair(x0 + (ts*x)/np, y0 - (ts*y)/np); };
    
    //auto p = forward(0,4096,4096);
    //auto q = forward(4096,0,4096);
    //std::cout << tx << ", " << ty << ", " << tz << "=> " << p.first << ", " << p.second << ", " << q.first << ", " << q.second << std::endl;
    
    sqlselect::Data_map result;
    
    size_t pos=0;
    oqt::PbfTag tg = oqt::readPbfTag(data_in, pos);
    
    for ( ; tg.tag>0; tg=oqt::readPbfTag(data_in, pos)) {
        if (tg.tag==3) {
            read_mvt_layer(result, forward, tg.data);
        } else {
            std::cout << "?? " << tg.tag << " " << tg.value << " " << tg.data << std::endl;
        }
    }
    
    return result;
}



void export_mvt(py::module& m) {
    m.def("read_mvt_tile", &read_mvt_tile);
};
