import cv2
import numpy as np
import math
from scipy.interpolate import splprep, splev

# --- CONFIGURATION ---
INPUT_IMAGE = "image.png"

# GLOBAL POINT LIMIT (The "Budget")
# The script will never generate more points than this TOTAL.
# 150-200 is a safe range for Arduino Uno/Nano.
GLOBAL_MAX_POINTS = 30 

# MINIMUM points per shape (to maintain basic geometry like squares)
MIN_POINTS_PER_SHAPE = 5
# ---------------------

def get_contours_and_process(filename):
    img = cv2.imread(filename)
    if img is None:
        raise FileNotFoundError(f"Couldn't find '{filename}'")

    print(f"Processing '{filename}'...")

    # 1. Image Pre-processing
    img = cv2.resize(img, (600, 600))
    gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
    gray = cv2.GaussianBlur(gray, (5, 5), 0)
    gray = cv2.bilateralFilter(gray, 9, 75, 75)
    edges = cv2.Canny(gray, 50, 150)

    # 2. Find Contours
    contours, _ = cv2.findContours(edges, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_NONE)
    
    # Filter out tiny noise (shapes smaller than 20 pixels perimeter)
    valid_contours = [c for c in contours if cv2.arcLength(c, True) > 50]
    
    if not valid_contours:
        print("No valid shapes found.")
        return [], [], []

    # 3. Calculate Point Budget
    # We calculate the total perimeter of ALL shapes to decide how to split the points.
    total_perimeter = sum(cv2.arcLength(c, True) for c in valid_contours)
    print(f"Total Perimeter: {int(total_perimeter)}px across {len(valid_contours)} shapes.")
    print(f"Point Budget: {GLOBAL_MAX_POINTS} max.")

    master_points = [] # List of (x,y)
    master_laser = []  # List of True/False

    scale = 0.5
    offset_x, offset_y = -250, 250

    # 4. Process Each Shape
    for i, c in enumerate(valid_contours):
        perim = cv2.arcLength(c, True)
        
        # ALLOCATION MATH:
        # Give this shape a percentage of the total budget based on its size.
        ratio = perim / total_perimeter
        allocated_points = int(ratio * GLOBAL_MAX_POINTS)
        
        # Clamp: Ensure it has at least MIN_POINTS, but don't go crazy.
        num_points = max(MIN_POINTS_PER_SHAPE, allocated_points)
        
        # --- CUBIC SPLINE SMOOTHING ---
        x_raw = c[:, 0, 0]
        y_raw = c[:, 0, 1]
        
        # We need >4 points for cubic spline. If shape is tiny, use raw points.
        if len(x_raw) > 4:
            try:
                # Spline fit
                tck, u = splprep([x_raw, y_raw], s=100, per=True) 
                # Resample to exactly 'num_points'
                u_new = np.linspace(0, 1, num_points)
                x_smooth, y_smooth = splev(u_new, tck)
            except Exception as e:
                print(f"Spline error on shape {i}: {e}. Using Linear.")
                # Fallback: Simple resampling would go here, but raw is safer for error
                x_smooth, y_smooth = x_raw, y_raw
        else:
            x_smooth, y_smooth = x_raw, y_raw

        # --- GENERATE DATA FOR THIS SHAPE ---
        # We add the points to the master list with Explicit Laser States.
        
        for j in range(len(x_smooth)):
            # Coordinate Transform
            tx = x_smooth[j] * scale + offset_x
            ty = -y_smooth[j] * scale + offset_y
            
            master_points.append((tx, ty))
            
            # LASER LOGIC:
            # First point of the shape = FALSE (Move to start position without drawing)
            # All other points = TRUE (Draw the line connecting to this point)
            if j == 0:
                master_laser.append(False)
            else:
                master_laser.append(True)

    print(f"Final Count: {len(master_points)} points generated.")
    return master_points, master_laser

def convert_to_angles(points):
    if not points: return [], []

    pts = np.array(points)
    xs = pts[:, 0]
    ys = pts[:, 1]

    # Auto-Fit Geometry to 0-90 degrees
    min_x, max_x = np.min(xs), np.max(xs)
    min_y, max_y = np.min(ys), np.max(ys)
    
    center_x = (min_x + max_x) / 2
    center_y = (min_y + max_y) / 2
    
    max_reach = max((max_x - min_x) / 2, (max_y - min_y) / 2)
    
    # 40 degree half-FOV means drawing usually stays between 5° and 85°
    target_fov_half = 40 
    D = max_reach / np.tan(np.radians(target_fov_half))
    if D == 0: D = 1 

    theta_x_array = np.degrees(np.arctan((xs - center_x) / D)) + 45
    theta_y_array = np.degrees(np.arctan((ys - center_y) / D)) + 45

    return theta_x_array.tolist(), theta_y_array.tolist()

def save_to_files(laser_list, x_list, y_list):
    # Rounding
    x_list = [round(x, 2) for x in x_list]
    y_list = [round(y, 2) for y in y_list]

    # 1. Laser File (Boolean to String "true"/"false")
    with open("laser.txt", 'w') as f:
        str_data = ['true' if item else 'false' for item in laser_list]
        f.write(str(str_data).replace("'", '"').replace("[", "{").replace("]", "}"))

    # 2. X Angles
    with open("x.txt", 'w') as f:
        f.write(str(x_list).replace("[", "{").replace("]", "}"))

    # 3. Y Angles
    with open("y.txt", 'w') as f:
        f.write(str(y_list).replace("[", "{").replace("]", "}"))

    print("\nDONE. Files generated: laser.txt, x.txt, y.txt")
    print("Copy the contents inside the { } braces into your Arduino sketch.")

def main():
    try:
        points, laser_bools = get_contours_and_process(INPUT_IMAGE)
        if not points: return
        
        x_angles, y_angles = convert_to_angles(points)
        
        save_to_files(laser_bools, x_angles, y_angles)
    except Exception as e:
        print(f"Error: {e}")

if __name__ == "__main__":
    main()
