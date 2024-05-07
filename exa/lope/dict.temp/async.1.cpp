#include "stdafx.h"
#include "async.1.h"
#include <dh.tracing.h>
#include <boost/bind.hpp>

namespace Lope
{
    Client::Client(boost::asio::io_service& io_service, std::string const& server, std::string const& path)
        : resolver_(io_service)
        , socket_(io_service)
    {
        // Form the request. We specify the "Connection: close" header so that the
        // server will close the socket after transmitting the response. This will
        // allow us to treat all data up until the EOF as the content.
        std::ostream request_stream(&request_);
        request_stream << "GET " << path << " HTTP/1.0\r\n";
        request_stream << "Host: " << server << "\r\n";
        request_stream << "Accept: */*\r\n";
        request_stream << "Connection: close\r\n\r\n";

        // Start an asynchronous resolve to translate the server and service names
        // into a list of endpoints.
        boost::asio::ip::tcp::resolver::query query(server, "https");
        resolver_.async_resolve(query,
            boost::bind(&Client::handle_resolve, this,
                boost::asio::placeholders::error,
                boost::asio::placeholders::iterator));
    }

    void Client::handle_resolve(const boost::system::error_code& err, boost::asio::ip::tcp::resolver::iterator endpoint_iterator)
    {
        if (!err)
        {
            // Attempt a connection to each endpoint in the list until we
            // successfully establish a connection.
            boost::asio::async_connect(socket_, endpoint_iterator,
                boost::bind(&Client::handle_connect, this,
                boost::asio::placeholders::error));
        }
        else
        {
            Dh::TPrintf("HTTPSREST: Resolver fails - %d (%s)\n", err.value(), err.message().c_str());
        }
    }

    void Client::handle_connect(const boost::system::error_code& err)
    {
        if (!err)
        {
            // The connection was successful. Send the request.
            boost::asio::async_write(socket_, request_,
                boost::bind(&Client::handle_write_request, this,
                boost::asio::placeholders::error));
        }
        else
        {
            Dh::TPrintf("HTTPSREST: Connector fails - %d (%s)\n", err.value(), err.message().c_str());
        }
    }

    void Client::handle_write_request(const boost::system::error_code& err)
    {
        if (!err)
        {
            // Read the response status line. The response_ streambuf will
            // automatically grow to accommodate the entire line. The growth may be
            // limited by passing a maximum size to the streambuf constructor.
            boost::asio::async_read_until(socket_, response_, "\r\n",
                boost::bind(&Client::handle_read_status_line, this,
                boost::asio::placeholders::error));
        }
        else
        {
            Dh::TPrintf("HTTPSREST: Writer fails - %d (%s)\n", err.value(), err.message().c_str());
        }
    }

    void Client::handle_read_status_line(const boost::system::error_code& err)
    {
        if (!err)
        {
            // Check that response is OK.
            std::istream response_stream(&response_);
            std::string http_version;
            response_stream >> http_version;
            unsigned int status_code;
            response_stream >> status_code;
            std::string status_message;
            std::getline(response_stream, status_message);
            if (!response_stream || http_version.substr(0, 5) != "HTTP/")
            {
                Dh::TPrintf("HTTPSREST: Invalid response\n");
                return;
            }

            if (status_code != 200)
            {
                Dh::TPrintf("HTTPSREST: Response returned with status code %d\n", status_code);
                return;
            }

            // Read the response headers, which are terminated by a blank line.
            boost::asio::async_read_until(socket_, response_, "\r\n\r\n",
                boost::bind(&Client::handle_read_headers, this,
                boost::asio::placeholders::error));
        }
        else
        {
            Dh::TPrintf("HTTPSREST: Status read fails - %d (%s)\n", err.value(), err.message().c_str());
        }
    }

    void Client::handle_read_headers(const boost::system::error_code& err)
    {
        if (!err)
        {
            // Process the response headers.
            std::istream response_stream(&response_);
            std::string header;
            while (std::getline(response_stream, header) && header != "\r")
            std::cout << header << "\n";
            std::cout << "\n";

            // Write whatever content we already have to output.
            if (response_.size() > 0)
            std::cout << &response_;

            // Start reading remaining data until EOF.
            boost::asio::async_read(socket_, response_,
                boost::asio::transfer_at_least(1),
                boost::bind(&Client::handle_read_content, this,
                boost::asio::placeholders::error));
        }
        else
        {
            Dh::TPrintf("HTTPSREST: Header read fails - %d (%s)\n", err.value(), err.message().c_str());
        }
    }

    void Client::handle_read_content(const boost::system::error_code& err)
    {
        if (!err)
        {
            // Write all of the data that has been read so far.
            std::cout << &response_;

            // Continue reading remaining data until EOF.
            boost::asio::async_read(socket_, response_,
                boost::asio::transfer_at_least(1),
                boost::bind(&Client::handle_read_content, this,
                boost::asio::placeholders::error));
        }
        else if (err != boost::asio::error::eof)
        {
            Dh::TPrintf("HTTPSREST: Content read fails - %d (%s)\n", err.value(), err.message().c_str());
        }
    }
}