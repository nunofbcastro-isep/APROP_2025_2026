# Company Cost Calculator

This functional Rust exercise involves computing costs for a company with departments and employees. Focus on immutable data processing using iterators, closures, and functional patterns.

## Project Structure
- `src/main.rs`: Entry point with sample data and function calls.
- `src/cost.rs`: Implement the cost calculation functions here.
- `src/company.rs`: Contains the data structures (do not modify).

## Goal
Implement the functions in `src/quiz.rs` to calculate various costs functionally and add expected output on `src/main.rs`. Remove all `todo!()` macros, ensure no warnings or errors, and use iterators instead of loops.

## Data Structures
From `src/company.rs`:
```rust
pub struct Employee {
    pub name: String,
    pub salary: f64,  // Annual salary
}

pub struct Company {
    pub name: String,
    pub departments: HashMap<String, Vec<Employee>>,
}
```

## Functions to Implement in `src/quiz.rs`

### `read_department` [2 points]
```rust
pub fn read_department() -> String
```
Asks for input from the user and returns the retrieved value.

### `department_cost` [5 points]
```rust
pub fn department_cost(company: &Company, dept: &str) -> f64
```
Returns the total annual salary cost for the specified department. If department not found, stop app execution.

### `total_cost` [5 points]
```rust
pub fn total_cost(company: &Company) -> f64
```
Returns the total annual salary cost for the entire company.

### `highest_cost_department` [7 points]
```rust
pub fn highest_cost_department(company: &Company) -> Option<(String,f64)>
```
Returns the name of the department with the highest total cost, or `None` if no departments.

## Main Flow
1. `main.rs` generates sample company data.
2. Calls `read_department()` for user input.
3. Computes and prints department cost, total cost and thehighest department with its cost.

## Compilation + Expected Output [1 point]
```
Enter department name:
Sales
[Sales] Department cost: 273000 €
Total: 1534000 €
Highest: [Engineering] - 485000 €
```

## Hint

You can use the macro `panic!(msg)` to halt a application execution.
You can use the `std::io` module to deal with user input.
You can use the `println!(msg)` macro to print a message to the console.

## Setup
1. Implement functions in `src/quiz.rs`.
2. Return expected output in `src/main.rs`.
3. Run with `cargo run`.

Submit whole project with implementations.
