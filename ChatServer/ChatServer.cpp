#include <iostream>
#include <set>
#include <boost/asio.hpp>
#include <boost/asio/buffer.hpp>
using boost::asio::ip::tcp;
using namespace boost;
using namespace std;
class ChatMember
{
public:
	ChatMember() {}
	virtual ~ChatMember() {}
	virtual void SendChat(const std::string& msg) = 0; // 채팅 보내기용 가상 함수
};
using ChatMemberPtr = std::shared_ptr<ChatMember>; // using으로 shared_ptr<> 대체 제작

class ChatRoom
{
public:
	void Join(ChatMemberPtr member) // 서버에 접속시 Join을 통해 접속
	{
		Broadcast(std::string("We have new member.")); // => 멤버 목록에 등록된 모든 대상에게 메세지를 전송
		_members.insert(member);// 멤버 목록에 멤버를 등록
	}

	// 멤버목록 제거 후 남은 사람들에게 통보
	void Leave(ChatMemberPtr member)
	{
		_members.erase(member);
		Broadcast(std::string("A member left."));
	}
	
	// 멤버 목록에 등록된 모든 대상에게 메세지를 전송
	void Broadcast(const std::string& msg)
	{
		cout <<"----  start ----" << endl;
		for (const auto member : _members)
			member->SendChat(msg);
		cout <<"---- debug end ----" << endl;
	}
private:
	std::set<ChatMemberPtr> _members;
};

// ChatMember를 상속 => 클라이언트와 서버가 소통을 하면서 Chat 시스템을 사용하기 위함
class ChatSession : public ChatMember, public std::enable_shared_from_this<ChatSession>
{
public:
	ChatSession(asio::io_context& io_context, ChatRoom& room) : _socket(io_context), _room(room)
	{
		memset(_recvBuffer, 0, RecvBufferSize);
		memset(_sendBuffer, 0, SendBufferSize);
	}
	void Start()
	{
		_room.Join(this->shared_from_this());// 클라이언트가 처음 서버에 접속하면 자기 자신(내 클라이언트를) Room에 참가시킴
		AsyncRead(); // 클라이언트와 서버 간의 Read() 사용
		// sharedPtr로 인자를 받아야하는 Join() 함수인자에 일반적인 shared_ptr<세션>(this)를 넣어주면 안된다.
		// 새로 생성해서 넣어주기 때문에 같은 메모리를 두 개를 이용해서 관리=> 둘다 소멸이 되지 않는 문제가 발생함, 이는 RefCount가 새롭게 생성되며 RefCount를 다수 가지게 된 메모리는 누수 문제가 필연적으로 생길 수 밖에 없어진다.
		// 이때 enable_shared_from_this<클래스자기자신> => 현재 객체에 대해서 내부적으로 shared_ptr의 복사본을 생성해서 넘겨줌. 복사해서 넘겨주기 때문에 RefCount가 정상적으로 작동함
		// 내부적으로 weak_ptr을 생성해서 lock()을 넘겨줌, shared_ptr을 사용 할 때에는 단순 this는 절대 사용하지 않도록 해야한다.
	}
	tcp::socket& GetSocket()
	{
		return _socket;
	}
	void SendChat(const std::string& msg) override
	{
		std::cout << "SendChat " << msg.c_str() << std::endl;
		AsyncWrite(msg.c_str(), msg.size());
	}
protected:
	void AsyncRead()
	{
		memset(_recvBuffer, 0, RecvBufferSize);
		_socket.async_read_some(
			boost::asio::buffer(_recvBuffer, RecvBufferSize),
			[this](boost::system::error_code error, size_t bytes_transferred) {this->OnRead(error, bytes_transferred); }
		);
	}
	void OnRead(const boost::system::error_code err, size_t size)
	{
		std::cout << "OnRead " << size << std::endl;
		if (!err)
		{
			_room.Broadcast(std::string(_recvBuffer, size));
			AsyncRead();
		}
		else
		{
			std::cout << "error code: " << err.value() << ", msg: " << err.message() << std::endl;
			_room.Leave(this->shared_from_this());
		}
	}
	void AsyncWrite(const char* message, size_t size)
	{
		memcpy(_sendBuffer, message, size);
		asio::async_write(_socket, boost::asio::buffer(_sendBuffer, size), [this](boost::system::error_code error, size_t
			bytes_transferred)
			{this->OnWrite(error, bytes_transferred); });
	}
	void OnWrite(const boost::system::error_code& err, size_t bytes_transferred)
	{
		if (!err)
		{
		}
		else
		{
			std::cout << "error code: " << err.value() << ", msg: " << err.message() << std::endl;
			_room.Leave(this->shared_from_this());
		}
	}
private:
	tcp::socket _socket;
	const static int RecvBufferSize = 1024;
	char _recvBuffer[RecvBufferSize];
	const static int SendBufferSize = 1024;
	char _sendBuffer[SendBufferSize];
	ChatRoom& _room;
};
using ChatSessionPtr = std::shared_ptr<ChatSession>;

class ChatServer
{
public:
	ChatServer(asio::io_context& io_context, int port) : _acceptor(io_context, tcp::endpoint(tcp::v4(), port))
		, _io_context(io_context)
	{
	}
	void StartAccept()
	{
		ChatSession* session = new ChatSession(_io_context, _room); // 세션 생성 및 acceptor 등록
		ChatSessionPtr sessionPtr(session); // 아래와차이점 : new로 ChatSession 에 대한 메모리를 동적할당 한뒤에 RefCountblock도 따로 생성해줄때 메모리에 새롭게 생성(new)함  
		// new를 사용해 session을 만들어 준다음 새롭게 new를 사용해 sessionPtr을 생성하는 것
		// 평소처럼 아래의 구문을 사용할 경우 두번의 new 과정(동적할당)을 한번에 수행함. 차이점 및 다른 방법을 참고하라(성능적으론 아래가 더 나음)
		// std::shared_ptr<ChatSession> a = std::make_shared<ChatSession>(_io_context,_room);
		// ChatSessionPtr& sessionpptr = sessionPtr; 이러한 함수 인자 전달 방식은 복사가 아닌 주소값 전달이기 때문에 RefCount가 증가 하지 않음
		// 이때문에 한번 복사해서 함수로 넘겨주는 방식이 필요함 만약 RefCount 가 증가되자 않은 상태의 멀티스레드 환경에서 인자로 넘겨주는 와중에 소멸이 일어나면 어떤 문제가 일어날지 모름
		_acceptor.async_accept(sessionPtr->GetSocket(), [this, sessionptr = sessionPtr](boost::system::error_code error)
			{this->OnAccept(sessionptr, error); });
	}
protected:
	void OnAccept(ChatSessionPtr session, boost::system::error_code ec)
	{
		if (!ec)
		{
			std::cout << "Connected " << std::endl;
			session->Start(); // 방에 입장
		}
		StartAccept(); // 다음 클라이언트를 위해 입장 대기 상태로 만들기
	}
private:
	tcp::acceptor _acceptor;
	asio::io_context& _io_context;
	ChatRoom _room;
};
typedef boost::shared_ptr<ChatServer> ChatServerPtr;

// 여기서 알 수 있는 smart포인터의 장점: 기존의 포인터 방식은 Session 같은 경우 다 사용했는지 체크해주며 소멸시켜줬어야했는데
// 이젠 알아서 소멸 및 체크가 가능해짐

int main()
{
	try
	{
		int port = 4242;
		boost::asio::io_context io_context;
		ChatServer s(io_context, port);
		s.StartAccept();
		std::cout << "server start " << port << std::endl;
		io_context.run();
	}
	catch (std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << "\n";
	}
}