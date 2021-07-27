/* See LICENSE file for copyright and license details. */

/* behavior */
static const unsigned int snap      = 8;  /* snap pixel */
static const int lockfullscreen     = 1;  /* 1 will force focus on the fullscreen window */
/* appearance */
static const unsigned int borderpx  = 1;  /* border pixel of windows */
static const unsigned int barborder = 1;  /* border pixel of bar */
/* vanitygaps */
static const unsigned int gappih    = 8;  /* horiz inner gap between windows */
static const unsigned int gappiv    = 8;  /* vert inner gap between windows */
static const unsigned int gappoh    = 8;  /* horiz outer gap between windows and screen edge */
static const unsigned int gappov    = 8;  /* vert outer gap between windows and screen edge */
static       int smartgaps          = 0;  /* 1 means no outer gap when there is only one window */
static       int pertaggap          = 1;  /* 0 means all tag have same gap */
static       int edgegap            = 1;  /* 1 means floating client respect the outer gap */
/* bar */
static const unsigned int barheight = 29; /* 0 means default height*/
static const unsigned int vertpad   = 8;  /* vertical padding of bar */
static const unsigned int sidepad   = 8;  /* horizontal padding of bar */
static const int showbar            = 1;  /* 0 means no bar */
static const int topbar             = 1;  /* 0 means bottom bar */
static const int bargap             = 1;  /* 0 means vertpad & sidepad ignored */
static const int centertitle        = 1;  /* 0 means left title */
static const int colorfultitle      = 1;  /* 0 means title use SchemeTitle and SchemeTitleFloat */
static const int colorfultag        = 1;  /* 0 means use SchemeSel for selected non vacant tag */
static const int showindicator      = 0;  /* 0 means no line indicator for selected non vacant tag */
static const int showtitle          = 1;  /* 0 means no title */
static const int showvacanttags     = 0;  /* 0 means hide vacant tags */
/* font and color */
static const char *fonts[]               = {
        "SF Mono:style=Medium:size=10",
        "Material Design Icons Desktop:style=Regular:size=10"
};
static const char dmenufont[]            = "SF Mono:style=Medium:size=10";
static const char col_bg[]               = "#1a1b26";
static const char col_dark[]             = "#16161E";
static const char col_dark_1[]           = "#232433";
static const char col_dark_2[]           = "#2a2b3d";
static const char col_dark_3[]           = "#3b3d57";
static const char col_fg[]               = "#a9b1d6";
static const char col_black[]            = "#32344a";
static const char col_br_black[]         = "#444b6a";
static const char col_white[]            = "#787c99";
static const char col_br_white[]         = "#acb0d0";
static const char col_red[]              = "#f7768e";
static const char col_br_red[]           = "#ff7a93";
static const char col_green[]            = "#9ece6a";
static const char col_br_green[]         = "#b9f27c";
static const char col_yellow[]           = "#e0af68";
static const char col_br_yellow[]        = "#ff9e64";
static const char col_blue[]             = "#7aa2f7";
static const char col_br_blue[]          = "#7da6ff";
static const char col_magenta[]          = "#ad8ee6";
static const char col_br_magenta[]       = "#bb9af7";
static const char col_cyan[]             = "#449dab";
static const char col_br_cyan[]          = "#0db9d7";
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
        [SchemeFloat]      = { col_fg,         col_bg,         col_blue },       /* \x1e */
        [SchemeInactive]   = { col_br_black,   col_bg,         col_black },      /* \x1f */
        [SchemeBar]        = { col_fg,         col_bg,         col_black },
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
};

/* tagging */
static const char *tags[] = {
        "  ", "  ", "  ", "  " , "  ",
        "  ", "  ", "  ", "  "
};

static const char *tagsalt[] = {
        " 1 ", " 2 ", " 3 ", " 4 ", " 5 ",
        " 6 ", " 7 ", " 8 ", " 9 "
};

