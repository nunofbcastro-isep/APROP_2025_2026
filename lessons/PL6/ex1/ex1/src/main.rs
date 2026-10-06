use rand::Rng;
use std::thread;

fn generate_random_array(size: usize, min_value: i32, max_value: i32) -> Vec<i32> {
    let mut rng = rand::thread_rng();
    (0..size)
        .map(|_| rng.gen_range(min_value..=max_value))
        .collect()
}

fn max(v: &[i32]) -> i32 {
    let len = v.len();
    if len < 1000 {
        let mut max = v[0];
        for &i in v {
            if i > max {
                max = i;
            }
        }
        return max;
    }

    let num_threads = 4;
    let chunk_size = (len + num_threads - 1) / num_threads;

    thread::scope(|s| {
        let handles: Vec<_> = v.chunks(chunk_size).map(|chunk| {
            s.spawn(move || {
                let mut max = chunk[0];
                for &i in chunk {
                    if i > max {
                        max = i;
                    }
                }
                max
            })
        }).collect();

        handles.into_iter().map(|h| h.join().unwrap()).max().unwrap()
    })
}

fn min(v: &[i32]) -> i32 {
    let len = v.len();
    if len < 1000 {
        let mut min = v[0];
        for &i in v {
            if i < min {
                min = i;
            }
        }
        return min;
    }

    let num_threads = 4;
    let chunk_size = (len + num_threads - 1) / num_threads;

    thread::scope(|s| {
        let handles: Vec<_> = v.chunks(chunk_size).map(|chunk| {
            s.spawn(move || {
                let mut min = chunk[0];
                for &i in chunk {
                    if i < min {
                        min = i;
                    }
                }
                min
            })
        }).collect();

        handles.into_iter().map(|h| h.join().unwrap()).min().unwrap()
    })
}

fn sum(v: &[i32]) -> i32 {
    let len = v.len();
    if len < 1000 {
        let mut sum = 0;
        for &i in v {
            sum += i;
        }
        return sum;
    }

    let num_threads = 4;
    let chunk_size = (len + num_threads - 1) / num_threads;

    thread::scope(|s| {
        let handles: Vec<_> = v.chunks(chunk_size).map(|chunk| {
            s.spawn(move || {
                let mut sum = 0;
                for &i in chunk {
                    sum += i;
                }
                sum
            })
        }).collect();

        handles.into_iter().map(|h| h.join().unwrap()).sum()
    })
}

fn average(v: &[i32]) -> f32 {
    let sum = sum(v);
    sum as f32 / v.len() as f32
}

fn median(numbers: &[i32]) -> Option<f32> {
    if numbers.is_empty() {
        return None;
    }

    // If the slice is not sorted, work on a sorted copy so we don't mutate the caller
    let mut nums = numbers.to_vec();
    if !numbers.windows(2).all(|w| w[0] <= w[1]) {
        nums = quick_sort(nums);
    }

    let n = nums.len();
    let mid = n / 2;

    if n % 2 == 0 {
        Some((nums[mid - 1] + nums[mid]) as f32 / 2.0)
    } else {
        Some(nums[mid] as f32)
    }
}

fn add_vectors(v1: &[i32], v2: &[i32]) -> Vec<i32> {
    let len = std::cmp::min(v1.len(), v2.len());
    if len < 1000 {
        return (0..len).map(|i| v1[i] + v2[i]).collect();
    }

    let num_threads = 4;
    let chunk_size = (len + num_threads - 1) / num_threads;
    let chunks: Vec<_> = (0..num_threads).map(|i| {
        let start = i * chunk_size;
        let end = std::cmp::min(start + chunk_size, len);
        (start, end)
    }).filter(|(s, e)| s < e).collect();

    thread::scope(|s| {
        let handles: Vec<_> = chunks.into_iter().map(|(start, end)| {
            s.spawn(move || {
                (start..end).map(|i| v1[i] + v2[i]).collect::<Vec<i32>>()
            })
        }).collect();

        let mut result = Vec::with_capacity(len);
        for h in handles {
            result.extend(h.join().unwrap());
        }
        result
    })
}

