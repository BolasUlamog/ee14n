import numpy as np
import math

def calculate_distance(point1, point2):
    return math.sqrt((point2[0] - point1[0])**2 + (point2[1] - point1[1])**2)

def main():
    # --- 1. Load Points and Calculate Laser States ---
    points = []
    curr_line = 0
    try:
        with open("points.txt", "r") as f:
            for line in f:
                parts = line.strip().split(',')
                if len(parts) == 2:
                    points.append((float(parts[0]), float(parts[1])))
                curr_line += 1
    except FileNotFoundError:
        print("Error: points.txt not found.")
        return
    
    if not points:
        print("No points found in file.")
        return
    
    print(f"Read {len(points)} points")
    
    # Calculate laser states based on distance
    laser_states = []
    for i in range(len(points) - 1):
        distance = calculate_distance(points[i], points[i + 1])
        laser_on = distance <= 5
        laser_states.append(laser_on)
        print(f"Distance from point {i} to {i+1}: {distance:.2f} - Laser: {laser_on}")
    
    # Write output with laser state before each coordinate
    with open('points_with_laser.txt', 'w') as f:
        for i in range(len(points) - 1):
            f.write(f"{laser_states[i]},{points[i][0]},{points[i][1]}\n")
        # Last point gets True
        f.write(f"True,{points[-1][0]},{points[-1][1]}\n")
    
    print(f"\nOutput written to points_with_laser.txt")
    
    # --- 2. Convert to numpy array for angle calculations ---
    pts = np.array(points)
    xs = pts[:, 0]
    ys = pts[:, 1]

    # --- 3. Determine Geometry ---
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

    # --- 4. Set Projector Distance ---
    # We want the max reach to correspond to an angle of 40 degrees 
    # (so 45 +/- 40 = range 5 to 85). This keeps us safely within 0-90.
    target_fov_half = 40  # degrees
    D = max_reach / np.tan(np.radians(target_fov_half))

    # --- 5. Calculate Angles (Thetas) ---
    # Formula: angle = degrees(atan(offset / distance)) + 45
    theta_x_array = np.degrees(np.arctan((xs - center_x) / D)) + 45
    theta_y_array = np.degrees(np.arctan((ys - center_y) / D)) + 45

    # Convert back to standard python lists
    theta_x = theta_x_array.tolist()
    theta_y = theta_y_array.tolist()

    # --- 6. Output Stats ---
    print(f"\nProjector Distance (calculated): {D:.2f} units")
    print(f"Theta X range: {min(theta_x):.2f}° to {max(theta_x):.2f}°")
    print(f"Theta Y range: {min(theta_y):.2f}° to {max(theta_y):.2f}°")

    # --- 7. Write theta points with laser states ---
    with open("theta_points.txt", "w") as f:
        for i in range(len(points) - 1):
            f.write(f"{laser_states[i]},{theta_x[i]:.4f},{theta_y[i]:.4f}\n")
        # Last point gets True
        f.write(f"True,{theta_x[-1]:.4f},{theta_y[-1]:.4f}\n")
    
    print(f"Theta points with laser states written to theta_points.txt")

if __name__ == "__main__":
    main()
