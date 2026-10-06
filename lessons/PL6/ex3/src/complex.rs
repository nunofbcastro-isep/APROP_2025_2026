#[derive(Clone, Copy, Debug)]
pub struct Complex {
    pub real: f64,
    pub imag: f64,
}

impl Complex {
    pub fn init(real: f64, imag: f64) -> Complex {
        Complex { real, imag }
    }

    pub fn add(&self, other: &Complex) -> Complex {
        Complex {
            real: self.real + other.real,
            imag: self.imag + other.imag,
        }
    }

    pub fn subtract(&self, other: &Complex) -> Complex {
        Complex {
            real: self.real - other.real,
            imag: self.imag - other.imag,
        }
    }

    pub fn multiply(&self, other: &Complex) -> Complex {
        Complex {
            real: self.real * other.real - self.imag * other.imag,
            imag: self.real * other.imag + self.imag * other.real,
        }
    }

    pub fn divide(&self, other: &Complex) -> Complex {
        let denominator = other.real * other.real + other.imag * other.imag;
        Complex {
            real: (self.real * other.real + self.imag * other.imag) / denominator,
            imag: (self.imag * other.real - self.real * other.imag) / denominator,
        }
    }

    pub fn magnitude(&self) -> f64 {
        (self.real * self.real + self.imag * self.imag).sqrt()
    }
}
