/*****************************************************************************
 * Plus42 -- an enhanced HP-42S calculator simulator
 * Copyright (C) 2004-2025  Thomas Okken
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License, version 2,
 * as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see http://www.gnu.org/licenses/.
 *****************************************************************************/

#import <Cocoa/Cocoa.h>
#import <WebKit/WebKit.h>
#import "shell_skin.h"

#define FILENAMELEN 256
#define SHELL_VERSION 5

struct state_type {
    int printerToTxtFile;
    int printerToGifFile;
    char printerTxtFileName[FILENAMELEN];
    char printerGifFileName[FILENAMELEN];
    int printerGifMaxLength;
    char mainWindowKnown, printWindowKnown, printWindowMapped;
    int mainWindowX, mainWindowY;
    int printWindowX, printWindowY, printWindowHeight;
    char skinName[FILENAMELEN];
    char coreName[FILENAMELEN];
    bool matrix_singularmatrix;
    bool matrix_outofrange;
    bool auto_repeat;
    bool localized_copy_paste;
    int mainWindowWidth, mainWindowHeight;
};

extern state_type state;
extern int skin_mode;

extern char free42dirname[FILENAMELEN];

#define PRINT_LINES 9000
#define PRINT_BYTESPERLINE 36
#define PRINT_SIZE 324000

extern unsigned char *print_bitmap;
extern int printout_top;
extern int printout_bottom;

void calc_mousedown(int x, int y);
void calc_mouseup();
void calc_keydown(NSString *characters, NSUInteger flags, unsigned short keycode);
void calc_keyup(NSString *characters, NSUInteger flags, unsigned short keycode);
void calc_keymodifierschanged(NSUInteger flags);

void get_keymap(keymap_entry **map, int *length);
    
@class ProgramListDataSource;
@class SkinListDataSource;
@class CalcView;
@class PrintView;
@class StatesWindow;

@interface Free42AppDelegate : NSObject <WKNavigationDelegate> {
    NSWindow *mainWindow;
    CalcView *calcView;
    
    NSWindow *printWindow;
    PrintView *printView;
    
    NSWindow *preferencesWindow;
    NSButton *prefsSingularMatrix;
    NSButton *prefsMatrixOutOfRange;
    NSButton *prefsAutoRepeat;
    NSButton *prefsLocalizedCopyPaste;
    NSButton *prefsPrintText;
    NSTextField *prefsPrintTextFile;
    NSButton *prefsPrintGIF;
    NSTextField *prefsPrintGIFFile;
    NSTextField *prefsPrintGIFMaxHeight;
    
    NSWindow *selectProgramsWindow;
    NSTableView *programListView;
    ProgramListDataSource *programListDataSource;
    
    NSWindow *aboutWindow;
    NSTextField *aboutVersion;
    NSTextField *aboutCopyright;

    NSWindow *loadSkinsWindow;
    NSTextField *loadSkinsURL;
    NSButton *loadSkinButton;
    WKWebView *loadSkinsWebView;
    NSWindow *deleteSkinsWindow;
    NSTableView *skinListView;
    SkinListDataSource *skinListDataSource;
    
    StatesWindow *statesWindow;
    
    // Skin loading stuff
    NSURLSession *session;
    NSURLSessionDataTask *task[2];
    BOOL taskSuccess[2];
    NSString *skinName;
}

