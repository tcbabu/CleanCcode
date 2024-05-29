all	:   cleancc tarball

cleancc		: cleancc.c dlink.c dlink.h
		  cc -fPIC -o cleancc cleancc.c dlink.c

tarball		: cleancc
		  tar czf cleanccbin.tgz cleancc
		  

clean		:
		 rm   cleancc cleanccbin.tgz

install		:  cleancc
		  cp cleancc /usr/bin/
