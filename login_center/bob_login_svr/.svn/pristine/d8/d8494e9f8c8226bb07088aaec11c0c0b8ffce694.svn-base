#!/usr/bin/expect  --


if { [llength $argv] != 6} {
	puts "usage: $argv0 pem ip port user passwd \"command \[params\]\""
	exit 1
}


set maxRetry 1
for {set retryNum 0} {$retryNum<$maxRetry} {incr retryNum} {

spawn  /usr/bin/ssh -i [lindex $argv 0] -q [lindex $argv 1] -l[lindex $argv 3] [lindex $argv 5]

set timeout 600
expect {

	"assword:" {

		send "[lindex $argv 4]\r"

		expect eof

		break

	}

 
	"yes/no)?" {
  
		send "yes\r"

		expect "assword:"

		send "[lindex $argv 4]\r"

		expect eof

		break

	}
  
	timeout {continue}
	eof {continue}

}
}

 


