/* See LICENSE file for copyright and license details. */

/* appearance */
static const unsigned int borderpx       = 1;  /* border pixel of windows */
static const unsigned int snap           = 8;  /* snap pixel */
/* vanitygaps */
static const unsigned int pertaggap      = 1;  /* 0 means all tag have same gap */
static const unsigned int edgegap        = 1;  /* 1 means floating client respect the outer gap */
static const unsigned int gappih         = 8;  /* horiz inner gap between windows */
static const unsigned int gappiv         = 8;  /* vert inner gap between windows */
static const unsigned int gappoh         = 8;  /* horiz outer gap between windows and screen edge */
static const unsigned int gappov         = 8;  /* vert outer gap between windows and screen edge */
static       int smartgaps               = 0;  /* 1 means no outer gap when there is only one window */
/* bar */
static const unsigned int barheight      = 29; /* 0 means default height*/
static const unsigned int centertitle    = 0;  /* 0 means left title */
static const unsigned int colorfultitle  = 1;  /* 0 means title use SchemeTitle and SchemeTitleFloat */
static const unsigned int colorfultag    = 1;  /* 0 means use SchemeSel for selected tag */
static const unsigned int showvacanttags = 0;  /* 0 mean hide vacant tags */
static const unsigned int showtitle      = 1;  /* 0 means no title */
static const int vertpad                 = 8;  /* vertical padding of bar */
static const int sidepad                 = 8;  /* horizontal padding of bar */
static const int showbar                 = 1;  /* 0 means no bar */
static const int topbar                  = 1;  /* 0 means bottom bar */
/* font and color */
static const char *fonts[]               = { "SF Mono:style=Medium:size=10" };
static const char dmenufont[]            = "SF Mono:style=Medium:size=10";
static const char col_bg[]               = "#1a1b26";
static const char col_dark[]             = "#16161E";
static const char col_dark_1[]           = "#232433";
static const char col_dark_2[]           = "#2a2b3d";
static const char col_dark_3[]           = "#3b3d57";
static const char col_fg[]               = "#a9b1d6";
static const char col_black[]            = "#32344a";
static const char col_red[]              = "#f7768e";
static const char col_green[]            = "#9ece6a";
static const char col_yellow[]           = "#e0af68";
static const char col_blue[]             = "#7aa2f7";
static const char col_magenta[]          = "#ad8ee6";
static const char col_cyan[]             = "#449dab";
static const char col_white[]            = "#787c99";
static const char col_br_black[]         = "#444b6a";
static const char col_br_red[]           = "#ff7a93";
static const char col_br_green[]         = "#b9f27c";
static const char col_br_yellow[]        = "#ff9e64";
static const char col_br_blue[]          = "#7da6ff";
static const char col_br_magenta[]       = "#bb9af7";
static const char col_br_cyan[]          = "#0db9d7";
static const char col_br_white[]         = "#acb0d0";
static const char *colors[][3]           = {
	/*                     fg              bg              border   */
	[SchemeNorm]       = { col_fg,         col_bg,         col_black },      /* \x0b */
	[SchemeSel]        = { col_blue,       col_dark,       col_br_black },   /* \x0c */
        [SchemeDarker]     = { col_dark,       col_bg,         col_black },      /* \x0d */
        [SchemeRed]        = { col_red,        col_bg,         col_black },      /* \x0e */
        [SchemeGreen]      = { col_green,      col_bg,         col_black },      /* \x0f */
        [SchemeBlue]       = { col_blue,       col_bg,         col_black },      /* \x10 */
        [SchemeCyan]       = { col_cyan,       col_bg,         col_black },      /* \x11 */
        [SchemeMagenta]    = { col_magenta,    col_bg,         col_black },      /* \x12 */
        [SchemeYellow]     = { col_yellow,     col_bg,         col_black },      /* \x13 */
        [SchemeBlack]      = { col_black,      col_bg,         col_black },      /* \x14 */
        [SchemeWhite]      = { col_white,      col_bg,         col_black },      /* \x15 */
        [SchemeBrRed]      = { col_br_red,     col_bg,         col_black },      /* \x16 */
        [SchemeBrGreen]    = { col_br_green,   col_bg,         col_black },      /* \x17 */
        [SchemeBrBlue]     = { col_br_blue,    col_bg,         col_black },      /* \x18 */
        [SchemeBrCyan]     = { col_br_cyan,    col_bg,         col_black },      /* \x19 */
        [SchemeBrMagenta]  = { col_br_magenta, col_bg,         col_black },      /* \x1a */
        [SchemeBrYellow]   = { col_br_yellow,  col_bg,         col_black },      /* \x1b */
        [SchemeBrBlack]    = { col_br_black,   col_bg,         col_black },      /* \x1c */
        [SchemeBrWhite]    = { col_br_white,   col_bg,         col_black },      /* \x1d */
        [SchemeInactive]   = { col_br_black,   col_bg,         col_black },      /* \x1e */
        [SchemeTag]        = { col_br_black,   col_bg,         col_black },
        [SchemeTag1]       = { col_fg,         col_dark,       col_black },
	[SchemeTag2]       = { col_red,        col_dark,       col_black },
        [SchemeTag3]       = { col_br_yellow,  col_dark,       col_black },
        [SchemeTag4]       = { col_br_blue,    col_dark,       col_black },
        [SchemeTag5]       = { col_br_magenta, col_dark,       col_black },
	[SchemeTag6]       = { col_br_cyan,    col_dark,       col_black },
	[SchemeTag7]       = { col_br_green,   col_dark,       col_black },
	[SchemeTag8]       = { col_yellow,     col_dark,       col_black },
	[SchemeTag9]       = { col_br_red,     col_dark,       col_black },
	[SchemeLayout]     = { col_white,      col_bg,         col_black },
	[SchemeTitle]      = { col_fg,         col_bg,         col_black },
	[SchemeTitleFloat] = { col_br_blue,    col_bg,         col_black },
	[SchemeTitle1]     = { col_fg,         col_bg,         col_black },
	[SchemeTitle2]     = { col_red,        col_bg,         col_black },
        [SchemeTitle3]     = { col_br_yellow,  col_bg,         col_black },
        [SchemeTitle4]     = { col_br_blue,    col_bg,         col_black },
        [SchemeTitle5]     = { col_br_magenta, col_bg,         col_black },
	[SchemeTitle6]     = { col_br_cyan,    col_bg,         col_black },
	[SchemeTitle7]     = { col_br_green,   col_bg,         col_black },
	[SchemeTitle8]     = { col_yellow,     col_bg,         col_black },
	[SchemeTitle9]     = { col_br_red,     col_bg,         col_black },
        [SchemeFloat]      = { col_fg,         col_bg,         col_br_black },
};

