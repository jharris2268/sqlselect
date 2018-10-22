#ifndef TABLE_SELECT_HPP
#define TABLE_SELECT_HPP

#include "table.hpp"
#include "columnlist.hpp"
#include "where.hpp"
#include "orderby.hpp"

namespace sqlselect {
    
class Select : public Table {
    
    
    public:
        Select(
            ColumnList::Ptr columns_,
            Table::Ptr table_,
            Where::Ptr where_,            
            OrderBy::Ptr orderby_,
            const std::string& name_) :
            
            columns(columns_), table(table_), where(where_), orderby(orderby_), name(name_) {}
        
        
        virtual ~Select() {}
        typedef std::shared_ptr<Select> Ptr;
        static Ptr Create(
            ColumnList::Ptr columns, Table::Ptr table,
            Where::Ptr where,     OrderBy::Ptr orderby,
            const std::string& name) {
                return std::make_shared<Select>(columns,table,where,orderby,name); }
        
        
        Result::Ptr Call(Data::Ptr data, int64 zoom) const;
        std::string Str() const;
            
        ColumnList::Ptr GetColumns() const { return columns; }
        Table::Ptr GetTable() const { return table; }
        Where::Ptr GetWhere() const { return where; }
        OrderBy::Ptr GetOrderBy() const { return orderby; }
        const std::string& GetName() const { return name; }
        void SetName(const std::string& n) { name = n; }
            
    private:
        ColumnList::Ptr columns;
        Table::Ptr table;
        Where::Ptr where;
        OrderBy::Ptr orderby;
        std::string name;
};
    
}
#endif
