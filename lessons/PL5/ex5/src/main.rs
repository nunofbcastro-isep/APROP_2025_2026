use nalgebra::{DMatrix};
use std::time::Instant;

fn multiply_matrices(matrix1: &DMatrix<f64>, matrix2: &DMatrix<f64>) -> Option<DMatrix<f64>> {
    if matrix1.ncols() != matrix2.nrows() {
        return None;
    }
    Some(matrix1 * matrix2)
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