static const int tagschemes[] = {
        SchemeTag1, SchemeTag2, SchemeTag3,
        SchemeTag4, SchemeTag5, SchemeTag6,
        SchemeTag7, SchemeTag8, SchemeTag9
};

static const int titleschemes[] = {
        SchemeTitle1, SchemeTitle2, SchemeTitle3,
        SchemeTitle4, SchemeTitle5, SchemeTitle6,
        SchemeTitle7, SchemeTitle8, SchemeTitle9
};

static const int swallowfloating    = 0; /* 1 means swallow floating windows by default */
static int floatposgrid_x           = 5; /* float grid columns */
static int floatposgrid_y           = 5; /* float grid rows */

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 *      WM_WINDOW_ROLE(STRING) = role
	 *	_NET_WM_WINDOW_TYPE(ATOM) = wintype
	 *
         * RULE(
         *      .class = "class", .wintype = WTYPE "type", .role = "role",
         *      .instance = "instance", .title = "title",
         *      .tags = 0, .isfloating = 0, .isterminal = 0, .noswallow = -1,
         *      .floatpos = "x y w h", .monitor = -1
         * )
         */
        RULE(.class = "Alacritty", .isterminal = 1, .noswallow = 1)
        RULE(.class = "codespace", .tags = 1 << 1)
        RULE(.title = "Discord Updater", .tags = 1 << 6, .matchonce = 1,
                .floatpos = "50% 50%")
        RULE(.class = "discord", .tags = 1 << 6, .floatpos = "0% 50% 70% 100%")
        RULE(.title = "Event Tester", .isfloating = 1, .noswallow = 1)
        RULE(.class = "File Manager", .tags = 1 << 3, .isfloating = 1,
                .isterminal = 1)
        RULE(.class = "firefoxdeveloperedition", .tags = 1 << 2, .noswallow = 1)
        RULE(.class = "firefoxdeveloperedition", .role = "Organizer",
                .isfloating = 1)
        RULE(.class = "Float Term", .isfloating = 1, .isterminal = 1)
        RULE(.class = "Gimp", .isfloating = 1)
        RULE(.class = "jetbrains-idea", .isfloating = 1, .noswallow = 1)
        RULE(.class = "jetbrains-studio", .isfloating = 1, .noswallow = 1)
        RULE(.title = "LibreOffice", .tags = 1 << 4, .noswallow = 1)
        RULE(.class = "mpv", .isfloating = 1)
        RULE(.class = "Notify Term", .floatpos = "100% 0y")
        RULE(.class = "qutebrowser", .tags = 1 << 2, .noswallow = 1)
        RULE(.class = "scrcpy", .isfloating = 1, .noswallow = 1)
        RULE(.class = "SimpleScreenRecorder", .tags = 1 << 8, .isfloating = 1)
        RULE(.class = "Spotify", .tags = 1 << 5, .noswallow = 1)
        RULE(.class = "st", .isterminal = 1, .noswallow = 1)
        RULE(.class = "svkbd", .floatpos = "50% 100% 50% 50%")
        RULE(.class = "TelegramDesktop", .tags = 1 << 6,
                .floatpos = "100% 50% 30% 100%")
        RULE(.class = "Tor Browser", .tags = 1 << 2, .isfloating = 1,
                .noswallow = 1)
        RULE(.class = "zoom", .isfloating = 1)
        RULE(.wintype = WTYPE "UTILITY", .isfloating = 1)
};

/* layout(s) */
static const float mfact     = 0.50; /* factor of master area size [0.05..0.95] */
static const int nmaster     = 1;    /* number of clients in master area */
static const int resizehints = 1;    /* 1 means respect size hints in tiled resizals */
static const int decorhints  = 1;    /* 1 means respect decoration hints */

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "[]=",      tile },    /* first entry is default */
	{ "><>",      NULL },    /* no layout function means floating behavior */
	{ "[M]",      monocle },
        { "[@]",      dwindle },
        { ":::",      gaplessgrid },
        { "TTT",      bstack },
        { "|M|",      centeredmaster },
        { ">M>",      centeredfloatingmaster },
        { NULL,       NULL }
};

