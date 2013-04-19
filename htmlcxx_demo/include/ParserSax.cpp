#include "ParserSax.h"
#include <cctype>
#include <cstring>
#if !defined(WIN32) || defined(__MINGW32__)
#include <strings.h>
#endif

//#define DEBUG
//#include "debug.h"

static
struct literal_tag {
	int len;
	const char* str;
	int is_cdata;
}   
literal_mode_elem[] =
{   
	{6, "script", 1},
	{5, "style", 1},
	{3, "xmp", 1},
	{9, "plaintext", 1},
	{8, "textarea", 0},
	{0, 0, 0}
};

void htmlcxx::HTML::ParserSax::parse(const std::string &html)
{
//	std::cerr << "Parsing string" << std::endl;
	parse(html.c_str(), html.c_str() + html.length());
}



template <typename _Iterator>
void htmlcxx::HTML::ParserSax::parse(_Iterator begin, _Iterator end)
{
//	std::cerr << "Parsing iterator" << std::endl;
	parse(begin, end, typename std::iterator_traits<_Iterator>::iterator_category());
}


template <typename _Iterator>
void htmlcxx::HTML::ParserSax::parse(_Iterator &begin, _Iterator &end, std::forward_iterator_tag)
{
	typedef _Iterator iterator;
//	std::cerr << "Parsing forward_iterator" << std::endl;
	mCdata = false;
	mpLiteral = 0;
	mCurrentOffset = 0;
	this->beginParsing(); //��ʼ����

//	DEBUGP("Parsed text\n");


	/*
		���whileѭ����������ѭ������ѭ����������begin��end�����α꣬�������α�ʼ��ָ���˴��������ı�������ʼ�˺ͽ����ˡ�
	*/
	while (begin != end)
	{
		*begin; // This is for the multi_pass to release the buffer
		iterator c(begin);
		/*
			��whileѭ���Ǵ�����ѭ����ÿ�ν�������Ϊ������һ���ڵ㡣
		*/
		while (c != end)
		{
			// For some tags, the text inside it is considered literal and is
			// only closed for its </TAG> counterpart
			
			/*
				�����ѭ�����ж������У�mpLiteralĬ�ϳ�ʼֵΪ0�������к����ParseHtmlTag()����ʱ����������ֵ��
				�����Ϊ�㣬��˵����ǰ��tagNameӦ��Ϊ������������str��������֮һ��
			*/
			while (mpLiteral)
			{
				//DEBUGP("Treating literal %s\n", mpLiteral);

				//��󲻶ϲ��ң�ֱ���ҵ����ļ�ĩβ��<����ֹͣ���˿�˵��������ϻ��߽�������ĳ��tag��ǩ	
				while (c != end && *c != '<') ++c; 

				if (c == end) //�������˵���������
				{
					if (c != begin) this->parseContent(begin, c);//��ʱ���һ�������ı�ǩĩβ���ĵ�ĩβ֮����ܻ���һ��������Ҫ����
					goto DONE;//ת�������׶�
				}
				
				
				iterator end_text(c);//����һ�����α�end_text����ʼֵ����c,Ϊ����Ĵ����¼�µ�ǰ��<�����ŵ�λ�á�
				++c;//��c�α�ǰ��һ��

				if (*c == '/')//�жϡ�<������������Ƿ�Ϊ��/������
				{
					++c; //ǰ��һ����ָ��tagName����ʼλ��
					const char *l = mpLiteral;
					
					/*
						���ֽڽ�mpLiteral��tagName���бȽ�.
						�������һ��whileѭ��ʱ��c��lӦ��ָ�����mpLiteral��tagName����ʼλ�ÿ�ʼ����С�����Ӵ��Ľ���λ�ã�
						���ƥ��ɹ���lӦ��ָ���ַ��������һ��λ��+1����Ϊ�ա�
					*/
					while (*l && ::tolower(*c) == *l)
					{
						++c;
						++l;
					}

					// FIXME: Mozilla stops when it sees a /plaintext. Check
					// other browsers and decide what to do
					if (!*l && strcmp(mpLiteral, "plaintext"))
					{
						// matched all and is not tag plaintext
						//˵������plaintext��ǩ���Ҵ�ʱlָ��Ϊ��
						while (isspace(*c)) ++c; //�����ո��ַ�

						if (*c == '>') //ֱ���ҵ���ǩ����λ��
						{
							++c;
							if (begin != end_text)
								this->parseContent(begin, end_text); //������α�ǩ֮ǰ���ǶΡ��ı���
							mpLiteral = 0;//����ѭ����������
							c = end_text;//�޸��α�ֵ
							begin = c; //�޸��α�ֵ
							break;//����ѭ������ʱ˵���Ѿ��ҵ��˶�ӦtagName��</tagName>��ǩ
						}
					}
				}
				else if (*c == '!')//��Ѱ�ҹ���������ע�͵������Ҳ��Ҫ������Ӧ����
				{
					// we may find a comment and we should support it
					iterator e(c);
					++e;

					if (e != end && *e == '-' && ++e != end && *e == '-')
					{
						//DEBUGP("Parsing comment\n");
						++e;
						c = this->skipHtmlComment(e, end);
					}

					//if (begin != end_text)
					//this->parseContent(begin, end_text, end);

					//this->parseComment(end_text, c, end);

					// continue from the end of the comment
					//begin = c;
				}
			}

			if (*c == '<') //�ж��Ƿ���һ����ǩ�Ŀ�ʼ
			{
				iterator d(c);  //����һ�����α�d����ʼΪ�����<����λ��
				++d; //�������һλ
				if (d != end)//��������ĵ��Ľ�β
				{
					if (isalpha(*d)) //�ж��Ƿ�ΪӢ����ĸ
					{
						// beginning of tag
						if (begin != c)  //�����ǩ֮ǰ����һ���ı�
							this->parseContent(begin, c);

						//DEBUGP("Parsing beginning of tag\n");
						d = this->skipHtmlTag(d, end);//�ҵ�Tag��ǩ�Ľ���λ��
						this->parseHtmlTag(c, d);  //��Tag��ǩ����ʼλ�úͽ���λ��֮�����α�ǩ��������parseHtmltag(X,X)���д���

						// continue from the end of the tag
						c = d; //���α��ƶ�����ǩ֮���λ��
						begin = c;
						break;//�ɹ�������һ���ڵ㣬����ѭ��
					}

					if (*d == '/')
					{
						if (begin != c)//�����ǩ֮ǰ����һ���ı�
							this->parseContent(begin, c);

						iterator e(d); //����һ���±�ǩe����ʼ��Ϊd
						++e;//�������һλ
						if (e != end && isalpha(*e))  //˵����ʱeָ����ǽ�β�ͱ�ǩ����ʼ��
						{
							// end of tag
//							DEBUGP("Parsing end of tag\n");
							d = this->skipHtmlTag(d, end);//Ѱ�ұ�ǩ����λ��
							this->parseHtmlTag(c, d);//�����λ��֮��ı�ǩ���д���
						}
						else
						{
							// not a conforming end of tag, treat as comment
							// as Mozilla does
							//����һ����׼�Ľ����ͱ�ǩ����Mozilla����һ���Ĵ���ʽ
							//DEBUGP("Non conforming end of tag\n");
							d = this->skipHtmlTag(d, end); //���α��ƶ���tag��ǩ֮�����
							this->parseComment(c, d); //������һ���ڵ㣬����ѭ��
						}

						// continue from the end of the tag
						c = d;
						begin = c;
						break;
					}

					if (*d == '!')
					{
						// comment
						//˵����ע�ͱ�ǩ
						if (begin != c)//����˱�ǩ֮ǰ����һ���ı�
							this->parseContent(begin, c);

						iterator e(d);  //����һ���α�e����ʼ��Ϊd
						++e;

						if (e != end && *e == '-' && ++e != end && *e == '-')
						{
//							DEBUGP("Parsing comment\n");
							++e;
							d = this->skipHtmlComment(e, end);//���ҵ���ǩ��β
						}
						else
						{
							d = this->skipHtmlTag(d, end); //Ҳ����ע��ֱ���ĵ�����
						}

						this->parseComment(c, d);//������ı���Ϊע�ͽ��д���

						// continue from the end of the comment
						c = d;//�ƶ��α�����ǩ��β��
						begin = c;
						break;//�ɹ�������һ����ǩ������ѭ��
					}

					if (*d == '?' || *d == '%')
					{
						// something like <?xml or <%VBSCRIPT
						//����һЩ����<?xml �� <%VBSCRIPT֮��������ǩ
						if (begin != c)
							this->parseContent(begin, c);

						d = this->skipHtmlTag(d, end);

						this->parseComment(c, d);

						// continue from the end of the comment
						c = d;
						begin = c;
						break;
					}
				}
			}
			c++;
		}

		// There may be some text in the end of the document
		//�����б�ǩ��������󣬿������ĵ�ĩβ����һЩ�ı�Ҫ���д���
		if (begin != c)
		{
			this->parseContent(begin, c);
			begin = c;
		}
	}
//��ΪGOTO�����λ�ã���Ϊ����������ִ�е���β����
DONE:
	this->endParsing();
	return;
}

