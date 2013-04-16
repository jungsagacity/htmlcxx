#include "ParserSax.h"


/*
该方法实际上直接调用了另一个template<typename _Iterator> void parse(_Iterator begin, _Iterator end)方法，
而这个方法和一些其他主要方法都是在ParseSax.tcc中进行实现。
*/
void htmlcxx::HTML::ParserSax::parse(const std::string &html)
{
//	std::cerr << "Parsing string" << std::endl;
	parse(html.c_str(), html.c_str() + html.length());
}
