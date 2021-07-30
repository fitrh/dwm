/* See LICENSE file for copyright and license details.
 *
 * dynamic window manager is designed like any other X client as well. It is
 * driven through handling X events. In contrast to other X clients, a window
 * manager selects for SubstructureRedirectMask on the root window, to receive
 * events about window (dis-)appearance. Only one X connection at a time is
 * allowed to select for this event mask.
 *
 * The event handlers of dwm are organized in an array which is accessed
 * whenever a new event has been fetched. This allows event dispatching
 * in O(1) time.
 *
 * Each child of the root window is called a client, except windows which have
 * set the override_redirect flag. Clients are organized in a linked client
 * list on each monitor, the focus history is remembered through a stack list
 * on each monitor. Each client contains a bit array to indicate the tags of a
 * client.
 *
 * Keys and tagging rules are organized as arrays and defined in config.h.
 *
 * To understand everything else, start reading main().
 */
#include <errno.h>
#include <fcntl.h>
#include <locale.h>
#include <signal.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <X11/cursorfont.h>
#include <X11/keysym.h>
#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include <X11/Xlib-xcb.h>
#include <xcb/res.h>
#include <X11/Xproto.h>
#include <X11/Xutil.h>
#ifdef XINERAMA
#include <X11/extensions/Xinerama.h>
#endif /* XINERAMA */
#include <X11/Xft/Xft.h>

#include "drw.h"
#include "util.h"

/* macros */
#define BUTTONMASK                  (ButtonPressMask|ButtonReleaseMask)
#define CLEANMASK(mask)             (mask & ~(numlockmask|LockMask) & (ShiftMask|ControlMask|Mod1Mask|Mod2Mask|Mod3Mask|Mod4Mask|Mod5Mask))
#define INTERSECT(x,y,w,h,m)        (MAX(0, MIN((x)+(w),(m)->wx+(m)->ww) - MAX((x),(m)->wx)) \
                                    * MAX(0, MIN((y)+(h),(m)->wy+(m)->wh) - MAX((y),(m)->wy)))
#define ISVISIBLEONTAG(C, T)        ((C->tags & T))
#define ISVISIBLE(C)                ISVISIBLEONTAG(C, C->mon->tagset[C->mon->seltags])
#define LENGTH(X)                   (sizeof X / sizeof X[0])
#define MOUSEMASK                   (BUTTONMASK|PointerMotionMask)
#define MWM_HINTS_FLAGS_FIELD       0
#define MWM_HINTS_DECORATIONS_FIELD 2
#define MWM_HINTS_DECORATIONS       (1 << 1)
#define MWM_DECOR_ALL               (1 << 0)
#define MWM_DECOR_BORDER            (1 << 1)
#define MWM_DECOR_TITLE             (1 << 3)
#define NUMTAGS                     9
#define RULE(...)                   { .monitor = -1, __VA_ARGS__ },
#define WTYPE                       "_NET_WM_WINDOW_TYPE_"
#define WIDTH(X)                    ((X)->w + 2 * (X)->bw)
#define HEIGHT(X)                   ((X)->h + 2 * (X)->bw)
#define TAGMASK                     ((1 << LENGTH(tags)) - 1)
#define TAGSLENGTH                  (LENGTH(tags))
#define TEXTW(X)                    (drw_fontset_getwidth(drw, (X)) + lrpad)
#define TTEXTW(X)                   (drw_fontset_getwidth(drw, (X)))

#define STATUSLENGTH                256
#define DWMBLOCKSLOCKFILE           "/var/local/dwmblocks/dwmblocks.pid"
#define DELIMITERENDCHAR            10
#define LSPAD                       (lrpad / 2) /* padding on left side of status text */
#define RSPAD                       (lrpad / 2) /* padding on right side of status text */

/* enums */
enum { CurResizeBR, CurResizeBL, CurResizeTR, CurResizeTL,
       CurNormal, CurHand, CurResize, CurMove, CurLast }; /* cursor */
enum { SchemeNorm, SchemeSel, SchemeDarker,
       SchemeRed, SchemeGreen, SchemeBlue,
       SchemeCyan, SchemeMagenta, SchemeYellow,
       SchemeBlack, SchemeWhite,
       SchemeBrRed, SchemeBrGreen, SchemeBrBlue,
       SchemeBrCyan, SchemeBrMagenta, SchemeBrYellow,
       SchemeBrBlack, SchemeBrWhite, SchemeFloat, SchemeInactive, SchemeBar,
       SchemeTag, SchemeTag1, SchemeTag2, SchemeTag3,
       SchemeTag4, SchemeTag5, SchemeTag6, SchemeTag7,
       SchemeTag8, SchemeTag9, SchemeLayout,
       SchemeTitle, SchemeTitleFloat,
       SchemeTitle1, SchemeTitle2, SchemeTitle3,
       SchemeTitle4, SchemeTitle5, SchemeTitle6,
       SchemeTitle7, SchemeTitle8, SchemeTitle9 }; /* color schemes */
enum { NetSupported, NetWMName, NetWMState, NetWMCheck,
       NetWMFullscreen, NetActiveWindow, NetWMWindowType,
       NetWMWindowTypeDialog, NetClientList, NetDesktopNames,
       NetDesktopViewport, NetNumberOfDesktops, NetCurrentDesktop,
       NetClientListStacking, NetLast }; /* EWMH atoms */
enum { WMProtocols, WMDelete, WMState, WMTakeFocus, WMWindowRole, WMLast }; /* default atoms */
enum { ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle,
       ClkClientWin, ClkRootWin, ClkLast }; /* clicks */

typedef union {
	int i;
	unsigned int ui;
	float f;
	const void *v;
} Arg;

typedef struct {
	unsigned int click;
	unsigned int mask;
	unsigned int button;
	void (*func)(const Arg *arg);
	const Arg arg;
} Button;

typedef struct Monitor Monitor;
typedef struct Client Client;
struct Client {
	char name[256];
	float mina, maxa;
	float cfact;
	int x, y, w, h;
	int sfx, sfy, sfw, sfh; /* stored float geometry, used on mode revert */
	int oldx, oldy, oldw, oldh;
	int basew, baseh, incw, inch, maxw, maxh, minw, minh;
	int bw, oldbw;
	unsigned int tags;
	int isfixed, isfloating, isfloatpos, isurgent,  isfullscreen, isterminal;
        int neverfocus, oldstate, noswallow;
	int ignoresizehints;
	pid_t pid;
	Client *next;
	Client *snext;
	Client *swallowing;
	Monitor *mon;
	Window win;
};

typedef struct {
	unsigned int mod;
	KeySym keysym;
	void (*func)(const Arg *);
	const Arg arg;
} Key;

typedef struct {
	const char *symbol;
	void (*arrange)(Monitor *);
} Layout;

typedef struct Pertag Pertag;
struct Monitor {
	char ltsymbol[16];
	float mfact;
	int nmaster;
	int num;
	int by;               /* bar geometry */
	int mx, my, mw, mh;   /* screen size */
	int wx, wy, ww, wh;   /* window area  */
	int gappih;           /* horizontal gap between windows */
	int gappiv;           /* vertical gap between windows */
	int gappoh;           /* horizontal outer gaps */
	int gappov;           /* vertical outer gaps */
	int pertaggap;
	int edgegap;
	int bargap;
	int centertitle;
	int colorfultitle;
	int colorfultag;
	int showindicator;
	int showtitle;
        int showvacanttags;
	int showbar;
	int topbar;
        int statushandcursor;
	unsigned int seltags;
	unsigned int sellt;
	unsigned int tagset[2];
	unsigned int alttag;
	Client *clients;
	Client *sel;
	Client *stack;
	Monitor *next;
        Pertag *pertag;
	Window barwin;
	const Layout *lt[2];
};

struct Pertag {
	unsigned int curtag, prevtag; /* current and previous tag */
	int nmasters[NUMTAGS + 1]; /* number of windows in master area */
	float mfacts[NUMTAGS + 1]; /* mfacts per tag */
	int showbars[NUMTAGS + 1]; /* display bar for the current tag */
	Client *prevzooms[NUMTAGS + 1]; /* store zoom information */
	int enablegaps[NUMTAGS + 1]; /* toggle gap for the current tag */
        unsigned int gaps[NUMTAGS + 1]; /* gap value per tag */
	unsigned int sellts[NUMTAGS + 1]; /* selected layouts */
	const Layout *ltidxs[NUMTAGS + 1][2]; /* matrix of tags and layouts indexes  */
};

typedef struct {
	const char *class;
	const char *role;
	const char *instance;
	const char *title;
	const char *wintype;
	unsigned int tags;
	int isfloating;
        int isterminal;
        int noswallow;
        int matchonce;
	const char *floatpos;
	int monitor;
} Rule;

/* function declarations */
static void applyrules(Client *c);
static int applysizehints(Client *c, int *x, int *y,
                                int *w, int *h, int interact);
static void arrange(Monitor *m);
static void arrangemon(Monitor *m);
static void attach(Client *c);
static void attachbelow(Client *c);
static void attachstack(Client *c);
static void bstack(Monitor *m);
static void buttonpress(XEvent *e);
static void centeredfloatingmaster(Monitor *m);
static void centeredmaster(Monitor *m);
static void checkotherwm(void);
static void cleanup(void);
static void cleanupmon(Monitor *mon);
static void clientmessage(XEvent *e);
static void configure(Client *c);
static void configurenotify(XEvent *e);
static void configurerequest(XEvent *e);
static Monitor *createmon(void);
static void cyclelayout(const Arg *arg);
static void defaultgaps(const Arg *arg);
static void destroynotify(XEvent *e);
static void detach(Client *c);
static void detachstack(Client *c);
static Monitor *dirtomon(int dir);
static void drawbar(Monitor *m);
static void drawbars(void);
static void dwindle(Monitor *m);
static void enternotify(XEvent *e);
static void expose(XEvent *e);
static Client *findbefore(Client *c);
static void floatpos(const Arg *arg);
static void focus(Client *c);
static void focusin(XEvent *e);
static void focusmaster(const Arg *arg);
static void focusmon(const Arg *arg);
static void focusstack(const Arg *arg);
static void gaplessgrid(Monitor *m);
static Atom getatomprop(Client *c, Atom prop);
static void getfacts(Monitor *m, int msize, int ssize,
                        float *mf, float *sf, int *mr, int *sr);
static void getfloatpos(int pos, char pCh, int size, char sCh,
                        int min_p, int max_s, int cp, int cs, int cbw,
                        int defgrid, int *out_p, int *out_s);
static void getgaps(Monitor *m, int *oh, int *ov,
                        int *ih, int *iv, unsigned int *nc);
static pid_t getparentprocess(pid_t p);
static int getrootptr(int *x, int *y);
static long getstate(Window w);
static int gettextprop(Window w, Atom atom, char *text, unsigned int size);
static void grabbuttons(Client *c, int focused);
static void grabkeys(void);
static void incnmaster(const Arg *arg);
static void incrgaps(const Arg *arg);
static void incrigaps(const Arg *arg);
static void incrihgaps(const Arg *arg);
static void incrivgaps(const Arg *arg);
static void incrogaps(const Arg *arg);
static void incrohgaps(const Arg *arg);
static void incrovgaps(const Arg *arg);
static void inplacerotate(const Arg *arg);
static void insertclient(Client *item, Client *insertItem, int after);
static int isdescprocess(pid_t p, pid_t c);
static void keypress(XEvent *e);
static void killclient(const Arg *arg);
static void manage(Window w, XWindowAttributes *wa);
static void mappingnotify(XEvent *e);
static void maprequest(XEvent *e);
static void monocle(Monitor *m);
static void motionnotify(XEvent *e);
static void movemouse(const Arg *arg);
static Client *nexttagged(Client *c);
static Client *nexttiled(Client *c);
static void pop(Client *);
static void propertynotify(XEvent *e);
static void quit(const Arg *arg);
static Monitor *recttomon(int x, int y, int w, int h);
static void resetfact(const Arg *arg);
static void resize(Client *c, int x, int y, int w, int h, int interact);
static void resizeclient(Client *c, int x, int y, int w, int h);
static void resizemouse(const Arg *arg);
static void restack(Monitor *m);
static void run(void);
static void scan(void);
static int sendevent(Client *c, Atom proto);
static void sendmon(Client *c, Monitor *m);
static void setcfact(const Arg *arg);
static void setfloatpos(Client *c, const char *floatpos);
static void setgaps(int oh, int ov, int ih, int iv);
static void setclientstate(Client *c, long state);
static void setcurrentdesktop(void);
static void setdesktopnames(void);
static void setfocus(Client *c);
static void setfullscreen(Client *c, int fullscreen);
static void setlayout(const Arg *arg);
static void setmfact(const Arg *arg);
static void setnumdesktops(void);
static void setup(void);
static void setviewport(void);
static void seturgent(Client *c, int urg);
static void shiftclient(const Arg *arg);
static void shiftview(const Arg *arg);
static void showhide(Client *c);
static void sigchld(int unused);
static void sigdwmblocks(const Arg *arg);
static void spawn(const Arg *arg);
static void swallow(Client *p, Client *c);
static Client *swallowingclient(Window w);
static void tag(const Arg *arg);
static void tagmon(const Arg *arg);
static void tagview(const Arg *arg);
static Client *termforwin(const Client *c);
static void tile(Monitor *);
static void togglealttag(const Arg *arg);
static void togglebar(const Arg *arg);
static void togglebargap(const Arg *arg);
static void togglefloating(const Arg *arg);
static void togglegaps(const Arg *arg);
static void toggleindicator(const Arg *arg);
static void toggletag(const Arg *arg);
static void toggletagcolor(const Arg *arg);
static void toggletaggaps(const Arg *arg);
static void toggletitle(const Arg *arg);
static void toggletitlecolor(const Arg *arg);
static void toggletitlepos(const Arg *arg);
static void togglevacanttag(const Arg *arg);
static void toggleview(const Arg *arg);
static void unfocus(Client *c, int setfocus);
static void unmanage(Client *c, int destroyed);
static void unmapnotify(XEvent *e);
static void unswallow(Client *c);
static void updatebarpos(Monitor *m);
static void updatebars(void);
static void updateclientlist(void);
static void updatecurrentdesktop(void);
static void updatedwmblockssig(int x);
static int updategeom(void);
static void updatemotifhints(Client *c);
static void updatenumlockmask(void);
static void updatesizehints(Client *c);
static void updatestatus(void);
static void updatetitle(Client *c);
static void updatewindowtype(Client *c);
static void updatewmhints(Client *c);
static void view(const Arg *arg);
static pid_t winpid(Window w);
static Client *wintoclient(Window w);
static Monitor *wintomon(Window w);
static int xerror(Display *dpy, XErrorEvent *ee);
static int xerrordummy(Display *dpy, XErrorEvent *ee);
static int xerrorstart(Display *dpy, XErrorEvent *ee);
static void zoom(const Arg *arg);

/* variables */
static const char broken[] = "broken";
static char stextc[STATUSLENGTH];
static char stexts[STATUSLENGTH];
static int screen;
static int sw, sh;           /* X display screen geometry width, height */
static int bh, blw, ble;     /* bar geometry */
static int wstext;           /* width of status text */
static int lrpad;            /* sum of left and right padding for text */
static int vp;               /* vertical padding for bar */
static int sp;               /* side padding for bar */
static int (*xerrorxlib)(Display *, XErrorEvent *);
static unsigned int dwmblockssig;
static unsigned int numlockmask = 0;
static void (*handler[LASTEvent]) (XEvent *) = {
	[ButtonPress] = buttonpress,
	[ClientMessage] = clientmessage,
	[ConfigureRequest] = configurerequest,
	[ConfigureNotify] = configurenotify,
	[DestroyNotify] = destroynotify,
	[EnterNotify] = enternotify,
	[Expose] = expose,
	[FocusIn] = focusin,
	[KeyPress] = keypress,
	[MappingNotify] = mappingnotify,
	[MapRequest] = maprequest,
	[MotionNotify] = motionnotify,
	[PropertyNotify] = propertynotify,
	[UnmapNotify] = unmapnotify
};
static Atom wmatom[WMLast], netatom[NetLast], motifatom;
static int running = 1;
static Cur *cursor[CurLast];
static Clr **scheme;
static Display *dpy;
static Drw *drw;
static Monitor *mons, *selmon;
static Window root, wmcheckwin;
static xcb_connection_t *xcon;

/* configuration, allows nested code to access above variables */
#include "config.h"

/* compile-time check if all tags fit into an unsigned int bit array. */
struct NumTags { char limitexceeded[LENGTH(tags) > 31 ? -1 : 1]; };

