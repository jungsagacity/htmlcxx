#include "ParserSax.h"


/*
�÷���ʵ����ֱ�ӵ�������һ��template<typename _Iterator> void parse(_Iterator begin, _Iterator end)������
�����������һЩ������Ҫ����������ParseSax.tcc�н���ʵ�֡�
*/
void htmlcxx::HTML::ParserSax::parse(const std::string &html)
{
//	std::cerr << "Parsing string" << std::endl;
	parse(html.c_str(), html.c_str() + html.length());
}
