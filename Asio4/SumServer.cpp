//#include <iostream>
//#include <set>
//#include <boost/asio.hpp>
//#include <boost/asio/buffer.hpp>
//#include <boost/bind/bind.hpp>
//#include <boost/enable_shared_from_this.hpp>
//#include "Packet.h"
//#include "sumproto.pb.h"
//
//using namespace boost;
//using boost::asio::ip::tcp;
//
//class ChatMember
//{
//public:
//	ChatMember() {}
//	virtual ~ChatMember() {}
//	// virtual void SendChat(const std::string& msg) = 0; // ä�� �Ⱦ��� ����
//	virtual void SetName(const std::string& msg) = 0;
//	virtual std::string GetName() = 0;
//	virtual void Send(asio::mutable_buffer& buffer) = 0;
//};
//typedef boost::shared_ptr<ChatMember> ChatMemberPtr;
//
//class ChatRoom
//{
//public:
//	ChatRoom(asio::io_context& io_context) : _strand(boost::asio::make_strand(io_context)), _globalSum(0), _localTotal(0)
//	{
//
//	}
//
//	void Join(ChatMemberPtr member)
//	{	// asio::post()�� �Ȱ��� ����� ������ dispatch�� �ϰ��� �ְ� �� �ϰ��� ù �ϰ��̸� �����ϴ� ��ɱ��� ������ ������ ��������
//		asio::dispatch(asio::bind_executor(_strand, [this, member]
//			{
//				_members.insert(member);
//			}));
//	}
//	void Leave(ChatMemberPtr member)
//	{
//		asio::dispatch(asio::bind_executor(_strand, [this, member]
//			{
//				_members.erase(member);
//				if (_members.size() == 0)
//				{
//					_globalSum = 0;
//					_localTotal = 0;
//					std::cout << _globalSum << ", " << _localTotal << std::endl;
//				}
//			}));
//	}
//
//	//void BroadCast(const std::string& msg)
//	//{
//	//	std::for_each(_members.begin(), _members.end(),
//	//		[msg](ChatMemberPtr p)
//	//		{
//	//			p->SendChat(msg);
//	//		});
//	//}
//
//	void BroadCast(asio::mutable_buffer& buffer)
//	{
//		std::for_each(_members.begin(), _members.end(),
//			[&buffer](ChatMemberPtr p)
//			{
//				p->Send(buffer);
//			});
//	}
//
//	void Factor(int num) // �Ź� ���� ��Ŷ�� ���� �տ���
//	{
//		asio::dispatch(asio::bind_executor(_strand, [this, num]
//			{
//				_globalSum += num;
//			}));
//	}
//
//	void LocalSum(int num) // Ŭ���̾�Ʈ�� ������ ���� ���� ���� �ջ�
//	{
//		asio::dispatch(asio::bind_executor(_strand, [this, num]
//			{
//				_localTotal += num;
//				std::cout << "GlobalSum: " << _globalSum << std::endl;
//				std::cout << "LocalTotal: " << _localTotal << std::endl;
//			}));
//	}
//
//private:
//	std::set<ChatMemberPtr> _members;
//	boost::asio::strand<boost::asio::io_context::executor_type> _strand;
//	int _globalSum;
//	int _localTotal;
//};
//
//class ChatSession: public ChatMember, public boost::enable_shared_from_this<ChatSession>
//{
//public:
//	ChatSession(asio::io_context& io_context, ChatRoom& room) : _socket(io_context), _room(room), _strand(boost::asio::make_strand(io_context))
//	{
//		memset(_recvBuffer, 0, RecvBufferSize);
//		memset(_sendBuffer, 0, SendBufferSize);
//	}
//	void Start()
//	{
//		_room.Join(this->shared_from_this()); // boost::enable_shared_from_this<ChatSession>
//		AsyncRead();
//	}
//	tcp::socket& GetSocket()
//	{
//		return _socket;
//	}
//	void SetName(const std::string& name) override
//		{
//			_name = name;
//		}
//	std::string GetName() override
//		{
//			return _name;
//		}
////	void SendChat(const std::string& msg) override // ä�� ���� �ƴϴϱ� SendChat ��ŵ
////	{
////		std::cout << "SendChat " << msg.c_str() << std::endl;
////		AsyncWrite(msg.c_str(), msg.size());
////	}
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
//				boost::asio::placeholders::bytes_transferred
//			)
//		);
//	}
//	void OnRead(const boost::system::error_code err, size_t size)
//	{
//		if (!err)
//		{
//			HandlePacket(_recvBuffer, size);
//			AsyncRead();
//		}
//		else
//		{
//			std::cout << "error code : " << err.value() << " , msg: " << err.message() << std::endl;
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
//		while (offset < size) // ���� �ڵ�� �ٸ���, TCP�� ��Ŷ�� �Ϻ��ϰ� �������� ���� �� �ִٴ� ���� ���, ������ ����
//		{
//			PacketHeader header;
//			PacketUtil::ParseHeader(buffer, &header, offset);
//
//			switch (header.Code)
//			{
//			case pbsum::MessageCode::FACTOR:
//				HandleFactor(buffer, header, offset);
//				break;
//			case pbsum::MessageCode::LOCAL_SUM:
//				HandleLocalSum(buffer, header, offset);
//				break;
//			}
//		}
//	}
//	void HandleFactor(asio::mutable_buffer& buffer, const PacketHeader& header, int& offset)
//	{
//		pbsum::Factor pbMsg;
//		PacketUtil::Parse(pbMsg, buffer, header.Length, offset);
//		_room.Factor(pbMsg.num()); //_globalSum�����ֱ�
//	}
//
//	void HandleLocalSum(asio::mutable_buffer& buffer, const PacketHeader& header, int& offset)
//	{
//		pbsum::Local_sum pbMsg;
//		PacketUtil::Parse(pbMsg, buffer, header.Length, offset);
//		_room.LocalSum(pbMsg.sum());
//		
//	}
//
//
//private:
//	tcp::socket _socket;
//	const static int RecvBufferSize = 1024;
//	char _recvBuffer[RecvBufferSize];
//	const static int SendBufferSize = 1024;
//	char _sendBuffer[SendBufferSize];
//
//	ChatRoom& _room;
//	boost::asio::strand<boost::asio::io_context::executor_type> _strand;
//	std::string _name;
//};
//
//typedef boost::shared_ptr<ChatSession> ChatSessionPtr;
//
//class SumServer
//{
//public:
//	SumServer(asio::io_context& io_context, int port)
//		: _acceptor(io_context, tcp::endpoint(tcp::v4(), port))
//		, _io_context(io_context)
//		, _room(io_context)
//	{
//	}
//	void StartAccept()
//	{
//		ChatSession* session = new ChatSession(_io_context, _room);
//		ChatSessionPtr sessionPtr(session);
//		_acceptor.async_accept(sessionPtr->GetSocket(),
//			boost::bind(&SumServer::OnAccept, this, sessionPtr, asio::placeholders::error));
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
//typedef boost::shared_ptr<SumServer> SumServerPtr;
//
//int main()
//{
//	try
//	{
//		constexpr int port = 4242;
//		boost::asio::io_context io_context;
//		SumServer s(io_context, port);
//		s.StartAccept();
//		std::cout << "server start " << port << std::endl;
//		std::thread t1(boost::bind(&boost::asio::io_context::run, &io_context));
//		std::thread t2(boost::bind(&boost::asio::io_context::run, &io_context));
//		io_context.run();
//	}
//	catch(std::exception& e)
//	{
//		std::cerr << "Exception: " << e.what() << "\n";
//	}
//}