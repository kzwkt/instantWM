/* See LICENSE file for copyright and license details. */
#include <X11/XF86keysym.h>
#include "instantwm.h"

/* appearance */
static const unsigned int borderpx = 3;		  /* border pixel of windows */
static const unsigned int snap = 32;		  /* snap pixel */
static const unsigned int startmenusize = 30;		  /* snap pixel */
static const unsigned int systraypinning = 0; /* 0: sloppy systray follows selected monitor, >0: pin systray to monitor X */
static const unsigned int systrayspacing = 0; /* systray spacing */
static const int systraypinningfailfirst = 1; /* 1: if pinning fails, display systray on the first monitor, False: display systray on the last monitor*/
static const int showsystray = 1;			  /* 0 means no systray */
static const int showbar = 1;				  /* 0 means no bar */
static const int topbar = 1;				  /* 0 means bottom bar */
static const char *fonts[] = {"monospace:size=12"};

static int barheight;
static char xresourcesfont[30];


static char col_bg[] = "#121212";
static char col_text[] = "#DFDFDF";
static char col_black[] = "#000000";

static char col_bg_accent[] = "#384252";
static char col_bg_accent_hover[] = "#4C5564";
static char col_bg_hover[] = "#1C1C1C";

static char col_light_blue[] = "#89B3F7";
static char col_light_blue_hover[] = "#a1c2f9";
static char col_blue[] = "#536DFE";
static char col_blue_hover[] = "#758afe";


static char col_light_green[] = "#81c995";
static char col_light_green_hover[] = "#99d3aa";
static char col_green[] = "#1e8e3e";
static char col_green_hover[] = "#4ba465";

static char col_light_yellow[] = "#fdd663";
static char col_light_yellow_hover[] = "#fddd82";
static char col_yellow[] = "#f9ab00";
static char col_yellow_hover[] = "#f9bb33";

static char col_light_red[] = "#f28b82";
static char col_light_red_hover[] = "#f4a19a";
static char col_red[] = "#d93025";
static char col_red_hover[] = "#e05951";

static const char *tagcolors[2][5][3] = {
    [SchemeNoHover] = {
        [SchemeTagInactive] = {
            [ColFg] = col_text,
            [ColBg] = col_bg,
            [ColDetail] = col_bg,
        },
        [SchemeTagFilled] = {
            [ColFg] = col_text,
            [ColBg] = col_bg_accent,
            [ColDetail] = col_light_blue,
        },
        [SchemeTagFocus] = {
            [ColFg] = col_black,
            [ColBg] = col_light_green,
            [ColDetail] = col_green,
        },
        [SchemeTagNoFocus] = {
            [ColFg] = col_black,
            [ColBg] = col_light_yellow,
            [ColDetail] = col_yellow,
        },
        [SchemeTagEmpty] = {
            [ColFg] = col_black,
            [ColBg] = col_light_red,
            [ColDetail] = col_red,
        }
    },
    [SchemeHover] = {
        [SchemeTagInactive] = {
            [ColFg] = col_text,
            [ColBg] = col_bg_hover,
            [ColDetail] = col_bg,
        },
        [SchemeTagFilled] = {
            [ColFg] = col_text,
            [ColBg] = col_bg_accent_hover,
            [ColDetail] = col_light_blue_hover,
        },
        [SchemeTagFocus] = {
            [ColFg] = col_black,
            [ColBg] = col_light_green_hover,
            [ColDetail] = col_green_hover,
        },
        [SchemeTagNoFocus] = {
            [ColFg] = col_black,
            [ColBg] = col_light_yellow_hover,
            [ColDetail] = col_yellow_hover,
        },
        [SchemeTagEmpty] = {
            [ColFg] = col_black,
            [ColBg] = col_light_red_hover,
            [ColDetail] = col_red_hover,
        }
    }
};

