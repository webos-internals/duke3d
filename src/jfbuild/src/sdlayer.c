// SDL interface layer
// for the Build Engine
// by Jonathon Fowler (jonof@edgenetwk.com)
//
// Use SDL1.2 from http://www.libsdl.org

#include <stdlib.h>
#include <math.h>
#include "SDL.h"
#include "compat.h"
#include "sdlayer.h"
#include "cache1d.h"
#include "pragmas.h"
#include "a.h"
#include "build.h"
#include "osd.h"

// from keyboard.h from jfduke3d/jmact/
#define KB_KeyDown keystatus
#define  sc_None         0
#define  sc_Bad          0xff
#define  sc_Comma        0x33
#define  sc_Period       0x34
#define  sc_Return       0x1c
#define  sc_Enter        sc_Return
#define  sc_Escape       0x01
#define  sc_Space        0x39
#define  sc_BackSpace    0x0e
#define  sc_Tab          0x0f
#define  sc_LeftAlt      0x38
#define  sc_LeftControl  0x1d
#define  sc_CapsLock     0x3a
#define  sc_LeftShift    0x2a
#define  sc_RightShift   0x36
#define  sc_F1           0x3b
#define  sc_F2           0x3c
#define  sc_F3           0x3d
#define  sc_F4           0x3e
#define  sc_F5           0x3f
#define  sc_F6           0x40
#define  sc_F7           0x41
#define  sc_F8           0x42
#define  sc_F9           0x43
#define  sc_F10          0x44
#define  sc_F11          0x57
#define  sc_F12          0x58
#define  sc_Kpad_Star    0x37
#define  sc_Pause        0x59
#define  sc_ScrollLock   0x46
#define  sc_NumLock      0x45
#define  sc_Slash        0x35
#define  sc_SemiColon    0x27
#define  sc_Quote        0x28
#define  sc_Tilde        0x29
#define  sc_BackSlash    0x2b

#define  sc_OpenBracket  0x1a
#define  sc_CloseBracket 0x1b

#define  sc_1            0x02
#define  sc_2            0x03
#define  sc_3            0x04
#define  sc_4            0x05
#define  sc_5            0x06
#define  sc_6            0x07
#define  sc_7            0x08
#define  sc_8            0x09
#define  sc_9            0x0a
#define  sc_0            0x0b
#define  sc_Minus        0x0c
#define  sc_Equals       0x0d
#define  sc_Plus         0x0d

#define  sc_kpad_1       0x4f
#define  sc_kpad_2       0x50
#define  sc_kpad_3       0x51
#define  sc_kpad_4       0x4b
#define  sc_kpad_5       0x4c
#define  sc_kpad_6       0x4d
#define  sc_kpad_7       0x47
#define  sc_kpad_8       0x48
#define  sc_kpad_9       0x49
#define  sc_kpad_0       0x52
#define  sc_kpad_Minus   0x4a
#define  sc_kpad_Plus    0x4e
#define  sc_kpad_Period  0x53

#define  sc_A            0x1e
#define  sc_B            0x30
#define  sc_C            0x2e
#define  sc_D            0x20
#define  sc_E            0x12
#define  sc_F            0x21
#define  sc_G            0x22
#define  sc_H            0x23
#define  sc_I            0x17
#define  sc_J            0x24
#define  sc_K            0x25
#define  sc_L            0x26
#define  sc_M            0x32
#define  sc_N            0x31
#define  sc_O            0x18
#define  sc_P            0x19
#define  sc_Q            0x10
#define  sc_R            0x13
#define  sc_S            0x1f
#define  sc_T            0x14
#define  sc_U            0x16
#define  sc_V            0x2f
#define  sc_W            0x11
#define  sc_X            0x2d
#define  sc_Y            0x15
#define  sc_Z            0x2c

// Extended scan codes

#define  sc_UpArrow      0xc8 //0x5a
#define  sc_DownArrow    0xd0 //0x6a
#define  sc_LeftArrow    0xcb //0x6b
#define  sc_RightArrow   0xcd //0x6c
#define  sc_Insert       0xd2 //0x5e
#define  sc_Delete       0xd3 //0x5f
#define  sc_Home         0xc7 //0x61
#define  sc_End          0xcf //0x62
#define  sc_PgUp         0xc9 //0x63
#define  sc_PgDn         0xd1 //0x64
#define  sc_RightAlt     0xb8 //0x65
#define  sc_RightControl 0x9d //0x66
#define  sc_kpad_Slash   0xb5 //0x67
#define  sc_kpad_Enter   0x9c //0x68
#define  sc_PrintScreen  0xb7 //0x69
#define  sc_LastScanCode 0x6e


#define    VIDHEIGHT    240
#define    VIDWIDTH     320
#define    FIRE_SIZE    160
#define    JUMP_SIZE    120
#define    JOY_SIZE     160
#define    JOY_DEAD     10
#define    JOY_X        80
#define    JOY_Y        (VIDHEIGHT - 80)

#define    JOY_IMAGE_FILENAME        "images/joystick.png"
#define    JOY_PRESS_IMAGE_FILENAME  "images/joystick-press.png"
#define    JUMP_IMAGE_FILENAME       "images/jump.png"
#define    FIRE_IMAGE_FILENAME       "images/fire.png"

#define    OVERLAY_ITEM_COUNT   4
#define    OVERLAY_ALPHA       32

int    autofire = 0;
int    mousedown = 0;
int    normalkeyboard = 0;
int    drawoverlay = 1;
int    gesturedown = 0;
int    in_impulse = 0;

//Hacky way to show the 'jumping' overlay item
int     jumping_counter = 0;
#define JUMP_FRAME_COUNT 6

int     fire_counter = 0;
#define FIRE_FRAME_COUNT 1

static SDL_Surface *buffer = NULL;
static SDL_Surface *screen = NULL;

static int joy_x, joy_y;

// No support for option menus
void (*vid_menudrawfn)(void) = NULL;
void (*vid_menukeyfn)(int key) = NULL;

// I don't have the header for this...
SDL_Surface * IMG_Load( char * filename );

static SDL_Surface * joy_img = NULL;
static SDL_Surface * joy_press_img = NULL;
static SDL_Surface * jump_img = NULL;
static SDL_Surface * fire_img = NULL;

static SDL_Rect old_rects[OVERLAY_ITEM_COUNT];


#ifdef USE_OPENGL
#include "glbuild.h"
#endif

#define SURFACE_FLAGS	(SDL_SWSURFACE|SDL_HWPALETTE|SDL_HWACCEL)

// undefine to restrict windowed resolutions to conventional sizes
#define ANY_WINDOWED_SIZE

int   _buildargc = 1;
char **_buildargv = NULL;
extern long app_main(long argc, char *argv[]);

static char gamma_saved = 0, gamma_supported = 0;
static unsigned short sysgamma[3*256];

char quitevent=0, appactive=1;

// video
static SDL_Surface *sdl_surface;
long xres=-1, yres=-1, bpp=0, fullscreen=0, bytesperline, imageSize;
long frameplace=0, lockcount=0;
char modechange=1;
char offscreenrendering=0;
char videomodereset = 0;
char nofog=0;

#ifdef USE_OPENGL
// OpenGL stuff
#endif
static char nogl=0;

