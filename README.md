The TkIcon extension

Copyright (c) 1995 by Sven Delmas
All rights reserved.
See the file COPYRIGHT for the copyright notes.

Changes:
--------
        - Tk 4.0 adaptions
        - dropped TkMegaWidget (use Tix instead)
        - Enhanced configuration
        - Some minor bug fixes


What is TkIcon ?
----------------
This small extension to Tk allows the creation of icon and
edge items in a canvas widget. These items provide a lot of
additional functionality (take a look at the manual pages
tkicon.n and tkiconpane.n). The TkIconPane widget is a
complete implementation of a desktop displayer as well as a
graph widget (including a layout mechanism). The
advantage of TkIcon is that a big part of the work is done
in C, which makes the implementation much faster.


Graph layout:
-------------
The graph layout is done with a simple tree layouter
(named tree), and an ISI layouter. The tree layouter was
originally based upon a Tcl algorithm published by Juergen
Wagner (J_Wagner@iao.fhg.de and gandalf@csli.stanford.edu) in
his "theObjects" package. The ISI layouter was implemented
following an explanation published by Anette Kjaer. If
someone has interesting other layout algorithms, please
let me know. It should not be a big problem to add
additional algorithms...


Writing new algorithms:
-----------------------
In case you want to embed additional algorithms, you can add
the code to tkCanvLayout.c. The new layout command is added
to the C function named TkCanvLayoutCmd. The new subcommand
is added where the dummy command "testing" is located. This
piece of code gives a small example of the embedding of new
algorithms. The function TkCanvLayoutBuildGraphToString is
used to build a string that represents the canvas graph
structure. This function returns a string following this
format:

  - {{NodeList} {EdgeList}}
    - NodeList -------> {Node Node Node ...}
      - Node ------------> {NodeCanvasId x y width height {ConnectionList}}
        - ConnectionList ---> {Connection Connection ...}
          - Connection --------> {FromCanvasId ToCanvasId EdgeCanvasId}
    - EdgeList -------> {Edge Edge Edge ...}
      - Edge ------------> {EdgeCanvasId FromCanvasId ToCanvasId}

Your new layout algorithm can parse this string to build up
his internal representation. The function
TkCanvLayoutGraphPlaceNodesFromString shows how to parse the
string, and is used to place the icons and edges according
to the computed layout. The procedure receives a string
following this format:

  - {{NodeList} {EdgeList}}
    - NodeList -------> {Node Node Node ...}
      - Node ------------> {NodeCanvasId x y width height}
    - EdgeList -------> {Edge Edge Edge ...}
      - Edge ------------> {EdgeCanvasId x1 y1 x2 y2 ....}


Requirements:
-------------
        - The Tk-4.0 package (or newer ?!)
        - The Tix package (for IconPane)

        - To use drag & drop, you have to build
          a wish that contains the BLT extension


