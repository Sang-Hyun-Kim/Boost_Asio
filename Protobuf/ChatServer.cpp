//#include <iostream>
//#include <set>
//#include <boost/asio.hpp>
//#include <boost/asio/buffer.hpp>
//#include <boost/bind/bind.hpp>
//#include <boost/enable_shared_from_this.hpp>
//#include "Packet.h"
//#include "Chat.pb.h"
//
//using boost::asio::ip::tcp;
//using namespace boost;
//class ChatMember
//{
//public:
//	ChatMember() {}
//	virtual ~ChatMember() {}
//	virtual void SendChat(const std::string& msg) = 0;
//	virtual void SetName(const std::string& msg) = 0;
//	virtual std::string GetName() = 0;
//	virtual void Send(asio::mutable_buffer& buffer) = 0;
//};
//typedef boost::shared_ptr<ChatMember> ChatMemberPtr;
//
//class ChatRoom
//{
//public:
//	void Join(ChatMemberPtr member)
//	{
//		chat::ChatNoti pbNoti; // Chat.proto 에 message 객체
//		auto m = pbNoti.add_message();
//		*m = "We have new member.";
//		auto s = pbNoti.add_sender();
//		s->assign("admin"); // 위 아래 같은 방법임
//		const size_t requiredSize = PacketUtil::RequiredSize(pbNoti);
//		// todo: delete buffer
//		char* rawBuffer = new char[requiredSize];
//		auto sendBuffer = asio::buffer(rawBuffer, requiredSize);
//		PacketUtil::Serialize(sendBuffer, chat::MessageCode::CHAT_NOTI, pbNoti); // 5번 호출, 기존 클라, 서버 각각 정해준 직렬화 순서를 통합으로 관리하게됨
//		Broadcast(sendBuffer);
//		_members.insert(member);
//	}
//
//	void Leave(ChatMemberPtr member)
//	{
//		_members.erase(member);
//		chat::ChatNoti pbNoti;
//		auto m = pbNoti.add_message();
//		*m = "A member left.";
//		auto s = pbNoti.add_sender();
//		*s = "admin";
//		const size_t requiredSize = PacketUtil::RequiredSize(pbNoti);
//		// todo: delete buffer
//		char* rawBuffer = new char[requiredSize];
//		auto sendBuffer = asio::buffer(rawBuffer, requiredSize);
//		PacketUtil::Serialize(sendBuffer, chat::MessageCode::CHAT_NOTI, pbNoti);
//		Broadcast(sendBuffer);
//	}
//
//	void Broadcast(const std::string& msg) // 메세지 바로전성
//	{
//		std::for_each(_members.begin(), _members.end(),
//			[msg](ChatMemberPtr p) {
//				p->SendChat(msg);
//			});
//	}
//
//	void Broadcast(asio::mutable_buffer& buffer) // 버퍼전송
//	{
//		std::for_each(_members.begin(), _members.end(),
//			[&buffer](ChatMemberPtr p) {
//				p->Send(buffer);
//			});
//	}
//
//	void SendList(ChatMemberPtr member) // 멤버 리스트를 담아서 전송
//	{
//		chat::ListRes pbRes;
//		std::for_each(_members.begin(), _members.end(),
//			[&pbRes](ChatMemberPtr p) {
//				auto name = pbRes.add_names();
//				name->assign(p->GetName());
//			});
//		const size_t requiredSize = PacketUtil::RequiredSize(pbRes);
//		// todo: delete buffer
//		char* rawBuffer = new char[requiredSize];
//		auto buffer = asio::buffer(rawBuffer, requiredSize);
//		PacketUtil::Serialize(buffer, chat::MessageCode::LIST_RES, pbRes);
//		member->Send(buffer);
//	}
//
//	private:
//		std::set<ChatMemberPtr> _members;
//	
//};
//
//class ChatSession
//	: public ChatMember, public boost::enable_shared_from_this<ChatSession>
//{
//public:
//	ChatSession(asio::io_context& io_context, ChatRoom& room)
//		: _socket(io_context)
//		, _room(room)
//		, _strand(boost::asio::make_strand(io_context))
//	{
//		memset(_recvBuffer, 0, RecvBufferSize);
//		memset(_sendBuffer, 0, SendBufferSize);
//	}
//	void Start()
//	{
//		_room.Join(this->shared_from_this());
//		AsyncRead();
//	}
//	tcp::socket& GetSocket()
//	{
//		return _socket;
//	}
//	void SendChat(const std::string& msg) override
//	{
//		std::cout << "SendChat " << msg.c_str() << std::endl;
//		AsyncWrite(msg.c_str(), msg.size());
//	}
//	void SetName(const std::string& name) override
//	{
//		_name = name;
//	}
//	std::string GetName() override
//	{
//		return _name;
//	}
//	void Send(asio::mutable_buffer& buffer) override
//	{
//		AsyncWrite(static_cast<const char*>(buffer.data()), buffer.size());
//	}
//protected:
//	void AsyncRead()
//	{
//		memset(_recvBuffer, 0, RecvBufferSize);
//		_socket.async_read_some(
//			boost::asio::buffer(_recvBuffer, RecvBufferSize),
//			boost::bind(
//				&ChatSession::OnRead, this,
//				boost::asio::placeholders::error,
//				boost::asio::placeholders::bytes_transferred // 넘겨받은 크기를 알게 해줌
//			)
//		);
//	}
//
//	void OnRead(const boost::system::error_code err, size_t size)
//	{
//		std::cout << "OnRead " << size << std::endl;
//		if (!err)
//		{
//			HandlePacket(_recvBuffer, size); // 패킷 분해 후 검증 후 알맞는 함수 실행, 크기는 Async_Read에서 알 수 있음
//			AsyncRead();
//		}
//		else
//		{
//			std::cout << "error code: " << err.value() << ", msg: " << err.message() << std::endl;
//			_room.Leave(this->shared_from_this());
//		}
//	}
//
//	void AsyncWrite(const char* message, size_t size)
//	{
//		memcpy(_sendBuffer, message, size);
//		asio::async_write(_socket, boost::asio::buffer(_sendBuffer, size),
//			boost::asio::bind_executor(_strand, boost::bind(
//				&ChatSession::OnWrite, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred)));
//	}
//
//	void OnWrite(const boost::system::error_code& err, size_t bytes_transferred)
//	{
//		if (!err)
//		{
//		}
//		else
//		{
//			std::cout << "error code: " << err.value() << ", msg: " << err.message() << std::endl;
//			_room.Leave(this->shared_from_this());
//		}
//	}
//
//	void HandlePacket(char* ptr, size_t size)
//	{
//		asio::mutable_buffer buffer = asio::buffer(ptr, size);
//		int offset = 0;
//		PacketHeader header;
//		PacketUtil::ParseHeader(buffer, &header, offset); // 사이즈가 안맞으면 끝내버리고 맞으면 이어서 진행
//		std::cout << "HandlePacket " << chat::MessageCode_Name(header.Code) << std::endl; 
//		switch (header.Code) // 패킷 코드를 통해서 직렬화 방식을 파악, 직렬화 전 protobuf를 통해 패킷 코드를 생성한 주된 이유는 많아지는 패킷 종류에도 좀 더 알기쉬운 코드를 작성할 수 있게 돕는다.
//		{
//		case chat::MessageCode::LOGIN_REQ:
//			HandleLoginReq(buffer, header, offset);
//			break;
//		case chat::MessageCode::LIST_REQ:
//			HandleListReq(buffer, header, offset);
//			break;
//		case chat::MessageCode::CHAT_REQ:
//			HandleChatReq(buffer, header, offset);
//			break;
//		} // 각 패킷 코드 마다 알맞게 파싱하는 기능을 보유한 함수를 실행
//	}
//
//	void HandleLoginReq(asio::mutable_buffer& buffer, const PacketHeader& header, int& offset)
//	{
//		std::cout << "HandleLoginReq" << std::endl;
//		chat::LoginReq pbMsg;
//		PacketUtil::Parse(pbMsg, buffer, header.Length, offset);
//		std::cout << pbMsg.name(0) << std::endl;
//		SetName(pbMsg.name(0));
//
//		chat::LoginRes pbRes; // 클라이언트에게 답장 패킷 생성
//		pbRes.set_result(true); 
//		const size_t requiredSize = PacketUtil::RequiredSize(pbRes);
//
//		char* rawBuffer = new char[requiredSize];
//		auto sendBuffer = asio::buffer(rawBuffer, requiredSize);
//		PacketUtil::Serialize(sendBuffer, chat::MessageCode::LOGIN_RES, pbRes);
//		this->Send(sendBuffer); // 결과값 전송
//	}
//
//	void HandleListReq(asio::mutable_buffer& buffer, const PacketHeader& header, int& offset)
//	{
//		std::cout << "HandleListReq" << std::endl;
//		_room.SendList(this->shared_from_this()); // list전송
//	}
//
//	void HandleChatReq(asio::mutable_buffer& buffer, const PacketHeader& header, int& offset)
//	{
//		std::cout << "HandleChatReq" << std::endl;
//		chat::ChatReq pbReq;
//		PacketUtil::Parse(pbReq, buffer, header.Length, offset);
//
//		chat::ChatNoti pbNoti;
//		auto m = pbNoti.add_message();
//		*m = pbReq.message(0); // 메세지 내용, repeated로 저장되있기 때문에 0번째 string을 꺼내고
//
//		auto s = pbNoti.add_sender();
//		*s = this->GetName(); // 누가 저장했는지도 파악한다음
//		const size_t requiredSize = PacketUtil::RequiredSize(pbNoti);
//
//		char* rawBuffer = new char[requiredSize];
//		auto sendBuffer = asio::buffer(rawBuffer, requiredSize); // 다시 래핑(직렬화)해서 전송
//		PacketUtil::Serialize(sendBuffer, chat::MessageCode::CHAT_NOTI, pbNoti);
//		_room.Broadcast(sendBuffer);
//	}
//
//private:
//	tcp::socket _socket;
//	const static int RecvBufferSize = 1024;
//	char _recvBuffer[RecvBufferSize];
//	const static int SendBufferSize = 1024;
//	char _sendBuffer[SendBufferSize];
//	ChatRoom& _room;
//	boost::asio::strand<boost::asio::io_context::executor_type> _strand;
//	std::string _name;
//};
//
//typedef boost::shared_ptr<ChatSession> ChatSessionPtr;
//
//class ChatServer
//{
//public:
//	ChatServer(asio::io_context& io_context, int port)
//		: _acceptor(io_context, tcp::endpoint(tcp::v4(), port))
//		, _io_context(io_context)
//	{
//	}
//	void StartAccept()
//	{
//		ChatSession* session = new ChatSession(_io_context, _room);
//		ChatSessionPtr sessionPtr(session);
//		_acceptor.async_accept(sessionPtr->GetSocket(),
//			boost::bind(&ChatServer::OnAccept, this, sessionPtr, asio::placeholders::error));
//	}
//protected:
//	void OnAccept(ChatSessionPtr session, boost::system::error_code ec)
//	{
//		if (!ec)
//		{
//			std::cout << "Connected " << std::endl;
//			session->Start();
//		}
//		StartAccept();
//	}
//private:
//	tcp::acceptor _acceptor;
//	asio::io_context& _io_context;
//	ChatRoom _room;
//};
//
//typedef boost::shared_ptr<ChatServer> ChatServerPtr;
//
//int main()
//{
//	try
//	{
//		constexpr int port = 4242;
//		boost::asio::io_context io_context;
//		ChatServer s(io_context, port);
//		s.StartAccept();
//		std::cout << "server start " << port << std::endl;
//		io_context.run();
//	}
//	catch (std::exception& e)
//	{
//		std::cerr << "Exception: " << e.what() << "\n";
//	}
//}