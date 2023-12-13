#include <iostream>
#include <boost/asio.hpp> // 부스트 헤더를 프로젝트 인클루드 + 워닝 뜨는 _WIN32_WINNT=0x0601를 프로젝트 속성, C++/전처리기/전처리기 정의에 넣고 저장
// 넣는 법은 Boost 압축파일을 다운로드 받고 해제한 파일 위치를 프로젝트 속성->VC++ -> 포함디렉토리에 넣기

int main()
{
	boost::asio::io_context io_context;// io_context 객체
	boost::asio::ip::tcp::socket socket(io_context);
	boost::system::error_code ec;
	//	boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::make_address("45.79.112.203", ec), 4242); // 주소, 에러코드, 포트번호 // callback 해주는 에코 서버 tcpbin.com 사이트 사용
	boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::make_address("127.0.0.1", ec), 4242);
	if (ec)// null이 아니면 에러가 있는 것
	{
		std::cout << "endpoint error: " << ec.message() << std::endl;
		return 0;
	}
	socket.connect(endpoint,ec);

	if (!ec)
	{// 정상 코드
		std::cout << "connected!"<< std::endl;
	}
	else
	{// 비정상코드
		std::cout << "endpoint error: " << ec.message() << std::endl;
		return 0;
	}
	// 메세지 전송
	while (true)
	{
		std::string request = "Hello, World! and How are you?\n";// tcp bin 사이트규칙이 \n을 붙이는것
		socket.write_some(boost::asio::buffer(request.data(), request.size()), ec);
		// asio::buffer로 래핑
		// boost::asio::buffer는 데이터의 주소와 크기를 Asio 라이브러리에서 사용할 수 있는 형태로 변환하는 역할
		// 운영체제 별로 다른 버퍼 운용이 달라(버퍼 할당 방식, 크기와 사용), 크로스 플랫폼 환경을 보완하기위한 Asio 자체의 버퍼
		using namespace std::chrono_literals;
		std::this_thread::sleep_for(200ms);

		size_t size = socket.available(); // 바이트 사이즈 반환, 소켓의 입력버퍼에 현재 얼마만큼의 공간이 있는지 미리 알 수 있게 반환
		std::cout << "read available " << size << std::endl;

		std::vector<char>  recvBuffer(size); // 저장할 수 있는 만큼의 데이터를 저장
		socket.read_some(boost::asio::buffer(recvBuffer.data(), recvBuffer.size()), ec);

		for (auto c : recvBuffer)
		{
			std::cout << c;
		}
	}

	
	std::cout << "Terminated\n" << std::endl;

}
// 에러코드는 네트워크 프로그래밍 에러코드가 호출됨 MSDN에도 나옴
