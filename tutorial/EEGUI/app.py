"""
EEGUI - Laser Projector Image to Arduino Code Generator
A desktop application for converting images to stepper motor control code
"""

import customtkinter as ctk
from tkinter import filedialog, messagebox
from tkinterdnd2 import DND_FILES, TkinterDnD
from PIL import Image, ImageTk
import threading
from pathlib import Path
import os

from processor import ProcessingConfig, ProcessingResult, process_image
from cpp_generator import save_cpp_file


# === THEME CONFIGURATION ===
ctk.set_appearance_mode("dark")
ctk.set_default_color_theme("blue")

# Custom colors
COLORS = {
    "bg_dark": "#0d0d0d",
    "bg_card": "#1a1a1a", 
    "bg_input": "#242424",
    "accent": "#00d4aa",
    "accent_hover": "#00b894",
    "accent_dim": "#006b55",
    "text": "#ffffff",
    "text_dim": "#888888",
    "border": "#333333",
    "success": "#00d4aa",
    "warning": "#ffb347",
    "error": "#ff6b6b",
    "drop_zone": "#1e1e1e",
    "drop_zone_active": "#002d24",
}


class TkinterDnDCustom(ctk.CTk, TkinterDnD.DnDWrapper):
    """Custom CTk window with drag-and-drop support"""
    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self.TkdndVersion = TkinterDnD._require(self)


class DropZone(ctk.CTkFrame):
    """Image drop zone with preview"""
    
    def __init__(self, master, on_image_dropped, **kwargs):
        super().__init__(master, **kwargs)
        
        self.on_image_dropped = on_image_dropped
        self.current_image_path = None
        
        self.configure(
            fg_color=COLORS["drop_zone"],
            corner_radius=16,
            border_width=2,
            border_color=COLORS["border"]
        )
        
        # Inner container
        self.inner = ctk.CTkFrame(self, fg_color="transparent")
        self.inner.pack(expand=True, fill="both", padx=20, pady=20)
        
        # Icon/Preview area
        self.preview_label = ctk.CTkLabel(
            self.inner,
            text="",
            font=ctk.CTkFont(size=48)
        )
        self.preview_label.pack(pady=(20, 10))
        
        # Drop text
        self.drop_text = ctk.CTkLabel(
            self.inner,
            text="Drop image here",
            font=ctk.CTkFont(family="SF Pro Display", size=20, weight="bold"),
            text_color=COLORS["text"]
        )
        self.drop_text.pack(pady=(0, 5))
        
        self.subtext = ctk.CTkLabel(
            self.inner,
            text="or click to browse • PNG, JPG, BMP supported",
            font=ctk.CTkFont(size=12),
            text_color=COLORS["text_dim"]
        )
        self.subtext.pack(pady=(0, 10))
        
        # Browse button
        self.browse_btn = ctk.CTkButton(
            self.inner,
            text="Browse Files",
            font=ctk.CTkFont(size=13),
            fg_color=COLORS["bg_input"],
            hover_color=COLORS["border"],
            border_width=1,
            border_color=COLORS["border"],
            corner_radius=8,
            height=36,
            command=self.browse_file
        )
        self.browse_btn.pack(pady=(5, 20))
        
        # Image preview (hidden initially)
        self.image_preview = ctk.CTkLabel(self.inner, text="")
        
        # Setup drag and drop
        self.drop_target_register(DND_FILES)
        self.dnd_bind("<<Drop>>", self.on_drop)
        self.dnd_bind("<<DragEnter>>", self.on_drag_enter)
        self.dnd_bind("<<DragLeave>>", self.on_drag_leave)
        
        # Click to browse
        self.bind("<Button-1>", lambda e: self.browse_file())
        self.inner.bind("<Button-1>", lambda e: self.browse_file())
        
        self.reset_preview()
    
    def reset_preview(self):
        """Reset to initial state"""
        self.preview_label.configure(text="🖼️")
        self.image_preview.pack_forget()
        self.drop_text.configure(text="Drop image here")
        self.subtext.pack()
        self.browse_btn.pack()
        self.current_image_path = None
    
    def on_drag_enter(self, event):
        self.configure(
            fg_color=COLORS["drop_zone_active"],
            border_color=COLORS["accent"]
        )
        return event.action
    
    def on_drag_leave(self, event):
        self.configure(
            fg_color=COLORS["drop_zone"],
            border_color=COLORS["border"]
        )
        return event.action
    
    def on_drop(self, event):
        self.configure(
            fg_color=COLORS["drop_zone"],
            border_color=COLORS["border"]
        )
        
        # Parse dropped file path
        file_path = event.data
        # Handle paths with spaces (wrapped in braces on some systems)
        if file_path.startswith("{") and file_path.endswith("}"):
            file_path = file_path[1:-1]
        
        self.load_image(file_path)
        return event.action
    
    def browse_file(self):
        file_path = filedialog.askopenfilename(
            title="Select Image",
            filetypes=[
                ("Image files", "*.png *.jpg *.jpeg *.bmp *.gif"),
                ("All files", "*.*")
            ]
        )
        if file_path:
            self.load_image(file_path)
    
    def load_image(self, file_path):
        """Load and display image preview"""
        try:
            # Validate file exists
            if not os.path.exists(file_path):
                messagebox.showerror("Error", f"File not found: {file_path}")
                return
            
            # Load image for preview
            img = Image.open(file_path)
            
            # Calculate preview size (maintain aspect ratio)
            max_size = 180
            ratio = min(max_size / img.width, max_size / img.height)
            new_size = (int(img.width * ratio), int(img.height * ratio))
            img_preview = img.resize(new_size, Image.Resampling.LANCZOS)
            
            # Convert to CTkImage
            self.preview_photo = ctk.CTkImage(
                light_image=img_preview,
                dark_image=img_preview,
                size=new_size
            )
            
            # Update UI
            self.preview_label.configure(text="", image=self.preview_photo)
            self.image_preview.configure(image=self.preview_photo)
            
            # Show filename
            filename = Path(file_path).name
            self.drop_text.configure(text=filename)
            self.subtext.pack_forget()
            self.browse_btn.pack_forget()
            
            self.current_image_path = file_path
            self.on_image_dropped(file_path)
            
        except Exception as e:
            messagebox.showerror("Error", f"Could not load image: {e}")


