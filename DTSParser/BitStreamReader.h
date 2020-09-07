// BitStreamReader.h: interface for the BitStreamReader class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BITSTREAMREADER_H__B3150F49_FB9E_4BB7_AC04_574176498CC1__INCLUDED_)
#define AFX_BITSTREAMREADER_H__B3150F49_FB9E_4BB7_AC04_574176498CC1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <stdio.h>
#include "EOFBSException.h"
#include "WAVReader.h"

class BitStreamReader  
{
public:
	static int Debug();
	bool isByteAligned();
	static int GetValue(char *bits,int &l);
	static int GetValue(char *bits);


	int TestNextBits(char *bits);
	unsigned int ReadBits(int count);
	bool Open(CString filename);
	void Close();
	BitStreamReader();
	int ReadBit();
	virtual ~BitStreamReader();
	unsigned int m_bitcounter;
	unsigned int m_bytecounter;
public:
	void UnsetSwapWord();
	void SetSwapWord();
	long BytePosition();
	FILE *m_id;
	void ResetBitStream();
private:
	void LoadBuffer();

	CString m_filename;
	void Reset14bitDecoder();

	int m_bitpos;
	int m_bytepos;
	int m_bitbuffer_size;
	unsigned char *m_bitbuffer;

	bool m_usewav;
	WAVReader m_wav;

	bool m_swap_word;
	bool m_16bitmode;

	char *m_buffer;
	int m_buffer_pos;
	int m_buffer_size;
	unsigned char outchar;
	int bitpos;
	short v;
	int b;
};

#endif // !defined(AFX_BITSTREAMREADER_H__B3150F49_FB9E_4BB7_AC04_574176498CC1__INCLUDED_)
