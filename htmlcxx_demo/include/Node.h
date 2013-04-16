#ifndef __HTML_PARSER_NODE_H
#define __HTML_PARSER_NODE_H

#include <map>
#include <string>
#include <utility>

namespace htmlcxx {
	namespace HTML {
		class Node {

			public:
				Node() {} //构造函数
				//Node(const Node &rhs); //uses default
				~Node() {}  //析构函数

				inline void text(const std::string& text) { this->mText = text; }
				inline const std::string& text() const { return this->mText; }

				inline void closingText(const std::string &text) { this->mClosingText = text; }
				inline const std::string& closingText() const { return mClosingText; }

				inline void offset(unsigned int offset) { this->mOffset = offset; }
				inline unsigned int offset() const { return this->mOffset; }

				inline void length(unsigned int length) { this->mLength = length; }
				inline unsigned int length() const { return this->mLength; }

				inline void tagName(const std::string& tagname) { this->mTagName = tagname; }
				inline const std::string& tagName() const { return this->mTagName; }

				bool isTag() const { return this->mIsHtmlTag; }
				void isTag(bool is_html_tag){ this->mIsHtmlTag = is_html_tag; }

				bool isComment() const { return this->mComment; }
				void isComment(bool comment){ this->mComment = comment; }

				std::pair<bool, std::string> attribute(const std::string &attr) const
				{ 
					std::map<std::string, std::string>::const_iterator i = this->mAttributes.find(attr);
					if (i != this->mAttributes.end()) {
						return make_pair(true, i->second);
					} else {
						return make_pair(false, std::string());
					}
				}

				operator std::string() const;
				std::ostream &operator<<(std::ostream &stream) const;

				const std::map<std::string, std::string>& attributes() const { return this->mAttributes; }
				void parseAttributes();

				bool operator==(const Node &rhs) const;

			protected:

				std::string mText; //节点的内部文本
				std::string mClosingText;
				unsigned int mOffset;//节点在原文档中的偏移值
				unsigned int mLength; //节点的字符长度
				std::string mTagName; //节点的Tag名
				std::map<std::string, std::string> mAttributes;//存放属性的二维数组
				bool mIsHtmlTag;//节点是否是HtmlTag标签
				bool mComment; //节点是否是注释
		};
	}
}

#endif