// input
char inputdevices=0;
char keystatus[256], keyfifo[KEYFIFOSIZ], keyfifoplc, keyfifoend;
unsigned char keyasciififo[KEYFIFOSIZ], keyasciififoplc, keyasciififoend;
static unsigned char keynames[256][24];
long mousex=0,mousey=0,mouseb=0;
long *joyaxis = NULL, joyb=0, *joyhat = NULL;
char joyisgamepad=0, joynumaxes=0, joynumbuttons=0, joynumhats=0;
long joyaxespresent=0;


void (*keypresscallback)(long,long) = 0;
void (*mousepresscallback)(long,long) = 0;
void (*joypresscallback)(long,long) = 0;

// generated using misc/makesdlkeytrans.c
static unsigned char keytranslation[SDLK_LAST] = {
	0, 0, 0, 0, 0, 0, 0, 0, 14, 15, 0, 0, 0, 28, 0, 0, 0, 0, 0, 89, 0, 0, 0, 
	0, 0, 0, 0, 1, 0, 0, 0, 0, 57, 2, 40, 4, 5, 6, 8, 40, 10, 11, 9, 13, 51,
	12, 52, 53, 11, 2, 3, 4, 5, 6, 7, 8, 9, 10, 39, 39, 51, 13, 52, 53, 3, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	26, 43, 27, 7, 12, 41, 30, 48, 46, 32, 18, 33, 34, 35, 23, 36, 0xd0/*37*/, 0xc8/*38*/, 0xcd/*50*/,
	49, 24, 25, 1/*16*/, 19, 31, 20, 22, 47, 0xcb/*17*/, 45, 21, 44, 0, 0, 0, 0, 211, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 82, 79, 80, 81, 75, 76, 77, 71, 72, 73, 83, 181, 55, 74, 78, 156, 0,
	200, 208, 205, 203, 210, 199, 207, 201, 209, 59, 60, 61, 62, 63, 64, 65,
	66, 67, 68, 87, 88, 0, 0, 0, 0, 0, 0, 69, 58, 70, 54, 42, 0x39/*157*/, 0x39/*29*/, 184, 56,
	0, 0, 219, 220, 0, 0, 0, /*-2*/0, 84, 183, 221, 0, 0, 0
};

//static SDL_Surface * loadtarga(const char *fn);		// for loading the icon
static SDL_Surface * loadappicon(void);

#ifdef HAVE_GTK2
#include <gtk/gtk.h>
static int gtkenabled = 0;

void create_startwin(void);
void settitle_startwin(const char *title);
void puts_startwin(const char *str);
void close_startwin(void);
void update_startwin(void);
#endif

int wm_msgbox(char *name, char *fmt, ...)
{
	char buf[1000];
	va_list va;

	va_start(va,fmt);
	vsprintf(buf,fmt,va);
	va_end(va);

#ifdef HAVE_GTK2
	if (gtkenabled) {
		GtkWidget *dialog;

		dialog = gtk_message_dialog_new(NULL,
				GTK_DIALOG_DESTROY_WITH_PARENT,
				GTK_MESSAGE_INFO,
				GTK_BUTTONS_OK,
				buf);
		gtk_window_set_title(GTK_WINDOW(dialog), name);
		gtk_dialog_run(GTK_DIALOG(dialog));
		gtk_widget_destroy(dialog);
	} else
#endif
	{
		puts(buf);
		getchar();
	}
	return 0;
}

int wm_ynbox(char *name, char *fmt, ...)
{
	char buf[1000];
	va_list va;
	int r;

	va_start(va,fmt);
	vsprintf(buf,fmt,va);
	va_end(va);

#ifdef HAVE_GTK2
	if (gtkenabled) {
		GtkWidget *dialog;

		dialog = gtk_message_dialog_new(NULL,
				GTK_DIALOG_DESTROY_WITH_PARENT,
				GTK_MESSAGE_INFO,
				GTK_BUTTONS_YES_NO,
				buf);
		gtk_window_set_title(GTK_WINDOW(dialog), name);
		r = gtk_dialog_run(GTK_DIALOG(dialog));
		gtk_widget_destroy(dialog);
		if (r == GTK_RESPONSE_YES) return 1;
	} else
#endif
	{
		char c;
		puts(buf);
		do c = getchar(); while (c != 'Y' && c != 'y' && c != 'N' && c != 'n');
		if (c == 'Y' || c == 'y') return 1;
	}

	return 0;
}

void wm_setapptitle(char *name)
{
	if (name) {
		Bstrncpy(apptitle, name, sizeof(apptitle)-1);
		apptitle[ sizeof(apptitle)-1 ] = 0;
	}

	SDL_WM_SetCaption(apptitle, NULL);

#ifdef HAVE_GTK2
	if (gtkenabled) {
		settitle_startwin(apptitle);
	}
#endif
}