static const char *windowcolors[2][7][3] = {
    [SchemeNoHover] = {
        [SchemeWinFocus] = {
            [ColFg] = col_text,
            [ColBg] = col_bg_accent,
            [ColDetail] = col_light_blue,
        },
        [SchemeWinNormal] = {
            [ColFg] = col_text,
            [ColBg] = col_bg,
            [ColDetail] = col_bg,
        },
        [SchemeWinMinimized] = {
            [ ColFg ] = col_bg_accent,
            [ ColBg ] = col_bg,
            [ ColDetail ] = col_bg,
        },
        [SchemeWinSticky] = {
            [ ColFg ] = col_black,
            [ ColBg ] = col_light_yellow,
            [ ColDetail ] = col_yellow,
        },
        [ SchemeWinStickyFocus ] = {
            [ ColFg ] = col_black,
            [ ColBg ] = col_light_green,
            [ ColDetail ] = col_green
        },
        [SchemeWinOverlay] = {
            [ ColFg ] = col_black,
            [ ColBg ] = col_light_yellow,
            [ ColDetail ] = col_yellow,
        },
        [SchemeWinOverlayFocus] = {
            [ ColFg ] = col_black,
            [ ColBg ] = col_light_green,
            [ ColDetail ] = col_green,
        },
    },
    //TODO: different hover colors
    [SchemeHover] = {
        [SchemeWinFocus] = {
            [ColFg] = col_text,
            [ColBg] = col_bg_accent_hover,
            [ColDetail] = col_light_blue_hover,
        },
        [SchemeWinNormal] = {
            [ColFg] = col_text,
            [ColBg] = col_bg_hover,
            [ColDetail] = col_bg_hover,
        },
        [SchemeWinMinimized] = {
            [ ColFg ] = col_bg_accent_hover,
            [ ColBg ] = col_bg,
            [ ColDetail ] = col_bg,
        },
        [SchemeWinSticky] = {
            [ ColFg ] = col_black,
            [ ColBg ] = col_light_yellow_hover,
            [ ColDetail ] = col_yellow_hover,
        },
        [ SchemeWinStickyFocus ] = {
            [ ColFg ] = col_black,
            [ ColBg ] = col_light_green_hover,
            [ ColDetail ] = col_green_hover
        },
        [SchemeWinOverlay] = {
            [ ColFg ] = col_black,
            [ ColBg ] = col_light_yellow_hover,
            [ ColDetail ] = col_yellow_hover,
        },
        [SchemeWinOverlayFocus] = {
            [ ColFg ] = col_black,
            [ ColBg ] = col_light_green_hover,
            [ ColDetail ] = col_green_hover,
        },
    }
};

static const char *closebuttoncolors[2][3][3] = {
    [SchemeNoHover] = {
        [ SchemeCloseNormal ] = {
            [ColFg] = col_text,
            [ColBg] = col_light_red,
            [ColDetail] = col_red,
        }, 
        [ SchemeCloseLocked ] = {
            [ ColFg ] = col_text, 
            [ ColBg ] = col_light_yellow,
            [ ColDetail ] = col_yellow
        }, 
        [ SchemeCloseFullscreen ] = {
            [ColFg] = col_text,
            [ColBg] = col_light_red,
            [ColDetail] = col_red,
        }, 
    }, 
    [ SchemeHover ] = {
        [ SchemeCloseNormal ] = {
            [ColFg] = col_text,
            [ColBg] = col_light_red_hover,
            [ColDetail] = col_red_hover,
        }, 
        [ SchemeCloseLocked ] = {
            [ ColFg ] = col_text, 
            [ ColBg ] = col_light_yellow_hover,
            [ ColDetail ] = col_yellow_hover
        },
        [ SchemeCloseFullscreen ] = {
            [ColFg] = col_text,
            [ColBg] = col_light_red_hover,
            [ColDetail] = col_red_hover,
        }, 
    }
};

static const char *bordercolors[] = {
    [ SchemeBorderNormal ] = col_bg_accent,
    [ SchemeBorderTileFocus ] = col_light_blue,
    [ SchemeBorderFloatFocus ] = col_light_green,
    [ SchemeBorderSnap ] = col_light_yellow
};

static const char *statusbarcolors[] = {
    [ ColFg ] = col_text,
    [ ColBg ] = col_bg,
    [ ColDetail ] = col_bg
};

SchemePref schemehovertypes[] = {
    { "hover", SchemeHover }, 
    { "nohover", SchemeNoHover }
};

SchemePref schemewindowtypes[] = {
    {"normal", SchemeWinNormal},
    {"minimized", SchemeWinMinimized},
    {"sticky", SchemeWinSticky},
    {"focus", SchemeWinFocus},
    {"stickyfocus", SchemeWinStickyFocus},
    {"overlay", SchemeWinOverlay},
    {"overlayfocus", SchemeWinOverlayFocus},
};

SchemePref schemetagtypes[] = {
    {"inactive", SchemeTagInactive},
    {"filled", SchemeTagFilled},
    {"focus", SchemeTagFocus},
    {"nofocus", SchemeTagNoFocus},
    {"empty", SchemeTagEmpty},
};

SchemePref schemeclosetypes[] = {
    {"normal", SchemeCloseNormal},
    {"locked", SchemeCloseLocked},
    {"fullscreen", SchemeCloseFullscreen},
};

SchemePref schemecolortypes[] = {
    {"fg", ColFg},
    {"bg", ColBg},
    {"detail", ColDetail},
};


