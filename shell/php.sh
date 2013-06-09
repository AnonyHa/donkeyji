#!/bin/bash
spawn-fcgi -a 127.0.0.1 -p 9000 -C 1 -u huge -g huge -f /usr/bin/php5-cgi
