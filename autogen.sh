#! /bin/sh
#Taken from KDE project with conformance to FreeBSD binary layout!
# Global variables...

echo "***********************************************************"
echo "Make SURE you have installed automake and autoconf packages"
echo "***********************************************************"
sleep 1;

AUTOCONF="autoconf"
AUTOHEADER="autoheader"
AUTOM4TE="autom4te"
AUTOMAKE="automake"
ACLOCAL="aclocal"


# We don't use variable here for remembering the type ... strings.
# local variables are not that portable, but we fear namespace issues with
# our includer.  The repeated type calls are not that expensive.
checkAutoconf()
{
  if test -x "`$WHICH autoconf253`";then
   AUTOCONF="`$WHICH autoconf253`"
  elif test -x "`$WHICH autoconf257`" ; then	
    AUTOCONF="`$WHICH autoconf257`"
  elif test -x "`$WHICH autoconf258`" ; then
    AUTOCONF="`$WHICH autoconf258`"
  elif test -x "`$WHICH autoconf259`" ; then
    AUTOCONF="`$WHICH autoconf259`"
  elif test -x "`$WHICH autoconf-2.5x`" ; then
    AUTOCONF="`$WHICH autoconf-2.5x`"
  elif test -x "`$WHICH autoconf-2.53`" ; then
    AUTOCONF="`$WHICH autoconf-2.53`"
  elif test -x "`$WHICH autoconf-2.52`" ; then
    AUTOCONF="`$WHICH autoconf-2.52`"
  elif test -x "`$WHICH autoconf2.50`" ; then
    AUTOCONF="`$WHICH autoconf2.50`"
  elif test -x "`$WHICH autoconf`";then
   AUTOCONF="`$WHICH autoconf`"
  fi
}

checkAutoheader()
{
  if test -x "`$WHICH autoheader253`";then
   AUTOCONF="`$WHICH autoheader253`"
  elif test -x "`$WHICH autoheader258`" ; then
    AUTOCONF="`$WHICH autoheader258`"
  elif test -x "`$WHICH autoheader259`" ; then
    AUTOCONF="`$WHICH autoheader259`"
  elif test -x "`$WHICH autoheader-2.5x`" ; then
    AUTOHEADER="`$WHICH autoheader-2.5x`"
    AUTOM4TE="`$WHICH autom4te-2.5x`"
  elif test -x "`$WHICH autoheader-2.53`" ; then
    AUTOHEADER="`$WHICH autoheader-2.53`"
    AUTOM4TE="`$WHICH autom4te-2.53`"
  elif test -x "`$WHICH autoheader-2.52`" ; then
    AUTOHEADER="`$WHICH autoheader-2.52`"
  elif test -x "`$WHICH autoheader2.50`" ; then
    AUTOHEADER="`$WHICH autoheader2.50`"
  elif test -x "`$WHICH autoheader`" ; then
    AUTOHEADER="`$WHICH autoheader`"
  fi

}

checkAutomakeAclocal ()
{
  if test -z "$UNSERMAKE"; then
    if test -x "`$WHICH automake18`" ; then
      AUTOMAKE="`$WHICH automake18`"
      ACLOCAL="`$WHICH aclocal18`"
    elif test -x "`$WHICH automake-1.5`" ; then
      AUTOMAKE="`$WHICH automake-1.5`"
      ACLOCAL="`$WHICH aclocal-1.5`"
    elif test -x "`$WHICH automake-1.6`" ; then
      AUTOMAKE="`$WHICH automake-1.6`"
      ACLOCAL="`$WHICH aclocal-1.6`"
    elif test -x "`$WHICH automake`" ; then
      AUTOMAKE="`$WHICH automake`"
      ACLOCAL="`$WHICH aclocal`"
   fi
  else
     AUTOMAKE="$UNSERMAKE"
  fi
}

checkWhich ()
{
  WHICH=""
  for i in "type -p" "which" "type" ; do
    T=`$i sh`
    test -x "$T" && WHICH="$i" && break
  done
}

checkWhich
checkAutoconf
checkAutoheader
checkAutomakeAclocal

export WHICH AUTOHEADER AUTOCONF AUTOM4TE AUTOMAKE ACLOCAL
echo  "Execing $ACLOCAL"
$ACLOCAL || echo "Can not find aclocal on your system. Install it" 
# >/dev/null 2>/dev/null
echo "...Done"
echo  "Execing $AUTOMAKE"
$AUTOMAKE -ac  || echo "Can not find automake on your system. Install it"
# >/dev/null 2>/dev/null
echo "...Done"
echo  "Execing $AUTOCONF"
$AUTOCONF || echo "Can not find autoconf on your system. Install it"
#>/dev/null 2>/dev/null
echo "...Done"
