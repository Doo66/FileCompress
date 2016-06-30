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
	unsigned char _ch;	//�ַ�
	LongType _count;	//�ַ����ִ���
	string _code;		//����������

	/*���� > �����*/
	bool operator > (const CharInfo& com)const 
	{
		return _count > com._count;
	}
	/*���� < �����*/
	bool operator < (const CharInfo& com)const 
	{
		return _count < com._count;
	}
	/*���� != �����*/
	bool operator != (CharInfo com)const
	{
		return _count != com._count;
	}
	/*���� + �����*/
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
	//ѹ��
	void Compress(const char* file)
	{
		//���ļ�
		FILE* fOut = fopen(file, "rb");
		assert(fOut);
		unsigned long long ChCount = 0;
		//ͳ��ÿ���ַ����ֵĴ���
		char ch = fgetc(fOut);
		while(ch != EOF)
		{
			++ChCount;
			_Info[(unsigned char)ch]._count++;
			ch = fgetc(fOut);
		}

		//�ؽ���������
		CharInfo invalid(0); //����Ƿ�ֵ������δ���ֵ��ַ�
		HaffmanTree<CharInfo> ht(_Info, 256, invalid);
		
		//���ɹ���������
		string code;
		_CreateHaffCode(ht.Getroot(), code);
		
				
		//ѹ������ļ���
		string comfile = file;
		comfile += ".compress";
		FILE* fIn = fopen(comfile.c_str(), "wb");
		assert(fIn);

		//���ļ����ַ�����ѹ��
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
				//��8λ����д��ѹ���ļ���
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
		//����û����8λ������λ��0
		if(index != 0)
		{
			value <<= (7-index);
			fputc(value, fIn);
		}
		
		//д�����ļ�
		string config = file;
		config += ".config";
		FILE* fConfig = fopen(config.c_str(), "wb");
		assert(fConfig);

		//д���ַ��ܸ���
		char buffer[128];
		itoa(ChCount, buffer, 10);
		fputs(buffer, fConfig);
		fputc('\n', fConfig);

		//д�룺�ַ�������
		string line;

		for(int i = 0; i < 256; ++i)
		{
			if(_Info[i]._count > 0)
			{
				//line��ʽ����
				line += _Info[i]._ch;
				line += ",";
				itoa(_Info[i]._count, buffer, 10);
				line += buffer;
				line += '\n';

				fputs(line.c_str(), fConfig);	//��lineд�������ļ�
			}
			line.clear(); //���line
		}

		fclose(fOut);
		fclose(fIn);
		fclose(fConfig);
	}
	void UnCompress(const char* file)
	{
		//��ȡ�����ļ������н�ѹ��
		string config = file;
		config += ".config";
		FILE* fConfig = fopen(config.c_str(), "rb");
		assert(fConfig);

		//��ȡԭ�ļ��ַ��ܸ���
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
				line.clear(); //һ��Ҫ������ղ���������ͻὫ���е�׹��һ����
			}
		}

		//�ؽ�HaffmanTree
		CharInfo invalid(0);
		HaffmanTree<CharInfo> ht(_Info, 256, invalid);

		//��ȡѹ���ļ������н�ѹ��
		string uncompressfile = file;
		uncompressfile += ".uncompress";
		FILE* fWrite = fopen(uncompressfile.c_str(), "wb");
		assert(fWrite);

		//��ԭ�õ��ļ�
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
	//���ɹ���������
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