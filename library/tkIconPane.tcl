# CB_MODULE_DESCR_START tkIconPane
# Color selection.
#   options:
#     -binddragendcopy - the event that ends a drag
#     -binddragendmove - the event that ends a drag
#     -binddragmovecopy - the event that performs a drag
#     -binddragmovemove - the event that performs a drag
#     -binddragstartcopy - the event that starts a copy drag
#     -binddragstartmove - the event that starts a move drag
#     -canvasheight - the canvas height
#     -canvaswidth - the canvas width
#     -clickinterval - the time between two clicks
#     -computeiconsize - compute the icon size
#     -dropcommand - the iconpane drop command
#     -edgefont - the font used for edges
#     -edgeorder - the direction of the edges
#     -graphorder - the direction of the graph
#     -iconfont - the font used for icons
#     -iconheight - the standard icon height
#     -iconspaceh - horizontal icon space
#     -iconspacev - vertical icon space
#     -iconwidth  the standard icon width
#     -layout - automatic layouting
#     -layoutcommand - an alternative layout command
#     -layoutupdate - update layout on widget resize
#     -menu1 - the iconpane popupmenu no 1
#     -menu2 - the iconpane popupmenu no 2
#     -menu3 - the iconpane popupmenu no 3
#     -menupath - the icon pane menu path
#     -placing - placing of icons with the default drop command
#     -selectcommand - an alternative selection command
#     -selectmode - allow multiple selections
#     -sortcommand - an alternative icon sorting command
#     -tkiconlib - the icon library path
#     -toplevel - create a toplevel dialog
#     
#     -background - standard backgound color
#     -borderwidth - standard borderwidth for the outer frame
#     -buttonactivebg - active background color of buttons
#     -buttonactivefg - active foreground color of buttons
#     -canvasbg - the canvas background
#     -font - standard font
#     -foreground - standard foregound color
#     -scrollbaractivebg - active background color of scrollbars
#     -scrollbarbg - background color of scrollbars
#     -scrollbartrough - trough color of scrollbars
#
#   methods:
#     canvas - perform action with the canvas
#     config - standard widget configuration
#     dropposition - the dropposition
#     edgeconfig - configure an edge
#     edgecreate - create an edge
#     edgedelete - delete an edge
#     edgeinfo - retrieve info about an edge
#     iconconfig - configure an icon
#     iconcreate - create an icon
#     icondelete - delete an icon
#     iconinfo - retrieve info about an icon
#     layout - layout the graph
#
# CB_MODULE_DESCR_END tkIconPane

##################################
# the following procs are "global"
##################################

if {0} {
proc tkIconPane {w args} {
  # CBInfo: autoload
}
}

proc tkIconPane::info {{type "path"} args} {
  global tkIconPane

  set w $tkIconPane(currentPane)
  upvar #0 $w data
  case $type {
    {canvas} {
      if {[llength $args] == 0} {
        return $data(w:canvas)
      } {
        return [eval $w canvas $args]
     }
    }
    {iconpane} {
      if {[llength $args] == 0} {
        return $w
      } {
        return [eval $w $args]
     }
    }
    {edgeinfo} {
      return [$w edgeinfo $args]
    }
    {iconinfo} {
      return [$w iconinfo $args]
    }
    {default} {
      return $w
    }
  }
  return ""
}

#################################
# here begins the "local" section
#################################

