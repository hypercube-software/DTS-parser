// WAVReader.h: interface for the WAVReader class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WAVREADER_H__7D2389F3_1FB3_487A_A3DF_586E84A59F7E__INCLUDED_)
#define AFX_WAVREADER_H__7D2389F3_1FB3_487A_A3DF_586E84A59F7E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <windows.h>
#include <mmsystem.h>
#include <stdio.h>

class WAVReader
{
public:
	virtual long ReadDTSStream(unsigned char *buffer, long length);
	void PrintFormat();
	virtual bool Open(char *filename);
	long ReadWAVStream(char *buffer,long length);
	virtual void Close();
	WAVReader();
	virtual ~WAVReader();
	LONG m_startoffset;
private:
	bool SearchFirstDTSWAVFrame();
	void ResetDTSDecoder();
	HMMIO       m_hmmio;              /* file handle for open file */
	MMCKINFO    m_mmckinfoParent;		/* parent chunk information structure */
	MMCKINFO    m_mmckinfoSubchunk;	/* subchunk information structure */
	/* WAVEFORMATEX structure for reading in the WAVE fmt chunk */
	WAVEFORMATEX m_WaveFormat;
	long m_WaveDataSize;

	char *m_buffer;
	int m_buffer_pos;
	int m_buffer_size;
	unsigned char outchar;
	int bitpos;
	short v;
	int b;
	bool m_16_bit_mode;
};

#endif // !defined(AFX_WAVREADER_H__7D2389F3_1FB3_487A_A3DF_586E84A59F7E__INCLUDED_)
