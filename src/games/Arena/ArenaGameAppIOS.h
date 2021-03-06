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

#ifndef ArenaGameAppIOS_h__
#define ArenaGameAppIOS_h__

#include "ArenaGameApp.h"

#ifdef __OBJC__

#import <UIKit/UIKit.h>
#import <QuartzCore/QuartzCore.h>

// Defaulting to 2 means that we run at 30 frames per second. For 60 frames, use a value of 1.
// 30 FPS is usually sufficient and results in lower power consumption.
#define DISPLAYLINK_FRAME_INTERVAL 2

@interface AppDelegate : NSObject <UIApplicationDelegate>
{
    CADisplayLink *mArDisplayLink;
    NSDate* mArDate;
    NSTimeInterval mLastTime;
}

- (void)open;
- (void)render:(id)sender;

@property (nonatomic) NSTimeInterval mLastTime;

@end


@implementation AppDelegate

@dynamic mLastTime;

- (NSTimeInterval)mLastTime
{
    return mLastTime;
}

- (void)setLastFrameTime:(NSTimeInterval)frameInterval
{
    // Frame interval defines how many display frames must pass between each time the
    // display link fires. The display link will only fire 30 times a second when the
    // frame internal is two on a display that refreshes 60 times a second. The default
    // frame interval setting of one will fire 60 times a second when the display refreshes
    // at 60 times a second. A frame interval setting of less than one results in undefined
    // behavior.
    if (frameInterval >= 1)
    {
        mLastTime = frameInterval;
    }
}

- (void)open
{
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
    
    // init the engine
    Demi::ArGameApp::Get()->OpenImpl();
    
    [pool release];
}

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
    mLastTime = DISPLAYLINK_FRAME_INTERVAL;
    mArDisplayLink = nil;
    
    [self open];
    
    return YES;
}

- (void)applicationWillTerminate:(UIApplication *)application
{
    Demi::ArGameApp::Get()->CloseEngine();
}

- (void)applicationDidBecomeActive:(UIApplication *)application
{
    mArDate = [[NSDate alloc] init];
    mLastTime = DISPLAYLINK_FRAME_INTERVAL; // Reset the timer
    
    mArDisplayLink = [CADisplayLink displayLinkWithTarget:self selector:@selector(render:)];
    [mArDisplayLink setFrameInterval:mLastTime];
    [mArDisplayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
}

- (void)applicationWillResignActive:(UIApplication *)application
{
    [mArDate release];
    mArDate = nil;
    
    [mArDisplayLink invalidate];
    mArDisplayLink = nil;
}

- (void)render:(id)sender
{
    // NSTimeInterval is a simple typedef for double
    NSTimeInterval currentFrameTime = -[mArDate timeIntervalSinceNow];
    //NSTimeInterval differenceInSeconds = currentFrameTime - mLastFrameTime;
    mLastTime = currentFrameTime;
    
    dispatch_async(dispatch_get_main_queue(), ^(void)
    {
        if (Demi::ArGameApp::Get() && Demi::ArGameApp::Get()->IsOpen())
        {
            Demi::ArGameApp::Get()->Update();
        }
    });
}

@end

#else
#   error "Please set the compiler as objective c++ mode!"
#endif

#endif // ApplicationIOS_h__