#include "result.hpp"

namespace sqlselect {

Feature_vec read_result(Result::Ptr result) {
    Feature_vec ans;
    if (!result) { return ans; }
    auto f = result->next();
    while (f) {
        ans.push_back(f);
        f=result->next();
    }
    return ans;
}

}
