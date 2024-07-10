/* See LICENSE file for copyright and license details. */
#include <X11/XF86keysym.h>
#include "exitdwm.c"

/* appearance */
static const unsigned int borderpx  = 3;        /* border pixel of windows */
static const unsigned int gappx     = 10;        /* gaps between windows */
static const unsigned int snap      = 32;       /* snap pixel */
static const int swallowfloating    = 0;        /* 1 means swallow floating windows by default */
static const int showbar            = 1;        /* 0 means no bar */
static const int topbar             = 1;        /* 0 means bottom bar */
static const char *fonts[]          = { "monospace:size=11", "fontawesome:size=10" };
static const char dmenufont[]       = "monospace:size=11";
/* Colors _0 means lighter version */
static const char col_bg[]          = "#1d2021";
static const char col_bg_0[]        = "#282828";
static const char col_orange[]      = "#d65d0e";
static const char col_orange_0[]    = "#fe8019";
static const char col_green[]       = "#98971a";
static const char col_green_0[]     = "#b8bb26";
static const char col_yellow[]      = "#d79921";
static const char col_yellow_0[]    = "#fabd2f";
static const char col_blue[]        = "#458588";
static const char col_blue_0[]      = "#83a598";
static const char col_purple[]      = "#b16286";
static const char col_purple_0[]    = "#d3869b";
static const char col_aqua[]        = "#689d6a";
static const char col_aqua_0[]      = "#8ec07c";
static const char col_fg[]          = "#ebdbb2";
static const char col_fg_0[]        = "#fbf1c7";
static const char col_grey[]        = "#928374";
static const char col_grey_0[]      = "#a89984";

static const char *colors[][3]      = {
  /* Statusbar right { fg          bg          border/leave as-is} */
  [SchemeNorm]     = { col_fg,     col_bg,     col_grey     },
  [SchemeSel]      = { col_grey,   col_blue,   col_orange   }, /* Menu scheme */
  [SchemeStatus]   = { "#ffffff",  col_blue,   "#000000"    }, /* Status Bar */
  [SchemeTagsSel]  = { col_bg_0,   col_aqua_0, "#000000"    }, /* Tagbar left selected */
  [SchemeTagsNorm] = { col_fg,     col_bg,     "#000000"    }, /* Tagbar left unselected */
  [SchemeInfoSel]  = { col_bg_0,   col_aqua_0, "#000000"    }, /* infobar middle selected */
  [SchemeInfoNorm] = { col_fg,     col_bg_0,   "#000000"    }, /* infobar middle unselected */
};

static const char *tagsel[][2] = {
  { "#ffffff", "#ff0000" },
  { "#ffffff", "#ff7f00" },
  { "#000000", "#ffff00" },
  { "#000000", "#00ff00" },
  { "#ffffff", "#0000ff" },
  { "#ffffff", "#4b0082" },
  { "#ffffff", "#9400d3" },
  { "#000000", "#ffffff" },
  { "#ffffff", "#000000" },
};

static const char *const autostart[] = {
	"sh", "-c", "while xset q &>/dev/null; do xsetroot -name \"$(pgrep -x mpd >/dev/null && mpc current) | Disk: $(df -h / | awk 'NR==2 {print $4}') | Bat: $(cat /sys/class/power_supply/BAT0/capacity)% | Vol: $(amixer sget Master | awk -F'[][]' '/Left:/ { if ($4 == \"off\") print \"Muted\"; else print $2 }') | $(date '+%I:%M%p')\"; sleep 1; done", NULL,
	"xwallpaper", "--zoom", "/home/malcolm/.config/wallpapers/ds9.png", NULL,
	"mpd", NULL,
	"fastcompmgr", "-o", "0.4", "-r", "12", "-c", "-C", NULL,
	NULL /* terminate */
};

/* tagging */
static const char *tags[] = { "", "", "", "", "", "", "", "", "" };
static const char *alttags[] = { "", "", "", "", "", "", "", "", "" };

static const Rule rules[] = {
  /* xprop(1):
   *  WM_CLASS(STRING) = instance, class
   *  WM_NAME(STRING) = title
   */
    /* To Add swallow change the "isterminal" line to 1 to enable it */
  /* class          instance  title           tags mask  isfloating  isterminal  noswallow  monitor */
/* Add the swallow rule for st terminal */
{ "st",              NULL,       NULL,          0,         0,         1,           0,        -1 },

/* Existing rules */
{ "Firefox",        NULL,       NULL,           1 << 8,    0,          0,          -1,        -1 },
{ "Gimp",           NULL,       NULL,           0,         1,          0,           0,        -1 },
{ "Gcr-prompter",   NULL,       NULL,           0,         1,          1,           0,         1 }, /* GPG password prompt */
{ NULL,             NULL,       "Event Tester", 0,         0,          0,           1,        -1 }, /* xev */
};

/* layout(s) */
static const float mfact        = 0.50; /* factor of master area size [0.05..0.95] */
static const int nmaster        = 1;    /* number of clients in master area */
static const int resizehints    = 0;    /* 1 means respect size hints in tiled resizals */
static const int lockfullscreen = 1; /* 1 will force focus on the fullscreen window */

static const Layout layouts[] = {
  /* symbol     arrange function */
  { "[]=",      tile },
};

