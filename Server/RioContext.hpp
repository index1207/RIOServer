#pragma once

class Session;

enum class EventType
{
	Send,
	Recv
};

struct RioContext : RIO_BUF
{
	RioContext(EventType _eventType);
	std::shared_ptr<Session> owner;
	EventType eventType;
};

struct SendContext : RioContext
{
	SendContext() : RioContext(EventType::Send) { }
};

struct RecvContext : RioContext
{
	RecvContext() : RioContext(EventType::Recv) { }
};