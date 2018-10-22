#ifndef COLUMN_NULLIF_HPP
#define COLUMN_NULLIF_HPP

#include "column.hpp"
#include "where.hpp"
namespace sqlselect {



class NullIf : public Column {
    public:
        NullIf(Column::Ptr left_,Column::Ptr right_) : left(left_), right(right_) {}
        
        typedef std::shared_ptr<NullIf> Ptr;
        static Ptr Create(Column::Ptr left,Column::Ptr right) {
            return std::make_shared<NullIf>(left,right);
        }
        
        virtual Value Call(Feature::Ptr feat, int64 zoom) const {
            auto l = left->Call(feat,zoom);
            auto r = right->Call(feat,zoom);
            if (l==r) { return null_value(); }
            return l;
        }
        
        virtual std::string Str() const {
            return "NullIf("+left->Str()+", "+right->Str()+")";
        }
        
        Column::Ptr GetLeft() const { return left; }
        Column::Ptr GetRight() const { return right; }
    private:
        Column::Ptr left;
        Column::Ptr right;
};



}
#endif
