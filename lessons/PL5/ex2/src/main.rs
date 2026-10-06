mod complex;
use complex::Complex;

fn main() {
    let c1 = Complex::init(1.0, 2.0);
    let c2 = Complex::init(3.0, 4.0);

    println!("Complex Number 1: ({}, {}) ", c1.real, c1.imag);
    println!("Complex Number 2: ({}, {}) ", c2.real, c2.imag);

    let c_add = c1.add(&c2);
    println!("Addition: ({}, {}) ", c_add.real, c_add.imag);

    let c_sub = c1.subtract(&c2);
    println!("Subtraction: ({}, {}) ", c_sub.real, c_sub.imag);

    let c_mul = c1.multiply(&c2);
    println!("Multiplication: ({}, {}) ", c_mul.real, c_mul.imag);

    let c_div = c1.divide(&c2);
    println!("Division: ({}, {}) ", c_div.real, c_div.imag);
}