fn quick_sort(mut v: Vec<i32>) -> Vec<i32> {
    let len = v.len();
    if len <= 1 {
        return v;
    }

    // Choose middle element as pivot and remove it from v
    let pivot_index = len / 2;
    let pivot = v.remove(pivot_index);

    let mut less = Vec::new();
    let mut equal = vec![pivot];
    let mut greater = Vec::new();

    for x in v.into_iter() {
        if x < equal[0] {
            less.push(x);
        } else if x == equal[0] {
            equal.push(x);
        } else {
            greater.push(x);
        }
    }

    if len > 1000 {
        let (mut sorted_less, sorted_greater) = thread::scope(|s| {
            let less_handle = s.spawn(|| quick_sort(less));
            let greater_sorted = quick_sort(greater);
            (less_handle.join().unwrap(), greater_sorted)
        });

        sorted_less.extend(equal);
        sorted_less.extend(sorted_greater);
        sorted_less
    } else {
        let mut sorted = quick_sort(less);
        sorted.extend(equal);
        sorted.extend(quick_sort(greater));
        sorted
    }
}

fn bubble_sort(mut v: Vec<i32>) -> Vec<i32> {
    let n = v.len();
    if n <= 1 {
        return v;
    }

    for i in 0..n {
        let mut swapped = false;
        for j in 0..(n - 1 - i) {
            if v[j] > v[j + 1] {
                v.swap(j, j + 1);
                swapped = true;
            }
        }
        if !swapped {
            break;
        }
    }
    v
}

fn median_un_sorted(numbers: &[i32]) -> Option<f32> {
    if numbers.is_empty() {
        return None;
    }

    // sort a copy of the slice
    let temp_numbers = quick_sort(numbers.to_vec());

    let n = temp_numbers.len();
    let mid = n / 2;

    if n % 2 == 0 {
        Some((temp_numbers[mid - 1] + temp_numbers[mid]) as f32 / 2.0)
    } else {
        Some(temp_numbers[mid] as f32)
    }
}

fn main() {
    let v  = generate_random_array(10, 1, 50);
    let v_s = (1..100).collect::<Vec<i32>>();

    //a. Implement functions that return the max, min, average
    let max_value = max(&v);
    println!("The maximum value is: {}", max_value);

    let min_value = min(&v);
    println!("The minimum value is: {}", min_value);

    let average_value = average(&v);
    println!("The average value is: {}", average_value);

    // b. Implement a function that returns the median (when sorted, the value in the middle position), it should fail if the vector is not sorted
    match median(&v) {
        Some(median_value) => println!("The median value for unsorted v is: {}", median_value),
        None => println!("Error computing median for v"),
    }

    match median(&v_s) {
        Some(median_value) => println!("The median value for sorted v_s is: {}", median_value),
        None => println!("Error computing median for v_s"),
    }

    // c. Implement a function to create a vector by adding the elements in two other vectors v3[i] = v1[i] + v2[i]
    let a = generate_random_array(10, 1, 20);
    let b = generate_random_array(8, 1, 20);
    println!("a = {:?}", a);
    println!("b = {:?}", b);
    let c = add_vectors(&a, &b);
    println!("a + b (element-wise, truncated to min length) = {:?}", c);

    let unsorted = generate_random_array(10, 1, 50);
    println!("unsorted = {:?}", unsorted);

    // d. Implement a function that sorts the array using quick sort
    let qs = quick_sort(unsorted.clone());
    println!("quick_sorted = {:?}", qs);

    // e. Implement a function that sorts the array using bubble sort
    let bs = bubble_sort(unsorted.clone());
    println!("bubble_sorted = {:?}", bs);

    // f. Change the median function to calculate the median by sorting a copy of the array, if it is not sorted
    match median_un_sorted(&unsorted) {
        Some(m) => println!("Median of unsorted (computed from sorted copy) = {}", m),
        None => println!("Could not compute median for unsorted"),
    }

    match median_un_sorted(&qs) {
        Some(m) => println!("Median of quick_sorted = {}", m),
        None => println!("Could not compute median for quick_sorted"),
    }
}