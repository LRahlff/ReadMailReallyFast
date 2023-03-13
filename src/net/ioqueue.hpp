#pragma once

#include <algorithm>
#include <cstdint>
#include <deque>
#include <string>
#include <utility>
#include <vector>

#include "net/iorecord.hpp"

namespace rmrf::net {

    template<class iorecord_type = iorecord>
    class ioqueue {
    private:
        std::deque<iorecord_type> queue;

    public:
        static_assert(std::is_base_of<iorecord, iorecord_type>::value, "Your iorecord implementation needs to be a subclass of iorecord.");
        ioqueue() : queue{} {
            // NOP
        }

        ~ioqueue() {
            // NOP
        }

        bool empty() const {
            return this->queue.empty();
        }

        void push_back(const iorecord_type& data) {
            if (!data.empty()) {
                this->queue.push_back(data);
            }
        }
        
        void push_back(iorecord_type&& data) {
            if (!data.empty()) {
                this->queue.emplace_back(std::forward<iorecord_type>(data));
            }
        }

        void push_front(const iorecord_type &data) {
            if (!data.empty()) {
                this->queue.push_front(data);
            }
        }

        void push_front(iorecord_type &&data) {
            if (!data.empty()) {
                this->queue.emplace_front(std::forward<iorecord_type>(data));
            }
        }

        iorecord_type pop_front() {
            if (this->empty()) {
                return iorecord_type{};
            }

            auto result = this->queue.front();
            this->queue.pop_front();
            return result;
        }

        std::deque<iorecord_type>::iterator begin() {
            return this->queue.begin();
        }

        std::deque<iorecord_type>::iterator end() {
            return this->queue.end();
        }
    };

}
