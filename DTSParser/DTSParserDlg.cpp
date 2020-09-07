
// DTSParserDlg.cpp : implementation file
//

#include "framework.h"
#include "DTSParser.h"
#include "DTSParserDlg.h"
#include "afxdialogex.h"
#include <direct.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
//	tables DTS
/////////////////////////////////////////////////////////////////////////////

char* AMODE[] =
{
	"A (mono)",
	"A+B (dual mono)",
	"L+R (stereo)",
	"(L+R) + (L-R) (sum-difference)",
	"LT+RT (left and right total)",
	"C+L+R",
	"L+R+S",
	"C+L+R+S",
	"L+R+SL+SR",
	"C+L+R+SL+SR",
	"CL+CR+L+R+SL+SR",
	"C+L+R+LR+RR+OV",
	"CF+CR+LF+RD+LR+RR",
	"CL+C+CR+L+R+SL+SR",
	"CL+CR+L+R+SL1+SL2+SR1+SR2",
	"CL+C+CR+L+R+SL+S+SR",
};
int AMODE_nbChannels[] =
{
	1,
	2,
	2,
	2,
	2,
	3,
	3,
	4,
	4,
	5,
	6,
	6,
	6,
	7,
	8,
	8
};
double fFREQ[] = {
	0,
	8,
	16,
	32,
	0,
	0,
	11.025,
	22.050,
	44.100,
	0,
	0,
	12,
	24,
	48,
	0,
	0
};

char* SFREQ[] =
{
	"Invalid",
	"8 Khz",
	"16 Khz",
	"32 Khz",
	"Invalid",
	"Invalid",
	"11.025 Khz",
	"22.050 Khz",
	"44.100 Khz",
	"Invalid",
	"Invalid",
	"12 Khz",
	"24 Khz",
	"48 Khz",
	"Invalid",
	"Invalid"
};

char* RATE[] =
{
	"32",
	"56",
	"64",
	"96",
	"112",
	"128",
	"192",
	"224",
	"256",
	"320",
	"384",
	"448",
	"512",
	"576",
	"640",
	"768",
	"960",
	"1024",
	"1152",
	"1280",
	"1344",
	"1408",
	"1411.2",
	"1472",
	"1536",
	"1920",
	"2048",
	"3072",
	"3840",
	"open",
	"variable",
	"Loss-less",
	"768 / 754.50 DVD",
	"1411.2 / 1234.8 DTS-CD",
	"1536 / 1509.75 DVD"
};

int iPCMR[] =
{
	16,
	16,
	20,
	20,
	0,
	24,
	24,
	0
};

bool bPCMR_DTSES_MATRIX[] =
{
	false,
	true,
	false,
	true,
	false,
	true,
	false,
	false
};

CString GetDial(int vernum, int dial)
{
	CString sdial;
	int v = 0;
	if (vernum == 7)
		v = dial;
	else if (vernum == 6)
		v = dial + 16;
	sdial.Format("-%d dB", v);
	return sdial;
}

CString GetAMode(int amode)
{
	CString v;
	if (amode >= 0 && amode <= 0xF)
	{
		v = AMODE[amode];
	}
	else
	{
		v = "User defined";
	}
	return v;
}
int GetAModeNbChannels(int amode)
{
	int v;
	if (amode >= 0 && amode <= 0xF)
	{
		v = AMODE_nbChannels[amode];
	}
	else
	{
		v = 0;
	}
	return v;
}
CString GetSFreq(int sfreq, int extaudioid)
{
	bool DblFreq;
	if ((extaudioid & 2) == 0)
		DblFreq = false;
	else
		DblFreq = true;
	double freq = fFREQ[sfreq];
	if (DblFreq)
		freq = freq * 2;
	double y, n;
	y = modf(freq, &n);

	CString v;
	if (y != 0.0)
		v.Format("%f", freq);
	else
		v.Format("%d", (int)freq);
	return v;
}
CString GetRate(int rate)
{
	CString v = RATE[rate];
	return v;
}
CString GetFormat(int ext_audio, int extaudioid, int pcmr, int sfreq, int& nbchannels)
{
	bool  Xch;
	if (ext_audio == 1 && (extaudioid == 0 || extaudioid == 3))
		Xch = true;
	else
		Xch = false;


	bool DTSES_MATRIX = bPCMR_DTSES_MATRIX[pcmr];
	int bitdepth = iPCMR[pcmr];

	CString type;
	if (Xch && !DTSES_MATRIX)
	{
		type = "DTS ES Discrete";
		nbchannels++;
	}
	else if (!Xch && DTSES_MATRIX)
	{
		type = "DTS ES Matrix";
		nbchannels++;
	}
	else if (!Xch && !DTSES_MATRIX)
	{
		type = "DTS";
	}
	else if (Xch && DTSES_MATRIX)
	{
		type = "DTS ES Matrix & Discrete";
		nbchannels++;
	}

	CString format;

	format.Format("%s %d/%s", type, bitdepth, GetSFreq(sfreq, extaudioid));

	return format;
}