static int translateKeyWebOS(int sym, int state) {
	int modstate = SDL_GetModState();

	//Weapon cycling!
	if (state) {
		if (sym == 27)
		{
			//gesture down
			return sc_OpenBracket;
		} else if (sym == 229) {
			//gesture up
			return sc_CloseBracket;
		}
	}

	switch(sym)
	{
	   case SDLK_DELETE: sym = sc_Delete; break;
	   case SDLK_BACKSPACE: sym = sc_BackSpace; break;
	   case SDLK_F1: sym = sc_F1; break;
	   case SDLK_F2: sym = sc_F2; break;
	   case SDLK_F3: sym = sc_F3; break;
	   case SDLK_F4: sym = sc_F4; break;
	   case SDLK_F5: sym = sc_F5; break;
	   case SDLK_F6: sym = sc_F6; break;
	   case SDLK_F7: sym = sc_F7; break;
	   case SDLK_F8: sym = sc_F8; break;
	   case SDLK_F9: sym = sc_F9; break;
	   case SDLK_F10: sym = sc_F10; break;
	   case SDLK_F11: sym = sc_F11; break;
	   case SDLK_F12: sym = sc_F12; break;
	   case SDLK_BREAK:
	   case SDLK_PAUSE: sym = sc_Pause; break;
	   case SDLK_UP: sym = sc_UpArrow; break;
	   case SDLK_DOWN: sym = sc_DownArrow; break;
	   case SDLK_RIGHT: sym = sc_RightArrow; break;
	   case SDLK_LEFT: sym = sc_LeftArrow; break;
	   case SDLK_INSERT: sym = sc_Insert; break;
	   case SDLK_HOME: sym = sc_Home; break;
	   case SDLK_END: sym = sc_End; break;
	   case SDLK_PAGEUP: sym = sc_PgUp; break;
	   case SDLK_PAGEDOWN: sym = sc_PgDn; break;
	   case SDLK_RSHIFT: sym = sc_RightShift; break;
	   case SDLK_LSHIFT: sym = sc_LeftShift; break;
	   case SDLK_RCTRL: sym = sc_RightControl; break;
	   case SDLK_LCTRL: sym = sc_LeftControl; break;
	   case SDLK_RALT: sym = sc_RightAlt; break;
	   case SDLK_LALT: sym = sc_LeftAlt; break;
	   case SDLK_KP0: 
		   if(modstate & KMOD_NUM) sym = sc_Insert; 
		   else sym = sc_0;
		   break;
	   case SDLK_KP1:
		   if(modstate & KMOD_NUM) sym = sc_End;
		   else sym = sc_1;
		   break;
	   case SDLK_KP2:
		   if(modstate & KMOD_NUM) sym = sc_DownArrow;
		   else sym = sc_2;
		   break;
	   case SDLK_KP3:
		   if(modstate & KMOD_NUM) sym = sc_PgDn;
		   else sym = sc_3;
		   break;
	   case SDLK_KP4:
		   if(modstate & KMOD_NUM) sym = sc_LeftArrow;
		   else sym = sc_4;
		   break;
	   case SDLK_KP5: sym = sc_5; break;
	   case SDLK_KP6:
		   if(modstate & KMOD_NUM) sym = sc_RightArrow;
		   else sym = sc_6;
		   break;
	   case SDLK_KP7:
		   if(modstate & KMOD_NUM) sym = sc_Home;
		   else sym = sc_7;
		   break;
	   case SDLK_KP8:
		   if(modstate & KMOD_NUM) sym = sc_UpArrow;
		   else sym = sc_8;
		   break;
	   case SDLK_KP9:
		   if(modstate & KMOD_NUM) sym = sc_PgUp;
		   else sym = sc_9;
		   break;
	   case SDLK_KP_PERIOD:
		   if(modstate & KMOD_NUM) sym = sc_Delete;
		   else sym = sc_Period;
		   break;
	   case SDLK_KP_DIVIDE: sym = sc_Slash; break;
	   case SDLK_KP_MULTIPLY: sym = SDLK_ASTERISK; break;
	   case SDLK_KP_MINUS: sym = sc_Minus; break;
	   case SDLK_KP_PLUS: sym = sc_Plus; break;
	   case SDLK_KP_ENTER: sym = sc_Return; break;
	   case SDLK_KP_EQUALS: sym = sc_Equals; break;
	}
	// If we're not directly handled and still above 255
	// just force it to 0
	if(sym > 255) sym = 0;

	//XXX: This is a terrible hack
	// because for some reason
	// parsing configs doesn't work?

	//sym+i is tilde
	if ( sym == 37 ) sym = sc_Tilde;

	if ( !normalkeyboard )
	{
		//these values are from keys.h
		if ( sym == SDLK_j ) sym = sc_RightControl;//fire!
		if ( sym == SDLK_b ) sym = sc_A;//jump!
		//if ( sym == SDLK_j ) sym = K_UPARROW;//forward!
		//if ( sym == SDLK_b ) sym = K_DOWNARROW;//back
		if ( sym == SDLK_h ) sym = sc_U;//strafeleft
		if ( sym == SDLK_n ) sym = sc_N;//straferight

		//same, only sprint versions
		/*
		if ( sym == SDLK_i ) 
		{
			Key_Event( K_SHIFT, state );
			Key_Event( K_UPARROW, state );
			Key_Event( K_SHIFT, state );
			sym = 0;
		}
		if ( sym == SDLK_u ) 
		{
			Key_Event( K_SHIFT, state );
			Key_Event( 44, state );
			Key_Event( K_SHIFT, state );
			sym = 0;
		}
		if ( sym == SDLK_k ) 
		{
			Key_Event( K_SHIFT, state );
			Key_Event( 46, state );
			Key_Event( K_SHIFT, state );
			sym = 0;
		}
		*/

		//remap the numbers  to the weapons, so no orange needed
		if ( sym == SDLK_e ) sym = sc_1;
		if ( sym == SDLK_r ) sym = sc_2;
		if ( sym == SDLK_t ) sym = sc_3;
		if ( sym == SDLK_d ) sym = sc_4;
		if ( sym == SDLK_f ) sym = sc_5;
		if ( sym == SDLK_g ) sym = sc_6;
		if ( sym == SDLK_x ) sym = sc_7;
		if ( sym == SDLK_c ) sym = sc_8;
		if ( sym == SDLK_v ) sym = sc_9;

		//quick load/quick save
		if ( sym == SDLK_QUOTE ) sym = sc_F9;//load
		if ( sym == SDLK_UNDERSCORE ) sym = sc_F6;//save

		//menu
		if ( sym == SDLK_q ) sym = sc_Escape;

		//arrow keys for menu nav
		if ( sym == SDLK_w ) sym = sc_LeftArrow;
		if ( sym == SDLK_s ) sym = sc_UpArrow;
		if ( sym == SDLK_z ) sym = sc_RightArrow;
		if ( sym == SDLK_y ) sym = sc_RightArrow; // for German keyboards
		if ( sym == SDLK_a ) sym = sc_DownArrow;
		if ( sym == SDLK_RETURN) sym = sc_Return;

		if ( sym == SDLK_0 && state )
		{
			drawoverlay = !drawoverlay;
			if ( drawoverlay )
			{
				initprintf( "Overlay enabled. Press orange+'@' (0) to toggle back.\n" );
				OSD_Printf( "Overlay enabled. Press orange+'@' (0) to toggle back." );
			}
			else
			{
				initprintf( "Overlay disabled. Press orange+'@' (0) to toggle back.\n" );
				OSD_Printf( "Overlay disabled. Press orange+'@' (0) to toggle back." );
			}
		}
	}

	//gesture button
	if ( sym == 231 )
	{
		gesturedown = state;
	}

	if ( sym == SDLK_AT && state )
	{
		normalkeyboard = !normalkeyboard;
		if ( normalkeyboard )
		{
			initprintf( "Normal keyboard enabled. Press '@' to toggle back.\n" );
			OSD_Printf( "Normal keyboard enabled. Press '@' to toggle back." );
		}
		else
		{
			initprintf( "Action keyboard enabled. Press '@' to toggle back.\n" );
			OSD_Printf( "Action keyboard enabled. Press '@' to toggle back." );
		}
	}
	
	return sym;
}

//
//
// ---------------------------------------
//
// System
//
// ---------------------------------------
//
//

int main(int argc, char *argv[])
{
	int r;
	
#ifdef HAVE_GTK2
	if (getenv("DISPLAY") != NULL) {
		gtk_init(&argc, &argv);
		gtkenabled = 1;
	}
	if (gtkenabled) create_startwin();
#endif

	_buildargc = argc;
	_buildargv = (char**)argv;
	//_buildargv = (char**)malloc(argc * sizeof(char*));
	//memcpy(_buildargv, argv, sizeof(char*)*argc);

	baselayer_init();
	r = app_main(argc, argv);

#ifdef HAVE_GTK2
	if (gtkenabled) {
		close_startwin();
		gtk_exit(r);
	}
#endif
	return r;
}


//
// initsystem() -- init SDL systems
//
int initsystem(void)
{
	const SDL_VideoInfo *vid;
	const SDL_version *linked = SDL_Linked_Version();
	SDL_version compiled;
	char drvname[32];

	SDL_VERSION(&compiled);

	initprintf("Initialising SDL system interface "
		  "(compiled with SDL version %d.%d.%d, DLL version %d.%d.%d)\n",
		linked->major, linked->minor, linked->patch,
		compiled.major, compiled.minor, compiled.patch);

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER
#ifdef NOSDLPARACHUTE
			| SDL_INIT_NOPARACHUTE
#endif
		)) {
		initprintf("Initialisation failed! (%s)\n", SDL_GetError());
		return -1;
	}

	atexit(uninitsystem);

	frameplace = 0;
	lockcount = 0;

#ifdef USE_OPENGL
	if (loadgldriver(getenv("BUILD_GLDRV"))) {
		initprintf("Failed loading OpenGL driver. GL modes will be unavailable.\n");
		nogl = 1;
	}
#endif

	{
		SDL_Surface *icon;
		//icon = loadtarga("icon.tga");
		icon = loadappicon();
		if (icon) {
			SDL_WM_SetIcon(icon, 0);
			SDL_FreeSurface(icon);
		}
	}

	if (SDL_VideoDriverName(drvname, 32))
		initprintf("Using \"%s\" video driver\n", drvname);

	// dump a quick summary of the graphics hardware
