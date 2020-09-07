// BitStreamWriter.h: interface for the BitStreamWriter class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BITSTREAMWRITER_H__5F200465_3B1C_4C7B_BDDB_B7930D4DA18F__INCLUDED_)
#define AFX_BITSTREAMWRITER_H__5F200465_3B1C_4C7B_BDDB_B7930D4DA18F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class BitStreamWriter  
{
public:
	void WriteBits(int value,int count);
	bool Open(CString filename);	
	void WriteBit(int value);
	void Close();

	BitStreamWriter();
	virtual ~BitStreamWriter();
private:
	void ResetBitBuffer();
	void SaveBuffer();
	void ResetBitStream();
	CString m_filename;
	FILE *m_id;
	
	int m_bitcount;
	int m_bytepos;
	int m_bitbuffer_size;
	unsigned char *m_bitbuffer;
};

#endif // !defined(AFX_BITSTREAMWRITER_H__5F200465_3B1C_4C7B_BDDB_B7930D4DA18F__INCLUDED_)
