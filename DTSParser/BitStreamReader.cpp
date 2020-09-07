// BitStreamReader.cpp: implementation of the BitStreamReader class.
//
//////////////////////////////////////////////////////////////////////

#include "framework.h"
#include "BitStreamReader.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

BitStreamReader::BitStreamReader()
{
	m_id = NULL;
	m_filename = "";
	m_bitbuffer_size = 2048;
	m_bitbuffer = new unsigned char[m_bitbuffer_size];
	m_bitcounter = 0;
	m_swap_word = false;
}

BitStreamReader::~BitStreamReader()
{
	Close();
	if (m_bitbuffer!=NULL)
	{
		delete[] m_bitbuffer;
		m_bitbuffer = NULL;
	}
}

bool BitStreamReader::Open(CString filename)
{
	m_usewav = false;
	Close();
	Reset14bitDecoder();
	m_id = NULL;
	fopen_s(&m_id,filename.GetBuffer(0), "rb");
	if (m_id!=NULL)
	{
		unsigned char header[8];
		fread(header,1,8,m_id);
		if( strncmp((char*)header,"RIFF",4)==0)
		{
			m_usewav = true;
			m_16bitmode = true;
		}
		else if (
				(header[0] == 0x1F) &&
				(header[1] == 0xFF) &&
				(header[2] == 0xE8) &&
				(header[3] == 0x00) &&
				(header[4] == 0x07) &&
				((header[5]&0xF0) == 0xF0)
				)
		{
			m_16bitmode = false;
		}
		else
		{
			m_16bitmode = true;

		}

		fseek(m_id,0,SEEK_SET);
		if (m_usewav)
		{
			// RIFF WAV
			if (m_wav.Open(filename.GetBuffer(0)))
			{
				ResetBitStream();
				return true;
			}
			else
			{
				return false;
			}
		}
		else
		{
			ResetBitStream();
			return true;
		}
	}
	else
	{
		return false;
	}
}
	
int BitStreamReader::ReadBit()
{
	if (m_bitpos==-1)
	{
		m_bytepos++;
		if (m_bytepos == m_bitbuffer_size)
		{
			LoadBuffer();
		}
		m_bitpos = 7;
	}
	int mask = (1<<m_bitpos);
	int bit = m_bitbuffer[m_bytepos] & mask;
	m_bitpos--;
	m_bitcounter++;
	if (m_bitpos==-1)
	{
		m_bytecounter++;
	}
	if (bit!=0)
		return 1;
	else
		return 0;
}
void BitStreamReader::Close()
{
	if (m_usewav)
	{
		m_wav.Close();
	}
	else
	{
		if (m_id!=NULL)
		{
			fclose(m_id);
			m_id=NULL;
		}
	}
}

unsigned int BitStreamReader::ReadBits(int count)
{
	unsigned int v=0;
	unsigned int bit = 0;
	for (int b=0;b<count;b++)
	{
		bit = ReadBit();
		v = (v<<1) | bit;
	}
	return v;		
}

int BitStreamReader::TestNextBits(char *bits)
{
	int v = 0;
	int l = 0;
	
	v = BitStreamReader::GetValue(bits,l);

	int prevbytecounter = m_bytecounter;
	int prevbitcounter = m_bitcounter;
	int prevbitpos = m_bitpos;
	int prevbytepos = m_bytepos;
	int next_bits;
	long prevpos = ftell(m_id);

	if (prevpos==0)
	{
		// no buffer have been already read
		next_bits = ReadBits(l);
		fseek(m_id,prevpos,SEEK_SET);
	}
	else
	{
		// at least one buffer have been already read
		next_bits = ReadBits(l);
		prevpos -= m_bitbuffer_size;
		fseek(m_id,prevpos,SEEK_SET);
		LoadBuffer();
	}
	m_bytecounter = prevbytecounter;
	m_bitcounter = prevbitcounter;
	m_bitpos = prevbitpos;
	m_bytepos = prevbytepos;
	
	if (next_bits == v)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}



