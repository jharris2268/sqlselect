#ifndef COLUMN_COLUMNOP_HPP
#define COLUMN_COLUMNOP_HPP

#include "column.hpp"

namespace sqlselect {
    
class ColumnOp : public Column {
    public:
        typedef std::shared_ptr<ColumnOp> Ptr;
        ColumnOp(Column::Ptr left_, arithmitic_type op_, Column::Ptr right_) : left(left_), op(op_), right(right_) {}
        
        virtual ~ColumnOp() {}
        static Ptr Create(Column::Ptr left, arithmitic_type op, Column::Ptr right) {
            return std::make_shared<ColumnOp>(left,op,right);
        }
        
        
        virtual Value Call(Feature::Ptr feat, int64 zoom) const {
            auto l = left->Call(feat,zoom);
            auto r = right->Call(feat,zoom);
            return arithmitic(l,op,r);
        }
            
        virtual std::string Str() const {
            return "ColumnOp("+left->Str()+", "+arithmitic_type_str(op)+", "+right->Str()+")";
        }
        
        Column::Ptr GetLeft() const { return left; }
        arithmitic_type GetOp() const { return op; }
        Column::Ptr GetRight() const { return right; }
        
    private:
        Column::Ptr left;
        arithmitic_type op;
        Column::Ptr right;
};



}
#endif
