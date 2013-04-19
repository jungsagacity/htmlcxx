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
	this->beginParsing(); //开始解析

//	DEBUGP("Parsed text\n");


	/*
		这个while循环是最外层的循环，其循环的依据是begin和end两个游标，这两个游标始终指向了待解析的文本串的起始端和结束端。
	*/
	while (begin != end)
	{
		*begin; // This is for the multi_pass to release the buffer
		iterator c(begin);
		/*
			该while循环是次外层的循环，每次结束都即为解析出一个节点。
		*/
		while (c != end)
		{
			// For some tags, the text inside it is considered literal and is
			// only closed for its </TAG> counterpart
			
			/*
				在这个循环的判断条件中，mpLiteral默认初始值为0，在运行后面的ParseHtmlTag()方法时候会更改它的值，
				如果不为零，则说明当前的tagName应当为以下这个数组的str属性其中之一。
			*/
			while (mpLiteral)
			{
				//DEBUGP("Treating literal %s\n", mpLiteral);

				//向后不断查找，直到找到了文件末尾或’<’才停止，此刻说明解析完毕或者解析到了某个tag标签	
				while (c != end && *c != '<') ++c; 

				if (c == end) //如果成立说明解析完毕
				{
					if (c != begin) this->parseContent(begin, c);//此时最后一个解析的标签末尾和文档末尾之间可能还有一段内容需要解析
					goto DONE;//转到结束阶段
				}
				
				
				iterator end_text(c);//申请一个新游标end_text，初始值赋入c,为后面的代码记录下当前’<’符号的位置。
				++c;//将c游标前进一步

				if (*c == '/')//判断’<’后面紧跟的是否为’/’符号
				{
					++c; //前进一步，指向tagName的起始位置
					const char *l = mpLiteral;
					
					/*
						逐字节将mpLiteral和tagName进行比较.
						跳出最后一个while循环时，c和l应当指向的是mpLiteral和tagName从起始位置开始的最小公共子串的结束位置，
						如果匹配成功则l应当指向字符串的最后一个位置+1，即为空。
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
						//说明不是plaintext标签，且此时l指针为空
						while (isspace(*c)) ++c; //跳过空格字符

						if (*c == '>') //直到找到标签结束位置
						{
							++c;
							if (begin != end_text)
								this->parseContent(begin, end_text); //处理这段标签之前的那段“文本”
							mpLiteral = 0;//更改循环进入条件
							c = end_text;//修改游标值
							begin = c; //修改游标值
							break;//跳出循环，此时说明已经找到了对应tagName的</tagName>标签
						}
					}
				}
				else if (*c == '!')//在寻找过程中遇到注释的情况，也需要做出相应处理。
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

			if (*c == '<') //判断是否是一个标签的开始
			{
				iterator d(c);  //申请一个新游标d，初始为这个’<’的位置
				++d; //将其后移一位
				if (d != end)//如果不是文档的结尾
				{
					if (isalpha(*d)) //判断是否为英文字母
					{
						// beginning of tag
						if (begin != c)  //如果标签之前还有一段文本
							this->parseContent(begin, c);

						//DEBUGP("Parsing beginning of tag\n");
						d = this->skipHtmlTag(d, end);//找到Tag标签的结束位置
						this->parseHtmlTag(c, d);  //对Tag标签的起始位置和结束位置之间的这段标签内容适用parseHtmltag(X,X)进行处理。

						// continue from the end of the tag
						c = d; //将游标移动到标签之后的位置
						begin = c;
						break;//成功解析出一个节点，跳出循环
					}

					if (*d == '/')
					{
						if (begin != c)//如果标签之前还有一段文本
							this->parseContent(begin, c);

						iterator e(d); //申请一个新标签e，初始化为d
						++e;//将其后移一位
						if (e != end && isalpha(*e))  //说明此时e指向的是结尾型标签的起始处
						{
							// end of tag
//							DEBUGP("Parsing end of tag\n");
							d = this->skipHtmlTag(d, end);//寻找标签结束位置
							this->parseHtmlTag(c, d);//对这段位置之间的标签进行处理
						}
						else
						{
							// not a conforming end of tag, treat as comment
							// as Mozilla does
							//不是一个标准的结束型标签，和Mozilla采用一样的处理方式
							//DEBUGP("Non conforming end of tag\n");
							d = this->skipHtmlTag(d, end); //将游标移动到tag标签之后继续
							this->parseComment(c, d); //解析出一个节点，跳出循环
						}

						// continue from the end of the tag
						c = d;
						begin = c;
						break;
					}

					if (*d == '!')
					{
						// comment
						//说明是注释标签
						if (begin != c)//如果此标签之前还有一段文本
							this->parseContent(begin, c);

						iterator e(d);  //申请一个游标e，初始化为d
						++e;

						if (e != end && *e == '-' && ++e != end && *e == '-')
						{
//							DEBUGP("Parsing comment\n");
							++e;
							d = this->skipHtmlComment(e, end);//查找到标签结尾
						}
						else
						{
							d = this->skipHtmlTag(d, end); //也可能注释直到文档结束
						}

						this->parseComment(c, d);//将这段文本作为注释进行处理

						// continue from the end of the comment
						c = d;//移动游标至标签结尾处
						begin = c;
						break;//成功解析出一个标签，跳出循环
					}

					if (*d == '?' || *d == '%')
					{
						// something like <?xml or <%VBSCRIPT
						//处理一些诸如<?xml 或 <%VBSCRIPT之类的特殊标签
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
		//在所有标签都处理完后，可能在文档末尾还有一些文本要进行处理
		if (begin != c)
		{
			this->parseContent(begin, c);
			begin = c;
		}
	}
//作为GOTO代码的位置，即为解析结束后执行的收尾工作
DONE:
	this->endParsing();
	return;
}

template <typename _Iterator>
void htmlcxx::HTML::ParserSax::parseComment(_Iterator b, _Iterator c)
{
	//DEBUGP("Creating comment node %s\n", std::string(b, c).c_str());
	
	//申请一个新的Node节点
	htmlcxx::HTML::Node com_node;
	
	//FIXME: set_tagname shouldn't be needed, but first I must check
	//legacy code
	
	//申请一个string类型，用游标b,c之间的这段文本为其初始化赋值
	std::string comment(b, c);
	com_node.tagName(comment);
	com_node.text(comment);
	com_node.offset(mCurrentOffset);
	com_node.length((unsigned int)comment.length());
	com_node.isTag(false);
	com_node.isComment(true);

	mCurrentOffset += com_node.length(); //将当前解析位置后移，移动到该节点的结束位置

	// Call callback method
	//回调方法，将该节点插入已有的Html树中
	this->foundComment(com_node);
}


/*
	这段代码的形参为两个游标，他们分别指示了要处理的文本的起始位置和结束位置，
	并将这段文本作为文本内容来进行处理。
*/
template <typename _Iterator>
void htmlcxx::HTML::ParserSax::parseContent(_Iterator b, _Iterator c)
{
	//DEBUGP("Creating text node %s\n", (std::string(b, c)).c_str());
	
	//申请一个新的Node节点
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
	//回调方法，将该节点插入已有的Html树中
	this->foundText(txt_node);
}

/*
	这段代码的形参为两个游标，他们分别指示了要处理的文本的起始位置和结束位置，
	并将这段文本作为Html标签来进行处理。
*/

template <typename _Iterator>
void htmlcxx::HTML::ParserSax::parseHtmlTag(_Iterator b, _Iterator c)
{
	_Iterator name_begin(b);  //申请一个新的游标name_begin游标，初始为b
	++name_begin;//将name_begin后移一位
	
	bool is_end_tag = (*name_begin == '/'); //判断name_begin是否是’/’，如果是则说明是结束型tag标签
	if (is_end_tag) ++name_begin;//如果是结束型的tag标签，则再将name_begin前进一位，跳过’/’符号

	_Iterator name_end(name_begin); //申请一个新的name_end游标，记录一下name_begin标签。
	while (name_end != c && isalnum(*name_end)) //将name_end后移，直到遇到了文档末尾或者普通字符(字母或者数字)为止。
	{
		++name_end;
	}

	std::string name(name_begin, name_end);//取name_begin和name_end之间的这段文本认为是tagName并赋值给一个string型的变量name
	//DEBUGP("Found %s tag %s\n", is_end_tag ? "closing" : "opening", name.c_str());

	/*
		之后，我们要对这个Name做一些判断，判断其是否是特殊类型的tag标签，
		即是否是前面literal_mode_elem[]数组中的某个标签。
	*/
	if (!is_end_tag)  //首先得判断这不是一个结束型的标签
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
	
	
	//之后，我们就可以将其作为Node节点来做一些存储的工作了。
	htmlcxx::HTML::Node tag_node;
	//by now, length is just the size of the tag
	std::string text(b, c);
	tag_node.length(static_cast<unsigned int>(text.length()));
	tag_node.tagName(name);
	tag_node.text(text);
	tag_node.offset(mCurrentOffset);
	tag_node.isTag(true);
	tag_node.isComment(false);

	mCurrentOffset += tag_node.length();//将当前的解析位置后移

	this->foundTag(tag_node, is_end_tag);//调用方法将其加入到已有的Html树中
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
			//代替上面的一句代码
			while (++c != end &&((unsigned)*c > 255 || !isspace(*c) ) && *c != '>');
			if (c == end || *c++ == '>') break;
			c = d;
		}
	}

	return c;
}




namespace htmlcxx { namespace HTML {

//本方法主要是从游标c处开始，直到游标end之间，寻找到第一个quote字符的位置并返回其游标，未找到则返回end的位置。

template <typename _Iterator>
static inline
_Iterator find_next_quote(_Iterator c, _Iterator end, char quote)
{
//	std::cerr << "generic find" << std::endl;
	while (c != end && *c != quote) ++c;
	return c;
}


//本方法主要是从字符指针*c开始，直到字符指针*end之间，寻找到第一个quote字符的位置并返回指向其的字符指针，未找到则返回*end指针。
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
	从游标c的位置开始，直到游标end的位置之间，寻找到该标签所对应的结束’>’符号的位置，并以游标形式返回。
	其中要注意对tag标签内的属性做了另外的处理，即能够识别并跳过属性中的’>’符号，防止出错。
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
