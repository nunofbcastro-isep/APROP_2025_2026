use threadpool::ThreadPool;
use std::sync::mpsc::channel;
use std::time::Instant;
use std::io::{self, Write};

#[derive(Clone, Copy)]
struct Complex {
    r: f64,
    i: f64,
}

const EPS: f64 = 1e-5;

fn read_u32(prompt: &str) -> u32 {
    loop {
        print!("{}", prompt);
        io::stdout().flush().unwrap();
        
        let mut input = String::new();
        io::stdin().read_line(&mut input).unwrap();
        
        match input.trim().parse::<u32>() {
            Ok(num) if num > 0 => return num,
            _ => println!("Por favor, insira um número inteiro positivo válido."),
        }
    }
}

fn main() {
    println!("Mandelbrot - ThreadPool Implementation!");
    println!("=========================================\n");
    
    let npoints = read_u32("NPOINTS (número de pontos por dimensão, ex: 1000): ");
    let maxiter = read_u32("MAXITER (iterações máximas, ex: 1000): ");
    
    println!("\nExecutando com NPOINTS={}, MAXITER={}\n", npoints, maxiter);

    let start = Instant::now();
    let (area, erro) = estimate_area(npoints, maxiter);
    let duration = start.elapsed();

    println!(
        "Area of Mandlebrot set = {:12.8} +/- {:12.8}",
        area, erro
    );
    println!("Elapsed time: {:?} ({:.2} ms)", duration, duration.as_secs_f64() * 1000.0);
}

fn estimate_area(npoints: u32, maxiter: u32) -> (f64, f64) {
    let np = npoints as f64;
    let size = np * np;
    let n_workers = num_cpus::get();
    let pool = ThreadPool::new(n_workers);
    let (tx, rx) = channel();

    for i in 0..npoints {
        let tx = tx.clone();
        pool.execute(move || {
            let mut local_outside = 0;
            for j in 0..npoints {
                let c = Complex {
                    r: -2.0 + 2.5 * (i as f64) / np + EPS,
                    i: 1.125 * (j as f64) / np + EPS,
                };
                local_outside += test_point(c, maxiter) as u32;
            }
            tx.send(local_outside).expect("channel will be there waiting for the pool");
        });
    }

    // Drop the original sender so the receiver knows when to stop
    drop(tx);

    let num_outside: u32 = rx.iter().sum();

    let area = 2.0 * 2.5 * 1.125 * ((size - num_outside as f64) / size);
    let erro = area / np;

    (area, erro)
}

fn test_point(c: Complex, maxiter: u32) -> i32 {
    let mut z = c;
    for _ in 0..maxiter {
        let temp = (z.r * z.r) - (z.i * z.i) + c.r;
        z.i = 2.0 * z.r * z.i + c.i;
        z.r = temp;

        if z.r * z.r + z.i * z.i > 4.0 {
            return 1;
        }
    }
    0
}