template <typename _Iterator>
void htmlcxx::HTML::ParserSax::parseComment(_Iterator b, _Iterator c)
{
	//DEBUGP("Creating comment node %s\n", std::string(b, c).c_str());
	
	//����һ���µ�Node�ڵ�
	htmlcxx::HTML::Node com_node;
	
	//FIXME: set_tagname shouldn't be needed, but first I must check
	//legacy code
	
	//����һ��string���ͣ����α�b,c֮�������ı�Ϊ���ʼ����ֵ
	std::string comment(b, c);
	com_node.tagName(comment);
	com_node.text(comment);
	com_node.offset(mCurrentOffset);
	com_node.length((unsigned int)comment.length());
	com_node.isTag(false);
	com_node.isComment(true);

	mCurrentOffset += com_node.length(); //����ǰ����λ�ú��ƣ��ƶ����ýڵ�Ľ���λ��

	// Call callback method
	//�ص����������ýڵ�������е�Html����
	this->foundComment(com_node);
}


/*
	��δ�����β�Ϊ�����α꣬���Ƿֱ�ָʾ��Ҫ������ı�����ʼλ�úͽ���λ�ã�
	��������ı���Ϊ�ı����������д���
*/
template <typename _Iterator>
void htmlcxx::HTML::ParserSax::parseContent(_Iterator b, _Iterator c)
{
	//DEBUGP("Creating text node %s\n", (std::string(b, c)).c_str());
	
	//����һ���µ�Node�ڵ�
	htmlcxx::HTML::Node txt_node;
	
	//FIXME: set_tagname shouldn't be needed, but first I must check
	//legacy code
	
	std::string text(b, c);
	txt_node.tagName(text);
	txt_node.text(text);
	txt_node.offset(mCurrentOffset);
	txt_node.length((unsigned int)text.length());
	txt_node.isTag(false);
	txt_node.isComment(false);

	mCurrentOffset += txt_node.length();

	// Call callback method
	//�ص����������ýڵ�������е�Html����
	this->foundText(txt_node);
}

