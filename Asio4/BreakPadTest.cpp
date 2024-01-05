//#include <iostream>
//#include "client/windows/handler/exception_handler.h"
//
//static void MyTest(std::string& msg, int level)
//{
//	// 메모리 참조 오류
//	*(int*)0x1 = 1; // 오류 발생 코드
//}
//using namespace google_breakpad;
//
//int main()
//{
//	std::wstring prod(L"MMORPG TEST");
//	std::wstring ver(L"1.04");	
//
//	// 크래쉬 덤프 생성시 이름과 버전을 출력하는 구조
//	static google_breakpad::CustomInfoEntry custom_entries[] = {
//		google_breakpad::CustomInfoEntry(L"prod", prod.c_str()), // 어디서 크래쉬가 났는지
//		google_breakpad::CustomInfoEntry(L"ver", ver.c_str()), // 현재 버전은?
//	};
//
//	static google_breakpad::CustomClientInfo custom_info = { custom_entries, 2 };
//	 
//	// 인자
//	//// ExceptionHandler::ExceptionHandler(const wstring& dump_path,
//	//	FilterCallback filter,
//	//	MinidumpCallback callback,
//	//	void* callback_context,
//	//	int handler_types,
//	//	MINIDUMP_TYPE dump_type,
//	//	const wchar_t* pipe_name,
//	//	const CustomClientInfo* custom_info) {
//	// const char* dump_path: 크래시 덤프 파일을 저장할 경로를 나타냅니다.
//	//  FilterCallback filter : 크래시 리포팅을 필터링하기 위한 콜백 함수를 지정할 수 있습니다.
//	//	MinidumpCallback callback : 크래시가 발생했을 때 호출되는 콜백 함수를 지정할 수 있습니다.
//	//  void* callback_context : 콜백 함수에 전달되는 추가적인 컨텍스트 데이터를 지정할 수 있습니다.
//
//	// 덤프 저장 위치(. :현재), 특정 종류의 크래쉬 필터링을 위한 콜백 함수, handler랑 오류가 났을때 어떤 callback 함수를 호출할 것인가, callback 함수 실항시 추가적으로 전달될 데이터, 어떤 오류를 처리 할 것인지, 크래쉬 덤프 종류를 설정(Minidump: 가장 간단한 것)
//	// , pipline: 프로레스 간의 통신 => pipe에 대한 상태 결정=> 운영체제 관련 지식, 사용자 정의의 크래쉬 덤프를 남길 수 있는 인자
//	auto handler = new google_breakpad::ExceptionHandler(
//		L".", nullptr, nullptr, nullptr,
//		google_breakpad::ExceptionHandler::HANDLER_ALL, MiniDumpNormal, (const wchar_t*)nullptr, &custom_info
//	); // 현재 디렉토리에 크래쉬 덤프 생성, 모든 에러를 체크 한 후에 Minidump를 생성하도록 설정하는 함수
//
//
//	auto s = std::string("hello");
//	MyTest(s, 22); // 여기서 1번 주소로 강제 접근 했으니 오류가 날 것
//	// exe 파일 실행시 오류와 함께 dmp 파일(덤프 파일) 생성
//	// 해당 덤프 파일을 visualstudio에서 오픈하면 오류 코드 및 내용이 나옴
//	// 해당 코드를 혼합해서 실행 버튼을 누르면 어떤 코드에서 어떤 호출스택에서 오류가 났는지 까지 보여줌(디버그 모드처럼)
//	std::cout << "HELLO" << std::endl;
//}