#include "FbBookTree.h"
#include "FbBookData.h"
#include "FbBookEvent.h"
#include "FbConst.h"

//-----------------------------------------------------------------------------
//  FbAuthParentData
//-----------------------------------------------------------------------------

static int ComareBooks(FbModelData ** x, FbModelData ** y)
{
	{
		FbSeqnParentData * xx = wxDynamicCast(*x, FbSeqnParentData);
		FbSeqnParentData * yy = wxDynamicCast(*y, FbSeqnParentData);
		if (xx && yy) return xx->Compare(*yy);
	}
	{
		FbAuthParentData * xx = wxDynamicCast(*x, FbAuthParentData);
		FbAuthParentData * yy = wxDynamicCast(*y, FbAuthParentData);
		if (xx && yy) return xx->Compare(*yy);
	}

	return 0;
}

IMPLEMENT_CLASS(FbAuthParentData, FbParentData)

void FbAuthParentData::SortItems()
{
	m_items.Sort(ComareBooks);
}

int FbAuthParentData::Compare(const FbAuthParentData &data)
{
	return GetTitle().CmpNoCase(data.GetTitle());
}

//-----------------------------------------------------------------------------
//  FbSeqnParentData
//-----------------------------------------------------------------------------

IMPLEMENT_CLASS(FbSeqnParentData, FbParentData)

int FbSeqnParentData::Compare(const FbSeqnParentData &data)
{
	if (GetCode() == 0) return +1;
	if (data.GetCode() == 0) return -1;
	return GetTitle().CmpNoCase(data.GetTitle());
}

//-----------------------------------------------------------------------------
//  FbBookChildData
//-----------------------------------------------------------------------------

IMPLEMENT_CLASS(FbBookChildData, FbChildData)

wxString FbBookChildData::GetValue(FbModel & model, size_t col) const
{
	if (col == BF_NUMB)
		return m_numb ? wxString::Format(wxT("%d"), m_numb) : wxString();
	else
		return FbCollection::GetBook(m_code, col);
}

//-----------------------------------------------------------------------------
//  FbBookTreeModel
//-----------------------------------------------------------------------------

IMPLEMENT_CLASS(FbBookTreeModel, FbTreeModel)

size_t FbBookTreeModel::GetSelected(wxArrayInt &items)
{
	items.Empty();
	FbModelItem root = GetRoot(); 
	if (root) GetSelected(root, items);
	return items.Count();
}

void FbBookTreeModel::GetSelected(FbModelItem &parent, wxArrayInt &items)
{
	if (parent.GetState() == 1) {
		FbBookChildData * book = wxDynamicCast(&parent, FbBookChildData);
		if (book) items.Add(book->GetCode());
	}

	size_t count = parent.Count();
	for (size_t i = 0; i < count; i++) {
		GetSelected(parent.Items(i), items);
	}
}