/*
	��δ�����β�Ϊ�����α꣬���Ƿֱ�ָʾ��Ҫ������ı�����ʼλ�úͽ���λ�ã�
	��������ı���ΪHtml��ǩ�����д���
*/

template <typename _Iterator>
void htmlcxx::HTML::ParserSax::parseHtmlTag(_Iterator b, _Iterator c)
{
	_Iterator name_begin(b);  //����һ���µ��α�name_begin�α꣬��ʼΪb
	++name_begin;//��name_begin����һλ
	
	bool is_end_tag = (*name_begin == '/'); //�ж�name_begin�Ƿ��ǡ�/�����������˵���ǽ�����tag��ǩ
	if (is_end_tag) ++name_begin;//����ǽ����͵�tag��ǩ�����ٽ�name_beginǰ��һλ��������/������

	_Iterator name_end(name_begin); //����һ���µ�name_end�α꣬��¼һ��name_begin��ǩ��
	while (name_end != c && isalnum(*name_end)) //��name_end���ƣ�ֱ���������ĵ�ĩβ������ͨ�ַ�(��ĸ��������)Ϊֹ��
	{
		++name_end;
	}

	std::string name(name_begin, name_end);//ȡname_begin��name_end֮�������ı���Ϊ��tagName����ֵ��һ��string�͵ı���name
	//DEBUGP("Found %s tag %s\n", is_end_tag ? "closing" : "opening", name.c_str());

	/*
		֮������Ҫ�����Name��һЩ�жϣ��ж����Ƿ����������͵�tag��ǩ��
		���Ƿ���ǰ��literal_mode_elem[]�����е�ĳ����ǩ��
	*/
	if (!is_end_tag)  //���ȵ��ж��ⲻ��һ�������͵ı�ǩ
	{
		std::string::size_type tag_len = name.length();
		for (int i = 0; literal_mode_elem[i].len; ++i)
		{
			if (tag_len == literal_mode_elem[i].len)
			{
                #if defined(WIN32) && !defined(__MINGW32__)
				if (!_stricmp(name.c_str(), literal_mode_elem[i].str))
				#else
				if (!strcasecmp(name.c_str(), literal_mode_elem[i].str))
				#endif
				{
					mpLiteral = literal_mode_elem[i].str;
					break;
				}
			}
		}
	} 
	
	
	//֮�����ǾͿ��Խ�����ΪNode�ڵ�����һЩ�洢�Ĺ����ˡ�
	htmlcxx::HTML::Node tag_node;
	//by now, length is just the size of the tag
	std::string text(b, c);
	tag_node.length(static_cast<unsigned int>(text.length()));
	tag_node.tagName(name);
	tag_node.text(text);
	tag_node.offset(mCurrentOffset);
	tag_node.isTag(true);
	tag_node.isComment(false);

	mCurrentOffset += tag_node.length();//����ǰ�Ľ���λ�ú���

	this->foundTag(tag_node, is_end_tag);//���÷���������뵽���е�Html����
}



