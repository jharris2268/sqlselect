#include "sqlselect_py.hpp"


namespace sqlselect {


    
Table::Ptr example() {
    return Select::Create(
        ColumnList::Create({Label::Create("name"), Label::Create("highway"), Label::Create("railway")},false),
        PickTable::Create("planet_osm_line"),
        And::Create(
            Not::Create(IsNull::Create(Label::Create("name"))),
            Or::Create(
                Op::Create(Label::Create("railway"),EQ,Val::Create(s("rail"))),
                In::Create(Label::Create("highway"),
                    {Val::Create(s("residential")),Val::Create(s("unclassified")),Val::Create(s("tertiary")),Val::Create(s("secondary")),Val::Create(s("primary"))})
            )
        ),
        nullptr,
        ""
    );
}
}

/*
struct extract_value {
    public:
        py::object operator()(const std::string& s) const { return py::str(s); }
        
        py::object operator()(sqlselect::null) const { return py::none(); }
        py::object operator()(sqlselect::boolean b) const { return py::cast(b.b); }

        py::object operator()(const sqlselect::string_vec& sv) { return py::cast(sv); }
        template <class T>
        py::object operator()(const T& v) const { return py::cast(v); }
};*/

struct extract_value {
    public:
        py::object operator()(const mapnik::value_unicode_string& r) const {
            if (r.isEmpty()) { return py::str(""); }
            std::string s;
            r.toUTF8String(s);
            return py::str(s);
        }
        py::object operator()(mapnik::value_null) const { return py::none(); }
        template <class T>
        py::object operator()(const T& v) const { return py::cast(v); }
};

py::object conv_val(const sqlselect::Value& v) {
    return mapnik::util::apply_visitor(extract_value(), v);
}
//#include <oqt/utils.hpp>
#include <oqt/elements/block.hpp>
#include <oqt/geometry/elements/point.hpp>
#include <oqt/geometry/elements/linestring.hpp>
#include <oqt/geometry/elements/simplepolygon.hpp>
#include <oqt/geometry/elements/complicatedpolygon.hpp>
#include <mapnik/feature.hpp>
#include <mapnik/wkb.hpp>
#include <mapnik/util/geometry_to_wkb.hpp>
#include <mapnik/featureset.hpp>
using namespace oqt;