Calling configure:
------------------
When you call ./configure in the temporary TkIcon
build directory (see below, several additional
parameters allow the selection of new packages etc..
Several packages allow the setting of additional 
subparameters. Setting one of these subparameters
means that all default settings for the subparameters
are ignored. So you have to list all required
subparameters by hand. Most parameters are directly
mapped to compiler options, so the configuration can
also be adapted by changing the Makefile directly. The
configuration reads the file "config.init", where the
several default settings are defined. The current
parameters are stored in the file "config.local". This
file can be used by calling ./configure with
-with-config-local.

IMPORTANT!!!!
If you call ./configure with new parameters (compared to
a previous call of ./configure), you should call "make
clean" before you build the new wish.

        -with-blt (default=off)
                This adds the BLT extension to the
                wish. Adding this feature makes it
                necessary to build and install the
                BLT package before you build this
                package. If there is no globally
                installed library, configure looks
                in the directory ../blt for a local
                copy.

        -with-config-local (default=off)
                Call ./configure with the parameters
                of the last ./configure call.

        -with-expect (default=off)
                This adds the expect extension to the
                wish. Adding this feature makes it
                necessary to build and install the
                expect package before you build this
                package. If there is no globally
                installed library, configure looks
                in the directory ../expect for a local
                copy.

        -with-itcl (default=off)
                This adds the itcl extension to the
                wish. Adding this feature makes it
                necessary to build and install the
                itcl package before you build this
                package. If there is no globally
                installed library, configure looks
                in the directory ../itcl for a local
                copy.

        -with-kanji (default=off)
                This allows the building of a Kanji
                wish. You have to provide the already
                patched Tk source tree under the name
                ../tk4.0-jp (or ../tk4.0jp) and the
                patched Tcl source tree under the name
                ../tcl7.4-jp (or ../tcl7.4jp).

        -with-nogcc (default=off)
                This disables the usage of gcc, irrespective
                of the existance of gcc.

        -with-noshlib (default=off)
                Even if the system supports shared
                libraries, don't build them.

        -with-tcldp (default=off)
                This adds the Tcl-DP package to the
                wish. Adding this feature makes it
                necessary to build and install the
                tcl-dp package before you build this
                package. If there is no globally
                installed library, configure looks
                in the directory ../tcl-dp for a local
                copy.

        -with-tclload (default=off)
                This adds the dynamic loading package to
                the wish. Adding this feature makes it
                necessary to build the shells package
                before you build this package. The shells
                package must be located in the same
                directory as the other extension packages
                under the name "shells".

        -with-tclX (default=off)
                This adds the TclX extension to the
                wish. Adding this feature makes it
                necessary to build and install the
                extended Tcl package before you build
                this package. If there is no globally
                installed library, configure looks
                in the directory ../tclX for a local
                copy.

        -with-tix (default=off)
                This adds the Tix extension to the
                wish. Adding this feature makes it
                necessary to build and install the
                Tix package before you build this
                package. If there is no globally
                installed library, configure looks
                in the directory ../tix for a local
                copy.

        -with-tkicon (default=on)
                This adds the TkIcon extension to
                the wish.

        -with-tklibrary=pathName (default=/usr/local/tk...)
                This specifies a new TK_LIBRARY path.
                The new Tk library searches for the
                Tcl library files in this directory.

        -with-tkmegawidget (default=on)
                This adds the TkMegaWidget extension to
                the wish. There are several 
                subparameters for this parameter:

                -with-tkmegawidget-des (default=off)
                        adds the DES encryption library
                        to the TkMegaWidget.
                -with-tkmegawidget-localdes (default=off)
                        causes the use of the local des
                        library. Otherwise, a globally 
                        installed des library is used, if
                        there is any.
                -with-tkmegawidget-debug (default=on)
                        adds the debugger code. This
                        means that it is possible to run
                        two separate interpreters where
                        one interpreter is used to debug
                        the second one.

        -with-tkmobal (default=off)
                This adds the TkMobal extension to
                the wish.

        -with-tksteal (default=off)
                This adds the TkSteal extension to
                the wish.

        -with-turndial (default=off)
                This adds the tkTurndial widget to the
                wish. Adding this feature makes it
                necessary to build and install the
                tkTurndial package before you build
                this package. If there is no globally
                installed library, configure looks
                in the directory ../tkTurndial for a
                local copy named "libtkTurndial.a".

        -with-xpm (default=off)
                This adds the Xpm3 library to the link
                command in case Tk was extended with the
                externally available Pixmap support. If
                there is no globally installed library,
                configure looks in the directory ../xpm
                for a local copy.


Building a patched wish:
------------------------
         1) Please take a look into the "Portability"
            section at the bottom first. The installation
            may require a special handling for your system.

         2) Unpack the Tk distribution. Build the Tk
            distribution as described in the Tk README. If
            your system supports shared libraries, and
            you want to use them, you should build the
            Tcl and the Tk distribution with the
            tcltk_shlib package published by Xiaokun Zhu.

         3) Unpack the TkMegaWidget distribution in the same
            directory where you have unpacked the Tk
            distribution. You don't have to build this
            package.

         4) Unpack the TkIcon distribution in the same
            directory where you have unpacked the Tk
            distribution.

         5) The resulting directory structure will look
            about like that:

            .../
               |
               +-- tcl7.4
               |
               +-- tk4.0
               |
               +-- TkIcon
               |
               +-- TkMegaWidget
               |
               ....

            Now change into the new TkIcon directory.

         6) Configure the distribution by running ./configure.
            And do all the things you usually do to get a
            working Tk. To select certain additional 
            packages and perform other changes of the new
            wish, you can call ./configure with several
            parameters. Take a look into the configure
            section above for a detailed list. Certain
            extensions have to be built and installed before
            you can use them. You should take care that the
            TK_LIBRARY path is valid. This means that you
            either use the path from a previous Tk
            installation, or you create the new path, and
            copy all Tk library files to that directory.

         7) A new libtk will be built, as there are some
            minor problems with certain packages (especially
            with tcl-DP). This libtk is totally compatible with
            the previous libtk (the static and the shared
            version). So, I decided to install the library
            under the same name. If you do not want this,
            change the TK_LIB_NAME in the toplevel Makefile
            (go to 9)).

         8) Build the new wish (to build a new wish with
            a manually adapted Makefile, go to 9)). This
            will copy the required files to the local
            directory. The required local extension
            libraries are built. Finally the new wish is
            linked. If your system supports shared
            libraries, they will be generated automatically.
            As the position of shared libraries is usually
            very critical, the libraries are automatically 
            installed in the final library directory when
            they are built. This makes it necessary that you 
            have write permission for that directory. To
            build the new wish, type:

                make

            Now go to 10) to continue the installation.

         9) If you want to change the toplevel Makefile
            manually (usually this is not required), first,
            you have to call:

                make prepare

            Adapt the Makefile to your personal needs. Now
            you can build the new wish (don't call "make"
            as you would normally do (see 8)), but call:

                make wish
        
        10) You can now install everything by typing this:

                make install

            This will install the new wish, the new library
            files and the new doc. By default, the library
            files are placed in the TK_LIBRARY directory.
            The original Tk tclIndex file in the directory 
            where the new Tk library files are installed
            (this is usually the Tk library directory... e.g.
            /usr/local/lib/tk4.0) has to be changed. It must
            contain the autoload code for the new Tcl commands
            ("tkIconPane"). If you do not add the autoload 
            code, you have to load the file "tkIconPaneCmd.tcl" 
            explicitly in your code. Please take care to keep 
            the Tk tclIndex file up to date when you install
            other packages. The tclIndex file must contain
            the following line:

                set auto_index(tkIconPane) "source [set dir]/tkIconPaneCmd.tcl"

        11) To see what the widgets can do for you, change to
            the directory named ./demo.TkIcon and type
            "../wish/icon-wish -f tkIconPane1.tcl" (do this for
            all tcl files in the directory).

        12) After that, you should test the new installed
            wish. If everything works fine, you can remove
            the temporary build directories.

        13) To prevent problems, please don't use this directory
            structure for totally different configurations. The
            subdirectories may contain newly copied files that
            are not removed automatically when you reconfigure
            the distribution. So you will end up in a totally
            corrupted distribution (especially the libtk directory
            will be corrupted, and will create libtk.a versions
            that cannot work). So, if you decide to reconfigure
            this package, and to drop certain packages from the
            new wish, please delete the current directorytree.
            Use a newly unpacked distribution to prevent trouble.


