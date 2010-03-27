const char _engine_cflags[] = "-ggdb -O0 -DDEBUGGINGAIDS -W -Wall -Wimplicit -Wno-char-subscripts -Wno-unused -funsigned-char -fno-strict-aliasing -DNO_GCC_BUILTINS -DKSFORBUILD -Iinclude  -DRENDERTYPESDL=1 -DUSE_A_C -DNOASM -I/inc -I/usr/local/include/SDL -D_GNU_SOURCE=1 -D_REENTRANT";
const char _engine_libs[] = " -L/usr/local/lib -Wl,-rpath,/usr/local/lib -lSDL -lpthread -lSDL_ttf -lSDL_mixer -lSDL_image  -lm";
const char _engine_uname[] = "Linux ubuntu-desktop 2.6.31-20-generic #57-Ubuntu SMP Mon Feb 8 09:05:19 UTC 2010 arm GNU/Linux";
const char _engine_compiler[] = "gcc 4.2.1 arm-none-linux-gnueabi";
const char _engine_date[] = __DATE__ " " __TIME__;
