#-----------------------------------------------------------------------
#
# This file is part of sqlselect
#
# Copyright (C) 2018 James Harris
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public
# License as published by the Free Software Foundation; either
# version 3 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# General Public License for more details.
#
# You should have received a copy of the GNU General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
#
#-----------------------------------------------------------------------

from __future__ import print_function
from sqlparse import proc
import _sqlselect as sq
import pymapnik11 as mk
import oqt
import math
import oqtmvt as om

import shapely.geometry as sg, shapely.ops as sop

views_xx = {'planet_osm_roads': proc("""
SELECT osm_id,null as part,tile,quadtree,name,ref,admin_level,highway,railway,boundary,
            service,tunnel,bridge,z_order,covered,surface, way
        FROM planet_osm_line
        WHERE highway in (
            'secondary','secondary_link','primary','primary_link',
            'trunk','trunk_link','motorway','motorway_link')
        OR railway is not null or boundary = 'administrative'

    UNION SELECT osm_id,part,tile,quadtree,name,null as ref, admin_level,null as highway,
            null as railway, boundary, null as service,
            null as tunnel,null as bridge, 0  as z_order,null as covered,null as surface,
            way as way
        FROM planet_osm_polygon WHERE
            osm_id<0 and boundary='administrative'
"""),
'planet_osm_highways': proc("""
SELECT
    osm_id,length,quadtree,tile,minzoom,
    way,
    (CASE WHEN substr(feature, length(feature)-3, 4) = 'link' THEN substr(feature, 0, length(feature)-4) ELSE feature END) AS feature,
    horse,
    foot,
    bicycle,
    tracktype,
    int_surface,
    access,
    construction,
    service,
    link,
    layernotnull,
    name,ref,bridge,tunnel,covered,prio
  FROM (
    SELECT
        osm_id,length,quadtree,tile,minzoom,
        way,
        ('highway_' || highway) AS feature,
        horse,
        foot,
        bicycle,
        tracktype,
        CASE WHEN surface IN ('unpaved', 'compacted', 'dirt', 'earth', 'fine_gravel', 'grass', 'grass_paver', 'gravel', 'ground',
                              'mud', 'pebblestone', 'salt', 'sand', 'woodchips', 'clay') THEN 'unpaved'
          WHEN surface IN ('paved', 'asphalt', 'cobblestone', 'cobblestone:flattened', 'sett', 'concrete', 'concrete:lanes',
                              'concrete:plates', 'paving_stones', 'metal', 'wood') THEN 'paved'
          ELSE NULL
        END AS int_surface,
        CASE WHEN access IN ('destination') THEN 'destination'::text
          WHEN access IN ('no', 'private') THEN 'no'::text
          ELSE NULL
        END AS access,
        construction,
        CASE
          WHEN service IN ('parking_aisle', 'drive-through', 'driveway') THEN 'INT-minor'::text
          ELSE 'INT-normal'::text
        END AS service,
        CASE
          WHEN substr(highway, length(highway)-3, 4) = 'link' THEN 'yes'
          ELSE 'no'
        END AS link,
        CASE WHEN layer~E'^-?\\d+$' AND length(layer)<10 THEN layer::integer ELSE 0 END AS layernotnull,
        name,ref,bridge,tunnel,covered
      FROM planet_osm_line
      WHERE highway IS NOT NULL
    UNION ALL
    SELECT
        osm_id,length,quadtree,tile,minzoom,
        way,
        COALESCE(
          ('railway_' || (CASE WHEN railway = 'preserved' AND service IN ('spur', 'siding', 'yard') THEN 'INT-preserved-ssy'::text 
                               WHEN (railway = 'rail' AND service IN ('spur', 'siding', 'yard')) THEN 'INT-spur-siding-yard'  
                               WHEN (railway = 'tram' AND service IN ('spur', 'siding', 'yard')) THEN 'tram-service' ELSE railway END)),
          ('aeroway_' || aeroway)
        ) AS feature,
        horse,
        foot,
        bicycle,
        tracktype,
        'null' as int_surface,
        CASE
          WHEN access IN ('destination') THEN 'destination'::text
          WHEN access IN ('no', 'private') THEN 'no'::text
          ELSE NULL
        END AS access,
        construction,
        CASE WHEN service IN ('parking_aisle', 'drive-through', 'driveway') THEN 'INT-minor'::text ELSE 'INT-normal'::text END AS service,
        'no' AS link,
        CASE WHEN layer~E'^-?\\d+$' AND length(layer)<10 THEN layer::integer ELSE 0 END AS layernotnull,
        name,ref,bridge,tunnel,covered
      FROM planet_osm_line
      WHERE (railway IS NOT NULL OR aeroway IS NOT NULL)
    ) AS features
  JOIN (VALUES
      ('railway_rail', 440),
      ('railway_INT-preserved-ssy', 430),
      ('railway_INT-spur-siding-yard', 430),
      ('railway_subway', 420),
      ('railway_narrow_gauge', 420),
      ('railway_light_rail', 420),
      ('railway_preserved', 420),
      ('railway_funicular', 420),
      ('railway_monorail', 420),
      ('railway_miniature', 420),
      ('railway_turntable', 420),
      ('railway_tram', 410),
      ('railway_tram-service', 405),
      ('railway_disused', 400),
      ('railway_construction', 400),
      ('highway_motorway', 380),
      ('highway_trunk', 370),
      ('highway_primary', 360),
      ('highway_secondary', 350),
      ('highway_tertiary', 340),
      ('highway_residential', 330),
      ('highway_unclassified', 330),
      ('highway_road', 330),
      ('highway_living_street', 320),
      ('highway_pedestrian', 310),
      ('highway_raceway', 300),
      ('highway_motorway_link', 240),
      ('highway_trunk_link', 230),
      ('highway_primary_link', 220),
      ('highway_secondary_link', 210),
      ('highway_tertiary_link', 200),
      ('highway_service', 150),
      ('highway_track', 110),
      ('highway_path', 100),
      ('highway_footway', 100),
      ('highway_bridleway', 100),
      ('highway_cycleway', 100),
      ('highway_steps', 100),
      ('highway_platform', 90),
      ('railway_platform', 90),
      ('aeroway_runway', 60),
      ('aeroway_taxiway', 50),
      ('highway_construction', 10)
    ) AS ordertable (feature, prio)
    USING (feature)
  ORDER BY
    layernotnull,
    prio,
    CASE WHEN access IN ('no', 'private') THEN 0 WHEN access IN ('destination') THEN 1 ELSE 2 END,
    CASE WHEN int_surface IN ('unpaved') THEN 0 ELSE 2 END
"""),
}
            