#ifdef DEBUGGINGAIDS
	vid = SDL_GetVideoInfo();
	initprintf("Video device information:\n");
	initprintf("  Can create hardware surfaces?          %s\n", (vid->hw_available)?"Yes":"No");
	initprintf("  Window manager available?              %s\n", (vid->wm_available)?"Yes":"No");
	initprintf("  Accelerated hardware blits?            %s\n", (vid->blit_hw)?"Yes":"No");
	initprintf("  Accelerated hardware colourkey blits?  %s\n", (vid->blit_hw_CC)?"Yes":"No");
	initprintf("  Accelerated hardware alpha blits?      %s\n", (vid->blit_hw_A)?"Yes":"No");
	initprintf("  Accelerated software blits?            %s\n", (vid->blit_sw)?"Yes":"No");
	initprintf("  Accelerated software colourkey blits?  %s\n", (vid->blit_sw_CC)?"Yes":"No");
	initprintf("  Accelerated software alpha blits?      %s\n", (vid->blit_sw_A)?"Yes":"No");
	initprintf("  Accelerated colour fills?              %s\n", (vid->blit_fill)?"Yes":"No");
	initprintf("  Total video memory:                    %dKB\n", vid->video_mem);
#endif

	return 0;
}


//
// uninitsystem() -- uninit SDL systems
//
void uninitsystem(void)
{
	uninitinput();
	uninitmouse();
	uninittimer();

	SDL_Quit();

#ifdef USE_OPENGL
	unloadgldriver();
#endif
}


//
// initprintf() -- prints a string to the intitialization window
//
void initprintf(const char *f, ...)
{
	va_list va;
	char buf[1024];
	
	va_start(va, f);
	vprintf(f,va);
	va_end(va);

	va_start(va, f);
	Bvsnprintf(buf, 1024, f, va);
	va_end(va);
	OSD_Printf(buf);

#ifdef HAVE_GTK2
	if (gtkenabled) {
		puts_startwin(buf);
		update_startwin();
	}
#endif
}


//
// debugprintf() -- prints a debug string to stderr
//
void debugprintf(const char *f, ...)
{
#ifdef DEBUGGINGAIDS
	va_list va;

	va_start(va,f);
	Bvfprintf(stderr, f, va);
	va_end(va);
#endif
}


//
//
// ---------------------------------------
//
// All things Input
//
// ---------------------------------------
//
//

static char mouseacquired=0;
static char moustat = 0;
static long joyblast=0;
static SDL_Joystick *joydev = NULL;

//
// initinput() -- init input system
//
int initinput(void)
{
	int i,j;
	
	if (SDL_EnableKeyRepeat(250, 30)) initprintf("Error enabling keyboard repeat.\n");
	inputdevices = 1|2;	// keyboard (1) and mouse (2)
	mouseacquired = 0;

	SDL_EnableUNICODE(1);	// let's hope this doesn't hit us too hard

	memset(keynames,0,sizeof(keynames));
	for (i=0; i<SDLK_LAST; i++) {
		if (!keytranslation[i]) continue;
		strncpy(keynames[ keytranslation[i] ], SDL_GetKeyName(i), sizeof(keynames[i])-1);
	}

	if (!SDL_InitSubSystem(SDL_INIT_JOYSTICK)) {
		i = SDL_NumJoysticks();
		initprintf("%d joystick(s) found\n",i);
		for (j=0;j<i;j++) initprintf("  %d. %s\n", j+1, SDL_JoystickName(j));
		joydev = SDL_JoystickOpen(0);
		if (joydev) {
			SDL_JoystickEventState(SDL_ENABLE);

			joynumaxes    = SDL_JoystickNumAxes(joydev);
			joynumbuttons = min(32,SDL_JoystickNumButtons(joydev));
			joynumhats    = SDL_JoystickNumHats(joydev);
			initprintf("Joystick 1 has %d axes, %d buttons, and %d hat(s).\n",
				joynumaxes,joynumbuttons,joynumhats);

			joyaxis = (long *)Bcalloc(joynumaxes, sizeof(long));
			joyhat = (long *)Bcalloc(joynumhats, sizeof(long));
		}
	}

	return 0;
}

//
// uninitinput() -- uninit input system
//
void uninitinput(void)
{
	grabmouse(0);

	if (joydev) {
		SDL_JoystickClose(joydev);
		joydev = NULL;
	}
}

const unsigned char *getkeyname(int num)
{
	if ((unsigned)num >= 256) return NULL;
	return keynames[num];
}

const unsigned char *getjoyname(int what, int num)
{
	static char tmp[64];

	switch (what) {
		case 0:	// axis
			if ((unsigned)num > (unsigned)joynumaxes) return NULL;
			sprintf(tmp,"Axis %d",num);
			return tmp;

		case 1: // button
			if ((unsigned)num > (unsigned)joynumbuttons) return NULL;
			sprintf(tmp,"Button %d",num);
			return tmp;

		case 2: // hat
			if ((unsigned)num > (unsigned)joynumhats) return NULL;
			sprintf(tmp,"Hat %d",num);
			return tmp;

		default:
			return NULL;
	}
}

//
// bgetchar, bkbhit, bflushchars -- character-based input functions
//
unsigned char bgetchar(void)
{
	unsigned char c;
	if (keyasciififoplc == keyasciififoend) return 0;
	c = keyasciififo[keyasciififoplc];
	keyasciififoplc = ((keyasciififoplc+1)&(KEYFIFOSIZ-1));
	return c;
}

int bkbhit(void)
{
	return (keyasciififoplc != keyasciififoend);
}

void bflushchars(void)
{
	keyasciififoplc = keyasciififoend = 0;
}


//
// set{key|mouse|joy}presscallback() -- sets a callback which gets notified when keys are pressed
//
void setkeypresscallback(void (*callback)(long, long)) { keypresscallback = callback; }
void setmousepresscallback(void (*callback)(long, long)) { mousepresscallback = callback; }
void setjoypresscallback(void (*callback)(long, long)) { joypresscallback = callback; }

//
// initmouse() -- init mouse input
//
int initmouse(void)
{
	if (moustat) return 0;

	initprintf("Initialising mouse\n");

	// grab input
	grabmouse(1);
	moustat=1;

	return 0;
}

//
// uninitmouse() -- uninit mouse input
//
void uninitmouse(void)
{
	if (!moustat) return;

	grabmouse(0);
	moustat=mouseacquired=0;
}


//
// grabmouse() -- show/hide mouse cursor
//
void grabmouse(char a)
{
#ifndef DEBUGGINGAIDS
	SDL_GrabMode g;

	if (!moustat) return;
	if (appactive) {
		if (a != mouseacquired) {
			g = SDL_WM_GrabInput( a ? SDL_GRAB_ON : SDL_GRAB_OFF );
			mouseacquired = (g == SDL_GRAB_ON);

			SDL_ShowCursor(mouseacquired ? SDL_DISABLE : SDL_ENABLE);
		}
	} else {
		mouseacquired = a;
	}
#endif
}


//
// readmousexy() -- return mouse motion information
//
void readmousexy(long *x, long *y)
{
	if (!moustat || !mouseacquired) { *x = *y = 0; return; }
	*x = mousex;
	*y = mousey;
	mousex = mousey = 0;
}

//
// readmousebstatus() -- return mouse button information
//
void readmousebstatus(long *b)
{
	if (!moustat || !mouseacquired) *b = 0;
	else *b = mouseb;
}

