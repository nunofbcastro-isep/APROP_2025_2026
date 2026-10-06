#[derive(Clone, Copy)]
struct Complex {
    r: f64,
    i: f64,
}

const NPOINTS: u32 = 1000;
const MAXITER: u32 = 1000;
const EPS: f64 = 1e-5;

fn main() {
    println!("Mandelbrot!");

    let (area, erro) = estimate_area();

    println!(
        "Area of Mandlebrot set = {:12.8} +/- {:12.8}",
        area, erro
    );
}

fn estimate_area() -> (f64, f64) {
    let np = NPOINTS as f64;
    let size = np * np;

    let num_outside: u32 = (0..NPOINTS)
        .flat_map(|i| {
            (0..NPOINTS).map(move |j| {
                let c = Complex {
                    r: -2.0 + 2.5 * (i as f64) / np + EPS,
                    i: 1.125 * (j as f64) / np + EPS,
                };
                test_point(c) as u32
            })
        })
        .sum();

    let area = 2.0 * 2.5 * 1.125 * ((size - num_outside as f64) / size);
    let erro = area / np;

    (area, erro)
}

fn test_point(c: Complex) -> i32 {
    let mut z = c;
    for _ in 0..MAXITER {
        let temp = (z.r * z.r) - (z.i * z.i) + c.r;
        z.i = 2.0 * z.r * z.i + c.i;
        z.r = temp;

        if z.r * z.r + z.i * z.i > 4.0 {
            return 1;
        }
    }
    0
}