class ParameterInput(ctk.CTkFrame):
    """Styled parameter input with label"""
    
    def __init__(self, master, label, default_value, unit="", tooltip="", **kwargs):
        super().__init__(master, fg_color="transparent", **kwargs)
        
        # Label row
        label_frame = ctk.CTkFrame(self, fg_color="transparent")
        label_frame.pack(fill="x", pady=(0, 6))
        
        self.label = ctk.CTkLabel(
            label_frame,
            text=label,
            font=ctk.CTkFont(size=13, weight="bold"),
            text_color=COLORS["text"],
            anchor="w"
        )
        self.label.pack(side="left")
        
        if unit:
            self.unit_label = ctk.CTkLabel(
                label_frame,
                text=unit,
                font=ctk.CTkFont(size=11),
                text_color=COLORS["text_dim"],
                anchor="e"
            )
            self.unit_label.pack(side="right")
        
        # Input field
        self.entry = ctk.CTkEntry(
            self,
            height=40,
            font=ctk.CTkFont(size=14),
            fg_color=COLORS["bg_input"],
            border_color=COLORS["border"],
            border_width=1,
            corner_radius=8,
            placeholder_text=str(default_value)
        )
        self.entry.insert(0, str(default_value))
        self.entry.pack(fill="x")
        
        # Tooltip
        if tooltip:
            self.tooltip = ctk.CTkLabel(
                self,
                text=tooltip,
                font=ctk.CTkFont(size=11),
                text_color=COLORS["text_dim"],
                anchor="w"
            )
            self.tooltip.pack(fill="x", pady=(4, 0))
    
    def get_value(self) -> str:
        return self.entry.get()
    
    def get_float(self) -> float:
        try:
            return float(self.entry.get())
        except ValueError:
            return 0.0
    
    def get_int(self) -> int:
        try:
            return int(float(self.entry.get()))
        except ValueError:
            return 0