/* function implementations */
void
applyrules(Client *c)
{
	const char *class, *instance;
	Atom wintype;
	char role[64];
	unsigned int i;
	const Rule *r;
	Monitor *m;
	XClassHint ch = { NULL, NULL };

	/* rule matching */
	c->isfloating = 0;
        c->isfloatpos = 0;
        c->noswallow = 0;
	c->tags = 0;
	XGetClassHint(dpy, c->win, &ch);
	class    = ch.res_class ? ch.res_class : broken;
	instance = ch.res_name  ? ch.res_name  : broken;
	wintype  = getatomprop(c, netatom[NetWMWindowType]);
	gettextprop(c->win, wmatom[WMWindowRole], role, sizeof(role));

	for (i = 0; i < LENGTH(rules); i++) {
		r = &rules[i];
		if ((!r->title || strstr(c->name, r->title))
		&& (!r->class || strstr(class, r->class))
		&& (!r->role || strstr(role, r->role))
		&& (!r->instance || strstr(instance, r->instance))
		&& (!r->wintype || wintype == XInternAtom(dpy, r->wintype, 0)))
		{
			c->isterminal = r->isterminal;
			c->noswallow  = r->noswallow;
			c->isfloating = r->isfloating;
			c->tags |= r->tags;
			for (m = mons; m && m->num != r->monitor; m = m->next);
			if (m)
				c->mon = m;
			if (r->floatpos) {
                                c->isfloating = 1;
                                setfloatpos(c, r->floatpos);
                                c->isfloatpos = 1;
			}
                        if (r->matchonce)
                                break;
		}
	}
	if (ch.res_class)
		XFree(ch.res_class);
	if (ch.res_name)
		XFree(ch.res_name);
	c->tags = c->tags & TAGMASK
                        ? c->tags & TAGMASK
                        : c->mon->tagset[c->mon->seltags];
}

int
applysizehints(Client *c, int *x, int *y, int *w, int *h, int interact)
{
	int baseismin;
	Monitor *m = c->mon;

	/* set minimum possible */
	*w = MAX(1, *w);
	*h = MAX(1, *h);
	if (interact) {
		if (*x > sw)
			*x = sw - WIDTH(c);
		if (*y > sh)
			*y = sh - HEIGHT(c);
		if (*x + *w + 2 * c->bw < 0)
			*x = 0;
		if (*y + *h + 2 * c->bw < 0)
			*y = 0;
	} else {
		if (*x >= m->wx + m->ww)
			*x = m->wx + m->ww - WIDTH(c);
		if (*y >= m->wy + m->wh)
			*y = m->wy + m->wh - HEIGHT(c);
		if (*x + *w + 2 * c->bw <= m->wx)
			*x = m->wx;
		if (*y + *h + 2 * c->bw <= m->wy)
			*y = m->wy;
	}
	if (*h < bh)
		*h = bh;
	if (*w < bh)
		*w = bh;
	if (!c->ignoresizehints
        && (resizehints || c->isfloating || !c->mon->lt[c->mon->sellt]->arrange))
        { /* see last two sentences in ICCCM 4.1.2.3 */
		baseismin = c->basew == c->minw && c->baseh == c->minh;
		if (!baseismin) { /* temporarily remove base dimensions */
			*w -= c->basew;
			*h -= c->baseh;
		}
		/* adjust for aspect limits */
		if (c->mina > 0 && c->maxa > 0) {
			if (c->maxa < (float)*w / *h)
				*w = *h * c->maxa + 0.5;
			else if (c->mina < (float)*h / *w)
				*h = *w * c->mina + 0.5;
		}
		if (baseismin) { /* increment calculation requires this */
			*w -= c->basew;
			*h -= c->baseh;
		}
		/* adjust for increment value */
		if (c->incw)
			*w -= *w % c->incw;
		if (c->inch)
			*h -= *h % c->inch;
		/* restore base dimensions */
		*w = MAX(*w + c->basew, c->minw);
		*h = MAX(*h + c->baseh, c->minh);
		if (c->maxw)
			*w = MIN(*w, c->maxw);
		if (c->maxh)
			*h = MIN(*h, c->maxh);
	}
	return *x != c->x || *y != c->y || *w != c->w || *h != c->h;
}

void
arrange(Monitor *m)
{
	if (m)
		showhide(m->stack);
	else for (m = mons; m; m = m->next)
		showhide(m->stack);
	if (m) {
		arrangemon(m);
		restack(m);
	} else for (m = mons; m; m = m->next)
		arrangemon(m);
}

void
arrangemon(Monitor *m)
{
	strncpy(m->ltsymbol, m->lt[m->sellt]->symbol, sizeof m->ltsymbol);
	if (m->lt[m->sellt]->arrange)
		m->lt[m->sellt]->arrange(m);
}

void
attach(Client *c)
{
	c->next = c->mon->clients;
	c->mon->clients = c;
}

void
attachbelow(Client *c)
{
	/* If there is nothing on the monitor or */
        /* the selected client is floating, attach as normal */
	if (c->mon->sel == NULL || c->mon->sel->isfloating) {
                Client *at = nexttagged(c);
                if (!at) {
                        attach(c);
                        return;
                }
                c->next = at->next;
                at->next = c;
                return;
	}

	/* Set the new client's next property to */
        /* the same as the currently selected clients next */
	c->next = c->mon->sel->next;
	/* Set the currently selected clients next property to the new client */
	c->mon->sel->next = c;
}

void
attachstack(Client *c)
{
	c->snext = c->mon->stack;
	c->mon->stack = c;
}

/*
 * Bottomstack layout + gaps
 * https://dwm.suckless.org/patches/bottomstack/
 */
static void
bstack(Monitor *m)
{
	unsigned int i, n;
	int oh, ov, ih, iv;
	int mx = 0, my = 0, mh = 0, mw = 0;
	int sx = 0, sy = 0, sh = 0, sw = 0;
	float mfacts, sfacts;
	int mrest, srest;
	Client *c;

	getgaps(m, &oh, &ov, &ih, &iv, &n);
	if (n == 0)
		return;

	sx = mx = m->wx + ov;
	sy = my = m->wy + oh;
	sh = mh = m->wh - 2*oh;
	mw = m->ww - 2*ov - iv * (MIN(n, m->nmaster) - 1);
	sw = m->ww - 2*ov - iv * (n - m->nmaster - 1);

	if (m->nmaster && n > m->nmaster) {
		sh = (mh - ih) * (1 - m->mfact);
		mh = mh - ih - sh;
		sx = mx;
		sy = my + mh + ih;
	}

	getfacts(m, mw, sw, &mfacts, &sfacts, &mrest, &srest);

	for (i = 0, c = nexttiled(m->clients); c; c = nexttiled(c->next), i++) {
		if (i < m->nmaster) {
			resize( c, mx, my,
                                mw * (c->cfact / mfacts)
                                + (i < mrest ? 1 : 0) - (2*c->bw),
                                mh - (2*c->bw), 0);
			mx += WIDTH(c) + iv;
		} else {
			resize( c, sx, sy,
                                sw * (c->cfact / sfacts)
                                + ((i - m->nmaster) < srest ? 1 : 0)
                                - (2*c->bw), sh - (2*c->bw), 0);
			sx += WIDTH(c) + iv;
		}
	}
}

void
buttonpress(XEvent *e)
{
        int i, x;
        unsigned int click = 0, occ = 0;
	Arg arg = {0};
	Client *c;
	Monitor *m;
	XButtonPressedEvent *ev = &e->xbutton;

	/* focus monitor if necessary */
	if ((m = wintomon(ev->window)) && m != selmon) {
		unfocus(selmon->sel, 1);
		selmon = m;
		focus(NULL);
	}
	if (ev->window == selmon->barwin) {
                if (ev->x < ble - blw) {
                        for (c = m->clients;
                        !selmon->showvacanttags && c;
                        c = c->next)
                                occ |= c->tags == 255 ? 0 : c->tags;
                        i = x = 0;
                        do {
                                /* do not reserve space for vacant tags */
                                if (!selmon->showvacanttags) {
                                        if (!(occ & 1 << i
                                        || m->tagset[m->seltags] & 1 << i))
                                                continue;
                                }
                                x += TEXTW(tags[i]);
                        } while (ev->x >= x && ++i < LENGTH(tags));
                        if (i < LENGTH(tags)) {
                                click = ClkTagBar;
                                arg.ui = 1 << i;
                        }
                } else if (ev->x < ble) {
                        click = ClkLtSymbol;
                } else if (ev->x < selmon->ww - wstext) {
                        click = ClkWinTitle;
                } else if ((x = selmon->ww - (2 * sp) - RSPAD - ev->x) > 0
                        && (x -= wstext - LSPAD - RSPAD) <= 0) {
                        updatedwmblockssig(x);
                        click = ClkStatusText;
                } else {
                        return;
                }
	} else if ((c = wintoclient(ev->window))) {
		focus(c);
		restack(selmon);
		XAllowEvents(dpy, ReplayPointer, CurrentTime);
		click = ClkClientWin;
	} else {
                click = ClkRootWin;
        }
	for (i = 0; i < LENGTH(buttons); i++)
		if (click == buttons[i].click && buttons[i].func
                && buttons[i].button == ev->button
                && CLEANMASK(buttons[i].mask) == CLEANMASK(ev->state))
			buttons[i].func(click == ClkTagBar
                                        && buttons[i].arg.i == 0
                                        ? &arg : &buttons[i].arg);
}

/*
 * Centred master layout + gaps
 * https://dwm.suckless.org/patches/centeredmaster/
 */

void
centeredfloatingmaster(Monitor *m)
{
	unsigned int i, n;
	float mfacts, sfacts;
	float mivf = 1.0; // master inner vertical gap factor
	int oh, ov, ih, iv, mrest, srest;
	int mx = 0, my = 0, mh = 0, mw = 0;
	int sx = 0, sy = 0, sh = 0, sw = 0;
	Client *c;

	getgaps(m, &oh, &ov, &ih, &iv, &n);
	if (n == 0)
		return;

	sx = mx = m->wx + ov;
	sy = my = m->wy + oh;
	sh = mh = m->wh - 2*oh;
	mw = m->ww - 2*ov - iv*(n - 1);
	sw = m->ww - 2*ov - iv*(n - m->nmaster - 1);

	if (m->nmaster && n > m->nmaster) {
		mivf = 0.8;
		/* go mfact box in the center if more than nmaster clients */
		if (m->ww > m->wh) {
			mw = m->ww * m->mfact
                                - iv*mivf*(MIN(n, m->nmaster) - 1);
			mh = m->wh * 0.9;
		} else {
			mw = m->ww * 0.9 - iv*mivf*(MIN(n, m->nmaster) - 1);
			mh = m->wh * m->mfact;
		}
		mx = m->wx + (m->ww - mw) / 2;
		my = m->wy + (m->wh - mh - 2*oh) / 2;

		sx = m->wx + ov;
		sy = m->wy + oh;
		sh = m->wh - 2*oh;
	}

	getfacts(m, mw, sw, &mfacts, &sfacts, &mrest, &srest);

	for (i = 0, c = nexttiled(m->clients); c; c = nexttiled(c->next), i++)
		if (i < m->nmaster) {
			/* nmaster clients are stacked horizontally, in the center of the screen */
			resize( c, mx, my,
                                mw * (c->cfact / mfacts)
                                + (i < mrest ? 1 : 0)
                                - (2*c->bw), mh - (2*c->bw), 0);
			mx += WIDTH(c) + iv*mivf;
		} else {
			/* stack clients are stacked horizontally */
			resize( c, sx, sy,
                                sw * (c->cfact / sfacts)
                                + ((i - m->nmaster) < srest ? 1 : 0)
                                - (2*c->bw), sh - (2*c->bw), 0);
			sx += WIDTH(c) + iv;
		}
}

void
centeredmaster(Monitor *m)
{
	unsigned int i, n;
	int oh, ov, ih, iv;
	int mx = 0, my = 0, mh = 0, mw = 0;
	int lx = 0, ly = 0, lw = 0, lh = 0;
	int rx = 0, ry = 0, rw = 0, rh = 0;
	float mfacts = 0, lfacts = 0, rfacts = 0;
	int mtotal = 0, ltotal = 0, rtotal = 0;
	int mrest = 0, lrest = 0, rrest = 0;
	Client *c;

	getgaps(m, &oh, &ov, &ih, &iv, &n);
	if (n == 0)
		return;

	/* initialize areas */
	mx = m->wx + ov;
	my = m->wy + oh;
	mh = m->wh - 2*oh - ih * ((!m->nmaster ? n : MIN(n, m->nmaster)) - 1);
	mw = m->ww - 2*ov;
	lh = m->wh - 2*oh - ih * (((n - m->nmaster) / 2) - 1);
	rh = m->wh - 2*oh - ih
                * (((n - m->nmaster) / 2) - ((n - m->nmaster) % 2 ? 0 : 1));

	if (m->nmaster && n > m->nmaster) {
		/* go mfact box in the center if more than nmaster clients */
		if (n - m->nmaster > 1) {
			/* ||<-S->|<---M--->|<-S->|| */
			mw = (m->ww - 2*ov - 2*iv) * m->mfact;
			lw = (m->ww - mw - 2*ov - 2*iv) / 2;
			rw = (m->ww - mw - 2*ov - 2*iv) - lw;
			mx += lw + iv;
		} else {
			/* ||<---M--->|<-S->|| */
			mw = (mw - iv) * m->mfact;
			lw = 0;
			rw = m->ww - mw - iv - 2*ov;
		}
		lx = m->wx + ov;
		ly = m->wy + oh;
		rx = mx + mw + iv;
		ry = m->wy + oh;
	}

	/* calculate facts */
	for (n = 0, c = nexttiled(m->clients); c; c = nexttiled(c->next), n++) {
		if (!m->nmaster || n < m->nmaster)
			mfacts += c->cfact;
		else if ((n - m->nmaster) % 2)
			lfacts += c->cfact; // total factor of left hand stack area
		else
			rfacts += c->cfact; // total factor of right hand stack area
	}

	for (n = 0, c = nexttiled(m->clients); c; c = nexttiled(c->next), n++)
		if (!m->nmaster || n < m->nmaster)
			mtotal += mh * (c->cfact / mfacts);
		else if ((n - m->nmaster) % 2)
			ltotal += lh * (c->cfact / lfacts);
		else
			rtotal += rh * (c->cfact / rfacts);

	mrest = mh - mtotal;
	lrest = lh - ltotal;
	rrest = rh - rtotal;

	for (i = 0, c = nexttiled(m->clients); c; c = nexttiled(c->next), i++) {
		if (!m->nmaster || i < m->nmaster) {
			/* nmaster clients are stacked vertically, in the center of the screen */
			resize( c, mx, my, mw - (2*c->bw),
                                mh * (c->cfact / mfacts)
                                + (i < mrest ? 1 : 0)
                                - (2*c->bw), 0);
			my += HEIGHT(c) + ih;
		} else {
			/* stack clients are stacked vertically */
			if ((i - m->nmaster) % 2 ) {
				resize( c, lx, ly, lw - (2*c->bw),
                                        lh * (c->cfact / lfacts)
                                        + ((i - 2*m->nmaster) < 2*lrest ? 1 : 0)
                                        - (2*c->bw), 0);
				ly += HEIGHT(c) + ih;
			} else {
				resize( c, rx, ry, rw - (2*c->bw),
                                        rh * (c->cfact / rfacts)
                                        + ((i - 2*m->nmaster) < 2*rrest ? 1 : 0)
                                        - (2*c->bw), 0);
				ry += HEIGHT(c) + ih;
			}
		}
	}
}

void
checkotherwm(void)
{
	xerrorxlib = XSetErrorHandler(xerrorstart);
	/* this causes an error if some other window manager is running */
	XSelectInput(dpy, DefaultRootWindow(dpy), SubstructureRedirectMask);
	XSync(dpy, False);
	XSetErrorHandler(xerror);
	XSync(dpy, False);
}

void
cleanup(void)
{
	Arg a = {.ui = ~0};
	Layout foo = { "", NULL };
	Monitor *m;
	size_t i;

	view(&a);
	selmon->lt[selmon->sellt] = &foo;
	for (m = mons; m; m = m->next)
		while (m->stack)
			unmanage(m->stack, 0);
	XUngrabKey(dpy, AnyKey, AnyModifier, root);
	while (mons)
		cleanupmon(mons);
	for (i = 0; i < CurLast; i++)
		drw_cur_free(drw, cursor[i]);
	for (i = 0; i < LENGTH(colors); i++)
		free(scheme[i]);
        free(scheme);
	XDestroyWindow(dpy, wmcheckwin);
	drw_free(drw);
	XSync(dpy, 0);
	XSetInputFocus(dpy, PointerRoot, RevertToPointerRoot, CurrentTime);
	XDeleteProperty(dpy, root, netatom[NetActiveWindow]);
}

void
cleanupmon(Monitor *mon)
{
	Monitor *m;

	if (mon == mons)
		mons = mons->next;
	else {
		for (m = mons; m && m->next != mon; m = m->next);
		m->next = mon->next;
	}
	XUnmapWindow(dpy, mon->barwin);
	XDestroyWindow(dpy, mon->barwin);
        free(mon->pertag);
	free(mon);
}