views = {
    'planet_osm_roads': views_xx['planet_osm_roads'],
    'planet_osm_turningcircles': proc("""select osm_id, highway, tags, way from planet_osm_point where highway in ('turning_circle', 'turning_loop')"""),
    #'planet_osm_highway': proc("select * from planet_osm_line where z_order > 0"),
    #'planet_osm_boundary': proc("select osm_id,name,boundary,admin_level,way from planet_osm_polygon where boundary='administrative'"),
    'planet_osm_building': proc("select osm_id,way,building,aeroway,amenity,aerialway,tags,way_area from planet_osm_polygon where building is not null and building != 'no'"),
}

class Tiles:
    def __init__(self, tiles, bounds):
        self.tiles=tiles
        self.bounds=bounds
        self.cache={}
    
    def __call__(self, bounds, zoom):
    
        kk = (bounds,zoom)
        if kk in self.cache and self.cache[kk][1] is not None:
            return self.cache[kk][1]
        
        box_oqt = oqt._oqt.bbox(*map(oqt.intm, mk.backward(bounds)))
        
        a,dm = mk.utils.time_op(sq.make_datamap_filter, self.tiles, box_oqt, zoom, True)
        dd = sq.make_data(dm)
        for k,v in views.iteritems():
            dd.AddResult(k, v(dd, zoom))
            
        
        self.cache[kk]=[a,dd]
        return self.cache[kk][1]

def prep_geoms(json_data, reproj=None):
    result={}
    for tab, fc in json_data.iteritems():
        result[tab]=[]
        for feat in fc['features']:
            geom=sg.shape(feat['geometry'])
            if geom.is_empty: continue
            props=sq.KeyValueMap()
            mz=-1
            for k,v in feat['properties'].iteritems():
                if k=='min_zoom':
                    mz=v
                else:
                    props[str(k)] = sq.Value(v)
            props['tags']=sq.Value(False)
            if reproj:
                geom=sop.transform(reproj, geom)
                
            result[tab].append((sq.make_feature(feat['id'], props, mz, geom.wkb), mk.box2d(*geom.bounds)))
    
    return result
            
            
