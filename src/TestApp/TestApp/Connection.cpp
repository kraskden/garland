#include "connection.h"

void Connection::stop()
{
	HWND hWnd = bubble->getHWnd();
	MessageBox(hWnd, L"Connection closed", L"Info", MB_OK | MB_ICONASTERISK);
	DestroyWindow(hWnd);
	isStarted = false;
	sock.close();
}

void Connection::on_read(const error_code& err, size_t bytes)
{
	if (err)
		stop();
	if (!isStarted)
		return;
	Message msg;
	memcpy(&msg, readBuffer, sizeof(Message));
	if (msg.command == Message::SET_COLOR) {
		Color color;
		color.SetFromCOLORREF(msg.color);
		bubble->turnOn(color);
		timer.expires_from_now(boost::posix_time::millisec(msg.time));
		timer.async_wait(boost::bind(&Connection::clean_all, shared_from_this()));
	}
	do_write(msg);
}

void Connection::do_write(const Message& msg)
{
	if (!isStarted)
		return;
	memcpy(writeBuffer, &msg, sizeof(Message));
	sock.async_write_some(buffer(writeBuffer, sizeof(Message)),
		boost::bind(&Connection::on_write, shared_from_this(), _1, _2));
}
