#include <stdio.h>
#include <filesystem>

#include <mx_resources.h>

void mx_write_x_resources_needed(const std::string &file_name)
{
    if (std::filesystem::exists(file_name)) {
        return;
    }

    FILE *f = fopen(file_name.c_str(), "w");

    fprintf(f, 

R"(
##
## default font is Helvetica 14 point bold
##
*fontList:-adobe-helvetica-bold-r-normal--14-140-75-75-p-82-iso8859-1

*Text*fontList:-adobe-helvetica-*-r-normal-*-*-140-*-*-*-*-*-*
*XmText*fontList:-adobe-helvetica-*-r-normal-*-*-140-*-*-*-*-*-*

##
## main window
##
*startWP.labelString:New Document Window
*quitMaxwell.labelString:Exit

Maxwell.rowColumn.backgroundPixmap:mx_background.xpm
Maxwell.rowColumn.marginWidth:70
Maxwell.rowColumn.marginHeight:20
Maxwell.rowColumn*foreground:#ffff9c
Maxwell.rowColumn.dummy1.labelString: 
Maxwell.rowColumn.dummy2.labelString: 
Maxwell.rowColumn.dummy3.labelString: 
Maxwell.rowColumn.dummy4.labelString: 
Maxwell.rowColumn.dummy5.labelString: 
Maxwell.rowColumn.dummy6.labelString: 
Maxwell.rowColumn.dummy7.labelString: 
Maxwell.rowColumn.dummy8.labelString: 
Maxwell.rowColumn.dummy9.labelString: 
Maxwell.rowColumn.dummy10.labelString: 

*maxwellIcon.labelType:PIXMAP
*maxwellIcon.labelPixmap:mx_maxwell.xpm

##
## no traversal on toolbar except for font name, size and style name
##
*charStyleCheckbox*traversalOn: FALSE
*ssStyleRadiobox*traversalOn: FALSE
*paraStyleRadiobox*traversalOn: FALSE
*containerForm.form2.traversalOn: FALSE
*containerForm.form3.traversalOn: FALSE

##
## status bar
##
*statusBar*rowColumn*shadowThickness:2
*statusBar*fontList:-adobe-helvetica-*-r-normal-*-*-100-*-*-*-*-*-*

##
## initial device values
##
*screenDevice.width:300
*screenDevice.height:530

##
## delete key does backspace
##
*XmText.translations: #override <Key>osfDelete: delete-previous-character() 
*Text.translations: #override <Key>osfDelete: delete-previous-character() 

##
## Most background colours must be lightgrey to match the colour icons
## in various dialogs
##
*background:lightgrey

*maxwell*iconPixmap:mx_icon.xpm
*maxwell*iconName:mx/WP

*help*iconPixmap:mx_help_icon.xpm
*help*iconName:mx/Help

*help*iconName:mx/Help

maxwell.window.width:662
maxwell.window.height:720
help.help.width:501
help.help.height:568

##
## quit dialog
##
*exit*title:Quit
*quit*text.labelString:Quit from Maxwell/WP?
*quit*yes.labelString:Yes
*quit*yes.mnemonic:Y
*quit*no.labelString:No
*quit*no.mnemonic:N

##
## about dialog
##
*about*title:About
*about*background:#2121a5
*about*foreground:#ffff9c

*about*text1.alignment:ALIGNMENT_BEGINNING
*about*text1.labelString:Maxwell/WP Version 0.1 © 1996 Tangent Data Limited.
*about*text1.topOffset:10
*about*text1.leftOffset:5

*about*text3.labelString:Of:
*about*text3.topOffset:5
*about*text3.leftOffset:5

*about*text5.topOffset:5
*about*text6.topOffset:5
*about*text7.topOffset:5

*about*confirm.labelString:Confirm
*about*confirm.mnemonic:C
*about*maxwellIcon.labelPixmap:mx_about_maxwell.xpm


##
##open dialog
##
*open*title:Open File
*open.title:select a file
*open*selectText.labelString:select file
*open*pathText.labelString:Directory
*open*typeText.labelString:Type
*open*nameText.labelString:File Name
*open*nameText.leftOffset:5
*open*nameText.bottomOffset:6
*open*open.labelString:Open
*open*open.mnemonic:O
*open*cancel.labelString:Cancel
*open*cancel.mnemonic:C
*open*typePulldown*fontList:-adobe-helvetica-bold-o-normal-*-*-140-*-*-*-*-*-*
*open*pathPulldown*fontList:-adobe-helvetica-bold-o-normal-*-*-140-*-*-*-*-*-*
*open*permissions.labelString:Info
*open*permissions.mnemonic:I
*open*refresh.labelString:Refresh
*open*refresh.mnemonic:R

*open*fileList*fontList:-adobe-helvetica-*-r-normal-*-*-140-*-*-*-*-*-*=FILETAG,\
-adobe-helvetica-bold-o-normal-*-*-140-*-*-*-*-*-*=DIRTAG

*noWrite*yes.labelString:Open
*noWrite*yes.mnemonic:O
*noWrite*no.labelString:Cancel
*noWrite*no.mnemonic:C
*noWrite*text.labelString:Write permission denied, open read-only?

*noRead*confirm.labelString:Confirm
*noRead*confirm.mnemonic:C
*noRead*text.labelString:Permission denied.

*noExist*confirm.labelString:Confirm
*noExist*confirm.mnemonic:C
*noExist*text.labelString:No such file.

*noExec*confirm.labelString:Confirm
*noExec*confirm.mnemonic:C
*noExec*text.labelString:No execute permission for directory.

## file info sub-dialog
*openFilePerm*title:Permissions
*openFilePerm*writePermText.labelString:Write
*openFilePerm*readPermText.labelString:Read
*openFilePerm*ureadToggle.labelString:User
*openFilePerm*uwriteToggle.labelString:User
*openFilePerm*greadToggle.labelString:Group
*openFilePerm*gwriteToggle.labelString:Group
*openFilePerm*oreadToggle.labelString:Other
*openFilePerm*owriteToggle.labelString:Other
*openFilePerm*closePermButton.labelString:Close
*openFilePerm*closePermButton.mnemonic:C
*openFilePerm*rowColumn1.marginHeight:0
*openFilePerm*rowColumn1.spacing:17
*openFilePerm*nameText.labelString:Name
*openFilePerm*typeText.labelString:Type
*openFilePerm*ownerText.labelString:Owner
*openFilePerm*groupText.labelString:Group
*openFilePerm*modifiedText.labelString:Last Changed

*helpDisplay*background:white
*helpDisplay*helpButton*background:lightgrey
*helpArea.width:660
*helpArea.height:720

*workFrame*frame.shadowType:shadow_in
!!*workFrame.frame.shadowThickness:4

*statusBar*messageFrame.shadowType:shadow_in

*screenDevice.background:white
*dummyDevice.background:white

*fileItem.labelString:File
*fileItem.mnemonic:F

*editItem.labelString:Edit
*editItem.mnemonic:E

*viewItem.labelString:View
*viewItem.mnemonic:V

*tableMenuItem.labelString:Table
*tableMenuItem.mnemonic:T

*insertItem.labelString:Insert
*insertItem.mnemonic:I

*formatItem.labelString:Format
*formatItem.mnemonic:m

*toolsItem.labelString:Tools
*toolsItem.mnemonic:o

*helpItem.labelString:Help
*helpItem.mnemonic:H

*newItem.labelString:New...
*newItem.mnemonic:N
*newItem.accelerator:Ctrl<Key>n
*newItem.acceleratorText:Ctrl+N

*openItem.labelString:Open...
*openItem.mnemonic:O
*openItem.accelerator:Ctrl<Key>o
*openItem.acceleratorText:Ctrl+O

*closeItem.labelString:Close
*closeItem.mnemonic:C
#*closeItem.accelerator:
#*closeItem.acceleratorText:

*saveItem.labelString:Save
*saveItem.mnemonic:S
*saveItem.accelerator:Ctrl<Key>s
*saveItem.acceleratorText:Ctrl+S

*saveAsItem.labelString:Save As...
*saveAsItem.mnemonic:A
#*saveAsItem.accelerator:
#*saveAsItem.acceleratorText:

*printItem.labelString:Print...
*printItem.mnemonic:P
*printItem.accelerator:Ctrl<Key>p
*printItem.acceleratorText:Ctrl+P

*undoItem.labelString:Undo
*undoItem.mnemonic:U
*undoItem.accelerator:Ctrl<Key>z
*undoItem.acceleratorText:Ctrl+Z

*redoItem.labelString:Redo
*redoItem.mnemonic:R
*redoItem.accelerator:Ctrl<Key>y
*redoItem.acceleratorText:Ctrl+Y

*cutItem.labelString:Cut
*cutItem.mnemonic:t
*cutItem.accelerator:Ctrl<Key>x
*cutItem.acceleratorText:Ctrl+X

*copyItem.labelString:Copy
*copyItem.mnemonic:C
*copyItem.accelerator:Ctrl<Key>c
*copyItem.acceleratorText:Ctrl+C

*pasteItem.labelString:Paste
*pasteItem.mnemonic:P
*pasteItem.accelerator:Ctrl<Key>v
*pasteItem.acceleratorText:Ctrl+V

*deleteRowsItem.labelString:Delete Rows
*deleteRowsItem.mnemonic:w

*deleteColumnsItem.labelString:Delete Columns
*deleteColumnsItem.mnemonic:l

*selectAllItem.labelString:Select All
*selectAllItem.mnemonic:A
*selectAllItem.accelerator:Ctrl<Key>a
*selectAllItem.acceleratorText:Ctrl+A

*findItem.labelString:Find...
*findItem.mnemonic:F
*findItem.accelerator:Ctrl<Key>f
*findItem.acceleratorText:Ctrl+F

*findAgainItem.labelString:Find Again
*findAgainItem.mnemonic:A
*findAgainItem.accelerator:Ctrl<Key>g
*findAgainItem.acceleratorText:Ctrl+G

*replaceItem.labelString:Replace...
*replaceItem.mnemonic:e

*gotoItem.labelString:Goto Page...
*gotoItem.mnemonic:G
*gotoItem.accelerator:Ctrl<Key>#
*gotoItem.acceleratorText:Ctrl+#

*gotoStartItem.labelString:Goto Start
*gotoStartItem.mnemonic:S

*gotoEndItem.labelString:Goto End
*gotoEndItem.mnemonic:E

*outlineItem.labelString:¶ symbols

*rulerItem.labelString:Ruler
*rulerItem.mnemonic:R
*rulerItem.accelerator:Ctrl<Key>r
*rulerItem.acceleratorText:Ctrl+R

*headerItem.labelString:Header...
*headerItem.mnemonic:H

*footerItem.labelString:Footer...
*footerItem.mnemonic:F

*breakItem.labelString:Page Break
*breakItem.mnemonic:B

*pageNumberItem.labelString:Page Number
*pageNumberItem.mnemonic:N

*rowsItem.labelString:Insert Rows...
*rowsItem.mnemonic:R

*columnsItem.labelString:Insert Columns...
*columnsItem.mnemonic:C

*formatColumnsItem.labelString:Columns...
*formatColumnsItem.mnemonic:l

*dateTimeItem.labelString:Date & Time...

*symbolItem.labelString:Symbol...
*symbolItem.mnemonic:S

*imageItem.labelString:Image...
*imageItem.mnemonic:I

*diagramItem.labelString:Diagram...
*diagramItem.mnemonic:D

*insertTOCItem.labelString:Insert TOC...
*insertTOCItem.mnemonic:C

*insertIndexItem.labelString:Insert Index...
*insertIndexItem.mnemonic:n

*insertIndexEntryItem.labelString:Insert Index Entry
*insertIndexEntryItem.mnemonic:E
*insertIndexEntryItem.accelerator:
*insertIndexEntryItem.acceleratorText:

*tableItem.labelString:Table...
*tableItem.mnemonic:T

*insertFileItem.labelString:Document...
*insertFileItem.mnemonic:o

*characterItem.labelString:Character...
*characterItem.mnemonic:C
*characterItem.accelerator:Alt<Key>c
*characterItem.acceleratorText:Alt+C

*paragraphItem.labelString:Paragraph...
*paragraphItem.mnemonic:P
*paragraphItem.accelerator:Alt<Key>p
*paragraphItem.acceleratorText:Alt+P

*borderItem.labelString:Border...
*borderItem.mnemonic:B
*borderItem.accelerator:Alt<Key>b
*borderItem.acceleratorText:Alt+B

*tabsItem.labelString:Tabs...
*tabsItem.mnemonic:T

*styleItem.labelString:Style...
*styleItem.mnemonic:S
*styleItem.accelerator:Alt<Key>s
*styleItem.acceleratorText:Alt+S

*pageSetupItem.labelString:Page Setup...
*pageSetupItem.mnemonic:g

*imageFormatItem.labelString:Image...
*imageFormatItem.mnemonic:I

*diagramFormatItem.labelString:Diagram...
*diagramFormatItem.mnemonic:D

*tableFormatItem.labelString:Table Column...
*tableFormatItem.mnemonic:e

*tableStyleFormatItem.labelString:Table Position...
*tableStyleFormatItem.mnemonic:o

*makeNormalItem*fontList:-adobe-helvetica-*-r-normal-*-*-140-*-*-*-*-*-*
*makeNormalItem.labelString:Normal
*makeNormalItem.accelerator:Ctrl<Key>t
*makeNormalItem.acceleratorText:Ctrl+T

*makeBoldItem*fontList:-urw-nimbus sans l-bold-r-normal--14-140-75-75-p-82-iso8859-1
*makeBoldItem.labelString:Bold
*makeBoldItem.accelerator:Ctrl<Key>b
*makeBoldItem.acceleratorText:Ctrl+B

*makeItalicItem*fontList:-adobe-helvetica-bold-o-normal-*-*-140-*-*-*-*-*-*
*makeItalicItem.labelString:Italic
*makeItalicItem.accelerator:Ctrl<Key>i
*makeItalicItem.acceleratorText:Ctrl+I

*makeUnderlineItem*fontList:-adobe-helvetica-*-r-normal-*-*-140-*-*-*-*-*-*
*makeUnderlineItem.labelString:Underline
*makeUnderlineItem.accelerator:Ctrl<Key>u
*makeUnderlineItem.acceleratorText:Ctrl+U

*spellingItem.labelString:Spelling...
*spellingItem.mnemonic:S
*spellingItem.accelerator:Ctrl<Key>l
*spellingItem.acceleratorText:Ctrl+L

*optionsItem.labelString:Options...
*optionsItem.mnemonic:O

*helpIndexItem.labelString:Index...
*helpIndexItem.mnemonic:I

*productSupportItem.labelString:Support...
*productSupportItem.mnemonic:S

*aboutItem.labelString:About Maxwell...
*aboutItem.mnemonic:A

*zoomItem.labelString:Zoom...
*zoomItem.mnemonic:Z

*bulletsItem.labelString:Bullets
*bulletsItem.mnemonic:B

*envelopeItem.labelString:Envelope
*envelopeItem.mnemonic:E

*repaginateItem.labelString:Repaginate Now
*repaginateItem.mnemonic:R

*viewToolsItem.labelString:Tool Buttons
*viewStylesItem.labelString:Style Buttons
*viewRulerItem.labelString:Ruler
*viewTableGuidesItem.labelString:Table Guides

*menuForm*borderWidth:0
*menuForm*marginLeft:0
*menuForm*marginTop:2
*menuForm*marginRight:0
*menuForm*marginWidth:0
*menuForm*shadowThickness:1

*newButton.labelType:PIXMAP
*newButton.labelPixmap:mx_create.xpm
*newButton.leftOffset:5

*openButton.labelType:PIXMAP
*openButton.labelPixmap:mx_open.xpm

*saveButton.labelType:PIXMAP
*saveButton.labelPixmap:mx_save.xpm
*saveButton.labelInsensitivePixmap:mx_save_ins.xpm

*quitButton.labelType:PIXMAP
*quitButton.labelPixmap:mx_quit.xpm

*printButton.labelType:PIXMAP
*printButton.labelPixmap:mx_print.xpm

*cutButton.labelType:PIXMAP
*cutButton.labelPixmap:mx_cut.xpm
*cutButton.labelInsensitivePixmap:mx_cut_ins.xpm

*copyButton.labelType:PIXMAP
*copyButton.labelPixmap:mx_copy.xpm
*copyButton.labelInsensitivePixmap:mx_copy_ins.xpm

*pasteButton.labelType:PIXMAP
*pasteButton.labelPixmap:mx_paste.xpm
*pasteButton.labelInsensitivePixmap:mx_paste_ins.xpm

*findButton.labelType:PIXMAP
*findButton.labelPixmap:mx_find.xpm

*replaceButton.labelType:PIXMAP
*replaceButton.labelPixmap:mx_replace.xpm

*gotoButton.labelType:PIXMAP
*gotoButton.labelPixmap:mx_goto.xpm
*gotoButton.labelInsensitivePixmap:mx_goto_ins.xpm

*headerButton.labelType:PIXMAP
*headerButton.labelPixmap:mx_header.xpm

*footerButton.labelType:PIXMAP
*footerButton.labelPixmap:mx_footer.xpm

*insertImageButton.labelType:PIXMAP
*insertImageButton.labelPixmap:mx_insert_image.xpm

*insertDiagramButton.labelType:PIXMAP
*insertDiagramButton.labelPixmap:mx_insert_diagram.xpm

*insertTableButton.labelType:PIXMAP
*insertTableButton.labelPixmap:mx_insert_table.xpm
*insertTableButton.labelInsensitivePixmap:mx_insert_table_ins.xpm

*insertBulletsButton.labelType:PIXMAP
*insertBulletsButton.labelPixmap:mx_insert_bullets.xpm

*insertSymbolButton.labelType:PIXMAP
*insertSymbolButton.labelPixmap:mx_insert_symbol.xpm

*insertPageBreakButton.labelType:PIXMAP
*insertPageBreakButton.labelPixmap:mx_insert_page_break.xpm
*insertPageBreakButton.labelInsensitivePixmap:mx_insert_page_break_ins.xpm

*formatCharacterButton.labelType:PIXMAP
*formatCharacterButton.labelPixmap:mx_format_character.xpm

*formatParagraphButton.labelType:PIXMAP
*formatParagraphButton.labelPixmap:mx_format_paragraph.xpm

*formatColumnsButton.labelType:PIXMAP
*formatColumnsButton.labelPixmap:mx_format_columns.xpm
*formatColumnsButton.labelInsensitivePixmap:mx_format_columns_ins.xpm

*formatTableButton.labelType:PIXMAP
*formatTableButton.labelPixmap:mx_format_table.xpm
*formatTableButton.labelInsensitivePixmap:mx_format_table_ins.xpm

*spellingButton.labelType:PIXMAP
*spellingButton.labelPixmap:mx_spelling.xpm

*envelopeButton.labelType:PIXMAP
*envelopeButton.labelPixmap:mx_envelope.xpm

*boldButton.labelType:PIXMAP
*boldButton.labelPixmap:mx_bold.xpm
*boldButton.labelInsensitivePixmap:mx_bold_ins.xpm
*boldButton.spacing:1

*italicButton.labelType:PIXMAP
*italicButton.labelPixmap:mx_italic.xpm
*italicButton.labelInsensitivePixmap:mx_italic_ins.xpm
*italicButton.spacing:1

*underlineButton.labelType:PIXMAP
*underlineButton.labelPixmap:mx_underline.xpm
*underlineButton.spacing:1

*superscriptButton.labelType:PIXMAP
*superscriptButton.labelPixmap:mx_superscript.xpm
*superscriptButton.spacing:1

*subscriptButton.labelType:PIXMAP
*subscriptButton.labelPixmap:mx_subscript.xpm
*subscriptButton.spacing:1

*justifyButton.labelType:PIXMAP
*justifyButton.labelPixmap:mx_justify.xpm
*justifyButton.spacing:1

*leftJustifyButton.labelType:PIXMAP
*leftJustifyButton.labelPixmap:mx_left_justify.xpm
*leftJustifyButton.spacing:1

*rightJustifyButton.labelType:PIXMAP
*rightJustifyButton.labelPixmap:mx_right_justify.xpm
*rightJustifyButton.spacing:1

*centreJustifyButton.labelType:PIXMAP
*centreJustifyButton.labelPixmap:mx_centre_justify.xpm
*centreJustifyButton.spacing:1

*styleForm.background:gray60
*menuForm.background:gray60

*styleForm*fontNameCombo*fontList:-adobe-helvetica-*-r-normal-*-*-100-*-*-*-*-*-*
*styleForm*fontNameCombo.width:210
*styleForm*fontNameCombo.marginWidth:0
*styleForm*fontNameCombo.arrowSize:15
*styleForm*fontNameCombo.marginHeight:0
*styleForm*fontNameCombo*Text.marginHeight:1
*styleForm*fontNameCombo.topOffset:2
*styleForm*fontNameCombo.bottomOffset:2
*styleForm*fontNameCombo.highlightThickness:0
*styleForm*fontNameCombo*Text.translations: #override <Key>osfDelete: delete-previous-character() 

*styleForm*fontSizeCombo*fontList:-adobe-helvetica-*-r-normal-*-*-100-*-*-*-*-*-*
*styleForm*fontSizeCombo.width:60
*styleForm*fontSizeCombo.marginWidth:0
*styleForm*fontSizeCombo.arrowSize:15
*styleForm*fontSizeCombo.marginHeight:0
*styleForm*fontSizeCombo*Text.marginHeight:1
*styleForm*fontSizeCombo.topOffset:2
*styleForm*fontSizeCombo.bottomOffset:2
*styleForm*fontSizeCombo.highlightThickness:0
*styleForm*fontSizeCombo*Text.translations: #override <Key>osfDelete: delete-previous-character() 

*styleForm*namedStyleCombo*fontList:-adobe-helvetica-*-r-normal-*-*-100-*-*-*-*-*-*
*styleForm*namedStyleCombo.width:140
*styleForm*namedStyleCombo.marginWidth:0
*styleForm*namedStyleCombo.arrowSize:15
*styleForm*namedStyleCombo.marginHeight:0
*styleForm*namedStyleCombo*Text.marginHeight:1
*styleForm*namedStyleCombo.topOffset:2
*styleForm*namedStyleCombo.bottomOffset:2
*styleForm*namedStyleCombo.highlightThickness:0
*styleForm*namedStyleCombo*Text.translations: #override <Key>osfDelete: delete-previous-character() 

*charStyleCheckbox*background:gray60
*paraStyleRadiobox*background:gray60
*ssStyleRadiobox*background:gray60

*highlightColor:#E79D965879E7

##
## save dialog
##
*save.title:select a file
*save*selectText.labelString:select file
*save*pathText.labelString:Directory
*save*typeText.labelString:Type
*save*nameText.labelString:File Name
*save*nameText.leftOffset:5
*save*nameText.bottomOffset:6
*save*save.labelString:Save
*save*save.mnemonic:S
*save*cancel.labelString:Cancel
*save*cancel.mnemonic:C
*save*typePulldown*fontList:-adobe-helvetica-bold-o-normal-*-*-140-*-*-*-*-*-*
*save*pathPulldown*fontList:-adobe-helvetica-bold-o-normal-*-*-140-*-*-*-*-*-*
*save*permissions.labelString:Info
*save*permissions.mnemonic:I
*save*refresh.labelString:Refresh
*save*refresh.mnemonic:R

*save*fileList*fontList:-adobe-helvetica-*-r-normal-*-*-140-*-*-*-*-*-*=FILETAG,\
-adobe-helvetica-bold-o-normal-*-*-140-*-*-*-*-*-*=DIRTAG

*noWriteSave*confirm.labelString:Confirm
*noWriteSave*confirm.mnemonic:C
*noWriteSave*text.labelString:Write permission denied.

*noCreate*confirm.labelString:Confirm
*noCreate*confirm.mnemonic:C
*noCreate*text.labelString:Create permission denied (no write permission on directory).

*overwriteSave*text.labelString:Overwrite existing file?
*overwriteSave*yes.labelString:Yes
*overwriteSave*yes.mnemonic:Y
*overwriteSave*no.labelString:No
*overwriteSave*no.mnemonic:N

*closeLosingChanges*text.labelString:Close window losing changes?
*closeLosingChanges*yes.labelString:Yes
*closeLosingChanges*yes.mnemonic:Y
*closeLosingChanges*no.labelString:No
*closeLosingChanges*no.mnemonic:N

*new*new.labelString:New
*new*new.mnemonic:N
*new*cancel.labelString:Cancel
*new*cancel.mnemonic:C

##
## print dialog
##
*print*printerList.visibleItemCount:10
*print*resLabel.topOffset:5
*print*resLabel.leftOffset:5
*print*resLabel.labelString:Output Resolution (x, y)
*print*print.labelString:Print
*print*print.mnemonic:P
*print*cancel.labelString:Cancel
*print*cancel.mnemonic:C
*print*allButton.labelString:All
*print*currentButton.labelString:Current
*print*rangeButton.labelString:Range
*print*listButton.labelString:List
*print*rangeLabel.labelString:Pages
*print*startTextLabel.labelString:From
*print*endTextLabel.labelString:To
*print*listTextLabel.labelString:Pages
*print*startText.width:42
*print*endText.width:42
*print*listText.width:121
*print*numCopiesLabel.topOffset:15
*print*numCopiesLabel.labelString:Copies
*print*numCopiesLabel.leftOffset:5
*print*numCopiesText.topOffset:5
*print*parityLabel.topOffset:10
*print*parityLabel.labelString:Pages
*print*parityLabel.leftOffset:5
*print*collateButton.labelString:Collate Copies
*print*collateButton.leftOffset:5
*print*toFileButton.labelString:Print To File
*print*parityAllButton.labelString:All
*print*parityEvenButton.labelString:Even
*print*parityOddButton.labelString:Odd
*print*numCopiesText.width:42
*print*icon.topOffset:20
*print*combo1.width:110
*print*combo2.width:110
*print*includeFontsButton.labelString:Include Font Files
*print*includeFontsButton.leftOffset:5

*print*collateButton.labelString:Collate Copies
*print*collateButton.leftOffset:5

##
## exit losing changed dialog
##
*exit*title:Exit
*exit*text.labelString:Close losing changes to document?
*exit*yes.labelString:Yes
*exit*yes.mnemonic:Y
*exit*no.labelString:No
*exit*no.mnemonic:N

##
## save before exit dialog
##
*forceExit*title:Exit Now
*forceExit*text.labelString:Save changes before closing document?
*forceExit*yes.labelString:Yes
*forceExit*yes.mnemonic:Y
*forceExit*no.labelString:No
*forceExit*no.mnemonic:N

##
## search dialog
##
*search*findLabel.labelString:Search For
*search*findLabel.leftOffset:5
*search*findLabel.topOffset:10
*search*styleLabel.labelString:Match Style
*search*styleLabel.leftOffset:10
*search*caseToggle.topOffset:8
*search*caseToggle.labelType:PIXMAP
*search*caseToggle.labelPixmap:mx_find_dummy.xpm
*search*matchCase.labelString:Match Case
*search*matchCase.topOffset:10
*search*matchCase.leftOffset:13
*search*styleLabel.topOffset:10
*search*styleMenu.topOffset:1
*search*next.labelString:Forwards
*search*next.mnemonic:F
*search*prev.labelString:Backwards
*search*prev.mnemonic:B
*search*cancel.labelString:Dismiss
*search*cancel.mnemonic:D

##
## replace dialog
##
*replace*findLabel.labelString:Search For
*replace*findLabel.leftOffset:5
*replace*findLabel.topOffset:10
*replace*styleLabel.labelString:Match Style
*replace*styleLabel.leftOffset:10
*replace*caseToggle.topOffset:13
*replace*caseToggle.labelType:PIXMAP
*replace*caseToggle.labelPixmap:mx_find_dummy.xpm
*replace*matchCase.labelString:Match Case
*replace*matchCase.topOffset:15
*replace*matchCase.leftOffset:13
*replace*styleLabel.topOffset:15
*replace*styleMenu.topOffset:6
*replace*replaceWith.labelString:Replace With
*replace*replaceWith.topOffset:10
*replace*replaceWith.leftOffset:5
*replace*replaceText.topOffset:5
*replace*replaceText.leftOffset:5
*replace*next.labelString:Forwards
*replace*next.mnemonic:F
*replace*prev.labelString:Backwards
*replace*prev.mnemonic:B
*replace*all.labelString:All
*replace*all.mnemonic:A
*replace*cancel.labelString:Dismiss
*replace*cancel.mnemonic:D

##
## goto dialog
##
*goto*gotoLabel.leftOffset:5
*goto*gotoLabel.topOffset:10
*goto*gotoLabel.labelString:Goto Page Number
*goto*gotoText.topOffset:5
*goto*gotoText.leftOffset:5
*goto*cancel.labelString:Cancel
*goto*cancel.mnemonic:C
*goto*goto.labelString:Goto
*goto*goto.mnemonic:G

##
## zoom dialog
##
*zoom*ok.labelString:Ok
*zoom*ok.mnemonic:O
*zoom*cancel.labelString:Cancel
*zoom*cancel.mnemonic:C
*zoom*zoomLabel.labelString:Zoom Factor
*zoom*zoomLabel.topOffset:15
*zoom*zoomLabel.leftOffset:10
*zoom*combo.topOffset:5
*zoom*combo.leftOffset:5

##
## symbol dialog
##
*symbol*cancel.labelString:Cancel
*symbol*cancel.mnemonic:C

##
## table dialog
##
*table*rowsLabel.labelString:Number Of rows
*table*rowsLabel.topOffset:8
*table*rowsLabel.leftOffset:5
*table*columnsLabel.labelString:Number Of columns
*table*columnsLabel.topOffset:15
*table*columnsLabel.leftOffset:5
*table*widthLabel.labelString:Column Width
*table*widthLabel.topOffset:15
*table*widthLabel.leftOffset:5
*table*columnWidthText.width:100
*table*autoToggle.labelString:Automatic
*table*autoToggle.topOffset:3
*table*ok.labelString:Ok
*table*ok.mnemonic:O
*table*cancel.labelString:Cancel
*table*cancel.mnemonic:C
*table*rowsText.value:2
*table*columnsText.value:2

##
## info dialogs used by the table dialog
##
*invalidRows*confirm.labelString:Confirm
*invalidRows*confirm.mnemonic:C
*invalidRows*text.labelString:Invalid rows field

*invalidColumns*confirm.labelString:Confirm
*invalidColumns*confirm.mnemonic:C
*invalidColumns*text.labelString:Invalid columns field

*invalidWidth*confirm.labelString:Confirm
*invalidWidth*confirm.mnemonic:C
*invalidWidth*text.labelString:Invalid width field

##
## TOC dialog
##
*toc*cancel.labelString:Cancel
*toc*cancel.mnemonic:C
*toc*ok.labelString:Ok
*toc*ok.mnemonic:O
*toc*replaceLabel.labelString:Replace Existing
*toc*replaceLabel.topOffset:6
*toc*startLabel.labelString:From Page
*toc*startLabel.topOffset:15
*toc*endLabel.labelString:To Page
*toc*endLabel.topOffset:15
*toc*maxLevelLabel.labelString:Maximum Indent
*toc*maxLevelLabel.topOffset:15
*toc*replaceToggle.labelType:PIXMAP
*toc*replaceToggle.labelPixmap:mx_find_dummy.xpm

##
## info dialogs used by the TOC dialog
##
*invalidToTOC*confirm.labelString:Confirm
*invalidToTOC*confirm.mnemonic:C
*invalidToTOC*text.labelString:Invalid end page number

*invalidFromTOC*confirm.labelString:Confirm
*invalidFromTOC*confirm.mnemonic:C
*invalidFromTOC*text.labelString:Invalid start page number

*invalidIndentTOC*confirm.labelString:Confirm
*invalidIndentTOC*confirm.mnemonic:C
*invalidIndentTOC*text.labelString:Invalid maximum indent level

##
## Index dialog
##
*index*cancel.labelString:Cancel
*index*cancel.mnemonic:C
*index*ok.labelString:Ok
*index*ok.mnemonic:O
*index*replaceLabel.labelString:Replace Existing
*index*replaceLabel.topOffset:6
*index*startLabel.labelString:From Page
*index*startLabel.topOffset:15
*index*endLabel.labelString:To Page
*index*endLabel.topOffset:15
*index*replaceToggle.labelType:PIXMAP
*index*replaceToggle.labelPixmap:mx_find_dummy.xpm

##
## info dialogs used by the index dialog
##
*invalidToIndex*confirm.labelString:Confirm
*invalidToIndex*confirm.mnemonic:C
*invalidToIndex*text.labelString:Invalid end page number

*invalidFromIndex*confirm.labelString:Confirm
*invalidFromIndex*confirm.mnemonic:C
*invalidFromIndex*text.labelString:Invalid start page number

##
## character style dialog
##
*character*ok.labelString:Ok
*character*ok.mnemonic:O
*character*apply.labelString:Apply
*character*apply.mnemonic:A
*character*cancel.labelString:Cancel
*character*cancel.mnemonic:C

*character*boldToggle.labelType:PIXMAP
*character*boldToggle.labelPixmap:mx_bold_lg.xpm
*character*boldToggle.labelInsensitivePixmap:mx_bold_lg_ins.xpm
*character*boldToggle.spacing:1
*character*italicToggle.labelType:PIXMAP
*character*italicToggle.labelPixmap:mx_italic_lg.xpm
*character*italicToggle.labelInsensitivePixmap:mx_italic_lg_ins.xpm
*character*italicToggle.spacing:1
*character*underlineToggle.labelType:PIXMAP
*character*underlineToggle.labelPixmap:mx_underline_lg.xpm
*character*underlineToggle.spacing:1

*character*boldToggle.leftOffset:5
*character*boldToggle.topOffset:5
*character*italicToggle.leftOffset:5
*character*italicToggle.topOffset:5
*character*underlineToggle.leftOffset:5
*character*underlineToggle.topOffset:5

*character*alignmentOffsetCombo.width:120
*character*sizeCombo.width:120
*character*lineSpacingCombo.width:120
*character*charSpacingCombo.width:120

*character*alignmentOffsetCombo.marginHeight:0
*character*sizeCombo.marginHeight:0
*character*lineSpacingCombo.marginHeight:0
*character*charSpacingCombo.marginHeight:0

*character*alignmentOffsetCombo.marginWidth:0
*character*sizeCombo.marginWidth:0
*character*lineSpacingCombo.marginWidth:0
*character*charSpacingCombo.marginWidth:0

*character*alignmentOffsetCombo.arrowSize:15
*character*sizeCombo.arrowSize:15
*character*lineSpacingCombo.arrowSize:15
*character*charSpacingCombo.arrowSize:15

*character*alignmentOffsetCombo*Text.marginHeight:4
*character*sizeCombo*Text.marginHeight:4
*character*lineSpacingCombo*Text.marginHeight:4
*character*charSpacingCombo*Text.marginHeight:4

*character*charSpacingCombo*Text.translations: #override <Key>osfDelete: delete-previous-character() 
*character*lineSpacingCombo*Text.translations: #override <Key>osfDelete: delete-previous-character() 
*character*alignmentOffsetCombo*Text.translations: #override <Key>osfDelete: delete-previous-character() 
*character*sizeCombo*Text.translations: #override <Key>osfDelete: delete-previous-character() 

*character*charSpacingCombo*List.visibleItemCount:12
*character*lineSpacingCombo*List.visibleItemCount:13
*character*alignmentOffsetCombo*List.visibleItemCount:14
*character*sizeCombo*List.visibleItemCount:19

*character*alignmentOffsetLabel.marginHeight:0
*character*sizeLabel.marginHeight:0
*character*lineSpacingLabel.marginHeight:0
*character*charSpacingLabel.marginHeight:0

*character*alignmentOffsetLabel.labelString:Alignment Offset
*character*sizeLabel.labelString:Point Size
*character*lineSpacingLabel.labelString:Line Spacing
*character*charSpacingLabel.labelString:Character Spacing

*character*alignmentRAdio.leftOffset:5
*character*normal.labelString:Normal
*character*superscript.labelString:Superscript
*character*subscript.labelString:Subscript
*character*black.labelType:PIXMAP
*character*black.labelPixmap:mx_black.xpm
*character*white.labelType:PIXMAP
*character*white.labelPixmap:mx_white.xpm
*character*red.labelType:PIXMAP
*character*red.labelPixmap:mx_red.xpm
*character*orange.labelType:PIXMAP
*character*orange.labelPixmap:mx_orange.xpm
*character*yellow.labelType:PIXMAP
*character*yellow.labelPixmap:mx_yellow.xpm
*character*green.labelType:PIXMAP
*character*green.labelPixmap:mx_green.xpm
*character*blue.labelType:PIXMAP
*character*blue.labelPixmap:mx_blue.xpm
*character*indigo.labelType:PIXMAP
*character*indigo.labelPixmap:mx_indigo.xpm
*character*violet.labelType:PIXMAP
*character*violet.labelPixmap:mx_violet.xpm
*character*ltred.labelType:PIXMAP
*character*ltred.labelPixmap:mx_ltred.xpm
*character*ltorange.labelType:PIXMAP
*character*ltorange.labelPixmap:mx_ltorange.xpm
*character*ltyellow.labelType:PIXMAP
*character*ltyellow.labelPixmap:mx_ltyellow.xpm
*character*ltgreen.labelType:PIXMAP
*character*ltgreen.labelPixmap:mx_ltgreen.xpm
*character*ltblue.labelType:PIXMAP
*character*ltblue.labelPixmap:mx_ltblue.xpm
*character*ltindigo.labelType:PIXMAP
*character*ltindigo.labelPixmap:mx_ltindigo.xpm
*character*ltviolet.labelType:PIXMAP
*character*ltviolet.labelPixmap:mx_ltviolet.xpm
*character*colour.topOffset:5
*character*colour.rightOffset:5
*character*colour.leftOffset:15
*character*fontList*visibleItemCount:12
*character*sampleFrameLabel.labelString:Sample
*character*sampleFrame.leftOffset:5
*character*sampleFrame.topOffset:5
*character*sampleFrame.rightOffset:5
*character*sampleFrame.bottomOffset:5
*character*sampleFrame*height:60
*character*frame1.topOffset:5
*character*frame1.leftOffset:5
*character*frame1.rightOffset:5
*character*frame1.bottomOffset:5
*character*weightFrame.leftOffset:5
*character*weightFrame.topOffset:5
*character*weightFrame.bottomOffset:5
*character*radioFrame.leftOffset:5
*character*radioFrame.bottomOffset:5
*character*radioFrame.topOffset:5
*character*labelRC.topOffset:13
*character*labelRC.spacing:24

##
## help dialogs
##
*helpButton*armColor:   white
*helpButton*background: gray60
*helpFont: -adobe-helvetica-*-r-normal-*-*-140-*-*-*-*-*-*
*helpWindowSize: 500
*helpPixmap: help.xbm
 
*forwardButton.labelType:PIXMAP
*forwardButton.labelPixmap:mx_forward.xpm
*forwardButton.labelInsensitivePixmap:mx_forward_ins.xpm
*backButton.labelType:PIXMAP
*backButton.labelPixmap:mx_back.xpm
*backButton.labelInsensitivePixmap:mx_back_ins.xpm
*closeButton.labelType:PIXMAP
*closeButton.labelPixmap:mx_quit.xpm
*printButton.labelType:PIXMAP
*printButton.labelPixmap:mx_print.xpm
*findButton.labelType:PIXMAP
*findButton.labelPixmap:mx_find.xpm

*forwardItem.labelString:Forward
*forwardItem.mnemonic:F
*forwardItem.accelerator:Ctrl<Key>Prior
*forwardItem.acceleratorText:Ctrl+Page Up

*backwardItem.labelString:Backward
*backwardItem.mnemonic:B
*backwardItem.accelerator:Ctrl<Key>Next
*backwardItem.acceleratorText:Ctrl+Page Down

Help*blob: fffff

##
## paragraph style dialog
##
*paragraph*ok.labelString:Ok
*paragraph*ok.mnemonic:O
*paragraph*apply.labelString:Apply
*paragraph*apply.mnemonic:A
*paragraph*cancel.labelString:Cancel
*paragraph*cancel.mnemonic:C
*paragraph*frame1.leftOffset:5
*paragraph*frame1.topOffset:5
*paragraph*frame1.rightOffset:5
*paragraph*frame1.bottomOffset:5
*paragraph*frame2.leftOffset:5
*paragraph*frame2.topOffset:5
*paragraph*frame2.rightOffset:5
*paragraph*frame2.bottomOffset:5
*paragraph*frame3.topOffset:5
*paragraph*frame3.rightOffset:5
*paragraph*frame3.bottomOffset:5
*paragraph*form1Labels.leftOffset:5
*paragraph*justifyLabel.labelString:Justification
*paragraph*justifyLabel.topOffset:13
*paragraph*keepLabel.labelString:Keep
*paragraph*keepLabel.topOffset:18
*paragraph*spaceBeforeLabel.labelString:Space Before
*paragraph*spaceBeforeLabel.topOffset:27
*paragraph*spaceAfterLabel.labelString:Space After
*paragraph*spaceAfterLabel.topOffset:15
*paragraph*spaceAfterText.leftOffset:5
*paragraph*spaceBeforeText.topOffset:5
*paragraph*spaceBeforeText.leftOffset:5
*paragraph*leftIndentLabel.labelString:Left Indent
*paragraph*leftIndentLabel.topOffset:20
*paragraph*rightIndentLabel.labelString:Right Indent
*paragraph*rightIndentLabel.topOffset:15
*paragraph*firstLineIndentLabel.labelString:First Line Indent
*paragraph*firstLineIndentLabel.topOffset:15
*paragraph*lineSpacingLabel.labelString:Line Spacing
*paragraph*lineSpacingLabel.topOffset:22
*paragraph*leftIndentText.topOffset:5
*paragraph*leftIndentText.leftOffset:5
*paragraph*rightIndentText.leftOffset:5
*paragraph*firstLineIndentText.leftOffset:5
*paragraph*breakBefore.labelString:Page Break Before
*paragraph*breakBefore.topOffset:5

#*paragraph*left.labelString:Left
#*paragraph*right.labelString:Right
#*paragraph*full.labelString:Full
#*paragraph*centre.labelString:Centre

*paragraph*left.labelType:PIXMAP
*paragraph*left.labelPixmap:mx_left_justify_lg.xpm
*paragraph*right.labelType:PIXMAP
*paragraph*right.labelPixmap:mx_right_justify_lg.xpm
*paragraph*full.labelType:PIXMAP
*paragraph*full.labelPixmap:mx_justify_lg.xpm
*paragraph*centre.labelType:PIXMAP
*paragraph*centre.labelPixmap:mx_centre_justify_lg.xpm

*paragraph*together.labelString:Together
*paragraph*withNext.labelString:With Next
*paragraph*withPrev.labelString:With Previous
*paragraph*heading.labelString:Is Heading Style
*paragraph*toc.labelString:Is TOC Style
*paragraph*levelLabel.labelString:Level
*paragraph*heading.marginHeight:1
*paragraph*toc.marginHeight:1
#*paragraph*combo.topOffset:145
*paragraph*combo.leftOffset:5
*paragraph*combo.width:70
*paragraph*combo.marginWidth:0
*paragraph*combo.marginHeight:0
*paragraph*combo*Text.marginHeight:3
*paragraph*combo.arrowSize:15
*paragraph*combo1.topOffset:5
*paragraph*combo1.leftOffset:3
*paragraph*combo1.width:120
*paragraph*combo1.marginWidth:0
*paragraph*combo1.marginHeight:0
*paragraph*combo1*Text.marginHeight:3
*paragraph*combo1.arrowSize:15
*paragraph*border.labelString:Border
*paragraph*border.mnemonic:B
*paragraph*border.topOffset:20
*paragraph*character.labelString:Character
*paragraph*character.mnemonic:h
*paragraph*character.topOffset:20
*paragraph*tabs.labelString:Tabs
*paragraph*tabs.mnemonic:T
*paragraph*tabs.topOffset:20

##
## border style dialog
##
*border*drawingArea*width:270

*border*widthCombo.marginHeight:0
*border*widthCombo.marginWidth:0
*border*widthCombo.arrowSize:20

*border*distanceCombo.marginHeight:0
*border*distanceCombo.marginWidth:0
*border*distanceCombo.arrowSize:20

*border*widthCombo*width:50
*border*distanceCombo*width:50
*border*frame1.leftOffset:5
*border*frame1.topOffset:5
*border*frame1.bottomOffset:5
*border*frame2.bottomOffset:5
*border*frame2.topOffset:5
*border*frame2.leftOffset:5
*border*frame2.rightOffset:5
*border*transparent.labelType:PIXMAP
*border*transparent.labelPixmap:mx_dd_transparent.xpm
*border*black.labelType:PIXMAP
*border*black.labelPixmap:mx_dd_black.xpm
*border*white.labelType:PIXMAP
*border*white.labelPixmap:mx_dd_white.xpm
*border*red.labelType:PIXMAP
*border*red.labelPixmap:mx_dd_red.xpm
*border*orange.labelType:PIXMAP
*border*orange.labelPixmap:mx_dd_orange.xpm
*border*yellow.labelType:PIXMAP
*border*yellow.labelPixmap:mx_dd_yellow.xpm
*border*green.labelType:PIXMAP
*border*green.labelPixmap:mx_dd_green.xpm
*border*blue.labelType:PIXMAP
*border*blue.labelPixmap:mx_dd_blue.xpm
*border*indigo.labelType:PIXMAP
*border*indigo.labelPixmap:mx_dd_indigo.xpm
*border*violet.labelType:PIXMAP
*border*violet.labelPixmap:mx_dd_violet.xpm
*border*ltred.labelType:PIXMAP
*border*ltred.labelPixmap:mx_dd_ltred.xpm
*border*ltorange.labelType:PIXMAP
*border*ltorange.labelPixmap:mx_dd_ltorange.xpm
*border*ltyellow.labelType:PIXMAP
*border*ltyellow.labelPixmap:mx_dd_ltyellow.xpm
*border*ltgreen.labelType:PIXMAP
*border*ltgreen.labelPixmap:mx_dd_ltgreen.xpm
*border*ltblue.labelType:PIXMAP
*border*ltblue.labelPixmap:mx_dd_ltblue.xpm
*border*ltindigo.labelType:PIXMAP
*border*ltindigo.labelPixmap:mx_dd_ltindigo.xpm
*border*ltviolet.labelType:PIXMAP
*border*ltviolet.labelPixmap:mx_dd_ltviolet.xpm
*border*solid.labelType:PIXMAP
*border*solid.labelPixmap:mx_solid.xpm
*border*dotted.labelType:PIXMAP
*border*dotted.labelPixmap:mx_dotted.xpm
*border*dashed.labelType:PIXMAP
*border*dashed.labelPixmap:mx_dashed.xpm
*border*form.leftOffset:5
*border*form.topOffset:5
*border*ok.labelString:Ok
*border*ok.mnemonic:O
*border*apply.labelString:Apply
*border*apply.mnemonic:A
*border*cancel.labelString:Cancel
*border*cancel.mnemonic:C
*border*widthLabel.labelString:Width
*border*distanceLabel.labelString:From Text
*border*styleLabel.labelString:Line Style
*border*lineColourLabel.labelString:Line Colour
*border*bgColourLabel.labelString:Shading
*border*distanceText.width:50
*border*labelRC.topOffset:13
*border*labelRC.spacing:24
*border*distanceCombo*Text.translations: #override <Key>osfDelete: delete-previous-character() 

##
## tab dialog
##
*tab*ok.labelString:Ok
*tab*ok.mnemonic:O
*tab*apply.labelString:Apply
*tab*apply.mnemonic:p
*tab*cancel.labelString:Cancel
*tab*cancel.mnemonic:C
*tab*add.labelString:Add
*tab*add.mnemonic:A
*tab*delete.labelString:Delete
*tab*delete.mnemonic:D

*tab*justificationTypeLabel.labelString:Tab Type
*tab*tabLeadTypeLabel.labelString:Tab Leader Type
*tab*noTabLeadToggle.labelString:None
*tab*dotTabLeadToggle.labelString: ......................
*tab*ulTabLeadToggle.labelString: ___________
*tab*eqTabLeadToggle.labelString: ==========
*tab*hypTabLeadToggle.labelString: ----------
*tab*tabform3.verticalSpacing: 5
*tab*tabframe1.leftOffset:6
*tab*tabframe1.rightOffset:5
*tab*tabframe2.leftOffset:5
*tab*tabframe2.rightOffset:1
*tab*tabform1.horizontalSpacing:16
*tab*tabform2.horizontalSpacing:30
*tab*tabform1.verticalSpacing:5
*tab*tabform2.verticalSpacing:5

*tab*userSpaceToggle.labelString:Custom Tabs
*tab*defaultSpaceToggle.labelString:Default Tabs
*tab*defaultSpacingLabel.labelString:Default Tab Space
*tab*leftToggle.labelString:Left
*tab*rightToggle.labelString:Right
*tab*centreToggle.labelString:Centre
*tab*dpToggle.labelString:Decimal
*tab*positionLabel.labelString:Tab Position
*tab*tabListSW.topOffset:5
*tab*tabListSW.leftOffset:5
*tab*tabListSW.bottomOffset:8
*tab*tabList.width:150
*tab*tabList.listSizePolicy:CONSTANT
*tab*defaultSpacingLabel.leftOffset:5
*tab*defaultSpacingLabel.topOffset:6
*tab*positionLabel.leftOffset:5
*tab*positionLabel.topOffset:6
*tab*add.leftOffset:30
*tab*add.bottomOffset:4
*tab*delete.bottomOffset:4

*loseTabs*yes.labelString:Yes
*loseTabs*yes.mnemonic:Y
*loseTabs*no.labelString:No
*loseTabs*no.mnemonic:N
*loseTabs*text.labelString:Lose user defined tab defintions?

##
## style dialog
##
*style*ok.labelString:Ok
*style*ok.mnemonic:O
*style*cancel.labelString:Cancel
*style*cancel.mnemonic:C
*style*add.labelString:Add
*style*add.mnemonic:A
*style*delete.labelString:Delete
*style*delete.mnemonic:D
*style*edit.labelString:Edit
*style*edit.mnemonic:E
*style*styleList.width:150
*style*styleList.visibleItemCount:10
*style*styleList.listSizePolicy:CONSTANT
*style*nameLabel.labelString:Name
*style*nameLabel.leftOffset:5
*style*nameLabel.topOffset:10
*style*styleNameText.topOffset:5
*style*styleNameText.rightOffset:5
*style*add.topOffset:15
*style*add.leftOffset:5
*style*delete.topOffset:15
*style*edit.topOffset:15

*tooManyStyles*confirm.labelString:Confirm
*tooManyStyles*confirm.mnemonic:C
*tooManyStyles*text.labelString:Too many styles, can't add any more.

*deleteStyle*text.labelString:Delete selected style?
*deleteStyle*yes.labelString:Yes
*deleteStyle*yes.mnemonic:Y
*deleteStyle*no.labelString:No
*deleteStyle*no.mnemonic:N

##
## date/time dialog
##
*date*ok.labelString:Ok
*date*ok.mnemonic:O
*date*cancel.labelString:Cancel
*date*cancel.mnemonic:C

##
## page setup dialog
##
*page*ok.labelString:Ok
*page*ok.mnemonic:O
*page*cancel.labelString:Cancel
*page*cancel.mnemonic:C

*page*frame1Label.labelString:Paper Selection
*page*frame2Label.labelString:Margins
*page*widthLabel.labelString:Width
*page*heightLabel.labelString:Height
*page*leftMarginLabel.labelString:Left
*page*rightMarginLabel.labelString:Right
*page*topMarginLabel.labelString:Top
*page*bottomMarginLabel.labelString:Bottom
*page*portraitToggle.labelString:Portrait
*page*landscapeToggle.labelString:Landscape
*page*applyPageToggle.labelString:Current Page
*page*applyDocumentToggle.labelString:Whole Document
*page*applyToEndToggle.labelString:Upto End Of Document
*page*frame1.leftOffset:5
*page*frame1.topOffset:5
*page*frame2.leftOffset:5
*page*frame2.topOffset:5
*page*frame2.rightOffset:5
*page*frame3.leftOffset:5
*page*frame3.rightOffset:5
*page*widthLabel.leftOffset:5
*page*widthLabel.topOffset:10
*page*heightLabel.leftOffset:5
*page*heightLabel.topOffset:10
*page*widthText.leftOffset:5
*page*widthText.rightOffset:5
*page*heightText.leftOffset:5
*page*heightText.rightOffset:5
*page*orientation.topOffset:5
*page*leftMarginLabel.leftOffset:5
*page*leftMarginLabel.topOffset:10
*page*rightMarginLabel.leftOffset:5
*page*rightMarginLabel.topOffset:13
*page*topMarginLabel.leftOffset:5
*page*topMarginLabel.topOffset:13
*page*bottomMarginLabel.leftOffset:5
*page*bottomMarginLabel.topOffset:13

*page*heightText.width:100
*page*widthText.width:100
*page*leftMarginText.width:100
*page*rightMarginText.width:100
*page*topMarginText.width:100
*page*bottomMarginText.width:100

*page*leftMarginText.leftOffset:5
*page*rightMarginText.leftOffset:5
*page*topMarginText.leftOffset:5
*page*bottomMarginText.leftOffset:5
*page*leftMarginText.rightOffset:5
*page*rightMarginText.rightOffset:5
*page*topMarginText.rightOffset:5
*page*bottomMarginText.rightOffset:5

*page*sizeListSW.width:250

##
## columns dialog
##
*columns*ok.labelString:Ok
*columns*ok.mnemonic:O
*columns*apply.labelString:Apply
*columns*apply.mnemonic:A
*columns*cancel.labelString:Cancel
*columns*cancel.mnemonic:C
*columns*numColumnsLabel.labelString:Number Of Columns
*columns*numColumnsLabel.leftOffset:5
*columns*numColumnsLabel.topOffset:15
*columns*spaceBetweenLabel.labelString:Space Between Columns
*columns*spaceBetweenLabel.leftOffset:5
*columns*spaceBetweenLabel.topOffset:20
*columns*numColumnsCombo.topOffset:5
*columns*numColumnsCombo.leftOffset:5
*columns*numColumnsCombo.width:80
*columns*numColumnsCombo*Text.translations: #override <Key>osfDelete: delete-previous-character() 
*columns*spaceBetweenText.topOffset:5
*columns*spaceBetweenText.leftOffset:5

##
## insert columns dialog
##
*insertColumns*okLeft.labelString:Insert To Left
*insertColumns*okLeft.mnemonic:L
*insertColumns*okLeft.leftOffset:20
*insertColumns*okRight.labelString:Insert To Right
*insertColumns*okRight.mnemonic:R
*insertColumns*okRight.leftOffset:20
*insertColumns*okRight.rightOffset:20
*insertColumns*cancel.labelString:Cancel
*insertColumns*cancel.mnemonic:C
*insertColumns*cancel.leftOffset:20
*insertColumns*numColumnsCombo.leftOffset:5
*insertColumns*numColumnsCombo.topOffset:5
*insertColumns*numColumnsCombo.width:80
*insertColumns*numColumnsCombo*Text.translations: #override <Key>osfDelete: delete-previous-character() 
*insertColumns*numColumnsLabel.leftOffset:5
*insertColumns*numColumnsLabel.topOffset:15
*insertColumns*numColumnsLabel.labelString:Number Of Columns To Insert

##
## insert rows dialog
##
*insertRows*okAbove.labelString:Insert Above
*insertRows*okAbove.mnemonic:A
*insertRows*okAbove.leftOffset:20
*insertRows*okBelow.labelString:Insert Below
*insertRows*okBelow.mnemonic:B
*insertRows*okBelow.leftOffset:20
*insertRows*cancel.labelString:Cancel
*insertRows*cancel.mnemonic:C
*insertRows*cancel.leftOffset:20
*insertRows*numRowsCombo.leftOffset:5
*insertRows*numRowsCombo.topOffset:5
*insertRows*numRowsCombo.width:70
*insertRows*numRowsCombo*Text.translations: #override <Key>osfDelete: delete-previous-character() 
*insertRows*numRowsLabel.leftOffset:5
*insertRows*numRowsLabel.topOffset:15
*insertRows*numRowsLabel.labelString:Number Of Rows To Insert

##
## format cells dialog
##
*formatCells*ok.labelString:Ok
*formatCells*ok.mnemonic:O
*formatCells*apply.labelString:Apply
*formatCells*apply.mnemonic:A
*formatCells*cancel.labelString:Cancel
*formatCells*cancel.mnemonic:C
*formatCells*columnWidthLabel.labelString:Column Width
*formatCells*columnWidthLabel.topOffset:13
*formatCells*columnWidthLabel.leftOffset:5
*formatCells*widthText.topOffset:5
*formatCells*widthText.leftOffset:5

##
## spelling dialog
##
*spell*quit.labelString:Quit
*spell*quit.mnemonic:Q
*spell*wordList.width:150
*spell*wordList.listSizePolicy:CONSTANT
*spell*wordLabel.labelString:Word
*spell*wordLabel.leftOffset:5
*spell*wordLabel.topOffset:15
*spell*wordText.topOffset:5
*spell*replaceLabel.labelString:Replacement
*spell*replaceLabel.leftOffset:5
*spell*replaceLabel.topOffset:12
*spell*ignore.labelString:Ignore
*spell*ignore.mnemonic:I
*spell*ignore.leftOffset:5
*spell*ignoreAll.labelString:Ignore All
*spell*ignoreAll.mnemonic:A
*spell*ignoreAll.topOffset:5
*spell*replace.labelString:Replace
*spell*replace.mnemonic:R
*spell*replace.topOffset:5
*spell*back.labelString:Back
*spell*back.mnemonic:B
*spell*back.topOffset:5
*spell*addDict.labelString:Add To Dictionary
*spell*addDict.mnemonic:A
*spell*addDict.topOffset:5
*spell*LanguagePulldown.topOffset:5

##
## language name strings
##
*opt*english.labelString:English
*opt*american.labelString:American
*opt*francais.labelString:Français
*opt*deutsch.labelString:Deutsch
*opt*espanol.labelString:Español
*opt*portugues.labelString:Português
*opt*nederlands.labelString:Nederlands
*opt*dansk.labelString:Dansk
*opt*svenska.labelString:Svenska

*spell*english.labelString:English
*spell*american.labelString:American
*spell*francais.labelString:Français
*spell*deutsch.labelString:Deutsch
*spell*espanol.labelString:Español
*spell*portugues.labelString:Português
*spell*nederlands.labelString:Nederlands
*spell*dansk.labelString:Dansk
*spell*svenska.labelString:Svenska


##
## envelope setup dialog
##
*env*ok.labelString:Ok
*env*ok.mnemonic:O
*env*cancel.labelString:Cancel
*env*cancel.mnemonic:C

*env*frame1Label.labelString:Envelope Selection
*env*frame2Label.labelString:Address
*env*widthLabel.labelString:Width
*env*heightLabel.labelString:Height
*env*portraitToggle.labelString:Portrait
*env*landscapeToggle.labelString:Landscape
*env*frame1.leftOffset:5
*env*frame1.topOffset:5
*env*frame2.leftOffset:5
*env*frame2.topOffset:5
*env*frame2.rightOffset:5
*env*addressText.leftOffset:5
*env*addressText.rightOffset:5
*env*addressText.bottomOffset:5
*env*widthLabel.leftOffset:5
*env*widthLabel.topOffset:10
*env*heightLabel.leftOffset:5
*env*heightLabel.topOffset:10
*env*widthText.leftOffset:5
*env*widthText.rightOffset:5
*env*heightText.leftOffset:5
*env*heightText.rightOffset:5
*env*orientation.topOffset:5

*env*heightText.width:100
*env*widthText.width:100

##
## options dialog
##
*opt*cancel.labelString:Cancel
*opt*cancel.mnemonic:C
*opt*ok.labelString:Ok
*opt*ok.mnemonic:O
*opt*pageLabel.labelString:Default Page Size
*opt*envelopeLabel.labelString:Default Envelope Size
*opt*unitsLabel.labelString:Display Units
*opt*printerLabel.labelString:Default Printer
*opt*languageLabel.labelString:Default Language
*opt*xresLabel.labelString:Printer X Resolution
*opt*yresLabel.labelString:Printer Y Resolution
*opt*xresLabel.leftOffset:5
*opt*yresLabel.leftOffset:5
*opt*pageLabel.leftOffset:5
*opt*pageLabel.topOffset:10
*opt*envelopeLabel.leftOffset:5
*opt*unitsLabel.leftOffset:5
*opt*printerLabel.leftOffset:5
*opt*languageLabel.leftOffset:5
*opt*envelopeLabel.topOffset:17
*opt*unitsLabel.topOffset:17
*opt*printerLabel.topOffset:17
*opt*languageLabel.topOffset:17
*opt*xresLabel.topOffset:17
*opt*yresLabel.topOffset:17
*opt*xresText.leftOffset:10
*opt*yresText.leftOffset:10

##
## support info dialog
##
*support*ok.labelString:Ok
*support*ok.mnemonic:O
*support*infoText.height:200
*support*infoText.width:500
*support*infoText.leftOffset:5
*support*infoText.topOffset:5
*support*infoText.rightOffset:5
*support*infoText.fontList:-urw-nimbus mono l-regular-r-normal-*-14-*-*-*-*-*-*-1

##
## help search dialog
##
*hsearch*goto.labelString:Goto
*hsearch*goto.mnemonic:G
*hsearch*cancel.labelString:Cancel
*hsearch*cancel.mnemonic:C
*hsearch*search.labelString:Search
*hsearch*search.mnemonic:S
*hsearch*search.leftOffset:7
*hsearch*search.topOffset:12
*hsearch*search.rightOffset:5
*hsearch*findLabel.leftOffset:5
*hsearch*findLabel.topOffset:17
*hsearch*findLabel.labelString:Search for help on
*hsearch*topicList.visibleItemCount:10
*hsearch*combo.width:160
*hsearch*combo.topOffset:5

##
## ispell failure to start
##
*ispellStartFail*confirm.labelString:Confirm
*ispellStartFail*confirm.mnemonic:C
*ispellStartFail*text.labelString:Failed to start ispell

##
## ispell failure to reply
##
*ispellFail*confirm.labelString:Confirm
*ispellFail*confirm.mnemonic:C
*ispellFail*text.labelString:ispell has failed to respond, will attempt to restart it

##
## progress dialog
##
*progress*cancel.labelString:Cancel
*progress*cancel.mnemonic:C

##
## bad number of copies in print dialog
##
*badCopies*confirm.labelString:Confirm
*badCopies*confirm.mnemonic:C
*badCopies*text.labelString:Invalid number of copies

##
## bad first page number in print dialog
##
*badFirstPage*confirm.labelString:Confirm
*badFirstPage*confirm.mnemonic:C
*badFirstPage*text.labelString:Invalid first page number

##
## bad last page number in print dialog
##
*badLastPage*confirm.labelString:Confirm
*badLastPage*confirm.mnemonic:C
*badLastPage*text.labelString:Invalid last page number

##
## bad x resolution in print dialog
##
*badXres*confirm.labelString:Confirm
*badXres*confirm.mnemonic:C
*badXres*text.labelString:Invalid x resolution 

##
## bad y resolution in print dialog
##
*badYres*confirm.labelString:Confirm
*badYres*confirm.mnemonic:C
*badYres*text.labelString:Invalid y resolution

##
## bad page number in goto dialog
##
*badPageNumber*confirm.labelString:Confirm
*badPageNumber*confirm.mnemonic:C
*badPageNumber*text.labelString:Invalid page number

##
## bad number in insert columns dialog
##
*badInsColumns*confirm.labelString:Confirm
*badInsColumns*confirm.mnemonic:C
*badInsColumns*text.labelString:Invalid number of columns

##
## bad number in insert rows dialog
##
*badInsRows*confirm.labelString:Confirm
*badInsRows*confirm.mnemonic:C
*badInsRows*text.labelString:Invalid number of rows

##
## bad number in zoom dialog
##
*badZoomNumber*confirm.labelString:Confirm
*badZoomNumber*confirm.mnemonic:C
*badZoomNumber*text.labelString:Invalid zoom value

##
## bad width in page setup
##
*invalidPageWidth*confirm.labelString:Confirm
*invalidPageWidth*confirm.mnemonic:C
*invalidPageWidth*text.labelString:Invalid width

##
## bad height in page setup
##
*invalidPageHeight*confirm.labelString:Confirm
*invalidPageHeight*confirm.mnemonic:C
*invalidPageHeight*text.labelString:Invalid height

##
## bad left margin in page setup
##
*invalidPageLeftMargin*confirm.labelString:Confirm
*invalidPageLeftMargin*confirm.mnemonic:C
*invalidPageLeftMargin*text.labelString:Invalid left margin

##
## bad right margin in page setup
##
*invalidPageRightMargin*confirm.labelString:Confirm
*invalidPageRightMargin*confirm.mnemonic:C
*invalidPageRightMargin*text.labelString:Invalid right margin

##
## bad top margin in page setup
##
*invalidPageTopMargin*confirm.labelString:Confirm
*invalidPageTopMargin*confirm.mnemonic:C
*invalidPageTopMargin*text.labelString:Invalid top margin

##
## bad top margin in page setup
##
*invalidPageBottomMargin*confirm.labelString:Confirm
*invalidPageBottomMargin*confirm.mnemonic:C
*invalidPageBottomMargin*text.labelString:Invalid bottom margin

##
## bad space between in format columns
##
*invalidSpaceBetween*confirm.labelString:Confirm
*invalidSpaceBetween*confirm.mnemonic:C
*invalidSpaceBetween*text.labelString:Invalid space between columns

##
## bad number of columns format columns
##
*invalidNumColumns*confirm.labelString:Confirm
*invalidNumColumns*confirm.mnemonic:C
*invalidNumColumns*text.labelString:Invalid number of columns

##
## already locked dialog
##
*locked*confirm.labelString:Confirm
*locked*confirm.mnemonic:C
*locked*pixmap.labelType:PIXMAP
*locked*pixmap.labelPixmap:mx_error.xpm
*locked*pixmap.topOffset:20
*locked*pixmap.leftOffset:10
*locked*text1.labelString:The file you are trying to open is currently locked by another user
*locked*text1.topOffset:10
*locked*text2.labelString:File name:
*locked*text3.labelString:User:
*locked*text4.labelString:Host:

##
## open already
##
*openAlready*confirm.labelString:Confirm
*openAlready*confirm.mnemonic:C
*openAlready*text.labelString:You already have the document open

##
## error dialog
##
*error*label.labelString:An Internal Error Has Occurred
*error*label.leftOffset:10
*error*label.rightOffset:10
*error*label.topOffset:10
*error*label1.labelString:Error Text:
*error*label1.leftOffset:5
*error*label1.topOffset:5
*error*label2.labelString:Error Code:
*error*label2.leftOffset:5
*error*label2.topOffset:5
*error*continue.mnemonic:C
*error*continue.labelString:Continue
*error*exit.mnemonic:E
*error*exit.labelString:Exit Maxwell
*error*errorText.leftOffset:5
*error*errorText.topOffset:5
*error*errorCodeText.leftOffset:5
*error*errorCodeText.topOffset:5

##
## file damaged repair dialog
##
*damaged*text.labelString: The file is damaged, try to recover?
*damaged*yes.labelString:Yes
*damaged*yes.mnemonic:Y
*damaged*no.labelString:No
*damaged*no.mnemonic:N

##
## file broken dialog
##
*fileUnrecoverable*confirm.labelString:Confirm
*fileUnrecoverable*confirm.mnemonic:C
*fileUnrecoverable*text.labelString:The file is broken beyond repair. Sorry.

##
## unimportable file type
##
*cantImportFileType*confirm.labelString:Confirm
*cantImportFileType*confirm.mnemonic:C
*cantImportFileType*text.labelString:Can't import this type of file. Sorry.

##
## image file selected for text import
##
*tryInsertImage*confirm.labelString:Confirm
*tryInsertImage*confirm.mnemonic:C
*tryInsertImage*text.labelString:Select insert->image to insert this file.

##
## error during import
##
*errorDuringImport*confirm.labelString:Confirm
*errorDuringImport*confirm.mnemonic:C
*errorDuringImport*text.labelString:The imported file was corrupted or incomplete.

##
## file type unrecognized dialog
##
*unknownType*confirm.labelString:Confirm
*unknownType*confirm.mnemonic:C
*unknownType*text.labelString:Maxwell does not recognize the type of file you are trying to open

##
## cant paste that there
##
*cantPasteThatThere*confirm.labelString:Confirm
*cantPasteThatThere*confirm.mnemonic:C
*cantPasteThatThere*text.labelString:Can't paste current clipboard into current position.

##
## table not big enough
##
*tableNotBigEnough*confirm.labelString:Confirm
*tableNotBigEnough*confirm.mnemonic:C
*tableNotBigEnough*text.labelString:The table doesn't have enough cells to hold the selection

##
## no text selected warning
##
*noTextSelected*confirm.labelString:Confirm
*noTextSelected*confirm.mnemonic:C
*noTextSelected*text.labelString:No text selected

##
## all text searched/checked etc
##
*allChecked*confirm.labelString:Confirm
*allChecked*confirm.mnemonic:C
*allChecked*text.labelString:All text searched

##
## continue from the beginning of the document?
##
*startFromTop*yes.labelString:Yes
*startFromTop*yes.mnemonic:Y
*startFromTop*no.labelString:No
*startFromTop*no.mnemonic:N
*startFromTop*text.labelString:Reached the end of the document, continue from start?

##
## continue from the end of the document?
##
*startFromBottom*yes.labelString:Yes
*startFromBottom*yes.mnemonic:Y
*startFromBottom*no.labelString:No
*startFromBottom*no.mnemonic:N
*startFromBottom*text.labelString:Reached the start of the document, continue from end?

##
## replace exisiting TOC?
##
*replaceExistingTOC*yes.labelString:Yes
*replaceExistingTOC*yes.mnemonic:Y
*replaceExistingTOC*no.labelString:No
*replaceExistingTOC*no.mnemonic:N
*replaceExistingTOC*text.labelString:Replace exisiting table of contents?

##
## no heading paragraphs
##
*noHeadingParas*confirm.labelString:Confirm
*noHeadingParas*confirm.mnemonic:C
*noHeadingParas*text.labelString:No heading paragraphs found

##
## table paragraph style dialog
##
*tabf*ok.labelString:Ok
*tabf*ok.mnemonic:O
*tabf*apply.labelString:Apply
*tabf*apply.mnemonic:A
*tabf*cancel.labelString:Cancel
*tabf*cancel.mnemonic:C
*tabf*frame1.leftOffset:5
*tabf*frame1.topOffset:5
*tabf*frame1.rightOffset:5
*tabf*frame1.bottomOffset:5
*tabf*frame2.leftOffset:5
*tabf*frame2.topOffset:5
*tabf*frame2.rightOffset:5
*tabf*frame2.bottomOffset:5
*tabf*form1Labels.leftOffset:5
*tabf*justifyLabel.labelString:Justification
*tabf*justifyLabel.topOffset:13
*tabf*keepLabel.labelString:Keep
*tabf*keepLabel.topOffset:18
*tabf*spaceBeforeLabel.labelString:Space Before
*tabf*spaceBeforeLabel.topOffset:27
*tabf*spaceAfterLabel.labelString:Space After
*tabf*spaceAfterLabel.topOffset:15
*tabf*spaceAfterText.leftOffset:5
*tabf*spaceBeforeText.topOffset:5
*tabf*spaceBeforeText.leftOffset:5
*tabf*leftIndentLabel.labelString:Left Indent
*tabf*leftIndentLabel.topOffset:20
*tabf*rightIndentLabel.labelString:Right Indent
*tabf*rightIndentLabel.topOffset:15
*tabf*leftIndentText.topOffset:5
*tabf*leftIndentText.leftOffset:5
*tabf*rightIndentText.leftOffset:5
*tabf*firstLineIndentText.leftOffset:5
*tabf*breakBefore.labelString:Page Break Before
*tabf*breakBefore.topOffset:5

*tabf*left.labelType:PIXMAP
*tabf*left.labelPixmap:mx_left_justify_lg.xpm
*tabf*right.labelType:PIXMAP
*tabf*right.labelPixmap:mx_right_justify_lg.xpm
*tabf*full.labelType:PIXMAP
*tabf*full.labelPixmap:mx_justify_lg.xpm
*tabf*centre.labelType:PIXMAP
*tabf*centre.labelPixmap:mx_centre_justify_lg.xpm
*tabf*together.labelString:Together
*tabf*withNext.labelString:With Next
*tabf*withPrev.labelString:With Previous)");

    fclose(f);
}
