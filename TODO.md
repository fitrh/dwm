# DWM TODO

---

## PATCH INTEGRATION

- [ ] [tagothermonitor](https://dwm.suckless.org/patches/tagothermonitor/)
- [ ] [tagswapmon](https://github.com/bakkeby/patches/wiki/tagswapmon)
- [ ] [tagallmon](https://github.com/bakkeby/patches/wiki/tagallmon)
- [ ] [swaptags](https://dwm.suckless.org/patches/swaptags)
- [ ] [warp](https://dwm.suckless.org/patches/warp)
- [ ] [cursorwarp](https://dwm.suckless.org/patches/cursorwarp)
- [x] [2e8d3d6][1] - [Bar Border](https://codemadness.org/paste/dwm-border-bar.patch)
- [ ] [focusedontop](https://github.com/bakkeby/patches/commit/cf9ea5e)

---

## FIX/ADD BEHAVIOUR

- [ ] bar line indicator multiple tag, toggletag, tagall client
- [ ] configurable layout for multi-tag view
- [ ] glitch when open client that have specific tag
- [x] centering client in floating layout
- [ ] remove loop for colorful patch and replace with some get function
- [x] [09c2703][4] - rule for LibreOffice
- [x] remove horizgrid layout
- [ ] option for bar line indicator
- [x] state of multi tag view when bar line disabled
- [x] center using floatpos, possibility for removing alwayscenter patch
  - ref: [bakkeby dusk](https://github.com/bakkeby/dusk/commit/cd0eeb8)
- [x] toggle bar gap on toggle gap
- [x] [c797410][2] [aa0b056][3] - configurable bar border and create separe color scheme for it

---

## BLINK

- [ ] configurable bar per monitor or all monitor

---

## SEND PATCH

- [ ] relative centered title
- [ ] dwmblocks integration with hidevacanttags and barpadding

---

## CODE READABILITY

- [ ] split drawbar function
- [x] use macro for rule

---

## COMPILER WARN

- [ ] setdesktopnames

[1]: <https://github.com/fitrh/dwm/commit/2e8d3d61e2758f63bf4c11cb6df07049543b19e6> "2e8d3d6"
[2]: <https://github.com/fitrh/dwm/commit/c797410334f26c90b544ec74b98425844838f16f> "c797410"
[3]: <https://github.com/fitrh/dwm/commit/aa0b0563aa459f830fc8ade1bef9b9ad582b0621> "aa0b056"
[4]: <https://github.com/fitrh/dwm/commit/09c270314aad2182cf597bc27261696c5a4d1fc9> "09c2703"
