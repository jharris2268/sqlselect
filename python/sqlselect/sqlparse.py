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

from __future__ import unicode_literals, print_function
from arpeggio import *
from arpeggio import RegExMatch as _
import collections, json
import _sqlselect as sq

def get_comparision_op(opf):
    if opf == '=': return sq.comparison_type.EQ
    if opf == '!=': return sq.comparison_type.NE
    if opf == '>': return sq.comparison_type.GT
    if opf == '<': return sq.comparison_type.LT
    if opf == '>=': return sq.comparison_type.GE
    if opf == '<=': return sq.comparison_type.LE
    raise Exception("unexpected comparison_op "+opf)

def get_arithmitic_op(opf):
    if opf == '+': return sq.arithmitic_type.ADD
    if opf == '-': return sq.arithmitic_type.SUBTRACT
    if opf == '*': return sq.arithmitic_type.MULTIPLY
    if opf == '/': return sq.arithmitic_type.DIVIDE
    if opf == '%': return sq.arithmitic_type.MODULO
    raise Exception("unexpected arithmitic_op "+opf)

class IKwd(StrMatch):
    """
    A specialization of StrMatch to specify keywords of the language.
    """
    def __init__(self, to_match):
        super(IKwd, self).__init__(to_match,ignore_case=True)
        self.to_match = to_match
        self.root = True
        self.rule_name = 'keyword'

def number():       return _(r'\-?(\d+\.\d*|\d+)')
def strliteral():   return _(r'[eE]?\'(?:[^\']|\'\')*\''),Optional("::text")
def unq_label():    return _(r'[a-z|A-Z][a-z|A-Z|0-9|_]*')
def qq_label(): return '"',_(r'[a-z|A-Z][a-z|A-Z|0-9|_|:]*'),'"'
def label():        return Optional([StrMatch("p."),StrMatch("l.")]),[unq_label,qq_label]#,Optional("::",unq_label)

def function():     return unq_label,"(",Optional(valuelist),")"


def calculation():  return map(StrMatch,['->>','->','||','*','+','-','/','%',])
def operation():    return map(StrMatch,['==','<>','<=','>=','<','>','=','!=','~','?','@>'])



def case_value():    return IKwd("case"), OneOrMore(case_when), Optional(case_else), IKwd("end")
def case_when():    return IKwd("when"), wherefield, IKwd("then"), value
def case_weird_when():    return IKwd("when"), value, IKwd("then"), value
def case_else():    return IKwd("else"), value
def case_weird_else():    return IKwd("else"), value
def case_weird():   return IKwd("case"), label, OneOrMore(case_weird_when), Optional(case_weird_else), IKwd("end")

def null():         return IKwd("null")
def literal_value(): return [number,strliteral]
def value():        return ([literal_value,case_value,case_weird,function,null,label,("(",value,")")],Optional(calculation,value)),Optional([StrMatch("::"),],unq_label)
def valuelist():    return value,ZeroOrMore(",",value)

def asfield():      return value, IKwd("as"), [unq_label,qq_label]
def field():        return [asfield,value,label]

def allfield():     return StrMatch("*")
def fieldlist():    return (field, ZeroOrMore(",", field))


def table():        return [(unq_label,Optional(["p","l"])),("(",select,")", IKwd("as"), label), valuestable],Optional(IKwd("join"), table, IKwd("using"), labellist_brackets)
def where():        return IKwd("where"), wherefield
def labellist_brackets():   return "(", unq_label, ZeroOrMore(",", unq_label), ")"
def simple_valuelist_brackets():   return "(", [number,strliteral], ZeroOrMore(",",[number,strliteral]),")"
def valuestable():  return "(", IKwd("values"), simple_valuelist_brackets, ZeroOrMore(",",simple_valuelist_brackets), ")", IKwd("as"), unq_label, labellist_brackets

def wherefield():  return wherefield_entry, Optional(where_join)
def wherefield_entry(): return [binary_comp, unary_comp, where_in, bracket_wherefield,not_wherefield]
def bracket_wherefield(): return "(",wherefield,")"
def not_wherefield(): return IKwd("not"), wherefield

