import os
from PIL import Image, ImageChops

def remove_background_by_color(image_path, target_color=(255, 255, 255), threshold=30):
    """
    Remove pixels close to target_color and replace with transparency.
    Useful for AI textures generated with solid backgrounds.
    """
    if not os.path.exists(image_path):
        print(f"Error: File {image_path} not found.")
        return None
    
    img = Image.open(image_path).convert("RGBA")
    data = img.getdata()
    
    new_data = []
    for item in data:
        # Check distance from target color
        dist = sum((item[i] - target_color[i]) ** 2 for i in range(3)) ** 0.5
        if dist < threshold:
            new_data.append((item[0], item[1], item[2], 0)) # Fully transparent
        else:
            new_data.append(item)
            
    img.putdata(new_data)
    save_path = image_path.replace(".png", "_alpha.png")
    img.save(save_path)
    print(f"Processed: {save_path}")
    return save_path

def remove_background_by_luminosity(image_path, invert=False, threshold=50):
    """
    Convert brightness to transparency. 
    Useful for black or white backgrounds.
    """
    if not os.path.exists(image_path):
        return None
        
    img = Image.open(image_path).convert("RGBA")
    gray = img.convert("L")
    
    if invert:
        gray = ImageChops.invert(gray)
        
    data = img.getdata()
    alpha_mask = gray.getdata()
    
    new_data = []
    for i in range(len(data)):
        item = data[i]
        alpha = alpha_mask[i]
        
        # Apply thresholding to force clean edges
        if alpha < threshold:
            current_alpha = 0
        else:
            current_alpha = alpha
            
        new_data.append((item[0], item[1], item[2], current_alpha))
    
    img.putdata(new_data)
    save_path = image_path.replace(".png", "_alpha.png")
    img.save(save_path)
    print(f"Processed via Luminosity: {save_path}")
    return save_path

if __name__ == "__main__":
    # Test on known forest textures
    # Fern plant (often white background)
    remove_background_by_color("assets/textures/fern_plant.png", (255, 255, 255), 100)
    # Mushroom (often white background)
    remove_background_by_color("assets/textures/mushroom.png", (255, 255, 255), 100)
    # Oak leaves
    remove_background_by_color("assets/textures/oak_leaves.png", (255, 255, 255), 80)
