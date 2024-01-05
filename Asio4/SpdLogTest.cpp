#include <iostream>
#include <ostream>
#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/rotating_file_sink.h"
#include "spdlog/sinks/daily_file_sink.h"


void basic_logfile_example()
{
	try
	{
		auto logger = spdlog::basic_logger_mt("basic_logger", "logs/basic-log.txt");// logger 이름, 로그 파일 명칭으로 저장

		logger->debug("Welcome to spdLog");
		logger->info("this is info");
		logger->error("this is error");
	}
	catch(const spdlog::spdlog_ex& ex)
	{
		std::cout << "Log init failed: " << ex.what() << std::endl;
	}
}

void rotating_example()
{
	auto max_size = 1048576 * 1;
	auto max_files = 3;
	auto logger = spdlog::rotating_logger_mt("some_logger_name", "logs/rotating.txt",max_size, max_files);

	for (int i = 0; i < 10000; ++i)
	{
		logger->info("{:d} Sinks are the objects that actuallyt write the log to their target. Each sink should be responsible for only single targer (e.g file, console, db), and each sink has its own private instance of formatter object.", i);
	} // max_size 가 넘어가는 파일이 될 경우 다음 파일을 생성해서 다음 로그를 기록(rotating)
}

int main()
{
	spdlog::info("Welcom to spdlog!");
	spdlog::error("Some error message with arg: {}", 1);

	spdlog::warn("Easy padding in numbers like {:08d}", 12);
	spdlog::critical("Support for int: {0:d}; hex:{0:x}; oct:{0:o}; bin: {0:b}",42);
	spdlog::info("Support for floats {:03.2f}", 1.23456);
	spdlog::info("Positional args are {1} {0}...","too","supported");
	spdlog::info("{:<30}", "left aligned");
	
	spdlog::set_level(spdlog::level::debug); // 레벨 설정이 debug가 아니면 밑의 문구가 출력되지않음
	spdlog::debug("This message should be displayed.."); // 특정 레벨 설정후 해당 레벨 이하는 출력 되지않게 만들 수 있음


	basic_logfile_example();
	rotating_example();
	return 0;
}