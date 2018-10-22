#ifndef WHERE_OP_HPP
#define WHERE_OP_HPP

#include "where.hpp"
#include "column.hpp"
namespace sqlselect {
    
enum comparison_type {
    EQ = 0,
    NE,
    LT,
    GT,
    LE,
    GE
};    
std::string comparison_type_str(comparison_type c);

class Op : public Where {
    public:
        Op(Column::Ptr left_, comparison_type op_, Column::Ptr right_) : left(left_), op(op_), right(right_) {}
        virtual ~Op() {}
        
        typedef std::shared_ptr<Op> Ptr;
        static Ptr Create(Column::Ptr left, comparison_type op, Column::Ptr right) { return std::make_shared<Op>(left,op,right); }
        virtual bool Call(Feature::Ptr feat, int64 zoom) const;
        virtual std::string Str() const;
        
        Column::Ptr GetLeft() const { return left; }
        comparison_type GetOp() const { return op; }
        Column::Ptr GetRight() const { return right; }
        
        
    private:
        Column::Ptr left;
        comparison_type op;
        Column::Ptr right;
};

}
#endif
