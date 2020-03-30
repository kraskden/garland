#pragma once

#include <iostream>

#include <boost/noncopyable.hpp>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

#include "bubble.h"

// From ../Message
#include "message.h"

using namespace boost::asio;

class Connection : public boost::enable_shared_from_this<Connection>,
	boost::noncopyable
{
	typedef boost::system::error_code error_code;
	deadline_timer timer;

	WindowBubble* bubble;


	Connection(io_service& service, WindowBubble* bubble) : timer(service), sock(service), bubble(bubble), isStarted(false) {}

public:

	static boost::shared_ptr<Connection> create(io_service &service, WindowBubble* bubble) {
		return boost::shared_ptr<Connection>(new Connection(service, bubble));
	}

	void start(ip::tcp::endpoint ep) {
		sock.async_connect(ep, boost::bind(&Connection::on_connect, shared_from_this(), _1));
		isStarted = true;
	}

	void stop();

	void on_connect(const error_code& code) {
		code ? stop() : do_read();
	}

	void do_read() {
		async_read(sock, buffer(readBuffer), transfer_exactly(sizeof(Message)),
			boost::bind(&Connection::on_read, shared_from_this(), _1, _2));
	}

	void on_read(const error_code& err, size_t bytes);

	void clean_all() {
		bubble->turnOff();
	}

	void do_write(const Message& msg);

	void on_write(error_code err, size_t bytes) {
		do_read();
	}

private:
	ip::tcp::socket sock;
	bool isStarted;
	char writeBuffer[sizeof(Message)];
	char readBuffer[sizeof(Message)];

};