/* default layout per tags */
/* The first element is for all-tag view, following i-th element corresponds to */
/* tags[i]. Layout is referred using the layouts array index.*/
static int taglayouts[1 + LENGTH(tags)]  = { 4, 0, 0, 2, 1, 2, 2, 0, 6, 4 };

/* key definitions */
#define M Mod4Mask
#define A Mod1Mask
#define S ShiftMask
#define C ControlMask
#define TAGKEYS(KEY,TAG) \
	{ M,     KEY,  view,       {.ui = 1 << TAG} }, \
	{ M|C,   KEY,  toggleview, {.ui = 1 << TAG} }, \
	{ M|S,   KEY,  tag,        {.ui = 1 << TAG} }, \
	{ M|A,   KEY,  tagview,    {.ui = 1 << TAG} }, \
	{ M|C|S, KEY,  toggletag,  {.ui = 1 << TAG} },

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
	/* {
         *      modifier, key
         *      function, argument
         * }
         */
	{ M,      XK_p,            spawn,            {.v = dmenucmd } },
	{ M,      XK_space,        spawn,            {.v = termcmd } },
	{ M,      XK_b,            togglebar,        {0} },
        { M|C,    XK_b,            togglebargap,     {0} },
	{ M,      XK_Return,       focusmaster,      {0} },
	{ M,      XK_j,            focusstack,       {.i = +1 } },
	{ M,      XK_k,            focusstack,       {.i = -1 } },
	{ M,      XK_i,            incnmaster,       {.i = +1 } },
	{ M,      XK_d,            incnmaster,       {.i = -1 } },
	{ M|C,    XK_j,            inplacerotate,    {.i = +1 } },
	{ M|C,    XK_k,            inplacerotate,    {.i = -1 } },
	{ M|C,    XK_h,            inplacerotate,    {.i = +2 } },
	{ M|C,    XK_l,            inplacerotate,    {.i = -2 } },
	{ M,      XK_h,            setmfact,         {.f = -0.01 } },
	{ M,      XK_l,            setmfact,         {.f = +0.01 } },
	{ M,      XK_o,            setmfact,         {.f = mfact } },
	{ M|S,    XK_h,            setcfact,         {.f = -0.05 } },
	{ M|S,    XK_l,            setcfact,         {.f = +0.05 } },
	{ M|S,    XK_o,            setcfact,         {.f =  0.00 } },
	{ M|C,    XK_o,            setcfact,         {.f =  1.00 } },
	{ M|A,    XK_space,        resetfact,        {0} },
        { M|A,    XK_0,            togglegaps,       {0} },
        { M|C,    XK_0,            toggletaggaps,    {0} },
        { M|S,    XK_equal,        defaultgaps,      {0} },
	{ M,      XK_equal,        incrgaps,         {.i = +1 } },
	{ M,      XK_minus,        incrgaps,         {.i = -1 } },
	{ M|A,    XK_i,            incrigaps,        {.i = +1 } },
	{ M|A|S,  XK_i,            incrigaps,        {.i = -1 } },
	{ M|A,    XK_o,            incrogaps,        {.i = +1 } },
	{ M|A|S,  XK_o,            incrogaps,        {.i = -1 } },
	{ M|A,    XK_x,            incrihgaps,       {.i = +1 } },
	{ M|A|S,  XK_x,            incrihgaps,       {.i = -1 } },
	{ M|A,    XK_y,            incrivgaps,       {.i = +1 } },
	{ M|A|S,  XK_y,            incrivgaps,       {.i = -1 } },
	{ M|C,    XK_x,            incrohgaps,       {.i = +1 } },
	{ M|C|S,  XK_x,            incrohgaps,       {.i = -1 } },
	{ M|C,    XK_y,            incrovgaps,       {.i = +1 } },
	{ M|C|S,  XK_y,            incrovgaps,       {.i = -1 } },
	{ M|S,    XK_Return,       zoom,             {0} },
	{ M,      XK_Tab,          view,             {0} },
	{ M|S,    XK_c,            killclient,       {0} },
	{ M|C,    XK_comma,        cyclelayout,      {.i = -1 } },
	{ M|C,    XK_period,       cyclelayout,      {.i = +1 } },
        { M,      XK_t,            setlayout,        {.v = &layouts[0] } /* tile */ },
        { M,      XK_f,            setlayout,        {.v = &layouts[1] } /* float */ },
        { M,      XK_m,            setlayout,        {.v = &layouts[2] } /* monocle */ },
        { M|A,    XK_t,            setlayout,        {.v = &layouts[3] } /* dwindle */ },
        { M,      XK_g,            setlayout,        {.v = &layouts[4] } /* gaplessgrid */ },
        { M|A,    XK_b,            setlayout,        {.v = &layouts[5] } /* bstack */ },
	{ M|C,    XK_c,            setlayout,        {.v = &layouts[6] } /* centeredmaster */ },
	{ M|A,    XK_c,            setlayout,        {.v = &layouts[7] } /* centeredfloatingmaster */ },
	{ M|C,    XK_space,        setlayout,        {0} },
	{ M|S,    XK_space,        togglefloating,   {.v = " 50%  50%" } },
	{ M,      XK_0,            view,             {.ui = ~0 } },
	{ M|S,    XK_0,            tag,              {.ui = ~0 } },
	{ M,      XK_n,            togglealttag,     {0} },
	{ M|A|C,  XK_n,            toggleindicator,  {0} },
	{ M|S,    XK_n,            toggletagcolor,   {0} },
	{ M|A,    XK_n,            toggletitle,      {0} },
	{ M|C|S,  XK_n,            toggletitlepos,   {0} },
	{ M|C,    XK_n,            toggletitlecolor, {0} },
	{ M|A|S,  XK_n,            togglevacanttag,  {0} },
	{ A|S,    XK_bracketright, shiftview,        {.i = +1 } },
	{ A|S,    XK_bracketleft,  shiftview,        {.i = -1 } },
	{ M|S,    XK_bracketright, shiftclient,      {.i = +1 } },
	{ M|S,    XK_bracketleft,  shiftclient,      {.i = -1 } },
	{ M,      XK_comma,        focusmon,         {.i = -1 } },
	{ M,      XK_period,       focusmon,         {.i = +1 } },
	{ M|S,    XK_comma,        tagmon,           {.i = -1 } },
	{ M|S,    XK_period,       tagmon,           {.i = +1 } },
	/* Client position is limited to monitor window area */
        { S|A,    XK_k,            floatpos,         {.v = " 0x -8y" } }, // ↑
        { S|A,    XK_h,            floatpos,         {.v = "-8x  0y" } }, // ←
	{ S|A,    XK_l,            floatpos,         {.v = " 8x  0y" } }, // →
	{ S|A,    XK_j,            floatpos,         {.v = " 0x  8y" } }, // ↓
	/* Client is positioned in the edge or in the middle of the screen. */
        { M|A,    XK_k,            floatpos,         {.v = "  0x   0%" } }, // ↑
        { M|A,    XK_h,            floatpos,         {.v = "  0%   0y" } }, // ←
        { M,      XK_c,            floatpos,         {.v = " 50%  50%" } }, // ·
        { M|A,    XK_l,            floatpos,         {.v = "100%   0y" } }, // →
        { M|A,    XK_j,            floatpos,         {.v = "  0x 100%" } }, // ↓
	/* Resize, increase client size from every side*/
	{ C|A,    XK_k,            floatpos,         {.v = "-1Z -1Z  0w  8h" } }, // ↑
	{ C|A,    XK_h,            floatpos,         {.v = "-1Z -1Z  8w  0h" } }, // ←
        { C|A,    XK_l,            floatpos,         {.v = "-1S -1S  8w  0h" } }, // →
	{ C|A,    XK_j,            floatpos,         {.v = "-1S -1S  0w  8h" } }, // ↓
	/* Resize, decrease client size from every side*/
	{ M|C|A,  XK_j,            floatpos,         {.v = "-1Z -1Z  0w -8h" } }, // ↑
	{ M|C|A,  XK_l,            floatpos,         {.v = "-1Z -1Z -8w  0h" } }, // ←
        { M|C|A,  XK_h,            floatpos,         {.v = "-1S -1S -8w  0h" } }, // →
	{ M|C|A,  XK_k,            floatpos,         {.v = "-1S -1S  0w -8h" } }, // ↓
        /* Unify Resize */
        { M|A|S,  XK_k,            floatpos,         {.v = " 8w  8h" } }, // 
        { C|A|S,  XK_l,            floatpos,         {.v = " 8w  0h" } }, // ←→
        { C|A|S,  XK_k,            floatpos,         {.v = " 0w  8h" } }, // ↑↓
        { M|A|S,  XK_j,            floatpos,         {.v = "-8w -8h" } }, // 
        { C|A|S,  XK_h,            floatpos,         {.v = "-8w  0h" } }, // →←
        { C|A|S,  XK_j,            floatpos,         {.v = " 0w -8h" } }, // ↓↑
        /* Maximize the client in any given direction */
        { C|S,    XK_k,            floatpos,         {.v = " 0x  0Z   0%   0%" } }, // ↑
        { C|S,    XK_h,            floatpos,         {.v = " 0Z  0y   0%   0%" } }, // ←
        { M|C|S,  XK_c,            floatpos,         {.v = "50% 50%  80%  80%" } }, // ·
        { C|S,    XK_l,            floatpos,         {.v = "-1S  0y 100%   0%" } }, // →
        { C|S,    XK_j,            floatpos,         {.v = " 0x -1S   0% 100%" } }, // ↓
	{ M|S,    XK_q,            quit,             {0} },
	TAGKEYS(  XK_1,                              0)
	TAGKEYS(  XK_2,                              1)
	TAGKEYS(  XK_3,                              2)
	TAGKEYS(  XK_4,                              3)
	TAGKEYS(  XK_5,                              4)
	TAGKEYS(  XK_6,                              5)
	TAGKEYS(  XK_7,                              6)
	TAGKEYS(  XK_8,                              7)
	TAGKEYS(  XK_9,                              8)
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static Button buttons[] = {
	/* click         event mask   button   function        argument */
	{ ClkLtSymbol,   0,           Button1, setlayout,      {0} },
	{ ClkLtSymbol,   0,           Button3, setlayout,      {.v = &layouts[2]} },
	{ ClkWinTitle,   0,           Button2, zoom,           {0} },
	{ ClkStatusText, 0,           Button1, sigdwmblocks,   {.i = 1} },
	{ ClkStatusText, 0,           Button2, sigdwmblocks,   {.i = 2} },
	{ ClkStatusText, 0,           Button3, sigdwmblocks,   {.i = 3} },
	{ ClkClientWin,  M,           Button1, movemouse,      {0} },
	{ ClkClientWin,  M,           Button2, togglefloating, {0} },
	{ ClkClientWin,  M,           Button3, resizemouse,    {0} },
	{ ClkTagBar,     0,           Button1, view,           {0} },
	{ ClkTagBar,     0,           Button3, toggleview,     {0} },
	{ ClkTagBar,     M,           Button1, tag,            {0} },
	{ ClkTagBar,     M,           Button3, toggletag,      {0} },
        { ClkTagBar,	 0,           Button4, shiftview,      {.i = -1} },
	{ ClkTagBar,	 0,	      Button5, shiftview,      {.i = 1} },
};
