# -*- mode: python ; coding: utf-8 -*-


a = Analysis(
    ['gui/main.py'],
    pathex=[],
    binaries=[],
    datas=[('/home/henri/Pulsebench/PulseBench/.venv/lib/python3.13/site-packages/PySide6/Qt/plugins/platforms', 'PySide6/Qt/plugins/platforms'), ('/home/henri/Pulsebench/PulseBench/.venv/lib/python3.13/site-packages/PySide6/Qt/plugins/imageformats', 'PySide6/Qt/plugins/imageformats')],
    hiddenimports=[],
    hookspath=[],
    hooksconfig={},
    runtime_hooks=[],
    excludes=[],
    noarchive=False,
    optimize=0,
)
pyz = PYZ(a.pure)

exe = EXE(
    pyz,
    a.scripts,
    a.binaries,
    a.datas,
    [],
    name='pulsebench-gui',
    debug=False,
    bootloader_ignore_signals=False,
    strip=False,
    upx=True,
    upx_exclude=[],
    runtime_tmpdir=None,
    console=False,
    disable_windowed_traceback=False,
    argv_emulation=False,
    target_arch=None,
    codesign_identity=None,
    entitlements_file=None,
)
