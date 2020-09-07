// WAVReader.cpp: implementation of the WAVReader class.
//
//////////////////////////////////////////////////////////////////////

#include "WAVReader.h"
#include <stdio.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

WAVReader::WAVReader()
{
	m_buffer_size = 2048;
	m_buffer = new char[m_buffer_size*2];
}

WAVReader::~WAVReader()
{
	delete[] m_buffer;
	Close();
}
long WAVReader::ReadWAVStream(char *buffer,long length)
{
	return mmioRead(m_hmmio, (HPSTR)buffer, length);
}
void WAVReader::Close()
{
	if (m_hmmio!=NULL)
	{
		mmioClose(m_hmmio, 0);
		m_hmmio = NULL;
	}
}
bool WAVReader::Open(char *filename)
{
	m_startoffset=0;
	ResetDTSDecoder();
	m_hmmio = mmioOpen(filename, 0, MMIO_READ|MMIO_ALLOCBUF);
	if (m_hmmio==NULL)
	{
		return false;
	}
	m_mmckinfoParent.fccType = mmioFOURCC('W', 'A', 'V', 'E'); 
	if (mmioDescend(m_hmmio, (LPMMCKINFO)&m_mmckinfoParent, 0, MMIO_FINDRIFF)!=MMSYSERR_NOERROR)
	{
		Close();
		return false;
	}
	m_mmckinfoSubchunk.ckid = mmioFOURCC('f', 'm', 't', ' '); 
	if (mmioDescend(m_hmmio, &m_mmckinfoSubchunk, &m_mmckinfoParent, MMIO_FINDCHUNK)!=MMSYSERR_NOERROR)
	{		
		Close();
		return false;
	}
	if (mmioRead(m_hmmio, (HPSTR)&m_WaveFormat, m_mmckinfoSubchunk.cksize) != (LRESULT)m_mmckinfoSubchunk.cksize)
	{
		Close();
		return false;
	}

	if (mmioAscend(m_hmmio, &m_mmckinfoSubchunk, 0)!=MMSYSERR_NOERROR)
	{
		Close();
		return false;
	}
	m_mmckinfoSubchunk.ckid = mmioFOURCC('d', 'a', 't', 'a'); 
	if (mmioDescend(m_hmmio, &m_mmckinfoSubchunk, &m_mmckinfoParent, MMIO_FINDCHUNK)!=MMSYSERR_NOERROR) 
	{
		Close();
		return false;
	}
	/* Get the size of the data chunk (ie, the size of the waveform data) */
	m_WaveDataSize = m_mmckinfoSubchunk.cksize;
	
	SearchFirstDTSWAVFrame();

	return true;
}

void WAVReader::PrintFormat()
{
	printf("%d channels %ld Hz %d bits\n",
		(int)m_WaveFormat.nChannels,
		(long)m_WaveFormat.nSamplesPerSec,
		(int)m_WaveFormat.wBitsPerSample);

}

long WAVReader::ReadDTSStream(unsigned char *buffer, long length)
{
	int count = 0;
	if (m_16_bit_mode)
	{
		count = ReadWAVStream((char*)buffer,length);
	}
	else
	{
		short *ptr=(short*)m_buffer;
		while (count!=length)
		{
			if (b==-1)
			{
				if (m_buffer_pos>=m_buffer_size)
				{
					long r = ReadWAVStream(m_buffer,m_buffer_size*2);
					if (r<=0)
						return -1;
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
	}
	return count; 
}

void WAVReader::ResetDTSDecoder()
{
	outchar = 0;
	bitpos = 7;
	m_buffer_pos = m_buffer_size;
	b = -1;
}

bool WAVReader::SearchFirstDTSWAVFrame()
{
	unsigned short sample;
	int STATE = 0;
	bool found = false;
	LONG startpos = mmioSeek(m_hmmio,0,SEEK_CUR);
	while (!found)
	{
		int r = ReadWAVStream((char*)&sample,sizeof(short));
		if (r!=sizeof(short))
			return false;
		switch (STATE)
		{
		case 0 : 
			if (sample == 0x1FFF)
			{
				STATE = 1;
			}
			else if (sample == 0x7FFE)
			{
				STATE = 2;
			}
			break;
		case 1 :
			if (sample == 0xE800)
			{
				found = true;
				m_16_bit_mode = false;
			}
			else
			{
				STATE = 0;
			}
			break;
		case 2 :
			if (sample == 0x8001)
			{
				found = true;
				m_16_bit_mode = true;
			}
			else
			{
				STATE = 0;
			}
			break;
		}
	}
	LONG curpos = mmioSeek(m_hmmio,-4,SEEK_CUR);
	if (curpos!=-1)
	{
		m_startoffset = curpos-startpos;
		return true;
	}
	else
		return false;
}
