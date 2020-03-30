#include "pch.h"
#include "server.h"

#include "server.h"

void ClientConnection::stop()
{
	if (!isStarted)
		return;
	isStarted = false;
	sock.close();

	client_ptr ptr = shared_from_this();
	srv->deleteClient(ptr);
}

void ClientConnection::do_write(const Message& msg, AnswerCallback callback)
{
	if (!isStarted)
		return;
	memcpy(writeBuffer, &msg, sizeof(Message));
	sock.async_write_some(buffer(writeBuffer, sizeof(Message)),
		boost::bind(&ClientConnection::on_write, shared_from_this(), _1, _2, callback));
}

void ClientConnection::do_read(AnswerCallback callback)
{
	async_read(sock, buffer(readBuffer), transfer_exactly(sizeof(Message)),
		boost::bind(&ClientConnection::on_read, shared_from_this(), _1, _2, callback));
}

void ClientConnection::on_read(ClientConnection::error_code err, size_t bytes, AnswerCallback callback)
{
	if (callback) {
		callback(!err && bytes == sizeof(Message));
	}
	if (err || bytes != sizeof(Message)) {
		stop();
	}
	if (!isStarted)
		return;
	Message msg;
	memcpy(&msg, readBuffer, sizeof(Message));
}

void Server::manager()
{
	if (active_client != clients.end()) {
		Message msg;
		msg.time = light_time;
		msg.color = *active_color;
		msg.command = Message::SET_COLOR;
		server_ptr ptr = shared_from_this();
		(*active_client)->do_write(msg, [ptr](bool isCorrect) {
			if (isCorrect) {
				auto old_client = ptr->active_client;
				ptr->nextClient();
				ptr->nextColor();
				auto sleep_time = old_client == ptr->active_client ? ptr->light_time * 2 : ptr->light_time;
				ptr->timer.expires_from_now(boost::posix_time::milliseconds(sleep_time));
				ptr->timer.async_wait(boost::bind(&Server::manager, ptr));
			}
			else {
				ptr->nextClient();
				return ptr->manager();
			}
			});
	}
}

void Server::handle_accept(client_ptr client, const boost::system::error_code& err)
{
	client->start();
	int oldSize = clients.size();
	clients.push_back(client);
	if (oldSize == 0) {
		active_client = clients.begin();
		manager();
	}
	client_ptr new_client = ClientConnection::create(service, shared_from_this());
	acceptor.async_accept(new_client->getSock(),
		boost::bind(&Server::handle_accept, shared_from_this(), new_client, _1));
}

void Server::run()
{
	client_ptr client = ClientConnection::create(service, shared_from_this());
	acceptor.async_accept(client->getSock(),
		boost::bind(&Server::handle_accept, shared_from_this(), client, _1));
}

void Server::deleteClient(client_ptr client)
{
	auto it = std::find(clients.begin(), clients.end(), client);
	if (it == active_client && it != clients.end()) {
		nextClient();
	}
	clients.erase(it);
}
