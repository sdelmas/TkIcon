#!/usr/local/bin/icon-wish -f

global auto_path
set libraryPath ""
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
}
lappend auto_path $libraryPath
source $libraryPath/tkIconPane.tcl

image create bitmap test -file test.bmp
image create bitmap none -file none.bmp
image create bitmap standard -file standard.bmp

proc demoEval {command} {
  .t.t delete 1.0 end
  .t.t insert end "[string trim $command]\n"
  .t.t insert end "--------------------------->>>>>>\n"
  .t.t insert end [string trim [eval $command]]
}

proc selectCommand {args} {
  puts stdout "selected: $args"
}

proc toolbarDropCommand {interp sourcePathName sourceIconId mode targetPathName} {
  puts stdout "$interp $sourcePathName $sourceIconId $mode $targetPathName"
}

proc dropCommand {interp sourcePathName sourceIconId mode targetPathName targetIconId} {
  if {"$targetIconId" != ""} {
    puts stdout "I received an icon from: $interp $sourcePathName $sourceIconId ${mode}to $targetPathName $targetIconId"
  } {
    if {"$sourcePathName" != "$targetPathName"} {
      puts stdout "I received an icon from: $interp $sourcePathName $sourceIconId ${mode}to $targetPathName $targetIconId"
    } {
      set tmpPos [$sourcePathName dropposition]
      $sourcePathName iconconfig $sourceIconId -x [lindex $tmpPos 0] -y [lindex $tmpPos 1]
      catch "$sourcePathName layout edges" res
    }
  }
}

proc DAGLayout {pathName args} {
  $pathName-graph layout isi -graphorder 0 -iconspaceh 10 -iconspacev 10 -keeprandompositions 1 -xoffset 30 -yoffset 70
}

proc iconSort {canvasPath idList} {
  set iconList ""
  set nameCounter 0
  foreach counter $idList {
    set tmpName [lindex [$canvasPath itemconfig $counter -name] 4]
    if {![catch "set tmpNameList($tmpName)"]} {
      set tmpName [lindex [$canvasPath itemconfig $counter -text] 4]
      if {![catch "set tmpNameList($tmpName)"]} {
        while {![catch "set tmpNameList($tmpName$nameCounter)"]} {
          incr nameCounter
        }
        set tmpName $tmpName$nameCounter
      }
    }
    set tmpNameList($tmpName) $counter
  }
  if {[info exists tmpNameList]} {
    foreach counter [lsort [array names tmpNameList]] {
      lappend iconList [set tmpNameList($counter)]
    }
  }
  return $iconList
}

menu .root
.root add command -label {Relayout} -command {
  [tkIconPane::info iconpane] layout}
.root add command -label {Fit} -command {
  [tkIconPane::info iconpane] config -scale -1}
.root add command -label {Scale 1} -command {
  [tkIconPane::info iconpane] config -scale 0.3}
.root add command -label {Scale 2} -command {
  [tkIconPane::info iconpane] config -scale 0.6}
.root add command -label {Scale 3} -command {
  [tkIconPane::info iconpane] config -scale 1.0}
.root add command -label {Scale 4} -command {
  [tkIconPane::info iconpane] config -scale 2.2}
.root add command -label {Scale 5} -command {
  [tkIconPane::info iconpane] config -scale 3.3}

menu .edge
.edge add command -label {Edge handling}

wm maxsize . 1000 1000

tkIconPane .v -toplevel 0 \
  -layoutupdate 1\
  -dropcommand {dropCommand} \
  -tkiconlib ".:$libraryPath/menus:$libraryPath/icons:$libraryPath/status" \
  -layout 0 \
  -menupath "$libraryPath/menus"
wm geometry . 300x300

tkIconPane .u -toplevel 1 \
  -dropcommand {dropCommand} \
  -tkiconlib ".:$libraryPath/menus:$libraryPath/icons:$libraryPath/status" \
  -layout 0 \
  -layoutcommand {random} \
  -menu1 standard \
  -menupath "$libraryPath/menus"
wm geometry .u 300x300

tkIconPane .w -toplevel 1 \
  -dropcommand {dropCommand} \
  -tkiconlib ".:$libraryPath/menus:$libraryPath/icons:$libraryPath/status" \
  -layout 0 \
  -layoutcommand {matrix -computeiconsize 1 -iconspaceh 10 -iconspacev 10 -xoffset 30 -yoffset 70} \
  -menu1 .root \
  -menu2 .root \
  -menu3 .root \
  -menupath "$libraryPath/menus" \
  -placing 1 \
  -sortcommand iconSort
