#ifndef WHERE_IN_HPP
#define WHERE_IN_HPP

#include "where.hpp"
#include "column.hpp"
namespace sqlselect {

class In : public Where {
    public:
        In(Column::Ptr left_, std::vector<Column::Ptr> right_) : left(left_), right(right_) {}
        virtual ~In() {}
        
        typedef std::shared_ptr<In> Ptr;
        static Ptr Create(Column::Ptr left, std::vector<Column::Ptr> right) { return std::make_shared<In>(left,right); }
        
        bool Call(Feature::Ptr feat, int64 zoom) const;
        std::string Str() const;
        
        Column::Ptr GetLeft() const { return left; }
        const std::vector<Column::Ptr> GetRight() { return right; }
        
    private:
        Column::Ptr left;
        std::vector<Column::Ptr> right;
};
    
}
#endif
