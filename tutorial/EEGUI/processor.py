"""
Image Processor Module for Laser Projector
Converts images to stepper motor angle coordinates
"""

import cv2
import numpy as np
from dataclasses import dataclass
from typing import Tuple, List, Optional
from pathlib import Path


@dataclass
class ProcessingConfig:
    """Configuration for image processing"""
    max_points: int = 100
    wall_distance_meters: float = 1.6
    projected_size_meters: float = 4.0
    aspect_ratio_correction: float = 1.0


@dataclass
class ProcessingResult:
    """Result of image processing"""
    x_angles: List[float]
    y_angles: List[float]
    laser_states: List[bool]
    point_count: int
    success: bool
    message: str


def resize_maintain_aspect(img: np.ndarray, max_size: int = 600) -> np.ndarray:
    """Resize image maintaining aspect ratio"""
    h, w = img.shape[:2]
    scale = max_size / max(h, w)
    new_w, new_h = int(w * scale), int(h * scale)
    return cv2.resize(img, (new_w, new_h))


def skeletonize(img: np.ndarray) -> np.ndarray:
    """Extract skeleton from grayscale image"""
    thresh = cv2.adaptiveThreshold(
        img, 255, cv2.ADAPTIVE_THRESH_GAUSSIAN_C,
        cv2.THRESH_BINARY_INV, 11, 2
    )
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


def sort_points_nearest_neighbor(pixels: np.ndarray) -> Tuple[List[float], List[float], List[bool]]:
    """Sort pixels using nearest neighbor algorithm to create continuous path"""
    if len(pixels) == 0:
        return [], [], []
    
    points_left = np.array(pixels, dtype=float)
    
    path_x = [points_left[0][1]]
    path_y = [points_left[0][0]]
    path_laser = [False]
    
    current_point = points_left[0]
    points_left = np.delete(points_left, 0, axis=0)
    JUMP_THRESHOLD = 20.0

    while len(points_left) > 0:
        dists = np.sum((points_left - current_point) ** 2, axis=1)
        nearest_idx = np.argmin(dists)
        dist_sq = dists[nearest_idx]
        
        next_point = points_left[nearest_idx]
        path_x.append(next_point[1])
        path_y.append(next_point[0])
        
        if dist_sq > (JUMP_THRESHOLD ** 2):
            path_laser.append(False)
        else:
            path_laser.append(True)
        
        current_point = next_point
        points_left = np.delete(points_left, nearest_idx, axis=0)

    return path_x, path_y, path_laser


def process_image_to_pixels(
    image_path: str,
    max_points: int
) -> Tuple[List[float], List[float], List[bool], int, int]:
    """Process image and extract pixel path"""
    img = cv2.imread(image_path)
    if img is None:
        raise FileNotFoundError(f"Could not load image: {image_path}")
    
    img = resize_maintain_aspect(img, 600)
    gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
    gray = cv2.GaussianBlur(gray, (5, 5), 0)
    
    skel = skeletonize(gray)
    pixels = np.column_stack(np.where(skel > 0))
    
    if len(pixels) == 0:
        return [], [], [], 0, 0
    
    raw_x, raw_y, raw_laser = sort_points_nearest_neighbor(pixels)
    
    # Downsample to max points
    total_pixels = len(raw_x)
    step = max(1, int(total_pixels / max_points))
    
    final_x = raw_x[::step]
    final_y = raw_y[::step]
    final_laser = raw_laser[::step]
    
    # Ensure we include the last point
    if (len(raw_x) - 1) % step != 0:
        final_x.append(raw_x[-1])
        final_y.append(raw_y[-1])
        final_laser.append(True)

    return final_x, final_y, final_laser, img.shape[1], img.shape[0]


def convert_to_angles(
    x_pixels: List[float],
    y_pixels: List[float],
    img_width: int,
    img_height: int,
    config: ProcessingConfig
) -> Tuple[List[float], List[float]]:
    """Convert pixel coordinates to servo angles using flat field projection math"""
    if not x_pixels:
        return [], []

    xs = np.array(x_pixels)
    ys = np.array(y_pixels)
    
    # Normalize pixels to range -1.0 to 1.0
    max_dim = max(img_width, img_height)
    x_norm = (xs - (img_width / 2)) / (max_dim / 2)
    y_norm = (ys - (img_height / 2)) / (max_dim / 2)

    # Apply aspect ratio correction
    y_norm = y_norm * config.aspect_ratio_correction

    # Convert to physical meters on wall
    x_meters = x_norm * (config.projected_size_meters / 2.0)
    y_meters = y_norm * (config.projected_size_meters / 2.0)

    # Calculate angles using arctangent
    theta_x_rad = np.arctan(x_meters / config.wall_distance_meters)
    theta_y_rad = np.arctan(y_meters / config.wall_distance_meters)
    
    # Convert to degrees and center at 45
    theta_x = np.degrees(theta_x_rad) + 45
    theta_y = np.degrees(theta_y_rad) + 45
    
    # Flip Y axis
    theta_y = 90 - theta_y

    return theta_x.tolist(), theta_y.tolist()


def process_image(image_path: str, config: ProcessingConfig) -> ProcessingResult:
    """
    Main processing function - takes image path and config, returns angles and laser states
    """
    try:
        # Process image to get pixel path
        raw_x, raw_y, laser_bools, w, h = process_image_to_pixels(
            image_path, config.max_points
        )
        
        if not raw_x:
            return ProcessingResult(
                x_angles=[],
                y_angles=[],
                laser_states=[],
                point_count=0,
                success=False,
                message="No drawable content found in image"
            )
        
        # Convert to angles
        x_angles, y_angles = convert_to_angles(raw_x, raw_y, w, h, config)
        
        # Round angles for cleaner output
        x_angles = [round(x, 2) for x in x_angles]
        y_angles = [round(y, 2) for y in y_angles]
        
        # Check angle bounds
        warning = ""
        if min(x_angles) < 0 or max(x_angles) > 90:
            warning = " Warning: Some X angles outside 0-90° range."
        if min(y_angles) < 0 or max(y_angles) > 90:
            warning += " Warning: Some Y angles outside 0-90° range."
        
        return ProcessingResult(
            x_angles=x_angles,
            y_angles=y_angles,
            laser_states=laser_bools,
            point_count=len(x_angles),
            success=True,
            message=f"Successfully processed {len(x_angles)} points.{warning}"
        )
        
    except FileNotFoundError as e:
        return ProcessingResult(
            x_angles=[],
            y_angles=[],
            laser_states=[],
            point_count=0,
            success=False,
            message=str(e)
        )
    except Exception as e:
        return ProcessingResult(
            x_angles=[],
            y_angles=[],
            laser_states=[],
            point_count=0,
            success=False,
            message=f"Processing error: {str(e)}"
        )

