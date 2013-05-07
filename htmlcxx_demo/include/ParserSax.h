#ifndef __HTML_PARSER_SAX_H__
#define __HTML_PARSER_SAX_H__

#include <string>

#include "Node.h"

namespace htmlcxx
{
	namespace HTML
	{
		class ParserSax
		{
			public:
				ParserSax(): mpLiteral(0),mCdata(false) {}//构造方法，初始化两个成员变量的值
				virtual ~ParserSax() {}//析构方法
				/** Parse the html code */
				void parse(const std::string &html);//解析一个string格式的html文档

				template <typename _Iterator>
				void parse(_Iterator begin, _Iterator end);//使用两个类型相同的形参格式的解析器模板	
				

			protected:
				// Redefine this if you want to do some initialization before
				// the parsing
				virtual void beginParsing() {}
	
				//以下几个虚函数，都在ParseDom.cpp中进行了实现
				virtual void foundTag(Node node, bool isEnd) {}
				virtual void foundText(Node node) {}
				virtual void foundComment(Node node) {}

				virtual void endParsing() {}

				//以下是几个模板，大部分都在ParseSax.cpp文件中实现
				template <typename _Iterator>
				void parse(_Iterator &begin, _Iterator &end,std::forward_iterator_tag);

				template <typename _Iterator>
				void parseHtmlTag(_Iterator b, _Iterator c);

				template <typename _Iterator>
				void parseContent(_Iterator b, _Iterator c);

				template <typename _Iterator>
				void parseComment(_Iterator b, _Iterator c);

				template <typename _Iterator>
				_Iterator skipHtmlTag(_Iterator ptr, _Iterator end);
				
				template <typename _Iterator>
				_Iterator skipHtmlComment(_Iterator ptr, _Iterator end);


				//数据成员
				unsigned long mCurrentOffset; //当前正在解析的偏移位置
				const char *mpLiteral;
				bool mCdata;
		};

	}//namespace HTML
}//namespace htmlcxx

#endif
