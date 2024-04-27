@echo off

:: UTF-8 ...
chcp 65001 > nul 2> nul

set pwd=%~dp0
set pdir=%pwd%utf-8

mkdir %pdir% > nul 2> nul

for %%H in (*.*) do (
    iconv -f CP1251 -t UTF-8 %%H > %pdir%\%%H
)
    