/* tagging */
static const char *tags[] = { 
                                "  ", "  ", "  ", "  " , "  ", 
                                "  ", "  ", "  ", "  " 
};

static const char *tagsalt[] = {
                                " 1 ", " 2 ", " 3 ", " 4 ", " 5 ",
                                " 6 ", " 7 ", " 8 ", " 9 "
};

static const int tagschemes[] = { SchemeTag1, SchemeTag2, SchemeTag3,
                                  SchemeTag4, SchemeTag5, SchemeTag6,
                                  SchemeTag7, SchemeTag8, SchemeTag9 };

static const int titleschemes[] = { SchemeTitle1, SchemeTitle2, SchemeTitle3,
                                    SchemeTitle4, SchemeTitle5, SchemeTitle6,
                                    SchemeTitle7, SchemeTitle8, SchemeTitle9 };

static const int swallowfloating    = 0; /* 1 means swallow floating windows by default */
static int floatposgrid_x           = 5; /* float grid columns */
static int floatposgrid_y           = 5; /* float grid rows */

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 *      WM_WINDOW_ROLE(STRING) = role
	 */
	/* class                      role                 instance         title            tags mask  isfloating  isterminal  noswallow  floatpos (x y w h)         monitor */
	{ "Alacritty",                NULL,                NULL,            NULL,            0,         0,          1,          1,         NULL,                      -1 },
	{ "Float Term",               NULL,                NULL,            NULL,            0,         1,          1,          0,         NULL,                      -1 },
	{ NULL,                       NULL,                "File Manager",  NULL,            1 << 3,    1,          1,          0,         NULL,                      -1 },
	{ "discord",                  NULL,                NULL,            NULL,            1 << 6,    1,          0,          0,         "   0x   50%   70%  100%", -1 },
	{ NULL,                       NULL,                NULL,            "Event Tester",  0,         1,          0,          1,         NULL,                      -1 },
        { "firefoxdeveloperedition",  NULL,                NULL,            NULL,            1 << 2,    0,          0,          1,         NULL,                      -1 },
        { "firefoxdeveloperedition",  "Organizer",         NULL,            NULL,            1 << 2,    1,          0,          1,         NULL,                      -1 },
        { "firefoxdeveloperedition",  "PictureInPicture",  NULL,            NULL,            1 << 2,    1,          0,          1,         NULL,                      -1 },
	{ "Gimp",                     NULL,                NULL,            NULL,            0,         1,          0,          0,         NULL,                      -1 },
	{ "jetbrains-idea",           NULL,                NULL,            NULL,            0,         1,          0,          1,         NULL,                      -1 },
	{ "jetbrains-studio",         NULL,                NULL,            NULL,            0,         1,          0,          1,         NULL,                      -1 },
	{ "mpv",                      NULL,                NULL,            NULL,            0,         1,          0,          0,         NULL,                      -1 },
	{ "qutebrowser",              NULL,                NULL,            NULL,            1 << 2,    0,          0,          1,         NULL,                      -1 },
	{ "scrcpy",                   NULL,                NULL,            NULL,            0,         1,          0,          1,         NULL,                      -1 },
	{ "Spotify",                  NULL,                NULL,            NULL,            1 << 5,    0,          0,          1,         NULL,                      -1 },
	{ "st",                       NULL,                NULL,            NULL,            0,         0,          1,          1,         NULL,                      -1 },
	{ "svkbd",                    NULL,                NULL,            NULL,            0,         1,          0,          1,         "  50%  100%   50%   50%", -1 },
	{ "TelegramDesktop",          NULL,                NULL,            NULL,            1 << 6,    1,          0,          0,         " 100%   50%   30%  100%", -1 },
	{ "Tor Browser",              NULL,                NULL,            NULL,            1 << 2,    1,          0,          1,         NULL,                      -1 },
	{ "tty-clock",                NULL,                NULL,            NULL,            0,         1,          0,          1,         " 100%    0y  440W  190H", -1 },
};