namespace xxx {


sqlselect::Feature::Ptr make_feature_from_geom(ElementPtr ele, int64 block_quadtree, bool extra_tags_included) {
    if (!ele) { return nullptr; }
    
    if (ele->Type()<ElementType::Point) { return nullptr; }
        
    auto geom = std::dynamic_pointer_cast<BaseGeometry>(ele);
    std::string other_tags;
    
    sqlselect::KeyValueMap kvm;
    for (const auto& tg: geom->Tags()) {
        if (tg.val[0] == '\0') {
            kvm[tg.key] = sqlselect::Value(true);
            other_tags=tg.val;
        } else {
            
            kvm[tg.key]=sqlselect::s(tg.val);
        }
    }
    if (extra_tags_included) {
        kvm["tags"] = sqlselect::Value(false);
    }
    
    
    if (block_quadtree!=-1) {
        kvm["block_quadtree"]=block_quadtree;
    }
    kvm["quadtree"] = geom->Quadtree();
    int64 id_ = geom->Id();
    
    if (ele->Type() == ElementType::Point) {
        //pass
    } else if (ele->Type() == ElementType::Linestring) {
        auto ls = std::dynamic_pointer_cast<oqt::geometry::Linestring>(ele);
        if (ls) {
            kvm["length"] = ls->Length();
            kvm["z_order"] = (int64) ls->ZOrder();
            kvm["layer"] = ls->Layer();
        }
    } else if (ele->Type() == ElementType::SimplePolygon) {
        auto sp = std::dynamic_pointer_cast<oqt::geometry::SimplePolygon>(ele);
        if (sp) {
            kvm["way_area"] = sp->Area();
            kvm["z_order"] = (int64) sp->ZOrder();
            kvm["layer"] = sp->Layer();
        }
    } else if (ele->Type() == ElementType::ComplicatedPolygon) {
        auto cp = std::dynamic_pointer_cast<oqt::geometry::ComplicatedPolygon>(ele);
        if (cp) {
            kvm["way_area"] = cp->Area();
            kvm["z_order"] = (int64) cp->ZOrder();
            kvm["layer"] = cp->Layer();
        }
        id_*=-1;
    } else {
        return nullptr;
    }
    auto wkb = geom->Wkb(true,false);
    return sqlselect::make_feature(id_, kvm, geom->MinZoom(), {wkb}, other_tags);
}

bool test_minzoom(int64 minzoom, int64 testval) {
    if (minzoom < 0) { return true; }
    return testval <= minzoom;
}

sqlselect::Data_map make_datamap_filter(std::vector<PrimitiveBlockPtr> blocks, const bbox& box, int64 minzoom, bool extra_tags_included, bool split_bnd_hw) {
    sqlselect::Feature_vec points, lines, polys, boundary, highway;
    
    for (auto bl: blocks) {
        if (bl && !bl->Objects().empty() && overlaps_quadtree(box, bl->Quadtree())) {
            if (!test_minzoom(minzoom, bl->Quadtree()&31)) {
                continue;
            }
            for (auto ele: bl->Objects()) {
                if (ele->Type()>=ElementType::Point) {
                    auto geom = std::dynamic_pointer_cast<BaseGeometry>(ele);
                    
                    if (test_minzoom(minzoom, geom->MinZoom()) && overlaps(box, geom->Bounds())) {
                        auto feat = make_feature_from_geom(geom,bl->Quadtree(),extra_tags_included);
                        if (feat) {
                            if (ele->Type() == ElementType::Point) {
                                points.push_back(feat);
                            } else if (ele->Type() == ElementType::Linestring) {
                                if (split_bnd_hw && (feat->Get("z_order", false)>sqlselect::Value(0))) {
                                    highway.push_back(feat);
                                } else {
                                    lines.push_back(feat);
                                }
                            } else {
                                if (split_bnd_hw && (feat->Id()<0) && (feat->Get("boundary",false)==sqlselect::s("administrative"))) {
                                    boundary.push_back(feat);
                                } else {
                                    polys.push_back(feat);
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    sqlselect::Data_map dm;
    if (!points.empty()) {
        dm["planet_osm_point"] = points;
    }
    if (!lines.empty()) {
        dm["planet_osm_line"] = lines;
    }
    if (!polys.empty()) {
        dm["planet_osm_polygon"] = polys;
    }
    
    if (!highway.empty()) {
        dm["planet_osm_highway"] = highway;
    }
    if (!polys.empty()) {
        dm["planet_osm_boundary"] = boundary;
    }
    
    return dm;
}

/*
struct to_mapnik_value_impl {
    public:
        
        mapnik::value operator()(sqlselect::null) const {
            mapnik::value_null n;
            return n;
        }
        
        mapnik::value operator()(sqlselect::boolean b) const {
            return b.b;
        }
        
        mapnik::value operator()(const sqlselect::string_vec& v) const {
            if (v.empty()) {
                mapnik::value_null{};
            }
            
            return mapnik::value_unicode_string::fromUTF8(v.front());
        }
            
    
        mapnik::value operator()(double v) const {
            return (mapnik::value_double) v;
        }
        
        mapnik::value operator()(int64 i) const {
            return (mapnik::value_integer) i;
        }
        
        
        mapnik::value operator()(std::string s) const {
            return mapnik::value_unicode_string::fromUTF8(s);
        }
};
mapnik::value to_mapnik_value(sqlselect::Value v) {
    return boost::apply_visitor(to_mapnik_value_impl(), v);
}
*/
mapnik::feature_ptr to_mapnik(mapnik::context_ptr ctx, sqlselect::Feature::Ptr feat) {
    auto mpfeat = std::make_shared<mapnik::feature_impl>(ctx, feat->Id());
    
    try {
        std::string wkb = feat->Geometry();
        auto geom = mapnik::geometry_utils::from_wkb(wkb.c_str(), wkb.size());
        mpfeat->set_geometry_copy(geom);
    } catch (...) {
        throw std::domain_error("reading geometry wkb failed");
    }
    
    for (const auto& k : *ctx) {
        auto v = feat->Get(k.first, false);
        //auto vm = to_mapnik_value(v);
        if (!v.is_null()) {
            mpfeat->put(k.first, v);
        }
    }
    
    return mpfeat;
 
}
mapnik::feature_ptr to_mapnik_split(mapnik::context_ptr ctx, sqlselect::Feature::Ptr feat, size_t geom_idx) {
    auto mpfeat = std::make_shared<mapnik::feature_impl>(ctx, feat->Id());
    
    try {
        std::string wkb = feat->GeometryAt(geom_idx);
        auto geom = mapnik::geometry_utils::from_wkb(wkb.c_str(), wkb.size());
        mpfeat->set_geometry_copy(geom);
    } catch (...) {
        throw std::domain_error("reading geometry wkb failed");
    }
    
    for (const auto& k : *ctx) {
        auto v = feat->Get(k.first, false);
        //auto vm = to_mapnik_value(v);
        if (!v.is_null()) {
            mpfeat->put(k.first, v);
        }
    }
    
    return mpfeat;
 
}


class result_featureset : public mapnik::Featureset {
    public:
        result_featureset(sqlselect::Result::Ptr result_, mapnik::context_ptr ctx_) : result(result_), ctx(ctx_) {}
        virtual ~result_featureset() {}
        virtual mapnik::feature_ptr next() {
            auto f = result->next();
            if (!f) { return nullptr; }
            return to_mapnik(ctx, f);
        }
    private:
        sqlselect::Result::Ptr result;
        mapnik::context_ptr ctx;
};

class result_split_featureset : public mapnik::Featureset {
    public:
        result_split_featureset(sqlselect::Result::Ptr result_, mapnik::context_ptr ctx_) : result(result_), ctx(ctx_), geom_idx(0) {
            f=result->next();
        }
        virtual ~result_split_featureset() {}
        virtual mapnik::feature_ptr next() {
            
            
            while ((f) && (geom_idx >= f->NumGeometries())) {
                f=result->next();
                geom_idx=0;
            }
            if (!f) { return nullptr; }
            
            auto r = to_mapnik_split(ctx, f, geom_idx);
            geom_idx++;
            return r;
        }
    private:
        sqlselect::Result::Ptr result;
        mapnik::context_ptr ctx;
        
        sqlselect::Feature::Ptr f;
        size_t geom_idx;
};


mapnik::featureset_ptr make_result_featureset(sqlselect::Result::Ptr result, mapnik::context_ptr ctx) {
    return std::make_shared<result_featureset>(result,ctx);
}
mapnik::featureset_ptr make_result_split_featureset(sqlselect::Result::Ptr result, mapnik::context_ptr ctx) {
    return std::make_shared<result_split_featureset>(result,ctx);
}


std::vector<std::string> extract_geoms(std::vector<py::bytes> g) {
    size_t ng = g.size();
    std::vector<std::string> result;
    result.reserve(ng);
    
    for (size_t i=0; i < ng; i++) {
        result.push_back(py::cast<std::string>(g[i]));
    }
    return result;
}
    
}


PYBIND11_PLUGIN(_sqlselect) {
    
    using namespace sqlselect;
    py::module m("_sqlselect", "sqlselect");
    
    py::class_<Result, Result::Ptr>(m, "Result")
        .def("reset", &Result::reset)
        .def("next", &Result::next)
    ;
    
    py::bind_vector<Feature_vec>(m,"FeatureVec");
    py::bind_map<Data_map>(m,"DataMap");
    py::class_<Data, Data::Ptr>(m, "Data")
        .def("Call", &Data::Call)
        .def("AddResult", &Data::AddResult)
        .def_property_readonly("Contents", &Data::Contents)
    ;
    
    m.def("make_data", [](Data_map dm) { return Data::Create(std::move(dm)); });
    
  
    /*
    //py::implicitly_convertible<py::none,null>();
    //py::implicitly_convertible<bool,boolean>();
    */
    py::class_<Value>(m,"Value")
        .def("__init__", [](Value& v, py::none) {
            new (&v) Value;
            v = null_value();
        })
        .def(py::init<bool>())
        .def(py::init<int64>())
        .def(py::init<double>())
        .def("__init__", [](Value& v, std::string s) {
            new (&v) Value;
            v = mapnik::value_unicode_string::fromUTF8(s);
        })
        
        .def("__call__", &conv_val)
    ;
    m.def("null", &null_value);
    
   
    
    py::implicitly_convertible<py::none,Value>();
    py::implicitly_convertible<bool,Value>();
    py::implicitly_convertible<int64,Value>();
    py::implicitly_convertible<double,Value>();
    py::implicitly_convertible<std::string,Value>();
    
    
    
    py::bind_map<sqlselect::KeyValueMap>(m,"KeyValueMap");
    
    py::class_<Feature, Feature::Ptr>(m, "Feature")
        .def_property_readonly("Id", &Feature::Id)
        .def_property_readonly("MinZoom", &Feature::MinZoom)
        .def_property_readonly("Geometry", [](const Feature& f) { return py::bytes(f.Geometry()); })
        .def("GeometryAt", [](const Feature& f, size_t i) { return py::bytes(f.GeometryAt(i)); })
        .def("NumGeometries", &Feature::NumGeometries)
        .def_property_readonly("Keys", &Feature::Keys)
        .def_property_readonly("OtherTags", [](const Feature& f) { return py::bytes(f.OtherTags()); })
        .def("__getitem__", [](const Feature& f, const std::string& k) { return f.Get(k,false); })
    ;
    
    m.def("make_feature", [](int64 i, KeyValueMap p, int64 m,std::vector<py::bytes> g) { return make_feature(i,p,m,xxx::extract_geoms(g)); });
    m.def("make_feature_modified", &make_feature_modified);
    
    py::class_<Table, Table::Ptr>(m, "Table")
        .def("__call__", &Table::Call)
        .def("__str__", &Table::Str)
        .def_property_readonly("type", [](const Table& c) { return "Table"; })
    ;
    
    py::class_<Column, Column::Ptr>(m, "Col")
        .def("__call__", &Column::Call)
        .def("__str__", &Column::Str)
        .def_property_readonly("type", [](const Column& c) { return "Column"; })
        .def_property_readonly("Key", &Column::Key)
    ;
    
    
        
    py::class_<ColumnList, ColumnList::Ptr>(m, "ColumnList")
        .def("__call__", &ColumnList::Call)
        .def("__str__", &ColumnList::Str)
        .def_property_readonly("type", [](const ColumnList& c) { return "ColumnList"; })
        .def(py::init<std::vector<Column::Ptr>,bool>())
        .def_property_readonly("Columns", &ColumnList::Cols)
        .def_property_readonly("Star", &ColumnList::HasStar)
    ;
    
    py::class_<Where, Where::Ptr>(m, "Where")
        .def("__call__", &Where::Call)
        .def("__str__", &Where::Str)
        .def_property_readonly("type", [](const Where& c) { return "Where"; })

    ;
    
    py::class_<OrderItem>(m,"OrderItem")
        .def(py::init<Column::Ptr,bool>())
        .def("__str__", &OrderItem::Str)
        .def_readonly("Column", &OrderItem::column)
        .def_readonly("IsAsc", &OrderItem::asc)
    ;
    
    py::class_<OrderBy, OrderBy::Ptr>(m, "OrderBy")
        .def(py::init<OrderItemVec>())
        .def("__call__", &OrderBy::Call)
        .def("__str__", &OrderBy::Str)
        .def_property_readonly("type", [](const OrderBy& c) { return "OrderBy"; })
        .def_property_readonly("Spec", &OrderBy::GetSpec)
    ;
    
    
    py::class_<Select, Select::Ptr, Table>(m, "Select")
        .def(py::init<ColumnList::Ptr,Table::Ptr,Where::Ptr,OrderBy::Ptr,std::string>())
        .def_property_readonly("Columns", &Select::GetColumns)
        .def_property_readonly("Where", &Select::GetWhere)        
        .def_property_readonly("Table", &Select::GetTable)
        .def_property_readonly("OrderBy", &Select::GetOrderBy)
        .def_property("Name", &Select::GetName, &Select::SetName)
    ;
    py::class_<PickTable,PickTable::Ptr, Table>(m, "PickTable")
        .def(py::init<const std::string&>())
        .def_property_readonly("TableKey", &PickTable::GetTableKey)
    ;
    
    py::class_<Union,Union::Ptr, Table>(m, "Union")
        .def(py::init<Table::Ptr,Table::Ptr,const std::string&>())
        .def_property_readonly("Left", &Union::GetLeft)
        .def_property_readonly("Right", &Union::GetRight)
        .def_property("Name", &Union::GetName, &Union::SetName)
    ;
    
    py::class_<ValuesTable,ValuesTable::Ptr, Table>(m,"ValuesTable")
        .def(py::init<const Feature_vec&, const std::string&>())
        .def_property_readonly("Features", &ValuesTable::GetFeatures)
        .def_property("Name", &ValuesTable::GetName, &ValuesTable::SetName)
    ;
    
    py::class_<Join,Join::Ptr, Table>(m, "Join")
        .def(py::init<Table::Ptr,Table::Ptr,const std::vector<Column::Ptr>&,const std::string&>())
        .def_property_readonly("Left", &Join::GetLeft)
        .def_property_readonly("Right", &Join::GetRight)
        .def_property_readonly("Columns", &Join::GetColumns)
        .def_property("Name", &Join::GetName, &Join::SetName)
    ;
    py::class_<Label,Label::Ptr, Column>(m, "Label")
        .def(py::init<std::string>())
    ;
    py::class_<AsLabel,AsLabel::Ptr, Column>(m, "AsLabel")
        .def(py::init<std::string,Column::Ptr>())
        .def_property_readonly("Parent", &AsLabel::Parent)
    ;
    
    py::class_<Val,Val::Ptr, Column>(m, "Val")
        .def(py::init<Value>())
        .def_property_readonly("StoredValue", &Val::StoredValue)
    ;
    
    py::class_<Coalesce,Coalesce::Ptr, Column>(m,"Coalesce")
        .def(py::init<std::vector<Column::Ptr>>())
        .def_property_readonly("Columns", &Coalesce::GetColumns)
    ;
    
    py::class_<Concat,Concat::Ptr, Column>(m,"Concat")
        .def(py::init<std::vector<Column::Ptr>,bool,std::string>())
        .def_property_readonly("Columns", &Concat::GetColumns)
        .def_property_readonly("AllowNulls", &Concat::AllowNulls)
        .def_property_readonly("Seperator", &Concat::GetSeperator)
    ;
    
    py::class_<Clause>(m,"Clause")
        .def(py::init<Where::Ptr,Column::Ptr>())
        .def_readonly("where", &Clause::where)
        .def_readonly("column", &Clause::column)
        .def("__str__", &Clause::Str)
    ;
    
    py::class_<Case,Case::Ptr, Column>(m,"Case")
        .def(py::init<Clause_Vec>())
        .def_property_readonly("Clauses", &Case::GetClauses)
    ;
    
    py::class_<ColumnOp,ColumnOp::Ptr, Column>(m,"ColumnOp")
        .def(py::init<Column::Ptr,arithmitic_type,Column::Ptr>())
        .def_property_readonly("Left", &ColumnOp::GetLeft)
        .def_property_readonly("Op", &ColumnOp::GetOp)
        .def_property_readonly("Right", &ColumnOp::GetRight)
    ;
    
    py::class_<ColumnAccess, ColumnAccess::Ptr, Column>(m, "ColumnAccess")
        .def(py::init<Column::Ptr,std::string>())
        .def_property_readonly("Src", &ColumnAccess::GetSrc)
        .def_property_readonly("Key", &ColumnAccess::GetKey)
        
    ;
    
    
    py::class_<PixelArea,PixelArea::Ptr, Column>(m,"PixelArea")
        .def(py::init<>())
    ;
    
    py::class_<NullIf,NullIf::Ptr, Column>(m,"NullIf")
        .def(py::init<Column::Ptr,Column::Ptr>())
        .def_property_readonly("Left", &NullIf::GetLeft)
        .def_property_readonly("Right", &NullIf::GetRight)
    ;
    
    py::class_<CharLength,CharLength::Ptr, Column>(m,"CharLength")
        .def(py::init<Column::Ptr>())
        .def_property_readonly("Column", &CharLength::GetColumn)
        
    ;
    
    py::class_<AsInteger,AsInteger::Ptr, Column>(m,"AsInteger")
        .def(py::init<Column::Ptr>())
        .def_property_readonly("Column", &AsInteger::GetColumn)
        
    ;
    py::class_<SubStr,SubStr::Ptr, Column>(m,"SubStr")
        .def(py::init<Column::Ptr,Column::Ptr,Column::Ptr>())
        .def_property_readonly("Column", &SubStr::GetColumn)
        .def_property_readonly("From", &SubStr::GetFrom)
        .def_property_readonly("To", &SubStr::GetTo)
        
    ;
    
    py::class_<AsString,AsString::Ptr, Column>(m,"AsString")
        .def(py::init<Column::Ptr>())
        .def_property_readonly("Column", &AsString::GetColumn)
        
    ;
    
    py::class_<FeatId,FeatId::Ptr, Column>(m, "FeatId")
        .def(py::init<std::string>())
    ;
    
    py::class_<AsFloat,AsFloat::Ptr, Column>(m,"AsFloat")
        .def(py::init<Column::Ptr>())
        .def_property_readonly("Column", &AsFloat::GetColumn)
        
    ;
    
    py::class_<StringToArray,StringToArray::Ptr, Column>(m,"StringToArray")
        .def(py::init<Column::Ptr,std::string>())
        .def_property_readonly("Column", &StringToArray::GetColumn)
        .def_property_readonly("Split", &StringToArray::GetSplit)
        
    ;
    
    
    py::class_<ArrayToString,ArrayToString::Ptr, Column>(m,"ArrayToString")
        .def(py::init<Column::Ptr,std::string>())
        .def_property_readonly("Column", &ArrayToString::GetColumn)
        .def_property_readonly("Join", &ArrayToString::GetJoin)
        
    ;
    
    py::class_<MaxEntryLength,MaxEntryLength::Ptr, Column>(m,"MaxEntryLength")
        .def(py::init<Column::Ptr>())
        .def_property_readonly("Column", &MaxEntryLength::GetColumn)

    ;
    
    py::class_<ArrayLength,ArrayLength::Ptr, Column>(m,"ArrayLength")
        .def(py::init<Column::Ptr>())
        .def_property_readonly("Column", &ArrayLength::GetColumn)

    ;
    
    py::class_<And,And::Ptr, Where>(m, "And")
        .def(py::init<Where::Ptr,Where::Ptr>())
        .def_property_readonly("Left", &And::GetLeft)
        .def_property_readonly("Right", &And::GetRight)
    ;
    py::class_<Or,Or::Ptr, Where>(m, "Or")
        .def(py::init<Where::Ptr,Where::Ptr>())
        .def_property_readonly("Left", &Or::GetLeft)
        .def_property_readonly("Right", &Or::GetRight)
    ;
    
    py::class_<Not,Not::Ptr, Where>(m, "Not")
        .def(py::init<Where::Ptr>())
        .def_property_readonly("Where", &Not::GetWhere)
    ;   
    
    py::class_<Op,Op::Ptr, Where>(m, "Op")
        .def(py::init<Column::Ptr,comparison_type,Column::Ptr>())
        .def_property_readonly("Left", &Op::GetLeft)
        .def_property_readonly("Op", &Op::GetOp)
        .def_property_readonly("Right", &Op::GetRight)
    ;  
    py::class_<In,In::Ptr, Where>(m, "In")
        .def(py::init<Column::Ptr,const std::vector<Column::Ptr>&>())
        .def_property_readonly("Left", &In::GetLeft)
        .def_property_readonly("Right", &In::GetRight)
    ;  
    py::class_<IsNull,IsNull::Ptr, Where>(m, "IsNull")
        .def(py::init<Column::Ptr>())
        .def_property_readonly("Val", &IsNull::GetVal)
    ; 
    
    py::class_<RegEx,RegEx::Ptr, Where>(m, "RegEx")
        .def(py::init<Column::Ptr,const std::string&>())
        .def_property_readonly("Column", &RegEx::GetColumn)
        .def_property_readonly("Match", &RegEx::GetMatch)
    ; 
    
    
    m.def("example", &example);
    
    py::enum_<comparison_type>(m, "comparison_type")
        .value("EQ",comparison_type::EQ)
        .value("NE",comparison_type::NE)
        .value("GT",comparison_type::GT)
        .value("LT",comparison_type::LT)
        .value("GE",comparison_type::GE)
        .value("LE",comparison_type::LE)
        .export_values();
    
    py::enum_<arithmitic_type>(m, "arithmitic_type")
        .value("ADD",arithmitic_type::ADD)
        .value("SUBTRACT",arithmitic_type::SUBTRACT)
        .value("MULTIPLY",arithmitic_type::MULTIPLY)
        .value("DIVIDE",arithmitic_type::DIVIDE)
        .value("MODULO", arithmitic_type::MODULO)
        .export_values();
    
    
    m.def("make_feature_from_geom", &xxx::make_feature_from_geom);
    m.def("make_datamap_filter", &xxx::make_datamap_filter, py::arg("blocks"), py::arg("box"), py::arg("minzoom"), py::arg("extra_tags_included")=true, py::arg("split_bnd_hw")=true);
    m.def("to_mapnik", &xxx::to_mapnik);
    m.def("make_result_featureset", &xxx::make_result_featureset);
    m.def("make_result_split_featureset", &xxx::make_result_split_featureset);
    

    export_geojson(m);
    export_mvt(m);
    
    return m.ptr();
}

