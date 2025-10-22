import cv2
import turtle as t
import numpy as np

# --- IMAGE PROCESSING ---
img = cv2.imread("image.jpg")
if img is None:
    raise FileNotFoundError("Couldn't find 'image.jpg' in the current folder.")

# Resize to a manageable size
img = cv2.resize(img, (600, 600))

# Convert to grayscale
gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)

# Smooth and denoise to improve edges
gray = cv2.GaussianBlur(gray, (5, 5), 0)
gray = cv2.bilateralFilter(gray, 9, 75, 75)

# Detect edges
edges = cv2.Canny(gray, 50, 150)
cv2.imwrite("edges.png", edges)

# Find contours (smooth outlines)
contours, _ = cv2.findContours(edges, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_NONE)
contours = [c for c in contours if len(c) > 50]  # filter out noise

# Simplify contours for smoother lines
smooth_contours = []
for c in contours:
    epsilon = 0.002 * cv2.arcLength(c, True)
    approx = cv2.approxPolyDP(c, epsilon, True)
    smooth_contours.append(approx)

# --- TURTLE SETUP ---
t.colormode(1.0)
t.bgcolor("black")
t.speed(0)
t.hideturtle()
t.tracer(0, 0)  # disable animation for speed
t.width(2)

# --- RAINBOW COLOR FUNCTION ---
def rainbow_color(step, total_steps, cycles=3):
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
total_points = sum(len(c) for c in smooth_contours)
counter = 0

scale = 0.5
offset_x, offset_y = -250, 250

for contour in smooth_contours:
    t.penup()
    first = True
    for point in contour:
        x, y = point[0]
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

t.update()  # draw all at once
t.done()