/* layout(s) */
static const float mfact     = 0.50; /* factor of master area size [0.05..0.95] */
static const int nmaster     = 1;    /* number of clients in master area */
static const int resizehints = 0;    /* 1 means respect size hints in tiled resizals */
static const int decorhints  = 1;    /* 1 means respect decoration hints */

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "[]=",      tile },    /* first entry is default */
	{ "><>",      NULL },    /* no layout function means floating behavior */
	{ "[M]",      monocle },
        { "[@]",      dwindle },
        { ":::",      gaplessgrid },
        { "---",      horizgrid },
        { "TTT",      bstack },
        { "|M|",      centeredmaster },
        { ">M>",      centeredfloatingmaster },
        { NULL,       NULL }
};

/* default layout per tags */
/* The first element is for all-tag view, following i-th element corresponds to */
/* tags[i]. Layout is referred using the layouts array index.*/
static int taglayouts[1 + LENGTH(tags)]  = { 4, 0, 0, 2, 0, 0, 2, 0, 0, 0};


/* key definitions */
#define MOD Mod4Mask
#define ALT Mod1Mask
#define SHIFT ShiftMask
#define CTRL  ControlMask
#define TAGKEYS(KEY,TAG) \
	{ MOD,            KEY,      view,           {.ui = 1 << TAG} }, \
	{ MOD|CTRL,       KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MOD|SHIFT,      KEY,      tag,            {.ui = 1 << TAG} }, \
	{ MOD|ALT,        KEY,      tagview,        {.ui = 1 << TAG} }, \
	{ MOD|CTRL|SHIFT, KEY,      toggletag,      {.ui = 1 << TAG} },

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[] = { 
        "dmenu_run", 
        "-m", dmenumon, 
        "-fn", dmenufont, 
        "-h", "32",
        "-i", "-c", NULL };