wm geometry .w 300x300

tkIconPane .x -toplevel 1 \
  -computeiconsize 0 \
  -dropcommand {dropCommand} \
  -tkiconlib ".:$libraryPath/menus:$libraryPath/icons:$libraryPath/status" \
  -iconheight 50 \
  -iconspaceh 5 \
  -iconspacev 5 \
  -iconwidth 50 \
  -layout 0 \
  -menupath "$libraryPath/menus" \
  -placing 0
wm geometry .x 300x300

tkIconPane .y -toplevel 1 \
  -dropcommand {dropCommand} \
  -tkiconlib ".:$libraryPath/menus:$libraryPath/icons:$libraryPath/status" \
  -layout 0 \
  -layoutcommand {isi} \
  -menu1 standard \
  -menu2 .root \
  -menupath "$libraryPath/menus" \
  -placing 1 \
  -selectcommand selectCommand
wm geometry .y 300x300

foreach counter [glob * -nocomplain] {
  .u iconcreate $counter \
    -borderwidth 1 \
    -command "puts stdout " \
    -dropcommand dropCommand \
    -status none \
    -text $counter \
    -image test
}
.u layout

foreach counter [glob * -nocomplain] {
  .w iconcreate $counter \
    -borderwidth 1 \
    -command "puts stdout " \
    -dropcommand dropCommand \
    -menu1 [string range [file extension $counter] 1 end] \
    -status none \
    -text $counter \
    -image standard
}
.w layout

foreach counter [glob * -nocomplain] {
  .x iconcreate $counter \
    -borderwidth 1 \
    -command "puts stdout " \
    -dropcommand dropCommand \
    -status none \
    -text $counter \
    -image [string range [file extension $counter] 1 end]
}
.x layout

