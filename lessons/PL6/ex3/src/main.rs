mod complex;
mod complex_vector;
use complex::{Complex};
use complex_vector::*;
use rand::Rng;

fn generate_random_complex_vector(size: usize) -> Vec<Complex> {
    let mut rng = rand::thread_rng();
    let mut vec: Vec<Complex> = Vec::new();

    for _ in 0..size {
        let real = rng.r#gen::<f64>();
        let imag = rng.r#gen::<f64>();
        vec.push(Complex::init(real, imag));
    }

    vec
}

fn main() {
    let vec = generate_random_complex_vector(10);

    println!("Generated complex numbers:");
    for c in &vec {
        println!("{} + {}i", c.real, c.imag);
    }

    // a. Max, min, average
    if let Some(max) = max_complex(&vec) {
        println!("Max (by magnitude): {} + {}i (magnitude: {})", max.real, max.imag, max.magnitude());
    }
    if let Some(min) = min_complex(&vec) {
        println!("Min (by magnitude): {} + {}i (magnitude: {})", min.real, min.imag, min.magnitude());
    }
    if let Some(avg) = average_complex(&vec) {
        println!("Average: {} + {}i", avg.real, avg.imag);
    }

    // b. Sum
    let sum = sum_complex(&vec);
    println!("Sum: {} + {}i", sum.real, sum.imag);

    // c. Modules
    let mods = modules(&vec);
    println!("Modules: {:?}", mods);
}
