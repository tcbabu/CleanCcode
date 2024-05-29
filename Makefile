all	:   cleancc

cleancc		: cleancc.c dlink.c dlink.h
		  cc -fPIC -o cleancc cleancc.c dlink.c
		  

clean		:
		 rm *.o  cleancc 

install		:  cleancc
		  cp cleancc /usr/bin/
