import cv2
import numpy as np

# --- IMAGE PROCESSING ---
img = cv2.imread("image.jpg")
if img is None:
    raise FileNotFoundError("Couldn't find 'image.jpeg' in the current folder.")

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
contours = [c for c in contours if len(c) > 20]  # filter out noise

# Simplify contours for smoother lines
smooth_contours = []
for c in contours:
    epsilon = 0.002 * cv2.arcLength(c, True)
    approx = cv2.approxPolyDP(c, epsilon, True)
    smooth_contours.append(approx)

# --- SAVE POINTS INSTEAD OF DRAWING ---
points = []

scale = 0.5
offset_x, offset_y = -250, 250

for contour in smooth_contours:
    for point in contour:
        x, y = point[0]
        tx = x * scale + offset_x
        ty = -y * scale + offset_y
        points.append((tx, ty))

# Save to TXT
with open("points.txt", "w") as f:
    for (x, y) in points:
        f.write(f"{x},{y}\n")

print(f"Saved {len(points)} points to points.txt")
