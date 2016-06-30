#ifndef __FILECOMPRESSION_H__
#define __FILECOMPRESSION_H__

#include <string>
#include <cassert>

typedef long long LongType;

struct CharInfo
{
	CharInfo(LongType count = 0)
		:_count(count)
	{}
	unsigned char _ch;	//字符
	LongType _count;	//字符出现次数
	string _code;		//哈夫曼编码

	/*重载 > 运算符*/
	bool operator > (const CharInfo& com)const 
	{
		return _count > com._count;
	}
	/*重载 < 运算符*/
	bool operator < (const CharInfo& com)const 
	{
		return _count < com._count;
	}
	/*重载 != 运算符*/
	bool operator != (CharInfo com)const
	{
		return _count != com._count;
	}
	/*重载 + 运算符*/
	CharInfo operator + (const CharInfo& com)const
	{
		return CharInfo(_count + com._count);
	}
};

class FileCompress
{
public:
	FileCompress()
	{
		for(int i = 0; i < 256; ++i)
		{
			_Info[i]._ch = i;
		}
	}
public:
	//压缩
	void Compress(const char* file)
	{
		//打开文件
		FILE* fOut = fopen(file, "rb");
		assert(fOut);
		unsigned long long ChCount = 0;
		//统计每个字符出现的次数
		char ch = fgetc(fOut);
		while(ch != EOF)
		{
			++ChCount;
			_Info[(unsigned char)ch]._count++;
			ch = fgetc(fOut);
		}

		//重建哈夫曼树
		CharInfo invalid(0); //定义非法值，过滤未出现的字符
		HaffmanTree<CharInfo> ht(_Info, 256, invalid);
		
		//生成哈夫曼编码
		string code;
		_CreateHaffCode(ht.Getroot(), code);
		
				
		//压缩后的文件名
		string comfile = file;
		comfile += ".compress";
		FILE* fIn = fopen(comfile.c_str(), "wb");
		assert(fIn);

		//从文件首字符进行压缩
		fseek(fOut, 0, SEEK_SET);
		unsigned char value = 0;
		size_t index = 0;
		ch = fgetc(fOut);
		while(ch != EOF)
		{
			string& code = _Info[(unsigned char)ch]._code;
			for(size_t i = 0; i < code.size(); ++i)
			{
				if(code[i] == '1')
				{
					value |= 1;
				}
				//满8位将其写入压缩文件中
				if(++index == 8)
				{
					fputc(value, fIn);
					value = 0;
					index = 0;
				}
				value <<= 1;
			}
			ch = fgetc(fOut);
		}
		//处理没有满8位，后面位补0
		if(index != 0)
		{
			value <<= (7-index);
			fputc(value, fIn);
		}
		
		//写配置文件
		string config = file;
		config += ".config";
		FILE* fConfig = fopen(config.c_str(), "wb");
		assert(fConfig);

		//写入字符总个数
		char buffer[128];
		itoa(ChCount, buffer, 10);
		fputs(buffer, fConfig);
		fputc('\n', fConfig);

		//写入：字符，次数
		string line;

		for(int i = 0; i < 256; ++i)
		{
			if(_Info[i]._count > 0)
			{
				//line格式控制
				line += _Info[i]._ch;
				line += ",";
				itoa(_Info[i]._count, buffer, 10);
				line += buffer;
				line += '\n';

				fputs(line.c_str(), fConfig);	//将line写入配置文件
			}
			line.clear(); //清空line
		}

		fclose(fOut);
		fclose(fIn);
		fclose(fConfig);
	}
	void UnCompress(const char* file)
	{
		//读取配置文件，进行解压缩
		string config = file;
		config += ".config";
		FILE* fConfig = fopen(config.c_str(), "rb");
		assert(fConfig);

		//读取原文件字符总个数
		string line;
		unsigned long long ChCount = 0;
		
		if(ReadLine(fConfig, line))
		{
			ChCount += atoi(line.c_str());
			line.clear();
			if(ChCount == 0)
			{
				return;
			}
		}
		else
		{
			return;
		}

		while(ReadLine(fConfig, line))
		{
			if(line.empty())
			{
				line += '\n';
			}
			else
			{
				_Info[(unsigned char)line[0]]._count = atoi(line.substr(2).c_str());
				line.clear(); //一定要进行清空操作，否则就会将所有的坠在一行上
			}
		}

		//重建HaffmanTree
		CharInfo invalid(0);
		HaffmanTree<CharInfo> ht(_Info, 256, invalid);

		//读取压缩文件，进行解压缩
		string uncompressfile = file;
		uncompressfile += ".uncompress";
		FILE* fWrite = fopen(uncompressfile.c_str(), "wb");
		assert(fWrite);

		//还原好的文件
		string compressfile = file;
		compressfile += ".compress";
		FILE* fOut = fopen(compressfile.c_str(), "rb");
		assert(fOut);

		HaffmanTreeNode<CharInfo>* root = ht.Getroot();
		HaffmanTreeNode<CharInfo>* cur = root;

		char ch = fgetc(fOut);
		int pos = 7;

		while(/*ch != EOF*/ChCount != 0)
		{
			
			if((unsigned char)ch & (1<<pos))
			{
				cur = cur->_right;
			}
			else
			{
				cur = cur->_left;
			}
			if(cur->_left == NULL && cur->_right == NULL)
			{
				fputc(cur->_weight._ch, fWrite);
				cur = root;
				if(--ChCount == 0)
				{
					return;
				}
			}


			if(pos-- == 0)
			{
				ch = fgetc(fOut);
				pos = 7;
			}
		}

		fclose(fOut);
		fclose(fWrite);
		fclose(fConfig);
	}
protected:
	//生成哈夫曼编码
	void _CreateHaffCode(HaffmanTreeNode<CharInfo>* root, string& code)
	{
		if(root == NULL)
		{	
			return;
		}
		if(root->_left == NULL && root->_right == NULL)
		{
			_Info[root->_weight._ch]._code = code;
			return;
		}
		_CreateHaffCode(root->_left, code + '0'); 
		_CreateHaffCode(root->_right, code + '1');

	}

	bool ReadLine(FILE* fOut, string& line)
	{
		char ch = fgetc(fOut);
		if(ch == EOF)
			return false;
		while(ch != EOF && ch != '\n')
		{
			line += ch;
			ch = fgetc(fOut);
		}
		return true;
	}
private:
	CharInfo _Info[256];
};


#endif