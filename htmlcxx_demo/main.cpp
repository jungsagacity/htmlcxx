#include <string.h>
#include <iostream>
#include "include\ParserDom.h"

using namespace std;
using namespace htmlcxx;


int main()
{

	//解析一段Html代码
	string html = "<td>我们";
	HTML::ParserDom parser;
	tree<HTML::Node> dom = parser.parseTree(html);
	cout<<dom<<endl;
	tree<HTML::Node>::iterator it = dom.begin();
	tree<HTML::Node>::iterator end = dom.end();
	//it++;
	while(it!=end)
	{
		
		if (it->isTag()) 
		{
			cout<<"tagName:\t"<<it->tagName()<<endl;			
			it->parseAttributes();
			map<std::string, std::string> mAttributes = it->attributes();

			map<std::string, std::string>::iterator it = mAttributes.begin();

			while(it != mAttributes.end())
			{
				cout<<"AttributName:"<<it->first<<":\t"<<it->second<<endl;
				it++;
			}

			
		}else if(it->isComment())
		{
			cout<<"comments:\t"<<it->text()<<endl;
		}else
		{
			cout<<"text:\t"<<it->text()<<endl;
		}

		it++;
	}
}