/* tagging */
#define MAX_TAGLEN 16
static const char *tags_default[] = {"1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "13", "14", "15", "16", "17", "18", "19", "20", "s"};
static char tags[][MAX_TAGLEN] = { "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "13", "14", "15", "16", "17", "18", "19", "20", "s" };
/* ffox, programming1, term, music, steam, folder, play icon, document, message  */
static const char *tagsalt[] = { "", "{}", "$", "", "", "", "", "", "" };

static const char scratchpadname[] = "instantscratchpad";

static const char *upvol[] = {"amixer", "set", "Master", "2%+", NULL};
static const char *downvol[] = {"amixer", "set", "Master", "2%-", NULL};
static const char *mutevol[] = {"amixer", "set", "Master", "toggle", NULL};

static const char *upbright[] = {"light", "-A", "2", NULL};   
static const char *downbright[] = {"light", "-U", "2", NULL}; 


static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class                        instance  title  tags mask  isfloating  monitor */
	{"Pavucontrol",                 NULL,     NULL,  0,         1,          -1},
	{"floatmenu",                   NULL,     NULL,  0,         1,          -1},
	{"instantfloat",                NULL,     NULL,  0,         2,          -1},
	{scratchpadname,                NULL,     NULL,  0,         4,          -1},
	{"Panther",                     NULL,     NULL,  0,         3,          -1},
	{"Peek",                        NULL,     NULL,  0,         1,          -1},
};

/* layout(s) */
static const float mfact = 0.55;  /* factor of master area size [0.05..0.95] */
static const int nmaster = 1;	 /* number of clients in master area */
static const int resizehints = 1; /* 1 means respect size hints in tiled resizals */
static const int decorhints  = 1;    /* 1 means respect decoration hints */

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "+",        tile },    /* first entry is default */
	{ "#",        grid },
	{ "-",        NULL },    /* no layout function means floating behavior */
	{ "[M]",      monocle },
	{ "|||",      tcl },
	{ "H[]",      deck },
	{ "O",        overviewlayout },
	{ "TTT",      bstack },
	{ "===",      bstackhoriz },
	{ NULL,       NULL },
};

/* key definitions */
#define MODKEY Mod4Mask
#define TAGKEYS(KEY, TAG)                                          \
		{MODKEY, KEY, view, {.ui = 1 << TAG}},                     \
		{MODKEY|ControlMask, KEY, toggleview, {.ui = 1 << TAG}}, \
		{MODKEY|ShiftMask, KEY, tag, {.ui = 1 << TAG}},          \
		{MODKEY|Mod1Mask, KEY, followtag, {.ui = 1 << TAG}},          \
		{MODKEY|ControlMask|ShiftMask, KEY, toggletag, {.ui = 1 << TAG}}, \
		{MODKEY|Mod1Mask|ShiftMask, KEY, swaptags, {.ui = 1 << TAG}},


#define SHCMD(cmd)                                           \
	{                                                        \
		.v = (const char *[]) { "/bin/sh", "-c", cmd, NULL } \
	}

/* commands */
static char instantmenumon[2] = "0"; /* component of instantmenucmd, manipulated in spawn() */
static const char *instantmenucmd[] = {"instantmenu_run", NULL};
static const char *clipmenucmd[] = {"instantclipmenu", NULL};
static const char *smartcmd[] = {"instantmenu_smartrun", NULL};
static const char *instantmenustcmd[] = {"instantmenu_run_st", NULL};
static const char *termcmd[] = {".config/instantos/default/terminal", NULL};
static const char *termscratchcmd[] = {".config/instantos/default/terminal", "-c", scratchpadname, NULL};
static const char *quickmenucmd[] = {"quickmenu", NULL};
static const char *instantassistcmd[] = {"instantassist", NULL};
static const char *instantrepeatcmd[] = {"instantrepeat", NULL};
static const char *instantsharecmd[] = {"instantshare", "snap", NULL};
static const char *nautiluscmd[] = {".config/instantos/default/filemanager", NULL};
static const char *slockcmd[] = {".config/instantos/default/lockscreen", NULL};
static const char *onekeylock[] = {"ilock", "-o", NULL};
static const char *langswitchcmd[] = {"ilayout", NULL};
static const char *oslockcmd[] = {"instantlock", "-o", NULL};
static const char *helpcmd[] = {"instanthotkeys", "gui", NULL};
static const char *searchcmd[] = {"instantsearch", NULL};
static const char *keylayoutswitchcmd[] = {"instantkeyswitch", NULL};
static const char *iswitchcmd[] = {"iswitch", NULL};
static const char *instantswitchcmd[] = {"rofi", "-show", "window", "-kb-row-down", "Alt+Tab,Down", "-kb-row-up", "Alt+Ctrl+Tab,Up", "-kb-accept-entry", "!Alt_L,!Alt+Tab,Return", "-me-select-entry", "", "-me-accept-entry", "MousePrimary", NULL};
static const char *caretinstantswitchcmd[] = {"rofi", "-show", "window", "-kb-row-down", "Alt+Tab,Down", "-kb-row-up", "Alt+Ctrl+Tab,Up", "-kb-accept-entry", "!Alt_L,!Alt+Tab,Return", "-me-select-entry", "", "-me-accept-entry", "MousePrimary", "-theme", ".config/rofi/appmenu.rasi", NULL};
static const char *instantskippycmd[] = {"instantskippy", NULL};
static const char *instantshutdowncmd[] = {"instantshutdown", NULL};
static const char *systemmonitorcmd[] = {".config/instantos/default/systemmonitor", NULL};
static const char *notifycmd[] = {"instantnotify", NULL};
static const char *rangercmd[] = { ".config/instantos/default/termfilemanager", NULL };
static const char *panther[] = { ".config/instantos/default/appmenu", NULL};
static const char *controlcentercmd[] = { "instantsettings", NULL};
static const char *displaycmd[] = { "instantdisper", NULL};
static const char *pavucontrol[] = { "pavucontrol", NULL};
static const char *onboardcmd[] = {"onboard", NULL};
static const char *instantsettings[] = { "instantsettings", NULL};
// static const char  *clickcmd[] = { "autoclicker", NULL };
static const char  *codecmd[] = { ".config/instantos/default/code", NULL };
static const char  *startmenucmd[] = { "instantstartmenu", NULL };

