# Non-obfuscated brainfuck interpreter in TCL.
# Runs the code in the variable K.

set K {++[->++[->++<]<]>>.[->++++++++<]>.[-]++++++++++.}

fconfigure stdout -encoding binary
fconfigure stdin -encoding binary
set p 0
set ip 0
set mem {0}
for {set i 0} {$i<0xffff} {incr i} {lappend mem 0}
lappend stack 0
while {$ip < [string length $K]} {
    switch -exact -- [string index $K $ip] {
	">" {incr p}
	"<" {set p [expr $p-1]}
	"," {set c [read stdin 1]; if {[string length $c]==1} {lset mem $p [scan $c %c]}}
	"." {puts -nonewline stdout [format %c [lindex $mem $p]]}
	"[" {
	    lappend stack [expr $ip - 1]
	    if {[lindex $mem $p] == 0} {
		set depth 1
		while {$depth != 0} {
		    incr ip
		    set c [string index $K [expr $ip+1]]
		    if {[string equal $c {[} ]} {
			incr depth
		    } elseif {[string equal $c "\]"]} {
			set depth [expr $depth-1]
		    }
		}
	    }
	}
	    "]" {if {[lindex $mem $p] != 0} {set ip [lindex $stack end]}; set stack [lrange $stack 0 end-1];}
	"+" {lset mem $p [expr ([lindex $mem $p] + 1)%256]}
	"-" {lset mem $p [expr ([lindex $mem $p] - 1)%256]}
	default {}
    }
    incr ip
}
