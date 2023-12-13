#include <iostream>
#include <boost/asio.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/bind/bind.hpp>

using boost::asio::ip::tcp;
using namespace boost;

// ������ ���� Ŭ���̾�Ʈ�� �����͸� �ְ�ޱ� ���� ���� ����
class TcpSession
{
public:
	TcpSession(asio::io_context& io_context) : _socket(io_context)
	{
		memset(_recvBuffer, 0, RecvBufferSize);
		memset(_sendBuffer, 0, SendBufferSize);
	}
	void Start() // ���� ����, ������ ������� ����=> client�� ������ ���ؼ������� ���� ����, ������ ������ ���ΰ� Read()�� ������ ����
	{
		AsyncRead(); // �ٷ� Read() ����
	}

	// �ش� ���ǿ� ��ϵ� ���� ��ȯ
	tcp::socket& GetSocket()
	{
		return _socket;
	}
protected:
	void AsyncRead()
	{
		_socket.async_read_some(
			boost::asio::buffer(_recvBuffer, RecvBufferSize),
			[this](const boost::system::error_code& err, const size_t bytes_transferred) {this->OnRead(err,
				bytes_transferred); });
	}
	void OnRead(const boost::system::error_code err, size_t size)
	{
		std::cout << "OnRead " << size << ", " << _recvBuffer << std::endl;
		if (!err)
		{
			AsyncRead();
			AsyncWrite(_recvBuffer, size);
		}
		else
		{
			std::cout << "Error: " << err.message() << "\n";
		}
	}
	void AsyncWrite(char* message, size_t size)
	{
		memcpy(_sendBuffer, message, size);
		asio::async_write(_socket, boost::asio::buffer(_sendBuffer, SendBufferSize),
			[this](const boost::system::error_code& err, const size_t bytes_transferred) {this->OnWrite(err,
				bytes_transferred); });
	}
	void OnWrite(const boost::system::error_code& err, size_t bytes_transferred)
	{
		std::cout << "OnWrite " << bytes_transferred << std::endl;
		if (!err)
		{
		}
		else
		{
			std::cout << "Error: " << err.message() << "\n";
		}
	}
private:
	tcp::socket _socket;
	const static int RecvBufferSize = 1024;
	char _recvBuffer[RecvBufferSize];
	const static int SendBufferSize = 1024;
	char _sendBuffer[SendBufferSize];
};

class TcpServer
{
public:
	TcpServer(asio::io_context& io_context, int port) : _acceptor(io_context, tcp::endpoint(tcp::v4(), port))
		, _io_context(io_context)
	{
	}
	void StartAccept()
	{
		TcpSession* session = new TcpSession(_io_context);
		_acceptor.async_accept(session->GetSocket(), [this, session](const boost::system::error_code& err) {this -> OnAccept(session, err); });
	}// �񵿱� �Լ� ������ ������ ���� �񵿱� Acceptor ����
protected:
	void OnAccept(TcpSession* session, boost::system::error_code ec)
	{
		if (!ec)
		{
			std::cout << "Accept " << std::endl;
			session->Start();
		}
		StartAccept();
	}
private:
	tcp::acceptor _acceptor; // ���� ������ acceptor�� ����, IOCP�� �ٸ��� ������ ���ǿ��� bind�ϰ� �����;��ϴµ� �׷� ������ ����
	asio::io_context& _io_context;
};
int main(int argc, char* argv[])
{
	boost::asio::io_context io_context;
	TcpServer server(io_context, 4242);
	server.StartAccept();
	std::cout << "server start" << std::endl;																		   
	io_context.run();
	return 0;
}
