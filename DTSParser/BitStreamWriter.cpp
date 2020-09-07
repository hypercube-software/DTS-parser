// BitStreamWriter.cpp: implementation of the BitStreamWriter class.
//
//////////////////////////////////////////////////////////////////////

#include "framework.h"
#include "BitStreamWriter.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

BitStreamWriter::BitStreamWriter()
{
	m_id = NULL;
	m_filename = "";
	m_bitbuffer_size = 2048;
	m_bitbuffer = new unsigned char[m_bitbuffer_size];
}

BitStreamWriter::~BitStreamWriter()
{
	Close();
	if (m_bitbuffer!=NULL)
	{
		delete m_bitbuffer;
		m_bitbuffer = NULL;
	}
}
void BitStreamWriter::WriteBits(int value,int count)
{
	for (int i=count-1;i>=0;i--)
	{
		int mask = 1<<i;
		if ((value & mask)!=0)
		{
			WriteBit(1);
		}
		else
		{
			WriteBit(0);
		}
	}
}
void BitStreamWriter::WriteBit(int value)
{
	int mask = 0;
	if (value==1)
	{
		mask = 1<<m_bitcount;
	}
	m_bitbuffer[m_bytepos] |= mask;

	m_bitcount--;
	if (m_bitcount==-1)
	{
		m_bitcount = 7;
		m_bytepos++;
		if (m_bytepos==m_bitbuffer_size)
		{
			SaveBuffer();
		}
	}
}
bool BitStreamWriter::Open(CString filename)
{
	Close();
	m_id = NULL;
	fopen_s(&m_id,filename.GetBuffer(0), "wb");
	if (m_id!=NULL)
	{
		ResetBitStream();
		return true;
	}
	else
	{
		return false;
	}
}
void BitStreamWriter::Close()
{
	if (m_bitcount!=7)
	{
		SaveBuffer();
	}
	if (m_id!=NULL)
	{
		fclose(m_id);
		m_id=NULL;
	}

}

void BitStreamWriter::ResetBitStream()
{
	m_bitcount = 7;
	m_bytepos = 0;
	ResetBitBuffer();
}

void BitStreamWriter::SaveBuffer()
{
	if (m_id!=NULL)
	{
		int remainingBytes = 0;
		if (m_bitcount!=7)
		{
			remainingBytes = 1;
		}
		fwrite(m_bitbuffer,1,m_bytepos+remainingBytes,m_id);
		ResetBitStream();
	}
}

void BitStreamWriter::ResetBitBuffer()
{
	memset(m_bitbuffer,0,m_bitbuffer_size);
}
