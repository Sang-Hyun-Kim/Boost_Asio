// BoostAsio의 Async 비동기 기능을 활용한 ChatClient
#include <iostream>
#include <boost/asio.hpp>
#include <boost/asio/buffer.hpp>
class ChatClient
{
public:
	ChatClient(boost::asio::io_context& io_context) : _socket(io_context)
	{
		memset(_recvBuffer, 0, RecvBufferSize);
	}
	~ChatClient()
	{
	}

	void Connect(std::string host, int port)
	{
		const boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::make_address(host), port);
		_socket.async_connect(endpoint, [this](const boost::system::error_code& err) {this->OnConnect(err); });
	}
	void AsyncWrite(std::string msg)
	{
		_sendMsg = msg;
		boost::asio::async_write(_socket, boost::asio::buffer(_sendMsg), [this](const boost::system::error_code& err, const
			size_t bytes_transferred)
			{this->OnWrite(err, bytes_transferred); });
	}
private:
	void OnConnect(const boost::system::error_code& err)
	{
		if (!err)
		{
			std::cout << "Onconnect" << std::endl;
			AsyncRead();
		}
		else
		{
			std::cout << "Onconnect Fail" << std::endl;
		}
	}
	// Onwrite, OnRead=>읽고쓰기가 완료된 것을 말해주는 함수
	void OnWrite(const boost::system::error_code & err, size_t bytes_transferred)
	{
		if (!err)
		{
			std::cout << "OnWrite " << bytes_transferred << std::endl;
		}
		else
		{
			std::cout << "error code: " << err.value() << ", msg: " << err.message() << std::endl;
		}
	}
	void OnRead(const boost::system::error_code err, size_t size)
	{
		// 메세지 출력, _recvBuffer에서 size만큼만 복사해서 출력
		std::string msg(_recvBuffer, size);
		std::cout << "OnRead size: " << size << ", msg: " << msg.c_str() << std::endl;
		if (!err)
		{
			AsyncRead();
		}
		else
		{
			std::cout << "error code: " << err.value() << ", msg: " << err.message() << std::endl;
		}
	}
	void AsyncRead()
	{
		memset(_recvBuffer, 0, RecvBufferSize);
		_socket.async_read_some(
			boost::asio::buffer(_recvBuffer, RecvBufferSize), [this](const boost::system::error_code& err, const size_t
				bytes_transferred)
			{this->OnRead(err, bytes_transferred); });
	}
private:
	static const int RecvBufferSize = 1024;
	boost::asio::ip::tcp::socket _socket;
	char _recvBuffer[RecvBufferSize];
	std::string _sendMsg;
};
int main()
{
	boost::asio::io_context io_context;
	ChatClient client(io_context);
	client.Connect(std::string("127.0.0.1"), 4242);

	// 기존 작업 환경을 싱글 스레드라 Read()를 하면서 Io_context.run()호출이 어려움
	// run() 과 읽고쓰기를 동시에 처리하기 위해
	// thread를 추가해줌으로써 가능하게함 해당 스레드는 run()을 별도로 실행 함으로 써 대기중인 비동기 함수들을 수행하게됨 
	std::thread t([&io_context]() {io_context.run(); });

	char line[256]; // 메세지를 입력받아 그대로 쓰기 호출=>복사해서 buffer() 에 넣어 서버로 전송 
	while (std::cin.getline(line, 256))
	{
		client.AsyncWrite(std::string(line));
		memset(line, 0, 256);
	}
	t.join();// thread가 작동중일때 스레드가 호출된 스레드(지금은 main())을 끝내지 마라
}