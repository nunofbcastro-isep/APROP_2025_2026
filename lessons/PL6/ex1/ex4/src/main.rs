use rand::Rng;
use std::collections::HashMap;
use std::thread;

fn generate_random_array(size: usize, min_value: i32, max_value: i32) -> Vec<i32> {
    let mut rng = rand::thread_rng();
    (0..size)
        .map(|_| rng.gen_range(min_value..=max_value))
        .collect()
}

fn mode(v: &[i32]) -> Option<i32> {
    let len = v.len();
    if len < 1000 {
        let mut map = HashMap::new();
        for &val in v {
            *map.entry(val).or_insert(0) += 1;
        }
        return map.into_iter().max_by_key(|&(_, count)| count).map(|(val, _)| val);
    }

    let num_threads = 4;
    let chunk_size = (len + num_threads - 1) / num_threads;

    let maps = thread::scope(|s| {
        let handles: Vec<_> = v.chunks(chunk_size).map(|chunk| {
            s.spawn(move || {
                let mut map = HashMap::new();
                for &val in chunk {
                    *map.entry(val).or_insert(0) += 1;
                }
                map
            })
        }).collect();

        handles.into_iter().map(|h| h.join().unwrap()).collect::<Vec<_>>()
    });

    let mut final_map = HashMap::new();
    for map in maps {
        for (val, count) in map {
            *final_map.entry(val).or_insert(0) += count;
        }
    }

    final_map.into_iter()
        .max_by_key(|&(_, count)| count)
        .map(|(val, _)| val)
}

fn main() {
    let v  = generate_random_array(10, 1, 50);
    println!("The mode of the array is: {:?}", mode(&v));
}