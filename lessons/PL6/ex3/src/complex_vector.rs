use crate::complex::Complex;
use rayon::prelude::*;

pub fn max_complex(vec: &[Complex]) -> Option<&Complex> {
    vec.into_par_iter()
    .max_by(|a, b| a.magnitude().partial_cmp(&b.magnitude())
    .unwrap_or(std::cmp::Ordering::Equal))
}

pub fn min_complex(vec: &[Complex]) -> Option<&Complex> {
    vec.into_par_iter().min_by(|a, b| a.magnitude().partial_cmp(&b.magnitude()).unwrap_or(std::cmp::Ordering::Equal))
}

pub fn average_complex(vec: &[Complex]) -> Option<Complex> {
    if vec.is_empty() {
        None
    } else {
        let sum = sum_complex(vec);
        let count = vec.len() as f64;
        Some(Complex::init(sum.real / count, sum.imag / count))
    }
}

pub fn sum_complex(vec: &[Complex]) -> Complex {
    vec.into_par_iter()
        .fold(|| Complex::init(0.0, 0.0), |acc, c| acc.add(c))
        .reduce(|| Complex::init(0.0, 0.0), |a, b| a.add(&b))
}

pub fn modules(vec: &[Complex]) -> Vec<f64> {
    vec.into_par_iter().map(|c| c.magnitude()).collect()
}
