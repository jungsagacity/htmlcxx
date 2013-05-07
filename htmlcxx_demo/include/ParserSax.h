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
				ParserSax(): mpLiteral(0),mCdata(false) {}//���췽������ʼ��������Ա������ֵ
				virtual ~ParserSax() {}//��������
				/** Parse the html code */
				void parse(const std::string &html);//����һ��string��ʽ��html�ĵ�

				template <typename _Iterator>
				void parse(_Iterator begin, _Iterator end);//ʹ������������ͬ���βθ�ʽ�Ľ�����ģ��	
				

			protected:
				// Redefine this if you want to do some initialization before
				// the parsing
				virtual void beginParsing() {}
	
				//���¼����麯��������ParseDom.cpp�н�����ʵ��
				virtual void foundTag(Node node, bool isEnd) {}
				virtual void foundText(Node node) {}
				virtual void foundComment(Node node) {}

				virtual void endParsing() {}

				//�����Ǽ���ģ�壬�󲿷ֶ���ParseSax.cpp�ļ���ʵ��
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


				//���ݳ�Ա
				unsigned long mCurrentOffset; //��ǰ���ڽ�����ƫ��λ��
				const char *mpLiteral;
				bool mCdata;
		};

	}//namespace HTML
}//namespace htmlcxx

#endif