static const char *scrotcmd[] = {"scrot", NULL};                          // Take a screenshot
static const char *fscrotcmd[] = {"scrot", "-f", NULL};                    // Select a region to screenshot
static const char *clipscrotcmd[] = {"scrot", "-e", "mv $f ~/Pictures/", NULL}; // Take a screenshot and save it to ~/Pictures
static const char *fclipscrotcmd[] = {"scrot", "-f", "-e", "mv $f ~/Pictures/", NULL}; // Select a region and save to ~/Pictures


static const char  *firefoxcmd[] = { ".config/instantos/default/browser", NULL };
static const char  *editorcmd[] = { ".config/instantos/default/editor", NULL };

static const char *playernext[] = { "playerctl", "next", NULL};
static const char *playerprevious[] = { "playerctl", "previous", NULL};
static const char *playerpause[] = { "playerctl", "play-pause", NULL};

#include "push.c"

ResourcePref resources[] = {
    { "barheight",        INTEGER, &barheight },
    { "font",             STRING,  &xresourcesfont },

    // set tag labels
    { "tag1",             STRING,  &tags[0] },
    { "tag2",             STRING,  &tags[1] },
    { "tag3",             STRING,  &tags[2] },
    { "tag4",             STRING,  &tags[3] },
    { "tag5",             STRING,  &tags[4] },
    { "tag6",             STRING,  &tags[5] },
    { "tag7",             STRING,  &tags[6] },
    { "tag8",             STRING,  &tags[7] },
    { "tag9",             STRING,  &tags[8] },

};

// instantwmctrl commands
static Xcommand commands[] = {
	/* signum       function        default argument  arg handler*/
	// 0 means off, 1 means toggle, 2 means on
    // arg handlers:
    // 0  no argument
    // 1  binary toggle
    // 3  tag number (bitmask)
    // 4  string
    // 5  integer
	{ "overlay",                setoverlay,                   {0},         0 },
	{ "warpfocus",              warpfocus,                   {0},         0 },
	{ "tag",                    view,                         { .ui = 2 }, 3 },
	{ "animated",               toggleanimated,               { .ui = 2 }, 1 },
	{ "border",                 setborderwidth,               { .i =  borderpx  }, 5 },
	{ "focusfollowsmouse",      togglefocusfollowsmouse,      { .ui = 2 }, 1 },
	{ "focusfollowsfloatmouse", togglefocusfollowsfloatmouse, { .ui = 2 }, 1 },
	{ "alttab",                 alttabfree,                   { .ui = 2 }, 1 },
	{ "layout",                 commandlayout,                { .ui = 0 }, 1 },
	{ "prefix",                 commandprefix,                { .ui = 1 }, 1 },
	{ "alttag",                 togglealttag,                 { .ui = 0 }, 1 },
	{ "hidetags",               toggleshowtags,               { .ui = 0 }, 1 },
	{ "specialnext",            setspecialnext,               { .ui = 0 }, 3 },
	{ "tagmon",                 tagmon,                       { .i = +1 }, 0 },
	{ "followmon",              followmon,                    { .i = +1 }, 0 },
	{ "focusmon",               focusmon,                     { .i = +1 }, 0 },
	{ "focusnmon",               focusnmon,                   { .i = 0 }, 5 },
	{ "nametag",                nametag,                      { .v = "tag" }, 4 },
	{ "resetnametag",           resetnametag,                 {0}, 0 },
};

