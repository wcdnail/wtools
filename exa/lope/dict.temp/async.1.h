#pragma once

#include <boost/noncopyable.hpp>
#include <boost/asio.hpp>
#include <string>

namespace Lope
{
    class Client: boost::noncopyable
    {
    public:
        Client(boost::asio::io_service& io_service, std::string const& server, std::string const& path);

    private:
        void handle_resolve(const boost::system::error_code& err, boost::asio::ip::tcp::resolver::iterator endpoint_iterator);
        void handle_connect(const boost::system::error_code& err);
        void handle_write_request(const boost::system::error_code& err);
        void handle_read_status_line(const boost::system::error_code& err);
        void handle_read_headers(const boost::system::error_code& err);
        void handle_read_content(const boost::system::error_code& err);

        boost::asio::ip::tcp::resolver resolver_;
        boost::asio::ip::tcp::socket socket_;
        boost::asio::streambuf request_;
        boost::asio::streambuf response_;
    };
}