def overlaps(A,B):
    if A[2]>B[0]: return False
    if A[3]>B[1]: return False
    if B[2]>A[0]: return False
    if B[3]>A[1]: return False
    return True

class ReprojectFromTile:
    def __init__(self, x, y, z):
        #np = (1<<16) if z==14 else (1<<10)
        np=256
        nn=(1<<z)
        oo = nn/2
        self.xx = (x-oo)*np
        self.yy = (oo-y-1)*np #y coords from bottom of tile
        self.sc = mk.earth_width / nn / np
    def __call__(self, x, y):
        return (x+self.xx)*self.sc, (y+self.yy)*self.sc



def add_to_merged(merged, feats, bounds, zoom):
                
    for tab,ff in feats.items():
        if not tab in merged:
            merged[tab]=sq.FeatureVec()
        if False:
            for f in ff:
                if f.MinZoom <= zoom:
                    merged[tab].append(f)
        else:
            for f in ff:
                merged[tab].append(f)


def tile_area(mx,my,Mx,My,xi,yi):
    a = max(xi,mx)
    b = max(yi,my)
    c = min(xi+1,Mx)
    d = min(yi+1,My)
    return (c-a)*(d-b)

def iter_tile_range(mx,my,Mx,My, minp=0):
    for xi in xrange(int(mx), int(Mx+1)):
        for yi in xrange(int(my), int(My+1)):
            if minp>0 and tile_area(mx,my,Mx,My,xi,yi)<minp:
                continue
            yield xi, yi

class TiledDataCommon:
    def __init__(self, maxzoom, bounds, singletile,msgs, tabprfx, check_cache,mintilearea=0):
        self.maxzoom=maxzoom
        
        self.bounds=bounds
        if self.bounds is None:
            self.bounds = mk.box2d(-mk.earth_width, -mk.earth_width, mk.earth_width, mk.earth_width)
        self.last=None
        self.tabprfx=tabprfx
        self.singletile=singletile
        self.msgs=msgs
        self.mintilearea=mintilearea
        if check_cache:
            self.cached={}
        else:
            self.cached=None
        self.cnt=-1
    
    def find_tile_range_single(self, bounds, tile_zoom):
        cx,cy=bounds.center()
        x,y = mk.coord_to_tile(cx, cy, tile_zoom)
        mx,my = int(x), int(y)
        
        if self.msgs:
            a=mk.tile_bound(mx,my,tile_zoom)
            b=a.intersection(bounds)
            print('tile %d %d %d %0.1f // %0.1f [%5.1f%%]' % (mx,my,tile_zoom, a.area,bounds.area, 100.*b.area/bounds.area))
        
        return (mx,my)
    
    def find_tile_range(self, bounds, tile_zoom):
        if self.singletile:
            yield self.find_tile_range_single(bounds, tile_zoom)
                
            return
        
        mx,my = mk.coord_to_tile(bounds.minx, bounds.maxy, tile_zoom)
        Mx,My = mk.coord_to_tile(bounds.maxx, bounds.miny, tile_zoom)
        if self.msgs: 
            print('tile range zoom %d: %0.1f %0.1f => %0.1f %0.1f' % (tile_zoom, mx,my,Mx,My))
        
        for xi,yi in iter_tile_range(mx,my,Mx,My,self.mintilearea):
            yield xi, yi
    
    
    def check_cache(self, x, y, z):
        if self.cached is None:
            return self.load_tile(x,y,z)
        
        if not (x,y,z) in self.cached:
            self.cached[x,y,z]=[self.load_tile(x,y,z),self.cnt]
        
        r=self.cached[x,y,z]
        r[1]=self.cnt
        return r[0]
    
    def __call__(self, bounds, zoom):
        self.cnt+=1
        
        if self.last and self.last[0]==bounds and self.last[1]==zoom:
            return self.last[2]
        
        tile_zoom = self.maxzoom if zoom>self.maxzoom else zoom
        merged={}
        
        for xi,yi in self.find_tile_range(bounds, tile_zoom):
            feats=self.check_cache(xi,yi,tile_zoom)
            if feats is None:
                continue
            add_to_merged(merged, feats, bounds, zoom)
                
                
        if self.msgs:
            print(", ".join("%s: %d" % (k,len(v)) for k,v in merged.iteritems()))
        dm=sq.DataMap()
        for k,v in merged.iteritems():
            dm[self.tabprfx+k]=v
        res= sq.make_data(dm)
        self.last=[bounds,zoom,res]
        return res