static const Key dkeys[] = {
	/* modifier  key        function     argument */
	{0,          XK_r,      spawn,       {.v = rangercmd } },
	{0,          XK_e,      spawn,       {.v = editorcmd } },
	{0,          XK_n,      spawn,       {.v = nautiluscmd } },
	{0,          XK_space,  spawn,       {.v = panther} },
	{0,          XK_f,      spawn,       {.v = firefoxcmd} },
	{0,          XK_a,      spawn,       {.v = instantassistcmd} },
	{0,          XK_F1,     spawn,       {.v = helpcmd} },
	{0,          XK_s,      spawn,       {.v = termcmd} },
	{0,          XK_plus,   spawn,       {.v = upvol} },
	{0,          XK_minus,  spawn,       {.v = downvol} },
	{0,          XK_Tab,    spawn,       {.v = caretinstantswitchcmd} },
	{0,          XK_c,      spawn,       {.v = codecmd} },
	{0,          XK_y,      spawn,       {.v = smartcmd} },
	{0,          XK_v,      spawn,       {.v = quickmenucmd} },

	{0,          XK_h,      viewtoleft,  {0}},
	{0,          XK_l,      viewtoright, {0}},
	{0,          XK_k,      shiftview,   {.i = +1 } },
	{0,          XK_j,      shiftview,   {.i = -1 } },

	{0,          XK_Left,   viewtoleft,  {0}},
	{0,          XK_Right,  viewtoright, {0}},
	{0,          XK_Up,     shiftview,   {.i = +1 } },
	{0,          XK_Down,   shiftview,   {.i = -1 } },

	{0,          XK_1,      view,        {.ui = 1 << 0}},
	{0,          XK_2,      view,        {.ui = 1 << 1}},
	{0,          XK_3,      view,        {.ui = 1 << 2}},
	{0,          XK_4,      view,        {.ui = 1 << 3}},
	{0,          XK_5,      view,        {.ui = 1 << 4}},
	{0,          XK_6,      view,        {.ui = 1 << 5}},
	{0,          XK_7,      view,        {.ui = 1 << 6}},
	{0,          XK_8,      view,        {.ui = 1 << 7}},
	{0,          XK_9,      view,        {.ui = 1 << 8}},

};

