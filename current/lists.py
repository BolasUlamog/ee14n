import os

# --- CONFIGURATION ---
# Change this to match the name of your source file
input_filename = 'theta_points.txt' 
# ---------------------

def process_data():
    # Initialize lists
    laser_list = []
    x_list = []
    y_list = []

    try:
        with open(input_filename, 'r') as f:
            for line in f:
                # Remove whitespace and skip empty lines
                line = line.strip()
                if not line:
                    continue
                
                parts = line.split(',')
                
                # Check if the line has enough data points to avoid errors
                if len(parts) >= 3:
                    # Parse Laser (Boolean)
                    # This checks if the string is exactly "True"
                    laser_list.append(parts[0].strip() == 'True')
                    
                    # Parse X and Y (Floats)
                    x_list.append(float(parts[1].strip()))
                    y_list.append(float(parts[2].strip()))

        # Helper function to write a list to a single-line file
        def write_file(name, data):
            with open(name, 'w') as out_file:
                out_file.write(str(data))

        # Generate the files
        write_file("laser.txt", laser_list)
        write_file("x.txt", x_list)
        write_file("y.txt", y_list)

        print(f"Success! Processed {len(laser_list)} lines.")
        print("Generated: laser.txt, x.txt, y.txt")

    except FileNotFoundError:
        print(f"Error: The file '{input_filename}' was not found.")
    except ValueError as e:
        print(f"Error processing data types: {e}")

if __name__ == "__main__":
    process_data()
