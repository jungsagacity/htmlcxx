#include "ParserDom.h"


#include <iostream>
#include <vector>

//#define DEBUG


#define TAG_NAME_MAX 10

using namespace std;
using namespace htmlcxx; 
using namespace HTML; 
using namespace kp; 

const tree<HTML::Node>& ParserDom::parseTree(const std::string &html)
{
	this->parse(html);//调用ParserSax.h中的parse方法对html字符串进行解析，并且将结果存入数据成员mHtmlTree
	return this->getTree();//返回受保护的数据成员mHtmlTree
}


//该方法主要是为解析之前做一些初始化的工作，主要是在树的根节点之前插入一个新的节点作为新根节点。
void ParserDom::beginParsing()
{
	mHtmlTree.clear();
	tree<HTML::Node>::iterator top = mHtmlTree.begin();
	HTML::Node lambda_node;
	lambda_node.offset(0);
	lambda_node.length(0);
	lambda_node.isTag(true);
	lambda_node.isComment(false);
	mCurrentState = mHtmlTree.insert(top,lambda_node);
}


//该方法主要作用是停止当前的解析，并记录下已经解析的长度。
void ParserDom::endParsing()
{
	tree<HTML::Node>::iterator top = mHtmlTree.begin();
	top->length(mCurrentOffset);//mCurrentOffset是ParserSax类中的一个protected成员变量，记录当前解析长度
}

/*
该方法的主要作用是在当前正在解析的节点(即mCurrentState游标所指向的节点)下将节点作为注释内容插入进去。
实际这个方法上和后面的foundText()方法完全一样，因为无论是Comment或Text都是作为Node添加进去的，
对于程序来讲没有本质上的区别。
*/
void ParserDom::foundComment(Node node)
{
	//Add child content node, but do not update current state
	//在当前节点下添加一个新的节点node，但是不更新当前解析的进度，即不修改mCurrentState游标
	mHtmlTree.append_child(mCurrentState, node);
}
/*
同ParserDom::foundComment(Node node)
*/
void ParserDom::foundText(Node node)
{
	//Add child content node, but do not update current state
	 //在当前节点下添加一个新的节点node，但是不更新当前解析的进度，即不修改mCurrentState游标
	mHtmlTree.append_child(mCurrentState, node);
}


/*
该方法的主要作用是添加一个新的标签节点，并且需要根据isEnd变量进行判断是起始标签如<div>或<a>等，
还是结束标签如</div>或</a>等。
*/

void ParserDom::foundTag(Node node, bool isEnd)
{
	if (!isEnd) 
	{
		//append to current tree node
		tree<HTML::Node>::iterator next_state;
		next_state = mHtmlTree.append_child(mCurrentState, node);
		mCurrentState = next_state;
	} 
	else 
	{
		//Look if there is a pending open tag with that same name upwards
		//If mCurrentState tag isn't matching tag, maybe a some of its parents
		// matches
		vector< tree<HTML::Node>::iterator > path;
		tree<HTML::Node>::iterator i = mCurrentState;
		bool found_open = false;
		while (i != mHtmlTree.begin())
		{
#ifdef DEBUG
			cerr << "comparing " << node.tagName() << " with " << i->tagName()<<endl<<":";
			if (!i->tagName().length()) cerr << "Tag with no name at" << i->offset()<<";"<<i->offset()+i->length();
#endif
			assert(i->isTag());
			assert(i->tagName().length());

			bool equal;
			const char *open = i->tagName().c_str();
			const char *close = node.tagName().c_str();
			equal = !(strcmp(open,close));


			if (equal) 
			{
				//DEBUGP("Found matching tag %s\n", i->tagName().c_str());
				//Closing tag closes this tag
				//Set length to full range between the opening tag and
				//closing tag
				i->length(node.offset() + node.length() - i->offset());
				i->closingText(node.text());

				mCurrentState = mHtmlTree.parent(i);
				found_open = true;
				break;
			} 
			else 
			{
				path.push_back(i);
			}

			i = mHtmlTree.parent(i);
		}

		if (found_open)
		{
			//If match was upper in the tree, so we need to invalidate child
			//nodes that were waiting for a close
			for (unsigned int j = 0; j < path.size(); ++j)
			{
//				path[j]->length(node.offset() - path[j]->offset());
				mHtmlTree.flatten(path[j]);
			}
		} 
		else 
		{
			//DEBUGP("Unmatched tag %s\n", node.text().c_str());

			// Treat as comment
			node.isTag(false);
			node.isComment(true);
			mHtmlTree.append_child(mCurrentState, node);
		}
	}
}

ostream &HTML::operator<<(ostream &stream, const tree<HTML::Node> &tr) 
{

	tree<HTML::Node>::pre_order_iterator it = tr.begin();
	tree<HTML::Node>::pre_order_iterator end = tr.end();

	int rootdepth = tr.depth(it);
	stream << "-----" << endl;

	unsigned int n = 0;
	while ( it != end ) 
	{

		int cur_depth = tr.depth(it);
		for(int i=0; i < cur_depth - rootdepth; ++i) 
		{
			stream << "  ";
		}

		stream << n << "@";
		stream << "[" << it->offset() << "~";
		stream << it->offset() + it->length() << "] ";
		stream << (string)(*it) << endl;
		++it, ++n;
	}
	stream << "-----" << endl;
	return stream;
}
