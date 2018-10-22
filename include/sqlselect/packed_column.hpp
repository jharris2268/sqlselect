#ifndef PACKED_COLUMN_HPP
#define PACKED_COLUMN_HPP

#include <iostream>

inline size_t read_int(const std::string& src, size_t pos) {
    size_t res=0;
    
    res += ((unsigned char) src[pos])*256;
    res += ((unsigned char) src[pos+1]);
    
    return res;
}
    

inline std::string get_key(const std::string& src, const std::string& key) {
    size_t pos=1;
    
    while (pos < src.size()) {
        size_t kl = read_int(src, pos);
        if (pos+2+kl > src.size() ) {
            std::cout << "??? " << src << " @ " << pos << " : " << kl << "/" << src.size() << std::endl;
            return std::string();
        }
        std::string k = src.substr(pos+2, kl);
        pos += (2+kl);
        size_t vl = read_int(src, pos);
        
        if (key == k) {
            if (pos+2+vl > src.size() ) {
                std::cout << "??? " << src << " (key=" << k << ") @ " << pos << " : " << vl << "/" << src.size() << std::endl;
                return std::string();
            }
            
            
            //std::cout << k << "==" << k << " => " << src.substr(pos+2, vl) << std::endl;
            return src.substr(pos+2, vl);
        }
        pos += (2+vl);
    }
    
    return std::string();
}
    

#endif
