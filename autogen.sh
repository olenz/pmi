#! /bin/sh -x

rm -f config.cache acconfig.hpp

aclocal -I ../config			&& \
autoheader				&& \
automake --copy --foreign --add-missing	&& \
autoconf				&& \
exit 0

exit 1