foreach counter [glob -nocomplain /usr/local/bin/*] {
  .v iconcreate $counter \
    -borderwidth 2 \
    -command "puts stdout " \
    -dropcommand dropCommand \
    -relief raised \
    -status none \
    -text $counter \
    -image standard
}
.v layout

set oldCounter ""
foreach counter {{node1[1.2]} node2 node3 node4 node5 longnode6 longnode7 verylongnode8 node9 node10 node11 node13 node14 node15 node16} {
  set item [.y iconcreate $counter \
    -command "puts stdout " \
    -menu1 c \
    -status none \
    -name $counter \
    -text $counter]
}
foreach counter {{1 2} {1 5} {6 3} {1 8} {2 4} {8 6} {4 11} {2 5} {8 9} {7 8} {1 7} {1 10}} {
  .y edgecreate [list [lindex $counter 0] [lindex $counter 1] 0 -fill red -arrow last -label {sabber[1.3]} -menu1 standard -menu2 .edge -name sabber$counter]
}

foreach counter {{12 13} {12 14} {14 15} {12 8}} {
  .y edgecreate [list [lindex $counter 0] [lindex $counter 1] 0 -fill blue -arrow last -label {sabber[1.3]} -menu1 standard -menu2 .edge -name sabber$counter]
}

.y layout
.w config -layoutcommand {nolayout}

pack .v -side top -fill both -expand 1

toplevel .t

wm maxsize .t 1000 1000

frame .t.f -borderwidth 2 -relief raised

frame .t.f.f1 -borderwidth 2 -relief flat

frame .t.f.f2 -borderwidth 2 -relief flat

frame .t.f.f3 -borderwidth 2 -relief flat

frame .t.f.f4 -borderwidth 2 -relief flat

frame .t.f.f5 -borderwidth 2 -relief flat

button .t.f.f1.b1 \
  -text {All icon ids in VIEW3} \
  -command {demoEval {.y iconinfo ids}}

button .t.f.f1.b2 \
  -text {All icon names in VIEW3} \
  -command {demoEval {.y iconinfo names}}

button .t.f.f1.b3 \
  -text {All graphs in VIEW3} \
  -command {demoEval {.y edgeinfo graphs}}

button .t.f.f1.b4 \
  -text {All graph edge ids in VIEW3} \
  -command {demoEval {.y edgeinfo ids}}

button .t.f.f1.b5 \
  -text {All graph edge names in VIEW3} \
  -command {demoEval {.y edgeinfo names}}

button .t.f.f1.b6 \
  -text {All graph connection ids in VIEW3} \
  -command {demoEval {.y edgeinfo fromtoids}}

button .t.f.f1.b7 \
  -text {All graph connection names in VIEW3} \
  -command {demoEval {.y edgeinfo fromtonames}}

button .t.f.f2.b1 \
  -text {Current pane} \
  -command {demoEval {tkIconPane::info iconpane}}

button .t.f.f2.b2 \
  -text {Current pane canvas} \
  -command {demoEval {tkIconPane::info canvas}}

button .t.f.f2.b3 \
  -text {Current pane current icon name} \
  -command {demoEval {tkIconPane::info iconinfo name}}

button .t.f.f2.b4 \
  -text {Current pane selected icon ids} \
  -command {demoEval {tkIconPane::info iconinfo selectedids}}

button .t.f.f2.b5 \
  -text {Current pane selected icon names} \
  -command {demoEval {tkIconPane::info iconinfo selectednames}}

button .t.f.f2.b6 \
  -text {Current pane current icon state} \
  -command {demoEval {tkIconPane::info iconinfo state}}

button .t.f.f2.b7 \
  -text {Current pane current icon status} \
  -command {demoEval {tkIconPane::info iconinfo status}}

button .t.f.f2.b8 \
  -text {Current pane current icon tags} \
  -command {demoEval {tkIconPane::info iconinfo tags}}

button .t.f.f2.b9 \
  -text {Current edge current edge id} \
  -command {demoEval {tkIconPane::info edgeinfo id}}

button .t.f.f2.b10 \
  -text {Current edge current edge name} \
  -command {demoEval {tkIconPane::info edgeinfo name}}

button .t.f.f2.b11 \
  -text {Current edge current edge fromto id} \
  -command {demoEval {tkIconPane::info edgeinfo fromtoid}}

button .t.f.f2.b12 \
  -text {Current edge current edge fromto name} \
  -command {demoEval {tkIconPane::info edgeinfo fromtoname}}

button .t.f.f3.b1 \
  -text {Layout} \
  -command {demoEval {.y layout}}

button .t.f.f3.b2 \
  -text {Configure all} \
  -command {demoEval {.y config}}

button .t.f.f3.b3 \
  -text {Configure get} \
  -command {demoEval {.y config -background}}

button .t.f.f3.b4 \
  -text {Configure set} \
  -command {demoEval {.y config -background blue
.y config -canvasbg white
.y config -cursor arrow
.y config -font *times*
.y config -foreground red
.y config -computeiconsize 0
.y config -iconheight 50
.y config -iconwidth 50
.y config -iconspaceh 10
.y config -iconspacev 10
.y config -scrollbaractivebg magenta
.y config -scrollbarbg green
.y config -scrollbartrough white}}

button .t.f.f4.b1 \
  -text {Create icon} \
  -command {demoEval {.y iconcreate sabber -text sabber -image test}
.y layout}

button .t.f.f4.b2 \
  -text {Create edge} \
  -command {demoEval {.y edgecreate [list [.y iconinfo id sabber] [.y iconinfo id node5] 2]}}

button .t.f.f4.b3 \
  -text {Edge longnode6-node3} \
  -command {demoEval {.y edgeinfo id [.y iconinfo id node2] [.y iconinfo id node4]}}

button .t.f.f4.b4 \
  -text {Configure graph} \
  -command {demoEval {.y edgeconf -fill blue}}

button .t.f.f4.b5 \
  -text {Configure edge all} \
  -command {demoEval {.y edgeconf [.y edgeinfo id [.y iconinfo id node2] [.y iconinfo id node4]]}}

button .t.f.f4.b6 \
  -text {Configure edge get} \
  -command {demoEval {.y edgeconf [.y edgeinfo id [.y iconinfo id node2] [.y iconinfo id node4]] -fill}}

button .t.f.f4.b7 \
  -text {Configure edge set} \
  -command {demoEval {.y edgeconf [.y edgeinfo id [.y iconinfo id node2] [.y iconinfo id node4]] -fill green}}

button .t.f.f4.b8 \
  -text {Configure icon status} \
  -command {demoEval {.y iconconf [.y iconinfo id sabber] -status none}}

button .t.f.f4.b9 \
  -text {Delete icon} \
  -command {demoEval {.y icondelete [.y iconinfo id node10]}}

button .t.f.f4.b10 \
  -text {Delete edge} \
  -command {demoEval {.y edgedelete [.y iconinfo id verylongnode8] [.y iconinfo id node9]}}

button .t.f.f4.b11 \
  -text {Delete graph} \
  -command {demoEval {.y edgedelete}}

button .t.f.f4.b12 \
  -text {Set icon state} \
  -command {demoEval {.y iconinfo statedeselect}}

button .t.f.f4.b13 \
  -text {Toggle icon state} \
  -command {demoEval {.y iconinfo statetoggle [.y iconinfo id longnode7]}}

button .t.f.f5.b1 \
  -text {Set icon bitmap} \
  -command {demoEval {.y iconconfig [.y iconinfo id node2] -image tcl}
.y layout}

button .t.f.f5.b2 \
  -text {Layout graph} \
  -command {demoEval {.y.tkcanvas-graph layout isi}}

button .t.f.f5.b3 \
  -text {Create broken edge} \
  -command {
set coords [.y.tkcanvas coords [.y edgeinfo id [.y iconinfo id node2] [.y iconinfo id node4]]]
set x1 [lindex $coords 0]
set x2 [expr [lindex $coords 0]+40]
set x3 [lindex $coords 2]
set y1 [lindex $coords 1]
set y2 [expr [lindex $coords 1]+40]
set y3 [lindex $coords 3]
demoEval ".y.tkcanvas coords \[.y edgeinfo id \[.y iconinfo id node2\] \[.y iconinfo id node4\]\] $x1 $y1 $x2 $y2 $x3 $y3"}

button .t.f.f5.b4 \
  -text {Destroy .v} \
  -command {destroy .v}

button .t.f.f5.b5 \
  -text {Destroy all} \
  -command {destroy .u; destroy .y; destroy .x; destroy .w}

button .t.f.f5.b6 \
  -text {X activity} \
  -command {puts stdout [winfo width .]; puts stdout [info commands]}

text .t.t -borderwidth 2 -relief raised -height 10

pack .t.f.f1.b1 -side top -fill both
pack .t.f.f1.b2 -side top -fill both
pack .t.f.f1.b3 -side top -fill both
pack .t.f.f1.b4 -side top -fill both
pack .t.f.f1.b5 -side top -fill both
pack .t.f.f1.b6 -side top -fill both
pack .t.f.f1.b7 -side top -fill both

pack .t.f.f2.b1 -side top -fill both
pack .t.f.f2.b2 -side top -fill both
pack .t.f.f2.b3 -side top -fill both
pack .t.f.f2.b4 -side top -fill both
pack .t.f.f2.b5 -side top -fill both
pack .t.f.f2.b6 -side top -fill both
pack .t.f.f2.b7 -side top -fill both
pack .t.f.f2.b8 -side top -fill both
pack .t.f.f2.b9 -side top -fill both
pack .t.f.f2.b10 -side top -fill both
pack .t.f.f2.b11 -side top -fill both
pack .t.f.f2.b12 -side top -fill both

pack .t.f.f3.b1 -side top -fill both
pack .t.f.f3.b2 -side top -fill both
pack .t.f.f3.b3 -side top -fill both
pack .t.f.f3.b4 -side top -fill both

pack .t.f.f4.b1 -side top -fill both
pack .t.f.f4.b2 -side top -fill both
pack .t.f.f4.b3 -side top -fill both
pack .t.f.f4.b4 -side top -fill both
pack .t.f.f4.b5 -side top -fill both
pack .t.f.f4.b6 -side top -fill both
pack .t.f.f4.b7 -side top -fill both
pack .t.f.f4.b8 -side top -fill both
pack .t.f.f4.b9 -side top -fill both
pack .t.f.f4.b10 -side top -fill both
pack .t.f.f4.b11 -side top -fill both
pack .t.f.f4.b12 -side top -fill both
pack .t.f.f4.b13 -side top -fill both

pack .t.f.f5.b1 -side top -fill both
pack .t.f.f5.b2 -side top -fill both
pack .t.f.f5.b3 -side top -fill both
pack .t.f.f5.b4 -side top -fill both
pack .t.f.f5.b5 -side top -fill both
pack .t.f.f5.b6 -side top -fill both
 
pack .t.f.f1 -side left -fill both -expand 1
pack .t.f.f2 -side left -fill both -expand 1
pack .t.f.f3 -side left -fill both -expand 1
pack .t.f.f4 -side left -fill both -expand 1
pack .t.f.f5 -side left -fill both -expand 1

pack .t.f -side top -fill both
pack .t.t -side top -fill both

# eof