//
// setjoydeadzone() -- sets the dead and saturation zones for the joystick
//
void setjoydeadzone(int axis, unsigned short dead, unsigned short satur)
{
}


//
// getjoydeadzone() -- gets the dead and saturation zones for the joystick
//
void getjoydeadzone(int axis, unsigned short *dead, unsigned short *satur)
{
	*dead = *satur = 0;
}


//
// releaseallbuttons()
//
void releaseallbuttons(void)
{
}


//
//
// ---------------------------------------
//
// All things Timer
// Ken did this
//
// ---------------------------------------
//
//

static Uint32 timerfreq=0;
static Uint32 timerlastsample=0;
static Uint32 timerticspersec=0;
static void (*usertimercallback)(void) = NULL;

//
// inittimer() -- initialise timer
//
int inittimer(int tickspersecond)
{
	if (timerfreq) return 0;	// already installed

	initprintf("Initialising timer\n");

	timerfreq = 1000;
	timerticspersec = tickspersecond;
	timerlastsample = SDL_GetTicks() * timerticspersec / timerfreq;

	usertimercallback = NULL;

	return 0;
}

//
// uninittimer() -- shut down timer
//
void uninittimer(void)
{
	if (!timerfreq) return;

	timerfreq=0;
}

//
// sampletimer() -- update totalclock
//
void sampletimer(void)
{
	Uint32 i;
	long n;
	
	if (!timerfreq) return;
	
	i = SDL_GetTicks();
	n = (long)(i * timerticspersec / timerfreq) - timerlastsample;
	if (n>0) {
		totalclock += n;
		timerlastsample += n;
	}

	if (usertimercallback) for (; n>0; n--) usertimercallback();
}

//
// getticks() -- returns the sdl ticks count
//
unsigned long getticks(void)
{
	return (unsigned long)SDL_GetTicks();
}


//
// gettimerfreq() -- returns the number of ticks per second the timer is configured to generate
//
int gettimerfreq(void)
{
	return timerticspersec;
}


//
// installusertimercallback() -- set up a callback function to be called when the timer is fired
//
void (*installusertimercallback(void (*callback)(void)))(void)
{
	void (*oldtimercallback)(void);

	oldtimercallback = usertimercallback;
	usertimercallback = callback;

	return oldtimercallback;
}



//
//
// ---------------------------------------
//
// All things Video
//
// ---------------------------------------
//
// 


//
// getvalidmodes() -- figure out what video modes are available
//
static int sortmodes(const struct validmode_t *a, const struct validmode_t *b)
{
	int x;

	if ((x = a->fs   - b->fs)   != 0) return x;
	if ((x = a->bpp  - b->bpp)  != 0) return x;
	if ((x = a->xdim - b->xdim) != 0) return x;
	if ((x = a->ydim - b->ydim) != 0) return x;

	return 0;
}
static char modeschecked=0;
void getvalidmodes(void)
{
	static int cdepths[] = {
		8,
#ifdef USE_OPENGL
		16,24,32,
#endif
		0 };
	static int defaultres[][2] = {
		{1280,1024},{1280,960},{1152,864},{1024,768},{800,600},{640,480},
		{640,400},{512,384},{480,360},{400,300},{320,240},{320,200},{0,0}
	};
	SDL_Rect **modes;
	SDL_PixelFormat pf = { NULL, 8, 1, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0 };
	int i, j, maxx=0, maxy=0;

	if (modeschecked) return;

	validmodecnt=0;
	initprintf("Detecting video modes:\n");

#define ADDMODE(x,y,c,f) if (validmodecnt<MAXVALIDMODES) { \
	int mn; \
	for(mn=0;mn<validmodecnt;mn++) \
		if (validmode[mn].xdim==x && validmode[mn].ydim==y && \
		    validmode[mn].bpp==c  && validmode[mn].fs==f) break; \
	if (mn==validmodecnt) { \
		validmode[validmodecnt].xdim=x; \
		validmode[validmodecnt].ydim=y; \
		validmode[validmodecnt].bpp=c; \
		validmode[validmodecnt].fs=f; \
		validmodecnt++; \
		initprintf("  - %dx%d %d-bit %s\n", x, y, c, (f&1)?"fullscreen":"windowed"); \
	} \
}	

#define CHECK(w,h) if ((w < maxx) && (h < maxy))

	// do fullscreen modes first
	for (j=0; cdepths[j]; j++) {
		pf.BitsPerPixel = cdepths[j];
		pf.BytesPerPixel = cdepths[j] >> 3;

		modes = SDL_ListModes(&pf, SURFACE_FLAGS | SDL_FULLSCREEN);
		if (modes == (SDL_Rect **)0) {
			if (cdepths[j] > 8) cdepths[j] = -1;
			continue;
		}

		if (modes == (SDL_Rect **)-1) {
			for (i=0; defaultres[i][0]; i++)
				ADDMODE(defaultres[i][0],defaultres[i][1],cdepths[j],1)
		} else {
			for (i=0; modes[i]; i++) {
				if ((modes[i]->w > MAXXDIM) || (modes[i]->h > MAXYDIM)) continue;

				ADDMODE(modes[i]->w, modes[i]->h, cdepths[j], 1)

				if ((modes[i]->w > maxx) && (modes[i]->h > maxy)) {
					maxx = modes[i]->w;
					maxy = modes[i]->h;
				}
			}
		}
	}

	if (maxx == 0 && maxy == 0) {
		initprintf("No fullscreen modes available!\n");
		maxx = MAXXDIM; maxy = MAXYDIM;
	}

	// add windowed modes next
	for (j=0; cdepths[j]; j++) {
		if (cdepths[j] < 0) continue;
		for (i=0; defaultres[i][0]; i++)
			CHECK(defaultres[i][0],defaultres[i][1])
				ADDMODE(defaultres[i][0],defaultres[i][1],cdepths[j],0)
	}

#undef CHECK
#undef ADDMODE

	qsort((void*)validmode, validmodecnt, sizeof(struct validmode_t), (int(*)(const void*,const void*))sortmodes);

	modeschecked=1;
}


//
// checkvideomode() -- makes sure the video mode passed is legal
//
int checkvideomode(int *x, int *y, int c, int fs)
{
	int i, nearest=-1, dx, dy, odx=9999, ody=9999;

	getvalidmodes();

	if (c>8 && nogl) return -1;

	// fix up the passed resolution values to be multiples of 8
	// and at least 320x200 or at most MAXXDIMxMAXYDIM
	if (*x < 320) *x = 320;
	if (*y < 200) *y = 200;
	if (*x > MAXXDIM) *x = MAXXDIM;
	if (*y > MAXYDIM) *y = MAXYDIM;
	*x &= 0xfffffff8l;

	for (i=0; i<validmodecnt; i++) {
		if (validmode[i].bpp != c) continue;
		if (validmode[i].fs != fs) continue;
		dx = klabs(validmode[i].xdim - *x);
		dy = klabs(validmode[i].ydim - *y);
		if (!(dx | dy)) { 	// perfect match
			nearest = i;
			break;
		}
		if ((dx <= odx) && (dy <= ody)) {
			nearest = i;
			odx = dx; ody = dy;
		}
	}
		
#ifdef ANY_WINDOWED_SIZE
	if ((fs&1) == 0 && (nearest < 0 || (validmode[nearest].xdim!=*x || validmode[nearest].ydim!=*y)))
		return 0x7fffffffl;
#endif

	if (nearest < 0) {
		// no mode that will match (eg. if no fullscreen modes)
		return -1;
	}

	*x = validmode[nearest].xdim;
	*y = validmode[nearest].ydim;

	return nearest;		// JBF 20031206: Returns the mode number
}


