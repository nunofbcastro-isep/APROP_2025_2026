fn sum_1_to_100() -> i32 {
    (1..=100).sum()
}

fn sum_squares_below(upper_bound: i32) -> i32 {
    let square = |n: i32| n * n;

    (1..)
        .map(|n| square(n))
        .take_while(|&sq| sq < upper_bound)
        .sum()
}

fn main() {
    let total = sum_1_to_100();
    println!("Sum of 1 to 100 = {}", total);

    let bound = 1000;
    let squares_sum = sum_squares_below(bound);
    println!("Sum of squares less than {} = {}", bound, squares_sum);
}
