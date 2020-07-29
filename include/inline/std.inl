/**
 * @file   std.inl
 * @author Vic P.
 * @brief  Inline for STL
 */

namespace std
{

#include "inline/spechrs.inl"

#ifdef _UNICODE
#define tcerr           wcerr
#define tcin            wcin
#define tclog           wclog
#define tcout           wcout

typedef wstring         tstring;

typedef wstringbuf      tstringbuf;
typedef wstringstream   tstringstream;
typedef wostringstream  tostringstream;
typedef wistringstream  tistringstream;

typedef wstreambuf      tstreambuf;

typedef wistream        tistream;
typedef wostream        tostream;
typedef wiostream       tiostream;

typedef wfilebuf        tfilebuf;
typedef wfstream        tfstream;
typedef wifstream       tifstream;
typedef wofstream       tofstream;

typedef wios            tios;
#else // !_UNICODE
#define tcerr           cerr
#define tcin            cin
#define tclog           clog
#define tcout           cout

typedef string          tstring;

typedef stringbuf       tstringbuf;
typedef stringstream    tstringstream;
typedef ostringstream   tostringstream;
typedef istringstream   tistringstream;

typedef streambuf       tstreambuf;

typedef istream         tistream;
typedef ostream         tostream;
typedef iostream        tiostream;

typedef filebuf         tfilebuf;
typedef fstream         tfstream;
typedef ifstream        tifstream;
typedef ofstream        tofstream;

typedef ios             tios;
#endif // _UNICODE

} // namespace std