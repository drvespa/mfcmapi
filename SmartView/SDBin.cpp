#include "stdafx.h"
#include "SDBin.h"
#include "String.h"
#include "MySecInfo.h"
#include "MAPIFunctions.h"
#include "ExtraPropTags.h"
#include "InterpretProp2.h"

SDBin::SDBin()
{
	m_lpMAPIProp = nullptr;
	m_bFB = false;
}

SDBin::~SDBin()
{
	if (m_lpMAPIProp) m_lpMAPIProp->Release();
}

void SDBin::Init(_In_opt_ LPMAPIPROP lpMAPIProp, bool bFB)
{
	if (m_lpMAPIProp) m_lpMAPIProp->Release();
	m_lpMAPIProp = lpMAPIProp;
	if (m_lpMAPIProp) m_lpMAPIProp->AddRef();
	m_bFB = bFB;
}

void SDBin::Parse()
{
}

_Check_return_ wstring SDBin::ToStringInternal()
{
	auto hRes = S_OK;
	auto lpSDToParse = m_Parser.GetCurrentAddress();
	auto ulSDToParse = m_Parser.RemainingBytes();
	m_Parser.Advance(ulSDToParse);

	auto acetype = acetypeMessage;
	switch (GetMAPIObjectType(m_lpMAPIProp))
	{
	case MAPI_STORE:
	case MAPI_ADDRBOOK:
	case MAPI_FOLDER:
	case MAPI_ABCONT:
		acetype = acetypeContainer;
		break;
	}

	if (m_bFB) acetype = acetypeFreeBusy;

	wstring szDACL;
	wstring szInfo;

	WC_H(SDToString(lpSDToParse, ulSDToParse, acetype, szDACL, szInfo));

	auto szFlags = InterpretFlags(flagSecurityVersion, SECURITY_DESCRIPTOR_VERSION(lpSDToParse));
	auto szResult = formatmessage(IDS_SECURITYDESCRIPTORHEADER);
	szResult += szInfo;
	szResult += formatmessage(IDS_SECURITYDESCRIPTORVERSION, SECURITY_DESCRIPTOR_VERSION(lpSDToParse), szFlags.c_str());
	szResult += szDACL;

	return szResult;
}