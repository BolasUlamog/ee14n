import numpy as np

# --- 1. Load Points ---
points = []
try:
    with open("points.txt", "r") as f:
        for line in f:
            parts = line.strip().split(',')
            if len(parts) == 2:
                points.append((float(parts[0]), float(parts[1])))
except FileNotFoundError:
    print("Error: points.txt not found. Please run the previous step first.")
    points = []

if points:
    # Convert to numpy array for fast math
    pts = np.array(points)
    xs = pts[:, 0]
    ys = pts[:, 1]

    # --- 2. Determine Geometry ---
    # Find the bounding box of the current data
    min_x, max_x = np.min(xs), np.max(xs)
    min_y, max_y = np.min(ys), np.max(ys)

    # Find the center of the bounding box
    center_x = (min_x + max_x) / 2
    center_y = (min_y + max_y) / 2

    # Calculate the maximum reach from the center
    half_width = (max_x - min_x) / 2
    half_height = (max_y - min_y) / 2
    max_reach = max(half_width, half_height)

    # --- 3. Set Projector Distance ---
    # We want the max reach to correspond to an angle of 40 degrees 
    # (so 45 +/- 40 = range 5 to 85). This keeps us safely within 0-90.
    target_fov_half = 40  # degrees
    D = max_reach / np.tan(np.radians(target_fov_half))

    # --- 4. Calculate Angles (Thetas) ---
    # Formula: angle = degrees(atan(offset / distance)) + 45
    theta_x_array = np.degrees(np.arctan((xs - center_x) / D)) + 45
    theta_y_array = np.degrees(np.arctan((ys - center_y) / D)) + 45

    # Convert back to standard python lists
    theta_x = theta_x_array.tolist()
    theta_y = theta_y_array.tolist()

    # --- Output Stats ---
    print(f"Projector Distance (calculated): {D:.2f} units")
    print(f"Theta X range: {min(theta_x):.2f}° to {max(theta_x):.2f}°")
    print(f"Theta Y range: {min(theta_y):.2f}° to {max(theta_y):.2f}°")

    with open("theta_points.txt", "w") as f:
        for tx, ty in zip(theta_x, theta_y):
            f.write(f"{tx:.4f},{ty:.4f}\n")