//
// setvideomode() -- set SDL video mode
//
int setvideomode(int x, int y, int c, int fs)
{
	int modenum;
	static int warnonce = 0;
	
	if ((fs == fullscreen) && (x == xres) && (y == yres) && (c == bpp) &&
	    !videomodereset) {
		OSD_ResizeDisplay(xres,yres);
		return 0;
	}

	if (checkvideomode(&x,&y,c,fs) < 0) return -1;

#ifdef HAVE_GTK2
	if (gtkenabled) close_startwin();
#endif

	if (mouseacquired) {
		SDL_WM_GrabInput(SDL_GRAB_OFF);
		SDL_ShowCursor(SDL_ENABLE);
	}

	if (lockcount) while (lockcount) enddrawing();

#if defined(USE_OPENGL)
	if (bpp > 8 && sdl_surface) {
		polymost_glreset();
	}

	if (c > 8) {
		int i;
		struct {
			SDL_GLattr attr;
			int value;
		} attributes[] = {
#if 0
			{ SDL_GL_RED_SIZE, 8 },
			{ SDL_GL_GREEN_SIZE, 8 },
			{ SDL_GL_BLUE_SIZE, 8 },
			{ SDL_GL_ALPHA_SIZE, 8 },
			{ SDL_GL_BUFFER_SIZE, c },
			{ SDL_GL_STENCIL_SIZE, 0 },
			{ SDL_GL_ACCUM_RED_SIZE, 0 },
			{ SDL_GL_ACCUM_GREEN_SIZE, 0 },
			{ SDL_GL_ACCUM_BLUE_SIZE, 0 },
			{ SDL_GL_ACCUM_ALPHA_SIZE, 0 },
			{ SDL_GL_DEPTH_SIZE, 32 },
#endif
			{ SDL_GL_DOUBLEBUFFER, 1 }
		};

		if (nogl) return -1;
		
		for (i=0; i < (int)(sizeof(attributes)/sizeof(attributes[0])); i++)
			SDL_GL_SetAttribute(attributes[i].attr, attributes[i].value);
	}
#endif

	initprintf("Setting video mode %dx%d (%d-bpp %s)\n",
			x,y,c, ((fs&1) ? "fullscreen" : "windowed"));
	sdl_surface = SDL_SetVideoMode(x, y, c, (c>8?SDL_OPENGL:SURFACE_FLAGS) | ((fs&1)?SDL_FULLSCREEN:0));
	if (!sdl_surface) {
		initprintf("Unable to set video mode!\n");
		return -1;
	}

	if (mouseacquired) {
		SDL_WM_GrabInput(SDL_GRAB_ON);
		SDL_ShowCursor(SDL_DISABLE);
	}

#if 0
	{
	char flags[512] = "";
#define FLAG(x,y) if ((sdl_surface->flags & x) == x) { strcat(flags, y); strcat(flags, " "); }
	FLAG(SDL_SWSURFACE, "SWSURFACE")
	FLAG(SDL_HWSURFACE, "HWSURFACE")
	FLAG(SDL_ASYNCBLIT, "ASYNCBLIT")
	FLAG(SDL_ANYFORMAT, "ANYFORMAT")
	FLAG(SDL_HWPALETTE, "HWPALETTE")
	FLAG(SDL_DOUBLEBUF, "DOUBLEBUF")
	FLAG(SDL_FULLSCREEN, "FULLSCREEN")
	FLAG(SDL_OPENGL, "OPENGL")
	FLAG(SDL_OPENGLBLIT, "OPENGLBLIT")
	FLAG(SDL_RESIZABLE, "RESIZABLE")
	FLAG(SDL_HWACCEL, "HWACCEL")
	FLAG(SDL_SRCCOLORKEY, "SRCCOLORKEY")
	FLAG(SDL_RLEACCEL, "RLEACCEL")
	FLAG(SDL_SRCALPHA, "SRCALPHA")
	FLAG(SDL_PREALLOC, "PREALLOC")
#undef FLAG
	printOSD("SDL Surface flags: %s\n", flags);
	}
#endif

	{
		//static char t[384];
		//sprintf(t, "%s (%dx%d %s)", apptitle, x, y, ((fs) ? "fullscreen" : "windowed"));
		SDL_WM_SetCaption(apptitle, 0);
	}

#ifdef USE_OPENGL
	if (c > 8) {
		GLubyte *p,*p2,*p3;
		int i;

		polymost_glreset();
		
		bglEnable(GL_TEXTURE_2D);
		bglShadeModel(GL_SMOOTH); //GL_FLAT
		bglClearColor(0,0,0,0.5); //Black Background
		bglHint(GL_PERSPECTIVE_CORRECTION_HINT,GL_NICEST); //Use FASTEST for ortho!
		bglHint(GL_LINE_SMOOTH_HINT,GL_NICEST);
		bglDisable(GL_DITHER);
	
		glinfo.vendor     = bglGetString(GL_VENDOR);
		glinfo.renderer   = bglGetString(GL_RENDERER);
		glinfo.version    = bglGetString(GL_VERSION);
		glinfo.extensions = bglGetString(GL_EXTENSIONS);
		
		glinfo.maxanisotropy = 1.0;
		glinfo.bgra = 0;
		glinfo.texcompr = 0;

		// process the extensions string and flag stuff we recognize
		p = Bstrdup(glinfo.extensions);
		p3 = p;
		while ((p2 = Bstrtoken(p3==p?p:NULL, " ", (char**)&p3, 1)) != NULL) {
			if (!Bstrcmp(p2, "GL_EXT_texture_filter_anisotropic")) {
				// supports anisotropy. get the maximum anisotropy level
				bglGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &glinfo.maxanisotropy);
			} else if (!Bstrcmp(p2, "GL_EXT_texture_edge_clamp") ||
			           !Bstrcmp(p2, "GL_SGIS_texture_edge_clamp")) {
				// supports GL_CLAMP_TO_EDGE or GL_CLAMP_TO_EDGE_SGIS
				glinfo.clamptoedge = 1;
			} else if (!Bstrcmp(p2, "GL_EXT_bgra")) {
				// support bgra textures
				glinfo.bgra = 1;
			} else if (!Bstrcmp(p2, "GL_ARB_texture_compression")) {
				// support texture compression
				glinfo.texcompr = 1;
			} else if (!Bstrcmp(p2, "GL_ARB_texture_non_power_of_two")) {
				// support non-power-of-two texture sizes
				glinfo.texnpot = 1;
			} else if (!Bstrcmp(p2, "WGL_3DFX_gamma_control")) {
				// 3dfx cards have issues with fog
				nofog = 1;
				if (!(warnonce&1)) initprintf("3dfx card detected: OpenGL fog disabled\n");
				warnonce |= 1;
			}
		}
		Bfree(p);
	}
#endif
	
	xres = x;
	yres = y;
	bpp = c;
	fullscreen = fs;
	//bytesperline = sdl_surface->pitch;
	//imageSize = bytesperline*yres;
	numpages = c>8?2:1;
	frameplace = 0;
	lockcount = 0;
	modechange=1;
	videomodereset = 0;
	OSD_ResizeDisplay(xres,yres);

	if (c==8) setpalette(0,256,0);
	//baselayer_onvideomodechange(c>8);
	
	if (mouseacquired) {
		SDL_WM_GrabInput(SDL_GRAB_ON);
		SDL_ShowCursor(SDL_DISABLE);
	}

	return 0;
}


