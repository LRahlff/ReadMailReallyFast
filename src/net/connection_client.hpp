/*
 * connection_client.hpp
 *
 *  Created on: 03.01.2021
 *      Author: doralitze
 */

#pragma once

#include <ev++.h>

#include <functional>
#include <memory>
#include <string>

#include "net/async_fd.hpp"
#include "net/ioqueue.hpp"
#include "net/socketaddress.hpp"

#if EAGAIN != EWOULDBLOCK
    #define EAGAIN_WRAPPER ((errno != EAGAIN) && (errno != EWOULDBLOCK))
#else
    #define EAGAIN_WRAPPER (errno != EAGAIN)
#endif

namespace rmrf::net {

enum class exit_status_t : uint16_t {
    NO_ERROR = 0,
    TIMEOUT = 1
};

class connection_client {
public:
    /**
      * This function type accepts a reference to the incomming data string which it may not alter
      * and may not return any data.
      */
    typedef std::function<void(const iorecord&)> incomming_data_cb;
    
    /**
      * A callback for the destructor must match this definition.
      */
    typedef std::function<void(exit_status_t)> destructor_cb_type;
    
    static const unsigned int no_rate_limit = 0;

protected:
    auto_fd net_socket;
private:
    ::ev::io io;
    ioqueue<iorecord> write_queue;
    const destructor_cb_type destructor_cb;
    bool server_active = false;
    bool partial_write_allowed = true;
    bool data_write_active = false;
    
    unsigned int rate_limit = no_rate_limit;
    unsigned int current_burst_count = 0;

protected:
    incomming_data_cb in_data_cb;
    socketaddr own_address, peer_address;

public:

    /**
     * This constructor takes ownership over the provided file descriptor and registeres it
     * with libev if it is valid. If an invalid file descriptor or null_fd is passed, libev will not be invoked.
     * If this is intentional, an implementing sub class might overwrite write_data in such a case.
     * @brief Construct a new connection client.
     * @param socket_fd The file descriptor to register with libev
     * @param peer_address_ The remote address the client is connected to
     */
    connection_client(auto_fd&& socket_fd, const socketaddr& peer_address_, const destructor_cb_type destructor_cb_) :
        net_socket(std::forward<auto_fd>(socket_fd)), io{}, write_queue{}, destructor_cb{destructor_cb_},
        in_data_cb{}, own_address{}, peer_address{peer_address_}
    {
        if(this->net_socket) {
            io.set<connection_client, &connection_client::cb_ev>(this);
            io.start(this->net_socket.get(), 0);
            this->server_active = true;
        }
    }
    
    virtual ~connection_client();

    /**
     * Use this method to send data to the other endpoint. Overwrite it to modify the data beforehand,
     * but don't forget to call this super method, if you still intend to write the data using libev.
     * @param data The data to send
     */
    inline virtual void write_data(const iorecord& data) {
        // Create NICBuffer from data
        this->write_queue.push_back(data);
        this->io.set(::ev::READ | ::ev::WRITE);
    }
    
    /**
     * Use this method to send data to the other endpoint. In contrast to write_data this method is not virtual.
     * This data passes the raw buffer to the push_back_emplace method of the ioqueue.
     * @brief Push data to write queue with explicit single copy
     * @param buf The buffer of the data to be send
     * @param size The length of the buffer
     * @param sa The socket address associated with the data
     */
    inline void write_data(iorecord&& data) {
        this->write_queue.push_back(std::forward<iorecord>(data));
        this->io.set(::ev::READ | ::ev::WRITE);
    }
    
   /**
     * This method sends data over the socket. Keep in mind that this method only enqueues the
     * data and requests it's transmission but does not send the data directly. While this ensures
     * asynchronous execution you need to check if your data has been send using the is_write_queue_empty
     * method.
     * @brief Send data.
     * @param data The data to send
     */
    inline void write_data(const std::string& data) {
        this->write_data(iorecord{data.c_str(), data.size()});
    }

    /**
     * Use this method in order to register your callback function that should be
     * called when the client got data to process.
     * @param cb The callback function to register [void(iorecord& data)]
     */
    virtual void set_incomming_data_callback(const incomming_data_cb& cb);
    
    /**
     * Get the connected sockets address. Keep in mind that the lone existance of this address
     * does not guaruntee the integrity of the connection. Furthermore, in case of a one-to-many
     * connection setting, it is not garunteed which address might be reported back. Check the
     * documentation of the child class in such a case.
     * @brief Get the connected sockets address
     * @return The peers address.
     */
    inline const socketaddr get_peer_address() const {
        return this->peer_address;
    }

    /**
     * Use this method in order to check if all pending write requests have been successfully transmitted.
     * This method returns false if the write queue is not empty or the socket is currently transmitting data.
     * Note: If the socket connection crashes with an netio exception it might be possible that this
     * method returns true even if the last data send has been corrupted.
     * @brief Check if all pending data has been send.
     * @return true if all pending data has been send.
     */
    inline bool is_write_queue_empty() {
        return this->write_queue.empty() && !this->data_write_active;
    }
    
    /**
     * @brief Set a maximum number of packets to transmit per burst
     * @param new_limit The limit to set
     */
    void set_rate_limit(unsigned int new_limit);

    inline unsigned int get_rate_limit() {
        return this->rate_limit;
    }

protected:
    /**
     * @brief Implement the read operation of the implemented service
     */
    virtual void read_from_socket(::ev::io& w) = 0;

    /**
     * This method pushes the front of the write_queue to the socket. It will automatically advance
     * the internal buffers and discard empty ones.
     * @param w The io handle to use.
     * @param buffer The data to be transmitted
     */
    virtual ssize_t push_write_queue(::ev::io& w, iorecord& buffer) = 0;

    /**
     * @brief This method closes the socket and stops the server.
     */
    void stop_server();

private:
    /**
     * This method implements the io queue callback. It is responsible for the actual data transactions
     * @param w The io handle to use
     * @param events The event flag container
     */
    void cb_ev(::ev::io& w, int events);
    
    inline void set_new_flags() {
        auto new_flags = 0;
        if(this->in_data_cb) {
            new_flags |= ::ev::READ;
        }
        if (!this->write_queue.empty()) {
            new_flags |= ::ev::WRITE;
        }
        this->io.set(new_flags);
    }
};

}

