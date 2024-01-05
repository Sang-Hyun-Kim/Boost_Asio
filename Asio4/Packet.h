#include <google/protobuf/message.h>
#include <boost/asio.hpp>
#include <boost/asio/buffer.hpp>
#include "sumproto.pb.h"

using namespace boost;

// ��Ŷ ���� ��ȣ ����
enum
{
	TEST_PACKET = 1,
};

// ũ��, ��Ŷ ���� ��ȣ �ο�
struct PacketHeader
{
	short Length; // ���̸� ���� ������ ���� �񱳸� ���� ����ȭ/������ȭ ������ �Ǵ��� �� �ٷ� Ȯ�� �� �� �ִ�.(�ڿ� ������ ���� �������� ũ�⸸ŭ �� ã�� �������� �߻�)
	short Code;
};

// Message : ��������� ��ü(message)�� �ֻ��� Ÿ��(�θ�)
class PacketUtil
{
public:
	static bool ParseHeader(const asio::mutable_buffer& buffer, PacketHeader* header, int& offset)
	{
		if (buffer.size() <= offset) // offset : ���۰� �ſ� Ŀ���� �� ��Ŷ�� ������ �� ���ۿ� �����Ѵ�, �̶� ����ϴ� ������ offset�̶�� �Ѵ�. �� ������ ����� offset ���� ������ ���۰� �� �� �� ���ٴ� �ǹ�
			return false;

		const size_t remainedSize = buffer.size() - offset;
		if (remainedSize < sizeof(PacketHeader))// ������ ������� offset ��ŭ �� ���� PacketHeader�� ������ ��ŭ �ȳ��´ٸ� ��� �������� �Ұ����ϴٴ� �ǹ�
			return false;

		const char* headerPtr = static_cast<char*>(buffer.data()) + offset;
		memcpy(header, headerPtr, sizeof(PacketHeader));
		offset += sizeof(PacketHeader);
		return true;
	}
	static size_t RequiredSize(const google::protobuf::Message& msg)
	{
		return sizeof(PacketHeader) + msg.ByteSizeLong(); // ��� + ��Ŷ ������ ��ȯ
	}// ��Ŷ ����� ������ �ؼ� ũ�⸦ ��ȯ�ϴ� ���� �� �� �����ϴ�(����)

	static bool Serialize(const asio::mutable_buffer& buffer, const short packetCode, const google::protobuf::Message&
		msg)
	{
		const size_t requiredSize = RequiredSize(msg);
		if (buffer.size() < requiredSize)
			return false;
		PacketHeader header;
		header.Length = static_cast<short>(msg.ByteSizeLong());
		header.Code = packetCode;
		memcpy(buffer.data(), &header, sizeof(PacketHeader));
		char* payloadPtr = static_cast<char*>(buffer.data()) + sizeof(PacketHeader); // char* Ÿ���� �ٸ� ������ ó�� 4byte�� �ƴ� 1byte �̹Ƿ� 
		// Buffer�� �����͸� ��� ���ۺκ��� �����ϴ� �ڵ��, PacketHeader�� ����� ŭ�� ���ٴ� �ǹ�

		// �ش� �ڵ�� payloadPtr�� PacketHeader ���� ���� ����, �����͸� �ִ� ũ��� ��ü ������� packetHeader�� ����� ������ ũ�� ��ŭ�� ����ȭ�ϴ� �ǹ�
		if (!msg.SerializeToArray(payloadPtr, static_cast<int>(buffer.size()) - sizeof(PacketHeader)))
			return false;
		return true;
	}

	static bool Parse(google::protobuf::Message& msg, const asio::mutable_buffer& buffer, const int payloadSize, int&
		offset)
	{
		// ������ ����� üũ�ߴµ� ��Ŷ������� ���� ����Ǿ��ٴ� ���� �̹� ��Ŷ ����� �Ľ����� ���� ���¸� �ǹ���
		if (buffer.size() <= sizeof(PacketHeader))
			return false;

		// ������ ���� �������� �Ѱ� ���� offset�� ũ�⸸ŭ ���� �� ���� ��Ŷ ����� ������ ����ȭ�Ǿ��ִ� �������� ���� �κ�
		const char* payloadPtr = static_cast<char*>(buffer.data()) + offset;
		const size_t remainedSize = buffer.size() - offset;
		const bool parseResult = msg.ParseFromArray(payloadPtr, payloadSize); // �ش� �κк��� ������ ��ŭ ������ȭ�� ����
		if (parseResult)
		{
			offset += static_cast<int>(msg.ByteSizeLong()); // ���۰� ����� �迭 �� ��� ���� ��Ŷ���+��Ŷ�� ���� ����ȭ �ϱ� ���� offset ����
			return true;
		}
		else
		{
			return false;
		}
	}


};