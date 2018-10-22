#ifndef COLUMN_PIXELAREA_HPP
#define COLUMN_PIXELAREA_HPP

#include "column.hpp"

namespace sqlselect {
    
class PixelArea : public Column {
    public:
        typedef std::shared_ptr<PixelArea> Ptr;
        PixelArea() {}
        
        virtual ~PixelArea() {}
        static Ptr Create() { return std::make_shared<PixelArea>(); }
        
        virtual Value Call(Feature::Ptr feat, int64 zoom) const {
            double earth_circum = 40075016.6856;
            double ps = earth_circum / 256 / (1<<zoom);
            return ps*ps;
        }
            
        virtual std::string Str() const {
            return "PixelArea()";
        }
        
        
};



}
#endif
