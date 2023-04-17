use image::{Rgba, ImageBuffer, imageops};
use nalgebra::{Point3, Vector3};

use crate::ray::Ray;
mod ray;

fn color_intensity(color: &Rgba<u8>, intensity: f32) -> Rgba<u8> {
    let mut res: Rgba<u8> = Rgba([255, 255, 255, 255]);
    res.0[0] = (color.0[0] as f32 * intensity) as u8;
    res.0[1] = (color.0[1] as f32 * intensity) as u8;
    res.0[2] = (color.0[2] as f32 * intensity) as u8;
    res.0[3] = (color.0[3] as f32 * intensity) as u8;
    
    res
}

fn add_color(c1: &Rgba<u8>, c2: &Rgba<u8>) -> Rgba<u8> {
    let mut res: Rgba<u8> = Rgba([255, 255, 255, 255]);
    res.0[0] = (c1.0[0] + c2.0[0]) as u8;
    res.0[1] = (c1.0[1] + c2.0[1]) as u8;
    res.0[2] = (c1.0[2] + c2.0[2]) as u8;
    res.0[3] = (c1.0[3] + c2.0[0]) as u8;
    
    res
}

fn ray_color(r: &ray::Ray) -> Rgba<u8> {
    let unit_direction = r.dir.normalize();
    let t = 0.5 * (unit_direction.y + 1.0);
    let white = Rgba([255, 255, 255, 255]);
    let blue = Rgba([127, 178, 255, 255]);
    // let color = (1.0 - t) * white + t * blue;
    let color = add_color(&color_intensity(&white, 1.0-t), &color_intensity(&blue, t));
    color 
}


fn main() {
    // image
    const WIDTH: u32 = 1920;
    const HEIGHT: u32 = 1080;
    let aspect: f32 = WIDTH as f32 / HEIGHT as f32;

    let mut image: ImageBuffer<Rgba<u8>, _> = ImageBuffer::new(WIDTH, HEIGHT);

    // camera
    let viewport_height: f32 = 2.0;
    let viewport_width: f32 = aspect * viewport_height;
    let focal_length: f32 = 1.0;

    let origin: Point3<f32> = Point3::new(0.0, 0.0, 0.0);
    let horizontal: Vector3<f32> = Vector3::new(viewport_width, 0.0, 0.0);
    let vertical: Vector3<f32> = Vector3::new(0.0, viewport_height, 0.0);
    let lower_left_corner: Point3<f32> = origin - horizontal/2f32 - vertical/2f32 - Vector3::new(0.0, 0.0, focal_length);

    // render
    for h in 0..HEIGHT {
        for w in 0..WIDTH {
            let u = w as f32 / (WIDTH - 1) as f32;
            let v = h as f32 / (WIDTH - 1) as f32;
            let r = Ray::new(origin, lower_left_corner + u*horizontal + v*vertical - origin);
            let pixel = ray_color(&r);
            image.put_pixel(w, h, pixel);
        }
    }

    let flipped = imageops::flip_vertical(&image);

    image.save("image.png").unwrap();
    flipped.save("flipped.png").unwrap();
}
