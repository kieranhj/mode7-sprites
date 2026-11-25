# Wrapper so that `make` works from PowerShell / Windows Terminal.
# GNU make runs recipes with cmd.exe here (no sh on PATH).
# make.bat assembles with ..\..\Bin\beebasm.exe.
.PHONY: all build help
all: build

build:
	cmd /c .\make.bat

help:
	@echo make        - build m7plot.ssd (from m7plot.6502) (runs make.bat)
	@echo make help   - this list
	@echo to run      - ..\..\bin\b-em\b-em.exe m7plot.ssd (not a make target)
