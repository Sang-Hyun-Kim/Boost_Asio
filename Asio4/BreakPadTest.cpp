//#include <iostream>
//#include "client/windows/handler/exception_handler.h"
//
//static void MyTest(std::string& msg, int level)
//{
//	// �޸� ���� ����
//	*(int*)0x1 = 1; // ���� �߻� �ڵ�
//}
//using namespace google_breakpad;
//
//int main()
//{
//	std::wstring prod(L"MMORPG TEST");
//	std::wstring ver(L"1.04");	
//
//	// ũ���� ���� ������ �̸��� ������ ����ϴ� ����
//	static google_breakpad::CustomInfoEntry custom_entries[] = {
//		google_breakpad::CustomInfoEntry(L"prod", prod.c_str()), // ��� ũ������ ������
//		google_breakpad::CustomInfoEntry(L"ver", ver.c_str()), // ���� ������?
//	};
//
//	static google_breakpad::CustomClientInfo custom_info = { custom_entries, 2 };
//	 
//	// ����
//	//// ExceptionHandler::ExceptionHandler(const wstring& dump_path,
//	//	FilterCallback filter,
//	//	MinidumpCallback callback,
//	//	void* callback_context,
//	//	int handler_types,
//	//	MINIDUMP_TYPE dump_type,
//	//	const wchar_t* pipe_name,
//	//	const CustomClientInfo* custom_info) {
//	// const char* dump_path: ũ���� ���� ������ ������ ��θ� ��Ÿ���ϴ�.
//	//  FilterCallback filter : ũ���� �������� ���͸��ϱ� ���� �ݹ� �Լ��� ������ �� �ֽ��ϴ�.
//	//	MinidumpCallback callback : ũ���ð� �߻����� �� ȣ��Ǵ� �ݹ� �Լ��� ������ �� �ֽ��ϴ�.
//	//  void* callback_context : �ݹ� �Լ��� ���޵Ǵ� �߰����� ���ؽ�Ʈ �����͸� ������ �� �ֽ��ϴ�.
//
//	// ���� ���� ��ġ(. :����), Ư�� ������ ũ���� ���͸��� ���� �ݹ� �Լ�, handler�� ������ ������ � callback �Լ��� ȣ���� ���ΰ�, callback �Լ� ���׽� �߰������� ���޵� ������, � ������ ó�� �� ������, ũ���� ���� ������ ����(Minidump: ���� ������ ��)
//	// , pipline: ���η��� ���� ��� => pipe�� ���� ���� ����=> �ü�� ���� ����, ����� ������ ũ���� ������ ���� �� �ִ� ����
//	auto handler = new google_breakpad::ExceptionHandler(
//		L".", nullptr, nullptr, nullptr,
//		google_breakpad::ExceptionHandler::HANDLER_ALL, MiniDumpNormal, (const wchar_t*)nullptr, &custom_info
//	); // ���� ���丮�� ũ���� ���� ����, ��� ������ üũ �� �Ŀ� Minidump�� �����ϵ��� �����ϴ� �Լ�
//
//
//	auto s = std::string("hello");
//	MyTest(s, 22); // ���⼭ 1�� �ּҷ� ���� ���� ������ ������ �� ��
//	// exe ���� ����� ������ �Բ� dmp ����(���� ����) ����
//	// �ش� ���� ������ visualstudio���� �����ϸ� ���� �ڵ� �� ������ ����
//	// �ش� �ڵ带 ȥ���ؼ� ���� ��ư�� ������ � �ڵ忡�� � ȣ�⽺�ÿ��� ������ ������ ���� ������(����� ���ó��)
//	std::cout << "HELLO" << std::endl;
//}