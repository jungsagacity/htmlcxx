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
				ParserDom() {}//构造方法
				~ParserDom() {}//析构方法

				const tree<Node> &parseTree(const std::string &html);//通过字符串来解析树
				const tree<Node> &getTree()	{ return mHtmlTree; }//返回解析树

			protected://声明虚函数，用于重载
				virtual void beginParsing();//开始解析
				virtual void foundTag(Node node, bool isEnd);//寻找指定标签
				virtual void foundText(Node node);//寻找文本
				virtual void foundComment(Node node);//寻找注释文本
				virtual void endParsing();//结束解析
				
				tree<Node> mHtmlTree;//存放解析好的树
				tree<Node>::iterator mCurrentState;//用于遍历树的迭代器，具体声明可以见tree.h
		};
		//重写操作符 << ,通过该操作符可以直接输出tree<HTML::Node> 类型的变量
		std::ostream &operator<<(std::ostream &stream, const tree<HTML::Node> &tr);
	} //namespace HTML
} //namespace htmlcxx

#endif
