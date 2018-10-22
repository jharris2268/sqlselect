import _sqlselect
from _sqlselect import *
import sqlparse
import oqt
import shapely.wkb as swk, shapely.geometry as sg


def featurevec_repr(fv):
    return "FeatureVec(%d entries)" % len(fv)
FeatureVec.__repr__ = featurevec_repr
FeatureVec.__str__ = featurevec_repr

def feature_type(f):
    if not f.Geometry: return 'Null'
    
    gi=4 if f.Geometry[0]=='\0' else 1
    g=f.Geometry[gi]
    if g=='\1': return 'Point'
    if g=='\2': return 'LineString'
    if g=='\3': return 'Polygon'
    if g=='\4': return 'MultiPoint'
    if g=='\5': return 'MultiLineString'
    if g=='\6': return 'MultiPolygon'
    if g=='\7': return 'GeometryCollection'
    return 'Unknown'

Feature.Type = property(feature_type)

def feature_str(f):
    return "%s %d [%d keys, minzoom %d]" % (f.Type, f.Id, len(f.Keys), f.MinZoom)

Feature.__str__ = feature_str
Feature.__repr__ = feature_str

def feat_json(feat, expandgeom=False):
    
    
    res = {'id': feat.Id, 'minzoom': feat.MinZoom, 'properties':  {}}
    if expandgeom:
        g=swk.loads(feat.Geometry)
        res['geometry'] = sg.mapping(g)
    else:
        res['geom'] = feat.Geometry
    for k in feat.Keys:
        res['properties'][k] = feat[k]()
    return res

Feature.shapely = property(lambda f: swk.loads(f.Geometry))
Feature.json = property(feat_json)
Feature.geojson = property(lambda f: feat_json(f,True))

def iter_result(result):
    result.reset()
    f = result.next()
    while f:
        yield f
        f=result.next()

Result.__iter__ = iter_result


def make_feat_old(blq, o):
    kvm = KeyValueMap()
    for tg in o.Tags:
        kvm[tg.key]=tg.val
    kvm['quadtree']=o.Quadtree
    kvm['block_quadtree']=blq
    ii = o.Id
    
    if o.Type==oqt._oqt.Linestring:
        kvm['z_order'] = o.ZOrder
        kvm['length'] = o.Length
    elif o.Type==oqt._oqt.SimplePolygon:
        kvm['z_order'] = o.ZOrder
        kvm['way_area'] = o.Area
        
    elif o.Type==oqt._oqt.ComplicatedPolygon:
        kvm['z_order'] = o.ZOrder
        kvm['way_area'] = o.Area
        ii *= -1
    
    return make_feature(ii, kvm, o.MinZoom, o.Wkb(True,False))

def make_datamap(tiles, box=None, mz=None):
    if box==None:
        box=oqt._oqt.bbox(-1800000000,-900000000,1800000000,900000000)
    
    dm = make_datamap_filter(tiles,box,-1 if mz is None else mz,True)
    
    return make_data(dm)

def make_datamap_old(tiles,box):
    pts, lns,pys = FeatureVec(),FeatureVec(), FeatureVec()
    for bl in tiles:
        if box and not box.overlaps_quadtree(bl.quadtree):
            continue
        for o in bl:
            if box and not box.overlaps(o.Bounds):
                continue
            f = make_feat(bl.quadtree,o)
            if o.Type==oqt._oqt.Point:
                pts.append(f)
            elif o.Type==oqt._oqt.Linestring:
                lns.append(f)
            else:
                pys.append(f)
    dm = DataMap()
    dm['planet_osm_point']=pts
    dm['planet_osm_line']=lns
    dm['planet_osm_polygon']=pys
    
    return dm



def iter_keys(pp):
    if not type(pp).__module__=='sqlselect._sqlselect':
        return 
    if pp.__qualname__ in ('arithmitic_type','comparison_type'):
        return
    if pp.__qualname__ == 'Value':
        return
    
    if hasattr(pp,'Key'):
        if pp.Key:
            yield pp.Key
                
    for d in dir(pp):
        if d[0]=='_' or d=='type': continue
        a=getattr(pp, d)
        if not d: continue
        if type(a)==list:
            for b in a:
                for k in iter_keys(b):
                    yield k
            
        else:
            for k in iter_keys(a):
                yield k
    
    

def check_order_fields(pp, res):
    if not res['OrderBy']:
        return
    
    if res['Columns']['Star']==True:
        return
        
    order_keys = set(iter_keys(pp.OrderBy))
    column_keys = [p.Key for p in pp.Columns.Columns]
    
    extra_keys = order_keys.difference(column_keys)
    for k in extra_keys:
        res['Columns']['Columns'].append(as_json(Label(k)))
    

def as_json(pp):
    if not type(pp).__module__=='sqlselect._sqlselect':
        return pp
    if pp.__qualname__ in ('arithmitic_type','comparison_type'):
        return str(pp).split(".")[1]
    if pp.__qualname__ == 'Value':
        return pp()
    res = {'type': pp.__qualname__}
    for d in dir(pp):
        if d[0]=='_' or d=='type': continue
        a=getattr(pp, d)
        if not d: continue
        if type(a)==list:
            res[d] = map(as_json, a)
        else:
            res[d]=as_json(a)
    
    if res['type']=='Select':
        check_order_fields(pp, res)
    
    return res

def is_selectstar(select):
    
    if not isinstance(select, Select):
        return False
    
    if select.Columns.Columns: return False
    if not select.Columns.Star: return False
    
    if select.Where: return False
    if select.OrderBy: return False
    
    return True

def proc(qu, filterss=True):
    if len(qu.split())==1:
        return PickTable(qu)
    
    res = sqlparse.proc(qu)
    if filterss and is_selectstar(res):
        return res.Table
    
    return res