class MvtTiles(TiledDataCommon):
    def __init__(self, mbt, maxzoom, bounds, tabprfx='planet_osm_', singletile=False, use_om=False):
        TiledDataCommon.__init__(self, maxzoom, bounds,singletile,False, tabprfx, True)
        
        self.mbt=mbt
        self.use_om=use_om
        
    
    def load_tile(self, x, y, z):
        ss = self.mbt(x,y,z)
        if ss is None:
            return None
        
        
        if self.use_om:
            ff = om.read_mvttile(str(ss),x,y,z,True)
            rr = {}
            for s,t in ff.iteritems():
                rr[s]=[]
                for a,b,c in t:
                    pp=sq.KeyValueMap()
                    mz=-1
                    for k,v in b.iteritems():
                        if k=='minzoom':
                            mz=v
                        else:
                            pp[str(k)]=sq.Value(v)
                    rr[s].append(sq.make_feature(a,pp,mz,c.wkb))
            return rr
        else:
            try:
                return sq.read_mvt_tile(str(ss), x, y, z, True)
            except Exception as ex:
                pass
                return None
                
                
    

class GeoJsonTiles(TiledDataCommon):
    def __init__(self, gettile, maxzoom, bounds, singletile=False, msgs=False, tabprfx='planet_osm_', reproject=False):
        TiledDataCommon.__init__(self, maxzoom, bounds, singletile,msgs, tabprfx, True)
        
        self.gettile=gettile
        self.singletile=singletile
        self.msgs=msgs
        
        self.reproject=reproject

    def load_tile(self, xi,yi,tile_zoom):
               
        
        try:
            pps=self.gettile(xi,yi,tile_zoom)
            
            np=0 if not self.reproject else 256
            return sq.read_geojson_tile(pps,xi,yi,tile_zoom,np)
            
        except Exception as ex:
            print(ex)
        
        return None

    


class AltDs:
    def __init__(self, tiles, query, idx=None, msgs=False, split_multigeoms=False):
        self.envelope = tiles.bounds
        self.geom_type = mk.datasource_geometry_t.Unknown
        self.query = query if hasattr(query,'type') and query.type=='Table' else proc(query)
        self.tiles = tiles
        self.idx=idx
        self.msgs=msgs
        self.times=[]
        self.split_multigeoms=split_multigeoms
    
    def __call__(self, query):
        try:
            minzoom = int(math.ceil(mk.utils.find_zoom(1/query.resolution[0])))
            a,tables = mk.utils.time_op(self.tiles, query.bbox, minzoom)
            b,result = mk.utils.time_op(self.query, tables, minzoom)
            ctx=mk.context()
            for pn in query.property_names: ctx.push(pn)
            
            
            c,res = mk.utils.time_op((sq.make_result_split_featureset if self.split_multigeoms else sq.make_result_featureset),result,ctx)
            self.times.append((query,minzoom,a,b,c))
            if self.msgs and self.idx is not None:
                print("layer %d: %0.3fs, %0.3fs, %0.3fs" % (self.idx, a, b, c))
            
            if not result:
                return mk.make_python_featureset([])
            return res
        except Exception as ex:
            print(ex)
            return mk.make_python_featureset([])
    
    
    def set_to_layer(self, mp, i):
        mp.layers[i].datasource = mk.make_python_datasource({'type':'python', 'query': str(self.query)}, self.envelope, self.geom_type, self)
    
    
def convert_postgis_to_altds(mp, tiles, tabpp=None,**kwargs):
    
    dses={}
    
    roads_split=False
    if 'roads_split' in kwargs:
        roads_split = kwargs['roads_split']
        kwargs.pop('roads_split')
    
    for i,l in enumerate(mp.layers):
        if 'type' in l.datasource.params() and l.datasource.params()['type']=='postgis':
            try:
                tt=l.datasource.params()['table']
                if tabpp is not None:
                    tt=tt.replace(tabpp,'planet_osm')
                
                kw = kwargs.copy()
                if roads_split and 'roads' in l.name:
                    kw['split_multigeoms']=True
                dses[i] = AltDs(tiles, tt,idx=i,**kw)
            except Exception as ex:
                print('layer %d failed: %s' % (i,ex))
    
    for k,v in dses.iteritems():
        v.set_to_layer(mp, k)
    
    return dses


