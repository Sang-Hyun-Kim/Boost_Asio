#include <iostream>
#include <boost/asio.hpp> 

// 비동기 클라이언트를 제작하는 이유
// 성능 개선, 네트 워크에 대한 소켓처리를 수행함과 동시에 게임 로직도 동시에 진행해야하기 때문에

// 비동기 방식 연결 후 자신의 연결상태가 수락될 때까지 대기
//  
class TcpClient
{
public:
	TcpClient(boost::asio::io_context& io_context) : _socket(io_context) // 소켓에서 io_context 객체를 무조건 요구하기 때문에 받아와야함
	{
		memset(_recvBuffer, 0, RecvBufferSize);
	}

	void Connect(std::string host, int port)
	{
		const boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::make_address(host), port);
		_socket.async_connect(endpoint, [this](const boost::system::error_code& err) {this->OnConnect(err); }); // 그냥 connect에서 AsyncConnect로 바뀜
		// 람다식을 통해 async_connect의 필요 인자를 만족=> 에러코드를 인자로 받는 함수를 실행 시키는데 이때 실행되는 함수는 OnConnect()란 연결 함수를 우리가 직접 넣어 준 형태이다.
	}
private:
	void OnConnect(const boost::system::error_code& err)
	{
		std::cout << "Onconnect" << std::endl;
		if (!err)
		{
			AsyncWrite("Hello, World!");
			AsyncRead(); // write는 필수가 아니지만 Read가 필수인 이유는 지속적인 읽기상태를 통해연결을 유지해줘야한다, 연결이 시작하면 Read를 해야한다.
		}
	}
	void AsyncWrite(std::string msg)
	{
		// 해당 함수가 호출된 시점에서는 _socket이 연결이 완료된 상태이다. 해당 소켓에 메세지를 보내주는 것이다.
		// 이때 buffer() 함수에 크기를 명시하지 않아도 되는 이유는 string, vector, boost 제공 array에 경우에는 크기를 추론해서 알아서 작성해준다.
		// 이러한 문법에 주의
		_sendMsg = msg;
		boost::asio::async_write(_socket, boost::asio::buffer(_sendMsg), [this](const boost::system::error_code& err, const
			size_t bytes_transferred)
			{this->OnWrite(err, bytes_transferred); });
	}

	void AsyncRead()
	{
		// 여기서는 _socket 자체가 Readsome 함, writesome과의 차이점 유의
		// 받을 버퍼 데이터와 버퍼의 크기 전송
		// 이번 예시는 위에 명시한 buffer()가 추론 할 수 없는 일반 c스타일배열을 예시로 사용했기 때문에 명시해놓은 부분
		_socket.async_read_some(
			boost::asio::buffer(_recvBuffer, RecvBufferSize), [this](const boost::system::error_code& err, const size_t
				bytes_transferred)
			{this->OnRead(err, bytes_transferred); });
	}

	void OnWrite(const boost::system::error_code& err, size_t bytes_transferred)
	{
		std::cout << "OnWrite " << bytes_transferred << std::endl; // 전송 성공했고 ~byte 만큼 보냈어
		if (!err)
		{
		}
		else
		{
			std::cout << "Error: " << err.message() << "\n";
		}
	}
	void OnRead(const boost::system::error_code err, size_t size)
	{
		std::cout << "OnRead " << size << ", " << _recvBuffer << std::endl;
		if (!err)
		{
			AsyncRead(); // 여기서 다시 비동기 Read()를 걸어주는 이유는 한번 OnRead()가 끝나고 나면 다시 연결상태를 유지 해줘야하기 때문에 반드시 비동기상태의 읽기 기능(AsyncRead())을 재호출 해줘야한다
			using namespace std::chrono_literals;
			std::this_thread::sleep_for(500ms);
			AsyncWrite("Hello, World!22222");
		}
		else
		{
			std::cout << "Error: " << err.message() << "\n";
		}
	}


	static const int RecvBufferSize = 1024;
	boost::asio::ip::tcp::socket _socket;
	char _recvBuffer[RecvBufferSize];
	std::string _sendMsg;
};

int main()
{
	boost::asio::io_context io_context;
	TcpClient client(io_context);
	client.Connect(std::string("127.0.0.1"), 4242);

//	io_context.run(); // 지정된 함수를 하나씩 실행
	// IOCP에서의 Queue에 들어간 완료된 상태들을 run()이 꺼내오는 느낌
	// Callback()함수가 전부 사용 가능하다고 한번에 실행하면 원하는 실행 결과가안나오는 상황이 안생길 수 도 있어서
	// 호출 시점을 명확하게 구분하기 위해 사용
	while (true)
	{
		io_context.poll();
		Sleep(500);
	}
	// run() 과 반복문안에 들어있는 poll()의 차이점
	// => run()은 대기중인 모든 함수를 실행하고, poll()은 단 한개만 실행함

}