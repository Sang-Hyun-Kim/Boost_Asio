#include <iostream>
#include <boost/asio.hpp> 

using boost::asio::ip::tcp; // 코드 축약용
using namespace boost;

int main()
{
    boost::asio::io_context io_context;
    tcp::endpoint ep = tcp::endpoint(tcp::v4(), 4242); // Echoclient의 endpoint와 변경점 소켓 생성후 IPV4로 설정한 뒤 NIC에서 사용 가능한 주소를 자동으로 할당합니다
    std::cout << "server start in " << ep.port() << std::endl; // 서버 포트번호 출력 가능ㅎ

    while (true)
    {
        tcp::acceptor acceptor(io_context, ep); // 기존 소켓프로그래밍의 listen에 해당
        tcp::socket socket(io_context); // 소켓 생성
        system::error_code ec; 

        acceptor.accept(socket,ec);// 승인, 동기함수는 해당 부분에서 클라이언트의 요청이 올때까지 멈춤
        if (!ec) {
            std::cout << "accepted" << std::endl;
        }
        else {
            std::cout << "accept error: " << ec.message() << std::endl;
            return 0;
        }
        while (true)
        {
            char recvBuffer[32] = { 0, }; // EchoClient 코드처럼 Vector로도 할당 가능하며 배열로도 할당 가능하다.
            size_t receivedSize = socket.read_some(asio::buffer(recvBuffer, 32), ec);

            if (!ec) {
                std::cout << "Recv size: " << receivedSize << std::endl;
            }
            else {
                std::cout << "Recv error: " << ec.message() << std::endl;
                break;
            }

            size_t sentSize = socket.write_some(boost::asio::buffer(recvBuffer, 32), ec);

            if (!ec) {
                std::cout << "Sent size: " << sentSize << "\n";
            }
            else {
                std::cout << "Send error: " << ec.message() << std::endl;
                break;
            }
        }

    }

    return 0;
}
