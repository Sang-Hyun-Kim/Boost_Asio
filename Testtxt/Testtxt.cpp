// Testtxt.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//

#include <iostream>
#include <fstream>
#include <vector>
#include <string>

using namespace std;

int main()
{
	ifstream infile("C:\\Users\\Admin\\source\\repos\\BoostAsio\\Testtxt");

	if (!infile.is_open())
	{
		cout << "Unable to open file" << endl;
	}
	else {
		cout << "file open " << endl;
	}
}

