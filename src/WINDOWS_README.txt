Steps to compile this project on Windows:

1. Download and install Microsoft Visual Studio Community Edition (free)
   with C++.
2. Change Connect_4\misc\shell.bat correctly so it reflects the absolute
   filepath of both your version of Visual Studio and the Connect_4\misc
   folder (instead of the paths as they are set on my computer).
   The Visual Studio part is telling Windows where to look to execute "cl"
   (MSVC's compiler) from the command line.
3. Download "SDL2-devel-2.0.3-VC.zip" from:
   http://libsdl.org/download-2.0.php
4. Unzip the above zip file into some folder (on my computer it's in
   w:\vs_dev_lib).
5. Go through the build.bat file and fix up the library and include paths
   to match those absolute paths on your system.
6. At the top of graphics.h change #include "SDL2/SDL.h" to just 
   #include "SDL.h"
7. phew...
