#pragma once

#include <algorithm>
#include <cstdint>
#include <deque>
#include <utility>
#include <vector>

namespace rmrf::net {

    class iorecord {
    private:
        size_t offset;
        std::vector<uint8_t> data;
    public:
        iorecord() : offset{}, data{} {}
        iorecord(const void *buf, size_t size) :
            offset{0}, data((const uint8_t *)buf, (const uint8_t *)buf + size) {
            // Nothing special to do here ...
        };
        iorecord(iorecord&& other) :
            offset(other.offset), data(std::forward<iorecord>(other.data)) {
            // Nothing special to do here ...
        };
    public:
        size_t size() const {
            return this->data.size() - this->offset;
        }
        bool empty() const {
            return !this->size();
        }
        void *ptr() const {
            return (void*)(this->data.data() + this->offset);
        }

        void advance(size_t amount) {
            this->offset += std::min(amount, this->size());
        }
    };

    class ioqueue {
    private:
        std::deque<iorecord> queue;

    public:
        bool empty() const {
            return this->queue.empty();
        }

        void push_back(const iorecord& data) {
            if(!data.empty()) {
                this->queue.push_back(data);
            }
        }
        void push_back(iorecord &&data) {
            if (!data.empty()) {
                this->queue.emplace_back(std::forward(data));
            }
        }

        void push_front(const iorecord &data) {
            if (!data.empty()) {
                this->queue.push_front(data);
            }
        }
        void push_front(iorecord &&data) {
            if (!data.empty()) {
                this->queue.emplace_front(std::forward(data));
            }
        }

        iorecord pop_front() {
            if(this->empty()) {
                return iorecord{};
            }

            iorecord result = this->queue.front();
            this->queue.pop_front();
            return result;
        }
    };

}
