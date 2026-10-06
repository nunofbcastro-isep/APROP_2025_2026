use rand::Rng;
use std::collections::HashMap;

fn generate_random_array(size: usize, min_value: i32, max_value: i32) -> Vec<i32> {
    let mut rng = rand::thread_rng();
    (0..size)
        .map(|_| rng.gen_range(min_value..=max_value))
        .collect()
}

fn mode(v: &[i32]) -> Option<i32> {
    let mut map = HashMap::new();

    for &val in v {
        *map.entry(val).or_insert(0) += 1;
    }

    map.into_iter()
        .max_by_key(|&(_, count)| count)
        .map(|(val, _)| val)
}

fn main() {
    let v  = generate_random_array(10, 1, 50);
    println!("The mode of the array is: {:?}", mode(&v));
}