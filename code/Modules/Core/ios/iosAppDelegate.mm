//------------------------------------------------------------------------------
//  iosAppDelegate.mm
//------------------------------------------------------------------------------
#include "iosAppDelegate.h"
#include "iosBridge.h"
#import <GLKit/GLKit.h>

using namespace Oryol;
using namespace Oryol::_priv;

@implementation iosAppDelegate

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    // Override point for customization after application launch.
    iosBridge::ptr()->onDidFinishLaunching();
    return YES;
}

- (void)applicationWillResignActive:(UIApplication *)application {
    // Sent when the application is about to move from active to inactive state.
    // This can occur for certain types of temporary interruptions (such as an incoming
    // phone call or SMS message) or when the user quits the application and it
    // begins the transition to the background state.
    // Use this method to pause ongoing tasks, disable timers, and throttle down
    // OpenGL ES frame rates. Games should use this method to pause the game.
    iosBridge::ptr()->onWillResignActive();
}

- (void)applicationDidEnterBackground:(UIApplication *)application {
    // Use this method to release shared resources, save user data, invalidate timers,
    // and store enough application state information to restore your application
    // to its current state in case it is terminated later.
    // If your application supports background execution, this method is called
    // instead of applicationWillTerminate: when the user quits.
    iosBridge::ptr()->onDidEnterBackground();
}

- (void)applicationWillEnterForeground:(UIApplication *)application {
    // Called as part of the transition from the background to the inactive state;
    // here you can undo many of the changes made on entering the background.
    iosBridge::ptr()->onWillEnterForeGround();
}

- (void)applicationDidBecomeActive:(UIApplication *)application {
    // Restart any tasks that were paused (or not yet started) while the
    // application was inactive. If the application was previously in the background,
    // optionally refresh the user interface.
    iosBridge::ptr()->onDidBecomeActive();
}

- (void)applicationWillTerminate:(UIApplication *)application {
    // Called when the application is about to terminate. Save data if
    // appropriate. See also applicationDidEnterBackground:.
    iosBridge::ptr()->onWillTerminate();
}

- (void) drawRequested:(CADisplayLink *)sender {
    // this is our own per-frame callback from the DisplayLink
    iosBridge::ptr()->onDrawRequested();
}

- (void)glkView:(GLKView*)view drawInRect:(CGRect)rect {
    // this is called from inside the GLKView as a result
    // of drawRequested from the display link,
    // the entire per-frame logic will happen inside this method
    iosBridge::ptr()->onFrame();
}

@end