// CDTSParserDlg dialog



CDTSParserDlg::CDTSParserDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DTSPARSER_DIALOG, pParent)
{
	//{{AFX_DATA_INIT(CDTSParserDlg)
	m_swap_word = FALSE;
	//}}AFX_DATA_INIT
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CDTSParserDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDTSParserDlg)
	DDX_Control(pDX, IDC_PROGRESS, m_ctrlProgress);
	DDX_Control(pDX, IDC_REBUILD_STREAM, m_ctrlRebuildStream);
	DDX_Control(pDX, IDC_REMOVE_DIAG, m_ctrlRemoveDiag);
	DDX_Control(pDX, IDC_REMOVE_CRC, m_ctrlRemoveCRC);
	DDX_Control(pDX, IDC_ENABLE_LOG, m_ctrlEnableLog);
	DDX_Control(pDX, IDC_BATCH, m_batch);
	DDX_Control(pDX, IDC_OUTPUT_FILENAME, m_outfile_ctrl);
	DDX_Control(pDX, IDC_FRAME, m_frame_ctrl);
	DDX_Control(pDX, IDC_LOG, m_log);
	DDX_Check(pDX, IDC_SWAP_WORD, m_swap_word);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDTSParserDlg, CDialogEx)
	//{{AFX_MSG_MAP(CDTSParserDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_ADD_DTS, OnAddDts)
	ON_BN_CLICKED(IDC_START_BATCH, OnStartBatch)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_REBUILD_STREAM, OnRebuildStream)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


// CDTSParserDlg message handlers

BOOL CDTSParserDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	::SendMessage(m_log.m_hWnd, LVM_SETEXTENDEDLISTVIEWSTYLE, LVS_EX_GRIDLINES, 1);
	long lvStyle = ::SendMessage(m_log.m_hWnd, LVM_GETEXTENDEDLISTVIEWSTYLE, 0, 0);
	lvStyle = lvStyle | LVS_EX_FULLROWSELECT;
	::SendMessage(m_log.m_hWnd, LVM_SETEXTENDEDLISTVIEWSTYLE, 0, lvStyle);

	::SendMessage(m_batch.m_hWnd, LVM_SETEXTENDEDLISTVIEWSTYLE, LVS_EX_GRIDLINES, 1);
	lvStyle = ::SendMessage(m_batch.m_hWnd, LVM_GETEXTENDEDLISTVIEWSTYLE, 0, 0);
	lvStyle = lvStyle | LVS_EX_FULLROWSELECT;
	::SendMessage(m_batch.m_hWnd, LVM_SETEXTENDEDLISTVIEWSTYLE, 0, lvStyle);

	m_log.InsertColumn(0, "Offset", LVCFMT_CENTER, 50);
	m_log.InsertColumn(1, "Frame", LVCFMT_CENTER, 50);
	m_log.InsertColumn(2, "Bitrate", LVCFMT_CENTER, 50);
	m_log.InsertColumn(3, "Format", LVCFMT_CENTER, 50);
	m_log.InsertColumn(4, "Channels", LVCFMT_CENTER, 50);
	m_log.InsertColumn(5, "FrameSize", LVCFMT_CENTER, 50);
	m_log.InsertColumn(6, "DiagNorm", LVCFMT_CENTER, 50);
	m_log.InsertColumn(7, "CRC", LVCFMT_CENTER, 50);

	m_batch.InsertColumn(0, "Filename", LVCFMT_CENTER, 50);
	m_batch.InsertColumn(1, "State", LVCFMT_CENTER, 50);
	m_batch.InsertColumn(2, "Size", LVCFMT_CENTER, 50);
	m_batch.InsertColumn(3, "Detected Format", LVCFMT_CENTER, 50);
	m_batch.InsertColumn(4, "Channels", LVCFMT_CENTER, 50);
	m_batch.InsertColumn(5, "Channel arrangement", LVCFMT_CENTER, 50);

	ResizeGrid(m_log);
	ResizeGrid(m_batch);
	CheckSettings();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CDTSParserDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CDTSParserDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CDTSParserDlg::OnAddDts()
{
	CStringList filelist;
	if (FileOpen("DTS Padded audio Files (*.dts)\0*.dts\0DTS WAV audio Files (*.dtswav, *.wav)\0*.dtswav;*.wav\0DTS Compact audio Files (*.cpt)\0*.cpt\0", filelist))
	{
		for (POSITION pos = filelist.GetHeadPosition(); pos != NULL; )
		{
			CString filename = filelist.GetNext(pos);
			int p = m_batch.GetItemCount();
			m_batch.InsertItem(p, filename);
			m_batch.SetItemText(p, 1, "queued");
			m_batch.SetItemText(p, 2, GetFileSize(filename) + " bytes");
		}
		ResizeGrid(m_batch);
	}
}

