import turtle as t

# --- READ POINTS FROM FILE ---
points = []
with open("points.txt", "r") as f:
    for line in f:
        line = line.strip()
        if not line:
            continue
        x_str, y_str = line.split(",")
        points.append((float(x_str), float(y_str)))

print(f"Loaded {len(points)} points.")


# --- TURTLE SETUP ---
t.colormode(1.0)
t.bgcolor("black")
t.pencolor(0, 1, 0)  # green line
t.speed(0)
t.hideturtle()
t.tracer(1, 1)
t.width(2)

# --- DRAW THE PATH ---
first = True
for (x, y) in points:
    if first:
        t.penup()
        t.goto(x, y)
        t.pendown()
        first = False
    else:
        t.goto(x, y)

t.done()
