# -*- mode: autoconf -*-
# Configure macros for BES
#
# Based on bes.m4 by Patrice Dumas, et al.

# AC_CHECK_BES([MINIMUM-VERSION [, ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]]])
# Test for BES and define BES_CFLAGS and BES_LIBS.
# Check that the version is above MINIMUM-VERSION 
# use when linking with a c++ aware linker, with a c linker you may also
# need -lstdc++

AC_DEFUN([AC_CHECK_BES],
[
  AC_PATH_PROG([BES_CONFIG], [bes-config], [no])
  bes_min_version=m4_if([$1], [], [3.1.0], [$1])
  AC_MSG_CHECKING([for bes version >= $bes_min_version])
  bes_no=""
  if test "$BES_CONFIG" = "no" ; then
     bes_no=yes
  else
     bes_config_major_version=`$BES_CONFIG --version | sed 's/^bes \([[0-9]]\)*\.\([[0-9]]*\)\.\([[0-9]]*\)$/\1/'`
     bes_config_minor_version=`$BES_CONFIG --version | sed 's/^bes \([[0-9]]\)*\.\([[0-9]]*\)\.\([[0-9]]*\)$/\2/'`
     bes_config_micro_version=`$BES_CONFIG --version | sed 's/^bes \([[0-9]]\)*\.\([[0-9]]*\)\.\([[0-9]]*\)$/\2/'`
     bes_min_major_version=`echo $bes_min_version | sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\1/'`
     bes_min_minor_version=`echo $bes_min_version | sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\2/'`
     bes_min_micro_version=`echo $bes_min_version | sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\3/'`

     bes_config_is_lt=""
     if test $bes_config_major_version -lt $bes_min_major_version ; then
       bes_config_is_lt=yes
     else
       if test $bes_config_major_version -eq $bes_min_major_version ; then
         if test $bes_config_minor_version -lt $bes_min_minor_version ; then
           bes_config_is_lt=yes
         else
           if test $bes_config_minor_version -eq $bes_min_minor_version ; then
             if test $bes_config_micro_version -lt $bes_min_micro_version ; then
               bes_config_is_lt=yes
             fi
           fi
         fi
       fi
     fi
     if test x$bes_config_is_lt = xyes ; then
       bes_no=yes
     else
       BES_LIBS="`$BES_CONFIG --dap-libs`"
       BES_CPPFLAGS="`$BES_CONFIG --cflags`"
       BES_MODULE_DIR="`$BES_CONFIG --modulesdir`"
       BES_HELP_DIR="`$BES_CONFIG --helpdir`"
     fi
   fi
   if test x$bes_no = x ; then
     AC_MSG_RESULT([yes])
     m4_if([$2], [], [:], [$2])
   else
     AC_MSG_RESULT([no])
     if test "$BES_CONFIG" = "no" ; then
     AC_MSG_NOTICE([The bes-config script could not be found.])
     else
       if test x$bes_config_is_lt = xyes ; then
         AC_MSG_NOTICE([the installed bes is too old.])
       fi
     fi
     BES_LIBS=""
     BES_CPPFLAGS=""
     BES_MODULE_DIR=""
     BES_HELP_DIR=""
     m4_if([$3], [], [:], [$3])
   fi
   AC_SUBST([BES_LIBS])
   AC_SUBST([BES_CPPFLAGS])
   AC_SUBST([BES_MODULE_DIR])
   AC_SUBST([BES_HELP_DIR])
]) 
