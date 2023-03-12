#pragma once

#include <algorithm>
#include <cstdint>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include "net/socketaddress.hpp"

namespace rmrf::net {
    
    class iorecord {
    private:
        size_t offset;
        std::vector<uint8_t> data;
        const socketaddr addr;

    public:
        iorecord();
        iorecord(const void *buf, const size_t size);
        iorecord(const void *buf, const size_t size, const socketaddr& sa);
        iorecord(const iorecord& other);
        iorecord(iorecord&& other);

    public:
        size_t size() const;
        bool empty() const;
        void *ptr() const;

        void advance(size_t amount);
        
        [[nodiscard]] inline auto potential_strings_in_record() const {
                return std::count_if(this->data.begin(), this->data.end(), [](uint8_t x){return x == 0;}) + 1;
        }
		
        [[nodiscard]] std::vector<std::string> get_strings_in_record() const;
        
        [[nodiscard]] inline std::string str() const {
            std::stringstream ss;
            for(auto& c : this->data) {
                if(c == 0)
                    break;
                if (c < 32 || c > 126)
                    ss.put('?');
                else
                    ss.put((char) c);
            }
            return ss.str();
        }
        
        const socketaddr& get_address() const {
            return this->addr;
        }
        
        void reserve_space(size_t amount);
    };
}

std::ostream& operator<<(std::ostream& os, const rmrf::net::iorecord& obj);
