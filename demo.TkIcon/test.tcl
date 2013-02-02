scrollbar .s1 \
  -orient horizontal \
  -command ".c xview"
pack .s1 -side bottom -fill x
scrollbar .s2 \
  -orient vertical \
  -command ".c yview"
pack .s2 -side right -fill y
canvas .c \
  -scrollregion {0c 0c 100c 100c} \
  -confine false \
  -xscrollcommand ".s1 set" \
  -yscrollcommand ".s2 set"
pack .c -fill both -expand 1

puts "testing: graph handling"

if {"[tkgraph names]" != ""} {
  puts "Error1: [tkgraph names]"
}

tkgraph create test1
tkgraph create test2
if {"[tkgraph names]" != "test1 test2"} {
  puts "Error2: [tkgraph names]"
}

if {![catch "tkgraph create lappend" res] || "$res" != "tkgraph error: the command: \"lappend\" already exists"} {
  puts "Error3: $res"
}

if {![catch "test1 iconinfo ids" res] || "$res" != "\"test1\": this graph was not attached to a canvas widget"} {
  puts "Error4: $res"
}

puts "testing: icon minor commands"

puts [.c create icon 60 60 -graphname name1 -text text1 -name icon1 -edges {{icon1 icon2 edge3}}]
puts [.c create icon 100 90 -graphname name1 -text text2 -name icon2 -edges {{icon1 icon2 edge3}}]
puts [.c create icon 140 160 -graphname name1 -text text3 -name icon3]

if {"[name1 iconinfo ids]" != "1 2 3"} {
  puts "Error1: [name1 iconinfo ids]"
}

if {"[name1 iconinfo names]" != "icon1 icon2 icon3"} {
  puts "Error2: [name1 iconinfo names]"
}

if {"[name1 iconinfo id icon1]" != "1"} {
  puts "Error3: [name1 iconinfo id icon1]"
}

puts "testing: edge minor commands"

puts [.c create edge 10 10 20 30 -graphname name1 -name edge1 -label edge1]
puts [.c create edge 10 10 20 30 -graphname name1 -name edge2 -label edge2]
puts [.c create edge 10 10 20 30 -graphname name1 -name edge3 -label edge3 -from 1 -to 2]

if {"[name1 edgeinfo ids]" != "4 5 6"} {
  puts "Error1: [name1 edgeinfo ids]"
}

if {"[name1 edgeinfo names]" != "edge1 edge2 edge3"} {
  puts "Error2: [name1 edgeinfo names]"
}

if {"[name1 edgeinfo graphs]" != "name1"} {
  puts "Error3: [name1 edgeinfo graphs]"
}

if {"[name1 edgeinfo ids name1]" != "4 5 6"} {
  puts "Error4: [name1 edgeinfo ids name1]"
}

if {"[name1 edgeinfo names name1]" != "edge1 edge2 edge3"} {
  puts "Error5: [name1 edgeinfo names name1]"
}

if {"[name1 edgeinfo id name1 icon1 icon2]" != "edge3"} {
  puts "Error6: [name1 edgeinfo id name1 icon1 icon2]"
}

puts "testing: layouting"

name1 layout matrix -xoffset 50 -yoffset 90

puts "testing: image handling"
image create bitmap test -file test.bmp
image create bitmap tcl -file tcl.bmp
.c create image 60 60 -image tcl
.c create icon 160 60 -graphname name1 -borderwidth 2 -text text4 -name icon4 -image test
.c create icon 100 90 -graphname name1 -borderwidth 2 -text text5 -name icon5 -image test
.c create icon 60 60 -graphname name1 -textanchor ne -state selected -relief raised -borderwidth 2 -text text6 -name icon6 -image test

# eof


