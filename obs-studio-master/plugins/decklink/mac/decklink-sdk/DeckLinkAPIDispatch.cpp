/* -LICENSE-START-
** Copyright (c) 2009 Blackmagic Design
**
** Permission is hereby granted, free of charge, to any person or organization
** obtaining a copy of the software and accompanying documentation covered by
** this license (the "Software") to use, reproduce, display, distribute,
** execute, and transmit the Software, and to prepare derivative works of the
** Software, and to permit third-parties to whom the Software is furnished to
** do so, all subject to the following:
**
** The copyright notices in the Software and this entire statement, including
** the above license grant, this restriction and the following disclaimer,
** must be included in all copies of the Software, in whole or in part, and
** all derivative works of the Software, unless such copies or derivative
** works are solely in the form of machine-executable object code generated by
** a source language processor.
**
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
** IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
** FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
** SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
** FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
** ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
** DEALINGS IN THE SOFTWARE.
** -LICENSE-END-
*/
/* DeckLinkAPIDispatch.cpp */

#include "DeckLinkAPI.h"
#include <pthread.h>

#if BLACKMAGIC_DECKLINK_API_MAGIC != 1
	#error The DeckLink API version of DeckLinkAPIDispatch.cpp is not the same version as DeckLinkAPI.h
#endif

#define kDeckLinkAPI_BundlePath "/Library/Frameworks/DeckLinkAPI.framework"


typedef IDeckLinkIterator* (*CreateIteratorFunc)(void);
typedef IDeckLinkAPIInformation* (*CreateAPIInformationFunc)(void);
typedef IDeckLinkGLScreenPreviewHelper* (*CreateOpenGLScreenPreviewHelperFunc)(void);
typedef IDeckLinkCocoaScreenPreviewCallback* (*CreateCocoaScreenPreviewFunc)(void*);
typedef IDeckLinkVideoConversion* (*CreateVideoConversionInstanceFunc)(void);
typedef IDeckLinkDiscovery* (*CreateDeckLinkDiscoveryInstanceFunc)(void);

static pthread_once_t						gDeckLinkOnceControl		= PTHREAD_ONCE_INIT;
static CFBundleRef							gDeckLinkAPIBundleRef		= NULL;
static CreateIteratorFunc					gCreateIteratorFunc			= NULL;
static CreateAPIInformationFunc				gCreateAPIInformationFunc	= NULL;
static CreateOpenGLScreenPreviewHelperFunc	gCreateOpenGLPreviewFunc	= NULL;
static CreateCocoaScreenPreviewFunc			gCreateCocoaPreviewFunc		= NULL;
static CreateVideoConversionInstanceFunc	gCreateVideoConversionFunc	= NULL;
static CreateDeckLinkDiscoveryInstanceFunc  gCreateDeckLinkDiscoveryFunc= NULL;


void	InitDeckLinkAPI (void)
{
	CFURLRef		bundleURL;

	bundleURL = CFURLCreateWithFileSystemPath(kCFAllocatorDefault, CFSTR(kDeckLinkAPI_BundlePath), kCFURLPOSIXPathStyle, true);
	if (bundleURL != NULL)
	{
		gDeckLinkAPIBundleRef = CFBundleCreate(kCFAllocatorDefault, bundleURL);
		if (gDeckLinkAPIBundleRef != NULL)
		{
			gCreateIteratorFunc = (CreateIteratorFunc)CFBundleGetFunctionPointerForName(gDeckLinkAPIBundleRef, CFSTR("CreateDeckLinkIteratorInstance_0002"));
			gCreateAPIInformationFunc = (CreateAPIInformationFunc)CFBundleGetFunctionPointerForName(gDeckLinkAPIBundleRef, CFSTR("CreateDeckLinkAPIInformationInstance_0001"));
			gCreateOpenGLPreviewFunc = (CreateOpenGLScreenPreviewHelperFunc)CFBundleGetFunctionPointerForName(gDeckLinkAPIBundleRef, CFSTR("CreateOpenGLScreenPreviewHelper_0001"));
			gCreateCocoaPreviewFunc = (CreateCocoaScreenPreviewFunc)CFBundleGetFunctionPointerForName(gDeckLinkAPIBundleRef, CFSTR("CreateCocoaScreenPreview_0001"));
			gCreateVideoConversionFunc = (CreateVideoConversionInstanceFunc)CFBundleGetFunctionPointerForName(gDeckLinkAPIBundleRef, CFSTR("CreateVideoConversionInstance_0001"));
            gCreateDeckLinkDiscoveryFunc = (CreateDeckLinkDiscoveryInstanceFunc)CFBundleGetFunctionPointerForName(gDeckLinkAPIBundleRef, CFSTR("CreateDeckLinkDiscoveryInstance_0001"));
		}
		CFRelease(bundleURL);
	}
}

