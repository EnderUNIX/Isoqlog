bin_PROGRAMS = isoqlog


isoqlog_SOURCES = Store.c Parser.c main.c loadconfig.c LangCfg.c Html.c Dir.c Data.c 
isoqlog_LDADD   = 

SUBDIRS = lang htmltemp 

EXTRA_DIST = Data.c Dir.c Html.c LangCfg.c loadconfig.c main.c Parser.c Store.c Data.h Dir.h Html.h Global.h LangCfg.h loadconfig.h Parser.h Store.h isoqlog.conf-dist isoqlog.conf.sample1 isoqlog.conf.sample2 isoqlog.conf.sample3 isoqlog.domains-dist 

install-data-local:
	$(mkinstalldirs) /usr/local/etc/
	$(INSTALL_DATA) $(srcdir)/isoqlog.conf-dist /usr/local/etc/isoqlog.conf-dist
	$(mkinstalldirs) /usr/local/etc/
	$(INSTALL_DATA) $(srcdir)/isoqlog.domains-dist /usr/local/etc/isoqlog.domains-dist

uninstall-local:
	-rm -f /usr/local/etc/isoqlog.conf-dist
	-rm -f /usr/local/etc/isoqlog.domains-dist

# set the include path found by configure
INCLUDES= $(all_includes)

# the library search path.
isoqlog_LDFLAGS = $(all_libraries) 
                                                                                
