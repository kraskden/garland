#pragma once

struct Message {
	enum Command {
		NOP, SET_COLOR
	};
	Command command;

	UINT32 color;
	UINT32 time;
public:
	Message(Command command = Message::NOP, UINT32 color = 0, UINT32 time = 0) : color(color), time(time), command(command) {}
};

