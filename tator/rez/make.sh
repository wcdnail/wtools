hexdump -v -e '16/1 "_x%02X" "\n"' picker_icon.ico | sed 's/_/\\/g; s/\\x  //g; s/.*/    "&"/' > picker_icon.ico.txt