import cv2
import numpy as np
import math
from scipy.interpolate import splprep, splev

# --- CONFIGURATION ---
INPUT_IMAGE = "image.png"

# GLOBAL POINT LIMIT (Budget)
GLOBAL_MAX_POINTS = 100

# --- PHYSICAL CALIBRATION (METERS) ---
# 1. DISTANCE: How far is the laser from the wall?
WALL_DISTANCE_METERS = 1.6  # e.g., 2.0 meters away

# 2. SIZE: How big should the longest side of the drawing be on the wall?
PROJECTED_SIZE_METERS = 4 # e.g., 1.0 meter wide/tall

# 3. ASPECT RATIO CORRECTION
#    If the image looks too squashed vertically, change this.
#    1.0 = Original Image Ratio
ASPECT_RATIO_CORRECTION = 1.0
# ---------------------

def resize_maintain_aspect(img, max_size=600):
    h, w = img.shape[:2]
    scale = max_size / max(h, w)
    new_w, new_h = int(w * scale), int(h * scale)
    return cv2.resize(img, (new_w, new_h))

def skeletonize(img):
    thresh = cv2.adaptiveThreshold(img, 255, cv2.ADAPTIVE_THRESH_GAUSSIAN_C, 
                                   cv2.THRESH_BINARY_INV, 11, 2)
    element = cv2.getStructuringElement(cv2.MORPH_CROSS, (3, 3))
    skel = np.zeros(img.shape, np.uint8)
    while True:
        eroded = cv2.erode(thresh, element)
        temp = cv2.dilate(eroded, element)
        temp = cv2.subtract(thresh, temp)
        skel = cv2.bitwise_or(skel, temp)
        thresh = eroded.copy()
        if cv2.countNonZero(thresh) == 0:
            break
    return skel

def sort_points_nearest_neighbor(pixels):
    if len(pixels) == 0: return [], [], []
    points_left = np.array(pixels, dtype=float)
    
    path_x = [points_left[0][1]]
    path_y = [points_left[0][0]]
    path_laser = [False]
    
    current_point = points_left[0]
    points_left = np.delete(points_left, 0, axis=0)
    JUMP_THRESHOLD = 20.0 

    while len(points_left) > 0:
        dists = np.sum((points_left - current_point)**2, axis=1)
        nearest_idx = np.argmin(dists)
        dist_sq = dists[nearest_idx]
        
        next_point = points_left[nearest_idx]
        path_x.append(next_point[1])
        path_y.append(next_point[0])
        
        if dist_sq > (JUMP_THRESHOLD**2):
            path_laser.append(False)
        else:
            path_laser.append(True)
            
        current_point = next_point
        points_left = np.delete(points_left, nearest_idx, axis=0)

    return path_x, path_y, path_laser

def process_image(filename):
    img = cv2.imread(filename)
    if img is None: raise FileNotFoundError("Image not found")
    
    print(f"Processing {filename}...")
    img = resize_maintain_aspect(img, 600)
    gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
    gray = cv2.GaussianBlur(gray, (5, 5), 0)
    
    skel = skeletonize(gray)
    pixels = np.column_stack(np.where(skel > 0))
    
    if len(pixels) == 0: return [], [], [], 0, 0
    
    print(f"Sorting {len(pixels)} pixels...")
    raw_x, raw_y, raw_laser = sort_points_nearest_neighbor(pixels)
    
    total_pixels = len(raw_x)
    step = max(1, int(total_pixels / GLOBAL_MAX_POINTS))
    
    final_x = raw_x[::step]
    final_y = raw_y[::step]
    final_laser = raw_laser[::step]
    
    if (len(raw_x) - 1) % step != 0:
        final_x.append(raw_x[-1])
        final_y.append(raw_y[-1])
        final_laser.append(True)

    return final_x, final_y, final_laser, img.shape[1], img.shape[0]

def convert_to_angles_flat_field(x_pixels, y_pixels, img_width, img_height):
    """
    Corrects for Flat Wall Projection using Physical Distances (Meters).
    """
    if not x_pixels: return [], []

    xs = np.array(x_pixels)
    ys = np.array(y_pixels)
    
    # 1. Normalize Pixels to range -1.0 to 1.0 (relative to largest side)
    max_dim = max(img_width, img_height)
    x_norm = (xs - (img_width / 2)) / (max_dim / 2)
    y_norm = (ys - (img_height / 2)) / (max_dim / 2)

    # Apply Manual Aspect Ratio Correction
    y_norm = y_norm * ASPECT_RATIO_CORRECTION

    # 2. Convert to Physical Meters on the Wall
    # x_norm is -1 to 1. 
    # Multiply by half the projected size to get meters from center.
    x_meters = x_norm * (PROJECTED_SIZE_METERS / 2.0)
    y_meters = y_norm * (PROJECTED_SIZE_METERS / 2.0)

    # 3. Calculate Angles using Arctangent (Trigonometry)
    # theta = arctan( opposite / adjacent )
    theta_x_rad = np.arctan(x_meters / WALL_DISTANCE_METERS)
    theta_y_rad = np.arctan(y_meters / WALL_DISTANCE_METERS)
    
    # 4. Convert to Degrees and Center at 45
    theta_x = np.degrees(theta_x_rad) + 45
    theta_y = np.degrees(theta_y_rad) + 45
    
    # FLIP Y Axis (Standard Motor Logic)
    theta_y = 90 - theta_y

    # Check for safety limits
    if np.min(theta_x) < 0 or np.max(theta_x) > 90:
        print("WARNING: Calculated angles are outside 0-90 degree range!")
        print("Try moving the wall closer or making the projection smaller.")

    return theta_x.tolist(), theta_y.tolist()

def save_to_files(laser_list, x_list, y_list):
    x_list = [round(x, 2) for x in x_list]
    y_list = [round(y, 2) for y in y_list]

    with open("laser.txt", 'w') as f:
        str_data = ['true' if item else 'false' for item in laser_list]
        f.write(str(str_data).replace("'", '"').replace("[", "{").replace("]", "}"))

    with open("x.txt", 'w') as f:
        f.write(str(x_list).replace("[", "{").replace("]", "}"))

    with open("y.txt", 'w') as f:
        f.write(str(y_list).replace("[", "{").replace("]", "}"))

    print(f"\nDONE. Generated files for a {PROJECTED_SIZE_METERS}m wide projection at {WALL_DISTANCE_METERS}m distance.")

if __name__ == "__main__":
    raw_x, raw_y, laser_bools, w, h = process_image(INPUT_IMAGE)
    if raw_x:
        x_angles, y_angles = convert_to_angles_flat_field(raw_x, raw_y, w, h)
        save_to_files(laser_bools, x_angles, y_angles)