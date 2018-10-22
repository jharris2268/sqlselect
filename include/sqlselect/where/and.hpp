#ifndef WHERE_AND_HPP
#define WHERE_AND_HPP

#include "where.hpp"

namespace sqlselect {
    
class And : public Where {
    public:
        And(Where::Ptr left_, Where::Ptr right_) : left(left_), right(right_) {}
        
        virtual ~And() {}
        typedef std::shared_ptr<And> Ptr;
        static Ptr Create(Where::Ptr left, Where::Ptr right) { return std::make_shared<And>(left,right); }
        
        bool Call(Feature::Ptr feat, int64 zoom) const;
        
        std::string Str() const;
        
        Where::Ptr GetLeft() const { return left; }
        Where::Ptr GetRight() const { return right; }
        
    private:
        Where::Ptr left, right;
};

}
#endif
