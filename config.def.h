/* See LICENSE file for copyright and license details. */

/* appearance */
static const unsigned int borderpx  = 1;        /* border pixel of windows */
static const unsigned int snap      = 8;        /* snap pixel */
/* vanitygaps */
static const unsigned int gappih    = 8;       /* horiz inner gap between windows */
static const unsigned int gappiv    = 8;       /* vert inner gap between windows */
static const unsigned int gappoh    = 8;       /* horiz outer gap between windows and screen edge */
static const unsigned int gappov    = 8;       /* vert outer gap between windows and screen edge */
static       int smartgaps          = 0;        /* 1 means no outer gap when there is only one window */
static const unsigned int barheight = 29;       /* 0 means default height*/
static const int showbar            = 1;        /* 0 means no bar */
static const int topbar             = 1;        /* 0 means bottom bar */
static const char *fonts[]          = { "SF Mono:style=Medium:size=10" };
static const char dmenufont[]       = "SF Mono:style=Medium:size=10";
static const char col_bg[]          = "#1a1b26";
static const char col_dark[]        = "#16161E";
static const char col_fg[]          = "#a9b1d6";
static const char col_black[]       = "#32344a";
static const char col_red[]         = "#f7768e";
static const char col_green[]       = "#9ece6a";
static const char col_yellow[]      = "#e0af68";
static const char col_blue[]        = "#7aa2f7";
static const char col_magenta[]     = "#ad8ee6";
static const char col_cyan[]        = "#449dab";
static const char col_white[]       = "#787c99";
static const char col_br_black[]    = "#444b6a";
static const char col_br_red[]      = "#ff7a93";
static const char col_br_green[]    = "#b9f27c";
static const char col_br_yellow[]   = "#ff9e64";
static const char col_br_blue[]     = "#7da6ff";
static const char col_br_magenta[]  = "#bb9af7";
static const char col_br_cyan[]     = "#0db9d7";
static const char col_br_white[]    = "#acb0d0";
static const char *colors[][3]      = {
	/*               fg         bg         border   */
	[SchemeNorm] = { col_fg,    col_bg,    col_black },
	[SchemeSel]  = { col_blue,  col_dark,  col_br_black  },
};

