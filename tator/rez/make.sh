cd /c/_/wtools/tator/rez
hexdump -v -e '16/1 "_x%02X" "\n"' picker_icon_clr.ico | sed 's/_/\\/g; s/\\x  //g; s/.*/    "&"/' > picker_icon.ico.txt