if {[catch {tixWidgetClass tkIconPane { \
  -superclass tixScrolledWidget \
  -classname  TkIconPane \
  -method { \
    dropposition edgeconfig edgecreate edgedelete edgeinfo \
    iconconfig iconcreate icondelete iconinfo layout \
  } \
  -flag { \
    -binddragendcopy -binddragendmove -binddragmovecopy -binddragmovemove \
    -binddragstartcopy -binddragstartmove -canvasheight -canvaswidth  \
    -clickinterval -computeiconsize -dropcommand -edgefont -edgeorder  \
    -graphorder -iconfont -iconheight -iconspaceh -iconspacev -iconwidth \
    -layout -layoutcommand -layoutupdate -menu1 -menu2 -menu3 -menupath \
    -placing -selectcommand -selectmode -sortcommand -tkiconlib -toplevel \
    -background -borderwidth -buttonactivebg -buttonactivefg -canvasbg \
    -foreground -font -scrollbaractivebg -scrollbarbg -scrollbartrough \
  } \
  -configspec { \
    {-binddragendcopy bindDragEndCopy String {<ButtonRelease-2>}} \
    {-binddragendmove bindDragEndMove String {<ButtonRelease-2>}} \
    {-binddragmovecopy bindDragMoveCopy String {<B2-Motion>}} \
    {-binddragmovemove bindDragMoveMove String {<B2-Motion>}} \
    {-binddragstartcopy bindDragStartCopy String {<Button-2>}} \
    {-binddragstartmove bindDragStartMove String {<Shift-Button-2>}} \
    {-canvasheight canvasHeight Height {}} \
    {-canvaswidth canvaswidth Width {}} \
    {-clickinterval clickInterval Integer 300} \
    {-computeiconsize computeIconSize Boolean 1} \
    {-dropcommand dropCommand Command {}} \
    {-edgefont edgeFont Font *-helvetica-medium-r-normal-*-14-*-*-*-*-*-*-*} \
    {-edgeorder edgeOrder Boolean 0} \
    {-graphorder graphOrder Boolean 0} \
    {-iconfont iconFont Font *-helvetica-medium-r-normal-*-14-*-*-*-*-*-*-*} \
    {-iconheight iconHeight Integer 40} \
    {-iconspaceh iconSpaceH Integer 4} \
    {-iconspacev iconSpaceV Integer 4} \
    {-iconwidth iconWidth Integer 40} \
    {-layout layout Boolean 1} \
    {-layoutcommand layoutCommand Command {}} \
    {-layoutupdate layoutUpdate Boolean 0} \
    {-menu1 menu1 String {}} \
    {-menu2 menu2 String {}} \
    {-menu3 menu3 String {}} \
    {-menupath menuPath Path {}} \
    {-placing placing Boolean 0} \
    {-selectcommand selectCommand Command {}} \
    {-selectmode selectMode SelectMode single} \
    {-sortcommand sortCommand Command {}} \
    {-tkiconlib tkIconLib Path {}} \
    {-toplevel toplevel Boolean 1} \
    {-background background Background #d9d9d9} \
    {-borderwidth borderwidth BorderWidth 1} \
    {-buttonactivebg buttonActiveBg Foreground #ececec} \
    {-buttonactivefg buttonActiveFg Background Black} \
    {-canvasbg canvasBg Background #d9d9d9} \
    {-foreground foreground Foreground black} \
    {-font font Font -Adobe-Helvetica-Bold-R-Normal--*-120-*} \
    {-scrollbaractivebg scrollbarActiveBg Foreground #ececec} \
    {-scrollbarbg scrollbarBg ScrollbarBg #d9d9d9} \
    {-scrollbartrough scrollbarTrough ScrollbarTrough #c3c3c3} \
  } \
  -forcecall { \
    -canvasheight -canvaswidth -iconheight -iconspaceh -iconspacev \
    -iconwidth -layoutcommand -menu1 -menu2 -menu3 -menupath \
    -selectmode -tkiconlib -background -borderwidth -buttonactivebg \
    -buttonactivefg -canvasbg -foreground -font -scrollbaractivebg \
    -scrollbarbg -scrollbartrough \
  } \
}}]} {
  error "Please install Tix 4.0 (or later) on your system !!"
}

proc tkIconPane::CreateRootWidget {w args} {
  # create the root widget for the compound widget.

  upvar #0 $w data

  # a toplevel, or a frame ?
  set pos [lsearch $args -toplevel]
  if {$pos != -1} {
    set value [lindex $args [expr $pos + 1]]
  } {
    set value 1
  }
  if {"$value" == "0" || "$value" == "false" || "$value" == "False"} {
    frame $w -borderwidth 0 -class $data(ClassName)
  } {
    toplevel $w -borderwidth 0 -class $data(ClassName)
    wm protocol $w WM_DELETE_WINDOW "destroy $w"
    wm withdraw $w
  }
}

proc tkIconPane::InitWidgetRec {w} {
  global tkIconPane

  upvar #0 $w data

  set tkIconPane(currentPane) $w 
  set data(initialized) 0
  set data(iconsMoved) 0
  set data(clickCounter) 0
  set data(clickEval) 0
  set data(stretchX) 0
  set data(stretchY) 0
  set data(stretchRootX) 0
  set data(stretchRootY) 0
  set data(stretch) ""
  set data(currentId) ""
  set data(oldCurrentId) ""
  set data(eventType) ""
  set data(moveItems) ""
  set data(selectedId) ""
  set data(selectedEdge) ""
  set data(selectedEdgeList) ""
  set data(selectedIcon) ""
  set data(selectedIconList) ""
  set data(hiddenIcons) ""
  set data(dragMode) "copy"
}

proc tkIconPane::ConstructWidget {w args} {
  upvar #0 $w data

  tixChainMethod $w ConstructWidget

  scrollbar $w.hsb \
    -takefocus 0 \
    -orient horizontal
  set data(w:hsb) $w.hsb

  scrollbar $w.vsb \
    -takefocus 0 \
    -orient vertical
  set data(w:vsb) $w.vsb

  canvas $w.cbcanvas \
    -borderwidth 1 \
    -confine {true} \
    -height 2 \
    -relief sunken \
    -scrollregion {0c 0c 100c 100c} \
    -width 2
  set data(w:canvas) $w.cbcanvas
  set data(pw:client) $data(w:canvas)

  tkIconPane::InitWidgets $w
}

proc tkIconPane::InitWidgets {w} {
  global tkIconPane

  upvar #0 $w data

  # create the graph command
  tkgraph create $data(w:canvas)-graph -canvas $data(w:canvas)

  # initialize the data
  set tkIconPane(currentPane) $w

  # define the bindings
  bind $w <Map> "
    tkIconPane::IntBindMap $w"

  bind $data(w:canvas) <Configure> "
    tkIconPane::IntBindConfigure $w"
  # select icon (single)
  if {"$data(-selectmode)" == "multiple"} {
    bind $data(w:canvas) <Button-1> "
     tkIconPane::IntBindItemSelect $w startmove2 %W %x %y %X %Y"
    bind $data(w:canvas) <Shift-Button-1> "
      tkIconPane::IntBindItemSelect $w startmove1 %W %x %y %X %Y"
  } {
    bind $data(w:canvas) <Button-1> "
      tkIconPane::IntBindItemSelect $w startmove1 %W %x %y %X %Y"
    bind $data(w:canvas) <Shift-Button-1> "
      tkIconPane::IntBindItemSelect $w startmove2 %W %x %y %X %Y"
  }
  # select icon (multiple), or stretch a rectangle for selection
  bind $data(w:canvas) <B1-Motion> "
    tkIconPane::IntBindItemSelect $w moving %W %x %y %X %Y"
  bind $data(w:canvas) <ButtonRelease-1> "
    tkIconPane::IntBindItemSelect $w endmove %W %x %y %X %Y"
  # move the viewport, or the icon under the cursor
  bind $data(w:canvas) $data(-binddragstartcopy) "
    tkIconPane::IntBindItemSelect $w startmove3 %W %x %y %X %Y"
  bind $data(w:canvas) $data(-binddragstartmove) "
    tkIconPane::IntBindItemSelect $w startmove4 %W %x %y %X %Y"
  bind $data(w:canvas) $data(-binddragmovecopy) "
    tkIconPane::IntBindItemSelect $w moving %W %x %y %X %Y"
  bind $data(w:canvas) $data(-binddragmovemove) "
    tkIconPane::IntBindItemSelect $w moving %W %x %y %X %Y"
  bind $data(w:canvas) $data(-binddragendcopy) "
    tkIconPane::IntBindItemSelect $w endmove %W %x %y %X %Y"
  bind $data(w:canvas) $data(-binddragendmove) "
    tkIconPane::IntBindItemSelect $w endmove %W %x %y %X %Y"
  # popup the menus
  bind $data(w:canvas) <Button-3> "
    tkIconPane::IntBindMenuPost $w %x %y %X %Y 1"
  bind $data(w:canvas) <Alt-Button-3> "
    tkIconPane::IntBindMenuPost $w %x %y %X %Y 2"
  bind $data(w:canvas) <Control-Button-3> "
    tkIconPane::IntBindMenuPost $w %x %y %X %Y 3"

  $data(w:canvas) bind all <Enter> "
    tkIconPane::IntBindItemEnter $w current"
  $data(w:canvas) bind all <Leave> "
    tkIconPane::IntBindItemEnter $w"

  # initialize drag&drop
  if {"[info commands blt_drag&drop]" != ""} {
    blt_drag&drop source $data(w:canvas) config \
      -button 0 \
      -selftarget true \
      -send all \
      -tokenanchor nw \
      -packagecmd tkIconPane::IntDragDropIcon \
      -tokenborderwidth 0
    blt_drag&drop source $data(w:canvas) handler icon dd_send_icon
    blt_drag&drop target $data(w:canvas) handler icon "tkIconPane::IntHandleDragDrop $w icon $data(w:canvas)"
  }

  # special handling for toplevels
  if {$data(-toplevel)} {
    wm deiconify $w
  }
}

proc tkIconPane::SetBindings {w} {
  upvar #0 $w data

  catch "tixChainMethod $w SetBindings"

  $data(w:canvas) config \
    -xscrollcommand "$data(w:hsb) set" \
    -yscrollcommand "$data(w:vsb) set"

  $data(w:hsb) config -command "$data(w:canvas) xview"
  $data(w:vsb) config -command "$data(w:canvas) yview"
}

# widget configuration

proc tkIconPane::config {w option value} {
  upvar #0 $w data

  set data($option) $value
  switch -glob -- $option {
    {-canvasheight} {
      if {"$value" != ""} {
        $data(w:canvas) config -height $value
      }
    }
    {-canvaswidth} {
      if {"$value" != ""} {
        $data(w:canvas) config -width $value
      }
    }
    {-iconheight} -
    {-iconspaceh} -
    {-iconspacev} -
    {-iconwidth} {
      if {$data(initialized)} {
        tkIconPane::layout $w -force 0
      }
    }
    {-menu?} {
      if {[string compare "" $value] &&
          [string compare "" [string index $value 0]] &&
          ![string compare "" [info commands $data(w:canvas).vmenu-$value]]} {
        foreach tmpElement $data(-menupath) {
          if {[file exists $tmpElement/$value.vmn]} {
            set cbCanvasPath $data(w:canvas).vmenu
            catch "source $tmpElement/$value.vmn"
            break
          }
        }
      }
    }
    {-menupath} {
      $w config -menu1 $data(-menu1)
      $w config -menu2 $data(-menu2)
      $w config -menu3 $data(-menu3)
    }
    {-selectmode} {
      if {"$value" == "multiple"} {
        bind $data(w:canvas) <Button-1> "
          tkIconPane::IntBindItemSelect $w startmove2 %W %x %y %X %Y"
        bind $data(w:canvas) <Shift-Button-1> "
          tkIconPane::IntBindItemSelect $w startmove1 %W %x %y %X %Y"
      } {
        bind $data(w:canvas) <Button-1> "
          tkIconPane::IntBindItemSelect $w startmove1 %W %x %y %X %Y"
        bind $data(w:canvas) <Shift-Button-1> "
          tkIconPane::IntBindItemSelect $w startmove2 %W %x %y %X %Y"
      }
    }

    {-background} {
      $data(rootCmd) config -background $value
      $data(w:hsb) config -highlightbackground $value
      $data(w:vsb) config -highlightbackground $value
      $data(w:canvas) config -highlightbackground $value
    }
    {-borderwidth} {
      $data(w:canvas) config -borderwidth $value
    }
    {-canvasbg} {
      $data(w:canvas) config -background $value
    }
    {-scrollbaractivebg} {
      $data(w:hsb) config -activebackground $value
      $data(w:vsb) config -activebackground $value
    }
    {-scrollbarbg} {
      $data(w:hsb) config -background $value
      $data(w:vsb) config -background $value
    }
    {-scrollbartrough} {
      $data(w:hsb) config -troughcolor $value
      $data(w:vsb) config -troughcolor $value
    }
  }
  return $data($option)
}

# methods

proc tkIconPane::dropposition {w} {
  upvar #0 $w data

  set tmpPos [blt_drag&drop location]
  set tmpX [$w canvasx [expr [lindex $tmpPos 0]-[winfo rootx $w]]]
  set tmpY [$w canvasy [expr [lindex $tmpPos 1]-[winfo rooty $w]]]
  return [list $tmpX $tmpY]
}

proc tkIconPane::edgeconfig {w args} {
  upvar #0 $w data

  if {[llength $args] == 1} {
    return [$data(w:canvas) itemconfig [lindex $args 0]]
  } elseif {[llength $args] == 2} {
    if {[string match "-*" [lindex $args 0]]} {
      foreach counter [$data(w:canvas) find all] {
        if {![string compare "edge" [$data(w:canvas) type $counter]]} {
          $data(w:canvas) itemconfig $counter [lindex $args 0] [lindex $args 1]
        }
      }
    } {
      return [$data(w:canvas) itemconfig [lindex $args 0] [lindex $args 1]]
    }
  } elseif {[llength $args] > 2} {
    set edgeParams ""
    set cbCanvasPath $data(w:canvas).emenu
    for {set counter 1} {$counter < [llength $args]} {incr counter 2} {
      if {[string match "-menu*" [lindex $args $counter]]} {
        set value [lindex $args [expr $counter + 1]]
        if {![string match "." $value] &&
            "[info commands $data(w:canvas).emenu-$value]" == ""} {
          foreach dir [split $data(-tkiconlib) :] {
            catch "source $dir/$value.emn"
          }
        }
      }
      lappend edgeParams [lindex $args $counter]
      lappend edgeParams [lindex $args [expr $counter + 1]]
    }
    if {"$edgeParams" != ""} {
      eval $data(w:canvas) itemconfig [lindex $args 0] $edgeParams
    }
  }
  return ""
}

proc tkIconPane::edgecreate {w args} {
  upvar #0 $w data

  set result ""
  foreach edgeElement $args {
    set from [lindex $edgeElement 0]
    set to [lindex $edgeElement 1]
    if {[catch "$data(w:canvas) itemconfig $from -name"] ||
        [catch "$data(w:canvas) itemconfig $to -name"]} {
      error "No -from and -to specified"
    }
    switch -- [lindex $edgeElement 2] {
      {1} {
        set arrowStyle last
      }
      {2} {
        set arrowStyle both
      }
      {3} {
        set arrowStyle first
      }
      {default} {
        set arrowStyle none
      }
    }
    set edgeParams ""
    set cbCanvasPath $data(w:canvas).emenu
    for {set counter 3} {$counter < [llength $edgeElement]} {incr counter 2} {
      if {[string match "-menu*" [lindex $edgeElement $counter]]} {
        set value [lindex $edgeElement [expr $counter + 1]]
        if {![string match "." $value] &&
            "[info commands $data(w:canvas).emenu-$value]" == ""} {
          foreach dir [split $data(-tkiconlib) :] {
            catch "source $dir/$value.emn"
          }
        }
      }
      lappend edgeParams [lindex $edgeElement $counter]
      lappend edgeParams [lindex $edgeElement [expr $counter + 1]]
    }
    if {[catch "$data(w:canvas) create edge -10 -10 -10 -10 \
          -arrow $arrowStyle -from \{$from\} \
	  -to \{$to\} -font \{$data(-edgefont)\} $edgeParams \
          -graphname $data(w:canvas)-graph" edge]} {
      error $edge
    }
    if {$data(initialized)} {
      if {[catch "$data(w:canvas)-graph layout item -idlist $edge -edgeorder $data(-edgeorder) -graphorder $data(-graphorder)" result]} {
        error $result
      }
    }
    $data(w:canvas) lower $edge
    set currentEdges [lindex [$data(w:canvas) itemconfig $from -edges] 4]
    lappend currentEdges [list $from $to $edge]
    $data(w:canvas) itemconfig $from -edges $currentEdges
    set currentEdges [lindex [$data(w:canvas) itemconfig $to -edges] 4]
    lappend currentEdges [list $from $to $edge]
    $data(w:canvas) itemconfig $to -edges $currentEdges
    lappend result $edge
  }
  return $result
}

proc tkIconPane::edgedelete {w args} {
  upvar #0 $w data

  if {[llength $args] == 0} {
    set data(selectedEdge) ""
    set data(selectedEdgeList) ""
    foreach counter [$data(w:canvas) find all] {
      if {![string compare "edge" [$data(w:canvas) type $counter]]} {
        $data(w:canvas) delete $counter
      }
      if {![string compare "icon" [$data(w:canvas) type $counter]]} {
        $data(w:canvas) itemconfig $counter -edges {}
      }
    }
  } {
    set from ""
    set to ""
    if {[llength $args] == 1} {
      set from [lindex [$data(w:canvas) itemconfig [lindex $args 0] -from] 4]
      set to [lindex [$data(w:canvas) itemconfig [lindex $args 0] -to] 4]
    } {
      if {[llength $args] == 2} {
        set from [lindex $args 0]
        set to [lindex $args 1]
      }
    }
    if {![string compare "" $from] && ![string compare "" $to]} {
      foreach counter [$data(w:canvas) find all] {
        if {![string compare "edge" [$data(w:canvas) type $counter]]} {
          set from [lindex [$data(w:canvas) itemconfig $counter -from] 4]
          set to [lindex [$data(w:canvas) itemconfig $counter -to] 4]
          set fromEdges [lindex [$data(w:canvas) itemconfig $from -edges] 4]
          set newFromEdges ""
          set toEdges [lindex [$data(w:canvas) itemconfig $to -edges] 4]
          set newToEdges ""
          foreach edge $fromEdges {
            if {([lindex $edge 0] == $from && [lindex $edge 1] == $to) ||
                ([lindex $edge 0] == $to && [lindex $edge 1] == $from)} {
              set tmpIndex [lsearch $data(selectedEdgeList) [lindex $edge 2]]
              if {"$tmpIndex" != -1} {
                set data(selectedEdgeList) [lreplace $data(selectedEdgeList) $tmpIndex $tmpIndex]
              }
              if {"$data(selectedEdge)" == "[lindex $edge 2]"} {
                set data(selectedEdge) ""
              }
              catch "$data(w:canvas) delete [lindex $edge 2]"
            } {
              lappend newFromEdges $edge
            }
          }
          $data(w:canvas) itemconfig $from -edges $newFromEdges
          foreach edge $toEdges {
            if {([lindex $edge 0] == $from && [lindex $edge 1] == $to) ||
                ([lindex $edge 0] == $to && [lindex $edge 1] == $from)} {
              set tmpIndex [lsearch $data(selectedEdgeList) [lindex $edge 2]]
              if {"$tmpIndex" != -1} {
                set data(selectedEdgeList) [lreplace $data(selectedEdgeList) $tmpIndex $tmpIndex]
              }
              if {"$data(selectedEdge)" == "[lindex $edge 2]"} {
                set data(selectedEdge) ""
              }
              catch "$data(w:canvas) delete [lindex $edge 2]"
            } {
              lappend newToEdges $edge
            }
          }
          $data(w:canvas) itemconfig $to -edges $newToEdges
          set tmpIndex [lsearch $data(selectedEdgeList) $counter]
          if {"$tmpIndex" != -1} {
            set data(selectedEdgeList) [lreplace $data(selectedEdgeList) $tmpIndex $tmpIndex]
          }
          if {"$data(selectedEdge)" == "$counter"} {
            set data(selectedEdge) ""
          }
          $data(w:canvas) delete $counter
        }
      }
    } {
      if {[string compare "" $from] && [string compare "" $to]} {
        set fromEdges [lindex [$data(w:canvas) itemconfig $from -edges] 4]
        set newFromEdges ""
        set toEdges [lindex [$data(w:canvas) itemconfig $to -edges] 4]
        set newToEdges ""
        foreach edge $fromEdges {
          if {([lindex $edge 0] == $from && [lindex $edge 1] == $to) ||
              ([lindex $edge 0] == $to && [lindex $edge 1] == $from)} {
            set tmpIndex [lsearch $data(selectedEdgeList) [lindex $edge 2]]
            if {"$tmpIndex" != -1} {
              set data(selectedEdgeList) [lreplace $data(selectedEdgeList) $tmpIndex $tmpIndex]
            }
            if {"$data(selectedEdge)" == "[lindex $edge 2]"} {
              set data(selectedEdge) ""
            }
            catch "$data(w:canvas) delete [lindex $edge 2]"
          } {
            lappend newFromEdges $edge
          }
        }
        $data(w:canvas) itemconfig $from -edges $newFromEdges
        foreach edge $toEdges {
          if {([lindex $edge 0] == $from && [lindex $edge 1] == $to) ||
              ([lindex $edge 0] == $to && [lindex $edge 1] == $from)} {
            set tmpIndex [lsearch $data(selectedEdgeList) [lindex $edge 2]]
            if {"$tmpIndex" != -1} {
              set data(selectedEdgeList) [lreplace $data(selectedEdgeList) $tmpIndex $tmpIndex]
            }
            if {"$data(selectedEdge)" == "[lindex $edge 2]"} {
              set data(selectedEdge) ""
            }
            catch "$data(w:canvas) delete [lindex $edge 2]"
          } {
            lappend newToEdges $edge
          }
        }
        $data(w:canvas) itemconfig $to -edges $newToEdges
      }
    }
  }
  return ""
}

proc tkIconPane::edgeinfo {w args} {
  upvar #0 $w data

  set result ""
  if {[llength $args] == 1} {
    set currentEdge $data(selectedEdge)
  } {
    set currentEdge [lindex $args 1]
  }
  switch -- [lindex $args 0] {
    {id} {
      if {[llength $args] == 1} {
        return $currentEdge
      } elseif {[llength $args] == 3} {
        return [$data(w:canvas)-graph edgeinfo id iconList [lindex $args 1] [lindex $args 2]]
      }
    }
    {name} {
      return [lindex [$data(w:canvas) itemconfig $currentEdge -name] 4]
    }
    {fromtoid} {
      foreach counter [$data(w:canvas)-graph iconinfo ids] {
        foreach edge [lindex [$data(w:canvas) itemconfig $counter -edges] 4] {
          if {![string compare $currentEdge [lindex $edge 2]]} {
            return [list [lindex $edge 0] [lindex $edge 1]]
          }
        }
      }
    }
    {fromtoname} {
      foreach counter [$data(w:canvas)-graph iconinfo ids] {
        foreach edge [lindex [$data(w:canvas) itemconfig $counter -edges] 4] {
          if {![string compare $currentEdge [lindex $edge 2]]} {
            return [list [lindex [$data(w:canvas) itemconf [lindex $edge 0] -name] 4] [lindex [$data(w:canvas) itemconf [lindex $edge 1] -name] 4]]
          }
        }
      }
    }
    {fromtoids} {
      foreach counter [$data(w:canvas) find all] {
        if {![string compare "icon" [$data(w:canvas) type $counter]]} {
          foreach edge [lindex [$data(w:canvas) itemconfig $counter -edges] 4] {
            if {[lsearch $result [list [lindex $edge 0] [lindex $edge 1]]] == -1} {
              lappend result [list [lindex $edge 0] [lindex $edge 1]]
            }
          }
        }
      }
      return $result
    }
    {fromtonames} {
      foreach counter [$data(w:canvas) find all] {
        if {![string compare "icon" [$data(w:canvas) type $counter]]} {
          foreach edge [lindex [$data(w:canvas) itemconfig $counter -edges] 4] {
            if {[lsearch $result [list [lindex [$data(w:canvas) itemconf [lindex $edge 0] -name] 4] [lindex [$data(w:canvas) itemconf [lindex $edge 1] -name] 4]]] == -1} {
              lappend result [list [lindex [$data(w:canvas) itemconf [lindex $edge 0] -name] 4] [lindex [$data(w:canvas) itemconf [lindex $edge 1] -name] 4]]
            }
          }
        }
      }
      return $result
    }
    {ids} -
    {names} {
      return [$data(w:canvas)-graph edgeinfo [lindex $args 0]]
    }
    {graphs} {
      return [$data(w:canvas)-graph edgeinfo graphs]
    }
  }
  return ""
}

proc tkIconPane::iconconfig {w args} {
  upvar #0 $w data

  if {[llength $args] == 1} {
    return [$data(w:canvas) itemconfig [lindex $args 0]]
  } elseif {[llength $args] == 2} {
    return [$data(w:canvas) itemconfig [lindex $args 0] [lindex $args 1]]
  } elseif {[llength $args] > 2} {
    set iconParams ""
    set imageName ""
    set statusName ""
    set cbCanvasPath $data(w:canvas).imenu
    for {set counter 1} {$counter < [llength $args]} {incr counter 2} {
      if {[string match "-menu*" [lindex $args $counter]]} {
        set value [lindex $args [expr $counter + 1]]
        if {![string match "." $value] &&
            "[info commands $data(w:canvas).imenu-$value]" == ""} {
          foreach dir [split $data(-tkiconlib) :] {
            catch "source $dir/$value.imn"
          }
        }
      } elseif {[string match "-image*" [lindex $args $counter]] ||
                [string match "-status*" [lindex $args $counter]]} {
        set value [lindex $args [expr $counter + 1]]
        set tmpImageName $value
        if {[string match "-image*" [lindex $args $counter]]} {
          set imageName $value
        } {
          set statusName $value
        }
        if {[lsearch [image names] $value] == -1} {
          set tmpList [eval list [file dirname $value] [split $data(-tkiconlib) :]]
          set value [file tail $value]
          foreach dir $tmpList {
            if {"[file extension $value]" == ".bmp"} {
              if {![catch "image create bitmap $tmpImageName -file $dir/$value"]} {
                break
              }
            } elseif {"[file extension $value]" == ".ppm"} {
              if {![catch "image create photo $tmpImageName -file $dir/$value"]} {
                break
              }
            } elseif {"[file extension $value]" == ".xpm"} {
              if {![catch "image create pixmap $tmpImageName -file $dir/$value"]} {
                break
              }
            } elseif {"[file extension $value]" == ".gif"} {
              if {![catch "image create photo $tmpImageName -format gif -file $dir/$value"]} {
                break
              }
            } {
              if {[file exists $dir/$value.bmp]} {
                if {![catch "image create bitmap $tmpImageName -file $dir/$value.bmp"]} {
                  break
                }
              } elseif {[file exists $dir/$value.ppm]} {
                if {![catch "image create photo $tmpImageName -file $dir/$value.ppm"]} {
                  break
                }
              } elseif {[file exists $dir/$value.xpm]} {
                if {![catch "image create pixmap $tmpImageName -file $dir/$value.xpm"]} {
                  break
                }
              } elseif {[file exists $dir/$value.gif]} {
                if {![catch "image create photo $tmpImageName -format gif -file $dir/$value.gif"]} {
                  break
                }
              } {
                if {![catch "image create bitmap $tmpImageName -file $dir/$value"]} {
                  break
                } elseif {![catch "image create photo $tmpImageName -file $dir/$value"]} {
                  break
                } elseif {![catch "image create pixmap $tmpImageName -file $dir/$value"]} {
                  break
                } elseif {![catch "image create photo $tmpImageName -format gif -file $dir/$value"]} {
                  break
                }
              }
            }
          }
        }
        continue
      }
      lappend iconParams [lindex $args $counter]
      lappend iconParams [lindex $args [expr $counter + 1]]
    }
    if {"$iconParams" != ""} {
      eval $data(w:canvas) itemconfig [lindex $args 0] $iconParams
    }
    if {"$imageName" != ""} {
      catch "$data(w:canvas) itemconfig [lindex $args 0] -image $imageName"
    }
    if {"$statusName" != ""} {
      catch "$data(w:canvas) itemconfig [lindex $args 0] -status $statusName"
    }
  }
  return ""
}

proc tkIconPane::iconcreate {w args} {
  upvar #0 $w data

  set result ""
  if {[llength $args] > 0} {
    set iconName [lindex $args 0]
    set iconParams ""
    set imageName ""
    set statusName ""
    set cbCanvasPath $data(w:canvas).imenu
    for {set counter 1} {$counter < [llength $args]} {incr counter 2} {
      if {[string match "-menu*" [lindex $args $counter]]} {
        set value [lindex $args [expr $counter + 1]]
        if {![string match "." $value] &&
            "[info commands $data(w:canvas).imenu-$value]" == ""} {
          foreach dir [split $data(-tkiconlib) :] {
            catch "source $dir/$value.imn"
          }
        }
      } elseif {[string match "-image*" [lindex $args $counter]] ||
                [string match "-status*" [lindex $args $counter]]} {
        set value [lindex $args [expr $counter + 1]]
        set tmpImageName $value
        if {[string match "-image*" [lindex $args $counter]]} {
          set imageName $value
        } {
          set statusName $value
        }
        if {[lsearch [image names] $value] == -1} {
          set tmpList [eval list [file dirname $value] [split $data(-tkiconlib) :]]
          set value [file tail $value]
          foreach dir $tmpList {
            if {"[file extension $value]" == ".bmp"} {
              if {![catch "image create bitmap $tmpImageName -file $dir/$value"]} {
                break
              }
            } elseif {"[file extension $value]" == ".ppm"} {
              if {![catch "image create photo $tmpImageName -file $dir/$value"]} {
                break
              }
            } elseif {"[file extension $value]" == ".xpm"} {
              if {![catch "image create pixmap $tmpImageName -file $dir/$value"]} {
                break
              }
            } elseif {"[file extension $value]" == ".gif"} {
              if {![catch "image create photo $tmpImageName -format gif -file $dir/$value"]} {
                break
              }
            } {
              if {[file exists $dir/$value.bmp]} {
                if {![catch "image create bitmap $tmpImageName -file $dir/$value.bmp"]} {
                  break
                }
              } elseif {[file exists $dir/$value.ppm]} {
                if {![catch "image create photo $tmpImageName -file $dir/$value.ppm"]} {
                  break
                }
              } elseif {[file exists $dir/$value.xpm]} {
                if {![catch "image create pixmap $tmpImageName -file $dir/$value.xpm"]} {
                  break
                }
              } elseif {[file exists $dir/$value.gif]} {
                if {![catch "image create photo $tmpImageName -format gif -file $dir/$value.gif"]} {
                  break
                }
              } {
                if {![catch "image create bitmap $tmpImageName -file $dir/$value"]} {
                  break
                } elseif {![catch "image create photo $tmpImageName -file $dir/$value"]} {
                  break
                } elseif {![catch "image create pixmap $tmpImageName -file $dir/$value"]} {
                  break
                } elseif {![catch "image create photo $tmpImageName -format gif -file $dir/$value"]} {
                  break
                }
              }
            }
          }
        }
        continue
      }
      lappend iconParams [lindex $args $counter]
      lappend iconParams [lindex $args [expr $counter + 1]]
    }
    if {[catch "$data(w:canvas) create icon 10 10 -font \{$data(-iconfont)\} -name \{$iconName\} $iconParams -graphname $data(w:canvas)-graph" result]} {
      error $result
    }
    if {"$imageName" != ""} {
      catch "$data(w:canvas) itemconfig $result -image $imageName"
    }
    if {"$statusName" != ""} {
      catch "$data(w:canvas) itemconfig $result -status $statusName"
    }
    if {$data(initialized)} {
      tkIconPane::layout $w -force 0
    }
  }
  return $result
}

proc tkIconPane::icondelete {w args} {
  upvar #0 $w data

  set iconList [lindex $args 0]
  if {![string compare "" $iconList]} {
    set data(selectedIcon) ""
    set data(selectedIconList) ""
    foreach icon [$data(w:canvas) find all] {
      if {![string compare "icon" [$data(w:canvas) type $icon]] ||
          ![string compare "edge" [$data(w:canvas) type $icon]]} {
        $data(w:canvas) delete $icon
      }
    }
  } {
    foreach icon $iconList {
      foreach edge [lindex [$data(w:canvas) itemconfig $icon -edges] 4] {
        $data(w:canvas) delete [lindex $edge 2]
        set result ""
        if {![string compare $icon [lindex $edge 0]]} {
          foreach otherSide [lindex [$data(w:canvas) itemconfig [lindex $edge 1] -edges] 4] {
            if {[string compare $icon [lindex $otherSide 0]] &&
                [string compare $icon [lindex $otherSide 1]]} {
              lappend result $otherSide
            }
          }
          $data(w:canvas) itemconfig [lindex $edge 1] -edges $result
        } {
          if {![string compare $icon [lindex $edge 1]]} {
            foreach otherSide [lindex [$data(w:canvas) itemconfig [lindex $edge 0] -edges] 4] {
              if {[string compare $icon [lindex $otherSide 0]] &&
                  [string compare $icon [lindex $otherSide 1]]} {
                lappend result $otherSide
              }
            }
            $data(w:canvas) itemconfig [lindex $edge 0] -edges $result
          }
        }
      }
      set tmpIndex [lsearch $data(selectedIconList) $icon]
      if {"$tmpIndex" != -1} {
        set data(selectedIconList) [lreplace $data(selectedIconList) $tmpIndex $tmpIndex]
      }
      if {"$data(selectedIcon)" == "$icon"} {
        set data(selectedIcon) ""
      }
      $data(w:canvas) delete $icon
    }
  }
  return ""
}

proc tkIconPane::iconinfo {w args} {
  upvar #0 $w data

  set result ""
  if {[llength $args] == 1} {
    set currentIcon $data(selectedIcon)
  } {
    set currentIcon [lindex $args 1]
  }
  switch -- [lindex $args 0] {
    {id} {
      if {[llength $args] == 1} {
        return $currentIcon
      } {
        return [$data(w:canvas)-graph iconinfo id $currentIcon]
      }
    }
    {current} {
      return $data(currentId)
    }
    {selectedids} {
      set result ""
      foreach counter $data(selectedIconList) {
        if {![catch "$data(w:canvas) itemconfig $counter -name"]} {
          lappend result $counter
        }
      }
      return $result
    }
    {selectednames} {
      set result ""
      foreach counter $data(selectedIconList) {
        if {![catch "$data(w:canvas) itemconfig $counter -name"]} {
          lappend result [lindex [$data(w:canvas) itemconf $counter -name] 4]
        }
      }
      return $result
    }
    {stateselect} {
      if {[llength $args] == 1} {
        set data(selectedIconList) ""
        foreach counter [$data(w:canvas) find all] {
          if {![string compare "icon" [$data(w:canvas) type $counter]]} {
            catch "$data(w:canvas) itemconfig $counter -state selected"
            lappend data(selectedIconList) $counter
          }
        }
      } {
        catch "$data(w:canvas) itemconfig $currentIcon -state selected"
        if {[lsearch $data(selectedIconList) $currentIcon] == -1} {
          lappend data(selectedIconList) $currentIcon
        }
      }
    }
    {statedeselect} {
      if {[llength $args] == 1} {
        foreach counter $data(selectedIconList) {
          catch "$data(w:canvas) itemconfig $counter -state normal"
        }
        set data(selectedIconList) ""
      } {
        catch "$data(w:canvas) itemconfig $currentIcon -state normal"
        set tmpPosition [lsearch $data(selectedIconList) $currentIcon]
        if {$tmpPosition != -1} {
          set data(selectedIconList) [lreplace $data(selectedIconList) $tmpPosition $tmpPosition]
        }
      }
    }
    {statetoggle} {
      set tmpState [lindex [$data(w:canvas) itemconfig $currentIcon -state] 4]
      set tmpPosition [lsearch $data(selectedIconList) $currentIcon]
      if {![string compare "selected" "$tmpState"]} {
        catch "$data(w:canvas) itemconfig $currentIcon -state normal"
        if {$tmpPosition != -1} {
          set data(selectedIconList) [lreplace $data(selectedIconList) $tmpPosition $tmpPosition]
        }
      } {
        catch "$data(w:canvas) itemconfig $currentIcon -state selected"
        if {[lsearch $data(selectedIconList) $currentIcon] == -1} {
          lappend data(selectedIconList) $currentIcon
        }
      }
    }
    {name} -
    {state} -
    {status} -
    {tags} {
      return [lindex [$data(w:canvas) itemconf $currentIcon -[lindex $args 0]] 4]
    }
    {ids} -
    {names} {
      return [$data(w:canvas)-graph iconinfo [lindex $args 0]]
    }
  }
  return ""
}

proc tkIconPane::layout {w args} {
  upvar #0 $w data

  set forceLayout 1
  if {![string compare [lindex $args 0] -force]} {
    if {![string compare [lindex $args 1] 0]} {
      set forceLayout 0
    }
    set args [lrange $args 2 end]
  }
  set partialRelayout 0
  set completeRelayout 0
  set data(w:canvas) $w.cbcanvas
  if {![string compare "edges" [lindex $args 0]] ||
      ![string compare "2" [lindex $args 0]]} {
    if {[lsearch $args "edges"] != -1} {
      set args [lreplace $args [lsearch $args "edges"] [lsearch $args "edges"]]
    }
    if {$data(updateedges)} {
      eval $data(w:canvas)-graph layout edges -edgeorder $data(-edgeorder) -graphorder $data(-graphorder) $args
    }
    return
  }
  if {(!$data(initialized) && !$forceLayout) ||
      ($data(iconsMoved) && !$forceLayout) ||
      (!$data(-layout) && !$forceLayout) ||
       ![string compare "" [info commands $data(w:canvas)]]} {
    return
  }
  set data(iconsMoved) 0

  if {![string compare "nolayout" $data(-layoutcommand)]} {
    if {$data(updateedges)} {
      eval $data(w:canvas)-graph layout edges -edgeorder $data(-edgeorder) -graphorder $data(graphorder) $args
    }
    return
  } {
    if {![string compare "matrix" [lindex $data(-layoutcommand) 0]] ||
        ![string compare "isi" [lindex $data(-layoutcommand) 0]] ||
        ![string compare "random" [lindex $data(-layoutcommand) 0]] ||
        ![string compare "tree" [lindex $data(-layoutcommand) 0]]} {
      if {[string compare "" $data(-sortcommand)] &&
          [string compare "" [info commands $data(-sortcommand)]]} {
        eval $data(w:canvas)-graph layout [lindex $data(-layoutcommand) 0] \
          -iconwidth $data(-iconwidth) -iconheight $data(-iconheight) \
          -iconspaceh $data(-iconspaceh) -iconspacev $data(-iconspacev) \
          -computeiconsize $data(-computeiconsize) \
	  -sortcommand $data(-sortcommand) \
          -edgeorder $data(-edgeorder) -graphorder $data(-graphorder) \
          [lrange $data(-layoutcommand) 1 end]
      } {
        eval $data(w:canvas)-graph layout [lindex $data(-layoutcommand) 0] \
	  -iconwidth $data(-iconwidth) -iconheight $data(-iconheight) \
          -iconspaceh $data(-iconspaceh) -iconspacev $data(-iconspacev) \
          -computeiconsize $data(-computeiconsize) \
          -edgeorder $data(-edgeorder) -graphorder $data(-graphorder) \
          [lrange $data(-layoutcommand) 1 end]
      }
    } {
      if {[string compare "" [string trim $data(-layoutcommand)]]} {
        eval $data(-layoutcommand) $data(w:canvas) $args
      }
    }
  }
  return ""
}

# internal procedures

proc tkIconPane::GeometryInfo {w mW mH} {
  upvar #0 $w data

  if {[string compare "" $data(-layoutcommand)]} {
    if {"$data(-layoutcommand)" == "matrix" ||
        "$data(-layoutcommand)" == "list"} {
      return [list [list 0.0 1.0] [list 0.0 0.0]]
    } {
      return [list [list 0.0 0.0] [list 0.0 0.0]]
    }
  } {
    return [list [list 0.0 0.0] [list 0.0 0.0]]
  }
}

proc tkIconPane::IntBindConfigure {w} {
  upvar #0 $w data

  if {$data(initialized) && $data(-layoutupdate)} {
    tkIconPane::layout $w -force 0
  }
}

proc tkIconPane::IntBindItemEnter {w {type ""}} {
  upvar #0 $w data

  set data(oldCurrentId) $data(currentId)
  if {![string compare $type "current"]} {
    set tmpId [$data(w:canvas) find withtag current]
    set tmpType [$data(w:canvas) type $tmpId] 
    if {![string compare $tmpType "icon"] ||
        ![string compare $tmpType "edge"]} {
      set data(currentId) [$data(w:canvas) find withtag current]
    }
  } {
    set data(currentId) $type
  }
}

proc tkIconPane::IntBindMap {w} {
  upvar #0 $w data

  if {$data(initialized) || "$w" == ""} {
    return
  }
  set data(initialized) 1
}

proc tkIconPane::IntBindItemSelect {w eventType canvas x y X Y} {
  upvar #0 $w data

  if {![string compare "" $data(currentId)]} {
    if {"$eventType" == "startmove1"} {
      tkIconPane::IntBindItemMoveStart $w $eventType $canvas $x $y $X $Y
    } elseif {"$eventType" == "startmove2"} {
      tkIconPane::IntBindItemMoveStart $w $eventType $canvas $x $y $X $Y
    } elseif {"$eventType" == "startmove3"} {
      tkIconPane::IntBindItemMoveStart $w $eventType $canvas $x $y $X $Y
    } elseif {"$eventType" == "startmove4"} {
      tkIconPane::IntBindItemMoveStart $w $eventType $canvas $x $y $X $Y
    } elseif {"$eventType" == "moving"} {
      tkIconPane::IntBindItemMoveMove $w $canvas $x $y $X $Y
    } elseif {"$eventType" == "endmove"} {
      tkIconPane::IntBindItemMoveStop $w $canvas $x $y $X $Y
    }
  } {
    if {$eventType == "startmove1"} {
      if {$data(clickCounter) == 1} {
        set data(clickEval) 0
        set data(clickCounter) 2
        tkIconPane::IntBindItemEnter $w $data(clickId)
        tkIconPane::IntItemActivate $w $x $y
        set data(clickId) ""
      } {
        set data(clickEval) 1
        set data(clickCounter) 1
        set data(clickId) $data(currentId)
        after $data(-clickinterval) "set ${w}(clickCounter) 0"
        tkwait variable ${w}(clickCounter)
        if {$data(clickCounter) == 0 && $data(clickEval)} {
          set data(eventType) ""
          tkIconPane::IntBindItemEnter $w $data(clickId)
          tkIconPane::IntItemDeSelect $w
          tkIconPane::IntItemSetSelected $w
          tkIconPane::IntItemSelect $w $x $y
          set data(clickId) ""
        }
      }
    } elseif {$eventType == "startmove2"} {
      if {$data(clickCounter) == 1} {
        set data(clickEval) 0
        set data(clickCounter) 2
        tkIconPane::IntBindItemEnter $w $data(clickId)
        tkIconPane::IntItemSetSelected $w
        tkIconPane::IntItemSelect $w $x $y 1
        tkIconPane::IntItemActivate $w $x $y
        set data(clickId) ""
      } {
        set data(clickEval) 1
        set data(clickCounter) 1
        set data(clickId) $data(currentId)
        after $data(-clickinterval) "set ${w}(clickCounter) 0"
        tkwait variable ${w}(clickCounter)
        if {$data(clickCounter) == 0 && $data(clickEval)} {
          set data(eventType) ""
          tkIconPane::IntBindItemEnter $w $data(clickId)
          tkIconPane::IntItemSetSelected $w
          tkIconPane::IntItemSelect $w $x $y 1
          set data(clickId) ""
        }
      }
    } elseif {$eventType == "startmove3"} {
      set data(eventType) $eventType
      tkIconPane::IntItemMoveStart $w $canvas $x $y $X $Y
    } elseif {$eventType == "startmove4"} {
      set data(eventType) $eventType
      tkIconPane::IntItemMoveStart $w $canvas $x $y $X $Y
    } elseif {$eventType == "moving"} {
      tkIconPane::IntItemMoveMove $w $canvas $x $y $X $Y
    } elseif {$eventType == "endmove"} {
      tkIconPane::IntItemMoveStop $w $canvas $x $y $X $Y
      set data(eventType) ""
    }
  }
}

proc tkIconPane::IntBindItemMoveMove {w canvas x y X Y} {
  upvar #0 $w data

  tkIconPane::IntItemSetSelected $w
  tkIconPane::IntItemMoveMove $w $canvas $x $y $X $Y
}

proc tkIconPane::IntBindItemMoveStart {w eventType canvas x y X Y} {
  upvar #0 $w data

  if {"$data(eventType)" != "startmove3" &&
      "$data(eventType)" != "startmove4"} {
    set data(eventType) $eventType
    tkIconPane::IntItemSetSelected $w
    if {$data(eventType) == "startmove1"} {
      tkIconPane::IntItemDeSelect $w 1
    }
    tkIconPane::IntItemMoveStart $w $canvas $x $y $X $Y
  }
}

proc tkIconPane::IntBindItemMoveStop {w canvas x y X Y} {
  upvar #0 $w data

#  set data(clickEval) 0
  tkIconPane::IntItemSetSelected $w
  tkIconPane::IntItemMoveStop $w $canvas $x $y $X $Y
  set data(eventType) ""
}

proc tkIconPane::IntBindMenuPost {w x y X Y menuNumber} {
  upvar #0 $w data

  set menu ""
  set canvasItem $data(currentId)
  tkIconPane::IntItemSetSelected $w
  if {[string compare "" $data(selectedId)]} {
    set menu [lindex [$data(w:canvas) itemconfig $data(selectedId) -menu$menuNumber] 4]
    if {[string compare "" $menu] &&
        [string compare "." [string index $menu 0]]} {
      if {![string compare "icon" [$data(w:canvas) type $data(selectedId)]]} {
        set menu $data(w:canvas).imenu-$menu
      } {
        set menu $data(w:canvas).emenu-$menu
      }
    }
  } {
    set menu $data(-menu$menuNumber)
    if {[string compare "" $menu] &&
        [string compare "." [string index $menu 0]]} {
      set menu $data(w:canvas).vmenu-$menu
    }
  }

  if {"$menu" != ""} {
    tkIconPane::IntPopupPost $menu $X $Y
  }
}

proc tkIconPane::IntDragDropIcon {w} {
  if {"[winfo children $w]" == ""} {
    canvas $w.canvas
    pack $w.canvas
  }
  set parent [winfo parent $w]
  set class [winfo class $parent]
  upvar #0 [winfo parent [winfo parent $w]] data
  set tmpIcons ""
  set curIcon $data(currentId)
  set tmpIconsList ""
  if {[string compare "" $data(selectedIconList)] &&
      [lsearch $data(selectedIconList) $data(currentId)] != -1} {
    set tmpIcons $data(selectedIconList)
  } {
    if {[string compare "" $data(selectedIcon)] &&
        [lsearch $data(selectedIcon) $data(currentId)] != -1} {
      set tmpIcons $data(selectedIcon)
    } {
      if {[string compare "" $data(currentId)]} {
        set tmpIcons $data(currentId)
      }
    }
  }
  if {"$class" == "Canvas" && "$tmpIcons" != ""} {
    foreach option [$parent config] {
      if {[llength $option] == 5 &&
          [string compare "-confine" [lindex $option 0]] &&
          [string compare "-width" [lindex $option 0]] &&
          [string compare "-height" [lindex $option 0]] &&
          [string compare "-scrollregion" [lindex $option 0]] &&
          [string compare "-xscrollcommand" [lindex $option 0]] &&
          [string compare "-yscrollcommand" [lindex $option 0]] &&
          [string compare [lindex $option 3] [lindex $option 4]]} {
        $w.canvas config [lindex $option 0] [lindex $option 4]
      }
    }
    $w.canvas config -borderwidth 0
    set dragIcon [$w.canvas create icon 0 0]
    set bbox [$parent bbox [lindex $tmpIcons 0]]
    if {[llength $tmpIcons] > 1} {
      $w.canvas config \
        -height [expr [lindex $bbox 3] - [lindex $bbox 1]] \
        -width [expr [lindex $bbox 2] - [lindex $bbox 0] + 40]
      $w.canvas create text [expr [lindex $bbox 2] - [lindex $bbox 0] + 10] [expr ([lindex $bbox 3] - [lindex $bbox 1]) / 2] -text "..."
    } {
      $w.canvas config \
        -height [expr [lindex $bbox 3] - [lindex $bbox 1]] \
        -width [expr [lindex $bbox 2] - [lindex $bbox 0]]
    }
    foreach option [$parent itemconfig [lindex $tmpIcons 0]] {
      if {[llength $option] == 5 &&
          (![string compare "-background" [lindex $option 0]] ||
           ![string compare "-borderwidth" [lindex $option 0]] ||
           ![string compare "-font" [lindex $option 0]] ||
           ![string compare "-foreground" [lindex $option 0]] ||
           ![string compare "-foreground" [lindex $option 0]] ||
           ![string compare "-image" [lindex $option 0]] ||
           ![string compare "-relief" [lindex $option 0]] ||
           ![string compare "-state" [lindex $option 0]] ||
           ![string compare "-status" [lindex $option 0]] ||
           ![string compare "-textanchor" [lindex $option 0]] ||
           ![string compare "-text" [lindex $option 0]] ||
           ![string compare "-width" [lindex $option 0]])} {
        $w.canvas itemconfig $dragIcon [lindex $option 0] [lindex $option 4]
      }
    }
    if {"$curIcon" != ""} {
      set tmpIconsList $curIcon
      set tmpPos [lsearch $tmpIcons $curIcon]
      if {$tmpPos != -1} {
        set tmpIcons [lreplace $tmpIcons $tmpPos $tmpPos]
      }
    }
    if {[llength $tmpIcons] > 0} {
      eval lappend tmpIconsList $tmpIcons
    }
    return [list $parent $data(dragMode) $tmpIconsList]
  }
}

proc tkIconPane::IntHandleDragDrop {w type target} {
  global DragDrop

  upvar #0 $w data

  set targetId ""
  set dropCommand ""
  set tmpPos [tkIconPane::dropposition $data(w:canvas)]
  set tmpX [lindex $tmpPos 0]
  set tmpY [lindex $tmpPos 1]
  set targetId [$data(w:canvas) find closest $tmpX $tmpY]

  if {[string compare "" $targetId]} {
    set tmpBbox [$data(w:canvas) bbox $targetId]
    if {$tmpX < [lindex $tmpBbox 0] || $tmpX > [lindex $tmpBbox 2] ||
        $tmpY < [lindex $tmpBbox 1] || $tmpY > [lindex $tmpBbox 3]} {
      set targetId ""
    }
  }
  if {[string compare "" $targetId]} {
    if {![string compare "icon" [$data(w:canvas) type $targetId]]} {
      set dropCommand [lindex [$data(w:canvas) itemconfig $targetId -dropcommand] 4]
    } {
      set targetId ""
    }
  } {
    set dropCommand $data(-dropcommand)
  }

  if {[string compare "" $dropCommand]} {
    if {[catch "$dropCommand \{$DragDrop(iconInterpreter)\} \{$DragDrop(iconPath)\} \{$DragDrop(iconIds)\} \{$DragDrop(iconDragMode)\} \{$w\} \{$targetId\}" result]} {
      error $result
    }
  } {
    if {![string compare $data(w:canvas) $DragDrop(iconPath)] &&
        ![string compare "" $targetId] && $data(-placing) &&
        [string compare "" $DragDrop(iconIds)]} {
      set tmpPos [tkIconPane::dropposition $data(w:canvas)]
      set refX [lindex [$data(w:canvas) itemconfig [lindex $DragDrop(iconIds) 0] -x] 4]
      set refY [lindex [$data(w:canvas) itemconfig [lindex $DragDrop(iconIds) 0] -y] 4]
      set offsetX [expr [lindex $tmpPos 0] - $refX]
      set offsetY [expr [lindex $tmpPos 1] - $refY]
      foreach tmpIcon $DragDrop(iconIds) {
        set oldX [lindex [$data(w:canvas) itemconfig $tmpIcon -x] 4]
        set oldY [lindex [$data(w:canvas) itemconfig $tmpIcon -y] 4]
        $data(w:canvas) itemconfig $tmpIcon \
          -x [expr $oldX + $offsetX] -y [expr $oldY + $offsetY]
      }
      $data(w:canvas)-graph layout edges -edgeorder $data(-edgeorder) -graphorder $data(-graphorder)
    }
  }
}

proc tkIconPane::IntItemActivate {w x y} {
  upvar #0 $w data

  if {[string compare "" $data(selectedIconList)] &&
      [lsearch $data(selectedIconList) $data(currentId)] != -1} {
    foreach counter $data(selectedIconList) {
      set tmpCommand [lindex [$data(w:canvas) itemconfig $counter -command] 4]
      if {[string compare "" $tmpCommand]} {
        if {[catch "$tmpCommand $counter" result]} {
           puts stderr $result
        }
      }
    }
  } {
    set counter $data(currentId)
    if {![string compare [$data(w:canvas) type $counter] "icon"]} {
      set tmpCommand [lindex [$data(w:canvas) itemconfig $counter -command] 4]
      if {[string compare "" $tmpCommand]} {
        if {[catch "$tmpCommand $counter" result]} {
          puts stderr $result
        }
      }      
    }
  }
}

proc tkIconPane::IntItemDeSelect {w {canvasDesel 0}} {
  upvar #0 $w data

  if {!$canvasDesel || ![string compare "" $data(selectedId)]} {
    foreach counter $data(selectedIconList) {
      catch "$data(w:canvas) itemconfig $counter -state normal"
    }
    foreach counter $data(selectedEdgeList) {
      catch "$data(w:canvas) itemconfig $counter -state normal"
    }
    set data(selectedIconList) ""
    set data(selectedEdgeList) ""
  }
}

proc tkIconPane::IntItemMoveMove {w canvas x y X Y} {
  upvar #0 $w data

  if {[string compare "" $data(moveItems)] &&
      ($data(eventType) == "startmove3" ||
       $data(eventType) == "startmove4")} {
    blt_drag&drop drag $canvas $X $Y
  } elseif {$data(eventType) == "startmove2"} {
    set data(stretchX) [$data(w:canvas) canvasx $x]
    set data(stretchY) [$data(w:canvas) canvasy $y]
    $data(w:canvas) coords $data(stretch) $data(stretchRootX) $data(stretchRootY) $data(stretchX) $data(stretchY)
  } elseif {$data(eventType) == "startmove3"} {
    $data(w:canvas) scan dragto $x $y
  }
}

proc tkIconPane::IntItemMoveStart {w canvas x y X Y} {
  upvar #0 $w data

  if {[string compare "" $data(currentId)]} {
    if {"$data(eventType)" == "startmove3" ||
        "$data(eventType)" == "startmove4"} {
      set tmpIcons ""
      if {[string compare "" $data(selectedIconList)] &&
          [lsearch $data(selectedIconList) $data(currentId)] != -1} {
        set tmpIcons $data(selectedIconList)
      } {
        if {[string compare "" $data(selectedIcon)] &&
            [lsearch $data(selectedIcon) $data(currentId)] != -1} {
          set tmpIcons $data(selectedIcon)
        } {
          if {![string compare "icon" [$data(w:canvas) type $data(currentId)]]} {
            set tmpIcons $data(currentId)
          }
        }
      }
      if {"$tmpIcons" != ""} {
        if {"$data(eventType)" == "startmove4"} {
          set data(dragMode) "move"
          set data(hiddenIcons) ""
          foreach tmpIcon $tmpIcons {
            lappend data(hiddenIcons) [list $tmpIcon [lindex [$data(w:canvas) itemconfig $tmpIcon -x] 4] [lindex [$data(w:canvas) itemconfig $tmpIcon -y] 4]]
            $data(w:canvas) itemconfig $tmpIcon -x -1000 -y -1000
          }
        } {
          set data(dragMode) "copy"
        }
        blt_drag&drop drag $canvas $X $Y
        set data(moveItems) $tmpIcons
      }
    }
  } elseif {$data(eventType) == "startmove2"} {
    if {[string compare "" $data(stretch)]} {
      $data(w:canvas) delete $data(stretch)
    }
    set data(stretchRootX) [$data(w:canvas) canvasx $x]
    set data(stretchRootY) [$data(w:canvas) canvasy $y]
    set data(stretchX) [$data(w:canvas) canvasx $x]
    set data(stretchY) [$data(w:canvas) canvasy $y]
    set data(stretch) [$data(w:canvas) create rectangle $data(stretchRootX) $data(stretchRootY) $data(stretchX) $data(stretchY)]
  } elseif {$data(eventType) == "startmove3"} {
    $data(w:canvas) scan mark $x $y
  }
}

proc tkIconPane::IntItemMoveStop {w canvas x y X Y} {
  upvar #0 $w data

  set data(iconsMoved) 1
  if {[string compare "" $data(moveItems)] &&
      ($data(eventType) == "startmove3" ||
       $data(eventType) == "startmove4")} {
    if {"$data(eventType)" == "startmove4"} {
      foreach tmpIcon $data(hiddenIcons) {
        $data(w:canvas) itemconfig [lindex $tmpIcon 0] -x [lindex $tmpIcon 1] -y [lindex $tmpIcon 2]
      }
      set data(hiddenIcons) ""
    }
    blt_drag&drop drop $canvas $X $Y
    set data(moveItems) ""
  } elseif {[string compare "" $data(stretch)]} {
    if {$data(eventType) == "startmove2"} {
      foreach counter [$data(w:canvas) find enclosed $data(stretchRootX) $data(stretchRootY) $data(stretchX) $data(stretchY)] {
        if {![string compare "icon" [$data(w:canvas) type $counter]]} {
          catch "$data(w:canvas) itemconfig $counter -state selected"
          if {[lsearch $data(selectedIconList) $counter] == -1} {
            lappend data(selectedIconList) $counter
          }
        }
      }
    }
    $data(w:canvas) delete $data(stretch)
    set data(stretch) ""
  }
}

proc tkIconPane::IntItemSelect {w x y {keepSelection 0}} {
  upvar #0 $w data

  if {[string compare "" $data(selectedId)]} {
    set tmpState [lindex [$data(w:canvas) itemconfig $data(selectedId) -state] 4]
    if {![string compare "selected" $tmpState] && !$keepSelection} {
      catch "$data(w:canvas) itemconfig $data(selectedId) -state normal"
      if {![string compare "icon" [$data(w:canvas) type $data(selectedId)]]} {
        set tmpPosition [lsearch $data(selectedIconList) $data(selectedId)]
        if {$tmpPosition != -1} {
          set data(selectedIconList) [lreplace $data(selectedIconList) $tmpPosition $tmpPosition]
        }
      } {
        if {![string compare "edge" [$data(w:canvas) type $data(selectedId)]]} {
          set tmpPosition [lsearch $data(selectedEdgeList) $data(selectedId)]
          if {$tmpPosition != -1} {
            set data(selectedEdgeList) [lreplace $data(selectedEdgeList) $tmpPosition $tmpPosition]
          }
        }
      }
    } {
      if {![string compare "selected" [lindex [$data(w:canvas) itemconfig $data(selectedId) -state] 4]]} {
        catch "$data(w:canvas) itemconfig $data(selectedId) -state normal"
        if {![string compare "icon" [$data(w:canvas) type $data(selectedId)]]} {
          set tmpPosition [lsearch $data(selectedIconList) $data(selectedId)]
          if {$tmpPosition != -1} {
            set data(selectedIconList) [lreplace $data(selectedIconList) $tmpPosition $tmpPosition]
          }
        } {
          if {![string compare "edge" [$data(w:canvas) type $data(selectedId)]]} {
            set tmpPosition [lsearch $data(selectedEdgeList) $data(selectedId)]
            if {$tmpPosition != -1} {
              set data(selectedEdgeList) [lreplace $data(selectedEdgeList) $tmpPosition $tmpPosition]
            }
          }
        }
      } {
        catch "$data(w:canvas) itemconfig $data(selectedId) -state selected"
        if {![string compare "icon" [$data(w:canvas) type $data(selectedId)]]} {
          set tmpPosition [lsearch $data(selectedIconList) $data(selectedId)]
          if {$tmpPosition == -1} {
            lappend data(selectedIconList) $data(selectedId)
          }
        } {
          if {![string compare "edge" [$data(w:canvas) type $data(selectedId)]]} {
            set tmpPosition [lsearch $data(selectedEdgeList) $data(selectedId)]
            if {$tmpPosition == -1} {
              lappend data(selectedEdgeList) $data(selectedId)
            }
          }
        }
      }
    }
  } {
    tkIconPane::IntItemDeSelect $w
  }
  if {"$data(-selectcommand)" != ""} {
    eval $data(-selectcommand) \"$data(selectedIcon)\" \"$data(selectedEdge)\"
  }
}

proc tkIconPane::IntItemSetSelected {w {itemId ""}} {
  global tkIconPane

  upvar #0 $w data

  set tkIconPane(currentPane) $w
  if {![string compare "" $itemId]} {
    set itemId $data(currentId)
  }
  if {[string compare "" $itemId] &&
      ![string compare "icon" [$data(w:canvas) type $itemId]]} {
    set data(selectedEdge) ""
    set data(selectedIcon) $itemId
    set data(selectedId) $itemId
  } {
    if {[string compare "" $itemId] &&
        ![string compare "edge" [$data(w:canvas) type $itemId]]} {
      set data(selectedEdge) $itemId
      set data(selectedIcon) ""
      set data(selectedId) $itemId
    } {
      set data(selectedEdge) ""
      set data(selectedIcon) ""
      set data(selectedId) ""
    }
  }
}

proc tkIconPane::IntPopupPost {w x y} {
  global tkPriv

  if {![winfo exists $w.menubutton.menu]} {
    return
  }
  set menuWidth [winfo reqwidth $w.menubutton.menu]
  set width [winfo reqwidth $w]
  set height [winfo reqheight $w]
  wm geometry $w [format "%dx%d" [winfo reqwidth $w.menubutton.menu] $height]

  set wx $x
  set wy $y

  # trick: the following lines allow the popup menu
  # acquire a stable width and height when it is finally
  # put on the visible screen. Advoid flashing
  #
  wm geometry $w [format "+%d+%d" $wx $wy]
  wm deiconify $w
  raise $w

  update
  wm geometry $w $width\x$height+$wx+$wy
  update

  if {[grab current .] != ""} {
    grab release [grab current .]
  }

  set tkPriv(inMenubutton) $w.menubutton
  tkMbPost $w.menubutton
}

proc tkIconPane::IntUpdateMenu {{menuPath ""}} {
  if {[string compare "" $menuPath]} {
    set w [tkIconPane::info tkIconPane]
    upvar #0 $w data
    $menuPath conf -activebackground $data(-buttonactivebg)
    $menuPath conf -activeforeground $data(-buttonactivefg)
    $menuPath conf -background $data(-background)
    $menuPath conf -foreground $data(-foreground)
    $menuPath conf -font $data(-font)
    catch "[winfo parent $menuPath] conf -activebackground $data(-buttonactivebg)"
    catch "[winfo parent $menuPath] conf -activeforeground $data(-buttonactivefg)"
    catch "[winfo parent $menuPath] conf -background $data(-background)"
    catch "[winfo parent $menuPath] conf -foreground $data(-foreground)"
    catch "[winfo parent $menuPath] conf -font $data(-font)"
  }
  for {set counter 0} {$counter <= [$menuPath index last]} {incr counter} {
    if {![catch "$menuPath entryconf $counter -menu" result] &&
        "$result" != ""} {
      tkIconPane::IntUpdateMenu [lindex $result 4]
    }
  }
}

proc dd_send_icon {interpreter ddwin data} {
  set path [lindex $data 0]
  set dragMode [lindex $data 1]
  set ids [lindex $data 2]
  send $interpreter "
    global DragDrop
    set DragDrop(iconInterpreter) \{[winfo name .]\}
    set DragDrop(iconPath) \{$path\}
    set DragDrop(iconDragMode) \{$dragMode\}
    set DragDrop(iconIds) \{$ids\}"
  send $interpreter "blt_drag&drop target $ddwin handle icon"
}

# eof


