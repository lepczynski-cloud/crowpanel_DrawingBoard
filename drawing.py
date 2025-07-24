import serial
import pygame
import sys
import re
import time

# Settings
PORT = 'COM5'  # <- set your correct COM port
BAUDRATE = 115200
WIDTH, HEIGHT = 800, 480

# Pygame initialization
pygame.init()
screen = pygame.display.set_mode((WIDTH, HEIGHT))
pygame.display.set_caption("CrowPanel Drawing")
screen.fill((0, 0, 0))

# Serial port initialization
try:
    ser = serial.Serial(PORT, BAUDRATE, timeout=1)
except:
    print(f"Cannot open serial port {PORT}")
    sys.exit(1)

last_pos = None
last_draw_time = time.time()  # Time of last DRAW command

# Convert RGB565 to RGB888
def rgb565_to_rgb888(color565):
    r = ((color565 >> 11) & 0x1F) << 3
    g = ((color565 >> 5) & 0x3F) << 2
    b = (color565 & 0x1F) << 3
    return (r, g, b)

# Parse incoming serial line
def parse_line(line):
    # Expected format: DRAW x y color565
    match = re.match(r"DRAW (\d+) (\d+) (\d+)", line)
    if match:
        x = int(match.group(1))
        y = int(match.group(2))
        color565 = int(match.group(3))
        rgb = rgb565_to_rgb888(color565)
        return x, y, rgb
    return None

# Main loop
running = True
while running:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False

    # Auto-reset if no touch input received for 150ms
    if time.time() - last_draw_time > 0.15:
        last_pos = None

    try:
        if ser.in_waiting:
            line = ser.readline().decode(errors='ignore').strip()
            if line.startswith("DRAW"):
                result = parse_line(line)
                if result:
                    x, y, color = result
                    if last_pos is not None:
                        pygame.draw.line(screen, color, last_pos, (x, y), 2)
                    last_pos = (x, y)
                    last_draw_time = time.time()

            elif line.startswith("COLOR"):
                last_pos = None  # Reset on color change

    except Exception as e:
        print("Error:", e)

    pygame.display.update()

ser.close()
pygame.quit()