bool		IsDeckLinkAPIPresent (void)
{
	// If the DeckLink API bundle was successfully loaded, return this knowledge to the caller
	if (gDeckLinkAPIBundleRef != NULL)
		return true;

	return false;
}

IDeckLinkIterator*		CreateDeckLinkIteratorInstance (void)
{
	pthread_once(&gDeckLinkOnceControl, InitDeckLinkAPI);

	if (gCreateIteratorFunc == NULL)
		return NULL;

	return gCreateIteratorFunc();
}

IDeckLinkAPIInformation*	CreateDeckLinkAPIInformationInstance (void)
{
	pthread_once(&gDeckLinkOnceControl, InitDeckLinkAPI);

	if (gCreateAPIInformationFunc == NULL)
		return NULL;

	return gCreateAPIInformationFunc();
}

IDeckLinkGLScreenPreviewHelper*		CreateOpenGLScreenPreviewHelper (void)
{
	pthread_once(&gDeckLinkOnceControl, InitDeckLinkAPI);

	if (gCreateOpenGLPreviewFunc == NULL)
		return NULL;

	return gCreateOpenGLPreviewFunc();
}

IDeckLinkCocoaScreenPreviewCallback*	CreateCocoaScreenPreview (void* parentView)
{
	pthread_once(&gDeckLinkOnceControl, InitDeckLinkAPI);

	if (gCreateCocoaPreviewFunc == NULL)
		return NULL;

	return gCreateCocoaPreviewFunc(parentView);
}

IDeckLinkVideoConversion* CreateVideoConversionInstance (void)
{
	pthread_once(&gDeckLinkOnceControl, InitDeckLinkAPI);

	if (gCreateVideoConversionFunc == NULL)
		return NULL;

	return gCreateVideoConversionFunc();
}

IDeckLinkDiscovery* CreateDeckLinkDiscoveryInstance (void)
{
	pthread_once(&gDeckLinkOnceControl, InitDeckLinkAPI);

	if (gCreateDeckLinkDiscoveryFunc == NULL)
		return NULL;

	return gCreateDeckLinkDiscoveryFunc();
}


#define kBMDStreamingAPI_BundlePath "/Library/Application Support/Blackmagic Design/Streaming/BMDStreamingAPI.bundle"

typedef IBMDStreamingDiscovery* (*CreateDiscoveryFunc)(void);
typedef IBMDStreamingH264NALParser* (*CreateNALParserFunc)(void);

static pthread_once_t      gBMDStreamingOnceControl  = PTHREAD_ONCE_INIT;
static CFBundleRef         gBMDStreamingAPIBundleRef = NULL;
static CreateDiscoveryFunc gCreateDiscoveryFunc      = NULL;
static CreateNALParserFunc gCreateNALParserFunc      = NULL;

void InitBMDStreamingAPI(void)
{
	CFURLRef bundleURL;

	bundleURL = CFURLCreateWithFileSystemPath(kCFAllocatorDefault, CFSTR(kBMDStreamingAPI_BundlePath), kCFURLPOSIXPathStyle, true);
	if (bundleURL != NULL)
	{
		gBMDStreamingAPIBundleRef = CFBundleCreate(kCFAllocatorDefault, bundleURL);
		if (gBMDStreamingAPIBundleRef != NULL)
		{
			gCreateDiscoveryFunc = (CreateDiscoveryFunc)CFBundleGetFunctionPointerForName(gBMDStreamingAPIBundleRef, CFSTR("CreateBMDStreamingDiscoveryInstance_0001"));
			gCreateNALParserFunc = (CreateNALParserFunc)CFBundleGetFunctionPointerForName(gBMDStreamingAPIBundleRef, CFSTR("CreateBMDStreamingH264NALParser_0001"));
		}

		CFRelease(bundleURL);
	}
}

IBMDStreamingDiscovery* CreateBMDStreamingDiscoveryInstance()
{
	pthread_once(&gBMDStreamingOnceControl, InitBMDStreamingAPI);

	if (gCreateDiscoveryFunc == NULL)
		return NULL;

	return gCreateDiscoveryFunc();
}

IBMDStreamingH264NALParser* CreateBMDStreamingH264NALParser()
{
	pthread_once(&gBMDStreamingOnceControl, InitBMDStreamingAPI);

	if (gCreateNALParserFunc == NULL)
		return NULL;

	return gCreateNALParserFunc();
}
