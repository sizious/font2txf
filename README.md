http://celestia.teyssier.org/ttf2txf/index.html

Celestia Font Texture Generator
TTF font file:    
(Optional) codepoints file:    
Font size:    
Texture size:    
    
Guide

The default codepoints file used is the WGL4 file used to generate the font textures distributed with Celestia.

Save the file produced by the generator to the fonts directory of your celestia installation and set the Font, LabelFont or TitleFont property of your celestia.cfg (or ~/.celestia.cfg on Unix).
Recommanded Texture Size
Font Size    Texture Size    Texture Size (bold font)
<= 12    256x256    256x256
14-16    512x256    512x256
20    512x256    512x512
24    512x512    512x512
30    1024x512    1024x1024
Preview
TXF font file:    
    
    
[DIR] Parent Directory                               -   
[   ] Makefile                  04-Feb-2004 02:01  2.7K  
[   ] mkwgl4.sh                 04-Feb-2004 02:01  1.0K  
[TXT] texfont.c                 04-Feb-2004 02:01   17K  
[TXT] texfont.h                 04-Feb-2004 02:01  1.9K  
[TXT] ttf2txf.cpp               04-Feb-2004 02:01   18K  
[TXT] wgl4.txt                  04-Feb-2004 02:01  3.8K  


What things you need to install the software and how to install them Font Generation: ``` .\ttf2txf -o revenant.txf -s 22 -w 256 -h 256 -c "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ!@#$%^&*()-_=+[]{}|;:,.<>~ ?/" revenant.TTF