@property (nonatomic, retain) IBOutlet NSWindow *mainWindow;
@property (nonatomic, retain) IBOutlet CalcView *calcView;
@property (nonatomic, retain) IBOutlet NSWindow *printWindow;
@property (nonatomic, retain) IBOutlet PrintView *printView;
@property (nonatomic, retain) IBOutlet NSWindow *preferencesWindow;
@property (nonatomic, retain) IBOutlet NSButton *prefsSingularMatrix;
@property (nonatomic, retain) IBOutlet NSButton *prefsMatrixOutOfRange;
@property (nonatomic, retain) IBOutlet NSButton *prefsAutoRepeat;
@property (nonatomic, retain) IBOutlet NSButton *prefsLocalizedCopyPaste;
@property (nonatomic, retain) IBOutlet NSButton *prefsPrintText;
@property (nonatomic, retain) IBOutlet NSTextField *prefsPrintTextFile;
@property (nonatomic, retain) IBOutlet NSButton *prefsPrintGIF;
@property (nonatomic, retain) IBOutlet NSTextField *prefsPrintGIFFile;
@property (nonatomic, retain) IBOutlet NSTextField *prefsPrintGIFMaxHeight;
@property (nonatomic, retain) IBOutlet NSWindow *selectProgramsWindow;
@property (nonatomic, retain) IBOutlet NSTableView *programListView;
@property (nonatomic, retain) IBOutlet ProgramListDataSource *programListDataSource;
@property (nonatomic, retain) IBOutlet NSWindow *aboutWindow;
@property (nonatomic, retain) IBOutlet NSTextField *aboutVersion;
@property (nonatomic, retain) IBOutlet NSTextField *aboutCopyright;
@property (nonatomic, retain) IBOutlet NSWindow *loadSkinsWindow;
@property (nonatomic, retain) IBOutlet NSTextField *loadSkinsURL;
@property (nonatomic, retain) IBOutlet NSButton *loadSkinButton;
@property (nonatomic, retain) IBOutlet WKWebView *loadSkinsWebView;
@property (nonatomic, retain) IBOutlet NSWindow *deleteSkinsWindow;
@property (nonatomic, retain) IBOutlet NSTableView *skinListView;
@property (nonatomic, retain) IBOutlet SkinListDataSource *skinListDataSource;
@property (nonatomic, retain) IBOutlet StatesWindow *statesWindow;

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification;
- (void)applicationWillTerminate:(NSNotification *)aNotification;
- (void)windowWillClose:(NSNotification *)notification;
- (void)windowDidBecomeKey:(NSNotification *)notification;

- (IBAction) showAbout:(id)sender;
- (IBAction) showDocumentation:(id)sender;
- (IBAction) showWebSite:(id)sender;
- (IBAction) showFree42WebSite:(id)sender;
- (IBAction) editKeymap:(id)sender;
- (IBAction) showPreferences:(id)sender;
- (void) getPreferences;
- (IBAction) browsePrintTextFile:(id)sender;
- (IBAction) browsePrintGIFFile:(id)sender;
- (IBAction) states:(id)sender;
- (IBAction) showPrintOut:(id)sender;
- (IBAction) paperAdvance:(id)sender;
- (IBAction) importPrograms:(id)sender;
- (IBAction) exportPrograms:(id)sender;
- (IBAction) exportProgramsCancel:(id)sender;
- (IBAction) exportProgramsOK:(id)sender;
- (IBAction) copy:(id)sender;
- (IBAction) paste:(id)sender;
- (IBAction) doCopyPrintOutAsText:(id)sender;
- (IBAction) doCopyPrintOutAsImage:(id)sender;
- (IBAction) clearPrintOut:(id)sender;
- (IBAction) loadSkins:(id)sender;
- (IBAction) loadSkinsGo:(id)sender;
- (IBAction) loadSkinsBack:(id)sender;
- (IBAction) loadSkinsForward:(id)sender;
- (IBAction) loadSkinsLoad:(id)sender;
- (IBAction) deleteSkins:(id)sender;
- (IBAction) deleteSkinsCancel:(id)sender;
- (IBAction) deleteSkinsOK:(id)sender;
+ (const char *) getVersion;
+ (void) showMessage:(NSString *)message withTitle:(NSString *)title;
+ (void) showCMessage:(const char *)message withTitle:(const char *)title;
- (IBAction) menuNeedsUpdate:(NSMenu *)menu;
- (void) selectSkin:(id)sender;
+ (void) loadState:(const char *)name;

@end
