#include <iostream>
#include <boost/asio.hpp>
static boost::asio::chrono::milliseconds gTimerDelay = boost::asio::chrono::milliseconds(200);
const int MaxCount = 500;
class Printer
{
public:
	Printer(boost::asio::io_context& io)
		: strand_(boost::asio::make_strand(io)), _timer1(io, gTimerDelay), _timer2(io, gTimerDelay), _count(0)
	{
		_timer1.async_wait(boost::asio::bind_executor(strand_, [this](boost::system::error_code error) {this->Print1(); }));//async_wait=> 지정한 시간이 지나고 콜백함수가 호출되도록 실행
		_timer2.async_wait(boost::asio::bind_executor(strand_, [this](boost::system::error_code error) {this->Print2(); }));
	}
	~Printer()
	{
		std::cout << "Final count is " << _count << std::endl;
	}
	void Print1()
	{
		if (_count < MaxCount)
		{
			std::cout << "Timer 1: " << _count << ", id: " << std::this_thread::get_id() << std::endl;
			++_count;
			_timer1.expires_at(_timer1.expiry() + gTimerDelay); // 완료후 현재 시간을 기준으로 저장된 시간과 딜레이를 더해 다시 딜레이(타이머) 실행(0.2초마다 실행)
			_timer1.async_wait(boost::asio::bind_executor(strand_, [this](boost::system::error_code error) {this->Print1(); }));
		}
	}
	void Print2()
	{
		if (_count < MaxCount)
		{
			std::cout << "Timer 2: " << _count << ", id: " << std::this_thread::get_id() << std::endl;
			++_count;
			_timer2.expires_at(_timer2.expiry() + gTimerDelay);
			_timer2.async_wait(boost::asio::bind_executor(strand_, [this](boost::system::error_code error) {this->Print2(); }));
		}
	}
private:
	boost::asio::strand<boost::asio::io_context::executor_type> strand_; // 일종의 서버 프로그램에서 사용하는 jobQueue와 비슷한 개념
	// 기존 비동기 함수에 사용되는 io_context 내부의 queue 대신 strand에 사용된 비동기 함수들이 보관됨
	// 그리고 객체 하나에 strand 하나고 있고 이것을 스레드 하나가 관리하니까 멀티 스레드에서 싱글 스레드 같은 효과가 발생함
	// 장점: 1. 멀티스레드 환경 개발 편의성 증가 2. 기존 lock방식의 경우 한 스레드가 자원 사용시 다른 스레드가 쉬고있었지만 strand 방식을 통해 strand에 쌓인 것을 실행하는 스레드와 strand에 새로운 동작을 예약하는 스레드로 나뉘어 작업 효율이 상승함 
	// 다중 스레드 환경에서 비동기 작업을 수행하지만 이 작업들이 싱글 스레드처럼 차례로 실행하기 때문에 안전 접근 및 수행 될 수 있다. 이는 멀티스레드 이슈가 없고 속도도 더 빠르다.
	boost::asio::steady_timer _timer1;
	boost::asio::steady_timer _timer2;
	int _count;
};
int main()
{
	boost::asio::io_context io;
	Printer p(io);
	io.run();
	return 0;
}