void
clientmessage(XEvent *e)
{
	XClientMessageEvent *cme = &e->xclient;
	Client *c = wintoclient(cme->window);
	unsigned int i;

	if (!c)
		return;
	if (cme->message_type == netatom[NetWMState]) {
		if (cme->data.l[1] == netatom[NetWMFullscreen]
		|| cme->data.l[2] == netatom[NetWMFullscreen])
			setfullscreen(c, (cme->data.l[0] == 1 /* _NET_WM_STATE_ADD    */
				        || (cme->data.l[0] == 2 /* _NET_WM_STATE_TOGGLE */
                                                && !c->isfullscreen)));
	} else if (cme->message_type == netatom[NetActiveWindow]) {
		for (i = 0; i < LENGTH(tags) && !((1 << i) & c->tags); i++);
		if (i < LENGTH(tags)) {
			const Arg a = {.ui = 1 << i};
                        unfocus(selmon->sel, 0);
			selmon = c->mon;
			view(&a);
			focus(c);
			restack(selmon);
		}
	}
}

void
configure(Client *c)
{
	XConfigureEvent ce;

	ce.type = ConfigureNotify;
	ce.display = dpy;
	ce.event = c->win;
	ce.window = c->win;
	ce.x = c->x;
	ce.y = c->y;
	ce.width = c->w;
	ce.height = c->h;
	ce.border_width = c->bw;
	ce.above = None;
	ce.override_redirect = 0;
	XSendEvent(dpy, c->win, 0, StructureNotifyMask, (XEvent *)&ce);
}

void
configurenotify(XEvent *e)
{
	Monitor *m;
	Client *c;
	XConfigureEvent *ev = &e->xconfigure;
	int dirty;

	/* TODO: updategeom handling sucks, needs to be simplified */
	if (ev->window == root) {
		dirty = (sw != ev->width || sh != ev->height);
		sw = ev->width;
		sh = ev->height;
		if (updategeom() || dirty) {
			drw_resize(drw, sw, bh);
			updatebars();
			for (m = mons; m; m = m->next) {
				for (c = m->clients; c; c = c->next)
					if (c->isfullscreen)
						resizeclient(c, m->mx, m->my,
                                                        m->mw, m->mh);
				XMoveResizeWindow(dpy, m->barwin,
                                        m->wx + sp, m->by + vp,
                                        m->ww -  2 * sp, bh);
			}
			focus(NULL);
			arrange(NULL);
		}
	}
}

void
configurerequest(XEvent *e)
{
	Client *c;
	Monitor *m;
	XConfigureRequestEvent *ev = &e->xconfigurerequest;
	XWindowChanges wc;

	if ((c = wintoclient(ev->window))) {
		if (ev->value_mask & CWBorderWidth)
			c->bw = ev->border_width;
		else if (c->isfloating || !selmon->lt[selmon->sellt]->arrange) {
			m = c->mon;
			if (ev->value_mask & CWX) {
				c->oldx = c->x;
				c->x = m->mx + ev->x;
			}
			if (ev->value_mask & CWY) {
				c->oldy = c->y;
				c->y = m->my + ev->y;
			}
			if (ev->value_mask & CWWidth) {
				c->oldw = c->w;
				c->w = ev->width;
			}
			if (ev->value_mask & CWHeight) {
				c->oldh = c->h;
				c->h = ev->height;
			}
			if ((c->x + c->w) > m->mx + m->mw && c->isfloating)
				c->x = m->mx + (m->mw / 2 - WIDTH(c) / 2); /* center in x direction */
			if ((c->y + c->h) > m->my + m->mh && c->isfloating)
				c->y = m->my + (m->mh / 2 - HEIGHT(c) / 2); /* center in y direction */
			if ((ev->value_mask & (CWX|CWY))
                        && !(ev->value_mask & (CWWidth|CWHeight)))
				configure(c);
			if (ISVISIBLE(c))
				XMoveResizeWindow(dpy, c->win, c->x,
                                        c->y, c->w, c->h);
		} else
			configure(c);
	} else {
		wc.x = ev->x;
		wc.y = ev->y;
		wc.width = ev->width;
		wc.height = ev->height;
		wc.border_width = ev->border_width;
		wc.sibling = ev->above;
		wc.stack_mode = ev->detail;
		XConfigureWindow(dpy, ev->window, ev->value_mask, &wc);
	}
	XSync(dpy, 0);
}

Monitor *
createmon(void)
{
	Monitor *m;
	unsigned int i, taglayout;

	m = ecalloc(1, sizeof(Monitor));
	m->tagset[0] = m->tagset[1] = 1;
	m->mfact = mfact;
	m->nmaster = nmaster;
	m->gappih = gappih;
	m->gappiv = gappiv;
	m->gappoh = gappoh;
	m->gappov = gappov;
        m->pertaggap = pertaggap ? pertaggap : 0;
        m->edgegap = edgegap ? edgegap : 0;
        m->bargap = bargap ? bargap : 0;
        m->centertitle = centertitle ? centertitle : 0;
        m->colorfultag = colorfultag ? colorfultag : 0;
        m->colorfultitle = colorfultitle ? colorfultitle : 0;
	m->showbar = showbar;
        m->showindicator = showindicator ? showindicator : 0;
        m->showtitle = showtitle ? showtitle : 0;
        m->showvacanttags = showvacanttags ? showvacanttags : 0;
	m->topbar = topbar;
	m->lt[0] = &layouts[taglayouts[1] % LENGTH(layouts)];
	m->lt[1] = &layouts[1 % LENGTH(layouts)];
	m->pertag = ecalloc(1, sizeof(Pertag));
	m->pertag->curtag = m->pertag->prevtag = 1;

	for (i = 0; i <= LENGTH(tags); i++) {
		m->pertag->nmasters[i] = m->nmaster;
		m->pertag->mfacts[i] = m->mfact;

                taglayout = taglayouts[i % LENGTH(taglayouts)];
                taglayout %= LENGTH(layouts);
        	m->pertag->ltidxs[i][0] = &layouts[taglayout];
		m->pertag->ltidxs[i][1] = m->lt[1];
		m->pertag->sellts[i] = m->sellt;

		m->pertag->showbars[i] = m->showbar;
		m->pertag->prevzooms[i] = NULL;
		m->pertag->enablegaps[i] = 1;
                m->pertag->gaps[i] = ((gappoh & 0xFF) << 0)
                                        | ((gappov & 0xFF) << 8)
                                        | ((gappih & 0xFF) << 16)
                                        | ((gappiv & 0xFF) << 24);
	}

	strncpy(m->ltsymbol, layouts[0].symbol, sizeof m->ltsymbol);
	return m;
}

void
cyclelayout(const Arg *arg)
{
	Layout *l;
	for(l = (Layout *)layouts; l != selmon->lt[selmon->sellt]; l++);
	if(arg->i > 0) {
		if(l->symbol && (l + 1)->symbol)
			setlayout(&((Arg) { .v = (l + 1) }));
		else
			setlayout(&((Arg) { .v = layouts }));
	} else {
		if(l != layouts && (l - 1)->symbol)
			setlayout(&((Arg) { .v = (l - 1) }));
		else
			setlayout(&((Arg) {
                                        .v = &layouts[LENGTH(layouts) - 2]
                        }));
	}
}

void
defaultgaps(const Arg *arg)
{
	setgaps(gappoh, gappov, gappih, gappiv);
}

void
destroynotify(XEvent *e)
{
	Client *c;
	XDestroyWindowEvent *ev = &e->xdestroywindow;

	if ((c = wintoclient(ev->window)))
		unmanage(c, 1);
	else if ((c = swallowingclient(ev->window)))
		unmanage(c->swallowing, 1);
}

void
detach(Client *c)
{
	Client **tc;

	for (tc = &c->mon->clients; *tc && *tc != c; tc = &(*tc)->next);
	*tc = c->next;
}

void
detachstack(Client *c)
{
	Client **tc, *t;

	for (tc = &c->mon->stack; *tc && *tc != c; tc = &(*tc)->snext);
	*tc = c->snext;

	if (c == c->mon->sel) {
		for (t = c->mon->stack; t && !ISVISIBLE(t); t = t->snext);
		c->mon->sel = t;
	}
}

Monitor *
dirtomon(int dir)
{
	Monitor *m = NULL;

	if (dir > 0) {
		if (!(m = selmon->next))
			m = mons;
	} else if (selmon == mons)
		for (m = mons; m->next; m = m->next);
	else
		for (m = mons; m->next != selmon; m = m->next);
	return m;
}

void
drawbar(Monitor *m)
{
	int x, w, wdelta, tagscheme;
        int y = m->bargap || gaplessborder ? barborder : 0;
        int h = bh - y * 2;
	int boxs = drw->fonts->h / 9;
	int boxw = drw->fonts->h / 6 + 2;
        unsigned int titlepad = lrpad / 2;
	unsigned int i, occ = 0, urg = 0;
	Client *c;

        /* draw rectangle that will look like border */
        XSetForeground(drw->dpy, drw->gc, scheme[SchemeBar][ColBorder].pixel);
        XFillRectangle(drw->dpy, drw->drawable, drw->gc, 0, 0, m->ww, bh);

	/* draw status first so it can be overdrawn by tags later */
	if (m == selmon) { /* status is only drawn on selected monitor */
                char *stc = stextc;
                char *stp = stextc;
                char tmp;

                drw_setscheme(drw, scheme[SchemeBar]);
                x = m->ww - wstext - 2 * sp;
                drw_rect(drw, x, y, LSPAD, h, 1, 1); x += LSPAD; /* to keep left padding clean */
                for (;;) {
                        if ((unsigned char)*stc >= ' ') {
                                stc++;
                                continue;
                        }
                        tmp = *stc;
                        if (stp != stc) {
                                *stc = '\0';
                                x = drw_text(drw, x, y,
                                        TTEXTW(stp), h, 0, stp, 0);
                        }
                        if (tmp == '\0')
                                break;
                        if (tmp - DELIMITERENDCHAR - 1 < LENGTH(colors))
                                drw_setscheme(drw,
                                        scheme[tmp - DELIMITERENDCHAR - 1]);
                        *stc = tmp;
                        stp = ++stc;
                }
                drw_setscheme(drw, scheme[SchemeBar]);
                drw_rect(drw, x, y, LSPAD - y, h, 1, 1); /* to keep right padding clean */
	}

        for (c = m->clients; c; c = c->next) {
                occ |= c->tags == 255 ? 0 : c->tags;
                if (c->isurgent)
                        urg |= c->tags;
        }
        x = y;
        for (i = 0; i < LENGTH(tags); i++) {
                tagscheme = SchemeTag;
                /* do not draw vacant tags on current monitor */
                if (!m->showvacanttags
                && !(occ & 1 << i || m->tagset[m->seltags] & 1 << i))
                        continue;
                w = TEXTW(tags[i]);
                wdelta = m->alttag ? (TEXTW(tagsalt[i]) - w) / 2 : 0;
                if (m == selmon && m->tagset[m->seltags] & 1 << i) {
                        tagscheme = SchemeBar;
                        if (m->sel && m->sel->tags & 1 << i)
                                tagscheme = m->colorfultag
                                                ? tagschemes[i] : SchemeSel;
                        else if (m->sel && m->colorfultag && m->showindicator)
                                tagscheme = tagschemes[i];
                        else if (m->sel && !m->showvacanttags)
                                tagscheme = SchemeTag;
                }
                drw_setscheme(drw, scheme[tagscheme]);
                drw_text(drw, x, y, w + y, h, wdelta + lrpad / 2,
                        m->alttag ? tagsalt[i] : tags[i], urg & 1 << i);
                /* draw bar indicator for selected tag */
                /* check if the tag not a vacant tag*/
                if (occ & 1 << i) {
                        /* draw for current monitor and selected tag */
                        if (m == selmon && m->tagset[m->seltags] & 1 << i) {
                                if (m->sel->tags & 1 << i)
                                /* draw full line at the top of selected tag
                                * that have clients */
                                        drw_rect(m->showindicator ? drw : NULL,
                                                x, y, w, boxw - 2,
                                                1, urg & 1 << 1);
                                else if (!m->showvacanttags)
                                /* if more than one tag selected, draw smaller
                                 * line at the bottom of another selected non
                                 * vacant tag */
                                        drw_rect(drw,
                                                x + (3 * boxw + 1),
                                                h - (boxw - 2),
                                                w - (6 * boxw + 1), boxw - 2,
                                                1, urg & 1 << 1);
                        } else {
                                /* draw half line at the bottom of every non
                                 * vacant tag */
                                drw_rect(m->showvacanttags ? drw : NULL,
                                        x + (3 * boxw + 1), h - (boxw - 2),
                                        w - (6 * boxw + 1), boxw - 2,
                                        1, urg & 1 << 1);
                        }
                }
                x += w;
        }
        w = TEXTW(m->ltsymbol);
        drw_setscheme(drw, scheme[m == selmon ? SchemeLayout : SchemeInactive]);
        x = drw_text(drw, x, y, w, h, lrpad / 2, m->ltsymbol, 0);

        if (m == selmon) {
                blw = w, ble = x;
                w = m->ww - wstext - x;
        } else {
                w = m->ww - x;
        }
	if (w > h) {
		if (m->sel && m->showtitle) {
                        if (m->colorfultitle)
                                for (i = 0; i < LENGTH(tags); i++)
                                        drw_setscheme(m->sel->tags & 1 << i
                                                && m == selmon ? drw : NULL,
                                                scheme[titleschemes[i]]);
                        else
                                drw_setscheme(m == selmon ? drw : NULL,
                                        scheme[m->sel->isfloating ?
                                        SchemeTitleFloat : SchemeTitle]);
                        if (m->centertitle) {
                                int width = w - 2 * sp;
                                int txtwidth = (int)(TEXTW(m->sel->name))
                                                - (lrpad + (2 * sp));
                                int centerpad = (width - txtwidth) / 2;
                                titlepad = centerpad >= lrpad / 2
                                                ? centerpad - sp
                                                : (lrpad / 2) + ((2 * sp) / 2);
                        }
			drw_text(drw, x, y, w - 2 * sp, h,
                                titlepad, m->sel->name, 0);
                        drw_rect(m->sel->isfloating ? drw : NULL,
                                x + boxs + titlepad - lrpad / 2, boxs + y,
                                boxw, boxw, m->sel->isfixed, 0);
		} else {
			drw_setscheme(drw, scheme[SchemeBar]);
			drw_rect(drw, x, y, w - 2 * sp, h, 1, 1);
		}
	}
	drw_map(drw, m->barwin, 0, 0, m->ww, bh);
}

void
drawbars(void)
{
	Monitor *m;

	for (m = mons; m; m = m->next)
		drawbar(m);
}

void
dwindle(Monitor *m)
{
	unsigned int i, n;
	int nx, ny, nw, nh;
	int oh, ov, ih, iv;
	int nv, hrest = 0, wrest = 0, r = 1;
	Client *c;

	getgaps(m, &oh, &ov, &ih, &iv, &n);
	if (n == 0)
		return;

	nx = m->wx + ov;
	ny = m->wy + oh;
	nw = m->ww - 2*ov;
	nh = m->wh - 2*oh;

	for (i = 0, c = nexttiled(m->clients); c; c = nexttiled(c->next)) {
		if (r) {
			if ((i % 2 && (nh - ih) / 2 <= (bh + 2*c->bw))
			|| (!(i % 2) && (nw - iv) / 2 <= (bh + 2*c->bw)))
				r = 0;
			if (r && i < n - 1) {
				if (i % 2) {
					nv = (nh - ih) / 2;
					hrest = nh - 2*nv - ih;
					nh = nv;
				} else {
					nv = (nw - iv) / 2;
					wrest = nw - 2*nv - iv;
					nw = nv;
				}
			}

			if ((i % 4) == 0) {
                                ny += nh + ih;
                                nh += hrest;
			}
			else if ((i % 4) == 1) {
				nx += nw + iv;
				nw += wrest;
			}
			else if ((i % 4) == 2) {
				ny += nh + ih;
				nh += hrest;
				if (i < n - 1)
					nw += wrest;
			}
			else if ((i % 4) == 3) {
                                nx += nw + iv;
                                nw -= wrest;
			}
			if (i == 0)	{
				if (n != 1) {
					nw = (m->ww - iv - 2*ov)
                                                - (m->ww - iv - 2*ov)
                                                * (1 - m->mfact);
					wrest = 0;
				}
				ny = m->wy + oh;
			}
			else if (i == 1)
				nw = m->ww - nw - iv - 2*ov;
			i++;
		}

		resize(c, nx, ny, nw - (2*c->bw), nh - (2*c->bw), 0);
	}
}