template <typename _Iterator>
_Iterator
htmlcxx::HTML::ParserSax::skipHtmlComment(_Iterator c, _Iterator end)
{
	while ( c != end ) {
		if (*c++ == '-' && c != end && *c == '-')
		{
			_Iterator d(c);
			//while (++c != end && isspace(*c));
			//���������һ�����
			while (++c != end &&((unsigned)*c > 255 || !isspace(*c) ) && *c != '>');
			if (c == end || *c++ == '>') break;
			c = d;
		}
	}

	return c;
}




namespace htmlcxx { namespace HTML {

//��������Ҫ�Ǵ��α�c����ʼ��ֱ���α�end֮�䣬Ѱ�ҵ���һ��quote�ַ���λ�ò��������α꣬δ�ҵ��򷵻�end��λ�á�

template <typename _Iterator>
static inline
_Iterator find_next_quote(_Iterator c, _Iterator end, char quote)
{
//	std::cerr << "generic find" << std::endl;
	while (c != end && *c != quote) ++c;
	return c;
}


//��������Ҫ�Ǵ��ַ�ָ��*c��ʼ��ֱ���ַ�ָ��*end֮�䣬Ѱ�ҵ���һ��quote�ַ���λ�ò�����ָ������ַ�ָ�룬δ�ҵ��򷵻�*endָ�롣
template <>
inline
const char *find_next_quote(const char *c, const char *end, char quote)
{
//	std::cerr << "fast find" << std::endl;
	const char *d = reinterpret_cast<const char*>(memchr(c, quote, end - c));

	if (d) return d;
	else return end;
}

}}


/*
	���α�c��λ�ÿ�ʼ��ֱ���α�end��λ��֮�䣬Ѱ�ҵ��ñ�ǩ����Ӧ�Ľ�����>�����ŵ�λ�ã������α���ʽ���ء�
	����Ҫע���tag��ǩ�ڵ�������������Ĵ������ܹ�ʶ�����������еġ�>�����ţ���ֹ����
*/

template <typename _Iterator>
_Iterator htmlcxx::HTML::ParserSax::skipHtmlTag(_Iterator c, _Iterator end)
{
	while (c != end && *c != '>')
	{
		if (*c != '=') 
		{
			++c;
		}
		else
		{ // found an attribute
			++c;
			while (c != end && isspace(*c)) ++c;

			if (c == end) break;

			if (*c == '\"' || *c == '\'') 
			{
				_Iterator save(c);
				char quote = *c++;
				c = find_next_quote(c, end, quote);
//				while (c != end && *c != quote) ++c;
//				c = static_cast<char*>(memchr(c, quote, end - c));
				if (c != end) 
				{
					++c;
				} 
				else 
				{
					c = save;
					++c;
				}
//				DEBUGP("Quotes: %s\n", std::string(save, c).c_str());
			}
		}
	}

	if (c != end) ++c;
	
	return c;
}
