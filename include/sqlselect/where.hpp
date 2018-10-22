#ifndef WHERE_HPP
#define WHERE_HPP

#include "feature.hpp"

namespace sqlselect {

class Where {
    public:
        typedef std::shared_ptr<Where> Ptr;
        virtual bool Call(Feature::Ptr feature, int64 zoom) const=0;
        virtual std::string Str() const=0;
};


}

#endif