//
// resetvideomode() -- resets the video system
//
void resetvideomode(void)
{
	videomodereset = 1;
	modeschecked = 0;
}


//
// begindrawing() -- locks the framebuffer for drawing
//
void begindrawing(void)
{
	long i,j;

	if (bpp > 8) {
		if (offscreenrendering) return;
		frameplace = 0;
		bytesperline = 0;
		imageSize = 0;
		modechange = 0;
		return;
	}

	// lock the frame
	if (lockcount++ > 0)
		return;

	if (offscreenrendering) return;
	
	if (SDL_MUSTLOCK(sdl_surface)) SDL_LockSurface(sdl_surface);
	frameplace = (long)sdl_surface->pixels;
	
	if (sdl_surface->pitch != bytesperline || modechange) {
		bytesperline = sdl_surface->pitch;
		imageSize = bytesperline*yres;
		setvlinebpl(bytesperline);

		j = 0;
		for(i=0;i<=ydim;i++) ylookup[i] = j, j += bytesperline;
		modechange=0;
	}
}


//
// enddrawing() -- unlocks the framebuffer
//
void enddrawing(void)
{
	if (bpp > 8) {
		if (!offscreenrendering) frameplace = 0;
		return;
	}

	if (!frameplace) return;
	if (lockcount > 1) { lockcount--; return; }
	if (!offscreenrendering) frameplace = 0;
	if (lockcount == 0) return;
	lockcount = 0;

	if (offscreenrendering) return;

	if (SDL_MUSTLOCK(sdl_surface)) SDL_UnlockSurface(sdl_surface);
}


//
// showframe() -- update the display
//
void showframe(int w)
{
	long i,j;

#ifdef USE_OPENGL
	if (bpp > 8) {
		if (palfadedelta || palfadeclamp.f) {
			bglMatrixMode(GL_PROJECTION);
			bglPushMatrix();
			bglLoadIdentity();
			bglMatrixMode(GL_MODELVIEW);
			bglPushMatrix();
			bglLoadIdentity();

			bglDisable(GL_DEPTH_TEST);
			bglDisable(GL_ALPHA_TEST);
			bglDisable(GL_TEXTURE_2D);

			bglEnable(GL_BLEND);
			bglColor4ub(
				max(palfadeclamp.r,palfadergb.r),
				max(palfadeclamp.g,palfadergb.g),
				max(palfadeclamp.b,palfadergb.b),
				max(palfadeclamp.f,palfadedelta));

			bglBegin(GL_QUADS);
			bglVertex2i(-1, -1);
			bglVertex2i(1, -1);
			bglVertex2i(1, 1);
			bglVertex2i(-1, 1);
			bglEnd();
			
			bglMatrixMode(GL_MODELVIEW);
			bglPopMatrix();
			bglMatrixMode(GL_PROJECTION);
			bglPopMatrix();
		}

		SDL_GL_SwapBuffers();
		return;
	}
#endif
	
	if (offscreenrendering) return;

	if (lockcount) {
		printf("Frame still locked %ld times when showframe() called.\n", lockcount);
		while (lockcount) enddrawing();
	}

	SDL_Flip(sdl_surface);
}


//
// setpalette() -- set palette values
//
int setpalette(int start, int num, char *dapal)
{
	SDL_Color pal[256];
	int i,n;

	if (bpp > 8) return 0;	// no palette in opengl

	copybuf(curpalettefaded, pal, 256);
	
	for (i=start, n=num; n>0; i++, n--) {
		/*
		pal[i].b = dapal[0] << 2;
		pal[i].g = dapal[1] << 2;
		pal[i].r = dapal[2] << 2;
		*/
		curpalettefaded[i].f = pal[i].unused = 0;
		dapal += 4;
	}

	//return SDL_SetPalette(sdl_surface, SDL_LOGPAL|SDL_PHYSPAL, pal, 0, 256);
	return SDL_SetColors(sdl_surface, pal, 0, 256);
}

//
// getpalette() -- get palette values
//
/*
int getpalette(int start, int num, char *dapal)
{
	int i;
	SDL_Palette *pal;

	// we shouldn't need to lock the surface to get the palette
	pal = sdl_surface->format->palette;

	for (i=num; i>0; i--, start++) {
		dapal[0] = pal->colors[start].b >> 2;
		dapal[1] = pal->colors[start].g >> 2;
		dapal[2] = pal->colors[start].r >> 2;
		dapal += 4;
	}

	return 1;
}
*/

//
// setgamma
//
int setgamma(float ro, float go, float bo)
{
	int i;
	unsigned short gt[3*256];
	
	return -1;

	if (!gamma_saved) {
		gamma_saved = 1;
	}

	for (i=0;i<256;i++) {
		gt[i]     = min(255,(unsigned short)floor(255.0 * pow((float)i / 255.0, ro))) << 8;
		gt[i+256] = min(255,(unsigned short)floor(255.0 * pow((float)i / 255.0, go))) << 8;
		gt[i+512] = min(255,(unsigned short)floor(255.0 * pow((float)i / 255.0, bo))) << 8;
	}

	return 0;
}

/*
static SDL_Surface * loadtarga(const char *fn)
{
	int i, width, height, palsize;
	char head[18];
	SDL_Color palette[256];
	SDL_Surface *surf=0;
	long fil;

	clearbufbyte(palette, sizeof(palette), 0);

	if ((fil = kopen4load(fn,0)) == -1) return NULL;

	kread(fil, head, 18);
	if (head[0] > 0) klseek(fil, head[0], SEEK_CUR);

	if ((head[1] != 1) ||		// colormap type
		(head[2] != 1) ||	// image type
		(head[7] != 24)	||	// colormap entry size
		(head[16] != 8)) {	// image descriptor
		printOSD("%s in unsuitable format for icon\n", fn);
		goto nogo;
	}

	width = head[12] | ((int)head[13] << 8);
	height = head[14] | ((int)head[15] << 8);

	if (width != 32 && height != 32) goto nogo;

	surf = SDL_CreateRGBSurface(SDL_SWSURFACE, width, height, 8, 0,0,0,0);
	if (!surf) goto nogo;

	// palette first
	palsize = head[5] | ((int)head[6] << 8);
	for (i=(head[3]|((int)head[4]<<8)); palsize; --palsize, ++i) {
		kread(fil, &palette[i].b, 1);		// b
		kread(fil, &palette[i].g, 1);		// g
		kread(fil, &palette[i].r, 1);		// r
	}

	// targa renders bottom to top, from left to right
	for (i=height-1; i>=0; i--) {
		kread(fil, surf->pixels+i*width, width);
	}
	SDL_SetPalette(surf, SDL_PHYSPAL|SDL_LOGPAL, palette, 0, 256);

nogo:
	kclose(fil);
	return(surf);
}
*/

extern struct sdlappicon sdlappicon;
static SDL_Surface * loadappicon(void)
{
	SDL_Surface *surf;

	surf = SDL_CreateRGBSurfaceFrom((void*)sdlappicon.pixels,
			sdlappicon.width, sdlappicon.height, 8, sdlappicon.width,
			0,0,0,0);
	if (!surf) return NULL;

	SDL_SetPalette(surf, SDL_LOGPAL|SDL_PHYSPAL, (SDL_Color*)sdlappicon.colourmap, 0, sdlappicon.ncolours);

	return surf;
}