class StatusBar(ctk.CTkFrame):
    """Status bar with progress indicator"""
    
    def __init__(self, master, **kwargs):
        super().__init__(master, fg_color=COLORS["bg_card"], corner_radius=12, **kwargs)
        
        self.inner = ctk.CTkFrame(self, fg_color="transparent")
        self.inner.pack(fill="x", padx=16, pady=12)
        
        # Status icon
        self.icon = ctk.CTkLabel(
            self.inner,
            text="○",
            font=ctk.CTkFont(size=14),
            text_color=COLORS["text_dim"]
        )
        self.icon.pack(side="left", padx=(0, 8))
        
        # Status text
        self.status_text = ctk.CTkLabel(
            self.inner,
            text="Ready - Drop an image to begin",
            font=ctk.CTkFont(size=13),
            text_color=COLORS["text_dim"],
            anchor="w"
        )
        self.status_text.pack(side="left", fill="x", expand=True)
        
        # Progress bar (hidden initially)
        self.progress = ctk.CTkProgressBar(
            self.inner,
            height=4,
            corner_radius=2,
            fg_color=COLORS["bg_input"],
            progress_color=COLORS["accent"]
        )
    
    def set_status(self, message: str, status_type: str = "info"):
        """Update status message and icon"""
        icons = {
            "info": ("○", COLORS["text_dim"]),
            "processing": ("◐", COLORS["accent"]),
            "success": ("●", COLORS["success"]),
            "warning": ("●", COLORS["warning"]),
            "error": ("●", COLORS["error"]),
        }
        icon, color = icons.get(status_type, icons["info"])
        
        self.icon.configure(text=icon, text_color=color)
        self.status_text.configure(text=message, text_color=color if status_type != "info" else COLORS["text"])
    
    def show_progress(self, indeterminate=True):
        self.progress.pack(side="right", padx=(10, 0))
        if indeterminate:
            self.progress.configure(mode="indeterminate")
            self.progress.start()
    
    def hide_progress(self):
        self.progress.stop()
        self.progress.pack_forget()


