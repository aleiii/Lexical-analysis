#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <unordered_set>


namespace ALEI
{
	// 关键字集合
	const std::unordered_set<std::string> keywords_set
	{
		"auto", "break", "case", "char", "const", "continue",
		"default", "do", "double", "else", "enum", "extern",
		"float", "for", "goto", "if", "int", "long", "register",
		"return", "short", "signed", "sizeof", "static", "struct",
		"switch", "typedef", "union", "unsigned", "void", "volatile",
		"while"
	};

	// 标点符号集合
	const std::unordered_set<std::string> punctuator_set
	{
		"[", "]", "(", ")", "{", "}", ".", "->",
		"++", "--", "&", "*", "+", "-", "~", "!",
		"/", "%", "<<", ">>", "<", ">", "<=", ">=",
		"==", "!=", "^", "|", "&&", "||", "?", ":",
		",", ";", "...", "=", "*=", "/=", "%=", 
		"+=", "-=", "<<=", ">>=", "&=", "^=", "|="
	};

	// 转义字符集合
	const std::unordered_set<std::string> escape_set
	{
		"\\'", "\\\"", "\\?", "\\\\",
		"\\a", "\\b", "\\f", "\\n",
		"\\r", "\\t", "\\v"
	};

	// 枚举类
	enum class Tokens : unsigned int 
	{
		identifier,
		keyword,
		constant,
		punctuator
	};

	// 自定义hash函数，返回枚举类的值
	class hashToken
	{
	public:
		size_t operator()(const Tokens& token) const
		{
			return std::underlying_type<Tokens>::type(token);
		}
	};

	// token字典，获取枚举类的字符串表示
	const std::unordered_map<Tokens, std::string, hashToken> token_dict =
	{
		{Tokens::identifier, "Identifier"},
		{Tokens::keyword, "Keyword"},
		{Tokens::constant, "constant"},
		{Tokens::punctuator, "Punctuator"}
	};

	// 判断是否为空白符
	bool isWhite(const char &ch)
	{
		return (ch == ' ' || ch == '\n' || ch == '\t' || ch == '\r');
	}

	// 判断是否是关键字
	bool isKeyword(const std::string &str)
	{
		return (keywords_set.find(str) != keywords_set.end());
	}

	// 判断是否是标点符号
	bool isPunctuator(const std::string& str)
	{
		return (punctuator_set.find(str) != punctuator_set.end());
	}

