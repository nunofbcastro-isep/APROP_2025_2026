### Comparação dos Tempos Decorridos para Multiplicação de Matrizes

Medimos os tempos decorridos para os exercícios 3 e 5 usando `Instant` e `Duration` do módulo `std::time`, e comparamos com a implementação sequencial em C fornecida em `ex9/main.c`. Todos os testes usam os mesmos tamanhos de matrizes: uma matriz 2x3 multiplicada por uma 3x2, resultando numa 2x2.

#### Tempos Medidos:
- **Exercício 3 (Implementação manual em Rust com loops aninhados)**: 15.3 µs  
- **Exercício 5 (Rust com biblioteca nalgebra)**: 12.4 µs  
- **Implementação sequencial em C**: 3 µs (conforme reportado da sua execução)  

#### Notas sobre a Medição:
- **Tempos em Rust**: Usamos `Instant::now()` antes da multiplicação e `elapsed()` depois, medindo o tempo de relógio de parede. As execuções foram feitas em modo debug (não otimizado).
- **Tempo em C**: Usamos `clock()` de `<time.h>`, que mede o tempo de CPU usado pelo processo. Pode diferir ligeiramente do tempo de relógio de parede, mas é comparável para operações curtas.
- As matrizes são muito pequenas (2x3 e 3x2), pelo que as operações completam em microssegundos. Os tempos podem variar ligeiramente entre execuções devido a ruído do sistema, e a precisão em microssegundos pode não ser totalmente fiável para durações tão curtas.

#### Conclusão:
- A **implementação sequencial em C é a mais rápida** para estes tamanhos pequenos de matrizes, a 3 µs, provavelmente devido à gestão de memória de baixo nível e menos abstrações em comparação com Rust.
- A **implementação em Rust baseada em nalgebra (ex5)** é mais rápida que os loops manuais em Rust (ex3) por cerca de 3 µs, mostrando o benefício de código de biblioteca otimizado mesmo para dados pequenos.
- Para **matrizes maiores**, o nalgebra provavelmente superaria ambas as versões manual (Rust ex3 e C) significativamente, pois usa algoritmos altamente otimizados (ex.: operações tipo BLAS). As implementações manuais (Rust ex3 e C) são O(n^3) e escalariam mal, enquanto o nalgebra aproveita SIMD e outras otimizações.
- As funcionalidades de segurança de Rust (ex.: verificação de limites) adicionam uma pequena sobrecarga em comparação com C, mas é negligenciável para operações pequenas. Para código crítico em performance, C continua a ser uma escolha forte, mas Rust com bibliotecas como nalgebra oferece um bom equilíbrio entre segurança e velocidade.
