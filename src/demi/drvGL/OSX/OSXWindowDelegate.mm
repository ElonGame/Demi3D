/**********************************************************************
  This source file is a part of Demi3D
   __  ___  __  __  __
  |  \|_ |\/||   _)|  \ 
  |__/|__|  ||  __)|__/ 

  Copyright (c) 2013-2014 Demi team
  https://github.com/wangyanxing/Demi3D

  Released under the MIT License
  https://github.com/wangyanxing/Demi3D/blob/master/License.txt
***********************************************************************/

#include "OSXWindowDelegate.h"
#include "OSXWindow.h"
#include "Command.h"

using namespace Demi;
@implementation OSXWindowDelegate


-(id)initWithNSWindow:(NSWindow*)nswin demiWindow:(DiWindow*)demiwin
{
    if ((self = [super init]))
    {
		window = nswin;
		demiWindow = demiwin;
		
        // Register ourselves for several window event notifications
		// Note that
        [[NSNotificationCenter defaultCenter] addObserver:self
                                                 selector:@selector(windowDidResize:)
                                                     name:@"NSWindowDidResizeNotification"
												   object:window];
        
        [[NSNotificationCenter defaultCenter] addObserver:self
                                                 selector:@selector(windowWillMove:)
                                                     name:@"NSWindowWillMoveNotification"
												   object:window];
        
        [[NSNotificationCenter defaultCenter] addObserver:self
                                                 selector:@selector(windowWillClose:)
                                                     name:@"NSWindowWillCloseNotification"
												   object:window];
        
        [[NSNotificationCenter defaultCenter] addObserver:self
                                                 selector:@selector(windowDidBecomeKey:)
                                                     name:@"NSWindowDidBecomeKeyNotification"
												   object:window];
        
        [[NSNotificationCenter defaultCenter] addObserver:self
                                                 selector:@selector(windowDidResignKey:)
                                                     name:@"NSWindowDidResignKeyNotification"
												   object:window];
        
        [[NSNotificationCenter defaultCenter] addObserver:self
                                                 selector:@selector(windowDidMiniaturize:)
                                                     name:@"NSWindowDidMiniaturizeNotification"
												   object:window];
        
        [[NSNotificationCenter defaultCenter] addObserver:self
                                                 selector:@selector(windowDidDeminiaturize:)
                                                     name:@"NSWindowDidDeminiaturizeNotification"
												   object:window];
    }
    return self;
}

- (void)dealloc {
    // Stop observing notifications
    [[NSNotificationCenter defaultCenter] removeObserver:self];
    
    [super dealloc];
}

- (void)windowDidResize:(NSNotification *)notification
{
	DiOSXWindow* curWindow = static_cast<DiOSXWindow*>(demiWindow);
	curWindow->OnMoveOrResize();
}

- (void)windowWillMove:(NSNotification *)notification
{
    DiOSXWindow* curWindow = static_cast<DiOSXWindow*>(demiWindow);
    curWindow->OnMoveOrResize();
}

- (void)windowWillClose:(NSNotification *)notification
{
    if(DiBase::CommandMgr)
        DiBase::CommandMgr->ExecuteCommand("quit");
}

- (void)windowDidBecomeKey:(NSNotification *)notification
{
    DiOSXWindow* curWindow = static_cast<DiOSXWindow*>(demiWindow);
    curWindow->SetActive( true );
}

- (void)windowDidResignKey:(NSNotification *)notification
{
    DiOSXWindow* curWindow = static_cast<DiOSXWindow*>(demiWindow);
    curWindow->SetActive( false );
}

- (void)windowDidMiniaturize:(NSNotification *)notification
{
    DiOSXWindow* curWindow = static_cast<DiOSXWindow*>(demiWindow);
    curWindow->SetActive( false );
}

- (void)windowDidDeminiaturize:(NSNotification *)notification
{
    DiOSXWindow* curWindow = static_cast<DiOSXWindow*>(demiWindow);
    curWindow->SetActive( true );
}
@end