//
//
// ---------------------------------------
//
// Miscellany
//
// ---------------------------------------
//
// 

//
// handleevents() -- process the SDL message queue
//   returns !0 if there was an important event worth checking (like quitting)
//
int handleevents(void)
{
	int code, rv=0, j;
	SDL_Event ev;
	static int bMouseDown = 0;

#define SetKey(key,state) { \
	keystatus[key] = state; \
		if (state) { \
	keyfifo[keyfifoend] = key; \
	keyfifo[(keyfifoend+1)&(KEYFIFOSIZ-1)] = state; \
	keyfifoend = ((keyfifoend+2)&(KEYFIFOSIZ-1)); \
		} \
}

	while (SDL_PollEvent(&ev)) {
		switch (ev.type) {
			case SDL_KEYDOWN:
			case SDL_KEYUP:
				code = translateKeyWebOS(ev.key.keysym.sym, ev.key.state);
				//code = keytranslation[ev.key.keysym.sym];

				if (ev.key.keysym.unicode != 0 && ev.key.type == SDL_KEYDOWN &&
				    (ev.key.keysym.unicode & 0xff80) == 0 &&
				    ((keyasciififoend+1)&(KEYFIFOSIZ-1)) != keyasciififoplc) {
					keyasciififo[keyasciififoend] = ev.key.keysym.unicode & 0x7f;
					keyasciififoend = ((keyasciififoend+1)&(KEYFIFOSIZ-1));
				}

				// hook in the osd
				if (OSD_HandleKey(code, (ev.key.type == SDL_KEYDOWN)) == 0)
					break;

				if (ev.key.type == SDL_KEYDOWN) {
					if (!keystatus[code]) {
						SetKey(code, 1);
						if (keypresscallback)
							keypresscallback(code, 1);
					}
				} else {
					SetKey(code, 0);
					if (keypresscallback)
						keypresscallback(code, 0);
				}
				break;

			case SDL_ACTIVEEVENT:
				if (ev.active.state & SDL_APPINPUTFOCUS) {
					appactive = ev.active.gain;
					SetKey(sc_Pause, 1);
					SetKey(sc_Pause, 0);
					if (1/*mouseacquired*/) {
						if (appactive) {
							SDL_WM_GrabInput(SDL_GRAB_ON);
							SDL_ShowCursor(SDL_DISABLE);
						} else {
							SDL_WM_GrabInput(SDL_GRAB_OFF);
							SDL_ShowCursor(SDL_ENABLE);
						}
					}
					rv=-1;
				}
				break;

            case SDL_MOUSEBUTTONUP:
                if ( ev.motion.y > VIDHEIGHT - JOY_SIZE &&
                     ev.motion.x < JOY_SIZE )
                {
                    joy_x = joy_y = 0;
                    mousedown = 0;
                }
                if ( ev.motion.x > VIDWIDTH - FIRE_SIZE &&
                        ev.motion.y > VIDHEIGHT - FIRE_SIZE )
                {
                    //FIRE!
                    SetKey( sc_RightControl, ev.button.state );
                    autofire = ev.button.state;
                    if ( autofire )
                    {
                        fire_counter = FIRE_FRAME_COUNT;
                    }
                    break;
                }
				break;

            case SDL_MOUSEBUTTONDOWN:

				mousedown = 1;
                
                if ( ev.motion.x > VIDWIDTH - FIRE_SIZE &&
                        ev.motion.y > VIDHEIGHT - FIRE_SIZE )
                {
                    //FIRE!
                    SetKey( sc_RightControl, ev.button.state );
                    autofire = ev.button.state;
                    if ( autofire )
                    {
                        fire_counter = FIRE_FRAME_COUNT;
                    }
                    break;
                }
                break;
            case SDL_MOUSEMOTION:
                if ( mousedown && 
                        ev.motion.y > VIDHEIGHT - JOY_SIZE &&
                        ev.motion.x < JOY_SIZE )
                {
                    joy_x = ( ev.motion.x - JOY_X );
                    if ( joy_x < JOY_DEAD && joy_x > -JOY_DEAD )
                    {
                        joy_x = 0;
                    }
                    else
                    {
                        if ( joy_x >= JOY_DEAD )
                        {
                            joy_x -= JOY_DEAD;
                        }
                        else
                        {
                            joy_x += JOY_DEAD;
                        }
                    }

                    joy_y = ( ev.motion.y - JOY_Y );
                    if ( joy_y < JOY_DEAD && joy_y > -JOY_DEAD )
                    {
                        joy_y = 0;
                    }
                    else
                    {
                        if ( joy_y >= JOY_DEAD )
                        {
                            joy_y -= JOY_DEAD;
                        }
                        else
                        {
                            joy_y += JOY_DEAD;
                        }
                    }

					if (joynumaxes > 1) {
						joyaxis[1] = joy_y * 600;//10000 / 32767;
						joyaxis[0] = joy_x * 200;//10000 / 32767;
					//initprintf( "joy_x_y: %d, %d\n", joy_x, joy_y );
					//initprintf( "joyaxis: %d, %d\n", joyaxis[0], joyaxis[1] );
					}
					break;
                }

                if ( !mousedown )
                {
                    joy_x = 0;
                    joy_y = 0;
                }

                //jump: top
                if ( ev.motion.y < JUMP_SIZE )
                {
                    //top-left corner, jump!
                    jumping_counter = JUMP_FRAME_COUNT;
                    SetKey( sc_A, 1 );
                    SetKey( sc_A, 0 );
					//initprintf( "jumping jack flash\n" );
                    break;
                }
                break;

			case SDL_JOYAXISMOTION:
				//if (appactive && ev.jaxis.axis < joynumaxes) {
				//	joyaxis[ ev.jaxis.axis ] = ev.jaxis.value * 10000 / 32767;
				//	//initprintf("joyaxis[0] %d joyaxis[1] %d\n", joyaxis[0], joyaxis[1]);
				//}
				break;

			case SDL_JOYHATMOTION: {
				int hatvals[16] = {
					-1,	// centre
					0, 	// up 1
					9000,	// right 2
					4500,	// up+right 3
					18000,	// down 4
					-1,	// down+up!! 5
					13500,	// down+right 6
					-1,	// down+right+up!! 7
					27000,	// left 8
					27500,	// left+up 9
					-1,	// left+right!! 10
					-1,	// left+right+up!! 11
					22500,	// left+down 12
					-1,	// left+down+up!! 13
					-1,	// left+down+right!! 14
					-1,	// left+down+right+up!! 15
				};
				if (appactive && ev.jhat.hat < joynumhats)
					joyhat[ ev.jhat.hat ] = hatvals[ ev.jhat.value & 15 ];
				break;
			}

			case SDL_JOYBUTTONDOWN:
			case SDL_JOYBUTTONUP:
				if (appactive && ev.jbutton.button < joynumbuttons) {
					if (ev.jbutton.state == SDL_PRESSED)
						joyb |= 1 << ev.jbutton.button;
					else
						joyb &= ~(1 << ev.jbutton.button);
				}
				break;

			case SDL_QUIT:
				quitevent = 1;
				rv=-1;
				break;

			default:
				//printOSD("Got event (%d)\n", ev.type);
				break;
		}
	}

	sampletimer();

#ifdef HAVE_GTK2
	if (gtkenabled) update_startwin();
#endif

#undef SetKey

	return rv;
}

