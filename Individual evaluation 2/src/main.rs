#![allow(unused)]
use std::collections::HashMap;
mod company;
mod quiz;

fn main() {
    let mut departments = HashMap::new();
    departments.insert(
        "Engineering".to_string(),
        vec![
            company::Employee { name: "Alice Johnson".to_string(), salary: 120000.0 },
            company::Employee { name: "Bob Smith".to_string(), salary: 110000.0 },
            company::Employee { name: "Charlie Brown".to_string(), salary: 130000.0 },
            company::Employee { name: "Diana Prince".to_string(), salary: 125000.0 },
        ],
    );
    departments.insert(
        "Sales".to_string(),
        vec![
            company::Employee { name: "Eve Adams".to_string(), salary: 90000.0 },
            company::Employee { name: "Frank Miller".to_string(), salary: 95000.0 },
            company::Employee { name: "Grace Lee".to_string(), salary: 88000.0 },
        ],
    );
    departments.insert(
        "Marketing".to_string(),
        vec![
            company::Employee { name: "Henry Ford".to_string(), salary: 85000.0 },
            company::Employee { name: "Ivy Chen".to_string(), salary: 92000.0 },
        ],
    );
    departments.insert(
        "HR".to_string(),
        vec![
            company::Employee { name: "Jack Ryan".to_string(), salary: 78000.0 },
            company::Employee { name: "Kate Bishop".to_string(), salary: 80000.0 },
            company::Employee { name: "Liam Neeson".to_string(), salary: 82000.0 },
            company::Employee { name: "Mia Khalifa".to_string(), salary: 75000.0 },
            company::Employee { name: "Noah Centineo".to_string(), salary: 79000.0 },
        ],
    );
    departments.insert(
        "Finance".to_string(),
        vec![
            company::Employee { name: "Olivia Wilde".to_string(), salary: 105000.0 },
            company::Employee { name: "Peter Parker".to_string(), salary: 100000.0 },
        ],
    );
    let company = company::Company {
        name: "APROP Inc.".to_string(),
        departments,
    };
    
    let dept = quiz::read_department();
    let cost = quiz::department_cost(&company, &dept);
    println!("[{}] Department cost: {:.0} €", dept, cost);
    
    let total = quiz::total_cost(&company);
    println!("Total: {:.0} €", total);

    if let Some((name, cost)) = quiz::highest_cost_department(&company) {
        println!("Highest: [{}] - {:.0} €", name, cost);
    }
}
