//#include <iostream>
//#include "Protocol.pb.h"
//
//using namespace std;
//int main()
//{
//	Protocol::TEST test;
//	test.set_id(10);
//	test.set_hp(100);
//
//	//repeated 는 하나의 연속된 저장공간
//	string* name1 = test.add_name();
//	name1->assign("qwer!");
//	// 연속된 저장공간에 넣는 방법
//	string* name2 = test.add_name();
//	name2->assign("asdf!");
//
///*	string abc = "1234";
//	test.set_name(2, abc)*/; // 인덱스 지정 삽입방식
//
//	auto vectest = test.add_vec();
//	vectest->set_data(10);
//	
//	int size = test.ByteSizeLong();
//	cout << size << endl;
//	// 용량 4+ 4+ 6 + 6 + 4 =24
//	// 출력 : 22 => 비트플래그를 통한 용량 최적화 진행
//	// 사용하지 않는 패킷은 데이터를 주고받을때 (직렬화.역직렬화)할 때 문제가 생기지 않음(hp를 사용하지 않아 용량이 줄여도 문제 X)
//	vector<char> vector1(size);
//	
//	test.SerializeToArray(vector1.data(), size); // &vector1 하면 안됨 vector 클래스 자체를 넘겨줘버림 
//	// 벡터안에 있는 동적할당된 데이터의 주소를 넘겨줌
//
//	Protocol::TEST test2;
//	test2.ParseFromArray(vector1.data(), size);
//
//	cout << test2.id() << endl;
//
//	cout << test2.hp() << endl;
//
//	cout << test2.name(0) << endl;
//	cout << test2.name(1) << endl;
//	// 2접근시 없는 인덱스=>CRASH
//	for (auto n : test.name()) cout << n << endl;
//	for (auto n : test.vec()) cout << n.data() << endl;
//
//}