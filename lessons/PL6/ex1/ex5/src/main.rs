use nalgebra::{DMatrix};
use std::time::Instant;
use std::thread;

fn multiply_matrices(matrix1: &DMatrix<f64>, matrix2: &DMatrix<f64>) -> Option<DMatrix<f64>> {
    if matrix1.ncols() != matrix2.nrows() {
        return None;
    }

    let rows1 = matrix1.nrows();
    let cols2 = matrix2.ncols();
    let cols1 = matrix1.ncols();

    if rows1 < 100 {
        return Some(matrix1 * matrix2);
    }

    let num_threads = 4;
    let chunk_size = (rows1 + num_threads - 1) / num_threads;
    let chunks: Vec<_> = (0..num_threads).map(|i| {
        let start = i * chunk_size;
        let end = std::cmp::min(start + chunk_size, rows1);
        (start, end)
    }).filter(|(s, e)| s < e).collect();

    let data: Vec<f64> = thread::scope(|s| {
        let handles: Vec<_> = chunks.into_iter().map(|(start, end)| {
            s.spawn(move || {
                let mut chunk_data = Vec::with_capacity((end - start) * cols2);
                for i in start..end {
                    for j in 0..cols2 {
                        let mut sum = 0.0;
                        for k in 0..cols1 {
                            sum += matrix1[(i, k)] * matrix2[(k, j)];
                        }
                        chunk_data.push(sum);
                    }
                }
                chunk_data
            })
        }).collect();

        let mut result_data = Vec::with_capacity(rows1 * cols2);
        for h in handles {
            result_data.extend(h.join().unwrap());
        }
        result_data
    });

    Some(DMatrix::from_row_slice(rows1, cols2, &data))
}

fn main() {
    let m1 = DMatrix::from_row_slice(2, 3, &[
        1.0, 2.0, 3.0,
        4.0, 5.0, 6.0
    ]);
    let m2 = DMatrix::from_row_slice(3, 2, &[
        7.0, 8.0,
        9.0, 10.0,
        11.0, 12.0
    ]);

    let start = Instant::now();
    match multiply_matrices(&m1, &m2) {
        Some(result) => {
            let duration = start.elapsed();
            println!("Time for ex5 first multiplication: {:?}", duration);
            println!("Result of matrix multiplication:\n{}", result);
        }
        None => {
            println!("Error: Matrix dimensions are not compatible for multiplication.");
        }
    }

    let m3 = DMatrix::from_row_slice(2, 2, &[
        1.0, 2.0,
        3.0, 4.0
    ]);
    let m4 = DMatrix::from_row_slice(3, 2, &[
        5.0, 6.0,
        7.0, 8.0,
        9.0, 10.0
    ]);

    let start2 = Instant::now();
    match multiply_matrices(&m3, &m4) {
        Some(result) => {
            let duration2 = start2.elapsed();
            println!("Time for ex5 second multiplication: {:?}", duration2);
            println!("Result of matrix multiplication:\n{}", result);
        }
        None => {
            let duration2 = start2.elapsed();
            println!("Time for ex5 second multiplication (error): {:?}", duration2);
            println!("Error: Matrix dimensions are not compatible for multiplication.");
        }
    }
}
