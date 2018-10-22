#include "column/label.hpp"
#include "column/aslabel.hpp"
#include "column/val.hpp"

namespace sqlselect {

        
std::string Label::Str() const { return std::string("Label(\"")+key+"\")"; }
        
std::string AsLabel::Str() const { return std::string("AsLabel(\"")+key+"\", " +parent->Str()+")"; }
std::string Val::Str() const { return "Val("+to_string(val)+")"; }

}