void
enternotify(XEvent *e)
{
	Client *c;
	Monitor *m;
	XCrossingEvent *ev = &e->xcrossing;

	if ((ev->mode != NotifyNormal || ev->detail == NotifyInferior)
        && ev->window != root)
		return;
	c = wintoclient(ev->window);
	m = c ? c->mon : wintomon(ev->window);
	if (m != selmon) {
		unfocus(selmon->sel, 1);
		selmon = m;
	} else if (!c || c == selmon->sel)
		return;
	focus(c);
}

void
expose(XEvent *e)
{
	Monitor *m;
	XExposeEvent *ev = &e->xexpose;

	if (ev->count == 0 && (m = wintomon(ev->window)))
		drawbar(m);
}

Client *
findbefore(Client *c)
{
	Client *tmp;
	if (c == selmon->clients)
		return NULL;
	for (tmp = selmon->clients; tmp && tmp->next != c; tmp = tmp->next);
	return tmp;
}

void
floatpos(const Arg *arg)
{
	Client *c = selmon->sel;

	if (!c || (selmon->lt[selmon->sellt]->arrange && !c->isfloating))
		return;

	setfloatpos(c, (char *)arg->v);
	resizeclient(c, c->x, c->y, c->w, c->h);

	XRaiseWindow(dpy, c->win);
	XWarpPointer(dpy, None, c->win, 0, 0, 0, 0, c->w/2, c->h/2);
}

void
focus(Client *c)
{
	if (!c || !ISVISIBLE(c))
		for (c = selmon->stack; c && !ISVISIBLE(c); c = c->snext);
	if (selmon->sel && selmon->sel != c)
		unfocus(selmon->sel, 0);
	if (c) {
		if (c->mon != selmon)
			selmon = c->mon;
		if (c->isurgent)
			seturgent(c, 0);
		detachstack(c);
		attachstack(c);
		grabbuttons(c, 1);
		if(c->isfloating)
                        XSetWindowBorder(dpy, c->win,
                                        scheme[c->mon->showtitle ? SchemeSel
                                        : SchemeFloat][ColBorder].pixel);
		else
			XSetWindowBorder(dpy, c->win,
                                scheme[SchemeSel][ColBorder].pixel);
		setfocus(c);
	} else {
		XSetInputFocus(dpy, root, RevertToPointerRoot, CurrentTime);
		XDeleteProperty(dpy, root, netatom[NetActiveWindow]);
	}
	selmon->sel = c;
	drawbars();
}

/* there are some broken focus acquiring clients needing extra handling */
void
focusin(XEvent *e)
{
	XFocusChangeEvent *ev = &e->xfocus;

	if (selmon->sel && ev->window != selmon->sel->win)
		setfocus(selmon->sel);
}

void
focusmaster(const Arg *arg)
{
	Client *c;

	if (selmon->nmaster < 1) return;

	c = nexttiled(selmon->clients);

	if (c) focus(c);
}

void
focusmon(const Arg *arg)
{
	Monitor *m;

	if (!mons->next) return;
	if ((m = dirtomon(arg->i)) == selmon) return;
	unfocus(selmon->sel, 0);
	selmon = m;
	focus(NULL);
}

void
focusstack(const Arg *arg)
{
	Client *c = NULL, *i;

	if (!selmon->sel || (selmon->sel->isfullscreen && lockfullscreen))
		return;
	if (arg->i > 0) {
		for (c = selmon->sel->next; c && !ISVISIBLE(c); c = c->next);
		if (!c)
			for (c = selmon->clients;
                                c && !ISVISIBLE(c);
                                c = c->next);
	} else {
		for (i = selmon->clients; i != selmon->sel; i = i->next)
			if (ISVISIBLE(i)) c = i;
		if (!c)
			for (; i; i = i->next)
				if (ISVISIBLE(i)) c = i;
	}
	if (c) {
		focus(c);
		restack(selmon);
	}
}

/*
 * Gappless grid layout + gaps
 * https://dwm.suckless.org/patches/gaplessgrid/
 */
void
gaplessgrid(Monitor *m)
{
	unsigned int i, n;
	int x, y, cols, rows, ch, cw, cn, rn, rrest, crest; // counters
	int oh, ov, ih, iv;
	Client *c;

	getgaps(m, &oh, &ov, &ih, &iv, &n);
	if (n == 0) return;

	/* grid dimensions */
	for (cols = 0; cols <= n/2; cols++)
		if (cols*cols >= n) break;
        /* set layout against the general calculation: not 1:2:2, but 2:3 */
	if (n == 5) cols = 2;
	rows = n/cols;
	cn = rn = 0; // reset column no, row no, client count

	ch = (m->wh - 2*oh - ih * (rows - 1)) / rows;
	cw = (m->ww - 2*ov - iv * (cols - 1)) / cols;
	rrest = (m->wh - 2*oh - ih * (rows - 1)) - ch * rows;
	crest = (m->ww - 2*ov - iv * (cols - 1)) - cw * cols;
	x = m->wx + ov;
	y = m->wy + oh;

	for (i = 0, c = nexttiled(m->clients); c; i++, c = nexttiled(c->next)) {
		if (i/rows + 1 > cols - n%cols) {
			rows = n/cols + 1;
			ch = (m->wh - 2*oh - ih * (rows - 1)) / rows;
			rrest = (m->wh - 2*oh - ih * (rows - 1)) - ch * rows;
		}
		resize( c, x, y + rn*(ch + ih) + MIN(rn, rrest),
			cw + (cn < crest ? 1 : 0) - 2*c->bw,
			ch + (rn < rrest ? 1 : 0) - 2*c->bw, 0);
		rn++;
		if (rn >= rows) {
			rn = 0;
			x += cw + ih + (cn < crest ? 1 : 0);
			cn++;
		}
	}
}

Atom
getatomprop(Client *c, Atom prop)
{
	int di;
	unsigned long dl;
	unsigned char *p = NULL;
	Atom da, atom = None;

	if (XGetWindowProperty(dpy, c->win, prop, 0L, sizeof atom, 0, XA_ATOM,
		        &da, &di, &dl, &dl, &p)
        == Success && p) {
		atom = *(Atom *)p;
		XFree(p);
	}
	return atom;
}

void
getfacts(Monitor *m, int msize, int ssize,
        float *mf, float *sf, int *mr, int *sr)
{
	unsigned int n;
	float mfacts = 0, sfacts = 0;
	int mtotal = 0, stotal = 0;
	Client *c;

	for (n = 0, c = nexttiled(m->clients); c; c = nexttiled(c->next), n++)
		if (n < m->nmaster) mfacts += c->cfact;
		else sfacts += c->cfact;

	for (n = 0, c = nexttiled(m->clients); c; c = nexttiled(c->next), n++)
		if (n < m->nmaster) mtotal += msize * (c->cfact / mfacts);
		else stotal += ssize * (c->cfact / sfacts);

	*mf = mfacts; // total factor of master area
	*sf = sfacts; // total factor of stack area
	*mr = msize - mtotal; // the remainder (rest) of pixels after an even master split
	*sr = ssize - stotal; // the remainder (rest) of pixels after an even stack split
}

void
getfloatpos(int pos, char pCh, int size, char sCh,
        int min_p, int max_s, int cp, int cs, int cbw,
        int defgrid, int *out_p, int *out_s)
{
	int abs_p, abs_s, i, delta, rest;

	abs_p = pCh == 'A' || pCh == 'a';
	abs_s = sCh == 'A' || sCh == 'a';

	cs += 2*cbw;

	switch(pCh) {
	case 'A': // absolute position
		cp = pos;
		break;
	case 'a': // absolute relative position
		cp += pos;
		break;
	case 'y':
	case 'x': // client relative position
		cp = MIN(cp + pos, min_p + max_s);
		break;
	case 'Y':
	case 'X': // client position relative to monitor
		cp = min_p + MIN(pos, max_s);
		break;
	case 'S': // fixed client position (sticky)
	case 'C': // fixed client position (center)
	case 'Z': // fixed client right-hand position (position + size)
		if (pos == -1) break;
		pos = MAX(MIN(pos, max_s), 0);
		if (pCh == 'Z') cs = abs((cp + cs) - (min_p + pos));
		else if (pCh == 'C') cs = abs((cp + cs / 2) - (min_p + pos));
		else cs = abs(cp - (min_p + pos));
		cp = min_p + pos;
		sCh = 0; // size determined by position, override defined size
		break;
	case 'G': // grid
		if (pos <= 0) pos = defgrid; // default configurable
		if (size == 0 || pos < 2 || (sCh != 'p' && sCh != 'P')) break;
		delta = (max_s - cs) / (pos - 1);
		rest = max_s - cs - delta * (pos - 1);
		if (sCh == 'P') {
			if (size < 1 || size > pos) break;
			cp = min_p + delta * (size - 1);
		} else {
			for (i = 0;
                                i < pos && cp >= min_p + delta * i
                                + (i > pos - rest ? i + rest - pos + 1 : 0);
                                i++);
			cp = min_p + delta * (MAX(MIN(i + size, pos), 1) - 1)
                                + (i > pos - rest ? i + rest - pos + 1 : 0);
		}
		break;
	}

	switch(sCh) {
	case 'A': // absolute size
		cs = size;
		break;
	case 'a': // absolute relative size
		cs = MAX(1, cs + size);
		break;
	case '%': // client size percentage in relation to monitor window area size
		if (size <= 0) break;
		size = max_s * MIN(size, 100) / 100;
		/* falls through */
	case 'h':
	case 'w': // size relative to client
		if (sCh == 'w' || sCh == 'h') {
			if (size == 0) break;
			size += cs;
		}
		/* falls through */
	case 'H':
	case 'W': // normal size, position takes precedence
		if (pCh == 'S' && cp + size > min_p + max_s)
			size = min_p + max_s - cp;
		else if (size > max_s)
			size = max_s;

		if (pCh == 'C') { // fixed client center, expand or contract client
			delta = size - cs;
			if (delta < 0
                        || (cp - delta / 2 + size <= min_p + max_s))
				cp -= delta / 2;
			else if (cp - delta / 2 < min_p) cp = min_p;
			else if (delta) cp = min_p + max_s;
		} else if (pCh == 'Z') cp -= size - cs;

		cs = size;
		break;
	}

	if (pCh == '%') // client mid-point position in relation to monitor window area size
		cp = min_p + max_s * MAX(MIN(pos, 100), 0) / 100 - (cs) / 2;

	if (!abs_p && cp < min_p) cp = min_p;
	if (cp + cs > min_p + max_s && !(abs_p && abs_s)) {
		if (abs_p || cp == min_p) cs = min_p + max_s - cp;
		else cp = min_p + max_s - cs;
	}

	*out_p = cp;
	*out_s = MAX(cs - 2*cbw, 1);
}

void
getgaps(Monitor *m, int *oh, int *ov, int *ih, int *iv, unsigned int *nc)
{
	unsigned int n, oe, ie;
	oe = ie = selmon->pertag->enablegaps[selmon->pertag->curtag];
	Client *c;

	for (n = 0, c = nexttiled(m->clients); c; c = nexttiled(c->next), n++);
	if (smartgaps && n == 1) oe = 0; // outer gaps disabled when only one client

	*oh = m->gappoh*oe; // outer horizontal gap
	*ov = m->gappov*oe; // outer vertical gap
	*ih = m->gappih*ie; // inner horizontal gap
	*iv = m->gappiv*ie; // inner vertical gap
	*nc = n;            // number of clients
}

pid_t
getparentprocess(pid_t p)
{
	unsigned int v = 0;
	FILE *f;
	char buf[256];
	snprintf(buf, sizeof(buf) - 1, "/proc/%u/stat", (unsigned)p);

	if (!(f = fopen(buf, "r"))) return 0;

	fscanf(f, "%*u %*s %*c %u", &v);
	fclose(f);
	return (pid_t)v;
}

int
getrootptr(int *x, int *y)
{
	int di;
	unsigned int dui;
	Window dummy;

	return XQueryPointer(dpy, root, &dummy, &dummy, x, y, &di, &di, &dui);
}

long
getstate(Window w)
{
	int format;
	long result = -1;
	unsigned char *p = NULL;
	unsigned long n, extra;
	Atom real;

	if (XGetWindowProperty(
                        dpy, w, wmatom[WMState], 0L, 2L, False, wmatom[WMState],
		        &real, &format, &n, &extra, (unsigned char **)&p)
        != Success)
		return -1;
	if (n != 0) result = *p;
	XFree(p);
	return result;
}

int
gettextprop(Window w, Atom atom, char *text, unsigned int size)
{
	char **list = NULL;
	int n;
	XTextProperty name;

	if (!text || size == 0) return 0;
	text[0] = '\0';
	if (!XGetTextProperty(dpy, w, &name, atom) || !name.nitems)
		return 0;
	if (name.encoding == XA_STRING)
		strncpy(text, (char *)name.value, size - 1);
	else {
		if (XmbTextPropertyToTextList(dpy, &name, &list, &n)
                >= Success && n > 0 && *list) {
			strncpy(text, *list, size - 1);
			XFreeStringList(list);
		}
	}
	text[size - 1] = '\0';
	XFree(name.value);
	return 1;
}

void
grabbuttons(Client *c, int focused)
{
	updatenumlockmask();
	{
		unsigned int i, j;
		unsigned int modifiers[] = {
                        0, LockMask, numlockmask, numlockmask|LockMask
                };
		XUngrabButton(dpy, AnyButton, AnyModifier, c->win);
		if (!focused)
			XGrabButton(dpy, AnyButton, AnyModifier, c->win, 0,
				BUTTONMASK, GrabModeSync, GrabModeSync,
                                None, None);
		for (i = 0; i < LENGTH(buttons); i++)
			if (buttons[i].click == ClkClientWin)
				for (j = 0; j < LENGTH(modifiers); j++)
					XGrabButton(dpy, buttons[i].button,
						buttons[i].mask | modifiers[j],
						c->win, 0, BUTTONMASK,
						GrabModeAsync, GrabModeSync,
                                                None, None);
	}
}

void
grabkeys(void)
{
	updatenumlockmask();
	{
		unsigned int i, j;
		unsigned int modifiers[] = {
                        0, LockMask, numlockmask, numlockmask|LockMask
                };
		KeyCode code;

		XUngrabKey(dpy, AnyKey, AnyModifier, root);
		for (i = 0; i < LENGTH(keys); i++)
			if ((code = XKeysymToKeycode(dpy, keys[i].keysym)))
				for (j = 0; j < LENGTH(modifiers); j++)
					XGrabKey(dpy, code,
                                                keys[i].mod | modifiers[j],
                                                root, True,
                                                GrabModeAsync, GrabModeAsync);
	}
}

void
incnmaster(const Arg *arg)
{
        int nmaster = MAX(selmon->nmaster + arg->i, 0);
	selmon->pertag->nmasters[selmon->pertag->curtag] = nmaster;
	selmon->nmaster = selmon->pertag->nmasters[selmon->pertag->curtag];
	arrange(selmon);
}

void
incrgaps(const Arg *arg)
{
	setgaps(selmon->gappoh + arg->i,
		selmon->gappov + arg->i,
		selmon->gappih + arg->i,
		selmon->gappiv + arg->i);
}

void
incrigaps(const Arg *arg)
{
	setgaps(selmon->gappoh, selmon->gappov,
		selmon->gappih + arg->i, selmon->gappiv + arg->i);
}

void
incrihgaps(const Arg *arg)
{
	setgaps(selmon->gappoh, selmon->gappov,
		selmon->gappih + arg->i, selmon->gappiv);
}

void
incrivgaps(const Arg *arg)
{
	setgaps(selmon->gappoh, selmon->gappov,
		selmon->gappih, selmon->gappiv + arg->i);
}

void
incrogaps(const Arg *arg)
{
	setgaps(selmon->gappoh + arg->i, selmon->gappov + arg->i,
		selmon->gappih, selmon->gappiv);
}

void
incrohgaps(const Arg *arg)
{
	setgaps(selmon->gappoh + arg->i, selmon->gappov,
		selmon->gappih, selmon->gappiv);
}

void
incrovgaps(const Arg *arg)
{
	setgaps(selmon->gappoh, selmon->gappov + arg->i,
		selmon->gappih, selmon->gappiv);
}

void
inplacerotate(const Arg *arg)
{
	if(!selmon->sel || (selmon->sel->isfloating && !arg->f)) return;

	unsigned int selidx = 0, i = 0;
	Client *c = NULL;
        Client *shead = NULL, *stail = NULL;
        Client *mhead = NULL, *mtail = NULL;

	/* Determine positionings for insertclient */
	for (c = selmon->clients; c; c = c->next) {
		if (ISVISIBLE(c) && !(c->isfloating)) {
		        if (selmon->sel == c) selidx = i;
                        if (i == selmon->nmaster - 1) mtail = c;
                        if (i == selmon->nmaster) shead = c;
                        if (mhead == NULL) mhead = c;
                        stail = c;
                        i++;
		}
	}

	/* All clients rotate */
	if (arg->i == 2)
                insertclient(selmon->clients, stail, 0);
	if (arg->i == -2)
                insertclient(stail, selmon->clients, 1);
	/* Stack xor master rotate */
	if (arg->i == -1 && selidx >= selmon->nmaster)
                insertclient(stail, shead, 1);
	if (arg->i == 1 && selidx >= selmon->nmaster)
                insertclient(shead, stail, 0);
	if (arg->i == -1 && selidx < selmon->nmaster)
                insertclient(mtail, mhead, 1);
	if (arg->i == 1 && selidx < selmon->nmaster)
                insertclient(mhead, mtail, 0);

	arrange(selmon);
}

