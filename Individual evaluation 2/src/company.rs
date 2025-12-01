use std::collections::HashMap;

pub struct Employee {
    pub name: String,
    pub salary: f64,  // Annual salary
}

pub struct Company {
    pub name: String,
    pub departments: HashMap<String, Vec<Employee>>,
}
