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
	if (!(this->isTag())) return;//�ж��Ƿ���Tag��ǩ�ڵ㣬�����򷵻�

	const char *end;
	const char *ptr = mText.c_str();//*ptr��ΪmText��*char����ָ��
	if ((ptr = strchr(ptr, '<')) == 0) return; //��ptrָ���ַ��������ȳ��ֵ�<λ�ã����жϸ�λ���Ƿ�Ϸ���������Ϸ��򷵻ء�
	++ptr;//��ptr����һ���ֽ�

	// Skip initial blankspace
	//�������������������Ŀո��ַ�
	while (isspace(*ptr)) ++ptr; 

	// Skip tagname	
	if (!isalpha(*ptr)) return;	
	while (!isspace(*ptr)) 
	{
		if(*ptr == '>')  return; //added by wujun 2013.1.22 ��ʱ˵��û�и�tagû������
		++ptr;
	
	}

	// Skip blankspace after tagname
	//�������������������Ŀո��ַ�
	while (isspace(*ptr)) ++ptr;

	//һֱ������������tag��ǩ��ֱ�������ˡ�>���ַ�����ָ��Ϊ�ղ��˳�
	while (*ptr && *ptr != '>') 
	{
		string key, val;

		// skip unrecognized
		//���������޷�ʶ����ֽ�
		while (*ptr && !isalnum(*ptr) && !isspace(*ptr)) ++ptr;

		// skip blankspace
		 //�������еĿո��ַ�
		while (isspace(*ptr)) ++ptr;

		end = ptr;
		//��endָ��ptr֮���һ����Ϊ��ͨ�ַ���-���ַ���λ��
		while (isalnum(*end) || *end == '-') ++end;

		//��ptr��end֮�������ı���ΪСд�ַ���ֵ��key
		key.assign(end - ptr, '\0');
		transform(ptr, end, key.begin(), ::tolower);
		ptr = end;

		// skip blankspace
		//��ʱkey��Ӧ����ŵ���������,�������еĿո��ַ�
		while (isspace(*ptr)) ++ptr;
		
		//��������ȡ���Ե�ֵ
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
				//���������һ�����
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
				// ��������Ĵ��룬
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