void
insertclient(Client *item, Client *insertItem, int after)
{
	Client *c;
	if (item == NULL || insertItem == NULL || item == insertItem)
                return;
	detach(insertItem);
	if (!after && selmon->clients == item) {
		attach(insertItem);
		return;
	}
	if (after) {
		c = item;
	} else {
		for (c = selmon->clients; c; c = c->next) {
                        if (c->next == item) break;
                }
	}
	insertItem->next = c->next;
	c->next = insertItem;
}

int
isdescprocess(pid_t p, pid_t c)
{
        while (p != c && c != 0) c = getparentprocess(c);

	return (int)c;
}

#ifdef XINERAMA
static int
isuniquegeom(XineramaScreenInfo *unique, size_t n, XineramaScreenInfo *info)
{
	while (n--)
		if (unique[n].x_org == info->x_org
                && unique[n].y_org == info->y_org
                && unique[n].width == info->width
                && unique[n].height == info->height)
			return 0;
	return 1;
}
#endif /* XINERAMA */

void
keypress(XEvent *e)
{
	unsigned int i;
	KeySym keysym;
	XKeyEvent *ev;

	ev = &e->xkey;
	keysym = XKeycodeToKeysym(dpy, (KeyCode)ev->keycode, 0);
	for (i = 0; i < LENGTH(keys); i++)
		if (keysym == keys[i].keysym
		&& CLEANMASK(keys[i].mod) == CLEANMASK(ev->state)
		&& keys[i].func)
			keys[i].func(&(keys[i].arg));
}

void
killclient(const Arg *arg)
{
	if (!selmon->sel) return;
	if (!sendevent(selmon->sel, wmatom[WMDelete])) {
		XGrabServer(dpy);
		XSetErrorHandler(xerrordummy);
		XSetCloseDownMode(dpy, DestroyAll);
		XKillClient(dpy, selmon->sel->win);
		XSync(dpy, 0);
		XSetErrorHandler(xerror);
		XUngrabServer(dpy);
	}
}

void
manage(Window w, XWindowAttributes *wa)
{
	Client *c, *t = NULL, *term = NULL;
	Window trans = None;
	XWindowChanges wc;

	c = ecalloc(1, sizeof(Client));
	c->cfact = 1.0;
	c->win = w;
	c->pid = winpid(w);
	/* geometry */
	c->x = c->oldx = wa->x;
	c->y = c->oldy = wa->y;
	c->w = c->oldw = wa->width;
	c->h = c->oldh = wa->height;
	c->oldbw = wa->border_width;
	c->ignoresizehints = 0;

	updatetitle(c);
	c->bw = borderpx;
	if (XGetTransientForHint(dpy, w, &trans) && (t = wintoclient(trans))) {
		c->mon = t->mon;
		c->tags = t->tags;
	} else {
		c->mon = selmon;
		applyrules(c);
		term = termforwin(c);
	}

	if (c->x + WIDTH(c) > c->mon->mx + c->mon->mw)
		c->x = c->mon->mx + c->mon->mw - WIDTH(c);
	if (c->y + HEIGHT(c) > c->mon->my + c->mon->mh)
		c->y = c->mon->my + c->mon->mh - HEIGHT(c);
	c->x = MAX(c->x, c->mon->mx);
	/* only fix client y-offset, if the client center might cover the bar */
	c->y = MAX(c->y, ((c->mon->by == c->mon->my)
                && (c->x + (c->w / 2) >= c->mon->wx)
		&& (c->x + (c->w / 2) < c->mon->wx + c->mon->ww))
                ? bh : c->mon->my);

	wc.border_width = c->bw;
	XConfigureWindow(dpy, w, CWBorderWidth, &wc);
        XSetWindowBorder(dpy, w, scheme[SchemeNorm][ColBorder].pixel);
	configure(c); /* propagates border_width, if size doesn't change */
	updatewindowtype(c);
	updatesizehints(c);
	updatewmhints(c);
	updatemotifhints(c);
        if (((c->isfloating && !c->isfullscreen) || c->isfixed
                || !c->mon->lt[c->mon->sellt]->arrange) && !c->isfloatpos)
                setfloatpos(c, "50% 50%");
	c->sfx = c->x;
	c->sfy = c->y;
	c->sfw = c->w;
	c->sfh = c->h;
	XSelectInput(dpy, w,
                EnterWindowMask
                |FocusChangeMask
                |PropertyChangeMask
                |StructureNotifyMask);
	grabbuttons(c, 0);
	if (!c->isfloating)
		c->isfloating = c->oldstate = trans != None || c->isfixed;
	if (c->isfloating) {
                XSetWindowBorder(dpy, w, scheme[c->mon->showtitle ? SchemeSel
                                : SchemeFloat][ColBorder].pixel);
		XRaiseWindow(dpy, c->win);
        }
	attachbelow(c);
	attachstack(c);
	XChangeProperty(dpy, root, netatom[NetClientList],
                XA_WINDOW, 32, PropModeAppend,
		(unsigned char *) &(c->win), 1);
	XChangeProperty(dpy, root, netatom[NetClientListStacking],
                        XA_WINDOW, 32, PropModePrepend,
                        (unsigned char *) &(c->win), 1);
        Atom target = XInternAtom(dpy, "_IS_FLOATING", 0);
	unsigned int floating[1] = {c->isfloating};
	XChangeProperty(dpy, c->win, target,
                        XA_CARDINAL, 32, PropModeReplace,
                        (unsigned char *)floating, 1);
	XMoveResizeWindow(dpy, c->win, c->x + 2 * sw, c->y, c->w, c->h); /* some windows require this */
	setclientstate(c, NormalState);
	if (c->mon == selmon)
		unfocus(selmon->sel, 0);
	c->mon->sel = c;
	arrange(c->mon);
	XMapWindow(dpy, c->win);
	if (term)
		swallow(term, c);
	focus(NULL);
}

void
mappingnotify(XEvent *e)
{
	XMappingEvent *ev = &e->xmapping;

	XRefreshKeyboardMapping(ev);
	if (ev->request == MappingKeyboard)
		grabkeys();
}

void
maprequest(XEvent *e)
{
	static XWindowAttributes wa;
	XMapRequestEvent *ev = &e->xmaprequest;

	if (!XGetWindowAttributes(dpy, ev->window, &wa))
		return;
	if (wa.override_redirect)
		return;
	if (!wintoclient(ev->window))
		manage(ev->window, &wa);
}

void
monocle(Monitor *m)
{
	unsigned int n;
        int oh, ov, ih, iv;
	Client *c;

        getgaps(m, &oh, &ov, &ih, &iv, &n);

	if (n > 0) /* override layout symbol */
		snprintf(m->ltsymbol, sizeof m->ltsymbol, "[%d]", n);
        for (c = nexttiled(m->clients); c; c = nexttiled(c->next)) {
		resize( c, m->wx + ov, m->wy + oh,
                        m->ww - 2 * c->bw - 2 * ov,
                        m->wh - 2 * c->bw - 2 * oh, 0);
        }
}

void
motionnotify(XEvent *e)
{
	static Monitor *mon = NULL;
        int x;
	Monitor *m;
	XMotionEvent *ev = &e->xmotion;

        if (ev->window == root) {
                if ((m = recttomon(ev->x_root, ev->y_root, 1, 1)) != mon
                && mon) {
                        unfocus(selmon->sel, 1);
                        selmon = m;
                        focus(NULL);
                }
                mon = m;
        } else if (ev->window == selmon->barwin
        && (x = selmon->ww - (2 * sp) - RSPAD - ev->x) > 0
        && (x -= wstext - LSPAD - RSPAD) <= 0) {
                updatedwmblockssig(x);
        } else if (selmon->statushandcursor) {
                selmon->statushandcursor = 0;
                XDefineCursor(dpy, selmon->barwin, cursor[CurNormal]->cursor);
        }
}

void
movemouse(const Arg *arg)
{
	int x, y, ocx, ocy, nx, ny;
	Client *c;
	Monitor *m;
	XEvent ev;
	Time lasttime = 0;

	if (!(c = selmon->sel))
		return;
	if (c->isfullscreen) /* no support moving fullscreen windows by mouse */
		return;
	restack(selmon);
	ocx = c->x;
	ocy = c->y;
	if (XGrabPointer(dpy, root, 0, MOUSEMASK, GrabModeAsync, GrabModeAsync,
	                None, cursor[CurMove]->cursor, CurrentTime)
        != GrabSuccess)
		return;
	if (!getrootptr(&x, &y))
		return;
	do {
		XMaskEvent(dpy,
                        MOUSEMASK|ExposureMask|SubstructureRedirectMask, &ev);
		switch(ev.type) {
		case ConfigureRequest:
		case Expose:
		case MapRequest:
			handler[ev.type](&ev);
			break;
		case MotionNotify:
			if ((ev.xmotion.time - lasttime) <= (1000 / 60))
				continue;
			lasttime = ev.xmotion.time;

			nx = ocx + (ev.xmotion.x - x);
			ny = ocy + (ev.xmotion.y - y);
			if (abs(selmon->wx - nx) < snap)
				nx = selmon->wx;
			else if (abs((selmon->wx + selmon->ww) - (nx + WIDTH(c)))
                        < snap)
				nx = selmon->wx + selmon->ww - WIDTH(c);
			if (abs(selmon->wy - ny) < snap)
				ny = selmon->wy;
			else if (abs((selmon->wy + selmon->wh)
                                        - (ny + HEIGHT(c)))
                        < snap)
				ny = selmon->wy + selmon->wh - HEIGHT(c);
			if (!c->isfloating && selmon->lt[selmon->sellt]->arrange
			&& (abs(nx - c->x) > snap || abs(ny - c->y) > snap))
				togglefloating(NULL);
			if (!selmon->lt[selmon->sellt]->arrange || c->isfloating)
				resize(c, nx, ny, c->w, c->h, 1);
			break;
		}
	} while (ev.type != ButtonRelease);
	XUngrabPointer(dpy, CurrentTime);
	if ((m = recttomon(c->x, c->y, c->w, c->h)) != selmon) {
		sendmon(c, m);
		selmon = m;
		focus(NULL);
	}
}

Client *
nexttagged(Client *c) {
	Client *walked = c->mon->clients;
	for(; walked && (walked->isfloating || !ISVISIBLEONTAG(walked, c->tags));
		walked = walked->next);
	return walked;
}

Client *
nexttiled(Client *c)
{
	for (; c && (c->isfloating || !ISVISIBLE(c)); c = c->next);
	return c;
}

void
pop(Client *c)
{
	detach(c);
	attach(c);
	focus(c);
	arrange(c->mon);
}

void
propertynotify(XEvent *e)
{
	Client *c;
	Window trans;
	XPropertyEvent *ev = &e->xproperty;

	if ((ev->window == root) && (ev->atom == XA_WM_NAME))
		updatestatus();
	else if (ev->state == PropertyDelete)
		return; /* ignore */
	else if ((c = wintoclient(ev->window))) {
		switch(ev->atom) {
		default: break;
		case XA_WM_TRANSIENT_FOR:
			if (!c->isfloating
                        && (XGetTransientForHint(dpy, c->win, &trans))
                        && (c->isfloating = (wintoclient(trans)) != NULL))
				arrange(c->mon);
			break;
		case XA_WM_NORMAL_HINTS:
			updatesizehints(c);
			break;
		case XA_WM_HINTS:
			updatewmhints(c);
			drawbars();
			break;
		}
		if (ev->atom == XA_WM_NAME || ev->atom == netatom[NetWMName]) {
			updatetitle(c);
			if (c == c->mon->sel)
				drawbar(c->mon);
		}
		if (ev->atom == netatom[NetWMWindowType])
			updatewindowtype(c);
		if (ev->atom == motifatom)
			updatemotifhints(c);
	}
}

void
quit(const Arg *arg)
{
	running = 0;
}

Monitor *
recttomon(int x, int y, int w, int h)
{
	Monitor *m, *r = selmon;
	int a, area = 0;

	for (m = mons; m; m = m->next)
		if ((a = INTERSECT(x, y, w, h, m)) > area) {
			area = a;
			r = m;
		}
	return r;
}

void
resetfact(const Arg *arg)
{
        Client *c;

	selmon->pertag->mfacts[selmon->pertag->curtag] = mfact;
	selmon->mfact = selmon->pertag->mfacts[selmon->pertag->curtag];
        for (c = nexttiled(selmon->clients); c; c = nexttiled(c->next)) {
                c->cfact = 1.0;
        }
        setgaps(gappoh, gappov, gappih, gappiv);
}

void
resize(Client *c, int x, int y, int w, int h, int interact)
{
	if (applysizehints(c, &x, &y, &w, &h, interact))
		resizeclient(c, x, y, w, h);
}

void
resizeclient(Client *c, int x, int y, int w, int h)
{
	XWindowChanges wc;

	c->oldx = c->x; c->x = wc.x = x;
	c->oldy = c->y; c->y = wc.y = y;
	c->oldw = c->w; c->w = wc.width = w;
	c->oldh = c->h; c->h = wc.height = h;
	wc.border_width = c->bw;
	XConfigureWindow(dpy, c->win,
                        CWX|CWY|CWWidth|CWHeight|CWBorderWidth, &wc);
	configure(c);
	XSync(dpy, 0);
}

void
resizemouse(const Arg *arg)
{
	int opx, opy, ocx, ocy, och, ocw, nx, ny, nw, nh;
	Client *c;
	Monitor *m;
	XEvent ev;
	int horizcorner, vertcorner;
	unsigned int dui;
	Window w;
	Time lasttime = 0;

	if (!(c = selmon->sel))
		return;
	if (c->isfullscreen) /* no support resizing fullscreen windows by mouse */
		return;
	restack(selmon);
	ocx = c->x;
	ocy = c->y;
	och = c->h;
	ocw = c->w;
	if (!XQueryPointer(dpy, c->win, &w, &w, &opx, &opy, &nx, &ny, &dui))
		return;
	horizcorner = nx < c->w / 2;
	vertcorner  = ny < c->h / 2;
	if (XGrabPointer(dpy, root, 0,
                        MOUSEMASK, GrabModeAsync, GrabModeAsync,
                        None, cursor[horizcorner | (vertcorner << 1)]->cursor,
                        CurrentTime)
        != GrabSuccess)
		return;
	do {
		XMaskEvent(dpy,
                        MOUSEMASK|ExposureMask|SubstructureRedirectMask, &ev);
		switch(ev.type) {
		case ConfigureRequest:
		case Expose:
		case MapRequest:
			handler[ev.type](&ev);
			break;
		case MotionNotify:
			if ((ev.xmotion.time - lasttime) <= (1000 / 60))
				continue;
			lasttime = ev.xmotion.time;

			nx = horizcorner ? (ocx + ev.xmotion.x - opx) : c->x;
			ny = vertcorner ? (ocy + ev.xmotion.y - opy) : c->y;
			nw = MAX( horizcorner
                                ? (ocx + ocw - nx)
                                : (ocw + (ev.xmotion.x - opx)), 1);
			nh = MAX( vertcorner
                                ? (ocy + och - ny)
                                : (och + (ev.xmotion.y - opy)), 1);
			if (c->mon->wx + nw >= selmon->wx
                        && c->mon->wx + nw <= selmon->wx + selmon->ww
			&& c->mon->wy + nh >= selmon->wy
                        && c->mon->wy + nh <= selmon->wy + selmon->wh)
			{
				if (!c->isfloating
                                && selmon->lt[selmon->sellt]->arrange
                                && (abs(nw - c->w) > snap
                                || abs(nh - c->h) > snap))
					togglefloating(NULL);
			}
			if (!selmon->lt[selmon->sellt]->arrange || c->isfloating)
				resizeclient(c, nx, ny, nw, nh);
			break;
		}
	} while (ev.type != ButtonRelease);
	XUngrabPointer(dpy, CurrentTime);
	while (XCheckMaskEvent(dpy, EnterWindowMask, &ev));
	if ((m = recttomon(c->x, c->y, c->w, c->h)) != selmon) {
		sendmon(c, m);
		selmon = m;
		focus(NULL);
	}
}

