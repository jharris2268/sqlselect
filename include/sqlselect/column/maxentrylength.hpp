#ifndef COLUMN_MAXENTRYLENGTH_HPP
#define COLUMN_MAXENTRYLENGTH_HPP

#include "column.hpp"

namespace sqlselect {


class MaxEntryLength : public Column {
    public:
        typedef std::shared_ptr<MaxEntryLength> Ptr;
        MaxEntryLength(Column::Ptr col_) : col(col_) {}
        
        virtual ~MaxEntryLength() {}
        static Ptr Create(Column::Ptr col) {
            return std::make_shared<MaxEntryLength>(col);
        }
        
        
        virtual Value Call(Feature::Ptr feat, int64 zoom) const {
            auto r = col->Call(feat,zoom);
            
            auto sl = split_string(r.to_string(), 0);
            size_t res=0;
            for (const auto& s: sl) {
                if (s.size()>res) {
                    res=s.size();
                }
            }
            return (mapnik::value_integer) res;
                    
        }
        virtual std::string Str() const {
            return "MaxEntryLength("+col->Str()+")";
        }
        
        Column::Ptr GetColumn() const { return col; }
        
    private:
        
        Column::Ptr col;
        
};

}

#endif
