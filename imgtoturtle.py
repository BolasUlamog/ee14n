import cv2
import turtle as t
import math

# --- IMAGE PROCESSING ---
img = cv2.imread("image.jpg")
gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
edges = cv2.Canny(gray, 100, 200)
cv2.imwrite("edges.png", edges)

contours, _ = cv2.findContours(edges, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_NONE)

# --- TURTLE SETUP ---
t.colormode(1.0)
t.bgcolor("black")
t.speed(0)
t.hideturtle()
t.tracer(0, 0)  # draw faster (no live animation updates)
t.width(2)

# --- COLOR FUNCTION (RAINBOW) ---
def rainbow_color(step, total_steps, cycles=2):
    hue = (step / total_steps) * cycles * 6
    c = max(0, 1 - abs(hue % 2 - 1))
    if hue < 1: r, g, b = 1, c, 0
    elif hue < 2: r, g, b = c, 1, 0
    elif hue < 3: r, g, b = 0, 1, c
    elif hue < 4: r, g, b = 0, c, 1
    elif hue < 5: r, g, b = c, 0, 1
    else: r, g, b = 1, 0, c
    return (r, g, b)

# --- DRAW ---
total_points = sum(len(c) for c in contours)
counter = 0

scale = 0.5  # adjust to fit image size
offset_x, offset_y = -250, 250  # recenter drawing

for contour in contours:
    t.penup()
    first = True
    for point in contour:
        x, y = point[0]
        # convert coordinates
        tx = x * scale + offset_x
        ty = -y * scale + offset_y

        color = rainbow_color(counter, total_points)
        t.pencolor(color)

        if first:
            t.goto(tx, ty)
            t.pendown()
            first = False
        else:
            t.goto(tx, ty)

        counter += 1

t.update()  # render all at once for speed
t.done()