void
restack(Monitor *m)
{
	Client *c;
	XEvent ev;
	XWindowChanges wc;

	drawbar(m);
	if (!m->sel) return;
	if (m->sel->isfloating || !m->lt[m->sellt]->arrange)
		XRaiseWindow(dpy, m->sel->win);
	if (m->lt[m->sellt]->arrange) {
		wc.stack_mode = Below;
		wc.sibling = m->barwin;
		for (c = m->stack; c; c = c->snext)
			if (!c->isfloating && ISVISIBLE(c)) {
				XConfigureWindow(dpy, c->win,
                                        CWSibling|CWStackMode, &wc);
				wc.sibling = c->win;
			}
	}
	XSync(dpy, 0);
	while (XCheckMaskEvent(dpy, EnterWindowMask, &ev));
}

void
run(void)
{
	XEvent ev;
	/* main event loop */
	XSync(dpy, 0);
	while (running && !XNextEvent(dpy, &ev))
		if (handler[ev.type])
			handler[ev.type](&ev); /* call handler */
}

void
scan(void)
{
	unsigned int i, num;
	Window d1, d2, *wins = NULL;
	XWindowAttributes wa;

	if (XQueryTree(dpy, root, &d1, &d2, &wins, &num)) {
		for (i = 0; i < num; i++) {
			if (!XGetWindowAttributes(dpy, wins[i], &wa)
			|| wa.override_redirect
                        || XGetTransientForHint(dpy, wins[i], &d1))
				continue;
			if (wa.map_state == IsViewable
                        || getstate(wins[i]) == IconicState)
				manage(wins[i], &wa);
		}
		for (i = 0; i < num; i++) { /* now the transients */
			if (!XGetWindowAttributes(dpy, wins[i], &wa))
				continue;
			if (XGetTransientForHint(dpy, wins[i], &d1)
			&& (wa.map_state == IsViewable
                        || getstate(wins[i]) == IconicState))
				manage(wins[i], &wa);
		}
		if (wins)
			XFree(wins);
	}
}

void
sendmon(Client *c, Monitor *m)
{
	if (c->mon == m) return;
	unfocus(c, 1);
	detach(c);
	detachstack(c);
	c->mon = m;
	c->tags = m->tagset[m->seltags]; /* assign tags of target monitor */
	attachbelow(c);
	attachstack(c);
        if (c->isfloating) {
                setfloatpos(c, "50% 50%");
                resizeclient(c, c->x, c->y, c->w, c->h);
        }
	focus(NULL);
	arrange(NULL);
}

void
setcfact(const Arg *arg)
{
	float f;
	Client *c;

	c = selmon->sel;

	if(!arg || !c || !selmon->lt[selmon->sellt]->arrange)
                return;
	f = arg->f + c->cfact;
	if(arg->f == 1.0) {
                for (c = nexttiled(selmon->clients); c; c = nexttiled(c->next))
                        c->cfact = 1.0;
                arrange(selmon);
		return;
        } else if(arg->f == 0.0) {
		f = 1.0;
        } else if(f < 0.25 || f > 4.0) {
		return;
        }
	c->cfact = f;
	arrange(selmon);
}

void
setfloatpos(Client *c, const char *floatpos)
{
	char xCh, yCh, wCh, hCh;
	int x, y, w, h, wx, ww, wy, wh;
        int oh, ov, ih, iv;
        unsigned int n;

	if (!c || !floatpos) return;
	if (selmon->lt[selmon->sellt]->arrange && !c->isfloating)
		return;
	switch(sscanf(floatpos, "%d%c %d%c %d%c %d%c",
                        &x, &xCh, &y, &yCh, &w, &wCh, &h, &hCh)
        ) {
		case 4:
			if (xCh == 'w' || xCh == 'W') {
				w = x; wCh = xCh;
				h = y; hCh = yCh;
				x = -1; xCh = 'C';
				y = -1; yCh = 'C';
			} else if (xCh == 'p' || xCh == 'P') {
				w = x; wCh = xCh;
				h = y; hCh = yCh;
				x = 0; xCh = 'G';
				y = 0; yCh = 'G';
			} else {
				w = 0; wCh = 0;
				h = 0; hCh = 0;
			}
			break;
		case 8:
			break;
		default:
			return;
	}

	wx = c->mon->wx;
	wy = c->mon->wy;
	ww = c->mon->ww;
	wh = c->mon->wh;

        if (selmon->edgegap) {
                getgaps(c->mon, &oh, &ov, &ih, &iv, &n);
                wx += ov;
                wy += oh;
                ww -= 2*ov;
                wh -= 2*oh;
        }

	c->ignoresizehints = 1;

	getfloatpos(x, xCh, w, wCh,
                        wx, ww, c->x, c->w, c->bw,
                        floatposgrid_x, &c->x, &c->w);
	getfloatpos(y, yCh, h, hCh,
                        wy, wh, c->y, c->h, c->bw,
                        floatposgrid_y, &c->y, &c->h);
}

void
setclientstate(Client *c, long state)
{
	long data[] = { state, None };

	XChangeProperty(dpy, c->win, wmatom[WMState], wmatom[WMState], 32,
		PropModeReplace, (unsigned char *)data, 2);
}

void
setcurrentdesktop(void)
{
	long data[] = { 0 };
	XChangeProperty(dpy, root, netatom[NetCurrentDesktop],
                        XA_CARDINAL, 32, PropModeReplace,
                        (unsigned char *)data, 1);
}

void setdesktopnames(void)
{
	XTextProperty text;
	Xutf8TextListToTextProperty(dpy, tags, TAGSLENGTH,
                                        XUTF8StringStyle, &text);
	XSetTextProperty(dpy, root, &text, netatom[NetDesktopNames]);
}

int
sendevent(Client *c, Atom proto)
{
	int n;
	Atom *protocols;
	int exists = 0;
	XEvent ev;

	if (XGetWMProtocols(dpy, c->win, &protocols, &n)) {
		while (!exists && n--)
			exists = protocols[n] == proto;
		XFree(protocols);
	}
	if (exists) {
		ev.type = ClientMessage;
		ev.xclient.window = c->win;
		ev.xclient.message_type = wmatom[WMProtocols];
		ev.xclient.format = 32;
		ev.xclient.data.l[0] = proto;
		ev.xclient.data.l[1] = CurrentTime;
		XSendEvent(dpy, c->win, False, NoEventMask, &ev);
	}
	return exists;
}

void
setfocus(Client *c)
{
	if (!c->neverfocus) {
		XSetInputFocus(dpy, c->win, RevertToPointerRoot, CurrentTime);
		XChangeProperty(dpy, root, netatom[NetActiveWindow],
			XA_WINDOW, 32, PropModeReplace,
			(unsigned char *) &(c->win), 1);
	}
	sendevent(c, wmatom[WMTakeFocus]);
}

void
setfullscreen(Client *c, int fullscreen)
{
	if (fullscreen && !c->isfullscreen) {
		XChangeProperty(dpy, c->win, netatom[NetWMState], XA_ATOM, 32,
			PropModeReplace,
                        (unsigned char*)&netatom[NetWMFullscreen], 1);
		c->isfullscreen = 1;
		c->oldstate = c->isfloating;
		c->oldbw = c->bw;
		c->bw = 0;
		c->isfloating = 1;
		resizeclient(c, c->mon->mx, c->mon->my, c->mon->mw, c->mon->mh);
		XRaiseWindow(dpy, c->win);
	} else if (!fullscreen && c->isfullscreen){
		XChangeProperty(dpy, c->win, netatom[NetWMState], XA_ATOM, 32,
			PropModeReplace, (unsigned char*)0, 0);
		c->isfullscreen = 0;
		c->isfloating = c->oldstate;
		c->bw = c->oldbw;
		c->x = c->oldx;
		c->y = c->oldy;
		c->w = c->oldw;
		c->h = c->oldh;
		resizeclient(c, c->x, c->y, c->w, c->h);
		arrange(c->mon);
	}
}

void
setgaps(int oh, int ov, int ih, int iv)
{
	if (oh < 0) oh = 0;
	if (ov < 0) ov = 0;
	if (ih < 0) ih = 0;
	if (iv < 0) iv = 0;

	selmon->gappoh = oh;
	selmon->gappov = ov;
	selmon->gappih = ih;
	selmon->gappiv = iv;
        if (selmon->pertaggap)
                selmon->pertag->gaps[selmon->pertag->curtag] =
                        ((oh & 0xFF) << 0) | ((ov & 0xFF) << 8)
                        | ((ih & 0xFF) << 16) | ((iv & 0xFF) << 24);
	arrange(selmon);
}

void
setlayout(const Arg *arg)
{
	if (!arg || !arg->v || arg->v != selmon->lt[selmon->sellt]) {
                selmon->pertag->sellts[selmon->pertag->curtag] ^= 1;
		selmon->sellt = selmon->pertag->sellts[selmon->pertag->curtag];
        }
	if (arg && arg->v) {
                unsigned int curtag = selmon->pertag->curtag;
                unsigned int sellt = selmon->sellt;
                selmon->pertag->ltidxs[curtag][sellt] = (Layout *)arg->v;
		selmon->lt[sellt] = selmon->pertag->ltidxs[curtag][sellt];
        }
	strncpy(selmon->ltsymbol, selmon->lt[selmon->sellt]->symbol,
                sizeof selmon->ltsymbol);
	if (selmon->sel) arrange(selmon);
	else drawbar(selmon);
}

/* arg > 1.0 will set mfact absolutely */
void
setmfact(const Arg *arg)
{
	float f;

	if (!arg || !selmon->lt[selmon->sellt]->arrange)
		return;
	f = arg->f == mfact
                ? mfact : arg->f < 1.0
                        ? arg->f + selmon->mfact : arg->f - 1.0;
        if (f < 0.05 || f > 0.95) return;
	selmon->pertag->mfacts[selmon->pertag->curtag] = f;
	selmon->mfact = selmon->pertag->mfacts[selmon->pertag->curtag];
	arrange(selmon);
}

void
setnumdesktops(void)
{
	long data[] = { TAGSLENGTH };
	XChangeProperty(dpy, root, netatom[NetNumberOfDesktops],
                        XA_CARDINAL, 32, PropModeReplace,
                        (unsigned char *)data, 1);
}


void
setup(void)
{
	int i;
	XSetWindowAttributes wa;
	Atom utf8string;

	/* clean up any zombies immediately */
	sigchld(0);

	/* init screen */
	screen = DefaultScreen(dpy);
	sw = DisplayWidth(dpy, screen);
	sh = DisplayHeight(dpy, screen);
	root = RootWindow(dpy, screen);
	drw = drw_create(dpy, screen, root, sw, sh);
	if (!drw_fontset_create(drw, fonts, LENGTH(fonts)))
		die("no fonts could be loaded.");
	lrpad = drw->fonts->h;
	bh = (barheight ? barheight : drw->fonts->h + 2) + barborder * 2;
	bh = bargap ? bh : (bh - barborder * 2);
	sp = bargap ? sidepad : 0;
	vp = (topbar == 1) ? vertpad : - vertpad;
	vp = bargap ? vp : 0;
	updategeom();
	/* init atoms */
	utf8string = XInternAtom(dpy, "UTF8_STRING", 0);
	wmatom[WMProtocols] = XInternAtom(dpy, "WM_PROTOCOLS", 0);
	wmatom[WMDelete] = XInternAtom(dpy, "WM_DELETE_WINDOW", 0);
	wmatom[WMState] = XInternAtom(dpy, "WM_STATE", 0);
	wmatom[WMTakeFocus] = XInternAtom(dpy, "WM_TAKE_FOCUS", 0);
	wmatom[WMWindowRole] = XInternAtom(dpy, "WM_WINDOW_ROLE", 0);
	netatom[NetActiveWindow] = XInternAtom(dpy, "_NET_ACTIVE_WINDOW", 0);
	netatom[NetSupported] = XInternAtom(dpy, "_NET_SUPPORTED", 0);
	netatom[NetWMName] = XInternAtom(dpy, "_NET_WM_NAME", 0);
	netatom[NetWMState] = XInternAtom(dpy, "_NET_WM_STATE", 0);
	netatom[NetWMCheck] = XInternAtom(dpy, "_NET_SUPPORTING_WM_CHECK", 0);
	netatom[NetWMFullscreen] = XInternAtom(dpy,
                                        "_NET_WM_STATE_FULLSCREEN", 0);
	netatom[NetWMWindowType] = XInternAtom(dpy, "_NET_WM_WINDOW_TYPE", 0);
	netatom[NetWMWindowTypeDialog] = XInternAtom( dpy,
                                                "_NET_WM_WINDOW_TYPE_DIALOG", 0);
	netatom[NetClientList] = XInternAtom(dpy, "_NET_CLIENT_LIST", 0);
	netatom[NetClientListStacking] = XInternAtom( dpy,
                                                "_NET_CLIENT_LIST_STACKING", 0);
	netatom[NetDesktopViewport] = XInternAtom(dpy,
                                        "_NET_DESKTOP_VIEWPORT", 0);
	netatom[NetNumberOfDesktops] = XInternAtom(dpy,
                                        "_NET_NUMBER_OF_DESKTOPS", 0);
	netatom[NetCurrentDesktop] = XInternAtom(dpy, "_NET_CURRENT_DESKTOP", 0);
	netatom[NetDesktopNames] = XInternAtom(dpy, "_NET_DESKTOP_NAMES", 0);
	motifatom = XInternAtom(dpy, "_MOTIF_WM_HINTS", 0);
	/* init cursors */
	cursor[CurNormal] = drw_cur_create(drw, XC_left_ptr);
        cursor[CurHand] = drw_cur_create(drw, XC_hand2);
	cursor[CurResizeBR] = drw_cur_create(drw, XC_bottom_right_corner);
	cursor[CurResizeBL] = drw_cur_create(drw, XC_bottom_left_corner);
	cursor[CurResizeTR] = drw_cur_create(drw, XC_top_right_corner);
	cursor[CurResizeTL] = drw_cur_create(drw, XC_top_left_corner);
        cursor[CurMove] = drw_cur_create(drw, XC_fleur);
	/* init appearance */
	scheme = ecalloc(LENGTH(colors), sizeof(Clr *));
	for (i = 0; i < LENGTH(colors); i++)
		scheme[i] = drw_scm_create(drw, colors[i], 3);
	/* init bars */
	updatebars();
	updatestatus();
	updatebarpos(selmon);
	/* supporting window for NetWMCheck */
	wmcheckwin = XCreateSimpleWindow(dpy, root, 0, 0, 1, 1, 0, 0, 0);
	XChangeProperty(dpy, wmcheckwin, netatom[NetWMCheck], XA_WINDOW, 32,
		PropModeReplace, (unsigned char *) &wmcheckwin, 1);
	XChangeProperty(dpy, wmcheckwin, netatom[NetWMName], utf8string, 8,
		PropModeReplace, (unsigned char *) "dwm", 3);
	XChangeProperty(dpy, root, netatom[NetWMCheck], XA_WINDOW, 32,
		PropModeReplace, (unsigned char *) &wmcheckwin, 1);
	/* EWMH support per view */
	XChangeProperty(dpy, root, netatom[NetSupported], XA_ATOM, 32,
		PropModeReplace, (unsigned char *) netatom, NetLast);
	setnumdesktops();
	setcurrentdesktop();
	setdesktopnames();
	setviewport();
	XDeleteProperty(dpy, root, netatom[NetClientList]);
        XDeleteProperty(dpy, root, netatom[NetClientListStacking]);
	/* select events */
	wa.cursor = cursor[CurNormal]->cursor;
	wa.event_mask = SubstructureRedirectMask|SubstructureNotifyMask
		|ButtonPressMask|PointerMotionMask|EnterWindowMask
		|LeaveWindowMask|StructureNotifyMask|PropertyChangeMask;
	XChangeWindowAttributes(dpy, root, CWEventMask|CWCursor, &wa);
	XSelectInput(dpy, root, wa.event_mask);
	grabkeys();
	focus(NULL);
}

void
setviewport(void)
{
	long data[] = { 0, 0 };
	XChangeProperty(dpy, root, netatom[NetDesktopViewport],
                        XA_CARDINAL, 32, PropModeReplace,
                        (unsigned char *)data, 2);
}

void
seturgent(Client *c, int urg)
{
	XWMHints *h;

	c->isurgent = urg;
	if (!(h = XGetWMHints(dpy, c->win)))
		return;
	h->flags = urg ? (h->flags | XUrgencyHint) : (h->flags & ~XUrgencyHint);
	XSetWMHints(dpy, c->win, h);
	XFree(h);
}

void
shiftclient(const Arg *arg)
{
        if (selmon->sel) {
                Arg shifted;

                if (arg->i > 0) // left circular shift
                        shifted.ui = (selmon->tagset[selmon->seltags] << arg->i)
                                        | (selmon->tagset[selmon->seltags]
                                                >> (LENGTH(tags) - arg->i));

                else // right circular shift
                        shifted.ui = selmon->tagset[selmon->seltags] >> (-arg->i)
                                        | selmon->tagset[selmon->seltags]
                                                << (LENGTH(tags) + arg->i);

                selmon->sel->tags = shifted.ui & TAGMASK;
                view(&shifted);
        }
}