def where_in():     return Optional(IKwd("not")),field, Optional(IKwd("not")), IKwd("in"), "(", literal_value, ZeroOrMore(",",literal_value), ")"
def where_join():   return [IKwd("and"), IKwd("or")], wherefield

def binary_comp():  return Optional(IKwd("not")), value, operation, value
def unary_comp():   return value, IKwd("is"), Optional(IKwd("not")), IKwd("null")

def order_by():     return IKwd("order"),IKwd("by"), order_term, ZeroOrMore(",", order_term)
def order_term():   return value, Optional([IKwd("asc"),IKwd("desc")]), Optional(IKwd("nulls"),[IKwd("first"),IKwd("last")])


def select():       return IKwd("select"),[fieldlist,allfield],IKwd("from"),table,Optional(where),Optional(order_by),Optional(IKwd("union"),Optional(IKwd("all")),select)

def stmt():         return select, EOF


class Visitor(PTNodeVisitor):
    def visit_number(self,n,c):
        try:
            return int(str(n))
        except:
            return float(str(n))
    
    
    
    def visit_strliteral(self,n,c):
        r= str(c[0]) if c else str(n)
        if r[:2] in ("e'", "E'") and r[-1] == "'":
            return eval(r[1:])
        if r[0]=="'" and r[-1]=="'":
            return r[1:-1]
        return r
    
    def visit_literal_value(self, n, c):
        return sq.Val(c[0])    
    
    def visit_null(self,n,c):
        return sq.Val(sq.null())
    
    def visit_qq_label(self,n,c):
        return c[0]
    def visit_unq_label(self,n,c):
        return str(n)

    def visit_label(self,n,c):
        #if len(c)>1:
        #    print 'visit_label', c
        if c[0] in ('l.','p.'):
            
            c = [c[0]+c[1]]+c[2:]
        
        ll = sq.Label(c[0])
        if c[0]=='osm_id':
            ll = sq.FeatId(c[0])
        
                
                    
            
        return ll



    def visit_value(self,n, c):
        if len(c)==1:
            if type(c[0])==str:
                return sq.Label(c[0])
            return c[0]
        if len(c)!=3:
            
            raise Exception("??, visit_value "+repr(n)+" "+repr(c))
        if c[1]=='||':
            if c[2].type=='Op':
                return sq.Concat([c[0]]+c[2].Columns,False,'')
            return sq.Concat([c[0],c[2]],False,'')
        elif c[1] in ('->','?','@>','->>'):
            qq= sq.ColumnAccess(c[0], c[2].StoredValue())
            #print 'hstore', qq
            return qq
        elif c[1]=='::':
            #print 'cast_value', repr(c)
            ll,cc=c[0],c[2]
            if str(cc).lower() == 'integer':
                return sq.AsInteger(ll)
            
            elif str(cc).lower() == 'numeric':
                return sq.AsFloat(ll)
            elif str(cc).lower() == 'text':
                return sq.AsString(ll)
            elif str(cc).lower() == 'jsonb':
                return ll
            raise Exception("type cast ?? "+cc)
            
        else:
                
            return sq.ColumnOp(c[0],get_arithmitic_op(c[1]),c[2])

    def visit_asfield(self,n,c):
        if len(c)!=2:
            raise Exception("visit_asfield",repr(n),len(c),repr(c))
        
        return sq.AsLabel(c[1],c[0])

    def visit_allfield(self,n,c):
        return sq.ColumnList([],True)
        #return All()

    def visit_case_when(self,n,c):
        #return CaseWhen(c[0],c[1])
        return sq.Clause(c[0],c[1])
    def visit_case_else(self,n,c):
        #return CaseElse(c[0])
        return sq.Clause(None,c[0])

    def visit_case_value(self,n,c):
        return sq.Case(c)

    def visit_case_weird_when(self,n,c):
        raise Exception('case_weird')
        return ValWhen(c[0],c[1])
    def visit_case_weird_else(self,n,c):
        raise Exception('case_weird')
        return ValElse(c[0])

    def visit_case_weird(self,n,c):
        raise Exception('case_weird')
        return ValCase(c[0], c[1:])
        

    def visit_function(self,n,c):
        fn = str(c[0]).lower()
        if fn == 'coalesce':
            return sq.Coalesce(c[1])
        elif fn == 'concat':
            return sq.Concat(c[1],True,'')
        elif fn == 'concat_ws':
            #print 'concat_ws', c
            return sq.Concat(c[1][1:],True,c[0][0])
        elif fn == 'nullif':
            return sq.NullIf(c[1][0],c[1][1])
        elif fn == 'pixel_area':
            return sq.PixelArea()
        elif fn == 'pixel_size':
            return sq.PixelSize()
        
        
        elif fn in ('char_length', 'length'):
            return sq.CharLength(c[1][0])
        
        
        elif fn == 'substr':
            
            return sq.SubStr(c[1][0],c[1][1],c[1][2])
        
        elif fn == 'array_to_string':
            #print 'array_to_string', c[1][0],repr(c[1][1].StoredValue())
            return sq.ArrayToString(c[1][0],c[1][1].StoredValue())
        elif fn == 'string_to_array':
            return sq.StringToArray(c[1][0],c[1][1].StoredValue())
        elif fn == 'max_entry_length':
            return sq.MaxEntryLength(c[1][0])
        elif fn == 'array_length':
            return sq.ArrayLength(c[1][0])
        
        elif fn == 'round':
            return sq.AsInteger(c[1][0])
        print('other func', fn)
        return Func(c[0],c[1] if len(c)>1 else [])


    def visit_fieldlist(self,n,c):
        return sq.ColumnList(c,False)

    def visit_valuelist(self,n,c):
        return list(c)

    def visit_table(self,n,c):
        if len(c)==1:
            if hasattr(c[0],'type') and c[0].type=='Table':
                return c[0]
            return sq.PickTable(c[0])

        if len(c)==2:
            if hasattr(c[0],'type') and c[0].type=='Table':
                #c[0].label = c[1]
                return c[0]
            if c[1] in ('p','l'):
                return sq.PickTable(c[0])
            #return PickTable(c[1],c[0])
            raise Exception("???", 'visit_table', [str(z)[:25] for z in c])
        
        #print 'visit_table', map(repr,c)
        #return c[0]
        left = c[0]
        if not hasattr(left,'type'):
            left=sq.PickTable(left)
            
        return sq.Join(left, c[2], map(sq.Label,c[3]), '')


    def visit_not_wherefield(self, n, c):
        #print 'visit_not_wherefield', c
        return sq.Not(c[0])
        
    def visit_whereand(self,n,c):
        return sq.And(c[0],c[1])
    def visit_whereor(self,n,c):
        return sq.Or(c[0],c[1])
        
    def visit_binary_comp(self,n,c):
        
        if len(c)==4 and str(c[0])=='not':
            return sq.Not(self.visit_binary_comp(n,c[1:]))
            #opf = get_comparision_op(c[2])
            #return sq.Not(sq.Op(c[1],opf,c[3]))
        
        if c[1]=='~':
            return sq.RegEx(c[0],c[2].StoredValue())
        if c[1] in ('->','?','@>','->>'):
            
            if c[1]=='?':
                
                qq = sq.Not(sq.IsNull(sq.ColumnAccess(c[0],c[2].StoredValue())))
                #print 'hstore contains', qq
                return qq
            elif c[1]=='@>':
                nn = c[2].StoredValue()
                
                if '=>' in nn:
                    k,v=nn.split("=>")
                    return sq.Op(sq.ColumnAccess(c[0],k),get_comparision_op('='),sq.Val(v))
                else:
                    t=json.loads(nn)
                    if len(t)!=1:
                        raise Exception("@> with multiple values??")
                    (k,v),=t.items()
                    return sq.Op(sq.ColumnAccess(c[0],k),get_comparision_op('='),sq.Val(v))
                
            else:
                print('??',c)
        opf = get_comparision_op(c[1])
        
        return sq.Op(c[0],opf,c[2])

    def visit_unary_comp(self,n,c):
        if len(c)==1:
            return sq.IsNull(c[0])
        if len(c)==2 and str(c[1])=="not":
            return sq.Not(sq.IsNull(c[0]))

        raise Exception("??? visit_unary_comp %s {%d} %s" % (repr(n),len(c),repr(c)))

    def visit_bracketwhere(self, n, c):
        #print 'visit_bracketwhere', len(c)
        return c[0]

    def visit_where_join(self, n, c):
        #print 'where_join', len(c), c[0],c[1]
        return c

    def visit_wherefield(self, n, c):
        #print 'visit_wherefield', n, len(c), c[1] if len(c)>1 else None
        if len(c)==1:
            return c[0]
        a,b = c[1]
        if a=='and':
            return sq.And(c[0],b)
        elif a=='or':
            return sq.Or(c[0],b)
        raise Exception("??? visit_wherefield %s %s" % (repr(n),repr(c)))

    def visit_where(self,n,c):
        #print 'visit_where', repr(n), repr(c)
        return c[0]

    def visit_where_in(self,n,c):
        isnot=False
        lab = c[0]
        if str(lab)=='not':
            isnot=True
            lab=c[1]
        elif str(c[1]) == 'not':
            isnot=True
        i=1
        if isnot: i=2
        vals = c[i:]

        inv = sq.In(lab,vals)
        if isnot:
            return sq.Not(inv)
        return inv


    def visit_order_by(self,n,c):
        return sq.OrderBy(list(c))
        
        

    def visit_order_term(self,n,c):
        return sq.OrderItem(c[0], c[1]!='desc' if len(c)>1 else True)

    def visit_labellist_brackets(self,n,c):
        return list(c)

    def visit_simple_valuelist_brackets(self,n,c):
        return list(c)

    def visit_valuestable(self,n,c):
        #print 'visit_valuestable', repr(n), len(c), repr(c)
        
        
        vals = c[:-2]
        nn, cols=c[-2:]
        feats=sq.FeatureVec()
        for i,v in enumerate(vals):
            p=sq.KeyValueMap()
            for kk,vv in zip(cols,v):
                p[kk]=vv
            #print map(type,[1,p,0,str("")])
            f=sq.make_feature(1,p,0,str(""))
            #print f
            feats.append(f)
            
        
        return sq.ValuesTable(feats, nn)
        
        

    def visit_select(self,n,c):
        #print 'visit select', len(c), [str(z)[:25] for z in c]
        if len(c)==1 and (isinstance(c[0],sq.Select) or isinstance(c[0], sq.Union)): return c[0]
        
        
        cols = c[0]
        table = c[1]
        where = None
        order = None
        name = ""
        
        
        i=2
        if i<len(c) and hasattr(c[i],'type') and c[i].type == 'Where':
            
            where=c[i]
            i+=1
        if i<len(c) and hasattr(c[i],'type') and c[i].type == 'OrderBy':
            #print 'order',repr(c[i])[:25]
            order=c[i]
            i+=1
            
        
        ss=sq.Select(cols,table,where,order,name)
        
        if i<len(c):
            if c[i]=='all':
                i+=1

            return sq.Union(ss,c[i],name)
        return ss




def clean_query(s):
    s="\n".join(l[:l.find('--')] if '--' in l else l for l in s.split("\n"))
    s=s.replace("!pixel_width!*!pixel_height!","pixel_area()")
    s=s.replace("!pixel_width!::real*!pixel_height!::real","pixel_area()")
    s=s.replace("!pixel_width!::real","pixel_size()")
    #s=s.replace("layer~E'^-?\\\\d+$'","layer is not null")
    #s=s.replace("ele ~ \'^-?\\d{1,4}(\\.\\d+)?$\'", "ele is not null")
    #s=s.replace("population ~ '^[0-9]{1,8}$'", "population is not null")
    s=s.replace("CONCAT(REPLACE(ROUND(elevation)::TEXT, \'-\', U&\'\\2212\'), U&\'\\00A0\', \'m\')","CONCAT(elevation::TEXT,'m')")
    #s=s.replace("E\'\\n\'","'\\n'")
    s=s.replace("(SELECT MAX(char_length(ref)) FROM unnest(refs) AS u(ref))", "max_entry_length(refs)")

    return s

parser = ParserPython(stmt)
proc = lambda qu: visit_parse_tree(parser.parse((('select * from ') if qu.strip().startswith('(') else '')+clean_query(qu)), Visitor())