/* key definitions */
#define MODKEY Mod4Mask
#define TAGKEYS(KEY,TAG) \
  { MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
  { MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
  { MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
  { MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[] = { "dmenu_run", "-m", dmenumon, "-fn", dmenufont, "-nb", col_bg, "-nf", col_fg, "-sb", col_orange, "-sf", col_fg_0, NULL };
static const char *termcmd[]  = { "st", NULL };
static const char *webcmd[]  = { "chromium", NULL };
static const char *webcmdtor[]  = { "sh", "-c", "$HOME/Downloads/tor-browser/Browser/start-tor-browser", NULL };

/* Media and brightness binds */
/* Pulseaudio */
static const char *volup[]   = { "pactl", "set-sink-volume", "0", "+5%", NULL };
static const char *voldown[] = { "pactl", "set-sink-volume", "0", "-5%", NULL };
static const char *volmute[] = { "pactl", "set-sink-mute", "0", "toggle", NULL };
static const char *micmute[] = { "pactl", "set-source-mute", "0", "toggle", NULL };

/* Brightnessctl */
static const char *brup[]    = { "brightnessctl", "set", "5%+", NULL };
static const char *brdown[]  = { "brightnessctl", "set", "5%-", NULL };

static const Key keys[] = {
	/* modifier                     key        function        argument */
	{ MODKEY,                       XK_d,      spawn,          {.v = dmenucmd } },
	{ MODKEY,                       XK_Return, spawn,          {.v = termcmd } },
	{ MODKEY,             			XK_w,      spawn,          {.v = webcmd } },
	{ MODKEY|ShiftMask,             XK_w,      spawn,          {.v = webcmdtor } },

	/* Media and brightness control keys */
	{ 0,        XF86XK_AudioRaiseVolume,       spawn,          {.v = volup} },
	{ 0,        XF86XK_AudioLowerVolume,       spawn,          {.v = voldown} },
	{ 0,        XF86XK_AudioMute,              spawn,          {.v = volmute} },
	{ 0,        XF86XK_AudioMicMute,           spawn,          {.v = micmute}  },
	{ 0,        XF86XK_MonBrightnessUp,        spawn,          {.v = brup} },
	{ 0,        XF86XK_MonBrightnessDown,      spawn,          {.v = brdown} },
  
  	/* Screenshot using xsnip and xclip */
	{ MODKEY|ShiftMask, 		    XK_o,      spawn, SHCMD("xsnip -o") },

	/* Bar */
	{ MODKEY,                       XK_b,      togglebar,      {0} },

	/* Htop */
	{ MODKEY|ShiftMask,             XK_r, spawn, {.v = (const char*[]){"st", "-e", "htop", NULL}} },

	/* Resize Windows Horizontally */
	{ MODKEY,                       XK_h,      setmfact,       {.f = -0.05} },
	{ MODKEY,                       XK_l,      setmfact,       {.f = +0.05} },

	/* Select Opened Windows  */
	{ MODKEY,                       XK_j,      focusstack,     {.i = +1 } },
	{ MODKEY,                       XK_k,      focusstack,     {.i = -1 } },
	
	{ MODKEY,                       XK_Return, zoom,           {0} },
	{ MODKEY,                       XK_Tab,    view,           {0} },
  
	/* Layout */
	{ MODKEY,                       XK_space,  setlayout,      {0} },
	{ MODKEY|ShiftMask,             XK_space,  togglefloating, {0} },

	/* Fullscreen */
	{ MODKEY,                       XK_f,      togglefullscr,  {0} },
	
	{ MODKEY,                       XK_0,      view,           {.ui = ~0 } },
	{ MODKEY|ShiftMask,             XK_0,      tag,            {.ui = ~0 } },
	{ MODKEY,                       XK_comma,  focusmon,       {.i = -1 } },
	{ MODKEY,                       XK_period, focusmon,       {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_comma,  tagmon,         {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_period, tagmon,         {.i = +1 } },
  
	/* Gaps */
	{ MODKEY,                       XK_minus,  setgaps,        {.i = -1 } },
	{ MODKEY,                       XK_equal,  setgaps,        {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_equal,  setgaps,        {.i = 0  } },

	/* ShiftView */
	{ MODKEY|ShiftMask,                       XK_m,      shiftviewclients, { .i = +1 } },
	{ MODKEY|ShiftMask,                       XK_n,      shiftviewclients, { .i = -1 } },

	/* Newsboat */
	{ MODKEY,			  XK_n,      spawn, {.v = (const char*[]){"st", "-e", "newsboat", NULL}} },

	/* Ncmpcpp*/
	{ MODKEY,			  XK_m,	     spawn, {.v = (const char*[]){"st", "-e", "ncmpcpp", NULL}} },

	/* Virtual Tags */
	TAGKEYS(                        XK_1,                      0)
	TAGKEYS(                        XK_2,                      1)
	TAGKEYS(                        XK_3,                      2)
	TAGKEYS(                        XK_4,                      3)
	TAGKEYS(                        XK_5,                      4)
	TAGKEYS(                        XK_6,                      5)
	TAGKEYS(                        XK_7,                      6)
	TAGKEYS(                        XK_8,                      7)
	TAGKEYS(                        XK_9,                      8)

	/* ExitMenu/Quit DWM */
	{ MODKEY|ShiftMask,             XK_e,      exitdwm,        {0} },
	{ MODKEY|ControlMask|ShiftMask, XK_q,      quit,           {1} }, 
	
	/* Close Active Window */
	{ MODKEY,                       XK_q,      killclient,     {0} },
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static const Button buttons[] = {
  /* click                event mask      button          function        argument */
  { ClkLtSymbol,          0,              Button1,        setlayout,      {0} },
  { ClkLtSymbol,          0,              Button3,        setlayout,      {.v = &layouts[2]} },
  { ClkWinTitle,          0,              Button2,        zoom,           {0} },
  { ClkStatusText,        0,              Button2,        spawn,          {.v = termcmd } },
  { ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
  { ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },
  { ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },
  { ClkTagBar,            0,              Button1,        view,           {0} },
  { ClkTagBar,            0,              Button3,        toggleview,     {0} },
  { ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
  { ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
};
