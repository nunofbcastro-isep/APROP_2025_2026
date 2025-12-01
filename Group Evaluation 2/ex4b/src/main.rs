use rayon::prelude::*;
use std::time::Instant;
use std::io::{self, Write};
use threadpool::ThreadPool;
use std::sync::mpsc::channel;

#[derive(Clone, Copy)]
struct Complex {
    r: f64,
    i: f64,
}

const EPS: f64 = 1e-5;
const NUM_EXECUTIONS: usize = 10;

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
    println!("Mandelbrot - Benchmark (Sequential vs ThreadPool vs Rayon)");
    println!("=========================================================\n");
    
    let npoints = read_u32("NPOINTS (número de pontos por dimensão, ex: 1000): ");
    let maxiter = read_u32("MAXITER (iterações máximas, ex: 1000): ");
    
    println!("\nExecutando {} vezes para cada implementação com NPOINTS={}, MAXITER={}...\n", NUM_EXECUTIONS, npoints, maxiter);

    let implementations = vec![
        ("Sequential", estimate_area_seq as fn(u32, u32) -> (f64, f64)),
        ("ThreadPool Line", estimate_area_thread_pool_for_line as fn(u32, u32) -> (f64, f64)),
        ("ThreadPool Pixel", estimate_area_thread_pool_for_pixel as fn(u32, u32) -> (f64, f64)),
        ("Rayon", estimate_area_rayon as fn(u32, u32) -> (f64, f64)),
    ];

    println!("{:<20} | {:<15} | {:<15} | {:<15}", "Implementation", "Avg Time (ms)", "Area", "Error");
    println!("{:-<20}-+-{:-<15}-+-{:-<15}-+-{:-<15}", "", "", "", "");

    for (name, func) in implementations {
        let mut total_duration = 0.0;
        let mut last_area = 0.0;
        let mut last_error = 0.0;

        for _ in 0..NUM_EXECUTIONS {
            let start = Instant::now();
            let (area, error) = func(npoints, maxiter);
            let duration = start.elapsed();
            total_duration += duration.as_secs_f64() * 1000.0;
            last_area = area;
            last_error = error;
        }

        let avg_duration = total_duration / NUM_EXECUTIONS as f64;
        println!("{:<20} | {:<15.2} | {:<15.8} | {:<15.8}", name, avg_duration, last_area, last_error);
    }
}

fn estimate_area_seq(npoints: u32, maxiter: u32) -> (f64, f64) {
    let mut num_outside = 0;
    for i in 0..npoints {
        for j in 0..npoints {
            let c = Complex {
                r: -2.0 + 2.5 * (i as f64) / (npoints as f64) + EPS,
                i: 1.125 * (j as f64) / (npoints as f64) + EPS,
            };

            num_outside += test_point(c, maxiter);
        }
    }

    let np = npoints as f64;
    let size = np * np;
    let area = 2.0 * 2.5 * 1.125 * ((size - num_outside as f64) / size);
    let error = area / npoints as f64;
    (area, error)
}

fn estimate_area_thread_pool_for_line(npoints: u32, maxiter: u32) -> (f64, f64) {
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

    drop(tx);

    let num_outside: u32 = rx.iter().sum();

    let area = 2.0 * 2.5 * 1.125 * ((size - num_outside as f64) / size);
    let erro = area / np;

    (area, erro)
}

fn estimate_area_thread_pool_for_pixel(npoints: u32, maxiter: u32) -> (f64, f64) {
    let np = npoints as f64;
    let size = np * np;
    let n_workers = num_cpus::get();
    let pool = ThreadPool::new(n_workers);
    let (tx, rx) = channel();

    for i in 0..npoints {
        for j in 0..npoints {
            let tx = tx.clone();
            pool.execute(move || {
                let c = Complex {
                    r: -2.0 + 2.5 * (i as f64) / np + EPS,
                    i: 1.125 * (j as f64) / np + EPS,
                };
                let val = test_point(c, maxiter) as u32;
                tx.send(val).expect("channel will be there waiting for the pool");
            });
        }
    }

    drop(tx);

    let num_outside: u32 = rx.iter().sum();

    let area = 2.0 * 2.5 * 1.125 * ((size - num_outside as f64) / size);
    let erro = area / np;

    (area, erro)
}

fn estimate_area_rayon(npoints: u32, maxiter: u32) -> (f64, f64) {
    let np = npoints as f64;
    let size = np * np;

    let num_outside: u32 = (0..npoints)
        .into_par_iter()
        .map(|i| {
            (0..npoints).map(|j| {
                let c = Complex {
                    r: -2.0 + 2.5 * (i as f64) / np + EPS,
                    i: 1.125 * (j as f64) / np + EPS,
                };
                test_point(c, maxiter) as u32
            }).sum::<u32>()
        })
        .sum();

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