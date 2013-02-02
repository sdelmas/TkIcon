#!/usr/local/bin/icon-wish -f

# set the toplevel to an appropriate size
wm geometry . 400x400

# prepare autoloading
global auto_path
set libraryPath ""
set layoutPath ""
set searchList ""
foreach dirName $auto_path {
  lappend searchList $dirName
}
lappend searchList "../library"
lappend searchList ".."
lappend searchList "."
foreach dirName $searchList {
  if {[file exists $dirName/tkIconPane.tcl]} {
    set libraryPath $dirName
  } {
    if {[file exists $dirName/TkIcon/tkIconPane.tcl]} {
      set libraryPath $dirName/TkIcon
    } {
      if {[file exists [file dirname $dirName]/TkIcon/tkIconPane.tcl]} {
        set libraryPath [file dirname $dirName]/TkIcon
      }
    }
  }
  if {[file exists $dirName/tree.lay]} {
    set layoutPath $dirName
  } {
    if {[file exists $dirName/TkIcon/tree.lay]} {
      set layoutPath $dirName/TkIcon
    } {
      if {[file exists [file dirname $dirName]/TkIcon/tree.lay]} {
        set layoutPath [file dirname $dirName]/TkIcon
      }
    }
  }
}
lappend auto_path $libraryPath
source $libraryPath/tkIconPane.tcl

tkIconPane .u \
  -toplevel 1 \
  -dropcommand {dropCommand} \
  -tkiconlib "$libraryPath/menus:$libraryPath/icons:$libraryPath/status" \
  -layout 0 \
  -layoutcommand {random} \
  -menupath "$libraryPath/menus" \
  -menu1 standard \
  -canvaswidth 200 \
  -canvasheight 200

# eof