bool CDTSParserDlg::FileOpen(char* filter, CStringList& filelist)
{
	OPENFILENAME info = { 0 };
	info.lStructSize = sizeof(OPENFILENAME);
	info.hwndOwner = AfxGetApp()->m_pMainWnd->m_hWnd;
	info.hInstance = AfxGetApp()->m_hInstance;
	info.lpstrFile = new char[1024];
	info.nMaxFile = 1024;
	info.lpstrFilter = filter;
	info.nFilterIndex = 1;
	info.Flags = OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_ALLOWMULTISELECT | OFN_EXPLORER;
	info.lpstrFile[0] = 0;

	char dir[1024];
	_getcwd(dir, 1024);

	char* ret = NULL;

	if (GetOpenFileName(&info))
	{
		if (info.nFileExtension == 0) // selection multiple
		{
			char* file = info.lpstrFile + info.nFileOffset;
			CString path = info.lpstrFile;
			for (;;)
			{
				CString filename = path + "\\" + file;
				filelist.AddHead(filename);
				file += strlen(file) + 1;
				if ((*file) == 0)
					break;
			}
		}
		else // un seul fichier selectionné
		{
			filelist.AddHead(info.lpstrFile);
		}
		delete[] info.lpstrFile;
		return true;

	}
	delete[] info.lpstrFile;

	_chdir(dir);
	return false;
}

