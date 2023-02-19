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
        iorecord();
        iorecord(const void *buf, size_t size);
        iorecord(const iorecord &other);
        iorecord(iorecord &&other);

    public:
        size_t size() const;
        bool empty() const;
        void *ptr() const;

        void advance(size_t amount);
    };

    class ioqueue {
    private:
        std::deque<iorecord> queue;

    public:
        ioqueue();
        ~ioqueue();

        bool empty() const;

        void push_back(const iorecord& data);
        void push_back(iorecord &&data);

        void push_front(const iorecord &data);
        void push_front(iorecord &&data);

        iorecord pop_front();
    };

}
