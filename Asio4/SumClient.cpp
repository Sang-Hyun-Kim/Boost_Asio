//#include <iostream>
//#include <boost/asio.hpp>
//#include <boost/asio/buffer.hpp>
//#include <boost/bind/bind.hpp>
//#include <boost/algorithm/string/split.hpp>
//#include<boost/algorithm/string/classification.hpp>
//#include "Packet.h"
//#include "sumproto.pb.h"
//
//static boost::asio::chrono::milliseconds gTimerDelay = boost::asio::chrono::milliseconds(10);
//const int MaxCount = 1000;// 테스트를 위한 패킷 수
//
//static int GenerateRandomNumber() // 클라이언트가 보낼 랜덤한 수
//{
//	return rand() % 10 + 1;
//}
//
//class MyCircularBuffer
//{
//public:
//	MyCircularBuffer() :_offset(0)
//	{
//		_buffer = new char[MaxBufferSize];
//		memset(_buffer, 0, MaxBufferSize);
//	}
//
//	~MyCircularBuffer()
//	{
//		delete[] _buffer;
//		_buffer = nullptr;
//	}
//
//	char* Allocate(size_t size)
//	{
//		if (_offset + size >= MaxBufferSize)
//			_offset = 0;// 링형 버퍼: 끝까지 도달하면 다시 처음부터 덮어씌워 사용
//		
//		char* ptr = _buffer + _offset;
//		_offset += size;
//		return ptr;
//	}
//
//private:
//	static constexpr int MaxBufferSize = 1024;
//	char* _buffer;
//	size_t _offset;
//};
//
//class ChatClient
//{
//public:
//	ChatClient(boost::asio::io_context& io_context) : _socket(io_context), _timer1(io_context,gTimerDelay), _sum(0),_count(0)
//	{
//		memset(_recvBuffer, 0, RecvBufferSize);
//		_timer1.async_wait(
//			boost::bind(&ChatClient::OnTimer, this)
//		);
//	}
//	~ChatClient()
//	{
//
//	}
//	void Connect(std::string host, int port)
//	{
//		boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::make_address(host), port);
//		_socket.async_connect(endpoint, boost::bind(&ChatClient::OnConnect, this, boost::asio::placeholders::error));
//	}
//
//	void AsyncWrite(std::string msg)
//	{
//		_sendMsg = msg;
//		boost::asio::async_write(_socket, boost::asio::buffer(_sendMsg),
//			boost::bind(&ChatClient::OnWrite, this, boost::asio::placeholders::error,
//				boost::asio::placeholders::bytes_transferred));
//	}
//
//	void AsyncWrite(asio::mutable_buffer& buffer)
//	{
//		boost::asio::async_write(_socket, buffer,
//			boost::bind(&ChatClient::OnWrite, this, boost::asio::placeholders::error,
//				boost::asio::placeholders::bytes_transferred));
//	}
//private:
//	void OnConnect(const boost::system::error_code& err)
//	{
//		std::cout << "OnConnect" << std::endl;
//		if (!err)
//		{
//			AsyncRead();
//		}
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
//		}
//	}
//
//	void OnRead(const boost::system::error_code err, size_t size)
//	{
//		std::string msg(_recvBuffer, size);
//		std::cout << "OnRead size: " << size << std::endl;
//		if (!err)
//		{
//			HandlePacket(_recvBuffer, size); // 서버에서 보내준 패킷 확인
//			AsyncRead();
//		}
//		else
//		{
//			std::cout << "error code: " << err.value() << ", msg: " << err.message() << std::endl;
//		}
//	}
//
//	void AsyncRead()
//	{
//		memset(_recvBuffer, 0, RecvBufferSize);
//		_socket.async_read_some(
//			boost::asio::buffer(_recvBuffer, RecvBufferSize),
//			boost::bind(
//				&ChatClient::OnRead, this,
//				boost::asio::placeholders::error,
//				boost::asio::placeholders::bytes_transferred
//			)
//		);
//	}
//	void OnTimer()
//	{
//		std::cout << "OnTimer" << std::endl;
//		pbsum::Factor pbMsg;
//		pbMsg.set_num(GenerateRandomNumber()); // 1~10까지 랜덤한 수 생성
//		// 링버퍼를 통해서 버퍼에 할당
//		const size_t requiredSize = PacketUtil::RequiredSize(pbMsg);
//		auto buffer = asio::buffer(_ringBuffer.Allocate(requiredSize), requiredSize);
//		if (!PacketUtil::Serialize(buffer, pbsum::MessageCode::FACTOR, pbMsg))
//		{
//			// todo: error
//		}
//		
//		AsyncWrite(buffer);
//		// 직렬화 후 전송
//		_sum += pbMsg.num();// 내부적으로 sum값 더해주기
//		++_count;// count는 천번이고 그 이하 만큼 보냈다면
//		if (_count < MaxCount)
//		{
//			_timer1.expires_at(_timer1.expiry() + gTimerDelay);
//			_timer1.async_wait(boost::bind(&ChatClient::OnTimer, this)); // 계속 전송을 위해 OnTimer 실행
//		}
//		else
//		{ // 천번 부터
//			pbsum::Local_sum pbParity; // 패킷을 보낸만큼 직접 계산한 값을 LocalSum이라 했을때
//			pbParity.set_sum(_sum);
//			const size_t requiredSize = PacketUtil::RequiredSize(pbParity);
//			auto buffer = asio::buffer(_ringBuffer.Allocate(requiredSize), requiredSize);
//			if (!PacketUtil::Serialize(buffer, pbsum::MessageCode::LOCAL_SUM, pbParity))
//			{
//				// todo: error
//			}
//			AsyncWrite(buffer); // 클라가 직접 더한 LocalSum을 전송 이때 Factor의 총합(서버에게 보낸 값)과 LocalSum의 총합이 같다면 패킷이 누락되지 않는다라는 것을 알 수 있음
//		}
//	}
//	void HandlePacket(char* ptr, size_t size)
//	{
//		asio::mutable_buffer buffer = asio::buffer(ptr, size);
//		int offset = 0;
//		PacketHeader header;
//		PacketUtil::ParseHeader(buffer, &header, offset);
//		
//
//		std::cout << "HandlePacket " << pbsum::MessageCode_Name(header.Code) << std::endl;
//
//	}
//
//	static const int RecvBufferSize = 1024;
//
//	boost::asio::ip::tcp::socket _socket;
//	char _recvBuffer[RecvBufferSize];
//	std::string _sendMsg;
//	boost::asio::steady_timer _timer1;
//	int _sum;
//	int _count;
//	MyCircularBuffer _ringBuffer;
//};
//
//static void ExecuteCommand(const std::string& line, ChatClient& client) // 이해용 코드
//{
//	typedef std::vector<std::string> Tokens;
//	Tokens tokens;
//	boost::split(tokens, line, boost::is_any_of(" "));
//	const std::string& mainCmd = tokens[0];
//}
//
//int main()
//{
//	boost::asio::io_context io_context;
//	ChatClient client(io_context);
//	client.Connect(std::string("127.0.0.1"), 4242);
//	std::thread t(boost::bind(&boost::asio::io_context::run, &io_context));
//	// recieve를 구동하는 스레드와 입력을 받는 스레드를 구분 => 입력을 원할하게 만든다.
//	char line[256] = { 0, };
//	while (std::cin.getline(line, 256))
//	{
//		ExecuteCommand(line, client);
//		memset(line, 0, 256);
//	}
//	t.join();
//}