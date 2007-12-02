#ifndef _PLUGIN_H_
#define _PLUGIN_H_

#include "winamp/gen.h"
#include "winamp/wa_ipc.h"
#include "winamp/api/service/api_service.h"
#include "winamp/api/service/waservicefactory.h"
#include "winamp/tagz/api_tagz.h"

/*
class TagProvider : public ifc_tagprovider
{
public:
	wchar_t *GetTag(const wchar_t *name, ifc_tagparams *parameters); //return 0 if not found
	void FreeTag(wchar_t *tag);
protected:
	RECVS_DISPATCH; // macro for declaring the dispatch table
};

// ifc_tagparams is basically a context object.  we don't use it here, for clarity
wchar_t *TagProvider::GetTag(const wchar_t *name, ifc_tagparams *parameters)
{
	// just going to make this easy and return a string that's the same as the tag name
	return wcsdup(name);
}

void TagProvider::FreeTag(wchar_t *tag)
{
	free(tag);
}

// define the dispatch table
#define CBCLASS TagProvider
	START_DISPATCH;
	CB(IFC_TAGPROVIDER_GET_TAG, GetTag);
	VCB(IFC_TAGPROVIDER_FREE_TAG, FreeTag);
	END_DISPATCH;
#undef CBCLASS

int Format(HWND hWnd, LPTSTR lpszMessage)
{
	//==============================================================
	//=                                                            =
	//==============================================================

	// First, we need to get the Service Manager.  This is where all shared services are stored
	// This of it as the Winamp/Wasabi equivalent to CoCreateInstance
	api_service *serviceManager = 0; // in a normal plugin, you'd probably make this a global.  putting it here for clarity
	serviceManager = (api_service *)SendMessage(hWnd, WM_WA_IPC, 0, IPC_GET_API_SERVICE);
	if (!serviceManager || ((intptr_t)serviceManager) == 1)
		return 1; // unable to get service manager, must be an old version of Winamp (requires 5.12+)

	//==============================================================
	//=                                                            =
	//==============================================================

	// now we have to use the service manager to create a Tagz object
	api_tagz *tagz = 0;
	waServiceFactory *serviceFactory = serviceManager->service_getServiceByGuid(tagzGUID); // this GUID is defined in api_tagz.h
	if (serviceFactory)
		tagz = (api_tagz *)serviceFactory->getInterface();
	if (!tagz)
		return 1;

	//==============================================================
	//=                                                            =
	//==============================================================

	// now we use the tagz object
	TagProvider tagProvider; // our own simple tag provider object, see TagProvider.cpp/.h
	tagz->format(L"%artist% - $caps(%album%)", lpszMessage, 1024, &tagProvider, 0);

	//==============================================================
	//=                                                            =
	//==============================================================

	// now we need to release the tagz object
	// we're going reuse our service factory from above, but it's acceptable to retrieve it from the service manager again
	serviceFactory->releaseInterface(tagz);
}
//*/
#endif //_PLUGIN_H_