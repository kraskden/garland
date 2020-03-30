#pragma once

#include <list>

#include <boost/noncopyable.hpp>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

// from ../Message
#include "message.h"

using namespace boost::asio;

class ClientConnection;
class Server;
typedef boost::shared_ptr<ClientConnection> client_ptr;
typedef boost::shared_ptr<Server> server_ptr;

typedef std::function<void(bool)> AnswerCallback;

class ClientConnection : public boost::enable_shared_from_this<ClientConnection>,
	boost::noncopyable
{
	server_ptr srv;

	ClientConnection(io_service& service, server_ptr srv) : srv(srv), sock(service), isStarted(false)
	{

	}
public:
	typedef boost::system::error_code error_code;

	void start() {
		isStarted = true;
	}

	void stop();

	ip::tcp::socket& getSock() {
		return sock;
	}

	void do_write(const Message& msg, AnswerCallback callback = nullptr);

	void do_read(AnswerCallback callback);

	void on_write(error_code err, size_t bytes, AnswerCallback callback) {
		do_read(callback);
	}

	void on_read(error_code err, size_t bytes, AnswerCallback callback);

	static client_ptr create(io_service& service, server_ptr srv) {
		client_ptr ptr(new ClientConnection(service, srv));
		return ptr;
	}

private:
	ip::tcp::socket sock;
	char readBuffer[sizeof(Message)];
	char writeBuffer[sizeof(Message)];
	bool isStarted;
};


class Server : public boost::enable_shared_from_this<Server>,
	boost::noncopyable
{
	deadline_timer timer;
	ip::tcp::acceptor acceptor;
	io_service& service;

	int light_time = 400;

	std::list<client_ptr> clients;
	std::list<uint32_t> colors{ 0xFF0000, 0x00FF00, 0x0000FF, 0x00FFFF};
	std::list<client_ptr>::iterator active_client = clients.begin();
	std::list<uint32_t>::iterator active_color = colors.begin();

	Server(io_service& service, int port) : service(service), timer(service),
		acceptor(service, ip::tcp::endpoint(ip::tcp::v4(), port))
	{}

	void nextClient() {
		if (++active_client == clients.end()) {
			active_client = clients.begin();
		}
	}

	void nextColor() {
		if (++active_color == colors.end() || active_client == clients.begin()) {
			active_color = colors.begin();
		}
	}

	void manager();

	void handle_accept(client_ptr client, const boost::system::error_code& err);


public:
	static server_ptr create(io_service& service, int port) {
		return server_ptr(new Server(service, port));
	}

	void run();

	void deleteClient(client_ptr client);
};