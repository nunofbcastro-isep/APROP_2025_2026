
use std::time::Instant;
use std::thread;

fn multiply(array1: &Vec<Vec<i32>>, array2: &Vec<Vec<i32>>) -> Vec<Vec<i32>> {
    let rows1 = array1.len();
    let cols1 = array1[0].len();
    let rows2 = array2.len();
    let cols2 = array2[0].len();

    if cols1 != rows2 {
        panic!("Incompatible matrix dimensions for multiplication");
    }

    if rows1 < 100 {
        let mut result = vec![vec![0; cols2]; rows1];
        for i in 0..rows1 {
            for j in 0..cols2 {
                for k in 0..cols1 {
                    result[i][j] += array1[i][k] * array2[k][j];
                }
            }
        }
        return result;
    }

    let num_threads = 4;
    let chunk_size = (rows1 + num_threads - 1) / num_threads;

    thread::scope(|s| {
        let handles: Vec<_> = array1.chunks(chunk_size).map(|chunk| {
            s.spawn(move || {
                let mut chunk_result = Vec::with_capacity(chunk.len());
                for row in chunk {
                    let mut result_row = vec![0; cols2];
                    for j in 0..cols2 {
                        for k in 0..cols1 {
                            result_row[j] += row[k] * array2[k][j];
                        }
                    }
                    chunk_result.push(result_row);
                }
                chunk_result
            })
        }).collect();

        let mut result = Vec::with_capacity(rows1);
        for h in handles {
            result.extend(h.join().unwrap());
        }
        result
    })
}

fn main() {
    let array1 = vec![
        vec![1, 2, 3],
        vec![4, 5, 6],
    ];
    let array2 = vec![
        vec![7, 8],
        vec![9, 10],
        vec![11, 12],
    ];

    let start = Instant::now();
    let result = multiply(&array1, &array2);
    let duration = start.elapsed();
    println!("Time for ex3: {:?}", duration);

    for row in result {
        println!("{:?}", row);
    }
}
