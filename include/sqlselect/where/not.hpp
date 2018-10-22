#ifndef WHERE_NOT_HPP
#define WHERE_NOT_HPP

#include "where.hpp"

namespace sqlselect {


class Not : public Where {
    public:
        Not(Where::Ptr where_) : where(where_) {}
        
        virtual ~Not() {}
        typedef std::shared_ptr<Not> Ptr;
        static Ptr Create(Where::Ptr val) { return std::make_shared<Not>(val); }
        
        bool Call(Feature::Ptr feat, int64 zoom) const;        
        std::string Str() const;
        Where::Ptr GetWhere() const { return where; }
        
    private:
        Where::Ptr where;
};
    
}
#endif
