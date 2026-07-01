/*
 * Copyright (c) 2026 Stelo Engine
 *
 * This file is part of Stelo Engine.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * You may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and limitations
 * under the License.
*/

#include "../../Window.hpp"
#include "../../App.hpp"

#import <Cocoa/Cocoa.h>
#import <Foundation/Foundation.h>
#import <Carbon/Carbon.h>
#import <ApplicationServices/ApplicationServices.h>
#include <objc/runtime.h>

static char SteloWindowKey;

@interface WindowDelegate : NSObject <NSWindowDelegate>
@end
@implementation WindowDelegate
- (void)windowWillClose:(NSNotification*)notification {
    NSWindow* win = (NSWindow*)[notification object];
    NSValue* val = (NSValue*)objc_getAssociatedObject(win, &SteloWindowKey);
    if(!val) return;
    Stelo::Widget* window = (Stelo::Widget*)[val pointerValue];
    if(!window) return;
    if (Stelo::App::HasRegisterWidget(window)) {
        window->CallEvent(Stelo::Event::Close);
        Stelo::App::UnregisterWidget(window);
        window->Destroy();
    }
}
@end

@interface SteloView : NSView
@end
@implementation SteloView
- (BOOL)acceptsFirstResponder { return YES; }
- (void)viewDidMoveToWindow {
    [self.window makeFirstResponder:self];
}
- (void)keyDown:(NSEvent *)event {}
- (void)keyUp:(NSEvent *)event {}
- (void)flagsChanged:(NSEvent *)event {}
@end

namespace Stelo {
    RenderContext Widget::GetRenderContext() {
        RenderContext context = {};
        context.pNativeDisplay = NSApp;
        context.mNativeDisplayType = NATIVE_DISPLAY_TYPE_NS_APPKIT;

        if (_handle.handleType == HandleType::NSWindow) {
            NSWindow* window = static_cast<NSWindow*>(_handle.handle);
            context.pNativeView = [window contentView];
            context.mNativeViewType = NATIVE_VIEW_TYPE_NS_VIEW;
            return context;
        }

        if (_handle.handleType == HandleType::NSView) {
            context.pNativeView = _handle.handle;
            context.mNativeViewType = NATIVE_VIEW_TYPE_NS_VIEW;
            return context;
        }

        if (_handle.handleType == HandleType::CametalLayer) {
            context.pNativeView = _handle.handle;
            context.mNativeViewType = NATIVE_VIEW_TYPE_CAMETAL_LAYER;
            return context;
        }

        return context;
    }

    Handle Window::CreateHandle() {
        if (NSApp == nil) {
            [NSApplication sharedApplication];
            [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];
            [NSApp activateIgnoringOtherApps:YES];
        }

        Handle handle;        
        if(GetParent() == nullptr) {
            NSString* title = [NSString stringWithUTF8String: _title.c_str()];
            NSWindow* win = [[NSWindow alloc] initWithContentRect:NSMakeRect(GetPoint().x, GetPoint().y, GetSize().width, GetSize().height)
                                                        styleMask:(NSWindowStyleMaskTitled |
                                                                    NSWindowStyleMaskClosable |
                                                                    NSWindowStyleMaskResizable)
                                                        backing:NSBackingStoreBuffered
                                                        defer:NO];
            SteloView* view = [[SteloView alloc] initWithFrame:NSMakeRect(0, 0, GetSize().width, GetSize().height)];
            [view setWantsLayer:YES];
            [win setContentView:view];
            [view.window makeFirstResponder:view];
            [win setTitle:title];
            [win makeKeyWindow];
            [win makeKeyAndOrderFront:nil];

            WindowDelegate* delegate = [[WindowDelegate alloc] init];
            [win setDelegate:delegate];
            static char delegateKey;
            objc_setAssociatedObject(win, &delegateKey, delegate, OBJC_ASSOCIATION_RETAIN_NONATOMIC);
            NSValue* ptrValue = [NSValue valueWithPointer:this];
            objc_setAssociatedObject(win, &SteloWindowKey, ptrValue, OBJC_ASSOCIATION_RETAIN_NONATOMIC);

            handle.handle = (void*)win;
            handle.handleType = HandleType::NSWindow;
        }
        else {
            NSView* view = [[NSView alloc] initWithFrame:NSMakeRect(GetPoint().x, GetPoint().y, GetSize().width, GetSize().height)];
            [view setWantsLayer:YES];

            id parent = (__bridge id)GetParent();
            if ([parent isKindOfClass:[NSWindow class]]) [[(NSWindow*)parent contentView] addSubview:view];
            else [(NSView*)parent addSubview:view];

            handle.handle = (void*)view;
            handle.handleType = HandleType::NSView;
        }

        return handle;
    }
}
