Instructions on how to build a Win32 Privoxy setup program
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

1) Check out the "winsetup" and "current" modules from the Privoxy CVS.
   A clean checkout is strongly recommended:
      mkdir mydisttemp
      cd mydisttemp
      cvs -d:extssh:userid@ijbswa.cvs.sourceforge.net:/cvsroot/ijbswa co .
2) Run Make:
      cd windist
      make
   If make fails with the following error:
      cp: cannot stat `/bin/mgwz.dll': No such file or directory
   then you need to install the mingw-zlib cygwin package.
3) Rename "privoxy_setup.exe" to "privoxy_setup_x_y_z.exe" where x,y,z are
   the current version/release levels.
4) Test and distribute "privoxy_setup_x_y_z.exe".

The final product is a single file which is a self-extracting archive and 
setup program.  Currently it's only 460kb, but YMMV.