void
shiftview(const Arg *arg)
{
        Arg shifted;
        Client *client;
        unsigned int tagmask = 0;

        for (client = selmon->clients; client; client = client->next)
                tagmask = tagmask | client->tags;

        shifted.ui = selmon->tagset[selmon->seltags];

        if (arg->i > 0) // left circular shift
                do {
                        shifted.ui = (shifted.ui << arg->i)
                                        | (shifted.ui >> (LENGTH(tags) - arg->i));
                } while (tagmask && !(shifted.ui & tagmask));

        else // right circular shift
                do {
                        shifted.ui = (shifted.ui >> (-arg->i)
                                        | shifted.ui << (LENGTH(tags) + arg->i));
                } while (tagmask && !(shifted.ui & tagmask));

        view(&shifted);
}

void
showhide(Client *c)
{
	if (!c) return;
	if (ISVISIBLE(c)) {
		/* show clients top down */
		XMoveWindow(dpy, c->win, c->x, c->y);
		if ((!c->mon->lt[c->mon->sellt]->arrange || c->isfloating)
                && !c->isfullscreen)
			resize(c, c->x, c->y, c->w, c->h, 0);
		showhide(c->snext);
	} else {
		/* hide clients bottom up */
		showhide(c->snext);
		XMoveWindow(dpy, c->win, WIDTH(c) * -2, c->y);
	}
}

void
sigchld(int unused)
{
	if (signal(SIGCHLD, sigchld) == SIG_ERR)
		die("can't install SIGCHLD handler:");
	while (0 < waitpid(-1, NULL, WNOHANG));
}

void
sigdwmblocks(const Arg *arg)
{
        static int fd = -1;
        struct flock fl;
        union sigval sv;

        if (!dwmblockssig) return;
        fl.l_type = F_WRLCK;
        fl.l_whence = SEEK_SET;
        fl.l_start = 0;
        fl.l_len = 0;
        if (fd == -1) {
                if ((fd = open(DWMBLOCKSLOCKFILE, O_RDONLY)) == -1)
                        return;
                if (fcntl(fd, F_GETLK, &fl) == -1 || fl.l_type == F_UNLCK)
                        return;
        } else {
                if (fcntl(fd, F_GETLK, &fl) == -1) return;
                if (fl.l_type == F_UNLCK) {
                        close(fd);
                        if ((fd = open(DWMBLOCKSLOCKFILE, O_RDONLY)) == -1)
                                return;
                        fl.l_type = F_WRLCK;
                        if (fcntl(fd, F_GETLK, &fl) == -1
                        || fl.l_type == F_UNLCK)
                                return;
                }
        }
        sv.sival_int = (dwmblockssig << 8) | arg->i;
        sigqueue(fl.l_pid, SIGRTMIN, sv);
}

void
spawn(const Arg *arg)
{
	if (arg->v == dmenucmd) dmenumon[0] = '0' + selmon->num;
	if (fork() == 0) {
		if (dpy) close(ConnectionNumber(dpy));
		setsid();
		execvp(((char **)arg->v)[0], (char **)arg->v);
		fprintf(stderr, "dwm: execvp %s", ((char **)arg->v)[0]);
		perror(" failed");
		exit(EXIT_SUCCESS);
	}
}

void
swallow(Client *p, Client *c)
{

	if (c->noswallow || c->isterminal) return;
	if (c->noswallow && !swallowfloating && c->isfloating)
		return;

	detach(c);
	detachstack(c);

	setclientstate(c, WithdrawnState);
	XUnmapWindow(dpy, p->win);

	p->swallowing = c;
	c->mon = p->mon;

	Window w = p->win;
	p->win = c->win;
	c->win = w;
	updatetitle(p);
	XMoveResizeWindow(dpy, p->win, p->x, p->y, p->w, p->h);
	arrange(p->mon);
	configure(p);
	updateclientlist();
}

Client *
swallowingclient(Window w)
{
	Client *c;
	Monitor *m;

	for (m = mons; m; m = m->next) {
		for (c = m->clients; c; c = c->next) {
			if (c->swallowing && c->swallowing->win == w)
				return c;
		}
	}

	return NULL;
}

void
tag(const Arg *arg)
{
	if (selmon->sel && arg->ui & TAGMASK) {
		selmon->sel->tags = arg->ui & TAGMASK;
		focus(NULL);
		arrange(selmon);
	}
}

void
tagmon(const Arg *arg)
{
	if (!selmon->sel || !mons->next) return;
	sendmon(selmon->sel, dirtomon(arg->i));
}

void
tagview(const Arg *arg)
{
	if (selmon->sel && arg->ui & TAGMASK) {
		selmon->sel->tags = arg->ui & TAGMASK;
		focus(NULL);
                if(((arg->ui & TAGMASK) != TAGMASK))
			view(arg);
	}
}

Client *
termforwin(const Client *w)
{
	Client *c;
	Monitor *m;

	if (!w->pid || w->isterminal) return NULL;

	for (m = mons; m; m = m->next) {
		for (c = m->clients; c; c = c->next) {
			if (c->isterminal && !c->swallowing
                        && c->pid && isdescprocess(c->pid, w->pid))
				return c;
		}
	}

	return NULL;
}

void
tile(Monitor *m)
{
	unsigned int i, n;
	int oh, ov, ih, iv;
	int mx = 0, my = 0, mh = 0, mw = 0;
	int sx = 0, sy = 0, sh = 0, sw = 0;
	float mfacts = 0, sfacts = 0;
	int mrest, srest;
	Client *c;

	getgaps(m, &oh, &ov, &ih, &iv, &n);
	if (n == 0) return;

	sx = mx = m->wx + ov;
	sy = my = m->wy + oh;
	mh = m->wh - 2*oh - ih * (MIN(n, m->nmaster) - 1);
	sh = m->wh - 2*oh - ih * (n - m->nmaster - 1);
	sw = mw = m->ww - 2*ov;

	if (m->nmaster && n > m->nmaster) {
		sw = (mw - iv) * (1 - m->mfact);
		mw = mw - iv - sw;
		sx = mx + mw + iv;
	}

	getfacts(m, mh, sh, &mfacts, &sfacts, &mrest, &srest);

	for (i = 0, c = nexttiled(m->clients); c; c = nexttiled(c->next), i++) {
		if (i < m->nmaster) {
			resize( c, mx, my, mw - (2*c->bw),
                                mh * (c->cfact / mfacts)
                                + (i < mrest ? 1 : 0) - (2*c->bw), 0);
			my += HEIGHT(c) + ih;
		} else {
			resize( c, sx, sy, sw - (2*c->bw),
                                sh * (c->cfact / sfacts)
                                + ((i - m->nmaster) < srest ? 1 : 0)
                                - (2*c->bw), 0);
			sy += HEIGHT(c) + ih;
		}
        }
}

void
togglealttag(const Arg *arg)
{
	selmon->alttag = !selmon->alttag;
	drawbar(selmon);
}

void
togglebar(const Arg *arg)
{
        Client *c;
        selmon->pertag->showbars[selmon->pertag->curtag] = !selmon->showbar;
	selmon->showbar = selmon->pertag->showbars[selmon->pertag->curtag];
	updatebarpos(selmon);
        for (c = selmon->clients; c; c = c->next) {
                if ((c->isfloating && !c->isfullscreen)
                || !c->mon->lt[c->mon->sellt]->arrange)
                        setfloatpos(c, "0x 50%");
        }
	XMoveResizeWindow(dpy, selmon->barwin,
                selmon->wx + sp, selmon->by + vp,
                selmon->ww - 2 * sp, bh);
	arrange(selmon);
}

void
togglebargap(const Arg *arg)
{
        Client *c;
	Monitor *m;
        int h = (barheight ? barheight : drw->fonts->h + 2) + barborder * 2;
	for (m = mons; m; m = m->next) {
                m->bargap = !m->bargap;
                bh = m->bargap || gaplessborder ? h : h - barborder * 2;
                sp = m->bargap ? sidepad : 0;
                vp = m->bargap ? (topbar ? vertpad : - vertpad) : 0;
                updatebarpos(m);
                XMoveResizeWindow(dpy, m->barwin, m->wx + sp,
                        m->by + vp, m->ww - 2 * sp, bh);
                for (c = m->clients; c; c = c->next) {
                        if ((c->isfloating && !c->isfullscreen)
                        || !c->mon->lt[c->mon->sellt]->arrange)
                                setfloatpos(c, "0x 50%");
                }
                if (m->bargap && m == selmon) {
                        unsigned int curtag = selmon->pertag->curtag;
                        selmon->pertag->enablegaps[curtag] = m->bargap;
                        arrange(NULL);
                }
                arrange(m);
	}
}

void
togglefloating(const Arg *arg)
{
	if (!selmon->sel) return;
	if (selmon->sel->isfullscreen) /* no support for fullscreen windows */
		return;
        selmon->sel->isfloating = !selmon->sel->isfloating
                                  || selmon->sel->isfixed;
        if (selmon->sel->isfloating) {
		XSetWindowBorder(dpy, selmon->sel->win,
                                scheme[selmon->showtitle
                                        ? SchemeSel
                                        : SchemeFloat][ColBorder].pixel);
		resize(selmon->sel, selmon->sel->sfx, selmon->sel->sfy,
		       selmon->sel->sfw, selmon->sel->sfh, 0);
        } else {
		XSetWindowBorder(dpy, selmon->sel->win,
                                scheme[SchemeSel][ColBorder].pixel);
		/* save last known float dimensions */
		selmon->sel->sfx = selmon->sel->x;
		selmon->sel->sfy = selmon->sel->y;
		selmon->sel->sfw = selmon->sel->w;
		selmon->sel->sfh = selmon->sel->h;
        }

	Atom target = XInternAtom(dpy, "_IS_FLOATING", 0);
	unsigned int floating[1] = {selmon->sel->isfloating};
        XChangeProperty(dpy, selmon->sel->win, target,
                        XA_CARDINAL, 32, PropModeReplace,
                        (unsigned char *)floating, 1);
        floatpos(arg);
	arrange(selmon);
}

void
togglegaps(const Arg *arg)
{
        unsigned int curtag = selmon->pertag->curtag;
        int gap = !selmon->pertag->enablegaps[curtag];
	selmon->pertag->enablegaps[curtag] = gap;
        togglebargap(NULL);
	arrange(NULL);
}

void
toggleindicator(const Arg *arg)
{
        selmon->showindicator = !selmon->showindicator;
        drawbar(selmon);
}

void
toggletag(const Arg *arg)
{
	unsigned int newtags;

	if (!selmon->sel) return;
	newtags = selmon->sel->tags ^ (arg->ui & TAGMASK);
	if (newtags) {
		selmon->sel->tags = newtags;
		focus(NULL);
		arrange(selmon);
	}
	updatecurrentdesktop();
}

void
toggletagcolor(const Arg *arg)
{
        selmon->colorfultag = !selmon->colorfultag;
        drawbar(selmon);
}

void
toggletaggaps(const Arg *arg)
{
        selmon->pertaggap = !selmon->pertaggap;
	arrange(NULL);
}

void
toggletitle(const Arg *arg)
{
        selmon->showtitle = !selmon->showtitle;
        focus(NULL);
        drawbar(selmon);
}

void
toggletitlecolor(const Arg *arg)
{
        selmon->colorfultitle = !selmon->colorfultitle;
        drawbar(selmon);
}

void
toggletitlepos(const Arg *arg)
{
        selmon->centertitle = !selmon->centertitle;
        drawbar(selmon);
}

void
togglevacanttag(const Arg *arg)
{
	selmon->showvacanttags = !selmon->showvacanttags;
	drawbar(selmon);
}

void
toggleview(const Arg *arg)
{
	unsigned int newtagset = selmon->tagset[selmon->seltags]
                                        ^ (arg->ui & TAGMASK);
	int i;

        /* the first visible client should be the same after we add a new tag */
	/* we also want to be sure not to mutate the focus */
	Client *const c = nexttiled(selmon->clients);
	if (c) {
		Client * const selected = selmon->sel;
		pop(c);
		focus(selected);
	}

	if (newtagset) {

		if (newtagset == ~0) {
			selmon->pertag->prevtag = selmon->pertag->curtag;
			selmon->pertag->curtag = 0;
		}

		/* test if the user did not select the same tag */
		if (!(newtagset & 1 << (selmon->pertag->curtag - 1))) {
			selmon->pertag->prevtag = selmon->pertag->curtag;
			for (i = 0; !(newtagset & 1 << i); i++) ;
			selmon->pertag->curtag = i + 1;
		}

		/* apply settings for this view */
		selmon->nmaster = selmon->pertag->nmasters[
                                        selmon->pertag->curtag];
		selmon->mfact = selmon->pertag->mfacts[selmon->pertag->curtag];
		selmon->sellt = selmon->pertag->sellts[selmon->pertag->curtag];
		selmon->lt[selmon->sellt] = selmon->pertag->ltidxs
                                                [selmon->pertag->curtag]
                                                [selmon->sellt];
		selmon->lt[selmon->sellt^1] = selmon->pertag->ltidxs
                                                [selmon->pertag->curtag]
                                                [selmon->sellt^1];

		if (selmon->showbar
                != selmon->pertag->showbars[selmon->pertag->curtag])
			togglebar(NULL);

		selmon->tagset[selmon->seltags] = newtagset;
		focus(NULL);
		arrange(selmon);
	}
	updatecurrentdesktop();
}

void
unfocus(Client *c, int setfocus)
{
	if (!c) return;
	grabbuttons(c, 0);
	XSetWindowBorder(dpy, c->win, scheme[SchemeNorm][ColBorder].pixel);
	if (setfocus) {
		XSetInputFocus(dpy, root, RevertToPointerRoot, CurrentTime);
		XDeleteProperty(dpy, root, netatom[NetActiveWindow]);
	}
}

void
unmanage(Client *c, int destroyed)
{
	Monitor *m = c->mon;
	XWindowChanges wc;

	if (c->swallowing) {
		unswallow(c);
		return;
	}

	Client *s = swallowingclient(c->win);
	if (s) {
		free(s->swallowing);
		s->swallowing = NULL;
		arrange(m);
		focus(NULL);
		return;
	}

	detach(c);
	detachstack(c);
	if (!destroyed) {
		wc.border_width = c->oldbw;
		XGrabServer(dpy); /* avoid race conditions */
		XSetErrorHandler(xerrordummy);
		XConfigureWindow(dpy, c->win, CWBorderWidth, &wc); /* restore border */
		XUngrabButton(dpy, AnyButton, AnyModifier, c->win);
		setclientstate(c, WithdrawnState);
		XSync(dpy, 0);
		XSetErrorHandler(xerror);
		XUngrabServer(dpy);
	}
	free(c);
	if (!s) {
		arrange(m);
		focus(NULL);
		updateclientlist();
	}
}

void
unmapnotify(XEvent *e)
{
	Client *c;
	XUnmapEvent *ev = &e->xunmap;

	if ((c = wintoclient(ev->window))) {
		if (ev->send_event) setclientstate(c, WithdrawnState);
		else unmanage(c, 0);
	}
}

void
unswallow(Client *c)
{
	c->win = c->swallowing->win;

	free(c->swallowing);
	c->swallowing = NULL;

	/* unfullscreen the client */
	setfullscreen(c, 0);
	updatetitle(c);
	arrange(c->mon);
	XMapWindow(dpy, c->win);
	XMoveResizeWindow(dpy, c->win, c->x, c->y, c->w, c->h);
	setclientstate(c, NormalState);
	focus(NULL);
	arrange(c->mon);
}

void
updatebars(void)
{
	Monitor *m;
	XSetWindowAttributes wa = {
		.override_redirect = True,
		.background_pixmap = ParentRelative,
		.event_mask = ButtonPressMask|ExposureMask|PointerMotionMask
	};
	XClassHint ch = {"dwm", "dwm"};
	for (m = mons; m; m = m->next) {
		if (m->barwin) continue;
		m->barwin = XCreateWindow(dpy, root, m->wx + sp, m->by + vp,
                                m->ww - 2 * sp, bh, 0, DefaultDepth(dpy, screen),
				CopyFromParent, DefaultVisual(dpy, screen),
				CWOverrideRedirect|CWBackPixmap|CWEventMask, &wa);
		XDefineCursor(dpy, m->barwin, cursor[CurNormal]->cursor);
		XMapRaised(dpy, m->barwin);
		XSetClassHint(dpy, m->barwin, &ch);
	}
}

void
updatebarpos(Monitor *m)
{
        int vpad = m->bargap ? vertpad : 0;
	m->wy = m->my;
	m->wh = m->mh;
	if (m->showbar) {
		m->wh = m->wh - vpad - bh;
		m->by = m->topbar ? m->wy : m->wy + m->wh + vpad;
		m->wy = m->topbar ? m->wy + bh + vp : m->wy;
	} else
		m->by = -bh - vp;
}