Portability:
------------
General:
        - If the widgets do not work as expected under the
          new wish, maybe you have forgotten to adapt the
          TK_LIBRARY path in the Makefile, or you have not
          created the new TK_LIBRARY path with all Tk
          library files.

        - If you have installed the Tcl/Tk related libraries
          in non standard directories (i.e. directories that
          are not automatically searched by the linker), you
          have to set the environment variable LD_LIBRARY_PATH
          to contain these additional directories.

OSF:
        - It seems that the command "sed" (which is used by GNU's
          configure package) accepts no long commandline strings
          (at least not long enough for this configure program).
          The error message you get will look about like this:

                "Too many commands, last: s%@srcdir@%.%g"

          Please install the GNU sed package, and use this "sed"
          command (make sure the new sed is found before the
          original sed).

Extended Tcl:
        - If you build the wish with extended Tcl, you have
          to set the TCL_LIBRARY path to the extended Tcl
          library path before you call the new wish.


Availability:
-------------
The TkIcon distribution file is named TkIcon4.0*.tar.gz or
TkIcon4.0*.tar.Z. The * represents the current version.
TkIcon is available via anonymous ftp at:
        ftp.aud.alcatel.com:        tcl/extensions/
        ftp.ibp.fr:                 pub/tcl/contrib/extensions/
        nic.funet.fi:               pub/languages/tcl/extensions/
        syd.dit.csiro.au:           pub/tk/contrib/extensions/
        ftp.cs.tu-berlin.de:        pub/tcl/contrib/extensions/

To get a copy of it, just type the following:

        ftp ftp.cs.tu-berlin.de
        bin
        cd pub/tcl/contrib/extensions
        get TkIcon4.0*.tar.gz

To get a copy via ftpmail, just send the following mail to:

        mail-server@cs.tu-berlin.de

To get a help message, send the following body:

        send HELP
        end

To retrieve TkIcon, send the following body:

        send pub/tcl/contrib/extensions/TkIcon4.0*.tar.gz
        end


Feedback (Author):
-----------------
Sven Delmas
sven@ignoranceisbliss.com