void CDTSParserDlg::ParseDTS(int batch_entry)
{
	UpdateData(TRUE);
	CWaitCursor wait;
	m_ctrlProgress.SetRange(0, 100);
	m_ctrlProgress.SetPos(0);
	m_terminate = false;
	m_log.DeleteAllItems();
	int last_rate = 0;
	int last_amode = 0;
	int last_sfreq = 0;
	int last_framesize = 0;
	int frame = 0;
	int frameStartOffset = 0;
	int last_diag_norm = 0;
	int last_crc = 0;
	bool first_event = true;
	try
	{
		while (!m_terminate)
		{
			double progress = (m_bitstr.m_bytecounter * 100.0) / m_size;
			m_ctrlProgress.SetPos((int)progress);
			PumpMessages();

			//
			// Read the next DTS frame, it must start with 7FFE8001
			//
			frameStartOffset = m_bitstr.BytePosition();
			int sync = m_bitstr.ReadBits(32);
			if (sync != 0x7FFE8001)
			{
				break;
			}
			//
			// Read the frame header
			//
			int ftype = m_bitstr.ReadBit();
			int short_ = m_bitstr.ReadBits(5);
			int cpf = m_bitstr.ReadBit();
			int nblks = m_bitstr.ReadBits(7);
			int fsize = m_bitstr.ReadBits(14);
			int amode = m_bitstr.ReadBits(6);
			int sfreq = m_bitstr.ReadBits(4);
			int rate = m_bitstr.ReadBits(5);
			int mix = m_bitstr.ReadBit();
			int dynf = m_bitstr.ReadBit();
			int timef = m_bitstr.ReadBit();
			int auxf = m_bitstr.ReadBit();
			int hdcd = m_bitstr.ReadBit();
			int ext_audio_id = m_bitstr.ReadBits(3);
			int ext_audio = m_bitstr.ReadBit();
			int aspf = m_bitstr.ReadBit();
			int lff = m_bitstr.ReadBits(2);
			int hflag = m_bitstr.ReadBit();
			int hcrc = 0;
			if (cpf == 1)
				hcrc = m_bitstr.ReadBits(16);
			int filts = m_bitstr.ReadBit();
			int vernum = m_bitstr.ReadBits(4);
			int chist = m_bitstr.ReadBits(2);
			int pcmr = m_bitstr.ReadBits(3);
			int sumf = m_bitstr.ReadBit();
			int suls = m_bitstr.ReadBit();
			int dial = m_bitstr.ReadBits(4);

			if (m_ctrlRemoveDiag.GetCheck() == 1)
			{
				dial = 0;
				vernum = 7;
			}
			if (m_ctrlRemoveCRC.GetCheck() == 1)
			{
				cpf = 0;
			}
			int header_size = m_bitstr.BytePosition() - frameStartOffset;
			//
			// now we read the content of the frame
			//
			char* contentBuffer = NULL;
			int contentLength = 0;
			bool partial = false;
			try
			{
				if (rate != 0x1D)
				{
					contentLength = (fsize + 1) - header_size;
				}
				else
				{
					contentLength = (fsize)-header_size;
				}
				contentBuffer = new char[contentLength+1024]; // 1024 is in case we found padding
				//
				// This should read all the content given the header
				//
				int b = 0;
				while (b < contentLength)
				{
					int v = m_bitstr.ReadBits(8);
					contentBuffer[b++] = v;
				}
				//
				// Nevertheless some stream use padding, so we search the next frame
				// and update the real contentLength
				//
				while (!m_bitstr.TestNextBits("0111 1111 1111 1110 1000 0000 0000 0001"))
				{
					int v = m_bitstr.ReadBits(8);
					contentBuffer[b++] = v;
				}
				contentLength = b;
			}
			catch (EOFBSException)
			{
				partial = true;
			}
			int framesize = header_size + contentLength;

			//
			// Write frame if requested
			//
			if (!partial && (m_ctrlRebuildStream.GetCheck() == 1))
			{
				m_bitwrt.WriteBits(sync, 32);
				m_bitwrt.WriteBit(ftype);
				m_bitwrt.WriteBits(short_, 5);
				m_bitwrt.WriteBit(cpf);
				m_bitwrt.WriteBits(nblks, 7);
				m_bitwrt.WriteBits(fsize, 14);
				m_bitwrt.WriteBits(amode, 6);
				m_bitwrt.WriteBits(sfreq, 4);
				m_bitwrt.WriteBits(rate, 5);
				m_bitwrt.WriteBit(mix);
				m_bitwrt.WriteBit(dynf);
				m_bitwrt.WriteBit(timef);
				m_bitwrt.WriteBit(auxf);
				m_bitwrt.WriteBit(hdcd);
				m_bitwrt.WriteBits(ext_audio_id, 3);
				m_bitwrt.WriteBit(ext_audio);
				m_bitwrt.WriteBit(aspf);
				m_bitwrt.WriteBits(lff, 2);
				m_bitwrt.WriteBit(hflag);
				if (cpf == 1)
					m_bitwrt.WriteBits(hcrc, 16);
				m_bitwrt.WriteBit(filts);
				m_bitwrt.WriteBits(vernum, 4);
				m_bitwrt.WriteBits(chist, 2);
				m_bitwrt.WriteBits(pcmr, 3);
				m_bitwrt.WriteBit(sumf);
				m_bitwrt.WriteBit(suls);
				m_bitwrt.WriteBits(dial, 4);
				for (int b = 0; b < contentLength; b++)
				{
					m_bitwrt.WriteBits(contentBuffer[b], 8);
				}
			}
			//
			// Update the GUI if the frame format have changed
			//
			if (sfreq != last_sfreq ||
				rate != last_rate ||
				amode != last_amode ||
				last_framesize != framesize ||
				last_diag_norm != dial ||
				last_crc != hcrc)
			{
				last_crc = hcrc;
				last_amode = amode;
				last_sfreq = sfreq;
				last_rate = rate;
				last_framesize = framesize;
				last_diag_norm = dial;

				CString srate = GetRate(rate);
				CString channels = GetAMode(amode);
				int nbchannels = GetAModeNbChannels(amode);
				CString format = GetFormat(ext_audio, ext_audio_id, pcmr, sfreq, nbchannels);

				CString sdial = GetDial(vernum, dial);
				CString ch;
				ch.Format("%d", nbchannels);
				if (lff == 1 || lff == 2)
				{
					channels += "+LFE";
					ch += ".1";
				}
				else
				{
					ch += ".0";
				}
				if (first_event)
				{
					m_batch.SetItemText(batch_entry, 3, format);
					m_batch.SetItemText(batch_entry, 4, ch);
					m_batch.SetItemText(batch_entry, 5, channels);
					ResizeGrid(m_batch);
				}
				NewEvent(frameStartOffset, frame, srate, channels, framesize, sdial, hcrc, format);
				first_event = false;
			}

			char buf[100];
			sprintf_s(buf, 100, "%d", frame);
			m_frame_ctrl.SetWindowText(buf);

			frame++;

			if (contentBuffer != NULL)
			{
				delete[] contentBuffer;
				contentBuffer = NULL;
			}
		}
	}
	catch (EOFBSException)
	{
	}
}

