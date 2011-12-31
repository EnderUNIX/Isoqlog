
SUBDIRS = images library 

EXTRA_DIST = daily.html domain.html generaldaily.html generaldomain.html generalmonthly.html generalyearly.html index.html monthly.html yearly.html

install-data-local:
	$(mkinstalldirs) /usr/local/share/isoqlog/htmltemp/
	$(INSTALL_DATA) $(srcdir)/daily.html /usr/local/share/isoqlog/htmltemp/daily.html
	$(mkinstalldirs) /usr/local/share/isoqlog/htmltemp/
	$(INSTALL_DATA) $(srcdir)/domain.html /usr/local/share/isoqlog/htmltemp/domain.html
	$(mkinstalldirs) /usr/local/share/isoqlog/htmltemp/
	$(INSTALL_DATA) $(srcdir)/generaldaily.html /usr/local/share/isoqlog/htmltemp/generaldaily.html
	$(mkinstalldirs) /usr/local/share/isoqlog/htmltemp/
	$(INSTALL_DATA) $(srcdir)/yearly.html /usr/local/share/isoqlog/htmltemp/yearly.html
	$(mkinstalldirs) /usr/local/share/isoqlog/htmltemp/
	$(INSTALL_DATA) $(srcdir)/generaldomain.html /usr/local/share/isoqlog/htmltemp/generaldomain.html
	$(mkinstalldirs) /usr/local/share/isoqlog/htmltemp/
	$(INSTALL_DATA) $(srcdir)/generalmonthly.html /usr/local/share/isoqlog/htmltemp/generalmonthly.html
	$(mkinstalldirs) /usr/local/share/isoqlog/htmltemp/
	$(INSTALL_DATA) $(srcdir)/generalyearly.html /usr/local/share/isoqlog/htmltemp/generalyearly.html
	$(mkinstalldirs) /usr/local/share/isoqlog/htmltemp/
	$(INSTALL_DATA) $(srcdir)/index.html /usr/local/share/isoqlog/htmltemp/index.html
	$(mkinstalldirs) /usr/local/share/isoqlog/htmltemp/
	$(INSTALL_DATA) $(srcdir)/monthly.html /usr/local/share/isoqlog/htmltemp/monthly.html

uninstall-local:
	-rm -f /usr/local/share/isoqlog/htmltemp/daily.html
	-rm -f /usr/local/share/isoqlog/htmltemp/days.html
	-rm -f /usr/local/share/isoqlog/htmltemp/domain.html
	-rm -f /usr/local/share/isoqlog/htmltemp/generaldaily.html
	-rm -f /usr/local/share/isoqlog/htmltemp/generaldomain.html
	-rm -f /usr/local/share/isoqlog/htmltemp/generalmonthly.html
	-rm -f /usr/local/share/isoqlog/htmltemp/generalyearly.html
	-rm -f /usr/local/share/isoqlog/htmltemp/index.html
	-rm -f /usr/local/share/isoqlog/htmltemp/monthly.html

