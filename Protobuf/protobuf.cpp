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
//	//repeated �� �ϳ��� ���ӵ� �������
//	string* name1 = test.add_name();
//	name1->assign("qwer!");
//	// ���ӵ� ��������� �ִ� ���
//	string* name2 = test.add_name();
//	name2->assign("asdf!");
//
///*	string abc = "1234";
//	test.set_name(2, abc)*/; // �ε��� ���� ���Թ��
//
//	auto vectest = test.add_vec();
//	vectest->set_data(10);
//	
//	int size = test.ByteSizeLong();
//	cout << size << endl;
//	// �뷮 4+ 4+ 6 + 6 + 4 =24
//	// ��� : 22 => ��Ʈ�÷��׸� ���� �뷮 ����ȭ ����
//	// ������� �ʴ� ��Ŷ�� �����͸� �ְ������ (����ȭ.������ȭ)�� �� ������ ������ ����(hp�� ������� �ʾ� �뷮�� �ٿ��� ���� X)
//	vector<char> vector1(size);
//	
//	test.SerializeToArray(vector1.data(), size); // &vector1 �ϸ� �ȵ� vector Ŭ���� ��ü�� �Ѱ������ 
//	// ���;ȿ� �ִ� �����Ҵ�� �������� �ּҸ� �Ѱ���
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
//	// 2���ٽ� ���� �ε���=>CRASH
//	for (auto n : test.name()) cout << n << endl;
//	for (auto n : test.vec()) cout << n.data() << endl;
//
//}