void CDTSParserDlg::PumpMessages()
{
	MSG msg;
	if (::PeekMessage(&msg, NULL, NULL, NULL, PM_NOREMOVE))
	{
		AfxGetThread()->PumpMessage();
	}
}

void CDTSParserDlg::ResizeGrid(CListCtrl& grid)
{
	for (int c = 0;; c++)
	{
		LVCOLUMN column;
		column.mask = LVCF_TEXT;
		column.pszText = new char[1024];
		column.cchTextMax = 1024;

		if (!grid.GetColumn(c, &column))
		{
			delete[] column.pszText;
			break;
		}

		strcat_s(column.pszText, 1024, "          ");
		int maxWidth = grid.GetStringWidth(column.pszText);
		delete[] column.pszText;

		for (int i = 0; i < grid.GetItemCount(); i++)
		{
			LVITEM item;
			item.mask = LVIF_TEXT;
			item.iItem = i;
			item.iSubItem = c;
			item.pszText = new char[1024];
			item.cchTextMax = 1024;
			grid.GetItem(&item);
			strcat_s(item.pszText, 1024, "          ");
			int Width = grid.GetStringWidth(item.pszText);
			if (Width > maxWidth)
				maxWidth = Width;
			delete[] item.pszText;
		}

		grid.SetColumnWidth(c, maxWidth);
	}
}

void CDTSParserDlg::NewEvent(int offset, int frame, CString rate, CString channels, int Framesize, CString dial, int cpf, CString format)
{
	char buffer[1024];
	int p = m_log.GetItemCount();
	m_log.InsertItem(p, "");

	sprintf_s(buffer, 1024, "%8.8X", offset);
	m_log.SetItemText(p, 0, buffer);

	sprintf_s(buffer, 1024, "%8.8d", frame);
	m_log.SetItemText(p, 1, buffer);

	m_log.SetItemText(p, 2, rate);
	m_log.SetItemText(p, 3, format);
	m_log.SetItemText(p, 4, channels);

	sprintf_s(buffer, 1024, "%d bytes", Framesize);
	m_log.SetItemText(p, 5, buffer);

	m_log.SetItemText(p, 6, dial);

	if (cpf == 1)
		m_log.SetItemText(p, 7, "YES");
	else
		m_log.SetItemText(p, 7, "NO");

	ResizeGrid(m_log);
}


void CDTSParserDlg::OnStartBatch()
{
	m_terminate = false;
	for (int i = 0; i < m_batch.GetItemCount() && !m_terminate; i++)
	{
		m_filename = m_batch.GetItemText(i, 0);
		m_outfilename = m_filename + ".fixed.dts";
		m_outfile_ctrl.SetWindowText(m_outfilename);
		m_size = atoi(GetFileSize(m_filename));
		m_batch.SetItemText(i, 1, "parsing...");
		if (m_ctrlRebuildStream.GetCheck() == 1)
		{
			if (m_bitstr.Open(m_filename) &&
				m_bitwrt.Open(m_outfilename))
			{
				ParseDTS(i);
				m_bitwrt.Close();
				m_bitstr.Close();
			}
		}
		else
		{
			if (m_bitstr.Open(m_filename))
			{
				ParseDTS(i);
				m_bitstr.Close();
			}
		}
		m_batch.SetItemText(i, 1, "done");
	}
	if (!m_terminate)
	{
		MessageBox("Done", "DTS Parser", MB_OK);
	}
}

void CDTSParserDlg::CheckSettings()
{
	BOOL enable = TRUE;
	if (m_ctrlRebuildStream.GetCheck() != 1)
	{
		enable = FALSE;
	}
	m_ctrlRemoveCRC.EnableWindow(enable);
	m_ctrlRemoveDiag.EnableWindow(enable);
}

void CDTSParserDlg::OnRebuildStream()
{
	CheckSettings();
}
void CDTSParserDlg::OnOK()
{
	m_terminate = true;

	CDialog::OnOK();
}

CString CDTSParserDlg::GetFileSize(CString filename)
{
	CFile* f = new CFile(filename, CFile::modeRead);

	CString size;
	size.Format("%ld", f->GetLength());
	delete f;
	return size;
}