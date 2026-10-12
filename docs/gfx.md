# GFX folder

Copy the `GFX` folder of scpcb (https://github.com/Regalis11/scpcb) next to `CMakeLists.txt`.

Only `GFX/map` is used (92 MB):

- `*.rmesh` : the rooms
- `*_lm1.png`, `*_lm2.png`... : lightmaps
- `*.jpg`, `*.png` : textures
- `Props/*.x` and `Props/*.jpg` : props (monitors, desks, lamps...)

The other folders of GFX are not used.

File names are matched without case, and `.bmp` is also searched as `.png` / `.jpg`.

Missing in the official repository: `1074tex1`, `label1123.png`, and `room2.rmesh` is empty.

## Check

```
rmesh_check GFX/map
rmesh_check GFX/map --files
```

It should say `112/113 rooms loaded`.

## Viewer

```
scp_remake GFX/map
```

- list on the left: rooms and .m3d models
- right click: mouse look on / off
- WASD: move, Shift: faster
- Lightmaps: on / off
- Exposure: brightness
