#include <iostream>
#include <cctype>
#include <algorithm>

#include "Node.h"

//#define DEBUG


using namespace std;
using namespace htmlcxx;
using namespace HTML;

void Node::parseAttributes() 
{
	if (!(this->isTag())) return;//判断是否是Tag标签节点，不是则返回

	const char *end;
	const char *ptr = mText.c_str();//*ptr作为mText的*char类型指针
	if ((ptr = strchr(ptr, '<')) == 0) return; //将ptr指向字符串中首先出现的<位置，并判断该位置是否合法，如果不合法则返回。
	++ptr;//将ptr后移一个字节

	// Skip initial blankspace
	//跳过接下来所有遇到的空格字符
	while (isspace(*ptr)) ++ptr; 

	// Skip tagname
	 //判断是否是普通字符，不是则返回
	if (!isalpha(*ptr)) return;
	//不断后移ptr指针，直到遇到空格字符
	while (!isspace(*ptr)) 
	{
		if(*ptr == '>')  return; //added by wujun 2013.1.22
		++ptr;
	
	}

	// Skip blankspace after tagname
	//跳过接下来所有遇到的空格字符
	while (isspace(*ptr)) ++ptr;

	//一直不断向后解析该tag标签，直到遇到了’>’字符或者指针为空才退出
	while (*ptr && *ptr != '>') 
	{
		string key, val;

		// skip unrecognized
		//跳过所有无法识别的字节
		while (*ptr && !isalnum(*ptr) && !isspace(*ptr)) ++ptr;

		// skip blankspace
		 //跳过所有的空格字符
		while (isspace(*ptr)) ++ptr;

		end = ptr;
		//将end指向ptr之后第一个不为普通字符或’-’字符的位置
		while (isalnum(*end) || *end == '-') ++end;

		//将ptr至end之间的这段文本变为小写字符赋值给key
		key.assign(end - ptr, '\0');
		transform(ptr, end, key.begin(), ::tolower);
		ptr = end;

		// skip blankspace
		//此时key中应当存放的是属性名,跳过所有的空格字符
		while (isspace(*ptr)) ++ptr;
		
		//解析并获取属性的值
		if (*ptr == '=') 
		{
			++ptr;
			while (isspace(*ptr)) ++ptr;
			if (*ptr == '"' || *ptr == '\'') 
			{
				char quote = *ptr;
//				fprintf(stderr, "Trying to find quote: %c\n", quote);
				const char *end = strchr(ptr + 1, quote);
				if (end == 0)
				{
					//b = mText.find_first_of(" >", a+1);
					const char *end1, *end2;
					end1 = strchr(ptr + 1, ' ');
					end2 = strchr(ptr + 1, '>');
					if (end1 && end1 < end2) end = end1;
					else end = end2;
					if (end == 0) return;
				}
				const char *begin = ptr + 1;
				//while (isspace(*begin) && begin < end) ++begin;
				//代替上面的一句代码
				while (((unsigned)*begin > 255 || isspace(*begin)) && begin < end) ++begin;  
				
				
				const char *trimmed_end = end - 1;
				//while (isspace(*trimmed_end) && trimmed_end >= begin) --trimmed_end;
				while (((unsigned) *trimmed_end > 255 || isspace(*trimmed_end)) && trimmed_end >= begin) --trimmed_end;

				val.assign(begin, trimmed_end + 1);
				ptr = end + 1;
			}
			else 
			{
				end = ptr;

				//while (*end && !isspace(*end) && *end != '>') end++;
				// 代替上面的代码，
				while (*end &&((unsigned)*end > 255 || !isspace(*end) ) && *end != '>') end++;
				val.assign(ptr, end);
				ptr = end;
			}

//			fprintf(stderr, "%s = %s\n", key.c_str(), val.c_str());
			mAttributes.insert(make_pair(key, val));
		}
		else
		{
//			fprintf(stderr, "D: %s\n", key.c_str());
			mAttributes.insert(make_pair(key, string()));
		}
	}
}

bool Node::operator==(const Node &n) const 
{
	if (!isTag() || !n.isTag()) return false;
	return !(strcmp(tagName().c_str(), n.tagName().c_str()));
}

Node::operator string() const {
	if (isTag()) return this->tagName();
	return this->text();
}

ostream &Node::operator<<(ostream &stream) const {
	stream << (string)(*this);
	return stream;
}
