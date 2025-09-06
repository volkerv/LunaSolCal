/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   + HauteCapture - (c) 2005 by Volker Voecking Software Engineering
   +
   + $Source: C:\\CVS_Repository/CybonTailTNG/shared/DbgLogStream.h,v $
   + $Author: volker $
   + $Date: 2005/01/15 12:50:28 $
   + $Revision: 1.5 $
   +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/**
 * $Source: C:\\CVS_Repository/CybonTailTNG/shared/DbgLogStream.h,v $
 *
 * Declaration of a stream-buffer that allows to redirect the
 * cerr stream to the windows debug output.
 *
 * This class allows to redirect the cerr-stream (or any other stream) so
 * that the output appears on the Windows debugging terminal. The output may
 * (or may not) appear additionaly in a user-supplied log-file.
 * The following example illustrates the use of this class;
 * Example:
 * 1) static CWinDebugStreamBuf myErrStreamBuf;
 *    cerr = &myErrStreamBuf;
 *       // all output sent to cerr appears on the windows debugging terminal
 * 2) static CWinDebugStreamBuf myErrStreamBuf("stderr.txt");
 *    cerr = &myErrStreamBuf;
 *       // all output sent to cerr appears on the windows debugging terminal
 *       // as well as in a file named "stderr.txt"
 * The follwing example will send the output to the Windows debugging terminal
 * in the debug version of the application and to the file "myapp.log" in the 
 * retail version of the application:
 * 3) #ifdef _DEBUG
 *    static CWinDebugStreamBuf myErrStreamBuf;
 *    cerr = &myErrStreamBuf;
 *    #else
 *    static ofstream errorStream("arcislog.txt");
 *    cerr = errorStream;
 *    #endif
 *
 * @author  Volker Voecking
 * @version $Revision: 1.5 $
 *
 */

#ifndef __WDBGSTR_H__
#define __WDBGSTR_H__

#pragma warning( push, 3 )
#include "streambuf"
#include "fstream"
/*
#include "iostream.h"
#include "fstream.h"
*/
#pragma warning( pop )

//#include "stdafx.h" 

using namespace std;

typedef int filedesc;

class CWinDebugStreamBuf : public streambuf
{ 
public:
    CWinDebugStreamBuf();
    CWinDebugStreamBuf(char * filename);
    virtual ~CWinDebugStreamBuf();

    /** attach buffer to a file */

    BOOL open (LPSTR fileName, int nMode=ios::app); //, int nProt=filebuf::sh_read | filebuf::sh_write);
    
    void close();
    int is_open() const;
    virtual int overflow( int nCh = 0);
    virtual int underflow ();
    
    virtual int sync();
    
    /** return the associated file descriptor */
//    filedesc CWinDebugStreamBuf::fd() const{ 
//        return f.fd(); }

    /** connect the stream to an existing file */
    void attach(filedesc fd);
    
private:
    // helper function
    void WriteOutput(int len);
    void DebugOutput();
    
    static TCHAR buf[256];
    TCHAR *logFile;
    ofstream f;
    static const int cNoFile;
};

#endif