void
updateclientlist()
{
	Client *c;
	Monitor *m;

	XDeleteProperty(dpy, root, netatom[NetClientList]);
	for (m = mons; m; m = m->next)
		for (c = m->clients; c; c = c->next)
			XChangeProperty(dpy, root, netatom[NetClientList],
				XA_WINDOW, 32, PropModeAppend,
				(unsigned char *) &(c->win), 1);

	XDeleteProperty(dpy, root, netatom[NetClientListStacking]);
	for (m = mons; m; m = m->next)
		for (c = m->stack; c; c = c->snext)
			XChangeProperty(dpy, root,
                                netatom[NetClientListStacking], XA_WINDOW, 32,
                                PropModeAppend, (unsigned char *) &(c->win), 1);
}

void updatecurrentdesktop(void)
{
	long rawdata[] = { selmon->tagset[selmon->seltags] };
	int i=0;
	while(*rawdata >> (i+1)) i++;
	long data[] = { i };
	XChangeProperty(dpy, root, netatom[NetCurrentDesktop],
                        XA_CARDINAL, 32, PropModeReplace,
                        (unsigned char *)data, 1);
}

void
updatedwmblockssig(int x)
{
        char *sts = stexts;
        char *stp = stexts;
        char tmp;

        while (*sts != '\0') {
                if ((unsigned char)*sts >= ' ') {
                        sts++;
                        continue;
                }
                tmp = *sts;
                *sts = '\0';
                x += TTEXTW(stp);
                *sts = tmp;
                if (x > 0) {
                        if (tmp == DELIMITERENDCHAR) break;
                        if (!selmon->statushandcursor) {
                                selmon->statushandcursor = 1;
                                XDefineCursor(dpy, selmon->barwin,
                                        cursor[CurHand]->cursor);
                        }
                        dwmblockssig = tmp;
                        return;
                }
                stp = ++sts;
        }
        if (selmon->statushandcursor) {
                selmon->statushandcursor = 0;
                XDefineCursor(dpy, selmon->barwin, cursor[CurNormal]->cursor);
        }
        dwmblockssig = 0;
}

int
updategeom(void)
{
	int dirty = 0;

#ifdef XINERAMA
	if (XineramaIsActive(dpy)) {
		int i, j, n, nn;
		Client *c;
		Monitor *m;
		XineramaScreenInfo *info = XineramaQueryScreens(dpy, &nn);
		XineramaScreenInfo *unique = NULL;

		for (n = 0, m = mons; m; m = m->next, n++);
		/* only consider unique geometries as separate screens */
		unique = ecalloc(nn, sizeof(XineramaScreenInfo));
		for (i = 0, j = 0; i < nn; i++)
			if (isuniquegeom(unique, j, &info[i]))
				memcpy(&unique[j++], &info[i],
                                        sizeof(XineramaScreenInfo));
		XFree(info);
		nn = j;
		if (n <= nn) { /* new monitors available */
			for (i = 0; i < (nn - n); i++) {
				for (m = mons; m && m->next; m = m->next);
				if (m) m->next = createmon();
				else mons = createmon();
			}
			for (i = 0, m = mons; i < nn && m; m = m->next, i++)
				if (i >= n
				|| unique[i].x_org != m->mx
                                || unique[i].y_org != m->my
				|| unique[i].width != m->mw
                                || unique[i].height != m->mh)
				{
					dirty = 1;
					m->num = i;
					m->mx = m->wx = unique[i].x_org;
					m->my = m->wy = unique[i].y_org;
					m->mw = m->ww = unique[i].width;
					m->mh = m->wh = unique[i].height;
					updatebarpos(m);
				}
		} else { /* less monitors available nn < n */
			for (i = nn; i < n; i++) {
				for (m = mons; m && m->next; m = m->next);
				while ((c = m->clients)) {
					dirty = 1;
					m->clients = c->next;
					detachstack(c);
					c->mon = mons;
					attachbelow(c);
					attachstack(c);
				}
				if (m == selmon) selmon = mons;
				cleanupmon(m);
			}
		}
		free(unique);
	} else
#endif /* XINERAMA */
	{ /* default monitor setup */
		if (!mons) mons = createmon();
		if (mons->mw != sw || mons->mh != sh) {
			dirty = 1;
			mons->mw = mons->ww = sw;
			mons->mh = mons->wh = sh;
			updatebarpos(mons);
		}
	}
	if (dirty) {
		selmon = mons;
		selmon = wintomon(root);
	}
	return dirty;
}

void
updatemotifhints(Client *c)
{
	Atom real;
	int format;
	unsigned char *p = NULL;
	unsigned long n, extra;
	unsigned long *motif;
	int width, height;

	if (!decorhints) return;

	if (XGetWindowProperty(dpy, c->win, motifatom, 0L, 5L, 0, motifatom,
	                &real, &format, &n, &extra, &p)
        == Success && p != NULL) {
		motif = (unsigned long*)p;
		if (motif[MWM_HINTS_FLAGS_FIELD] & MWM_HINTS_DECORATIONS) {
			width = WIDTH(c);
			height = HEIGHT(c);

			if (motif[MWM_HINTS_DECORATIONS_FIELD] & MWM_DECOR_ALL
                        || motif[MWM_HINTS_DECORATIONS_FIELD] & MWM_DECOR_BORDER
                        || motif[MWM_HINTS_DECORATIONS_FIELD] & MWM_DECOR_TITLE)
				c->bw = c->oldbw = borderpx;
			else
				c->bw = c->oldbw = 0;

			resize( c, c->x, c->y,
                                width - (2*c->bw), height - (2*c->bw), 0);
		}
		XFree(p);
	}
}

void
updatenumlockmask(void)
{
	unsigned int i, j;
	XModifierKeymap *modmap;

	numlockmask = 0;
	modmap = XGetModifierMapping(dpy);
	for (i = 0; i < 8; i++)
		for (j = 0; j < modmap->max_keypermod; j++)
			if (modmap->modifiermap[i * modmap->max_keypermod + j]
			== XKeysymToKeycode(dpy, XK_Num_Lock))
				numlockmask = (1 << i);
	XFreeModifiermap(modmap);
}

void
updatesizehints(Client *c)
{
	long msize;
	XSizeHints size;

	if (!XGetWMNormalHints(dpy, c->win, &size, &msize))
		/* size is uninitialized, ensure that size.flags aren't used */
		size.flags = PSize;
	if (size.flags & PBaseSize) {
		c->basew = size.base_width;
		c->baseh = size.base_height;
	} else if (size.flags & PMinSize) {
		c->basew = size.min_width;
		c->baseh = size.min_height;
	} else
		c->basew = c->baseh = 0;
	if (size.flags & PResizeInc) {
		c->incw = size.width_inc;
		c->inch = size.height_inc;
	} else
		c->incw = c->inch = 0;
	if (size.flags & PMaxSize) {
		c->maxw = size.max_width;
		c->maxh = size.max_height;
	} else
		c->maxw = c->maxh = 0;
	if (size.flags & PMinSize) {
		c->minw = size.min_width;
		c->minh = size.min_height;
	} else if (size.flags & PBaseSize) {
		c->minw = size.base_width;
		c->minh = size.base_height;
	} else
		c->minw = c->minh = 0;
	if (size.flags & PAspect) {
		c->mina = (float)size.min_aspect.y / size.min_aspect.x;
		c->maxa = (float)size.max_aspect.x / size.max_aspect.y;
	} else
		c->maxa = c->mina = 0.0;
	c->isfixed = (c->maxw && c->maxh
                        && c->maxw == c->minw
                        && c->maxh == c->minh);
}

void
updatestatus(void)
{
	char rawstext[STATUSLENGTH];

	if (gettextprop(root, XA_WM_NAME, rawstext, sizeof rawstext)) {
                char stextp[STATUSLENGTH];
                char *stp = stextp, *stc = stextc, *sts = stexts;

                for (char *rst = rawstext; *rst != '\0'; rst++)
                        if ((unsigned char)*rst >= ' ')
                                *(stp++) = *(stc++) = *(sts++) = *rst;
                        else if ((unsigned char)*rst > DELIMITERENDCHAR)
                                *(stc++) = *rst;
                        else
                                *(sts++) = *rst;
                *stp = *stc = *sts = '\0';
                wstext = TTEXTW(stextp) + LSPAD + RSPAD;
        } else {
                strcpy(stextc, "dwm-"VERSION);
                strcpy(stexts, stextc);
                wstext = TTEXTW(stextc) + LSPAD + RSPAD;
        }
        drawbar(selmon);
}

void
updatetitle(Client *c)
{
	if (!gettextprop(c->win, netatom[NetWMName], c->name, sizeof c->name))
		gettextprop(c->win, XA_WM_NAME, c->name, sizeof c->name);
	if (c->name[0] == '\0') /* hack to mark broken clients */
		strcpy(c->name, broken);
}

void
updatewindowtype(Client *c)
{
	Atom state = getatomprop(c, netatom[NetWMState]);
	Atom wtype = getatomprop(c, netatom[NetWMWindowType]);

	if (state == netatom[NetWMFullscreen])
		setfullscreen(c, 1);
	if (wtype == netatom[NetWMWindowTypeDialog])
		c->isfloating = 1;
}

void
updatewmhints(Client *c)
{
	XWMHints *wmh;

	if ((wmh = XGetWMHints(dpy, c->win))) {
		if (c == selmon->sel && wmh->flags & XUrgencyHint) {
			wmh->flags &= ~XUrgencyHint;
			XSetWMHints(dpy, c->win, wmh);
		} else
			c->isurgent = (wmh->flags & XUrgencyHint) ? 1 : 0;
		if (wmh->flags & InputHint)
			c->neverfocus = !wmh->input;
		else
			c->neverfocus = 0;
		XFree(wmh);
	}
}

void
view(const Arg *arg)
{
	int i;
	unsigned int tmptag;

	if ((arg->ui & TAGMASK) == selmon->tagset[selmon->seltags])
		return;
	selmon->seltags ^= 1; /* toggle sel tagset */
	if (arg->ui & TAGMASK) {
		selmon->tagset[selmon->seltags] = arg->ui & TAGMASK;
		selmon->pertag->prevtag = selmon->pertag->curtag;

		if (arg->ui == ~0)
			selmon->pertag->curtag = 0;
		else {
			for (i = 0; !(arg->ui & 1 << i); i++) ;
			selmon->pertag->curtag = i + 1;
		}
	} else {
		tmptag = selmon->pertag->prevtag;
		selmon->pertag->prevtag = selmon->pertag->curtag;
		selmon->pertag->curtag = tmptag;
        }

        unsigned int curtag = selmon->pertag->curtag;
        unsigned int sellt = selmon->sellt;
	selmon->nmaster = selmon->pertag->nmasters[curtag];
	selmon->mfact = selmon->pertag->mfacts[curtag];
	selmon->sellt = selmon->pertag->sellts[curtag];
	selmon->lt[sellt] = selmon->pertag->ltidxs[curtag][sellt];
	selmon->lt[sellt^1] = selmon->pertag->ltidxs[curtag][sellt^1];

        if (selmon->pertaggap) {
                selmon->gappoh = (selmon->pertag->gaps[curtag] & 0xff) >> 0;
                selmon->gappov = (selmon->pertag->gaps[curtag] & 0xff00) >> 8;
                selmon->gappih = (selmon->pertag->gaps[curtag] & 0xff0000) >> 16;
                selmon->gappiv = (selmon->pertag->gaps[curtag] & 0xff000000) >> 24;
        }

	if (selmon->showbar != selmon->pertag->showbars[selmon->pertag->curtag])
		togglebar(NULL);

	focus(NULL);
	arrange(selmon);
	updatecurrentdesktop();
}

pid_t
winpid(Window w)
{
	pid_t result = 0;
	xcb_res_client_id_spec_t spec = {0};
	spec.client = w;
	spec.mask = XCB_RES_CLIENT_ID_MASK_LOCAL_CLIENT_PID;

	xcb_generic_error_t *e = NULL;
	xcb_res_query_client_ids_cookie_t c = xcb_res_query_client_ids(
                                                        xcon, 1, &spec);
	xcb_res_query_client_ids_reply_t *r = xcb_res_query_client_ids_reply(
                                                        xcon, c, &e);

	if (!r) return (pid_t)0;

	xcb_res_client_id_value_iterator_t i =
                xcb_res_query_client_ids_ids_iterator(r);
	for (; i.rem; xcb_res_client_id_value_next(&i)) {
		spec = i.data->spec;
		if (spec.mask & XCB_RES_CLIENT_ID_MASK_LOCAL_CLIENT_PID) {
			uint32_t *t = xcb_res_client_id_value_value(i.data);
			result = *t;
			break;
		}
	}

	free(r);

	if (result == (pid_t)-1) result = 0;

	return result;
}

Client *
wintoclient(Window w)
{
	Client *c;
	Monitor *m;

	for (m = mons; m; m = m->next)
		for (c = m->clients; c; c = c->next)
			if (c->win == w)
				return c;
	return NULL;
}

Monitor *
wintomon(Window w)
{
	int x, y;
	Client *c;
	Monitor *m;

	if (w == root && getrootptr(&x, &y))
		return recttomon(x, y, 1, 1);
	for (m = mons; m; m = m->next)
		if (w == m->barwin)
			return m;
	if ((c = wintoclient(w)))
		return c->mon;
	return selmon;
}

/* There's no way to check accesses to destroyed windows, thus those cases are
 * ignored (especially on UnmapNotify's). Other types of errors call Xlibs
 * default error handler, which may call exit. */
int
xerror(Display *dpy, XErrorEvent *e)
{
	if (e->error_code == BadWindow
	|| (e->request_code == X_SetInputFocus && e->error_code == BadMatch)
	|| (e->request_code == X_PolyText8 && e->error_code == BadDrawable)
	|| (e->request_code == X_PolyFillRectangle
                && e->error_code == BadDrawable)
	|| (e->request_code == X_PolySegment && e->error_code == BadDrawable)
	|| (e->request_code == X_ConfigureWindow && e->error_code == BadMatch)
	|| (e->request_code == X_GrabButton && e->error_code == BadAccess)
	|| (e->request_code == X_GrabKey && e->error_code == BadAccess)
	|| (e->request_code == X_CopyArea && e->error_code == BadDrawable))
		return 0;
	fprintf(stderr, "dwm: fatal error: request code=%d, error code=%d\n",
		e->request_code, e->error_code);
	return xerrorxlib(dpy, e); /* may call exit */
}

int
xerrordummy(Display *dpy, XErrorEvent *ee)
{
	return 0;
}

/* Startup Error handler to check if another window manager
 * is already running. */
int
xerrorstart(Display *dpy, XErrorEvent *ee)
{
	die("dwm: another window manager is already running");
	return -1;
}

void
zoom(const Arg *arg)
{
	Client *c = selmon->sel;
        Client *at = NULL, *cold, *cprevious = NULL, *previous;
        unsigned int curtag;
	if (!selmon->lt[selmon->sellt]->arrange
	|| (selmon->sel && selmon->sel->isfloating)
        || !c)
		return;

	if (c == nexttiled(selmon->clients)) {
                curtag = c->mon->pertag->curtag;
                previous = c->mon->pertag->prevzooms[curtag];
		at = findbefore(previous);
		if (at)
			cprevious = nexttiled(at->next);
		if (!cprevious || cprevious != previous) {
                        c->mon->pertag->prevzooms[curtag] = NULL;
			if (!c || !(c = nexttiled(c->next)))
				return;
		} else
			c = cprevious;
	}
	cold = nexttiled(selmon->clients);
	if (c != cold && !at)
		at = findbefore(c);
	detach(c);
	attach(c);
	/* swap windows instead of pushing the previous one down */
	if (c != cold && at) {
                curtag = c->mon->pertag->curtag;
		c->mon->pertag->prevzooms[curtag] = cold;
		if (cold && at != cold) {
			detach(cold);
			cold->next = at->next;
			at->next = cold;
		}
	}
	focus(c);
	arrange(c->mon);
}

int
main(int argc, char *argv[])
{
	if (argc == 2 && !strcmp("-v", argv[1]))
		die("dwm-"VERSION);
	else if (argc != 1)
		die("usage: dwm [-v]");
	if (!setlocale(LC_CTYPE, "") || !XSupportsLocale())
		fputs("warning: no locale support\n", stderr);
	if (!(dpy = XOpenDisplay(NULL)))
		die("dwm: cannot open display");
	if (!(xcon = XGetXCBConnection(dpy)))
		die("dwm: cannot get xcb connection\n");
	checkotherwm();
	setup();
#ifdef __OpenBSD__
	if (pledge("stdio rpath proc exec", NULL) == -1)
		die("pledge");
#endif /* __OpenBSD__ */
	scan();
	run();
	cleanup();
	XCloseDisplay(dpy);
	return EXIT_SUCCESS;
}
