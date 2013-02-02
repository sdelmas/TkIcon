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

# initialize global variables
set layouter matrix
set nodeCounter 0
set edgeCounter 0
set graphorder 0
set edgeorder 0

# create the widget structure
frame .menubar \
  -borderwidth 1 \
  -relief raised
pack .menubar -side top -fill both

menubutton .menubar.file \
  -text File \
  -underline 0 \
  -menu .menubar.file.m
pack .menubar.file -side left -padx 4 -pady 4

menu .menubar.file.m
.menubar.file.m add command \
  -command {
    [tkIconPane::info iconpane] layout} \
  -label {Relayout}
.menubar.file.m add separator
.menubar.file.m add command \
  -command {
    SaveIcons [tkIconPane::info iconpane]} \
  -label {Save to layout.inf}
.menubar.file.m add command \
  -command {
    [tkIconPane::info canvas] postscript -colormode mono -file test1.ps
    [tkIconPane::info canvas] postscript -colormode gray -file test2.ps
    [tkIconPane::info canvas] postscript -colormode color -file test3.ps} \
  -label {Save as PS}
.menubar.file.m add separator
.menubar.file.m add command \
  -command {destroy .} \
  -label {Quit}

menubutton .menubar.help \
  -text Help \
  -underline 0
pack .menubar.help -side right -padx 4 -pady 4

tixPopupMenu .vmenu-layoutmethods \
  -title "Layout Methods"

tkIconPane .layouter \
  -toplevel 0 \
  -tkiconlib ".:$libraryPath/menus:$libraryPath/icons:$libraryPath/status" \
  -menu1 layouter \
  -menu2 .vmenu-layoutmethods \
  -layout 0 \
  -menupath "$libraryPath/menus"
pack .layouter -side top -fill both -expand 1

# load the various layout methods and set the current layouter
foreach fileName [glob -nocomplain $layoutPath/*.lay] {
  source $fileName
}

# create the layout metyhod menu
foreach layouter [lsort [info commands LAYOUT_*]] {
  set popup [.vmenu-layoutmethods subwidget menu]
  $popup add radiobutton \
    -command "
      .layouter config -layoutcommand $layouter
      .layouter layout" \
    -label [string range $layouter 7 end] \
    -value [string range $layouter 7 end] \
    -variable layouter
}

if {$argc > 0} {
  if {[file exists $argv]} {
    source [lindex $argv 0]
  }
}
set layouter matrix
.layouter config -layoutcommand $layouter
.layouter layout

proc SaveIcons {pathName} {
  global nodeCounter
  global edgeCounter
  global layouter

  set canvasPath [$pathName subwidget canvas]
  set fd [open layout.inf w]

  puts $fd "global nodeCounter"
  puts $fd "global edgeCounter"
  puts $fd "global layouter"
  puts $fd "set nodeCounter [set nodeCounter]"
  puts $fd "set edgeCounter [set edgeCounter]"
  puts $fd "set layouter [set layouter]\n"

  foreach element [$pathName iconinfo ids] {
    puts $fd "$pathName iconcreate [lindex [$canvasPath itemconfig $element -name] 4] -menu1 layouter -status none -text [lindex [$canvasPath itemconfig $element -name] 4] -name [lindex [$canvasPath itemconfig $element -name] 4] -image standard"
  }
  foreach element [$pathName edgeinfo ids] {
    puts $fd "$pathName edgecreate \[list \[.layouter iconinfo id [lindex [$pathName edgeinfo fromtoname mygraph $element] 0]\] \[.layouter iconinfo id [lindex [$pathName edgeinfo fromtoname mygraph $element] 1]\] 1 -menu1 layouter -label [lindex [$canvasPath itemconfig $element -name] 4] -name [lindex [$canvasPath itemconfig $element -name] 4]\]"
  }

  close $fd
}

