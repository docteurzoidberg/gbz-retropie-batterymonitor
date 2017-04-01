import pygame
from pygame.locals import *
import fborx

# this will open a 800x800 window if xserver is running or fullscreen fb if not
screen = fborx.getScreen(320,240)

# this will draw a red rect
pygame.draw.rect(screen, (255,0,0), Rect(100,100,100,100))

# this will draw some text with custom font and color
# (I hope the font works for you, windows users may have to change to arial)
myfont = pygame.font.SysFont("monospace", 50)
label = myfont.render("The screen size is " + str(fborx.size[0]) +"x" + str(fborx.size[1]), 1, (0,255,0))
screen.blit(label, (10, 300))   

# now update the display to show the new graphics
pygame.display.flip()

# this code just waits for the ESC key (isn't beauty with the loop, but works for now)
running = True
while running:
    for event in pygame.event.get():
        if event.type == pygame.KEYDOWN:
            if event.key == K_ESCAPE:
                running = False

# make sure to call pygame.quit() if using the framebuffer to get back to your terminal
pygame.quit()