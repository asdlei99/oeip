#pragma once

#include <guiddef.h>
#include "MediaStruct.h"

struct IMFMediaType;
struct IMFAttributes;
LPCWSTR GetGUIDNameConstNew(const GUID& guid);
HRESULT GetGUIDNameNew(const GUID& guid, WCHAR** ppwsz);

HRESULT LogAttributeValueByIndexNew(IMFAttributes* pAttr, DWORD index);
HRESULT SpecialCaseAttributeValueNew(GUID guid, const PROPVARIANT& var, MediaType& out);
/// Class for parsing info from IMFMediaType into the local MediaType
class FormatReader
{
public:
	static bool Read(IMFMediaType* pType, MediaType& mediaType);
	~FormatReader(void);
private:
	FormatReader(void);
};


