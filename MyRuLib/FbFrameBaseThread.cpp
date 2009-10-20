#include "FbFrameBaseThread.h"
#include "FbConst.h"
#include "FbBookEvent.h"

wxCriticalSection FbFrameBaseThread::sm_queue;

wxString FbFrameBaseThread::GetSQL(const wxString & condition)
{
	wxString sql;
	switch (m_mode) {
		case FB2_MODE_TREE:
			sql = wxT("\
				SELECT (CASE WHEN bookseq.id_seq IS NULL THEN 1 ELSE 0 END) AS key, \
					books.id, books.title, books.file_size, books.file_type, books.id_author, \
					ratings.rating, authors.full_name as full_name, sequences.value AS sequence, bookseq.number as number\
				FROM books \
					LEFT JOIN authors ON books.id_author = authors.id  \
					LEFT JOIN bookseq ON bookseq.id_book=books.id AND bookseq.id_author = books.id_author \
					LEFT JOIN sequences ON bookseq.id_seq=sequences.id \
					LEFT JOIN ratings ON books.md5sum=ratings.md5sum \
                WHERE (%s) \
				ORDER BY authors.search_name, key, sequences.value, bookseq.number, books.title \
			"); break;
		case FB2_MODE_LIST:
			sql = wxT("\
				SELECT \
					books.id as id, books.title as title, books.file_size as file_size, books.file_type as file_type, \
					ratings.rating, authors.full_name as full_name, 0 as number \
				FROM books \
					LEFT JOIN authors ON books.id_author = authors.id \
					LEFT JOIN ratings ON books.md5sum=ratings.md5sum \
                WHERE (%s) \
				ORDER BY books.title, books.id, authors.full_name\
			"); break;
	}

	wxString str = wxT("(%s)");
	if (m_FilterFb2) str += wxT("AND(books.file_type='fb2')");
	if (m_FilterLib) str += wxT("AND(books.id>0)");
	if (m_FilterUsr) str += wxT("AND(books.id<0)");
	sql = wxString::Format(sql, str.c_str());

	return wxString::Format(sql, condition.c_str());
}

void FbFrameBaseThread::CreateTree(wxSQLite3ResultSet &result)
{
    wxString thisAuthor = wxT("@@@");
    wxString thisSequence = wxT("@@@");
    while (result.NextRow()) {
	    wxString nextAuthor = result.GetString(wxT("full_name"));
	    wxString nextSequence = result.GetString(wxT("sequence"));

	    if (thisAuthor != nextAuthor) {
	        thisAuthor = nextAuthor;
	        thisSequence = wxT("@@@");
			wxCommandEvent event(fbEVT_BOOK_ACTION, ID_APPEND_AUTHOR);
			event.SetString(thisAuthor);
			wxPostEvent(m_frame, event);
	    }
	    if (thisSequence != nextSequence) {
	        thisSequence = nextSequence;
			wxCommandEvent event(fbEVT_BOOK_ACTION, ID_APPEND_SEQUENCE);
			event.SetString(thisSequence);
			wxPostEvent(m_frame, event);
	    }

        BookTreeItemData data(result);
		FbBookEvent event(fbEVT_BOOK_ACTION, ID_APPEND_BOOK, &data);
		wxPostEvent(m_frame, event);
    }
}

void FbFrameBaseThread::CreateList(wxSQLite3ResultSet &result)
{
    result.NextRow();
    while (!result.Eof()) {
        BookTreeItemData data(result);
        wxString full_name = result.GetString(wxT("full_name"));
        do {
            result.NextRow();
            if ( data.GetId() != result.GetInt(wxT("id")) ) break;
            full_name = full_name + wxT(", ") + result.GetString(wxT("full_name"));
        } while (!result.Eof());

		FbBookEvent event(fbEVT_BOOK_ACTION, ID_APPEND_BOOK, &data);
		event.SetString(full_name);
		wxPostEvent(m_frame, event);
    }
}

void FbFrameBaseThread::EmptyBooks()
{
	wxCommandEvent event(fbEVT_BOOK_ACTION, ID_EMPTY_BOOKS);
	wxPostEvent(m_frame, event);
}

void FbFrameBaseThread::FillBooks(wxSQLite3ResultSet &result)
{
	switch (m_mode) {
		case FB2_MODE_TREE: CreateTree(result); break;
		case FB2_MODE_LIST: CreateList(result); break;
	}
}