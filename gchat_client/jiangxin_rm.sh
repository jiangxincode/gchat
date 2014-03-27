echo "make distclean"
make distclean
echo "end"

echo "Start rm the rabbish"
rm aclocal.m4 config.h.in configure Makefile.in missing install-sh INSTALL depcomp COPYING
rm src/Makefile.in
rm po/Makefile.in.in
echo "end"

echo "Start rm the test"
rm -rf test/
echo "end"

echo "Start rm the autom4te.cache"
rm -rf autom4te.cache/
echo "end"


