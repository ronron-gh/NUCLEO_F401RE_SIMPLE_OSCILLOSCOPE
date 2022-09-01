# -*- coding: utf-8 -*-

from PIL import Image

HIGHT = 240
WIDTH = 320

R_MAX = 31
G_MAX = 63
B_MAX = 31

R_BIT_OFFSET = 11
G_BIT_OFFSET = 5

color_format_str = ''

im = Image.open("startup_screen.bmp")
f = open('output.txt', 'w')

#RGBに変換
rgb_im = im.convert('RGB')

if rgb_im.size[0] != WIDTH or rgb_im.size[1] != HIGHT:
    print('Error: 画像データのサイズが320x240ではありません')
    print('画像サイズ:' + str(rgb_im.size[0]) + 'x' + str(rgb_im.size[1]) )


for y in range(HIGHT):
   
    for x in range(WIDTH):

        #ピクセルを取得
        r,g,b = rgb_im.getpixel((x,y))
        
        r = int(R_MAX * r/255)
        g = int(G_MAX * g/255)
        b = int(B_MAX * b/255)
        
        color_format = (r << R_BIT_OFFSET) + (g << G_BIT_OFFSET) + b
        
        color_format_str = color_format_str + hex(color_format) + ','
        
        if (x + 1) % 16 == 0:
            color_format_str = color_format_str + '\n'
        
        
#print(color_format_str)
print('変換完了')
f.write(color_format_str)
        

        