# mode7-sprites: start here

MODE 7 (Teletext graphics) sprite converter (Windows C++ tool) and 6502 plot routines for the BBC Micro. beebasm. 2016-2017 experiment.

| | |
|---|---|
| GitHub | https://github.com/kieranhj/mode7-sprites (public) |
| Local path | D:\beeb\mine\mode7-sprites (moved from OneDrive\BEEB\Repos\mode7-sprites on 2026-11-22) |
| Branch | investigation-sparse-circles (checked out); others: master, half-column-data, linear-data, refactored-clipping, stripe-data, wip/2026-11-tidy |
| Last commit | 2016-11-28: Investigation into sparse sprites + circles (master: 2017-04-25, Dense logo data) |

## What's in here
- `m7plot.6502`: the plot test; builds `m7plot.ssd` (committed), boots `M7Plot`.
- `*.6502` data (`ball*.6502`, `font*.6502`, `star*.6502`) and `*.asm` (`circles`, `circ4`, `dots`): converted sprites; source PNGs beside them.
- `mode7-sprites/`: the converter source (Visual Studio C++), using `jpeg-6b/` and `lpng1624/`. `mode7-sprites.exe` (+ .ilk/.pdb) is the built tool.
- `notes.txt`, `mode7-sprite-notes.txt` (untracked): design notes.

## Build and run
- `make`: runs `make.bat`, which assembles `m7plot.6502` with `..\..\Bin\beebasm.exe` into `m7plot.ssd`. `make help` lists targets.
- Output `m7plot.ssd` is tracked, so a rebuild shows it as modified.
- `run.bat` points at `..\..\Tools\B-emV2.2Win\B-em.exe`, which no longer exists on D: (other repos use `..\..\bin\B-em\B-em.exe`).
- The converter is a Visual Studio project; not rebuilt here.

## Local changes
- Untracked: `compile.txt` (a listing) and `mode7-sprite-notes.txt`; both also on `wip/2026-11-tidy` (pushed 2026-11-21).
- Local `master` (2017-04-25, Dense logo data) was ahead of origin/master; pushed as `wip/local-master-2017`.

## Status and next steps
- Checked-out branch is an investigation of sparse sprites and circles; no plan beyond `notes.txt`.
- Decide which branch is the real head (local master vs investigation-sparse-circles) and whether to push master properly.

## See also
- D:\beeb\mine\mode7-video (uses the same jpeg/png libraries), mode7-75, mode7-smooth.
