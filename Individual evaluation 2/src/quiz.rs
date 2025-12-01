#![allow(unused)]
use super::company::{Company, Employee};
use std::collections::HashMap;
use std::io::{self, Write};

/// Reads a department name from the user and returns it as a String.
pub fn read_department() -> String {
    println!("Enter department name:");
    let mut input = String::new();
    io::stdin().read_line(&mut input);
    input.trim().to_string()
}

/// Returns the total salary cost of the given department.
/// Stops execution if the department does not exist.
pub fn department_cost(company: &Company, dept: &str) -> f64 {
    match company.departments.get(dept) {
        Some(employees) => employees.iter().map(|e| e.salary).sum(),
        None => panic!("Department not found"),
    }
}

/// Returns the total salary cost of the entire company.
pub fn total_cost(company: &Company) -> f64 {
    company.departments
        .values()
        .map(|employees| employees.iter().map(|e| e.salary).sum::<f64>())
        .sum()
}

/// Returns the department with the highest total cost and its total as a tuple (name, cost).
/// Returns None if there are no departments.
pub fn highest_cost_department(company: &Company) -> Option<(String, f64)> {
    company.departments.iter()
        .map(|(name, employees)| {
            let cost: f64 = employees.iter().map(|e| e.salary).sum();
            (name.clone(), cost)
        })
        .max_by(|d1, d2| d1.1.partial_cmp(&d2.1).unwrap())
}