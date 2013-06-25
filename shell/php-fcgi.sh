#!/bin/bash
case $1 in
	start)
		spawn-fcgi -a 127.0.0.1 -p 9000 -C 1 -u huge -g huge -f /usr/bin/php5-cgi
		;;
	stop)
		pkill php5-cgi
		;;
	restart)
		pkill php5-cgi
		spawn-fcgi -a 127.0.0.1 -p 9000 -C 1 -u huge -g huge -f /usr/bin/php5-cgi
		;;
esac
