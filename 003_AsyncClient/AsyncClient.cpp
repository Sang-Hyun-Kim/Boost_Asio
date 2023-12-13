#include <iostream>
#include <boost/asio.hpp> 

// �񵿱� Ŭ���̾�Ʈ�� �����ϴ� ����
// ���� ����, ��Ʈ ��ũ�� ���� ����ó���� �����԰� ���ÿ� ���� ������ ���ÿ� �����ؾ��ϱ� ������

// �񵿱� ��� ���� �� �ڽ��� ������°� ������ ������ ���
//  
class TcpClient
{
public:
	TcpClient(boost::asio::io_context& io_context) : _socket(io_context) // ���Ͽ��� io_context ��ü�� ������ �䱸�ϱ� ������ �޾ƿ;���
	{
		memset(_recvBuffer, 0, RecvBufferSize);
	}

	void Connect(std::string host, int port)
	{
		const boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::make_address(host), port);
		_socket.async_connect(endpoint, [this](const boost::system::error_code& err) {this->OnConnect(err); }); // �׳� connect���� AsyncConnect�� �ٲ�
		// ���ٽ��� ���� async_connect�� �ʿ� ���ڸ� ����=> �����ڵ带 ���ڷ� �޴� �Լ��� ���� ��Ű�µ� �̶� ����Ǵ� �Լ��� OnConnect()�� ���� �Լ��� �츮�� ���� �־� �� �����̴�.
	}
private:
	void OnConnect(const boost::system::error_code& err)
	{
		std::cout << "Onconnect" << std::endl;
		if (!err)
		{
			AsyncWrite("Hello, World!");
			AsyncRead(); // write�� �ʼ��� �ƴ����� Read�� �ʼ��� ������ �������� �б���¸� ���ؿ����� ����������Ѵ�, ������ �����ϸ� Read�� �ؾ��Ѵ�.
		}
	}
	void AsyncWrite(std::string msg)
	{
		// �ش� �Լ��� ȣ��� ���������� _socket�� ������ �Ϸ�� �����̴�. �ش� ���Ͽ� �޼����� �����ִ� ���̴�.
		// �̶� buffer() �Լ��� ũ�⸦ ������� �ʾƵ� �Ǵ� ������ string, vector, boost ���� array�� ��쿡�� ũ�⸦ �߷��ؼ� �˾Ƽ� �ۼ����ش�.
		// �̷��� ������ ����
		_sendMsg = msg;
		boost::asio::async_write(_socket, boost::asio::buffer(_sendMsg), [this](const boost::system::error_code& err, const
			size_t bytes_transferred)
			{this->OnWrite(err, bytes_transferred); });
	}

	void AsyncRead()
	{
		// ���⼭�� _socket ��ü�� Readsome ��, writesome���� ������ ����
		// ���� ���� �����Ϳ� ������ ũ�� ����
		// �̹� ���ô� ���� ����� buffer()�� �߷� �� �� ���� �Ϲ� c��Ÿ�Ϲ迭�� ���÷� ����߱� ������ ����س��� �κ�
		_socket.async_read_some(
			boost::asio::buffer(_recvBuffer, RecvBufferSize), [this](const boost::system::error_code& err, const size_t
				bytes_transferred)
			{this->OnRead(err, bytes_transferred); });
	}

	void OnWrite(const boost::system::error_code& err, size_t bytes_transferred)
	{
		std::cout << "OnWrite " << bytes_transferred << std::endl; // ���� �����߰� ~byte ��ŭ ���¾�
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
			AsyncRead(); // ���⼭ �ٽ� �񵿱� Read()�� �ɾ��ִ� ������ �ѹ� OnRead()�� ������ ���� �ٽ� ������¸� ���� ������ϱ� ������ �ݵ�� �񵿱������ �б� ���(AsyncRead())�� ��ȣ�� ������Ѵ�
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

//	io_context.run(); // ������ �Լ��� �ϳ��� ����
	// IOCP������ Queue�� �� �Ϸ�� ���µ��� run()�� �������� ����
	// Callback()�Լ��� ���� ��� �����ϴٰ� �ѹ��� �����ϸ� ���ϴ� ���� ������ȳ����� ��Ȳ�� �Ȼ��� �� �� �־
	// ȣ�� ������ ��Ȯ�ϰ� �����ϱ� ���� ���
	while (true)
	{
		io_context.poll();
		Sleep(500);
	}
	// run() �� �ݺ����ȿ� ����ִ� poll()�� ������
	// => run()�� ������� ��� �Լ��� �����ϰ�, poll()�� �� �Ѱ��� ������

}