static Key keys[] = {
	/* modifier                             key                 function              argument */

	{MODKEY|Mod1Mask,                       XK_j,               keyresize,            {.i = 0}},
	{MODKEY|Mod1Mask,                       XK_k,               keyresize,            {.i = 1}},
	{MODKEY|Mod1Mask,                       XK_l,               keyresize,            {.i = 2}},
	{MODKEY|Mod1Mask,                       XK_h,               keyresize,            {.i = 3}},
	{MODKEY|ControlMask,                    XK_d,               distributeclients,    {0}},
	{MODKEY|ShiftMask,                      XK_d,               drawwindow,           {0}},
	{MODKEY|ShiftMask,                      XK_Escape,          spawn,                {.v = systemmonitorcmd}},

	{MODKEY,                                XK_r,               spawn,                {.v = rangercmd } },
	{MODKEY|ControlMask|Mod1Mask,           XK_r,               redrawwin,            {0} },
	{MODKEY,                                XK_n,               spawn,                {.v = nautiluscmd } },
	{MODKEY|ControlMask,                    XK_q,               spawn,                {.v = instantshutdowncmd } },
	{MODKEY,                                XK_y,               spawn,                {.v = panther} },
	{MODKEY,                                XK_a,               spawn,                {.v = instantassistcmd} },
	{MODKEY|ShiftMask,                      XK_a,               spawn,                {.v = instantrepeatcmd} },
	{MODKEY|ShiftMask,                      XK_i,               spawn,                {.v = instantsharecmd} },
	{MODKEY,                                XK_w,               setoverlay,           {0} },
	{MODKEY|ControlMask,                    XK_w,               createoverlay,        {0} },
	{MODKEY,                                XK_g,               spawn,                {.v = notifycmd} },
	{MODKEY|ControlMask,                    XK_space,           spawn,                {.v = instantmenucmd}},
	{MODKEY|ShiftMask,                      XK_v,               spawn,                {.v = clipmenucmd}},
	{MODKEY,                                XK_space,           spawn,                {.v = smartcmd}},
	{MODKEY,                                XK_minus,           spawn,                {.v = instantmenustcmd}},
	{MODKEY,                                XK_x,               spawn,                {.v = instantswitchcmd}},
	{Mod1Mask,                              XK_Tab,             spawn,                {.v = iswitchcmd}},
	{MODKEY|Mod1Mask|ControlMask|ShiftMask, XK_Tab,             alttabfree,           {0}},
	{MODKEY,                                XK_dead_circumflex, spawn,                {.v = caretinstantswitchcmd}},
	{MODKEY|ControlMask,                    XK_l,               spawn,                {.v = slockcmd}},
	{MODKEY|ControlMask|ShiftMask,          XK_l,               spawn,                {.v = onekeylock}},
	{MODKEY|ControlMask,                    XK_h,               hidewin,              {0}},
	{MODKEY|Mod1Mask|ControlMask,           XK_h,               unhideall,            {0}},
	{MODKEY|Mod1Mask|ControlMask,           XK_l,               spawn,                {.v = langswitchcmd}},
	{MODKEY,                                XK_Return,          spawn,                {.v = termcmd}},
	{MODKEY,                                XK_v,               spawn,                {.v = quickmenucmd}},
	{MODKEY,                                XK_b,               togglebar,            {0}},
	{MODKEY,                                XK_j,               focusstack,           {.i = +1}},
	{MODKEY,                                XK_Down,            downkey,              {.i = +1}},
	{MODKEY|ShiftMask,                      XK_Down,            downpress,            {0}},
	{MODKEY,                                XK_k,               focusstack,           {.i = -1}},
	{MODKEY,                                XK_Up,              upkey,                {.i = -1}},
	{MODKEY|ShiftMask,                      XK_Up,              uppress,              {0}},
	{MODKEY|ControlMask,                    XK_j,               pushdown,             {0} },
	{MODKEY|ControlMask,                    XK_k,               pushup,               {0} },
	{MODKEY|Mod1Mask,                       XK_s,               togglealttag,         { .ui = 2 } },
	{MODKEY|ShiftMask|Mod1Mask,             XK_s,               toggleanimated,       { .ui = 2 } },
	{MODKEY|ControlMask,                    XK_s,               togglesticky,         {0} },
	{MODKEY|ShiftMask,                      XK_s,               createscratchpad,     {0}},
	{MODKEY,                                XK_s,               togglescratchpad,     {0}},
	{MODKEY|ShiftMask,                      XK_f,               togglefakefullscreen, {0} },
	{MODKEY|ControlMask,                    XK_f,               tempfullscreen,       {0} },
	{MODKEY|Mod1Mask,                       XK_f,               spawn,                { .v = searchcmd } },
	{MODKEY|Mod1Mask,                       XK_space,           spawn,                { .v = keylayoutswitchcmd } },
	{MODKEY|ShiftMask|Mod1Mask,             XK_d,               toggledoubledraw,     {0} },
	{MODKEY|ShiftMask,                      XK_w,               warpfocus,            {0} },
	{MODKEY|Mod1Mask,                       XK_w,               centerwindow,         {0} },
	{MODKEY|ShiftMask|ControlMask,          XK_s,               toggleshowtags,       { .ui = 2 } },
	{MODKEY,                                XK_i,               incnmaster,           {.i = +1}},
	{MODKEY,                                XK_d,               incnmaster,           {.i = -1}},
	{MODKEY,                                XK_h,               setmfact,             {.f = -0.05}},
	{MODKEY,                                XK_l,               setmfact,             {.f = +0.05}},
	{MODKEY|ShiftMask,                      XK_Return,          zoom,                 {0}},
	{MODKEY,                                XK_Tab,             lastview,             {0}},
	{MODKEY|ShiftMask,                      XK_Tab,             focuslastclient,      {0}},
	{MODKEY|Mod1Mask,                       XK_Tab,             followview,           {0}},
	{MODKEY,                                XK_q,               shutkill,             {0}},
	{Mod1Mask,                              XK_F4,              killclient,           {0}},
	{MODKEY,                                XK_F1,              spawn,                {.v = helpcmd}},
	{MODKEY,                                XK_F2,              toggleprefix,         {0}},
	{MODKEY,                                XK_t,               setlayout,            {.v = &layouts[0]}},
	{MODKEY,                                XK_f,               setlayout,            {.v = &layouts[2]}},
	{MODKEY,                                XK_m,               setlayout,            {.v = &layouts[3]}},
	{MODKEY|ShiftMask,                      XK_m,               movemouse,            {0}},
	{MODKEY|Mod1Mask,                       XK_m,               resizemouse,          {0}},
	{MODKEY,                                XK_c,               setlayout,            {.v = &layouts[1]}},
	{MODKEY|ControlMask,                    XK_c,               spawn,                {.v = controlcentercmd}},

	{MODKEY,                                XK_Left,            animleft,             {0}},
	{MODKEY,                                XK_Right,           animright,            {0}},

	{MODKEY,                                XK_e,               overtoggle,           {.ui = ~0}},
	{MODKEY|ShiftMask,                      XK_e,               fullovertoggle,       {.ui = ~0}},
	{MODKEY|ControlMask,                      XK_e,               spawn,                {.v = instantskippycmd} },

	{MODKEY|ControlMask,                    XK_Left,            directionfocus,            {.ui = 3 }},
	{MODKEY|ControlMask,                    XK_Right,           directionfocus,            {.ui = 1 }},
	{MODKEY|ControlMask,                    XK_Up,              directionfocus,            {.ui = 0 }},
	{MODKEY|ControlMask,                    XK_Down,            directionfocus,            {.ui = 2 }},

	{MODKEY|ShiftMask|ControlMask,                    XK_Right,           shiftview,            {.i = +1 }},
	{MODKEY|ShiftMask|ControlMask,                    XK_Left,           shiftview,            {.i = -1 }},

	{MODKEY|Mod1Mask,                       XK_Left,            moveleft,             {0}},
	{MODKEY|Mod1Mask,                       XK_Right,           moveright,            {0}},

	{MODKEY|ShiftMask,                      XK_Left,            tagtoleft,            {0}},
	{MODKEY|ShiftMask,                      XK_Right,           tagtoright,           {0}},

	{MODKEY|ShiftMask,                      XK_j,               moveresize,           {.i = 0}},
	{MODKEY|ShiftMask,                      XK_k,               moveresize,           {.i = 1}},
	{MODKEY|ShiftMask,                      XK_l,               moveresize,           {.i = 2}},
	{MODKEY|ShiftMask,                      XK_h,               moveresize,           {.i = 3}},


	{MODKEY|ControlMask,                    XK_comma,           cyclelayout,          {.i = -1 } },
	{MODKEY|ControlMask,                    XK_period,          cyclelayout,          {.i = +1 } },
	{MODKEY,                                XK_p,               setlayout,            {0}},
	{MODKEY|ShiftMask,                      XK_p,               spawn,                {.v = displaycmd }},
	{MODKEY|ShiftMask,                      XK_space,           spacetoggle,          {0}},
	{MODKEY,                                XK_0,               view,                 {.ui = ~0}},
	{MODKEY|ShiftMask,                      XK_0,               tag,                  {.ui = ~0}},
	{MODKEY,                                XK_comma,           focusmon,             {.i = -1}},
	{MODKEY,                                XK_period,          focusmon,             {.i = +1}},
	{MODKEY|ShiftMask,                      XK_comma,           tagmon,               {.i = -1}},
	{MODKEY|ShiftMask,                      XK_period,          tagmon,               {.i = +1}},
	{MODKEY|Mod1Mask,                       XK_comma,           followmon,            {.i = -1}},
	{MODKEY|Mod1Mask,                       XK_period,                       followmon,  {.i = +1}},

	{MODKEY|ShiftMask|ControlMask|Mod1Mask, XK_period,                       desktopset, {0}},
	TAGKEYS(XK_1, 0)
	TAGKEYS(XK_2, 1)
	TAGKEYS(XK_3, 2)
	TAGKEYS(XK_4, 3)
	TAGKEYS(XK_5, 4)
	TAGKEYS(XK_6, 5)
	TAGKEYS(XK_7, 6)
	TAGKEYS(XK_8, 7)
	TAGKEYS(XK_9, 8)
    {MODKEY|ShiftMask|ControlMask, XK_q,                     quit,    {0}},
	{0,                            XF86XK_MonBrightnessUp,   spawn,   {.v = upbright}},
	{0,                            XF86XK_MonBrightnessDown, spawn,   {.v = downbright}},
	{0,                            XF86XK_AudioLowerVolume,  spawn,   {.v = downvol}},
	{0,                            XF86XK_AudioMute,         spawn,   {.v = mutevol}},
	{0,                            XF86XK_AudioRaiseVolume,  spawn,   {.v = upvol}},
	{0,                            XF86XK_AudioPlay,         spawn,   {.v = playerpause}},
	{0,                            XF86XK_AudioPause,        spawn,   {.v = playerpause}},
	{0,                            XF86XK_AudioNext,         spawn,   {.v = playernext}},
	{0,                            XF86XK_AudioPrev,         spawn,   {.v = playerprevious}},

	{MODKEY|ShiftMask,             XK_Print,                 spawn,   {.v = fscrotcmd}},
	{MODKEY,                       XK_Print,                 spawn,   {.v = scrotcmd}},
	{MODKEY|ControlMask,           XK_Print,                 spawn,   {.v = clipscrotcmd}},
	{MODKEY|Mod1Mask,              XK_Print,                 spawn,   {.v = fclipscrotcmd}},

	{ MODKEY,                      XK_o,                     winview, {0} },

};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static const Button buttons[] = {
	/* click          event mask          button   function           argument */
	{ ClkLtSymbol,    0,                  Button1, cyclelayout,       {.i = -1 } },
	{ ClkLtSymbol,    0,                  Button3, cyclelayout,       {.i = +1 } },
	{ ClkLtSymbol,    MODKEY,             Button1, createoverlay,     {0} },
	{ ClkLtSymbol,    0,                  Button2, setlayout,         {.v = &layouts[0]} },
	{ ClkWinTitle,    0,                  Button1, dragmouse,         {0} },
	{ ClkWinTitle,    MODKEY,             Button1, setoverlay,        {0} },
	{ ClkWinTitle,    MODKEY,             Button3, spawn,             {.v = notifycmd } },
	{ ClkStatusText,  0,                  Button3, spawn,             {.v = caretinstantswitchcmd } },
	{ ClkWinTitle,    0,                  Button2, closewin,          {0} },
	{ ClkCloseButton, 0,                  Button1, killclient,        {0} },
	{ ClkCloseButton, 0,                  Button3, togglelocked,      {0} },
	{ ClkWinTitle,    0,                  Button3, dragrightmouse,    {0} },
	{ ClkWinTitle,    0,                  Button5, focusstack,        {.i = +1} },
	{ ClkWinTitle,    0,                  Button4, focusstack,        {.i = -1} },
	{ ClkWinTitle,    ShiftMask,          Button5, pushdown,          {0} },
	{ ClkWinTitle,    ShiftMask,          Button4, pushup,            {0} },
	{ ClkWinTitle,    ControlMask,        Button5, downscaleclient,   {0} },
	{ ClkWinTitle,    ControlMask,        Button4, upscaleclient,     {0} },
	{ ClkStatusText,  0,                  Button2, spawn,             {.v = termcmd } },
	{ ClkStatusText,  0,                  Button4, spawn,             {.v = upvol } },
	{ ClkStatusText,  0,                  Button5, spawn,             {.v = downvol } },
	{ ClkStatusText,  MODKEY,             Button2, spawn,             {.v = mutevol } },
	{ ClkStatusText,  0,                  Button1, spawn,             {.v = panther } },
	{ ClkStatusText,  MODKEY|ShiftMask,   Button1, spawn,             {.v = pavucontrol } },
	{ ClkStatusText,  MODKEY|ControlMask, Button1, spawn,             {.v = notifycmd } },
	{ ClkStatusText,  MODKEY,             Button1, spawn,             {.v = instantsettings } },
	{ ClkStatusText,  MODKEY,             Button4, spawn,             {.v = upbright } },
	{ ClkStatusText,  MODKEY,             Button5, spawn,             {.v = downbright } },
	{ ClkRootWin,     MODKEY,             Button3, spawn,             {.v = notifycmd } },
	{ ClkRootWin,     0,                  Button1, spawn,             {.v = panther } },
	{ ClkRootWin,     MODKEY,             Button1, setoverlay,        {0} },
	{ ClkRootWin,     0,                  Button3, spawn,             {.v = smartcmd } },
	{ ClkRootWin,     0,                  Button5, showoverlay,       {0} },
	{ ClkRootWin,     0,                  Button4, hideoverlay,       {0} },
	{ ClkRootWin,     0,                  Button2, spawn,             {.v = instantmenucmd } },
	{ ClkClientWin,   MODKEY,             Button1, movemouse,         {0} },
	{ ClkClientWin,   MODKEY,             Button2, togglefloating,    {0} },
	{ ClkClientWin,   MODKEY,             Button3, resizemouse,       {0} },
	{ ClkClientWin,   MODKEY|Mod1Mask,    Button3, forceresizemouse,  {0} },
	{ ClkClientWin,   MODKEY|ShiftMask,   Button3, resizeaspectmouse, {0} },
	{ ClkTagBar,      0,                  Button1, dragtag,           {0} },
	{ ClkTagBar,      0,                  Button5, viewtoright,       {0} },
	{ ClkTagBar,      MODKEY,             Button4, shiftview,         {.i = -1 } },
	{ ClkTagBar,      MODKEY,             Button5, shiftview,         {.i = +1 } },
	{ ClkTagBar,      0,                  Button4, viewtoleft,        {0} },
	{ ClkTagBar,      0,                  Button3, toggleview,        {0} },
	{ ClkTagBar,      MODKEY,             Button1, tag,               {0} },
	{ ClkTagBar,      Mod1Mask,           Button1, followtag,         {0} },
	{ ClkTagBar,      MODKEY,             Button3, toggletag,         {0} },
	{ ClkShutDown,    0,                  Button1, spawn,             {.v = instantshutdowncmd } },
	{ ClkShutDown,    0,                  Button3, spawn,             {.v = slockcmd } },
	{ ClkShutDown,    0,                  Button2, spawn,             {.v = oslockcmd } },
	{ ClkSideBar,     0,                  Button1, gesturemouse,      {0} },
	{ ClkStartMenu,   0,                  Button1, spawn,             {.v = startmenucmd}},
	{ ClkStartMenu,   ShiftMask,          Button1, toggleprefix,      {0}},
	{ ClkStartMenu,   0,                  Button3, spawn,             {.v = quickmenucmd}},
};