static const char *termcmd[]  = { "alacritty", NULL };

static Key keys[] = {
	/* modifier           key              function             argument */
	{ ALT,                XK_space,        spawn,               {.v = dmenucmd } },
	{ MOD,                XK_space,        spawn,               {.v = termcmd } },
	{ MOD,                XK_b,            togglebar,           {0} },
	{ MOD,                XK_Return,       focusmaster,         {0} },
	{ MOD,                XK_j,            focusstack,          {.i = +1 } },
	{ MOD,                XK_k,            focusstack,          {.i = -1 } },
	{ MOD,                XK_i,            incnmaster,          {.i = +1 } },
	{ MOD,                XK_d,            incnmaster,          {.i = -1 } },
	{ MOD|CTRL,           XK_j,            inplacerotate,       {.i = +1 } },
	{ MOD|CTRL,           XK_k,            inplacerotate,       {.i = -1 } },
	{ MOD|CTRL,           XK_h,            inplacerotate,       {.i = +2 } },
	{ MOD|CTRL,           XK_l,            inplacerotate,       {.i = -2 } },
	{ MOD,                XK_h,            setmfact,            {.f = -0.01 } },
	{ MOD,                XK_l,            setmfact,            {.f = +0.01 } },
	{ MOD,                XK_o,            setmfact,            {.f = mfact } },
	{ MOD|SHIFT,          XK_h,            setcfact,            {.f = -0.05 } },
	{ MOD|SHIFT,          XK_l,            setcfact,            {.f = +0.05 } },
	{ MOD|SHIFT,          XK_o,            setcfact,            {.f =  0.00 } },
	{ MOD|CTRL,           XK_o,            setcfact,            {.f =  1.00 } },
	{ MOD|CTRL|SHIFT,     XK_o,            resetfact,           {0} },
        { MOD|ALT,            XK_0,            togglegaps,          {0} },
        { MOD|CTRL,           XK_0,            togglepertaggaps,    {0} },
        { MOD|SHIFT,          XK_equal,        defaultgaps,         {0} },
	{ MOD,                XK_equal,        incrgaps,            {.i = +1 } },
	{ MOD,                XK_minus,        incrgaps,            {.i = -1 } },
	{ MOD|ALT,            XK_i,            incrigaps,           {.i = +1 } },
	{ MOD|ALT|SHIFT,      XK_i,            incrigaps,           {.i = -1 } },
	{ MOD|ALT,            XK_o,            incrogaps,           {.i = +1 } },
	{ MOD|ALT|SHIFT,      XK_o,            incrogaps,           {.i = -1 } },
	{ MOD|ALT,            XK_x,            incrihgaps,          {.i = +1 } },
	{ MOD|ALT|SHIFT,      XK_x,            incrihgaps,          {.i = -1 } },
	{ MOD|ALT,            XK_y,            incrivgaps,          {.i = +1 } },
	{ MOD|ALT|SHIFT,      XK_y,            incrivgaps,          {.i = -1 } },
	{ MOD|CTRL,           XK_x,            incrohgaps,          {.i = +1 } },
	{ MOD|CTRL|SHIFT,     XK_x,            incrohgaps,          {.i = -1 } },
	{ MOD|CTRL,           XK_y,            incrovgaps,          {.i = +1 } },
	{ MOD|CTRL|SHIFT,     XK_y,            incrovgaps,          {.i = -1 } },
	{ MOD|SHIFT,          XK_Return,       zoom,                {0} },
	{ MOD,                XK_Tab,          view,                {0} },
	{ MOD|SHIFT,          XK_c,            killclient,          {0} },
	{ MOD|CTRL,           XK_comma,        cyclelayout,         {.i = -1 } },
	{ MOD|CTRL,           XK_period,       cyclelayout,         {.i = +1 } },
        { MOD,                XK_t,            setlayout,           {.v = &layouts[0] } }, /* tile */
        { MOD,                XK_f,            setlayout,           {.v = &layouts[1] } }, /* float */
        { MOD,                XK_m,            setlayout,           {.v = &layouts[2] } }, /* monocle */
        { MOD|ALT,            XK_t,            setlayout,           {.v = &layouts[3] } }, /* dwindle */
        { MOD,                XK_g,            setlayout,           {.v = &layouts[4] } }, /* gaplessgrid */
        { MOD|ALT,            XK_g,            setlayout,           {.v = &layouts[5] } }, /* horizgrid */
        { MOD|ALT,            XK_b,            setlayout,           {.v = &layouts[6] } }, /* bstack */
	{ MOD|CTRL,           XK_c,            setlayout,           {.v = &layouts[7] } }, /* centeredmaster */
	{ MOD|ALT,            XK_c,            setlayout,           {.v = &layouts[8] } }, /* centeredfloatingmaster */
	{ MOD|CTRL,           XK_space,        setlayout,           {0} },
	{ MOD|SHIFT,          XK_space,        togglefloating,      {.v = " 50%  50%" } },
	{ MOD,                XK_0,            view,                {.ui = ~0 } },
	{ MOD|SHIFT,          XK_0,            tag,                 {.ui = ~0 } },
	{ MOD,                XK_n,            togglealttag,        {0} },
	{ MOD|CTRL|SHIFT,     XK_n,            togglecentertitle,   {0} },
	{ MOD|SHIFT,          XK_n,            togglecolorfultag,   {0} },
	{ MOD|CTRL,           XK_n,            togglecolorfultitle, {0} },
	{ MOD|ALT,            XK_n,            toggletitle,         {0} },
	{ MOD|ALT|SHIFT,      XK_n,            togglevacanttag,     {0} },
	{ ALT|SHIFT,          XK_bracketright, shiftview,           {.i = +1 } },
	{ ALT|SHIFT,          XK_bracketleft,  shiftview,           {.i = -1 } },
	{ MOD|SHIFT,          XK_bracketright, shiftclient,         {.i = +1 } },
	{ MOD|SHIFT,          XK_bracketleft,  shiftclient,         {.i = -1 } },
	{ MOD,                XK_comma,        focusmon,            {.i = -1 } },
	{ MOD,                XK_period,       focusmon,            {.i = +1 } },
	{ MOD|SHIFT,          XK_comma,        tagmon,              {.i = -1 } },
	{ MOD|SHIFT,          XK_period,       tagmon,              {.i = +1 } },
	/* Client position is limited to monitor window area */
	{ SHIFT|ALT,          XK_k,            floatpos,            {.v = " 0x -8y" } }, // ↑
	{ SHIFT|ALT,          XK_h,            floatpos,            {.v = "-8x  0y" } }, // ←
	{ SHIFT|ALT,          XK_l,            floatpos,            {.v = " 8x  0y" } }, // →
	{ SHIFT|ALT,          XK_j,            floatpos,            {.v = " 0x  8y" } }, // ↓
	/* Absolute positioning (allows moving windows between monitors) */
	{ MOD|SHIFT|ALT,      XK_k,            floatpos,            {.v = " 0a -8a" } }, // ↑
	{ MOD|SHIFT|ALT,      XK_h,            floatpos,            {.v = "-8a  0a" } }, // ←
	{ MOD|SHIFT|ALT,      XK_l,            floatpos,            {.v = " 8a  0a" } }, // →
	{ MOD|SHIFT|ALT,      XK_j,            floatpos,            {.v = " 0a  8a" } }, // ↓
	/* Client is positioned in the edge or in the middle of the screen. */
        { MOD|ALT,            XK_k,            floatpos,            {.v = "  0x   0%" } }, // ↑
        { MOD|ALT,            XK_h,            floatpos,            {.v = "  0%   0y" } }, // ←
        { MOD,                XK_c,            floatpos,            {.v = " 50%  50%" } }, // ·
        { MOD|ALT,            XK_l,            floatpos,            {.v = "100%   0y" } }, // →
        { MOD|ALT,            XK_j,            floatpos,            {.v = "  0x 100%" } }, // ↓
	/* Resize, increase client size from every side*/
	{ CTRL|ALT,           XK_k,            floatpos,            {.v = "-1Z -1Z  0w  8h" } }, // ↑
	{ CTRL|ALT,           XK_h,            floatpos,            {.v = "-1Z -1Z  8w  0h" } }, // ←
        { CTRL|ALT,           XK_l,            floatpos,            {.v = "-1S -1S  8w  0h" } }, // →
	{ CTRL|ALT,           XK_j,            floatpos,            {.v = "-1S -1S  0w  8h" } }, // ↓
	/* Resize, decrease client size from every side*/
	{ MOD|CTRL|ALT,       XK_j,            floatpos,            {.v = "-1Z -1Z  0w -8h" } }, // ↑
	{ MOD|CTRL|ALT,       XK_l,            floatpos,            {.v = "-1Z -1Z -8w  0h" } }, // ←
        { MOD|CTRL|ALT,       XK_h,            floatpos,            {.v = "-1S -1S -8w  0h" } }, // →
	{ MOD|CTRL|ALT,       XK_k,            floatpos,            {.v = "-1S -1S  0w -8h" } }, // ↓
        /* Unify Resize */
        { MOD|CTRL|ALT|SHIFT, XK_k,            floatpos,            {.v = " 8w  8h" } }, // 
        { CTRL|ALT|SHIFT,     XK_l,            floatpos,            {.v = " 8w  0h" } }, // ←→
        { CTRL|ALT|SHIFT,     XK_k,            floatpos,            {.v = " 0w  8h" } }, // ↑↓
        { MOD|CTRL|ALT|SHIFT, XK_j,            floatpos,            {.v = "-8w -8h" } }, // 
        { CTRL|ALT|SHIFT,     XK_h,            floatpos,            {.v = "-8w  0h" } }, // →←
        { CTRL|ALT|SHIFT,     XK_j,            floatpos,            {.v = " 0w -8h" } }, // ↓↑
        /* Maximize the client in any given direction */
        { CTRL|SHIFT,         XK_k,            floatpos,            {.v = " 0x  0Z   0%   0%" } }, // ↑
        { CTRL|SHIFT,         XK_h,            floatpos,            {.v = " 0Z  0y   0%   0%" } }, // ←
        { MOD|CTRL|SHIFT,     XK_c,            floatpos,            {.v = "50% 50%  80%  80%" } }, // ·
        { CTRL|SHIFT,         XK_l,            floatpos,            {.v = "-1S  0y 100%   0%" } }, // →
        { CTRL|SHIFT,         XK_j,            floatpos,            {.v = " 0x -1S   0% 100%" } }, // ↓
	TAGKEYS(                        XK_1,                      0)
	TAGKEYS(                        XK_2,                      1)
	TAGKEYS(                        XK_3,                      2)
	TAGKEYS(                        XK_4,                      3)
	TAGKEYS(                        XK_5,                      4)
	TAGKEYS(                        XK_6,                      5)
	TAGKEYS(                        XK_7,                      6)
	TAGKEYS(                        XK_8,                      7)
	TAGKEYS(                        XK_9,                      8)
	{ MOD|SHIFT,                    XK_q,      quit,           {0} },
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static Button buttons[] = {
	/* click                event mask   button          function        argument */
	{ ClkLtSymbol,          0,           Button1,        setlayout,      {0} },
	{ ClkLtSymbol,          0,           Button3,        setlayout,      {.v = &layouts[2]} },
	{ ClkWinTitle,          0,           Button2,        zoom,           {0} },
	{ ClkStatusText,        0,           Button1,        sigdwmblocks,   {.i = 1} },
	{ ClkStatusText,        0,           Button2,        sigdwmblocks,   {.i = 2} },
	{ ClkStatusText,        0,           Button3,        sigdwmblocks,   {.i = 3} },
	{ ClkClientWin,         MOD,         Button1,        movemouse,      {0} },
	{ ClkClientWin,         MOD,         Button2,        togglefloating, {0} },
	{ ClkClientWin,         MOD,         Button3,        resizemouse,    {0} },
	{ ClkTagBar,            0,           Button1,        view,           {0} },
	{ ClkTagBar,            0,           Button3,        toggleview,     {0} },
	{ ClkTagBar,            MOD,         Button1,        tag,            {0} },
	{ ClkTagBar,            MOD,         Button3,        toggletag,      {0} },
        { ClkTagBar,	        0,           Button4,        shiftview,      {.i = -1} },
	{ ClkTagBar,	        0,	     Button5,        shiftview,      {.i = 1} },
};

