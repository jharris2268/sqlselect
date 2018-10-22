#ifndef WHERE_OR_HPP
#define WHERE_OR_HPP

#include "where.hpp"

namespace sqlselect {

class Or : public Where {
    public:
        Or(Where::Ptr left_, Where::Ptr right_) : left(left_), right(right_) {}
        
        virtual ~Or() {}
        typedef std::shared_ptr<Or> Ptr;
        static Ptr Create(Where::Ptr left, Where::Ptr right) { return std::make_shared<Or>(left,right); }
        
        bool Call(Feature::Ptr feat, int64 zoom) const;
        
        std::string Str() const;
        
        Where::Ptr GetLeft() const { return left; }
        Where::Ptr GetRight() const { return right; }
    private:
        Where::Ptr left, right;
};
    
}
#endif