class App(TkinterDnDCustom):
    """Main application window"""
    
    def __init__(self):
        super().__init__()
        
        self.title("EEGUI · Laser Projector Generator")
        self.geometry("720x820")
        self.minsize(640, 700)
        self.configure(fg_color=COLORS["bg_dark"])
        
        # State
        self.current_image_path = None
        self.processing_result = None
        
        self.create_ui()
    
    def create_ui(self):
        """Build the user interface"""
        
        # Main container with padding
        main = ctk.CTkFrame(self, fg_color="transparent")
        main.pack(fill="both", expand=True, padx=24, pady=24)
        
        # === HEADER ===
        header = ctk.CTkFrame(main, fg_color="transparent")
        header.pack(fill="x", pady=(0, 20))
        
        title = ctk.CTkLabel(
            header,
            text="EEGUI",
            font=ctk.CTkFont(family="SF Pro Display", size=32, weight="bold"),
            text_color=COLORS["accent"]
        )
        title.pack(side="left")
        
        subtitle = ctk.CTkLabel(
            header,
            text="Laser Projector Code Generator",
            font=ctk.CTkFont(size=14),
            text_color=COLORS["text_dim"]
        )
        subtitle.pack(side="left", padx=(12, 0), pady=(8, 0))
        
        # === DROP ZONE ===
        self.drop_zone = DropZone(
            main,
            on_image_dropped=self.on_image_loaded,
            height=280
        )
        self.drop_zone.pack(fill="x", pady=(0, 20))
        
        # === PARAMETERS SECTION ===
        params_label = ctk.CTkLabel(
            main,
            text="PROJECTION SETTINGS",
            font=ctk.CTkFont(size=11, weight="bold"),
            text_color=COLORS["text_dim"]
        )
        params_label.pack(anchor="w", pady=(0, 12))
        
        params_card = ctk.CTkFrame(main, fg_color=COLORS["bg_card"], corner_radius=12)
        params_card.pack(fill="x", pady=(0, 20))
        
        params_inner = ctk.CTkFrame(params_card, fg_color="transparent")
        params_inner.pack(fill="x", padx=20, pady=20)
        
        # Grid layout for parameters
        params_inner.grid_columnconfigure((0, 1), weight=1, uniform="col")
        
        # Wall Distance
        self.wall_distance = ParameterInput(
            params_inner,
            label="Wall Distance",
            default_value="1.6",
            unit="meters",
            tooltip="Distance from laser to wall"
        )
        self.wall_distance.grid(row=0, column=0, sticky="ew", padx=(0, 10), pady=(0, 16))
        
        # Projection Size
        self.projection_size = ParameterInput(
            params_inner,
            label="Projection Size",
            default_value="4.0",
            unit="meters",
            tooltip="Max dimension of projection"
        )
        self.projection_size.grid(row=0, column=1, sticky="ew", padx=(10, 0), pady=(0, 16))
        
        # Max Points
        self.max_points = ParameterInput(
            params_inner,
            label="Max Points",
            default_value="100",
            unit="points",
            tooltip="Maximum path points (budget)"
        )
        self.max_points.grid(row=1, column=0, sticky="ew", padx=(0, 10), pady=(0, 0))
        
        # Aspect Ratio
        self.aspect_ratio = ParameterInput(
            params_inner,
            label="Aspect Correction",
            default_value="1.0",
            unit="ratio",
            tooltip="Vertical stretch factor"
        )
        self.aspect_ratio.grid(row=1, column=1, sticky="ew", padx=(10, 0), pady=(0, 0))
        
        # === GENERATE BUTTON ===
        self.generate_btn = ctk.CTkButton(
            main,
            text="Generate Arduino Code",
            font=ctk.CTkFont(size=16, weight="bold"),
            height=52,
            corner_radius=12,
            fg_color=COLORS["accent"],
            hover_color=COLORS["accent_hover"],
            text_color=COLORS["bg_dark"],
            command=self.generate_code
        )
        self.generate_btn.pack(fill="x", pady=(0, 16))
        
        # === STATUS BAR ===
        self.status_bar = StatusBar(main)
        self.status_bar.pack(fill="x", pady=(0, 0))
        
        # Footer
        footer = ctk.CTkLabel(
            main,
            text="Converts images to stepper motor angles for laser projection",
            font=ctk.CTkFont(size=11),
            text_color=COLORS["text_dim"]
        )
        footer.pack(pady=(16, 0))
    
    def on_image_loaded(self, file_path: str):
        """Handle image loaded event"""
        self.current_image_path = file_path
        self.status_bar.set_status(f"Image loaded: {Path(file_path).name}", "success")
    
    def generate_code(self):
        """Process image and generate C++ code"""
        if not self.current_image_path:
            self.status_bar.set_status("Please drop an image first", "warning")
            return
        
        # Get parameters
        try:
            config = ProcessingConfig(
                max_points=self.max_points.get_int(),
                wall_distance_meters=self.wall_distance.get_float(),
                projected_size_meters=self.projection_size.get_float(),
                aspect_ratio_correction=self.aspect_ratio.get_float()
            )
        except Exception as e:
            self.status_bar.set_status(f"Invalid parameters: {e}", "error")
            return
        
        # Validate parameters
        if config.max_points < 1:
            self.status_bar.set_status("Max points must be at least 1", "error")
            return
        if config.wall_distance_meters <= 0:
            self.status_bar.set_status("Wall distance must be positive", "error")
            return
        if config.projected_size_meters <= 0:
            self.status_bar.set_status("Projection size must be positive", "error")
            return
        
        # Show processing state
        self.generate_btn.configure(state="disabled", text="Processing...")
        self.status_bar.set_status("Processing image...", "processing")
        self.status_bar.show_progress()
        
        # Process in background thread
        def process():
            result = process_image(self.current_image_path, config)
            self.after(0, lambda: self.on_processing_complete(result, config))
        
        thread = threading.Thread(target=process, daemon=True)
        thread.start()
    
    def on_processing_complete(self, result: ProcessingResult, config: ProcessingConfig):
        """Handle processing completion"""
        self.status_bar.hide_progress()
        self.generate_btn.configure(state="normal", text="Generate Arduino Code")
        
        if not result.success:
            self.status_bar.set_status(result.message, "error")
            return
        
        self.processing_result = result
        
        # Ask where to save
        default_name = Path(self.current_image_path).stem + "_laser.cpp"
        output_path = filedialog.asksaveasfilename(
            title="Save Arduino Code",
            defaultextension=".cpp",
            initialfile=default_name,
            filetypes=[("C++ files", "*.cpp"), ("All files", "*.*")]
        )
        
        if not output_path:
            self.status_bar.set_status("Export cancelled", "info")
            return
        
        # Generate and save C++ file
        try:
            save_cpp_file(
                output_path,
                result.x_angles,
                result.y_angles,
                result.laser_states,
                config.wall_distance_meters,
                config.projected_size_meters
            )
            
            self.status_bar.set_status(
                f"✓ Saved {result.point_count} points to {Path(output_path).name}",
                "success"
            )
            
            # Show success dialog
            messagebox.showinfo(
                "Success!",
                f"Generated Arduino code with {result.point_count} points.\n\n"
                f"File saved to:\n{output_path}\n\n"
                f"Wall Distance: {config.wall_distance_meters}m\n"
                f"Projection Size: {config.projected_size_meters}m"
            )
            
        except Exception as e:
            self.status_bar.set_status(f"Failed to save: {e}", "error")


def main():
    app = App()
    app.mainloop()


if __name__ == "__main__":
    main()

