#ifndef __HTML_PARSER_DOM_H__
#define __HTML_PARSER_DOM_H__

#include "ParserSax.h"
#include "tree.h"

namespace htmlcxx
{
	namespace HTML
	{
		class ParserDom : public ParserSax
		{
			public:
				ParserDom() {}//���췽��
				~ParserDom() {}//��������

				const tree<Node> &parseTree(const std::string &html);//ͨ���ַ�����������
				const tree<Node> &getTree()	{ return mHtmlTree; }//���ؽ�����

			protected://�����麯������������
				virtual void beginParsing();//��ʼ����
				virtual void foundTag(Node node, bool isEnd);//Ѱ��ָ����ǩ
				virtual void foundText(Node node);//Ѱ���ı�
				virtual void foundComment(Node node);//Ѱ��ע���ı�
				virtual void endParsing();//��������
				
				tree<Node> mHtmlTree;//��Ž����õ���
				tree<Node>::iterator mCurrentState;//���ڱ������ĵ������������������Լ�tree.h
		};
		//��д������ << ,ͨ���ò���������ֱ�����tree<HTML::Node> ���͵ı���
		std::ostream &operator<<(std::ostream &stream, const tree<HTML::Node> &tr);
	} //namespace HTML
} //namespace htmlcxx

#endif