int BitStreamReader::GetValue(char *bits)
{
	int l;
	return BitStreamReader::GetValue(bits,l);
}
int BitStreamReader::GetValue(char *bits,int &l)
{
	// strings like "0000 1011 1001" can be use.
	int v=0;
	int i=0;
	int sl = strlen(bits);
	for (i=0;i<sl;i++)
	{
		if (bits[i] == '0')
		{
			v = (v<<1) | 0;
			l++;
		}
		else if (bits[i]=='1')
		{
			v = (v<<1) | 1;
			l++;
		}
	}
	return v;
}
/*
Extract from MPEG2 norm:

The function bytealigned () returns 1 if the current position is 
on a byte boundary, that is the next bit in the bitstream is the 
first bit in a byte. Otherwise it returns 0.

*/
bool BitStreamReader::isByteAligned()
{
	if (m_bitpos==7)
	{
		return true;
	}
	else
	{
		return false;
	}
}

long BitStreamReader::BytePosition()
{
	return m_bytecounter;
}
void BitStreamReader::Reset14bitDecoder()
{
	m_buffer_size = m_bitbuffer_size;
	outchar = 0;
	bitpos = 7;
	m_buffer_pos = m_buffer_size;
	b = -1;
}
void BitStreamReader::LoadBuffer()
{
	int nb=0;
	if (m_usewav)
	{
		nb = m_wav.ReadDTSStream(m_bitbuffer,m_bitbuffer_size);
	}
	else
	{
		if (m_16bitmode)
		{
			nb = fread(m_bitbuffer,1,m_bitbuffer_size,m_id);
			if (m_swap_word)
			{
				for (int i=0;i<m_bitbuffer_size;)
				{
					char v = m_bitbuffer[i];
					m_bitbuffer[i] = m_bitbuffer[i+1];
					m_bitbuffer[i+1] = v;
					i+=2;
				}
			}
		}
		else
		{
			unsigned char *buffer = m_bitbuffer;
			int count = 0;
			short *ptr=(short*)m_bitbuffer;
			while (count!=m_buffer_size)
			{
				if (b==-1)
				{
					if (m_buffer_pos>=m_buffer_size)
					{
						long r = fread(m_bitbuffer,1,m_buffer_size,m_id);
						if (r<=0)
						{
							ResetBitStream(); // very important, otherwise we loose the fact that we are in EOF
							throw EOFBSException();
						}
						if (m_swap_word)
						{
							for (int i=0;i<m_buffer_size;)
							{
								char v = m_bitbuffer[i];
								m_bitbuffer[i] = m_bitbuffer[i+1];
								m_bitbuffer[i+1] = v;
								i+=2;
							}
						}
						m_buffer_pos = 0;
					}
					v = (ptr[m_buffer_pos++] & 0x3FFF) ; // 14 bits DTS data
					b = 13;
				}
				short mask = (1<<b);
				short bit = v & mask;
				if (bit)
				{
					unsigned char bmask = (1<<bitpos);
					outchar |= bmask;
				}
				bitpos--;
				b--;
				if (bitpos==-1)
				{
					*buffer++ = outchar;
					count++;
					outchar=0;
					bitpos = 7;
				}
			}
			nb = count;
		}
	}
	
	if (nb<=0)
	{
		ResetBitStream(); // very important, otherwise we loose the fact that we are in EOF
		throw EOFBSException();
	}
	else if (nb !=m_bitbuffer_size)
	{
		// last read before EOF
		m_bitbuffer_size = nb;
	}
	m_bytepos = 0;
	m_bitpos = 7;
}

void BitStreamReader::ResetBitStream()
{
	m_bitpos = -1;
	m_bytepos = m_bitbuffer_size-1;
	if (m_usewav)
	{
		m_bytecounter = m_wav.m_startoffset;
	}
	else
	{
		m_bytecounter = 0;
	}
	m_bitcounter = m_bytecounter*8;
}

void BitStreamReader::SetSwapWord()
{
		m_swap_word = true;
}

void BitStreamReader::UnsetSwapWord()
{
		m_swap_word = false;
}