/* tagging */
static const char *tags[] = { 
                                "  ", "  ", "  ", "  " , "  ", 
                                "  ", "  ", "  ", "  " 
};

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class                      instance         title            tags mask  isfloating   monitor */
	{ "Alacritty",                NULL,            NULL,            0,         0,           -1 },
	{ "Float Term",               NULL,            NULL,            0,         1,           -1 },
	{ NULL,                       "File Manager",  NULL,            1 << 3,    1,           -1 },
	{ "discord",                  NULL,            NULL,            1 << 6,    1,           -1 },
	{ NULL,                       NULL,            "Event Tester",  0,         1,           -1 },
        { "firefoxdeveloperedition",  NULL,            NULL,            1 << 2,    0,           -1 },
	{ "Gimp",                     NULL,            NULL,            0,         1,           -1 },
	{ "jetbrains-idea",           NULL,            NULL,            0,         1,           -1 },
	{ "jetbrains-studio",         NULL,            NULL,            0,         1,           -1 },
	{ "mpv",                      NULL,            NULL,            0,         1,           -1 },
	{ "qutebrowser",              NULL,            NULL,            1 << 2,    0,           -1 },
	{ "scrcpy",                   NULL,            NULL,            0,         1,           -1 },
	{ "Spotify",                  NULL,            NULL,            1 << 5,    0,           -1 },
	{ "st",                       NULL,            NULL,            0,         0,           -1 },
	{ "TelegramDesktop",          NULL,            NULL,            1 << 6,    1,           -1 },
	{ "Tor Browser",              NULL,            NULL,            1 << 2,    1,           -1 }
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
	/* modifier           key              function        argument */
	{ ALT,                XK_space,        spawn,          {.v = dmenucmd } },
	{ MOD,                XK_space,        spawn,          {.v = termcmd } },
	{ MOD,                XK_b,            togglebar,      {0} },
	{ MOD,                XK_j,            focusstack,     {.i = +1 } },
	{ MOD,                XK_k,            focusstack,     {.i = -1 } },
	{ MOD,                XK_i,            incnmaster,     {.i = +1 } },
	{ MOD,                XK_d,            incnmaster,     {.i = -1 } },
	{ MOD,                XK_h,            setmfact,       {.f = -0.05} },
	{ MOD,                XK_l,            setmfact,       {.f = +0.05} },
	{ MOD|SHIFT,          XK_h,            setcfact,       {.f = +0.25} },
	{ MOD|SHIFT,          XK_l,            setcfact,       {.f = -0.25} },
	{ MOD|SHIFT,          XK_o,            setcfact,       {.f =  0.00} },
	{ MOD,                XK_o,            resetcfact,     {0} },
        { MOD|ALT,            XK_0,            togglegaps,     {0} },
        { MOD|SHIFT,          XK_equal,        defaultgaps,    {0} },
	{ MOD,                XK_equal,        incrgaps,       {.i = +1 } },
	{ MOD,                XK_minus,        incrgaps,       {.i = -1 } },
	{ MOD|ALT,            XK_i,            incrigaps,      {.i = +1 } },
	{ MOD|ALT|SHIFT,      XK_i,            incrigaps,      {.i = -1 } },
	{ MOD|ALT,            XK_o,            incrogaps,      {.i = +1 } },
	{ MOD|ALT|SHIFT,      XK_o,            incrogaps,      {.i = -1 } },
	{ MOD|ALT,            XK_x,            incrihgaps,     {.i = +1 } },
	{ MOD|ALT|SHIFT,      XK_x,            incrihgaps,     {.i = -1 } },
	{ MOD|ALT,            XK_y,            incrivgaps,     {.i = +1 } },
	{ MOD|ALT|SHIFT,      XK_y,            incrivgaps,     {.i = -1 } },
	{ MOD|CTRL,           XK_x,            incrohgaps,     {.i = +1 } },
	{ MOD|CTRL|SHIFT,     XK_x,            incrohgaps,     {.i = -1 } },
	{ MOD|CTRL,           XK_y,            incrovgaps,     {.i = +1 } },
	{ MOD|CTRL|SHIFT,     XK_y,            incrovgaps,     {.i = -1 } },
	{ MOD,                XK_Return,       zoom,           {0} },
	{ MOD,                XK_Tab,          view,           {0} },
	{ MOD|SHIFT,          XK_c,            killclient,     {0} },
        { MOD,                XK_t,            setlayout,      {.v = &layouts[0]} }, /* tile */
        { MOD,                XK_f,            setlayout,      {.v = &layouts[1]} }, /* float */
        { MOD,                XK_m,            setlayout,      {.v = &layouts[2]} }, /* monocle */
        { MOD|ALT,            XK_t,            setlayout,      {.v = &layouts[3]} }, /* dwindle */
        { MOD,                XK_g,            setlayout,      {.v = &layouts[4]} }, /* gaplessgrid */
        { MOD|ALT,            XK_g,            setlayout,      {.v = &layouts[5]} }, /* horizgrid */
        { MOD|ALT,            XK_b,            setlayout,      {.v = &layouts[6]} }, /* bstack */
	{ MOD|CTRL,           XK_c,            setlayout,      {.v = &layouts[7]} }, /* centeredmaster */
	{ MOD|ALT,            XK_c,            setlayout,      {.v = &layouts[8]} }, /* centeredfloatingmaster */
	{ MOD|CTRL,           XK_space,        setlayout,      {0} },
	{ MOD|SHIFT,          XK_space,        togglefloating, {0} },
	{ MOD,                XK_0,            view,           {.ui = ~0 } },
	{ MOD|SHIFT,          XK_0,            tag,            {.ui = ~0 } },
	{ ALT|SHIFT,          XK_bracketright, shiftview,      {.i = +1  } },
	{ ALT|SHIFT,          XK_bracketleft,  shiftview,      {.i = -1  } },
	{ MOD|SHIFT,          XK_bracketright, shiftclient,    {.i = +1  } },
	{ MOD|SHIFT,          XK_bracketleft,  shiftclient,    {.i = -1  } },
	{ MOD,                XK_comma,        focusmon,       {.i = -1 } },
	{ MOD,                XK_period,       focusmon,       {.i = +1 } },
	{ MOD|SHIFT,          XK_comma,        tagmon,         {.i = -1 } },
	{ MOD|SHIFT,          XK_period,       tagmon,         {.i = +1 } },
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
	{ ClkStatusText,        0,           Button2,        spawn,          {.v = termcmd } },
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

