#ifndef COLUMNLIST_HPP
#define COLUMNLIST_HPP

#include "feature.hpp"
#include "column.hpp"
#include <vector>

namespace sqlselect {

class ColumnList {
    public:
        ColumnList(const std::vector<Column::Ptr>& cols_, bool star_) : cols(cols_), star(star_) {}
        ~ColumnList() {}
        
        typedef std::shared_ptr<ColumnList> Ptr;
        static Ptr Create(const std::vector<Column::Ptr>& cols, bool star) { return std::make_shared<ColumnList>(cols, star); }
        
        
        virtual Feature::Ptr Call(Feature::Ptr feat, int64 zoom) const;
        virtual std::string Str() const;
        
        virtual bool HasStar() const { return star; }
        const std::vector<Column::Ptr>& Cols() const { return cols; }
    private:
        
        std::vector<Column::Ptr> cols;
        bool star;
    
};        

}

#endif
