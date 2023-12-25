//#include <google/protobuf/message.h>
//#include <boost/asio.hpp>
//#include <boost/asio/buffer.hpp>
//#include "Protocol.pb.h"
//using namespace boost;
//
//// 패킷 고유 번호 결정
//enum
//{
//	TEST_PACKET = 1,
//};
//
//// 크기, 패킷 고유 번호 부여
//struct PacketHeader
//{
//	short Length; // 길이를 먼저 놓으면 길이 비교를 통한 직렬화/역직렬화 수행을 판단할 때 바로 확인 할 수 있다.(뒤에 놓으면 앞의 데이터의 크기만큼 더 찾는 느린연산 발생)
//	short Code;
//};
//
//// Message : 프로토버프 객체(message)의 최상위 타입(부모)
//class PacketUtil
//{
//public:
//	static bool ParseHeader(const asio::mutable_buffer& buffer, PacketHeader* header, int& offset)
//	{
//		if (buffer.size() <= offset) // offset : 버퍼가 매우 커졌을 때 패킷을 나눠서 한 버퍼에 저장한다, 이때 사용하는 단위를 offset이라고 한다. 즉 버퍼의 사이즈가 offset 보다 작으면 버퍼가 들어갈 수 가 없다는 의미
//			return false;
//
//		const size_t remainedSize = buffer.size() - offset;
//		if (remainedSize < sizeof(PacketHeader))// 버퍼의 사이즈에서 offset 만큼 뺀 것이 PacketHeader의 사이즈 만큼 안나온다면 헤더 역직렬이 불가능하다는 의미
//			return false;
//
//		const char* headerPtr = static_cast<char*>(buffer.data()) + offset;
//		memcpy(header, headerPtr, sizeof(PacketHeader));
//		offset += sizeof(PacketHeader);
//		return true;
//	}
//	static size_t RequiredSize(const google::protobuf::Message& msg)
//	{
//		return sizeof(PacketHeader) + msg.ByteSizeLong(); // 헤더 + 패킷 사이즈 반환
//	}// 패킷 헤더를 미포함 해서 크기를 반환하는 설계 또 한 가능하다(취향)
//
//	static bool Serialize(const asio::mutable_buffer& buffer, const short packetCode, const google::protobuf::Message&
//		msg)
//	{
//		const size_t requiredSize = RequiredSize(msg);
//		if (buffer.size() < requiredSize)
//			return false;
//		PacketHeader header;
//		header.Length = static_cast<short>(msg.ByteSizeLong());
//		header.Code = packetCode;
//		memcpy(buffer.data(), &header, sizeof(PacketHeader));
//		char* payloadPtr = static_cast<char*>(buffer.data()) + sizeof(PacketHeader); // char* 타입은 다른 포인터 처럼 4byte가 아닌 1byte 이므로 
//		// Buffer에 데이터를 담는 시작부분을 정의하는 코드로, PacketHeader의 사이즈만 큼을 띄운다는 의미
//
//		// 해당 코드는 payloadPtr이 PacketHeader 이후 부터 시작, 데이터를 넣는 크기는 전체 사이즈에서 packetHeader의 사이즈를 제거한 크기 만큼만 직렬화하는 의미
//		if (!msg.SerializeToArray(payloadPtr, static_cast<int>(buffer.size()) - sizeof(PacketHeader)))
//			return false;
//		return true;
//	}
//
//	static bool Parse(google::protobuf::Message& msg, const asio::mutable_buffer& buffer, const int payloadSize, int&
//		offset)
//	{
//		// 버퍼의 사이즈를 체크했는데 패킷헤더보다 적게 저장되었다는 것은 이미 패킷 헤더를 파스하지 못한 상태를 의미함
//		if (buffer.size() <= sizeof(PacketHeader))
//			return false;
//		
//		// 버퍼의 시작 지점에서 넘겨 받은 offset의 크기만큼 떨어 진 곳이 패킷 헤더가 끝나고 직렬화되어있는 데이터의 시작 부분
//		const char* payloadPtr = static_cast<char*>(buffer.data()) + offset;
//		const size_t remainedSize = buffer.size() - offset;
//		const bool parseResult = msg.ParseFromArray(payloadPtr, payloadSize); // 해당 부분부터 사이즈 만큼 역직렬화를 시작
//		if (parseResult)
//		{
//			offset += static_cast<int>(msg.ByteSizeLong()); // 버퍼가 저장된 배열 일 경우 뒤의 패킷헤더+패킷도 마저 직렬화 하기 위해 offset 갱신
//			return true;
//		}
//		else
//		{
//			return false;
//		}
//	}
//
//	
//};
//
//int main()
//{
//	Protocol::TEST test;
//	test.set_hp(10);
//	test.set_id(100);
//	
//	//직렬화
//	// 직렬화 하였을때 크기를 체크
//	const size_t requiredSize = PacketUtil::RequiredSize(test);
//	// std::cout << requiredSize << std::endl; // => 4 + 4 + 4 = 12가 나와야 하지만 위의 두 변수의 남는 공간을 최적화 하여 2+ 2+4로 8이 나온다 
//
//	char* rawBuffer = new char[requiredSize]; // 직렬화 하기 위한 버퍼 생성 
//	const auto buffer = asio::buffer(rawBuffer, requiredSize);// Asio 버퍼로 변환
//	PacketUtil::Serialize(buffer, TEST_PACKET, test); //버퍼에 패킷ID 를 넣고 test를 직렬화한 뒤 넣어라
//	// 헤더 붙이고 직렬화 해야하는거 아님? => 직렬화 함수 만들때 헤더를 생성해서 붙임, 이런식으로 자동화된 코드를 만들어라
//
//	// 역직렬화
//	PacketHeader header; 
//	char* PacketBuffer = new char[4];
//	int offset = 0;
//	memcpy(PacketBuffer, rawBuffer, 4);
//	auto combinedBuffer = asio::buffer(PacketBuffer, 4);
//	PacketUtil::ParseHeader(combinedBuffer, &header, offset);
//
//	std::cout << header.Length << std::endl;// 헤더를 제외한 패킷의 크기를 반환 함을 인지
//	std::cout << header.Code << std::endl;
//
//	delete[] PacketBuffer;
//	// 나머지 패킷 부분 가져오기
//	PacketBuffer = new char[4 + header.Length];
//	memcpy(PacketBuffer, rawBuffer, 4 + header.Length); // offset+header.Length로 변환가능
//	// 패킷 헤더를 포함해서 전부 memcpy
//	combinedBuffer = asio::buffer(PacketBuffer, 4);
//	Protocol::TEST test2;
//	PacketUtil::Parse(test2, combinedBuffer, header.Length, offset); //offset 만큼 뛰어넘어 해당 지점부터 역직렬화
//
//	std::cout << test.hp() << std::endl;
//	std::cout << test.id() << std::endl;
//}