	// 主循环
	void start(std::ifstream &infile, std::ofstream &outfile)
	{
		char ch;
		std::vector<std::pair<Tokens, std::string>> tokens;
		std::vector<std::string> errors;

		// 保存token
		auto addToken = [&tokens](Tokens token, std::string str)
		{
			tokens.emplace_back(token, str);
		};

		// 保存错误
		auto addError = [&errors](std::string str)
		{
			errors.emplace_back(str);
		};

		// 读取标识符
		auto readIdentifier = [&infile, &addToken](std::string &str)
		{
			char ch_p;
			while (true)
			{
				ch_p = infile.peek();
				if (!isalpha(ch_p) && !isdigit(ch_p) && ch_p != '_')
					break;
				str += infile.get();
			}
			// 判断读取的字符串是否为关键字或标识符
			if (isKeyword(str))
				addToken(Tokens::keyword, str);
			else
				addToken(Tokens::identifier, str);
		};
		
		// 读取数字
		auto readNum = [&infile, &addToken](std::string &str)
		{
			char ch_p;
			size_t state = 1;
			bool is_end = false;
			while (true)
			{
				ch_p = infile.peek();
				switch (state)
				{
				case 1:
					if (isdigit(ch_p))
						;
					else if (ch_p == 'E' || ch_p == 'e')
						state = 4;
					else if (ch_p == '.')
						state = 2;
					else
						is_end = true;
					break;
				case 2:
					if (isdigit(ch_p))
						state = 3;
					else
						is_end = true;
					break;
				case 3:
					if (isdigit(ch_p))
						;
					else if (ch_p == 'E' || ch_p == 'e')
						state = 4;
					else
						is_end = true;
					break;
				case 4:
					if (isdigit(ch_p))
						state = 6;
					else if (ch_p == '+' || ch_p == '-')
						state = 5;
					else
						is_end = true;
					break;
				case 5:
					if (isdigit(ch_p))
						state = 6;
					else
						is_end = true;
					break;
				case 6:
					if (isdigit(ch_p))
						;
					else
						is_end = true;
					break;
				default:
					exit(-1);
					break;
				} // end of switch
				if (is_end)
					break;
				str += infile.get();
			} // end of while
			// 无符号数都为常量
			addToken(Tokens::constant, str);
		};

		// 读取标点
		auto readPunctuator = [&infile, &addToken, &addError, &ch](std::string& str)
		{
			if (ch == '/') // 注释或运算符
			{
				char ch_p;
				ch_p = infile.peek();
				if (ch_p == '/')
				{
					// 读取掉整行注释
					while (true)
					{
						if (!infile.get(ch_p))
							break;
						if (ch_p == '\n')
							break;
					}
				}
				else if (ch_p == '*')
				{
					infile.get();
					while (true)
					{
						if (!infile.get(ch_p))
							break;
						if (ch_p == '*' && infile.peek() == '/')
						{
							infile.get();
							break;
						}
					}
				}
				else
					addToken(Tokens::punctuator, str);
			}
			else
			{
				char ch_p = infile.peek();
				// ...
				if (ch == '.' && ch_p == '.')
				{
					str += infile.get();
					if (infile.peek() == '.')
					{
						str += infile.get();
						addToken(Tokens::punctuator, str);
					}
					else
						addError("Invalid punctuator: " + str);
				}
				else
				{
					while (true)
					{
						if (!isPunctuator(str + ch_p))
							break;
						str += infile.get();
						ch_p = infile.peek();
					}
					addToken(Tokens::punctuator, str);
				}
			}
			
			//addToken(Tokens::punctuator, str);
			//else
			//	std::cout << "---------" << str << "--------" << std::endl;
		};
		
		// 读取字符串
		auto readString = [&infile, &addToken, &addError, &ch](std::string& str)
		{
			char ch_p = ch;
			while (true)
			{
				ch = infile.get();
				if (ch == EOF || ch == '\n') // EOF
				{
					addError("No end bracket: " + str);
					break;
				}
				str += ch;
				if (ch == ch_p) // 引号匹配成功
				{
					addToken(Tokens::constant, str);
					break;
				}
				else if (ch == '\\') // 转义字符
				{
					if (infile.peek() == '\n')
					{
						str.pop_back();
						infile.get();
					}
					else
					{
						std::string esc(1, ch);
						esc += infile.peek();
						if (escape_set.find(esc) != escape_set.end())
							str += infile.get();
						else
						{
							str.pop_back();
							infile.get();
							addError("Invalid escape: " + esc);
						}
					}
				}
			}
		};

		while (true)
		{
			// 保存读取的字符
			std::string str;
			// EOF
			if (!infile.get(ch))
				break;
			str += ch;
			bool a = (ch == '\'');
			bool b = (ch == '\"');
			bool c = (ch == '"');
			// 判断空白符
			if (isWhite(ch))
			{
				// Nothing to do
			}
			else if (isalpha(ch) || ch == '_') // 标识符和关键字的读取
			{
				readIdentifier(str);
			}
			else if (isdigit(ch)) // 无符号数的读取
			{ 
				readNum(str);
			}
			else if (isPunctuator(str)) // 标点符号，注释及字符串常量的读取
			{
				readPunctuator(str);
			}
			else if (ch == '\'' || ch == '\"') // 字符串
			{
				readString(str);
			}
			else if (ch == '\\') // 单独的'\'
			{
				if (infile.peek() == '\\')
				{
					infile.get();
					addError("Error char: \\");
				}
			}
			else
			{
				std::cout << ch << std::endl;
				addError("Error char: " + ch);
			}
		}
		// 输出
		for (auto i : tokens)
		{
			std::cout << "< " << token_dict.at(i.first) << ", " << i.second << " >" << std::endl;
			outfile << "< " << token_dict.at(i.first) << ", " << i.second << " >" << std::endl;
		}
		std::cout << "-----------------------" << std::endl << "-----------------------" << std::endl;
		outfile << "-----------------------" << std::endl << "-----------------------" << std::endl;
		for (auto i : errors)
		{
			std::cout << "< " << i << " >" << std::endl;
			outfile << "< " << i << " >" << std::endl;
		}
	}
}

int main(int argc, char* argv[])
{
	std::string file = "D:/learn/project_work/Compiler-principles/Lexer/Lexer/test.c";
	std::ifstream infile(file);
	std::ofstream outfile(file+".txt");
	ALEI::start(infile, outfile);
	return 0;
}